#include "Lab4_Tasks.h"

// set the pwm signals on both motors
void Set_PWM(int16_t pwm_left, int16_t pwm_right)
{
    MotorPWM_Set_Left(pwm_left);
    MotorPWM_Set_Left(pwm_right);
}

void Stop_PWM_Delay(float _time_since_last)
{
    Set_PWM(0,0);
    Task_Cancel(&task_stop_pwm);
}

void Stop_PWM(float _time_since_last)
{
    if(MotorPWM_Is_Enabled())
    {
        MotorPWM_Enable(0);
    }
}

void Send_PWM_ID(float _time_since_last)
{
    struct __attribute__((packed))
    {
        float time;
        int16_t pwm_left;
        int16_t pwm_right;
        int16_t encoder_left;
        int16_t encoder_right;

    } data;

    data.pwm_left = MotorPWM_Get_Left();
    data.pwm_right = MotorPWM_Get_Right();
    data.encoder_left = Encoder_Counts_Left();
    data.encoder_right = Encoder_Counts_Right();

    USB_Send_Msg("cii",'q', &data, sizeof(data));
}

void Send_PWM_ID_Loop(float _time_since_last)
{
    struct __attribute__((packed))
    {
        float time;
        int16_t pwm_left;
        int16_t pwm_right;
        int16_t encoder_left;
        int16_t encoder_right;

    } data;

    data.pwm_left = MotorPWM_Get_Left();
    data.pwm_right = MotorPWM_Get_Right();
    data.encoder_left = Encoder_Counts_Left();
    data.encoder_right = Encoder_Counts_Right();

    USB_Send_Msg("cii",'Q', &data, sizeof(data));
}