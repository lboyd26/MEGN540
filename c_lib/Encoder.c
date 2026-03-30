#include "Encoder.h"
#include <math.h>

/**
 * Internal counters for the Interrupts to increment or decrement as necessary.
 */
static volatile bool _last_right_A = 0;  // Static limits it's use to this file
static volatile bool _last_right_B = 0;  // Static limits it's use to this file

static volatile bool _last_left_A   = 0;  // Static limits it's use to this file
static volatile bool _last_left_B   = 0;  // Static limits it's use to this file
static volatile bool _last_left_XOR = 0;  // Necessary to check if PB4 triggered the ISR or not

static volatile int32_t _left_counts  = 0;  // Static limits it's use to this file
static volatile int32_t _right_counts = 0;  // Static limits it's use to this file

/** Helper Funcions for Accessing Bit Information */
// *** MEGN540 Lab 3 TODO ***
// Hint, use avr's bit_is_set function to help
static inline bool Right_XOR()
{
    return bit_is_set(PINE, 6);
}
static inline bool Right_B()
{
    return bit_is_set(PINF, 0);
}
static inline bool Right_A()
{
    return _last_right_A; //Really confused about this
}

static inline bool Left_XOR()
{
    return bit_is_set(PINB, 4);
}
static inline bool Left_B()
{
    return bit_is_set(PINE, 2);
}
static inline bool Left_A()
{
    return _last_left_A; //Really confused about this
}

/**
 * Function Encoders_Init initializes the encoders, sets up the pin change interrupts, and zeros the initial encoder
 * counts.
 */
void Initialize_Encoders()
{
    // *** MEGN540 Lab3 ***
    // YOUR CODE HERE

    // Left encoder uses PB4 and PE2 pins as digital inputs. External interrupt PCINT4 is necessary to detect
    // the change in XOR flag. You'll need to see Section 11.1.5 - 11.1.7 for setup and use.
    // Note that the PCINT interrupt is trigered by any PCINT pin. In the ISR you should check to make sure
    // the interrupt triggered is the one you intend on processing.

    // Right encoder uses PE6 adn PF0 as digital inputs. External interrupt INT6 is necessary to detect
    // the changes in XOR flag. You'll need to see Sections 11.1.2-11.1.4 for setup and use.
    // You'll use the INT6_vect ISR flag.

    // Initialize static file variables. These probably need to be updated.

    EIMSK &= ~(1 << INT6); //clear EIMSK INT 6
    EICRB &= ~(1 << ISC61); // clear EICRB ICS61
    EICRB |= (1 << ISC60); // set EICRB ISC60

   

    DDRB &= ~(1<<4); // Left XOR

    DDRE &= ~(1<<2); // Left Channel B
    DDRE &= ~(1<<6); // Right XOR

    DDRF &= ~(1<<0); // Right Channel B


    PORTE |= (1 << PE2) | (1 << PE6); 
    PORTB |= (1 << PB4);
    PORTF |= (1 << PF0);

    PCIFR |= (1 << PCIF0); // PCIFR TODO
    // Enable Interupts
    PCICR |= (1 << PCIE0);
    PCMSK0 |= (1 << PCINT4);

    EIFR = 0; // clear EIFR
    EIMSK |= (1 << INT6);




    _last_right_B = Right_B();
    _last_right_A = _last_right_B ^ Right_XOR();

    _last_left_B   = Left_B();
    _last_left_A   = _last_left_B ^ Left_XOR();
    _last_left_XOR = Left_XOR();

    _left_counts  = 0;
    _right_counts = 0;

    sei();
}

/**
 * Function Encoder_Counts_Left returns the number of counts from the left encoder.
 * @return [int32_t] The count number.
 */
int32_t Encoder_Counts_Left()
{
    // *** MEGN540 Lab3 ***
    // YOUR CODE HERE
    // Note: Interrupts can trigger during a function call and an int32 requires
    // multiple clock cycles to read/save. You may want to stop interrupts, copy the value,
    // and re-enable interrupts to prevent this from corrupting your read/write.
    int32_t counts = 0; //Figure out how to acutally access the counts

    cli();

    counts = _left_counts;

    sei();

    return counts;
}

/**
 * Function Encoder_Counts_Right returns the number of counts from the right encoder.
 * @return [int32_t] The count number.
 */
int32_t Encoder_Counts_Right()
{
    // *** MEGN540 Lab3 ***
    // YOUR CODE HERE
    // Note: Interrupts can trigger during a function call and an int32 requires
    // multiple clock cycles to read/save. You may want to stop interrupts, copy the value,
    // and re-enable interrupts to prevent this from corrupting your read/write.

    int32_t counts = 0; //Figure out how to acutally access the counts
    cli();

    counts = _right_counts;

    sei();

    return counts;
}

/**
 * Function Encoder_Rad_Left returns the number of radians for the left encoder.
 * @return [float] Encoder angle in radians
 */
float Encoder_Rad_Left()
{
    // *** MEGN540 Lab3 ***
    // YOUR CODE HERE.  How many counts per rotation???
    float counts_per_rev = 12.0f;
    float rad = ((float)Encoder_Counts_Left() / counts_per_rev) * (2.0f * M_PI);
    return rad;
}

/**
 * Function Encoder_Rad_Right returns the number of radians for the left encoder.
 * @return [float] Encoder angle in radians
 */
float Encoder_Rad_Right()
{
    // *** MEGN540 Lab3 ***
    // YOUR CODE HERE.  How many counts per rotation???
    float counts_per_rev = 12.0f;
    float rad = ((float)Encoder_Counts_Right() / counts_per_rev) * (2.0f * M_PI);
    return rad;
}

/**
 * Interrupt Service Routine for the left Encoder. Note: May need to check that it is actually PCINT4 that triggered, as
 * the Pin Change Interrupts can trigger for multiple pins.
 * @return
 */
ISR(PCINT0_vect)
{
    if(Left_XOR() != _last_left_XOR) {
        bool A = Left_A();
        bool B = Left_B();

        int8_t increment = (B != _last_left_B) ? ((B ^ A) ? 1 : -1) : 0;
        _left_counts += increment;

        _last_left_B = B;
        _last_left_A = B ^ Left_XOR();
        _last_left_XOR = Left_XOR();
    }
}

/**
 * Interrupt Service Routine for the right Encoder.
 * @return
 */
ISR(INT6_vect)
{
    bool A = Right_A();
    bool B = Right_B();

    int8_t increment = (B != _last_right_B) ? ((B ^ A) ? 1 : -1) : 0;
    _right_counts += increment;

    _last_right_B = B;
    _last_right_A = B ^ Right_XOR();
}