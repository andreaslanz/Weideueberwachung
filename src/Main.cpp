/***************************************

   Weide Ueberwachung (Stop / Wasser)

   SMS Alarmierung

****************************************/
//GEN 1
#include <Arduino.h>
//#include <main.h>
#include "main.h"
#include <avr/wdt.h>
#include <EEPROM.h>
//GEN 2
#include <DFROBOT_LCD_SHIELD.h>
#include <IO.h>
#include <stop.h>
#include <PUMPENSTEUERUNG.h>

/*
 *
 * Globale Variablen
 *
 * */


// GEN 2
/*Module*/
#if DFROBOT_LCD_SHIELD_ENABLE
DFROBOT_LCD_SHIELD my_dfRobotLcdShield(IO_my_UNO_PINS_DFROBOTSHIELD);
#endif
PUMPENSTEUERUNG my_pumpensteurung(IO_my_UNO_PUMPENSTEUERUNG_PINS);
STOP my_stop(IO_my_UNO_STOP_BAT_PINS,1000);



//GEN 1
/* LoopTime */
long looptime=0L; //Loop Time 

/* Display Screens */
int screen=0;

void HC_05_Setup() {
    pinMode(IO_my_UNO_HC05_SHIELD_RESET, OUTPUT); /*HC-05 Bluetooth Setup*/
    HC_05_On();
}
void HC_05_On() {
    digitalWrite(IO_my_UNO_HC05_SHIELD_RESET ,LOW); /*LOW = HC-05 Power On*/
}
void HC_05_Off() {
    digitalWrite(IO_my_UNO_HC05_SHIELD_RESET ,HIGH); /*HIGH = HC-05 Power Off*/
}
void HC_05_Reset() {
    HC_05_Off(); delay(500);
    HC_05_On(); delay(500);
}


const int screen_numbers=6;
long screen_timmer=millis();
long screen_display_time= 3000L; 
bool screen_setup;

/* Watchdog Timer */
const int num_watch_dog_overflows =3; //Anzahl Watchdog-Timer-Overflows bis reset
int volatile wd_count; //Reset nach Anzahl Overflows
const long Time_SMS_interval= 86400000L; //24L*60L*60L*1000L; // Status SMS senden Interval 24 Stunden

/* DFRobot DFRobot_LCD_Output Shield Stuff */
extern int lcd_key; //Taste

const String Help_Text =        "\r\n\r\nCommands:\r\n---------"
                                "\r\ng - Get Status"
                                "\r\nexx - Einschaltzeit-Pumpe(xx=Sec.)"
                                "\r\naxx - Ausschaltzeit-Pumpe(xx=Sec.)";



#ifdef alt
/* Externe Variablen */
extern int Status_Pumpe_On;
extern int Status_Pumpe_Off;
/* externe Variablen */
/* Pumpensteuerung */
extern long       Time_Stamp_next_SMS;                  // Nächste SMS
extern long       Time_Stamp_last_SMS;                  // Letzte SMS
extern long       Time_Stamp_last_test_SMS;             // Letzte Test-SMS
#endif

ISR(WDT_vect) {
  wdt_reset(); // reset watchdog counter
#if DFROBOT_LCD_SHIELD_ENABLE
  my_dfRobotLcdShield.clear();
  my_dfRobotLcdShield.print("Overflow ");
  my_dfRobotLcdShield.print(String(wd_count,DEC));
#endif
  if(wd_count-- > 0) //Overflowzähler bei 0 Reset
    WDTCSR |= (1<<WDIE); // reenable interrupt to prevent system reset
}



