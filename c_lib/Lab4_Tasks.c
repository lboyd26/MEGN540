#include "Lab4_Tasks.h"

//configure PWM on motors
void Set_PWM( int16_t left, int16_t right )
{
    MotorPWM_Set_Left(left);
    MotorPWM_Set_Right(right);
}

void Stop_PWM_Delay()
{
    Set_PWM(0, 0);
    Task_Cancel(&task_stop_pwm);
}

void Schedule_PWM_Stop(float time)
{
    Task_Activate(&task_stop_pwm, time);
}

void Stop_PWM()
{
    if (MotorPWM_Is_Enabled())
        {
            MotorPWM_Enable(false);
        }
}

void Send_Identification()
{
    struct __attribute__((__packed__)) {
        float time;
        int16_t PWM_left;
        int16_t PWM_right;
        int16_t E_left;
        int16_t E_right;
    } data;
    data.time = Timing_Get_Time_Sec();
    data.PWM_left = MotorPWM_Get_Left();
    data.PWM_right = MotorPWM_Get_Right();
    data.E_left = Encoder_Counts_Left();
    data.E_right = Encoder_Counts_Right();
    USB_Send_Msg("cf4h",'q', &data, sizeof(data));
}