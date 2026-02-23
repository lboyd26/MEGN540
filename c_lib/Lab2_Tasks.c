#include "Lab2_Tasks.h"
#include "Message_Handling.h"

// Put your lab-specific tasks here
// e.g. Task_t task_restart;  ///<-- Lab 1: This flag indicates that the device received a restart command from the hoast. Default inactive.
Task_t task_time_loop;                  ///<-- Lab 2: Indicates if the system should report time to complete a loop.
Task_t task_send_time;                  ///<-- Lab 2: Indicates if the system should send the current time.
Task_t task_message_handling_watchdog;  ///<-- LAB 2: Watchdog Timer for Flusing USB Input Messaging if incomplete message received.

void init()
{
    Initialize_Task(&task_time_loop, Send_Loop_Time); 
    Initialize_Task(&task_send_time, Send_Time_Now); 
    Initialize_Task(&task_message_handling_watchdog, Task_Message_Handling_Watchdog); 
}

// // /**
// //  * Return the time it requested followed by the time to complete the action specified by the second imput char. 
// //  * 0x00 -> Time Now 
// //  * 0x01 -> Time to complete a full loop iteration
// //  */
// void Fetch_and_Send_little_t(char time_now, char time_loop)
// {
//     // // Get time Now (in Seconds)
//     // float time_sec = Timing_Get_Time_Sec();


//     // // send response right here if appropriate.
//     // USB_Send_Msg( "cf", 't', &ret_val, sizeof( ret_val ) );

//     if(time_now)
//     {
//         // Send Current time once 
//         Send_Time_Now(0); 
//     }

//     if(time_loop)
//     {
//         // Activate task_time_loop member for a single run (-1)
//         Task_Activate(&task_time_loop, -1)
//     }
// }


// /**
//  * Return the time it requested followed by the time to complete the action specified by the second imput char and 
//  * returns the time every X milliseconds. If the time is zero or negative it canceles the request.
//  */
// void Fetch_and_Send_big_T(char time_now, char time_loop, float everyX) {
//     if(everyX <= 0)
//     {
//         // Cancel both tasks if the inverval is 0 or negative
//         Task_Cancel(&task_send_time); 
//         Task_Cancel(&task_time_loop); 
//         return; 
//     }
//     // convert to sec
//     float interval = everyX / 1000.0

//     if(time_now)
//     {
//         // repeat send time task every X in sec
//         Task_Activate(&task_send_time, interval)
//     }

//     if(time_loop)
//     {
//         // repeat loop task every X in sec
//         Task_Activate(&task_time_loop, interval)
//     }

// }


// Put your lab-specific task functionality and data_structures (if necessary) here so it is accessable to both
// message handeling and the Lab main loops.
void Send_Loop_Time( float _time_since_last ) {

    // _time_since_last already contains how long the 
    // last loop took
    _time_since_last /= 1000; // Convert to seconds

    USB_Send_Msg("cf", 't', &_time_since_last, sizeof(_time_since_last)); 

}
void Send_Time_Now( float _time_since_last ){
    // Get current time in sec
    float timeNow = Timing_Get_Time_Sec();

    // Send current time over USB
    USB_Send_Msg("cf", 't', &timeNow, sizeof(timeNow)); 
}