//
// Created by andi on 06.03.2020.
//

#ifndef INC_200224_141205_UNO_STOP_H
#define INC_200224_141205_UNO_STOP_H

//#include <IO.h>
//#include <main.h>

#ifdef GEN1
extern int  voltage_Stop;    //Volt Stop
extern int  voltage_Bat;    //Volt Baterie
extern int  anval_Bat;      //Analog Wandler Wert Baterie
extern int  anval_Stop;      //Analog Wandler Wert Stop
#endif

#ifdef GEN2
/***********************************
 *
 *          Modul Stop
 *
 * *********************************/
class STOP
{

public:
/**Funktionen*/
    STOP(uint8_t pin_Stop, uint8_t pin_Bat, long intervall);
    void setup();
    void loop();
/**Variablen*/
    int  voltage_Bat;    //Volt Baterie
    int  voltage_Stop;    //Volt Stop
    int  anval_Stop;      //Analog Wandler Wert Stop

private:
    int analogInPin_Stop;  // Analog input von Stop Spannungsteiler
    int Bat_Analog_In;    // Baterie-Spannungs Messung
    long Time_zwischen_Wire_read;
    long Time_Stamp_next_wire_read;
    long Time_Stamp_last_wire_read;
    int  anval_Bat;      //Analog Wandler Wert Baterie

};



#endif


#endif //INC_200224_141205_UNO_STOP_H
