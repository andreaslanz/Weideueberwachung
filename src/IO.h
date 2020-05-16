//
// Created by andi on 07.03.2020.
//

#ifndef INC_200224_141205_UNO_IO_H
#define INC_200224_141205_UNO_IO_H

#include <pins_arduino.h>
#include "main.h"
#include "GSM_Shield.h"
#include <GSM.h>


/**************************************************************
 *  Input-Output PIN Definitionen
 *************************************************************/

// !!! ACHTUNG PIN 7 wird nicht vom DFRoobotLCDShield benuzt SONDERN vom GSM-Schield
// !!! Anstelle von PIN 7 verwendet das DFRoobotLCDShield PIN 11


#define IO_my_UNO_DFROBOTSHIELD_RS 8
#define IO_my_UNO_DFROBOTSHIELD_EN 9
#define IO_my_UNO_DFROBOTSHIELD_D0 4
#define IO_my_UNO_DFROBOTSHIELD_D1 5
#define IO_my_UNO_DFROBOTSHIELD_D2 6
#define IO_my_UNO_DFROBOTSHIELD_D3 11  //!!! ACHTUNG
#define IO_my_UNO_DFROBOTSHIELD_BUT A0
#define IO_my_UNO_PINS_DFROBOTSHIELD IO_my_UNO_DFROBOTSHIELD_RS, IO_my_UNO_DFROBOTSHIELD_EN, IO_my_UNO_DFROBOTSHIELD_D0, IO_my_UNO_DFROBOTSHIELD_D1,\
                       IO_my_UNO_DFROBOTSHIELD_D2, IO_my_UNO_DFROBOTSHIELD_D3, IO_my_UNO_DFROBOTSHIELD_BUT

#define IO_my_UNO_PUMPENSTEUERUNG_POWER_OUT A1
#define IO_my_UNO_PUMPENSTEUERUNG_SCHWIMMER_IN A3
#define IO_my_UNO_PUMPENSTEUERUNG_PINS IO_my_UNO_PUMPENSTEUERUNG_POWER_OUT, IO_my_UNO_PUMPENSTEUERUNG_SCHWIMMER_IN

#define IO_my_UNO_BAT_PIN A2
#define IO_my_UNO_STOP_PIN A5
#define IO_my_UNO_STOP_BAT_PINS IO_my_UNO_BAT_PIN, IO_my_UNO_STOP_PIN

#define IO_my_UNO_GSM_SHIELD_TX 3
#define IO_my_UNO_GSM_SHIELD_RX 2
#define IO_my_UNO_GSM_SHIELD_RESET 7

///HC-05 Bluetooth Shield
#define IO_my_UNO_HC05_SHIELD_RESET A4
#define IO_my_UNO_HC05_SHIELD_Button 13



#ifdef ART
/**
                        +-----+
                        +----[PWR]-------------------| USB |--+
                        |                            +-----+  |
                        |         GND/RST2  [ ][ ]            |
                        |       MOSI2/SCK2  [ ][ ]  A5/SCL[ ] |   C5
                        |          5V/MISO2 [ ][ ]  A4/SDA[ ] |   C4
                        |                             AREF[ ] |
                        |                              GND[ ] |
                        | [ ]N/C                    SCK/13[ ] |   B5
                        | [ ]IOREF                 MISO/12[ ] |   .  HC-05 Button
                    C6  | [ ]RESET                 MOSI/11[ ]~|   .  DF-Robot d2 !!Nicht PIN 7!!
                        | [ ]3V3    +---+               10[ ]~|   .
                        | [ ]5V     | A |                9[ ]~|   .  DF-Robot en
                        | [ ]GND   -| R |-               8[ ] |   B0 DF-Robot rs
                        | [ ]GND   -| D |-                    |
                        | [ ]Vin   -| U |-               7[ ] |   D7 GSM-Shield Reset !!Normal DFRobot
                        |          -| I |-               6[ ]~|   .  DF-Robot d2
    DF-Robot But.   C0  | [ ]A0    -| N |-               5[ ]~|   .  DF-Robot d1
    Pumpe Power     C1  | [ ]A1    -| O |-               4[ ] |   .  DF-Robot d0
    Bat.Volt        C2  | [ ]A2     +---+           INT1/3[ ]~|   .  GSM-Shield TX
    Schwimmer       C3  | [ ]A3                     INT0/2[ ] |   .  GSM-Shield RX
    HC-05 Reset     C4  | [ ]A4/SDA  RST SCK MISO     TX>1[ ] |   .
    Stop Volt       C5  | [ ]A5/SCL  [ ] [ ] [ ]      RX<0[ ] |   D0
                        |            [ ] [ ] [ ]              |
                        |  UNO_R3    GND MOSI 5V  ____________/
                        \_______________________/

http://busyducks.com/ascii-art-arduinos

 */
#endif


/************************
 * HasPrint Struct
 *
 * Stuktur mit Kanälen
 *
 * Verwendung: hasprint.ser.print("Text"); //Ausgabe Serielle Schnittstelle
 *
 * **********************/
typedef  struct stru {
    HardwareSerial& ser;// =Serial;  //ser ist eine Referenz
#if DFROBOT_LCD_SHIELD_ENABLE
    DFROBOT_LCD_SHIELD& lcd;// =my_dfRobotLcdShield;
#endif
    GSM3SMSService& smsService;// =sms;
};
extern struct stru hasprint;

/**********************************
 * Print to mit Template Funktion
 * ********************************/
template <class to, typename s>
void print_to ( to &to1, s string) {
    to1.print(string);
}






#endif //INC_200224_141205_UNO_IO_H
