
#include <Arduino.h>
#include <PUMPENSTEUERUNG.h>
#include <GSM_Shield.h>
#include <DFROBOT_LCD_SHIELD.h>
#include <main.h>
#include <stop.h>
#include <EEPROM.h>
#include "IO.h"

// !!! Dirty Hack !!!!!!!!!
//#define lcd my_dfRobotLcdShield
//#define LCD my_dfRobotLcdShield.LCD

PUMPENSTEUERUNG::PUMPENSTEUERUNG(uint8_t PIN_Power_OUT, uint8_t PIN_Schwimmer_IN)
{
PumpePowerPin=PIN_Power_OUT;
schwimmerPin= PIN_Schwimmer_IN;
}

/*************************
Setup Pumpensteuerung
**************************/
void PUMPENSTEUERUNG::setup()
{
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(PumpePowerPin, OUTPUT);
  pinMode(schwimmerPin, INPUT_PULLUP);
  Time_Stamp_Wasser_leer=millis();  
  Time_Stamp_Wasser_voll=millis();
  Time_Stamp_last_test_SMS=millis(); //Timer für Test SMS
  Time_Stamp_next_houer=millis()+Time_one_houer; //1Std.
  Time_Stamp_last_houer=millis();
  Time_Stamp_next_SMS = millis()+Time_SMS_interval;
  EEPROM.get(EEPROM_ADR_PUMPE_EINSCHALTDAUER,Time_Pumpe_On);
  EEPROM.get(EEPROM_ADR_PUMPE_AUSSCHALTDAUER,Time_Pumpe_Off);
}

/*************************
Pumpe Intervall 
Pumpe in gewissen Zeitabständen ein und aus schalten
**************************/
void PUMPENSTEUERUNG::Pumpe_Intervall()
{
        /* Intervall für Pumpe */
      if(Status_Pumpe_On){   //Pumpe nach gewisser Laufzeit auschalten
        if(millis()-Time_Stamp_Pumpe_On > Time_Pumpe_On){ 
          Status_Pumpe_On=false;
          Status_Pumpe_Off=true;
          Time_Stamp_Pumpe_Off=millis();
        }
      }
      if(Status_Pumpe_Off){   //Pumpe nach gewisser Ruhezeit wieder einschalten
        if(millis()-Time_Stamp_Pumpe_Off > Time_Pumpe_Off){ 
          Status_Pumpe_On=true;
          Status_Pumpe_Off=false;
          Time_Stamp_Pumpe_On=millis();
        }
      }
}