/*************
SETUP Function 
***************/
void setup()
{
    //GEN2
#if DFROBOT_LCD_SHIELD_ENABLE
    my_dfRobotLcdShield.setup();
#endif

    my_pumpensteurung.setup();
    my_stop.setup();
    HC_05_Setup();
    HC_05_Reset();



    /* Setup Serial */
    if (SERIAL_ENABLE) {
        Serial.begin(115200);
        while (!Serial) {}
    }

    /* Begrüssungszext anzeigen */
#if SERIAL_ENABLE
    print_to(hasprint.ser,F("\r\n\r\nWeideueberwachung\r\n---------------_--"));
    print_to(hasprint.ser,F("\r\nPress h for Help"));
    print_to(hasprint.ser,Help_Text);
#endif
#if DFROBOT_LCD_SHIELD_ENABLE
    my_dfRobotLcdShield.LCD(F("WeideUeberwacher"));
#endif



    /* Setup GSM Shield  */
    if (GSM_ENABLE)
        GSM_Init();


    /* Watchdog enable 8 Sekunden  */
    wdt_enable(WDTO_8S);
    WDTCSR |= (1 << WDCE) | (1 << WDIE); //Set Watchdog Interrupt Enable
    wd_count = num_watch_dog_overflows; //Overflowzähler
    sei(); // globally enable interrupts


}/* Ende Setup */



