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

#include "Lab1_Tasks.h"
#include "Lab2_Tasks.h"
#include "Lab3_Tasks.h"
#include "Lab4_Tasks.h"


#include "Message_Handling.h"
#include "SerialIO.h"
#include "Task_Management.h"
#include "Timing.h"
#include "Battery_Monitor.h"
#include "Encoder.h"
#include "MotorPWM.h"

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

    //tune this value
    Initialize_MotorPWM(500);


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

    Initialize_Task(&task_battery_filter, Battery_Filter_Update);
    Task_Activate(&task_battery_filter, T);

    // LAB 4 stuff
    Initialize_Task(&task_stop_pwm, Stop_PWM);
    Initialize_Task(&task_send_pwm_id_loop, Send_PWM_ID);
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

        Task_Run_If_Ready( &task_send_encoder_now );
        Task_Run_If_Ready( &task_send_encoder_loop );

        Task_Run_If_Ready( &task_battery_filter );
        Task_Run_If_Ready( &task_send_battery_now );
        Task_Run_If_Ready( &task_send_battery_loop );

        Task_Run_If_Ready(&task_stop_pwm);
        Task_Run_If_Ready(&task_send_pwm_id_loop);



    }
    return 0;
}