/***********************
Loop Pumpensteuerung
************************/
char* PUMPENSTEUERUNG::loop()
{
 
  /* Pumpe Ein- und Aus- Schalten */
  //Pumpe_Intervall(); // Nur für Schwimmerunabhängiges EIN-AUS (zum Ansaugen)

  //Return Message
  char *Message = NULL;

  /* Eingang des Schwimmer Schalters lesen und entprellen */
  schwimmerReading = digitalRead(schwimmerPin);

  if (schwimmerReading != schwimemrLastReading) {
    last_debounce_time_schwimmer = millis();
  }
  if ((millis()-last_debounce_time_schwimmer) > debounce_time){
    schwimmerState = schwimmerReading;

    // Serial Debuging
    static int schwimmerLastState=99;
    if(schwimmerState != schwimmerLastState){
      schwimmerLastState=schwimmerState;
#if SERIAL_ENABLE && DEBUGG_PUMPENSTEUERUNG
        if(1){
        print_to(hasprint.ser,F("\r\nNew SchwimmerState: "))  ;
        Serial.println(schwimmerState );
      }
#endif
    }
  }

  schwimemrLastReading = schwimmerReading; /* Ende Taster entprellen */
  

  /* Wanne voll */
  if(Status_Wasser_voll)
  {
    /* Schwimmer Wechsel abwärts */
    if(schwimmerState == LOW) //zuwenig Wasser
    {
      Status_Wasser_leer=true;  //Pumpe einschalten
      Status_Wasser_voll=false;
      Status_Pumpe_On=true;
      Status_Pumpe_Off=false;
      Time_Stamp_Pumpe_On=millis();
      Time_Stamp_Wasser_leer=millis();
      Counter_Schimmer_Down++;
        Message = (char*)(F("LOW"));
#if SERIAL_ENABLE && DEBUGG_PUMPENSTEUERUNG
        print_to(hasprint.ser,F("\r\nSchwimmer LOW"));
#endif

        Message = (char*)(F("LOW"));
    }
    
    /* Total Zeit Schwimmer unten aktualisieren */
    Total_Time_Waser_leer_array [houer] = Time_Total_Wasser_leer; //Total Zeit Schwimmer unten
    Time_schwimmer_down = 0;
  }
  

  /* Wanne Leer */   
  if (Status_Wasser_leer)
  {
     /* Schwimmer Wechsel aufwärts */
    if( schwimmerState == HIGH)     //genug Wasser Pumpe aus
    {
      Status_Wasser_leer=false;
      Status_Wasser_voll=true;
      Status_Pumpe_On=false;
      Status_Pumpe_Off=true;
      Status_send_SMS_kein_Wasser = false;
      Time_Stamp_Wasser_voll = millis();
      Message = (char*)(F("HIGH"));
#if SERIAL_ENABLE
#if DEBUGG_PUMPENSTEUERUNG
      print_to(hasprint.ser,F("\r\nSchwimmer HIGH"));
#endif
#endif

      /* Total Zeit Schwimmer unten aktualisieren */
      Time_Total_Wasser_leer += (millis() - Time_Stamp_Wasser_leer)/1000; //Total Zeit in Sec. Schwimmer unten
    }

    /* Schwimmer unten */
    if(schwimmerState == LOW)       //zuwenig Wasser Pumpe läuft
    {
       /* Zeit ermitteln seit Schwimmer unten ist */
       Time_schwimmer_down = millis() - Time_Stamp_Wasser_leer;  //Aktuel wie lange kein Wasser
      
       Total_Time_Waser_leer_array [houer] = Time_Total_Wasser_leer + Time_schwimmer_down/1000; //Total

      /* Pumpe Ein- und Aus- Schalten */
      Pumpe_Intervall();

      /* SMS senden */
      if ( (Time_schwimmer_down > Time_Leer_to_SMS) ) {
        if(Status_send_SMS_kein_Wasser == false){
            if(GSM_ENABLE){
                Status_send_SMS_kein_Wasser = true;
                send_status_sms();}

            Time_Stamp_next_SMS = millis()+Time_SMS_interval;

        }
      }
      
    }/* End Schwimmer unten */
  }/* End Wasser leer */
  

  /* ...weiter in Loop Pumpensteuerung */
    
  /* Total Zeit Schwimmer unten letzte 24 Stunden */
  int dif = houer + 1;
  if (dif == 25) dif = 0;
  Time_Total_Wasser_leer_last_24_houer = Total_Time_Waser_leer_array [houer] - Total_Time_Waser_leer_array [dif];


  /* Stunden Zähler */
  if(millis() - Time_Stamp_last_houer > Time_one_houer)
  {
    Time_Stamp_next_houer += Time_one_houer;
    Time_Stamp_last_houer = millis();
    houer++; if(houer==25) houer=0;

    //Debugg
#if SERIAL_ENABLE && DEBUGG_PUMPENSTEUERUNG
      int a;
    if(1) {
      for(a=0;a<25;a++){
        if(houer==a)Serial.print(">");else Serial.print(" ");
        Serial.print(a);Serial.print(": ");
        Serial.println(Total_Time_Waser_leer_array[a]);
      }
      Serial.print("\nTotal: ");
      Serial.println(Time_Total_Wasser_leer_last_24_houer);
    }
#endif
  }

  /* Ausgänge Ansteuern */
  if(Status_Pumpe_On)
  {
#if DEBUGG_PUMPENSTEUERUNG
    digitalWrite(LED_BUILTIN, HIGH);        // LED ein
#endif
    digitalWrite(PumpePowerPin, HIGH);   // Pumpe ein
  }
  if(Status_Pumpe_Off)
  {
#if DEBUGG_PUMPENSTEUERUNG
    digitalWrite(LED_BUILTIN, LOW);         // LED aus
#endif
    digitalWrite(PumpePowerPin, LOW);    // Pumpe aus
  }

  /* Display aktualisieren */
#if DFROBOT_LCD_SHIELD_ENABLE
    display();
#endif


  return Message;

}/* end loop */

