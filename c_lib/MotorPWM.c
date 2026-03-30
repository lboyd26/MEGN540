#include MotorPWM.h
#include <avr/io.h>

// Initialize Variables
static int16_t left_pwm = 0;
static int16_t right_pwm = 0;
static uint16_t max_pwm = 0;
static bool pwm_enabled = false;


/**
 * Function Initialize_Motor_PWM initializes the motor PWM on Timer 1 for PWM based voltage control of the motors.
 * The Motor PWM system shall initialize in the disabled state for safety reasons. You should specifically enable
 * Motor PWM outputs only as necessary.
 * @param [uint16_t] MAX_PWM is the maximum PWM value to use. This controls the PWM frequency.
 */
void Initialize_MotorPWM( uint16_t MAX_PWM )
{    
    // Timer Setup
    TCCR1A = 0;
    TCCR1B = 0;

    // Reset Duty Cycles
    OCR1A = 0;
    OCR1B = 0;

    ICR1 = MAX_PWM;

    TCCR1A |= (1<<WGM11);
    TCCR1B |= (1<<WGM12) | (1<<WGM13); // Might want to remove WGM12

    TCCR1B |= (1<<CS10);

    DDRB |= (1<<DDB6); //Left Motor
    DDRB |= (1<<DDB5); //Right Motor
    DDRB |= (1<<DDB1);
    DDRB |= (1<<DDB2);

}

/**
 * Function MotorPWM_Enable enables or disables the motor PWM outputs.
 * @param [bool] enable (true set enable, false set disable)
 */
void MotorPWM_Enable( bool enable )
{
    pwm_enabled = enable;

    if(enable){
        TCCR1A |= (1<<COM1A1) | (1<<COM1B1);
    }
    else{
        TCCR1A &= ~(1<<COM1A1) | (1<<COM1B1);
        //PORTB &= ~((1 << 6) | (1 << 5));
    }
}

/**
 * Function MotorPWM_Is_Enabled returns if the motor PWM is enabled for output.
 * @param [bool] true if enabled, false if disabled
 */
bool MotorPWM_Is_Enabled()
{
    return pwm_enabled;
}

/**
 * Function MotorPWM_Set_Left sets the PWM duty cycle for the left motor.
 * @return [int32_t] The count number.
 */
void MotorPWM_Set_Left( int16_t pwm )
{
    if(Filter_Last_Output(&battery_filter)>4.75f)
    {
        if(!MotorPWM_Is_Enabled())
        {
            MotorPWM_Enable(true);
        }
        if(pwm > max_pwm){
            left_pwm = max_pwm;
        }
        else if(pwm < -max_pwm){
            left_pwm = -max_pwm;
        }
        else{
            left_pwm = pwm;
        }

        uint16_t duty = (left_pwm >=0) ? left_pwm : -left_pwm;
        ORC1A = duty;
    }
}

/**
 * Function MototPWM_Set_Right sets the PWM duty cycle for the right motor.
 * @return [int32_t] The count number.
 */
void MototPWM_Set_Right( int16_t pwm )
{
    if(Filter_Last_Output(&battery_filter)>4.75f)
    {
        if(!MotorPWM_Is_Enabled())
        {
            MotorPWM_Enable(true);
        }
        if(pwm > max_pwm){
            right_pwm = max_pwm;
        }
        else if(pwm < -max_pwm){
            right_pwm = -max_pwm;
        }
        else{
            right_pwm = pwm;
        }

        uint16_t duty = (right_pwm >=0) ? right_pwm : -right_pwm;
        ORC1A = duty;
    }
}

/**
 * Function MotorPWM_Get_Left returns the current PWM duty cycle for the left motor. If disabled it returns what the
 * PWM duty cycle would be.
 * @return [int16_t] duty-cycle for the left motor's pwm
 */
int16_t MotorPWM_Get_Left()
{
    return left_pwm;
}

/**
 * Function MotorPWM_Get_Right returns the current PWM duty cycle for the right motor. If disabled it returns what the
 * PWM duty cycle would be.
 * @return [int16_t] duty-cycle for the right motor's pwm
 */
int16_t MotorPWM_Get_Right()
{
    return right_pwm;
}

/**
 * Function MotorPWM_Get_Max() returns the PWM count that corresponds to 100 percent duty cycle (all on), this is the
 * same as the value written into ICR1 as (TOP).
 */
uint16_t MotorPWM_Get_Max()
{
    return max_pwm;
}

/**
 * Function MotorPWM_Set_Max sets the maximum pwm count. This function sets the timer counts to zero because
 * the ICR1 can cause undesired behaviors if change dynamically below the current counts.  See page 128 of the
 * atmega32U4 datasheat.
 */
void MotorPWM_Set_Max( uint16_t MAX_PWM )
{
    max_pwm = MAX_PWM;
    
    //Reset Timer
    TCNT1 = 0;
    OCR1A = 0;
    OCR1B = 0;

<<<<<<< HEAD
    ICR1 = max_pwm;

    //MototPWM_Set_Left(left_pwm);
    //MototPWM_Set_Right(right_pwm);
=======
    ICR1 = MAX_PWM;
>>>>>>> NorlenL/main
}