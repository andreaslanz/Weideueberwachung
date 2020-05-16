//
// Created by andi on 05.03.2020.
//

#ifndef INC_200224_141205_UNO_DFROBOT_LCD_SHIELD_H
#define INC_200224_141205_UNO_DFROBOT_LCD_SHIELD_H

#include <LiquidCrystal.h>
#include <Arduino.h>


#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

//GEN 1
//GEN 2
class DFROBOT_LCD_SHIELD : public LiquidCrystal
{
public:

    //lcd (8, 9, 4, 5, 6, 11);
    //a.l. Pin 11! nicht Pin 7 (von GSM-Shield verwendet
    DFROBOT_LCD_SHIELD(uint8_t rs, uint8_t enable, uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3
                      , uint8_t analog_read_pin);

    int DFRobot_read_LCD_buttons();       // read the buttons

    /* Display Screens */
    int screen=0;
    const int screen_numbers=6;
    long screen_timmer=millis();
    long screen_display_time= 3000L;
    bool screen_setup;

    void setup();             // DFRobot_LCD_Output Setup
    void loop();              // DFRobot_LCD_Output Loop
    template <typename T>
    void LCD(T s){
        screen=0;
        screen_timmer=millis();
        clear();
        print(s);
    }
    void DFRobot_LCD_Output (char* s);           // DFRobot_LCD_Output Output
    void DFRobot_LCD_Output (const __FlashStringHelper* s);  //DFRobot_LCD_Output Output

private:
    // define some values used by the panel and buttons
    int DFRobot_lcd_key     = 0;
    int DFRobot_adc_key_in  = 0;
    uint8_t  DFRobot_analog_read_pin;
};

#endif //INC_200224_141205_UNO_DFROBOT_LCD_SHIELD_H
