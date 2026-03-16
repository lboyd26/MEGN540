#include "Lab3_Tasks.h"

static Filter_Data_t battery_filter;
static bool battery_filter_initialized = false;

void Send_Loop_Battery( float _time_since_last )
{


    struct __attribute__((__packed__)) {
        float voltage;
    } data;
    data.voltage = Filter_Last_Output(&battery_filter);
    USB_Send_Msg("cf",'B', &data, sizeof(data));


    static bool warned = false;
    if(Filter_Last_Output(&battery_filter) < 4.0f){
        if (!warned){
            struct __attribute__((__packed__)) {
                char let[7];
                float volt;
            } msg = { .let = {'B','A','T',' ','L','O','W'}, .volt = Filter_Last_Output(&battery_filter) };
            USB_Send_Msg("c7sf", '!', &msg, sizeof(msg));
            warned = true;
        }
    else{
        warned = false;
    }

    }

}
void Send_Battery_Now( float _time_since_last )
{


  struct __attribute__((__packed__)) {
        float voltage;
    } data;
    data.voltage = Filter_Last_Output(&battery_filter);
    USB_Send_Msg("cf",'b', &data, sizeof(data));

}

void Send_Encoder_Now( float _time_since_last )
{
    struct __attribute__((__packed__)) {
        int32_t left;
        int32_t right;
    } data;
    data.left = Encoder_Counts_Left();
    data.right = Encoder_Counts_Right();
    USB_Send_Msg("cii",'e', &data, sizeof(data));
}

void Send_Loop_Encoder( float _time_since_last )
{
    struct __attribute__((__packed__)) {
        int32_t left;
        int32_t right;
    } data;
    data.left = Encoder_Counts_Left();
    data.right = Encoder_Counts_Right();
    USB_Send_Msg("cii",'E', &data, sizeof(data));
}


void Battery_Filter_Update( float _time_since_last ) 
{
    if (!battery_filter_initialized){
        //float data = Battery_Voltage();

        float pi = 3.14159265358979323846; //doubt need more accuracy...
        float T = 0.002f; //period or measure every x seconds
        float fc = 10; //cutoff frequency
        float a = 2.0f * pi * fc * T;
        float b_coeffs[] = { a / (1.0f + a), 0.0f };
        float a_coeffs[] = { 1.0f, -1.0f / (1.0f + a) };

        Filter_Init(&battery_filter, b_coeffs, a_coeffs, 1);
        Filter_SetTo(&battery_filter, Battery_Voltage());
        battery_filter_initialized = true;
    }
    
    Filter_Value(&battery_filter, Battery_Voltage());
       
}