/*********
Main loop
***********/
void loop() {
    int i;
    if(Serial.available()){
        i=Serial.read();
        switch (i) {
            case 'g': { /// Status
                print_to(hasprint.ser, "\r\n");
                my_pumpensteurung.send_status(hasprint.ser);
                break;
            }
            case 'e': { /// Einschaltzeit
                unsigned long val = hasprint.ser.parseInt();
                print_to(hasprint.ser, "\r\nNEW Einschaltzeit(Sec.): ");
                print_to(hasprint.ser, val);
                my_pumpensteurung.Time_Pumpe_On=val*1000L;
                my_pumpensteurung.set_Einschaltzeit_to_EEPROM(val*1000L);
                break;
            }
            case 'a': { /// Ausschaltzeit
                unsigned long val = hasprint.ser.parseInt();
                print_to(hasprint.ser, "\r\nNEW Ausschaltzei(Sec.): ");
                print_to(hasprint.ser, val);
                my_pumpensteurung.Time_Pumpe_Off=val*1000L;
                my_pumpensteurung.set_Ausschaltzeit_to_EEPROM(val*1000L);
                break;
            }
            case 'h': { /// Help
                print_to(hasprint.ser, F("\r\n\r\nCommands:\r\n---------"));
                print_to(hasprint.ser, F("\r\ng - Get Status"));
                print_to(hasprint.ser, F("\r\nexx - Einschaltzeit-Pumpe(xx=Sec.)"));
                print_to(hasprint.ser, F("\r\naxx - Ausschaltzeit-Pumpe(xx=Sec.)"));
                break;
            }
            default: {
                print_to(hasprint.ser, F("\r\nPress H for Help "));
            }
        }

    }



    /* Debugg */
#if DEBUGG_MAIN
    static unsigned long last_print;
    long loop_start = millis();
#endif
    /* Pumpensteuerung   */
    //GEN 2
    my_pumpensteurung.loop();
    my_stop.loop();
#if DFROBOT_LCD_SHIELD_ENABLE
    my_dfRobotLcdShield.loop();

    /* Send Test SMS bei Knopf SELECT   */
    if (lcd_key == btnSELECT) {

        if (GSM_ENABLE)
            send_SMS(F("Test2"));
    }

    /* Pumpe mit Tast ein   */
    if (lcd_key == btnUP) {
        //Status_Pumpe_On=true; Status_Pumpe_Off=false;   // Pumpe ein
        /***GEN 2*/
        my_pumpensteurung.Status_Pumpe_On = true;
        my_pumpensteurung.Status_Pumpe_Off = false;   // Pumpe ein
    }
    /* Pumpe mit Taste aus   */
    if (lcd_key == btnDOWN) {
        //Status_Pumpe_On=false; Status_Pumpe_Off=true;   // Pumpe aus
        /***GEN2*/
        my_pumpensteurung.Status_Pumpe_On = false;
        my_pumpensteurung.Status_Pumpe_Off = true;   // Pumpe aus
    }
#endif

    /* GSM Shield auf SMS abfragen */
    char anser[128];
#if GSM_ENABLE
    /* Recieve SMS  */
    if (recieve_SMS(anser)) {
#if DFROBOT_LCD_SHIELD_ENABLE
        my_dfRobotLcdShield.LCD(anser);
#endif

        /* Pump On  */
        if (anser[0] == '1') {
#if DFROBOT_LCD_SHIELD_ENABLE
            my_dfRobotLcdShield.LCD("Pumpe On");
#endif
            //Status_Pumpe_On=true; Status_Pumpe_Off=false;   // Pumpe ein
            /**GEB2*/
            my_pumpensteurung.Status_Pumpe_On = true;
            my_pumpensteurung.Status_Pumpe_Off = false;   // Pumpe ein
        }
        /* Pumpe Aus  */
        if (anser[0] == '0') {
#if DFROBOT_LCD_SHIELD_ENABLE
            my_dfRobotLcdShield.LCD("Pumpe Aus");
#endif
            //Status_Pumpe_On=false; Status_Pumpe_Off=true;   // Pumpe aus
            /***GEN2*/
            my_pumpensteurung.Status_Pumpe_On = false;
            my_pumpensteurung.Status_Pumpe_Off = true;   // Pumpe aus
        }
        /* Test SMS  */
        if (anser[0] == 't') {
#if DFROBOT_LCD_SHIELD_ENABLE
            my_dfRobotLcdShield.LCD("Send Test");
#endif
            send_SMS("hello i got your message");
        }
        /* Status SMS  */
        if (anser[0] == 's') {
#if DFROBOT_LCD_SHIELD_ENABLE
            my_dfRobotLcdShield.LCD("Send Status");
#endif
            //send_status_sms();
            //GEN2
            //my_pumpensteurung.send_status_sms();
            //my_pumpensteurung.send_status(Serial);
            //Time_Stamp_next_SMS = millis() + Time_SMS_interval;
            //Time_Stamp_last_test_SMS = millis();
            /***GEN2*/
            my_pumpensteurung.Time_Stamp_next_SMS = millis() + Time_SMS_interval;
            my_pumpensteurung.Time_Stamp_last_test_SMS = millis();
        }
    }

    /* Intervall SMS Time_Stamp_last_test_SMS*/
    //if( millis() > Time_Stamp_next_SMS )
    //if( millis() - Time_Stamp_last_test_SMS >  Time_SMS_interval)
    /***GEN2*/
    if (millis() - my_pumpensteurung.Time_Stamp_last_test_SMS > Time_SMS_interval) {
        //send_status_sms();
        /**GEN2*/
        my_pumpensteurung.send_status_sms();
        //Time_Stamp_next_SMS = millis() + Time_SMS_interval;
        //Time_Stamp_last_test_SMS = millis();
        /***GEN2*/
        my_pumpensteurung.Time_Stamp_next_SMS = millis() + Time_SMS_interval;
        my_pumpensteurung.Time_Stamp_last_test_SMS = millis();
    }
#endif

    /* Screen Display Managment. Zwischen verschieden Anzeigen wechseln  */
    if (millis() - screen_timmer > screen_display_time) {
        screen_timmer = millis();
        screen_setup = true;
        screen++;  /* Nächster Bildschirm */
        if (screen > screen_numbers) screen = 1;
    }

    /* Watchdog reset  */
    //__asm__ __volatile__ ("wdr");
    wdt_reset(); //WD Zähler auf 0
    wd_count = num_watch_dog_overflows;  //WD-Overflow-Zähler

    /* Debugg  */
#if DEBUGG_MAIN
    looptime = millis()-loop_start;
#if SERIAL_ENABLE
    if (millis() - last_print > 1000) {

        Serial.println(millis() - loop_start);

        last_print = millis();
    }

#endif
#endif
}
/* Ende Loop */


