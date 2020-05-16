#ifndef main_h
#define main_h

//GEN 1
//#include <LiquidCrystal.h>
//#include <GSM.h>
/* Module Ein/Aus */
#define GSM_ENABLE false         // GSM Ein/Aus  __VA_ARGS__
#define SERIAL_ENABLE true      // Seriel Ausgabe Ein/Aus
#define DFROBOT_LCD_SHIELD_ENABLE false      // Seriel Ausgabe Ein/Aus
#define DEBUGG_STOP false
#define DEBUGG_MAIN false
#define DEBUGG_PUMPENSTEUERUNG false
#define BLUTOOTH_ENABLE true      //Bluetooth Ausgabe Ein/Aus
#define GEN2
#define GEN1
//GEN 2
#include <DFROBOT_LCD_SHIELD.h>
#include <stop.h>

//GEN 2
//Globale Variablen
#if DFROBOT_LCD_SHIELD_ENABLE
extern DFROBOT_LCD_SHIELD my_dfRobotLcdShield;
#endif

void HC_05_Setup();
void HC_05_On();
void HC_05_Off();
void HC_05_Reset();


extern STOP my_stop;
extern int screen;
extern bool screen_setup;
extern long looptime; //Loop Time

//GEN 1





#endif



