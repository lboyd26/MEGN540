#include "Lab2_Tasks.h"

void Send_Loop_Time( float _time_since_last )
{
    USB_Send_Msg("cf",'t', &_time_since_last, sizeof(_time_since_last));
}
void Send_Time_Now( float _time_since_last )
{
    float time_now = Timing_Get_Time_Sec();
    USB_Send_Msg("cf",'t', &time_now, sizeof(time_now));
}