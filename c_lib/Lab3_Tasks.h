/*
         MEGN540 Mechatronics Lab
    Copyright (C) Andrew Petruska, 2021.
       apetruska [at] mines [dot] edu
          www.mechanical.mines.edu
*/

/*
    Copyright (c) 2021 Andrew Petruska at Colorado School of Mines

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to
   deal in the Software without restriction, including without limitation the
   rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
   sell copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
   IN THE SOFTWARE.

*/

/*
 * This file defines lab-spcific tasks for execution
 */

#ifndef LAB3_TASKS_H
#define LAB3_TASKS_H

// Include your lab-specific headers here
// e.g. #include "SerialIO.h"  // necessary for sending arithmatic results back to user
#include "SerialIO.h"
#include "Battery_Monitor.h"
#include "Encoder.h"
#include "Filter.h"
#include "Task_Management.h"


// Put your lab-specific tasks here
// e.g. Task_t task_restart;  ///<-- Lab 1: This flag indicates that the device received a restart command from the hoast. Default inactive.
Task_t task_send_encoder_now;
Task_t task_send_encoder_loop;            
Task_t task_send_battery_now;
Task_t task_send_battery_loop;
//Task_t task_battery_status;
Task_t task_battery_filter;

// Put your lab-specific task functionality and data_structures (if necessary) here so it is accessable to both
// message handeling and the Lab main loops.
// e.g. void Send_Time_Now( float _time_since_last );
void Send_Encoder_Now( float _time_since_last );
void Send_Loop_Encoder( float _time_since_last );
void Send_Loop_Battery( float _time_since_last );
void Send_Battery_Now( float _time_since_last );
void Battery_Filter_Update( float _time_since_last );


#endif  // ifndef LAB3_TASKS_H