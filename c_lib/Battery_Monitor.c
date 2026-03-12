#include "Battery_Monitor.h"

static const float BITS_TO_BATTERY_VOLTS = 5.0 * 2.0 / 1023.0;

/**
 * Function Initialize_Battery_Monitor initializes the Battery Monitor to record the current battery voltages.
 */

 
void Initialize_Battery_Monitor()
{
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0) | (1 << ADEN);
    /*
    // *** MEGN540 LAB3 YOUR CODE HERE ***

    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // set to 125 kHz
    //ADCSRA |= (1 << ADPS2) | (1 << ADPS1);
    //ADMUX &= ~(1<<MUX0);
    //ADMUX &= ~(1<<MUX3);
    ADMUX =  (1 << REFS0) | (1 << MUX1) | (1 << MUX2); // Internal 2.56V Voltage Reference with external capacitor on AREF pin
    //ADMUX = (1 << REFS0) | (1 << MUX0);
    //ADMUX &= ~(1<<REFS1);
    //ADMUX &= ~(1<<ADLAR);
    
    //missing DIDR0, check MUX, REFS1
    ADCSRB &= ~(1 << MUX5);

    DDRF &= ~(1 << PF6);
    PORTF &= ~(1 << PF6);

    //DIDR0 &= ~(1<<ADC0D);
    //DIDR0 &= ~(1<<ADC6D);
    DIDR0 |= (1 << ADC6D);

    ADCSRA |= (1 << ADEN);

    ADCSRA |= (1 << ADSC);          
    while(bit_is_set(ADCSRA, ADSC)) {}
    (void)ADCL;
    (void)ADCH;
    */
}

/**
 * Function Battery_Voltage initiates the A/D measurement and returns the result for the battery voltage.
 */

float Battery_Voltage()
{
    ADCSRB = 0;
    ADMUX =  (1 << REFS0) | (1 << MUX1) | (1 << MUX2);
    // A Union to assist with reading the LSB and MSB in the  16 bit register
    union {
        struct {
            uint8_t LSB;
            uint8_t MSB;
        } split;
        uint16_t value;
    } data = { .value = 0 };

    ADCSRA |= (1 << ADSC);          
    while(bit_is_set(ADCSRA, ADSC)) {}
    //data.value = ADC;
    data.split.LSB = ADCL;
    data.split.MSB = ADCH;

    // *** MEGN540 LAB3 YOUR CODE HERE ***
    return data.value * BITS_TO_BATTERY_VOLTS;
}







#include "Battery_Monitor.h"

// Battery voltage is divided by 2 before ADC6.
// ADC is 10-bit (0-1023), AVCC reference = 5V.
// Actual battery voltage = ADC_value * (AVCC / 1024) * 2
static const float BITS_TO_BATTERY_VOLTS = 2.0f * 5.0f / 1024.0f;

/**
 * Function Initialize_Battery_Monitor initializes the Battery Monitor to record the current battery voltages.
 * Configures the ADC for single-ended reading on ADC6 (PF6) with AVCC reference and prescaler of 128.
 * At 16MHz: 16MHz / 128 = 125kHz ADC clock (within the required 50-200kHz range per datasheet sec. 24.4).
 */
void Initialize_Battery_Monitor()
{
    // Set PF6 (ADC6) as input
    DDRF  &= ~( 1 << DDF6 );
    PORTF &= ~( 1 << PORTF6 );

    // Disable digital input buffer on PF6/ADC6
    DIDR0 |= ( 1 << ADC6D );

    ADMUX = ( 1 << REFS0 ) | ( 6 & 0x0F );
    ADCSRB &= ~( 1 << MUX5 );

    // ADCSRA: enable ADC (ADEN=1), prescaler 128 (ADPS2:0 = 111)
    ADCSRA = ( 1 << ADEN ) | ( 1 << ADPS2 ) | ( 1 << ADPS1 ) | ( 1 << ADPS0 );

    ADCSRA |= ( 1 << ADSC );
    while( ADCSRA & ( 1 << ADSC ) );
}

/**
 * Function Battery_Voltage initiates the A/D measurement and returns the result for the battery voltage.
 */
float Battery_Voltage()
{
    // A Union to assist with reading the LSB and MSB in the  16 bit register
    union {
        struct {
            uint8_t LSB;
            uint8_t MSB;
        } split;
        uint16_t value;
    } data = { .value = 0 };

    ADCSRA |= ( 1 << ADSC );

    while( ADCSRA & ( 1 << ADSC ) );

    data.split.LSB = ADCL;
    data.split.MSB = ADCH;

    return data.value * BITS_TO_BATTERY_VOLTS;
}
