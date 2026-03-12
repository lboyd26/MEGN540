#include "Lab3_Tasks.h"

static Filter_Data_t battery_filter;
static bool battery_filter_initalized = false;

//#define BATTERY_B (0.11162f)
//#define BATTERY_A (0.88838f)
//static float _battery_filtered = 0.0f;
void Send_Loop_Battery( float _time_since_last )
{
    //float voltage = Filter_Last_Output( &battery_filter );
    //USB_Send_Msg("cf",'B', &voltage, sizeof(voltage));
    //USB_Send_Msg("cf",'B', &_battery_filtered, sizeof(&_battery_filtered));

    struct __attribute__((__packed__)) {
        float voltage;
    } data;
    data.voltage = Filter_Last_Output(&battery_filter);
    USB_Send_Msg("cf",'B', &data, sizeof(data));
    if(data.voltage < 4.0f){
        struct __attribute__((__packed__)) {
            char let[7];
            float volt;
        } msg = { .let = {'B','A','T',' ','L','O','W'}, .volt = data.voltage };
        USB_Send_Msg("c7sf", '!', &msg, sizeof(msg));

    }
}
void Send_Battery_Now( float _time_since_last )
{

  //float voltage = Filter_Last_Output( &battery_filter );
  struct __attribute__((__packed__)) {
        float voltage;
    } data;
    data.voltage = Filter_Last_Output(&battery_filter);
    USB_Send_Msg("cf",'b', &data, sizeof(data));
  //USB_Send_Msg("cff",'b', &data, sizeof(data));
  // USB_Send_Msg("c",'b', &_battery_filtered, sizeof(_battery_filtered));
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
/*
void Check_Battery_Voltage( float _time_since_last )
{
    float bat = Filter_Last_Output( &battery_filter );
    //float bat = _battery_filtered;
    if( bat < 4.0f ){
        struct __attribute__((__packed__)) {
            char let[7];
            float volt;
        } msg = { .let = {'B','A','T',' ','L','O','W'}, .volt = bat };
        USB_Send_Msg("c7sf", '!', &msg, sizeof(msg));
    }
}
*/

void Battery_Filter_Update( float _time_since_last ) 
{
    float data = Battery_Voltage();

    float pi = 3.14159265358979323846; //doubt need more accuracy...
    float T = 0.002f; //period or measure every x seconds
    float fc = 10; //cutoff frequency
    float a = 2.0f * pi * fc * T;
    float b_coeffs[] = { a / (1.0f + a), 0.0f };
    float a_coeffs[] = { 1.0f, -1.0f / (1.0f + a) };

    Filter_Init(&battery_filter, b_coeffs, a_coeffs, 1);
    Filter_SetTo(&battery_filter, Battery_Voltage());
    battery_filter_initalized = true;
    
    Filter_Value(&battery_filter, data);    

    //_battery_filtered = (BATTERY_B * data) + (BATTERY_A * _battery_filtered);
}
/*
void Battery_Filter_Update( float _time_since_last ) 
{
    float data = Battery_Voltage();
    if (data > 0.5f) {
        float result = Filter_Value(&battery_filter, data);
        if (result != result || result <= 0.0f || result > 15.0f){
            Filter_SetTo(&battery_filter, data);
        }
        //Filter_Value(&battery_filter, data);
    }

    //_battery_filtered = (BATTERY_B * data) + (BATTERY_A * _battery_filtered);
}
*/

/*
void Battery_Filter_Update( float _time_since_last )
{
    if( !battery_filter_initialized ) {
        // 1st-order IIR LPF coefficients: fc=10Hz, fs=500Hz
        float num[2] = { 0.11819f, 0.0f };   // B0, B1
        float den[2] = { 1.0f, -0.88181f };  // A0, A1
        Filter_Init( &battery_filter, num, den, 1 );

        // Pre-load filter
        Filter_SetTo( &battery_filter, Battery_Voltage() );
        battery_filter_initialized = true;
    }

    Filter_Value( &battery_filter, Battery_Voltage() );
}
*/