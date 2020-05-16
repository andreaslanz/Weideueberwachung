//
// Created by andi on 05.03.2020.
//

#ifndef INC_200224_141205_UNO_GSM_SHIELD_H
#define INC_200224_141205_UNO_GSM_SHIELD_H

#define KONVENTIONEL true
#define MIT_KLASSE true

#if MIT_KLASSE

#endif //MIT_KLASSE




#if KONVENTIONEL

    #include <GSM.h>
    /* GSM-Shield Stuff */
    extern boolean serial_debug;
    template <class T>
    T& Debug_Output_Chanel=NULL;
    template <class T>
    void set_gsm_serial_debug_on(T &debug_chanel)
    {
        serial_debug=true;
        T Debug_Output_Chanel=debug_chanel;
    }
    extern GSM gsmAccess;
    extern GSM_SMS sms;
    void GSM_Init();
    void send_SMS(const __FlashStringHelper* s) ;
    void send_SMS(char s[]);
    bool recieve_SMS(char* s);

#endif //KONVENTIONEL


#endif //INC_200224_141205_UNO_GSM_SHIELD_H