/***************
Send Status SMS
****************/
void PUMPENSTEUERUNG::send_status_sms(){
#if DFROBOT_LCD_SHIELD_ENABLE
    my_dfRobotLcdShield.LCD(F("Send Status SMS"));
#endif

    send_status(hasprint.ser);
    delay(2000);

  // send the message
  sms.beginSMS("0794118653");

  send_status(hasprint.smsService);

  sms.endSMS();

#if DFROBOT_LCD_SHIELD_ENABLE
    my_dfRobotLcdShield.LCD(F("Send COMPLETE!"));
#endif

}

template <class T>
void PUMPENSTEUERUNG::send_status(T &to){

    to.println(F("Traenke Status Bericht neu"));
    to.println(F("--------------------------"));

    if(Status_Wasser_leer){
        to.print(F("Schwimer ist Unten "));
        to.print(F("Seit: "));
        to.print( get_Hour_Min_Sec (Time_schwimmer_down) );
        to.println(F(" (Stunden: Min:Sec)"));
    }
    if(Status_Wasser_voll){
        to.print(F("Schwimer ist Oben "));
        to.print(F("Seit: "));
        to.print( get_Hour_Min_Sec( (millis()-Time_Stamp_Wasser_voll) ) );
        to.println(F(" (Stunden:Min:Sec)"));

    }
    to.print(F("Schwimmer Total Zeit Unten letzte 24 Std. : "));
    to.print( get_Hour_Min_Sec( Time_Total_Wasser_leer_last_24_houer * 1000L ));
    to.println(F(" (Stunden:Min:Sec)"));

    to.print(F("Schwimmer ging nach unten Zaehler: "));
    to.println(Counter_Schimmer_Down);

    to.print(F("Total Laufzeit Computer: "));
    to.print( get_Hour_Min_Sec( millis() ) );
    to.println(F(" (Stunden:Min:Sec)"));

    to.print(F("Volt am Stop-Draht: "));
    to.println(my_stop.voltage_Stop);

    to.print(F("A/D-Sensor Zahl: "));
    to.println(my_stop.anval_Stop);

    to.print(F("Baterie-Spannung: "));
    to.print(my_stop.voltage_Bat/10); to.print("."); to.println(my_stop.voltage_Bat % 10);
    to.print(F("A/D-Sensor Zahl (Bat): "));
    to.println(anval_Bat);

    to.print(F("Einschaltzeit-Pumpe(Sec.): "));
    to.println(Time_Pumpe_On/1000L);

    to.print(F("Ausschaltzeit-Pumpe(Sec.): "));
    to.println(Time_Pumpe_Off/1000L);
}

