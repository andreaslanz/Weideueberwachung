//Wire zu Stop Arduino
#include <Arduino.h>
#include <main.h>
#include <stop.h>


#ifdef GEN1
//Pin Definition
const int analogInPin_Stop = A5;  // Analog input von Stop Spannungsteiler
const int Bat_Analog_In = A2;    // Baterie-Spannungs Messung

long Time_zwischen_Wire_read = 1000L;
long Time_Stamp_next_wire_read;
long Time_Stamp_last_wire_read;
int  voltage_Stop;    //Volt Stop
int  anval_Stop;      //Analog Wandler Wert Stop
int  voltage_Bat;    //Volt Baterie
int  anval_Bat;      //Analog Wandler Wert Baterie


#ifdef alt
void wire_loop() 
{

  //Request from Slave
  //------------------

  if (millis() - Time_Stamp_last_wire_read > Time_zwischen_Wire_read)  // pause
  {
    Time_Stamp_next_wire_read += Time_zwischen_Wire_read;
    Time_Stamp_last_wire_read = millis();
    

    // Direkt Analog Read
    anval_Stop = analogRead(analogInPin_Stop);
    delay(1);
    anval_Bat = analogRead(Bat_Analog_In);


    voltage_Stop = map(anval_Stop, 385, 460, 4000, 5000);// Gemessen 5V Arduino / Lanker Voltmeter

    voltage_Bat = map(anval_Bat, 328, 570, 80, 140); //Baterie Spannung Teiler: 10k/39k

    if(SERIAL_ENABLE) {
      Serial.print("volt-STOP: ");
      Serial.println(voltage_Stop);
      Serial.print("volt-bat: "); 
      Serial.print(voltage_Bat/10);Serial.print(".");Serial.println(voltage_Bat % 10);

      }
  }

}


void wire_init()
{

  Time_Stamp_next_wire_read = millis() + Time_zwischen_Wire_read;
  Time_Stamp_last_wire_read = millis();
  
}
#endif

#endif

#ifdef GEN2
/****************************
 *
 * Konstruktor Stop Modul
 *
 * ***************************/
STOP::STOP(uint8_t pin_Stop, uint8_t pin_Bat, long intervall)
{
    Time_zwischen_Wire_read=intervall;
    anval_Bat=pin_Bat;
    Bat_Analog_In=pin_Stop;
}
/****************************
 *
 * Stop Loop
 *
 * **************************/
void STOP::loop() {
    if (millis() - Time_Stamp_last_wire_read > Time_zwischen_Wire_read)  // pause
    {
        Time_Stamp_next_wire_read += Time_zwischen_Wire_read;
        Time_Stamp_last_wire_read = millis();


        // Direkt Analog Read
        anval_Stop = analogRead(analogInPin_Stop);
        delay(1);
        anval_Bat = analogRead(Bat_Analog_In);


        voltage_Stop = map(anval_Stop, 385, 460, 4000, 5000);// Gemessen 5V Arduino / Lanker Voltmeter

        voltage_Bat = map(anval_Bat, 328, 570, 80, 140); //Baterie Spannung Teiler: 10k/39k
#if DEBUGG_STOP
#if SERIAL_ENABLE
            Serial.print("volt-STOP: ");
            Serial.println(voltage_Stop);
            Serial.print("volt-bat: ");
            Serial.print(voltage_Bat/10);Serial.print(".");Serial.println(voltage_Bat % 10);
#endif
#endif
    }

}
/****************************
 *
 * Init
 *
 * **************************/
void STOP::setup() {
    Time_Stamp_next_wire_read = millis() + Time_zwischen_Wire_read;
    Time_Stamp_last_wire_read = millis();

}

#endif



