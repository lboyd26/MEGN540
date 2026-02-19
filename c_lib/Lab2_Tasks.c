
/**
 * Return the time it requested followed by the time to complete the action specified by the second imput char. 
 * 0x00 -> Time Now 
 * 0x01 -> Time to complete a full loop iteration
 */
void Fetch_and_Send_little_t(char time_now, char time_loop)
{
    // Get time Now (in Seconds)
    float time_sec = Timing_Get_Time_Sec();


    // send response right here if appropriate.
    USB_Send_Msg( "cf", 't', &ret_val, sizeof( ret_val ) );
}

/**
 * Return the time it requested followed by the time to complete the action specified by the second imput char and 
 * returns the time every X milliseconds. If the time is zero or negative it canceles the request.
 */
void Fetch_and_Send_big_T(char time_now, char time_loop, float everyX) {

}
