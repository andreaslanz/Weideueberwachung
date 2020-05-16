/*
 Weide Tränke Pumpen Steuerung
 Pumpe vor Dauer Einschaltung Schützen
 SMS Senden wenn lange kein Wasser vorhanden
*/

#ifdef alt
#include <Arduino.h>
#include <main.h>
#include <GSM_Shield.h>
#include "IO.h"


/* Pin Definitionen */
const int schwimmerPin = A3;     //Nummer des Eingangs des Schwimmer Schalters Pins
const int ledPin =  LED_BUILTIN; // Nummer des LES Pins
const int PumpePowerPin =  A1;   // Nummer des Eingangs des Pumpen Power Pins

/* Debouncing */
int schwimmerState = HIGH;  //Variable für Schwimmer-Schalter Status
int schwimmerReading;       //Spanung am Schwimer Eingang Pin
int schwimemrLastReading;   //Letzter gelesener Wert am Schwimmer Eingang   
long last_debounce_time_schwimmer;  //Entprellung
const long debounce_time=5000L;  

/* Schrittkette Status Variablen */
int Status_Wasser_leer = true;
int Status_Wasser_voll = false;
int Status_Pumpe_On   = false;
int Status_Pumpe_Off =  true;
int Status_send_SMS_kein_Wasser = false;
int Status_send_SMS_Test = false;


/* Timer Variablen */
long       Time_Stamp_Pumpe_On= 0L;      //Zeit wenn Pumpe ein
long       Time_Stamp_Pumpe_Off= 0L;     //Zeit wenn Pumpe aus
const long Time_Pumpe_On= 30*1000L;      //Laufzeit der Pumpe 30 sec
const long Time_Pumpe_Off= 30L*60L*1000L;//Pause der Pumpe  30 min

long       Time_Stamp_Wasser_voll;        //Zeit seit der Schwimmer oben ist
long       Time_Stamp_Wasser_leer;        //Zeit seit der Schwimmer unten ist
long       Time_Total_Wasser_leer= 0L;    //Gesamtzeit Schimmer unten
long       Time_schwimmer_down=0L;        //Aktuelle Zeit in Sec. wo Schwimmer unten ist
long       Temp_Time_Schwimmer_down=0L;    //= Time_schwimmer_down/1000 + Time_Total_Wasser_leer
long       Total_Time_Waser_leer_array[25]; //Array mit Stündlichen total Zeiten Schwimmer unten
int        Time_Total_Wasser_leer_last_24_houer=0;  //Total Zeit Schwimmer unten in letzten 24 Stunden
int        Counter_Schimmer_Down=0;        //wie oft ging der Schwimmer nach unten

const long Time_Leer_to_SMS= 2*60L*60L*1000L;    //nach welcher Zeit seit Wanne leer wird SMS gesendet 2std.
long       Time_Stamp_last_test_SMS;             //wann wurde letzte Test-SMS gesendet;
long       Time_Stamp_next_SMS;                  // Nächste SMS
const long Time_SMS_interval= 86400000L; //24L*60L*60L*1000L; // Status SMS senden Interval 24 Stunden

long       Time_Stamp_next_houer;          //Stundenzähler
long       Time_Stamp_last_houer;          //Stundenzähler
int        houer=0;                       //Aktuelle Stunde
const long Time_one_houer = 60L*60L*1000L; //1 Std. in ms

/* extene Variablen */
extern int voltage_Stop;           //von Stop Modul
extern int anval_Stop;             //     dto.
extern int voltage_Bat;
extern int anval_Bat;
extern int screen;
extern bool screen_setup;
extern long looptime;
extern GSM_SMS sms;
extern long Time_schwimmer_down;        //Aktuelle Zeit in Sec. wo Schwimmer unten ist
extern LiquidCrystal lcd;     //DFRobot DFRobot_LCD_Output Shield


