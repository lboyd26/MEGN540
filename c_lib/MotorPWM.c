#include "MotorPWM.h"

void Initialize_MotorPWM( uint16_t MAX_PWM )
{

    //PRR0 &= ~(1 << PRTIM1); //7.9.2 disable Power Reduction

    //Disable Timer1, 14.10
    TCCR1A = 0;
    TCCR1B = 0;

    //Reset Duty Cycles
    OCR1A = 0;
    OCR1B = 0;

    //Set Max PWM
    ICR1 = MAX_PWM; //14.10.15

    //no-prescaler, set Mode 10 PWM Phase Correct
    TCCR1B |= (1 << WGM13) | (1 << CS10); //Table 14-4 and Table 14-5
    TCCR1A |= (1 << WGM11); //Table 14-4

    //Set Output Pins 1 and 2, 10.4.3
    //set PB6 DDB6 to 1 for OC1B output, PB5 DDB5 to 1 for output, 10.3.1
    DDRB |= (1 << DDB5) | (1 << DDB6) | (1 << DDB1) | (1 << DDB2);

}

void MotorPWM_Enable( bool enable )
{
    if (enable)
    {
        //Enable Motors, Table 14-3
        TCCR1A |= (1 << COM1A1) | (1 << COM1B1);
    }
    else
    {
        TCCR1A &= ~((1 << COM1A1) | (1 << COM1B1));
    }
}

bool MotorPWM_Is_Enabled()
{
    uint8_t mask = (1 << COM1A1) | (1 << COM1B1);
    return (TCCR1A & mask) == mask; //see if motor bits match
}

void MotorPWM_Set_Left( int16_t pwm )
{
    if (Filter_Last_Output(&battery_filter) > 4.75f)
    {
        if (!MotorPWM_Is_Enabled())
        {
            MotorPWM_Enable(true);
        }
        if (pwm < 0)
        {
            //Reverse
            PORTB |= (1 << PB2); //TI Table 7-2
            OCR1B = (uint16_t)(-pwm);
        }
        else
        {
            //Forward
            PORTB &= ~(1 << PB2);
            OCR1B = (uint16_t)(pwm);
        }
    }
    else
    {
        if (MotorPWM_Is_Enabled())
        {
            MotorPWM_Enable(false);
        }
    }
}

void MotorPWM_Set_Right( int16_t pwm )
{
    if (Filter_Last_Output(&battery_filter) > 4.75f)
    {
        if (!MotorPWM_Is_Enabled())
        {
            MotorPWM_Enable(true);
        }
        if (pwm < 0)
        {
            //Reverse
            PORTB |= (1 << PB1); //TI Table 7-2
            OCR1A = (uint16_t)(-pwm);
        }
        else
        {
            //Forward
            PORTB &= ~(1 << PB1);
            OCR1A = (uint16_t)(pwm);
        }
    }
    else
    {
        if (MotorPWM_Is_Enabled())
        {
            MotorPWM_Enable(false);
        }
    }
}

int16_t MotorPWM_Get_Left()
{
    if (PORTB & (1 << PB2))
    {
        // reverse
        return -(int16_t)OCR1B;
    }
    else
    {
        return (int16_t)OCR1B;
    }
}

int16_t MotorPWM_Get_Right()
{
    if (PORTB & (1 << PB1))
    {
        return -(int16_t)OCR1A;
    }
    else
    {
        return (int16_t)OCR1A;
    }
}

uint16_t MotorPWM_Get_Max()
{
    return ICR1;
}

void MotorPWM_Set_Max(uint16_t MAX_PWM)
{
    // Reset timer count and duty cycles to avoid issues when changing TOP dynamically (p.128)
    TCNT1 = 0;
    OCR1A = 0;
    OCR1B = 0;

    ICR1 = MAX_PWM;
}