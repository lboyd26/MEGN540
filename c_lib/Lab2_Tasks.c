
// /**
//  * Return the time it requested followed by the time to complete the action specified by the second imput char. 
//  * 0x00 -> Time Now 
//  * 0x01 -> Time to complete a full loop iteration
//  */
// void Fetch_and_Send_little_t(char time_now, char time_loop)
// {
//     // Get time Now (in Seconds)
//     float time_sec = Timing_Get_Time_Sec();


//     // send response right here if appropriate.
//     USB_Send_Msg( "cf", 't', &ret_val, sizeof( ret_val ) );
// }

// /**
//  * Return the time it requested followed by the time to complete the action specified by the second imput char and 
//  * returns the time every X milliseconds. If the time is zero or negative it canceles the request.
//  */
// void Fetch_and_Send_big_T(char time_now, char time_loop, float everyX) {

// }


// Put your lab-specific task functionality and data_structures (if necessary) here so it is accessable to both
// message handeling and the Lab main loops.
void Send_Loop_Time( float _time_since_last ) {

}
void Send_Time_Now( float _time_since_last ){

}