/* String MIN:SEC aus Zeit machen */
String get_Min_Sec(long ms){
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
String get_Hour_Min_Sec(long ms){
    long h=ms/3600000L; //Stunden
    return String(h,DEC) + ":" + get_Min_Sec( ms-h*3600000L); //Min:Sec
}



/*************************
Setup Pumpensteuerung
**************************/
void setup_pumpensteuerung()
{
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(ledPin, OUTPUT);
  pinMode(PumpePowerPin, OUTPUT);
  pinMode(schwimmerPin, INPUT_PULLUP);
  Time_Stamp_Wasser_leer=millis();  
  Time_Stamp_Wasser_voll=millis();
  Time_Stamp_last_test_SMS=millis(); //Timer für Test SMS
  Time_Stamp_next_houer=millis()+Time_one_houer; //1Std.
  Time_Stamp_last_houer=millis();
  Time_Stamp_next_SMS = millis()+Time_SMS_interval;
}

/*************************
Pumpe Intervall 
Pumpe in gewissen Zeitabständen ein und aus schalten
**************************/
void Pumpe_Intervall()
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
void loop_pumpensteuerung() 
{
 
  /* Pumpe Ein- und Aus- Schalten 
  Unabhängig von Schwimmer Stautus
  zum Ansaugen*/
  //Pumpe_Intervall();



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
      if(SERIAL_ENABLE)
        Serial.println(schwimmerState );
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
      LCD(F("LOW"));
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
      LCD(F("HIGH"));
      
      /* Total Zeit Schwimmer unten aktualisieren */
      Time_Total_Wasser_leer += (millis() - Time_Stamp_Wasser_leer)/1000; //Total Zeit in Sec. Schwimmer unten
    }

    /* Schwimmer untne */
    if(schwimmerState == LOW)       //zuwenig Wasser Pumpe läuft
    {
       /* Zeit ermitteln seit Schwimmer unten ist */
       Time_schwimmer_down = millis() - Time_Stamp_Wasser_leer;  //Aktuel wie lange kein Wasser
      
       Total_Time_Waser_leer_array [houer] = Time_Total_Wasser_leer + Time_schwimmer_down/1000; //Total

      /* Pumpe Ein- und Aus- Schalten */
      Pumpe_Intervall();
     
      /* SMS senden */
      if ( (Time_schwimmer_down > Time_Leer_to_SMS) && GSM_ENABLE) {
        if(Status_send_SMS_kein_Wasser == false){
          Status_send_SMS_kein_Wasser = true;
          send_status_sms();
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
    int a;
    if(SERIAL_ENABLE) {
      for(a=0;a<25;a++){
        if(houer==a)Serial.print(">");else Serial.print(" ");
        Serial.print(a);Serial.print(": ");
        Serial.println(Total_Time_Waser_leer_array[a]);
      }
      Serial.print("\nTotal: ");
      Serial.println(Time_Total_Wasser_leer_last_24_houer);
    }
  }
  
  /* Ausgänge Ansteuern */
  if(Status_Pumpe_On)
  {
    //digitalWrite(ledPin, HIGH);        // LED ein
    digitalWrite(PumpePowerPin, HIGH);   // Pumpe ein
  }
  if(Status_Pumpe_Off)
  {
    //digitalWrite(ledPin, LOW);         // LED aus
    digitalWrite(PumpePowerPin, LOW);    // Pumpe aus
  }

  /* Display aktualisieren */
  display();

}/* end loop */



/***************
Send Status SMS
****************/
void send_status_sms(){
  LCD(F("Send Status SMS"));
  
  // send the message
  sms.beginSMS("0794118653");
  
  sms.print(F("Traenke Status Bericht\n\n"));
  //hasprint.smsService.print(F("Mother fucker"));


  if(Status_Wasser_leer){
    sms.print(F("Schwimer ist Unten\n"));
    sms.print(F("Seit: "));
    sms.print( get_Hour_Min_Sec (Time_schwimmer_down) );
    sms.print(F(" Stunden: Min:Sec"));
  }
  if(Status_Wasser_voll){
    sms.print(F("Schwimer ist Oben\n"));
    sms.print(F("Seit: "));
    sms.print( get_Hour_Min_Sec( (millis()-Time_Stamp_Wasser_voll) ) );
    sms.print(F(" Stunden:Min:Sec"));
   
  }
  sms.print(F("\n\n Schwimmer Total Zeit Unten letzte 24 Std. : "));
  sms.print( get_Hour_Min_Sec( Time_Total_Wasser_leer_last_24_houer * 1000L ));
  sms.print(F(" Stunden:Min:Sec"));

  sms.print(F("\n\nSchwimmer ging nach unten Zaehler: "));
  sms.print(Counter_Schimmer_Down);

  sms.print(F("\n\nTotal Laufzeit Computer: "));
  sms.print( get_Hour_Min_Sec( millis() ) );
  sms.print(F(" Stunden:Min:Sec"));

  sms.print(F("\n\nVolt am Stop-Draht: "));
  sms.print(voltage_Stop);
  sms.print(F("\n\nA/D-Sensor Zahl: "));
  sms.print(anval_Stop);

  sms.print(F("\n\nBaterie-Spannung: "));
  sms.print(voltage_Bat/10); sms.print("."); sms.print(voltage_Bat % 10);
  sms.print(F("\n\nA/D-Sensor Zahl (Bat): "));
  sms.print(anval_Bat);

  sms.endSMS();
  
  LCD(F("Send COMPLETE!"));

}

/*******
Display 
********/ 
void display()
{

  /* Volt Bat. */
  if (screen==6){  
    
    if(screen_setup){
      lcd.clear();
      lcd.print(F("Bat.-Volt: "));
      lcd.setCursor( 0,1);             
      lcd.print(F("Sensor: "));
      screen_setup=false;
    }
    lcd.setCursor(10,0);             
    lcd.print(voltage_Bat/10);lcd.print("."); lcd.print(voltage_Bat % 10);       // volt
    lcd.setCursor(9,1);             
    lcd.print(anval_Bat);       // Analog Wandler Input
  }
  

  
  /* Volt Stop */
  if (screen==5){  
    
    if(screen_setup){
      lcd.clear();
      lcd.print(F("Stop-Volt: "));
      lcd.setCursor( 0,1);             
      lcd.print(F("Sensor: "));
      screen_setup=false;
    }
    lcd.setCursor(10,0);             
    lcd.print(voltage_Stop);       // volt
    lcd.setCursor(9,1);             
    lcd.print(anval_Stop);       // Analog Wandler Input
  }
  
  /* Runtime */
  if (screen==4){  
    
    if(screen_setup){
      lcd.clear();
      lcd.print(F("Runtime:"));
      lcd.setCursor( 0,1);             // move cursor to second line "1" and 9 spaces over
      lcd.print(F("H:M:S"));
      screen_setup=false;
    }
    lcd.setCursor(8,0);             // move cursor to second line "1" and 9 spaces over
    lcd.print (get_Hour_Min_Sec(millis()));
    /* Loop-Time */
    lcd.setCursor(12,1);
    lcd.print("    ");
    lcd.setCursor(12,1);
    lcd.print(looptime);
  }

  
  /* Anzahl Schwimmer unten   */
  if(screen==2){
    
    if(screen_setup)   
    {
      lcd.clear();                   //DFRobot_LCD_Output Ausgabe
      lcd.print(F("Schwimmer ging"));
      lcd.setCursor(0,1);
      lcd.print(F("    x nach unten"));

      screen_setup=false;
    }
      lcd.setCursor(0,1);                   //DFRobot_LCD_Output Ausgabe
      lcd.print(Counter_Schimmer_Down);  // Sec. Schwimmer Unten             
  }

  /* Schwimmer letzter Tag */
  if(screen==3){   
    
    if(screen_setup)
    {
      lcd.clear();                   //DFRobot_LCD_Output Ausgabe
      lcd.print(F("Schwimmer unten"));
      lcd.setCursor(0,1);
      lcd.print(F("         m:s/Tag"));

      screen_setup=false;
    }
      lcd.setCursor(0,1);                   //DFRobot_LCD_Output Ausgabe
      lcd.print( get_Min_Sec (Time_Total_Wasser_leer_last_24_houer*1000L) );  // Sec. Schwimmer Unten             
  }

  
  /* Schwimmer Aktuell */
  if(screen==1){ 
    
    if(screen_setup)
    {
      lcd.clear();                   //DFRobot_LCD_Output Ausgabe
      if(Status_Wasser_leer)
        lcd.print(F("Schwimmer unten"));
      if(Status_Wasser_voll)
        lcd.print("Schwimmer oben");
      lcd.setCursor(0,1);
      lcd.print(F("           h:m:s"));

      screen_setup=false;
    }

    if(Status_Wasser_leer){
      lcd.setCursor(0,1);                   //DFRobot_LCD_Output Ausgabe
      lcd.print( get_Hour_Min_Sec(Time_schwimmer_down) );  // Sec. Schwimmer Unten             
    }
    if(Status_Wasser_voll){
      lcd.setCursor(0,1);
      lcd.print( get_Hour_Min_Sec( (millis()  -Time_Stamp_Wasser_voll)) );             
      
    } 
  }

}/* end display */
#endif