/*******
Display 
********/
#if DFROBOT_LCD_SHIELD_ENABLE
void PUMPENSTEUERUNG::display()
{

  /* Volt Bat. */
  if (screen==6){

      if (screen_setup) {
          //GEN 2
          my_dfRobotLcdShield.clear();
          my_dfRobotLcdShield.clear();
          my_dfRobotLcdShield.print(F("Bat.-Volt: "));
          my_dfRobotLcdShield.setCursor(0, 1);
          my_dfRobotLcdShield.print(F("Sensor: "));
          screen_setup = false;
      }
      my_dfRobotLcdShield.setCursor(10, 0);
      my_dfRobotLcdShield.print(voltage_Bat / 10);
      my_dfRobotLcdShield.print(".");
      my_dfRobotLcdShield.print(voltage_Bat % 10);       // volt
      my_dfRobotLcdShield.setCursor(9, 1);
      my_dfRobotLcdShield.print(anval_Bat);       // Analog Wandler Input
  }



  /* Volt Stop */
  if (screen==5){

      if (screen_setup) {
          my_dfRobotLcdShield.clear();
          my_dfRobotLcdShield.print(F("Stop-Volt: "));
          my_dfRobotLcdShield.setCursor(0, 1);
          my_dfRobotLcdShield.print(F("Sensor: "));
          screen_setup = false;
      }
      my_dfRobotLcdShield.setCursor(10, 0);
      my_dfRobotLcdShield.print(voltage_Stop);       // volt
      my_dfRobotLcdShield.setCursor(9, 1);
      my_dfRobotLcdShield.print(anval_Stop);       // Analog Wandler Input
  }

    /* Runtime */
    if (screen == 4) {

        if (screen_setup) {
            my_dfRobotLcdShield.clear();
            my_dfRobotLcdShield.print(F("Runtime:"));
            my_dfRobotLcdShield.setCursor(0, 1);             // move cursor to second line "1" and 9 spaces over
            my_dfRobotLcdShield.print(F("H:M:S"));
            screen_setup = false;
        }
        my_dfRobotLcdShield.setCursor(8, 0);             // move cursor to second line "1" and 9 spaces over
        my_dfRobotLcdShield.print(get_Hour_Min_Sec(millis()));
        /* Loop-Time */
        my_dfRobotLcdShield.setCursor(12, 1);
        my_dfRobotLcdShield.print("    ");
        my_dfRobotLcdShield.setCursor(12, 1);
        my_dfRobotLcdShield.print(looptime);
    }


    /* Anzahl Schwimmer unten   */
    if (screen == 2) {

        if (screen_setup) {
            my_dfRobotLcdShield.clear();                   //DFRobot_LCD_Output Ausgabe
            my_dfRobotLcdShield.print(F("Schwimmer ging"));
            my_dfRobotLcdShield.setCursor(0, 1);
            my_dfRobotLcdShield.print(F("    x nach unten"));

            screen_setup = false;
        }
        my_dfRobotLcdShield.setCursor(0, 1);                   //DFRobot_LCD_Output Ausgabe
        my_dfRobotLcdShield.print(Counter_Schimmer_Down);  // Sec. Schwimmer Unten
    }

    /* Schwimmer letzter Tag */
    if (screen == 3) {

        if (screen_setup) {
            my_dfRobotLcdShield.clear();                   //DFRobot_LCD_Output Ausgabe
            my_dfRobotLcdShield.print(F("Schwimmer unten"));
            my_dfRobotLcdShield.setCursor(0, 1);
            my_dfRobotLcdShield.print(F("         m:s/Tag"));

            screen_setup = false;
        }
        my_dfRobotLcdShield.setCursor(0, 1);                   //DFRobot_LCD_Output Ausgabe
        my_dfRobotLcdShield.print(get_Min_Sec(Time_Total_Wasser_leer_last_24_houer * 1000L));  // Sec. Schwimmer Unten
    }


    /* Schwimmer Aktuell */
    if (screen == 1) {

        if (screen_setup) {
            my_dfRobotLcdShield.clear();                   //DFRobot_LCD_Output Ausgabe
            if (Status_Wasser_leer)
                my_dfRobotLcdShield.print(F("Schwimmer unten"));
            if (Status_Wasser_voll)
                my_dfRobotLcdShield.print("Schwimmer oben");
            my_dfRobotLcdShield.setCursor(0, 1);
            my_dfRobotLcdShield.print(F("           h:m:s"));

            screen_setup = false;
        }

        if (Status_Wasser_leer) {
            my_dfRobotLcdShield.setCursor(0, 1);                   //DFRobot_LCD_Output Ausgabe
            my_dfRobotLcdShield.print(get_Hour_Min_Sec(Time_schwimmer_down));  // Sec. Schwimmer Unten
        }
        if (Status_Wasser_voll) {
            my_dfRobotLcdShield.setCursor(0, 1);
            my_dfRobotLcdShield.print(get_Hour_Min_Sec((millis() - Time_Stamp_Wasser_voll)));

        }
    }

}/* end display */
#endif

/* String MIN:SEC aus Zeit machen */
String PUMPENSTEUERUNG::get_Min_Sec(long ms){
    long m=ms/60000L; //minuten
    long sec= (ms-m*60000L)/1000L;
    String sec_string= String(sec,DEC);
    if (sec_string.length()<2)
        sec_string = "0"+sec_string;
    String m_string= String(m,DEC);
    if (m_string.length()<2)
        m_string = "0"+m_string;

    return m_string  +":"+ sec_string;
}

/* String Stunden:Minuten:Sec aus Milliseckunden machen */
String PUMPENSTEUERUNG::get_Hour_Min_Sec(long ms){
    long h=ms/3600000L; //Stunden
    return String(h,DEC) + ":" + get_Min_Sec( ms-h*3600000L); //Min:Sec
}


void PUMPENSTEUERUNG::set_Einschaltzeit_to_EEPROM(unsigned long ms) {
    EEPROM.put(EEPROM_ADR_PUMPE_EINSCHALTDAUER,ms);
}

void PUMPENSTEUERUNG::set_Ausschaltzeit_to_EEPROM(unsigned long ms) {
    EEPROM.put(EEPROM_ADR_PUMPE_AUSSCHALTDAUER,ms);
}

