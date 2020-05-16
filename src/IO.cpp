//
// Created by andi on 17.03.2020.
//
#include "main.h"
#include "GSM_Shield.h"
#include <IO.h>

/************************
 * HasPrint Struct
 *
 * Stuktur mit Kanälen
 *
 *
 * **********************/
#if DFROBOT_LCD_SHIELD_ENABLE
stru hasprint{Serial,my_dfRobotLcdShield,sms};
#endif
stru hasprint{Serial,
#if DFROBOT_LCD_SHIELD_ENABLE
        my_dfRobotLcdShield,
#endif
              sms};
/*
struct stru {
    HardwareSerial& ser =Serial;  //ser ist eine Referenz
    DFROBOT_LCD_SHIELD& lcd =my_dfRobotLcdShield;
    GSM3SMSService& smsService =sms;
} hasprint;
 */


