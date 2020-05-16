/*************************************************************************************

  DFRobot_LCD_Output Shield für Arduino

  DFRobot_LCD_Output Display für Arduino 2 x 16 Zeichen
  
  Mark Bramwell, July 2010

  This program will test the DFRobot_LCD_Output panel and the buttons.When you push the button on the shield，
  the screen will show the corresponding one.
 
  Connection: Plug the DFRobot_LCD_Output Keypad to the UNO(or other controllers)

**************************************************************************************/

#include <DFROBOT_LCD_SHIELD.h>

//GEN 2



//GEN 1

//lcd (8, 9, 4, 5, 6, 11);
//LiquidCrystal lcd(8, 9, 4, 5, 6, 11);           // select the pins used on the DFRobot_LCD_Output panel
                                                //a.l. Pin 11! nicht Pin 7 (von GSM-Shield verwendet
// define some values used by the panel and buttons
int lcd_key     = 0;
int adc_key_in  = 0;

extern int screen;
extern long screen_timmer;



/// ***** GEN 2 *****************

/**  Konstruktor * */
DFROBOT_LCD_SHIELD::DFROBOT_LCD_SHIELD(uint8_t rs, uint8_t enable, uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t Analog_pin)
        : LiquidCrystal(rs, enable, d0, d1, d2, d3)
{

    DFRobot_analog_read_pin= Analog_pin;
}

/** Read the Butoons * */
int DFROBOT_LCD_SHIELD::DFRobot_read_LCD_buttons() {
    DFRobot_adc_key_in = analogRead(DFRobot_analog_read_pin);
    return DFRobot_adc_key_in;
}

/** Setup * */
void DFROBOT_LCD_SHIELD::setup() {
    begin(16,2);
    setCursor(0,0);
    print(F("Push the buttons"));
}

/** Loop * */
void DFROBOT_LCD_SHIELD::loop(){

    setCursor(0,1);             // move to the begining of the second line
    lcd_key = DFRobot_read_LCD_buttons();   // read the buttons

    /* Screen Display Managment. Zwischen verschieden Anzeigen wechseln  */
    if (millis() - screen_timmer > screen_display_time)
    {
        screen_timmer = millis();
        screen_setup = true;
        screen++;  /* Nächster Bildschirm */
        if (screen > screen_numbers)  screen = 1;
    }


    switch (lcd_key){               // depending on which button was pushed, we perform an action

        case btnRIGHT:{             //  push button "RIGHT" and show the word on the screen
            LCD(F("RIGHT "));
            break;
        }
        case btnLEFT:{
            LCD(F("LEFT   ")); //  push button "LEFT" and show the word on the screen
            break;
        }
        case btnUP:{
            LCD(F("UP    "));  //  push button "UP" and show the word on the screen
            break;
        }
        case btnDOWN:{
            LCD(F("DOWN  "));  //  push button "DOWN" and show the word on the screen
            break;
        }
        case btnSELECT:{
            LCD(F("SELECT"));  //  push button "SELECT" and show the word on the screen
            break;
        }
        case btnNONE:{
            //lcd.print(F("NONE  "));  //  No action  will show "None" on the screen
            break;
        }
    }
}

/** Textausgabe*/
void DFROBOT_LCD_SHIELD::DFRobot_LCD_Output(const __FlashStringHelper *s) {
    screen=0;
    screen_timmer=millis();
    clear();
    print(s);
}

/** Textausgabe*/
void DFROBOT_LCD_SHIELD::DFRobot_LCD_Output(char *s) {
    screen=0;
    screen_timmer=millis();
    clear();
    //lcd.setCursor(0,0);
    print(s);
}


/// *********** GEN 1 *************
#ifdef GEN1

int read_LCD_buttons(){               // read the buttons
    adc_key_in = analogRead(0);       // read the value from the sensor



    // my buttons when read are centered at these valies: 0, 144, 329, 504, 741
    // we add approx 50 to those values and check to see if we are close
    // We make this the 1st option for speed reasons since it will be the most likely result

    if (adc_key_in > 1000) return btnNONE;

    // For V1.1 us this threshold
    if (adc_key_in < 50)   return btnRIGHT;
    if (adc_key_in < 250)  return btnUP;
    if (adc_key_in < 450)  return btnDOWN;
    if (adc_key_in < 650)  return btnLEFT;
    if (adc_key_in < 850)  return btnSELECT;

    // For V1.0 comment the other threshold and use the one below:
    /*
      if (DFRobot_adc_key_in < 50)   return btnRIGHT;
      if (DFRobot_adc_key_in < 195)  return btnUP;
      if (DFRobot_adc_key_in < 380)  return btnDOWN;
      if (DFRobot_adc_key_in < 555)  return btnLEFT;
      if (DFRobot_adc_key_in < 790)  return btnSELECT;
    */

    return btnNONE;                // when all others fail, return this.
}


/* Text auf Display aus Flash */
void LCD (const __FlashStringHelper* s)

{
    screen=0;
    screen_timmer=millis();
    lcd.clear();
    lcd.print(s);
}

void setup_LCD()                   /* DFRobot_LCD_Output Setup
---------------                    */
{
    lcd.begin(16, 2);               // start the library
    lcd.setCursor(0,0);             // set the DFRobot_LCD_Output cursor   position
    lcd.print(F("Push the buttons"));  // print a simple message on the DFRobot_LCD_Output
}

void loop_LCD()                    /* DFRobot_LCD_Output Loop
--------------                     */
{

    lcd.setCursor(0,1);             // move to the begining of the second line
    lcd_key = read_LCD_buttons();   // read the buttons

    switch (lcd_key){               // depending on which button was pushed, we perform an action

        case btnRIGHT:{             //  push button "RIGHT" and show the word on the screen
            LCD(F("RIGHT "));
            break;
        }
        case btnLEFT:{
            LCD(F("LEFT   ")); //  push button "LEFT" and show the word on the screen
            break;
        }
        case btnUP:{
            LCD(F("UP    "));  //  push button "UP" and show the word on the screen
            break;
        }
        case btnDOWN:{
            LCD(F("DOWN  "));  //  push button "DOWN" and show the word on the screen
            break;
        }
        case btnSELECT:{
            LCD(F("SELECT"));  //  push button "SELECT" and show the word on the screen
            break;
        }
        case btnNONE:{
            //lcd.print(F("NONE  "));  //  No action  will show "None" on the screen
            break;
        }
    }
}

/* Text auf Display */
void LCD_notused (char* s)
{
    screen=0;
    screen_timmer=millis();
    lcd.clear();
    //lcd.setCursor(0,0);
    lcd.print(s);

    //lcd.print("                 ");


}

#endif
