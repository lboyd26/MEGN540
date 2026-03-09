#include "Battery_Monitor.h"

static const float BITS_TO_BATTERY_VOLTS = 2.56 * 2.0 / 1023.0;

/**
 * Function Initialize_Battery_Monitor initializes the Battery Monitor to record the current battery voltages.
 */
void Initialize_Battery_Monitor()
{

    // *** MEGN540 LAB3 YOUR CODE HERE ***

    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // set to 125 kHz

    ADMUX |= (1 << REFS1) | (1 << REFS0) | (1 << MUX0) | (1 << MUX1) | (1 << MUX3); // Internal 2.56V Voltage Reference with external capacitor on AREF pin

    ADCSRB |= (1 << MUX5);

    ADCSRA |= (1 << ADEN);

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

    ADCSRA |= (1 << ADSC);          
    while(bit_is_set(ADCSRA, ADSC)); 
    data.value = ADC;

    // *** MEGN540 LAB3 YOUR CODE HERE ***

    return data.value * BITS_TO_BATTERY_VOLTS;
}
