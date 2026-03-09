#include "Lab3_Tasks.h"

void Send_Loop_Battery( float _time_since_last )
{
    float voltage = Filter_Last_Output( &battery_filter );
    USB_Send_Msg("cf",'B', &voltage, sizeof(voltage));
}
void Send_Battery_Now( float _time_since_last )
{
    float voltage = Filter_Last_Output( &battery_filter );
    USB_Send_Msg("c",'b', &voltage, sizeof(voltage));
}

void Send_Encoder_Now( float _time_since_last )
{
    struct __attribute__((__packed__)) {
        int32_t left;
        int32_t right;
    } data;
    data.left = Encoder_Counts_Left();
    data.right = Encoder_Counts_Right();
    USB_Send_Msg("c",'e', &data, sizeof(data));
}

void Send_Loop_Encoder( float _time_since_last )
{
    struct __attribute__((__packed__)) {
        int32_t left;
        int32_t right;
    } data;
    data.left = Encoder_Counts_Left();
    data.right = Encoder_Counts_Right();
    USB_Send_Msg("cf",'E', &data, sizeof(data));
}

void Check_Battery_Voltage( float _time_since_last )
{
    float bat = Filter_Last_Output( &battery_filter );
    if( bat < 4.0f ){
        struct __attribute__((__packed__)) {
            char let[7];
            float volt;
        } msg = { .let = {'B','A','T',' ','L','O','W'}, .volt = bat };
        USB_Send_Msg("c7sf", '!', &msg, sizeof(msg));
    }
}

void Battery_Filter_Update( float _time_since_last ) 
{
    float data = Battery_Voltage();
    Filter_Value(&battery_filter, data);
}