/*
         MEGN540 Mechatronics Lab
    Copyright (C) Andrew Petruska, 2021.
       apetruska [at] mines [dot] edu
          www.mechanical.mines.edu
*/

/*
    Copyright (c) 2021 Andrew Petruska at Colorado School of Mines

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.

*/

// put your includes here for all modules you need to use
#include "Lab1_Tasks.h"
#include "Lab2_Tasks.h"
#include "Lab3_Tasks.h"


#include "Message_Handling.h"
#include "SerialIO.h"
#include "Task_Management.h"
#include "Timing.h"
#include "Battery_Monitor.h"
#include "Encoder.h"

// put your task includes and/or function declarations here for future populaion

// put your global variables (only if absolutely required) here.
// Best to identify them as "static" to make them indentified as internal and start with a "_" to identify as internal.
// Try to initialize them if possible, so their values are never arbitrary.

// put your initialization function here

void Initialize_Modules( float _time_not_used_ )
{
    // Initialize (reinitialize) all global variables
    
    float T = 0.002f;
    
    //float T = 0.002; //period or measure every x seconds

    // reset USB input buffers
    USB_Flush_Input_Buffer();

    // Initialize all modules except USB (it can only be called once without messing things up)
    Initialize_Timing();

    Initialize_Encoders();
    Initialize_Battery_Monitor();

    //float num[2] = { 0.11819f, 0.0f };   // B0, B1
    //float den[2] = { 1.0f, -0.88181f };  // A0, A1
    //Filter_Init( &battery_filter, num, den, 1 );

    // Pre-load filter
    //Filter_SetTo( &battery_filter, Battery_Voltage() );
    //battery_filter_initialized = true;


    // Setup task handling 
    Initialize_Task( &task_restart, Initialize_Modules /*function pointer to call*/ );

    // Setup message handling to get processed at some desired rate.
    Initialize_Task( &task_message_handling, Task_Message_Handling );
    Task_Activate(&task_message_handling, 0);

    // Initialize_Task( &task_message_handling_watchdog, /*watchdog timout period*/,  Task_Message_Handling_Watchdog );
    Initialize_Task(&task_time_loop, Send_Loop_Time);
    Initialize_Task(&task_send_time, Send_Time_Now);
    Initialize_Task(&task_message_handling_watchdog, Task_Message_Handling_Watchdog);

    
    //LAB 3 stuff
    Initialize_Task(&task_send_encoder_now, Send_Encoder_Now);
    Initialize_Task(&task_send_encoder_loop, Send_Loop_Encoder);
    Initialize_Task(&task_send_battery_now, Send_Battery_Now);
    Initialize_Task(&task_send_battery_loop, Send_Loop_Battery);
    //Initialize_Task(&task_battery_status, Check_Battery_Voltage);
    //Task_Activate(&task_battery_status, 1.0f); //see if needs charging every 1 second forever

    Initialize_Task(&task_battery_filter, Battery_Filter_Update);
    Task_Activate(&task_battery_filter, T);
}


/** Main program entry point. This routine configures the hardware required by the application, then
 *  enters a loop to run the application tasks in sequence.
 */
int main( void )
{

    // call initialization stuff
    Initialize_USB();
    Initialize_Modules( 0.0 );
    for( ;; ) {
        // main loop logic
        Task_USB_Upkeep();

        Task_Run_If_Ready( &task_message_handling );
        Task_Run_If_Ready( &task_restart );

        Task_Run_If_Ready( &task_send_time );
        Task_Run_If_Ready( &task_time_loop );
        Task_Run_If_Ready( &task_message_handling_watchdog );

        //Task_USB_Upkeep();
        Task_Run_If_Ready( &task_send_encoder_now );
        Task_Run_If_Ready( &task_send_encoder_loop );

        //Task_USB_Upkeep();
        Task_Run_If_Ready( &task_send_battery_now );
        Task_Run_If_Ready( &task_send_battery_loop );
        Task_Run_If_Ready( &task_battery_filter );

        //Task_USB_Upkeep();


    }
    return 0;
}

// put your task function definitions here
