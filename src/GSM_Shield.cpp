/*
 *   Arduino GSM Shield V2 für Ardunio
 *   kann SMS senden und empfangen
 */
#include <Arduino.h>
#include <main.h>
#include <GSM_Shield.h>

#define KONVENTIONEL true
#define MIT_KLASSE true

#if MIT_KLASSE

#endif //MIT_KLASSE



#if KONVENTIONEL
    GSM gsmAccess;
    GSM_SMS sms;

    char senderNumber[20]; // Array to hold the number a SMS is retreived from



    boolean serial_debug = false;



    void GSM_Init()           /* GSM Initialisierung
    ---------------           */
    {

        Serial.print(F("GSM Intitialis."));

        // connection state
        boolean notConnected = true;

        /*
         *  Start GSM shield
         *  If your SIM has PIN, pass it as a parameter of begin() in quotes
         */

        while (notConnected) {
            if (gsmAccess.begin("") == GSM_READY) {
                notConnected = false;
#if DFROBOT_LCD_SHIELD_ENABLE
                my_dfRobotLcdShield.print(F("GSM initialized"));
#endif

            } else {
#if DFROBOT_LCD_SHIELD_ENABLE
                my_dfRobotLcdShield.print(F("Not connected"));
#endif
                delay(1000);

            }
        }


    }

    bool recieve_SMS(char* s)             /* SMS empfangen
    -------------------------             */
    {
      char c;
      int count=0;
      if (sms.available())
      {
        if(SERIAL_ENABLE)
          Serial.println(F("Message received from:"));

        // Get remote number
        sms.remoteNumber(senderNumber, 20);
        if(SERIAL_ENABLE)
          Serial.println(senderNumber);

        // An example of message disposal
        // Any messages starting with # should be discarded
        if (sms.peek() == '#')
        {
          if(SERIAL_ENABLE)
            Serial.println(F("Discarded SMS"));
          sms.flush();
        }
        /*
         * Read message bytes and print them
         */
#if DFROBOT_LCD_SHIELD_ENABLE
          my_dfRobotLcdShield.setCursor(0,0);             // set the DFRobot_LCD_Output cursor   position
#endif
        while (c = sms.read()){
          if(SERIAL_ENABLE)
            Serial.print(c);
          s[count++]=c;
#if DFROBOT_LCD_SHIELD_ENABLE
            my_dfRobotLcdShield.write(c);
#endif
        }
        s[count]=0;

        if(SERIAL_ENABLE)
          Serial.println(F("\nEND OF MESSAGE"));

        // Delete message from modem memory
        sms.flush();
        if(SERIAL_ENABLE)
          Serial.println(F("MESSAGE DELETED"));
        return true;
      }
      else
      {
        s[count]=0;
        return false;
      }

    }

    void send_SMS(const __FlashStringHelper* s)   /* SMS aus Flash senden
    ------------------------------------------    */
    {

#if DFROBOT_LCD_SHIELD_ENABLE
      my_dfRobotLcdShield.print(F("Send SMS"));
#endif

      // send the message
      sms.beginSMS("0794118653");
      sms.print(s);
      sms.endSMS();

#if DFROBOT_LCD_SHIELD_ENABLE
      my_dfRobotLcdShield.print(F("Send COMPLETE!"));
#endif

      delay(1000);
    }

    void send_SMS(char s[])                        /* SMS Senden
    ----------------------                         */
    {
#if DFROBOT_LCD_SHIELD_ENABLE
      my_dfRobotLcdShield.print(F("Send SMS"));
#endif

      // send the message
      sms.beginSMS("0794118653");
      sms.print(s);
      sms.endSMS();

#if DFROBOT_LCD_SHIELD_ENABLE
      my_dfRobotLcdShield.print(F("Send COMPLETE!"));
#endif

      delay(1000);

    }

#endif //KONVENTIONEL