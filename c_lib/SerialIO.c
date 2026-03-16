#include "SerialIO.h"

// Internal Ring Buffer Objects
static Ring_Buffer_Byte_t _usb_receive_buffer;
static Ring_Buffer_Byte_t _usb_send_buffer;

/**
 * (non-blocking) Internal function _USB_Read_Data takes the next USB byte and reads it
 * into a ring buffer for latter processing.
 */
static void _USB_Read_Data()
{
    if( USB_DeviceState != DEVICE_STATE_Configured )
        return;

    Endpoint_SelectEndpoint( CDC_RX_EPADDR );

    if( Endpoint_IsOUTReceived() ) {
        while( Endpoint_BytesInEndpoint() > 0 ) {
            uint8_t byte = Endpoint_Read_8();
            rb_push_back_B( &_usb_receive_buffer, byte );
        }

        Endpoint_ClearOUT();
    }
}

/**
 * (non-blocking) Internal function _USB_Write_Data takes the next byte from the output
 * ringbuffer and writes it to the USB port (if free).
 */
static void _USB_Write_Data()
{
    if( USB_DeviceState != DEVICE_STATE_Configured ) {
        return;
    }

    Endpoint_SelectEndpoint( CDC_TX_EPADDR );

    if( Endpoint_IsINReady() && rb_length_B( &_usb_send_buffer ) > 0 ) {
        while( rb_length_B( &_usb_send_buffer ) > 0 ) {
            uint8_t byte = rb_pop_front_B( &_usb_send_buffer );
            Endpoint_Write_8( byte );
        }

        Endpoint_ClearIN();
    }
}

void Task_USB_Upkeep()
{
    USB_USBTask();  // lufa internal upkeep task

    // *** MEGN540  ***
    // Each iteration you should send what you have in the buffer
    // and get what is there to get from the buffer.
    // this is done by leveraging your _USB_Read_Data and _USB_Write_Data functions
    cli();
    _USB_Read_Data();
    _USB_Write_Data();
    sei();
}

/** Function to manage CDC data transmission and reception to and from the host for the second CDC interface, which echoes back
 *  all data sent to it from the host.
 */
void Task_USB_Echo( void )
{
    /* Device must be connected and configured for the task to run */
    if( USB_DeviceState != DEVICE_STATE_Configured )
        return;

    /* Select the Serial Rx Endpoint */
    Endpoint_SelectEndpoint( CDC_RX_EPADDR );

    /* Check to see if any data has been received */
    if( Endpoint_IsOUTReceived() ) {
        /* Create a temp buffer big enough to hold the incoming endpoint packet */
        uint8_t Buffer[Endpoint_BytesInEndpoint()];

        /* Remember how large the incoming packet is */
        uint16_t DataLength = Endpoint_BytesInEndpoint();

        /* Read in the incoming packet into the buffer */
        Endpoint_Read_Stream_LE( &Buffer, DataLength, NULL );

        /* Finalize the stream transfer to send the last packet */
        Endpoint_ClearOUT();

        /* Select the Serial Tx Endpoint */
        Endpoint_SelectEndpoint( CDC_TX_EPADDR );

        /* Write the received data to the endpoint */
        Endpoint_Write_Stream_LE( &Buffer, DataLength, NULL );

        /* Finalize the stream transfer to send the last packet */
        Endpoint_ClearIN();

        /* Wait until the endpoint is ready for the next packet */
        Endpoint_WaitUntilReady();

        /* Send an empty packet to prevent host buffering */
        Endpoint_ClearIN();
    }

    // ************** MEGN540 FOR DEBUGGING ***************** //
    // once you get your _USB_Read_Data and _USB_Write_Data to work with your ring buffers
    // you can comment out the above example and reproduce the echo functionality with either
    // of the below
    //
    // if( rb_length_B( &_usb_receive_buffer ) != 0 )
    //     rb_push_back_B( &_usb_send_buffer, rb_pop_front_B( &_usb_receive_buffer ) );
    //
    // if( usb_msg_length() != 0 )
    //    usb_send_byte(usb_msg_get());
    //
}

/**
 * (non-blocking) Function USB_Send_Byte Adds a character to the output buffer
 * @param byte [uint8_t] Data to send
 */
void USB_Send_Byte( uint8_t byte )
{
    rb_push_back_B( &_usb_send_buffer, byte );
}

/**
 * (non-blocking) Function USB_Send_Data adds the data buffer to the output ring buffer.
 * @param p_data [void*] pointer to the data-object to be sent
 * @param data_len [uint8_t] size of data-object to be sent
 */
void USB_Send_Data( void* p_data, uint8_t data_len )
{
    uint8_t* bytes = (uint8_t*)p_data;

    for( uint8_t i = 0; i < data_len; i++ ) {
        rb_push_back_B( &_usb_send_buffer, bytes[i] );
    }
}

/**
 * (non-blocking) Function USB_Send_Str adds a c-style (null terminated) string to the output buffer
 * @param p_str [char*] Pointer to a c-string (null terminated) to send
 */
void USB_Send_Str( char* p_str )
{
    while( *p_str != '\0' ) {
        rb_push_back_B( &_usb_send_buffer, (uint8_t)*p_str );
        p_str++;
    }
    rb_push_back_B( &_usb_send_buffer, '\0' );
}

/**
 * (non-blocking) Function USB_Send_Msg sends a message according to the MEGN540 USB message format.
 *      [MSG Length] [Format C-Str][Host Initiating CMD Char][DATA]
 *      MSG Length: [uint8_t] Number of bytes to follow in full message.
 *              Length of: Format C-String + 1 (CMD Char) + Length of DATA Array
 *      Format C-Str: [char*] null-terminated c-string defining the interpertation of the data bytes.
 *              Valid characters are defined in the Python3 struct defintion.
 *              Some examples are: c (char), b (int8_t), B (uint8_t), h (int16_t), f (float32)
 *      Host Initiating CMD Char: [char] The command sent from the host this message is in response to.
 *           All transmissions are host initiate.
 *      DATA: [Byte Array] Data byes that make up the message to be sent.
 *
 * @param format [c-str pointer] Pointer to interpertation string. e.g. ccf.  This alwasy starts with c because of the
 *          CMD char, here teh DATA object is then a char and a float.
 * @param cmd [char] Command this message is in respose to.
 * @param p_data [void*] pointer to the data-object to send.
 * @param data_len [uint8_t] size of the data-object to send. Remember sizeof() can help you with this!
 */
void USB_Send_Msg( char* format, char cmd, void* p_data, uint8_t data_len )
{
    uint8_t format_length = 0;
    while( format[format_length] != '\0' ) {
        format_length++;
    }
    format_length++;

    uint8_t msg_length = format_length + 1 + data_len;

    USB_Send_Byte( msg_length );

    USB_Send_Str( format );

    USB_Send_Byte( (uint8_t)cmd );

    USB_Send_Data( p_data, data_len );
}

/**
 * (non-blocking) Funtion USB_Msg_Length returns the number of bytes in the receive buffer awaiting processing.
 * @return [uint8_t] Number of bytes ready for processing.
 */
uint8_t USB_Msg_Length()
{
    return rb_length_B( &_usb_receive_buffer );
}

/**
 * (non-blocking) Function USB_Send_Buffer_Empty returns true if the send buffer is empty.
 * @return [bool] True if send buffer is empty, false otherwise.
 */
bool USB_Send_Buffer_Empty()
{
    return rb_length_B( &_usb_send_buffer ) == 0;
}

/**
 * (non-blocking) Function USB_Msg_Peek returns (without removal) the next byte in teh receive buffer (null if empty).
 * @return [uint8_t] Next Byte
 */
uint8_t USB_Msg_Peek()
{
    if( rb_length_B( &_usb_receive_buffer ) == 0 ) {
        return 0;
    }
    return rb_get_B( &_usb_receive_buffer, 0 );
}

/**
 * (non-blocking) Function usb_msg_get removes and returns the next byte in the receive buffer (null if empty)
 * @return [uint8_t] Next Byte
 */
uint8_t USB_Msg_Get()
{
    if( rb_length_B( &_usb_receive_buffer ) == 0 ) {
        return 0;
    }
    return rb_pop_front_B( &_usb_receive_buffer );
}

/**
 * (non-blocking) Function usb_msg_read_into populates the object with the data in the recieve buffer and
 * removes the bytes as they are read.  Returns false if receive buffer does not contain enough bytes to
 * fill the container and terminates without reading or removing.
 *
 * @param buff
 * @param data_len
 * @return [bool]  True: sucess, False: not enough bytes available
 */
bool USB_Msg_Read_Into( void* p_obj, uint8_t data_len )
{
    if( rb_length_B( &_usb_receive_buffer ) < data_len ) {
        return false;
    }

    uint8_t* byte_ptr = (uint8_t*)p_obj;

    for( uint8_t i = 0; i < data_len; i++ ) {
        byte_ptr[i] = rb_pop_front_B( &_usb_receive_buffer );
    }

    return true;
}

/**
 * (non-blocking) Function USB_Flush_Input_Buffer sets the length of the recieve buffer to zero and disreguards
 * any bytes that remaining.
 */
void USB_Flush_Input_Buffer()
{
    rb_initialize_B( &_usb_receive_buffer );
}

/** Configures the board hardware and chip peripherals for the demo's functionality. */
void Initialize_USB( void )
{

    // *** MEGN540 ***//
    // We need to initialize the ring buffers here.
    rb_initialize_B( &_usb_receive_buffer );
    rb_initialize_B( &_usb_send_buffer );

    // THE following is LUFA specific setup to make sure the
    // watchdog timer is not active as we are not actively resetting it
    // It then goes and initializes the USB hardware redgesters and turns
    // on interrupt handeling (since lufa uses interrupts).

#if ( ARCH == ARCH_AVR8 )
    /* Disable watchdog if enabled by bootloader/fuses */
    MCUSR &= ~( 1 << WDRF );
    wdt_disable();

    /* Disable clock division */
    clock_prescale_set( clock_div_1 );
#endif

    /* Hardware Initialization */
    USB_Init();

    /* Enable Interupts for LUFA to respond to host requests */
    GlobalInterruptEnable();
}

// ************* LUFA SPECIFIC HELPING FUNCTIONS BELOW ********************* //

/** Contains the current baud rate and other settings of the first virtual serial port. While this demo does not use
 *  the physical USART and thus does not use these settings, they must still be retained and returned to the host
 *  upon request or the host will assume the device is non-functional.
 *
 *  These values are set by the host via a class-specific request, however they are not required to be used accurately.
 *  It is possible to completely ignore these value or use other settings as the host is completely unaware of the physical
 *  serial link characteristics and instead sends and receives data in endpoint streams.
 */
static CDC_LineEncoding_t LineEncoding1 = { .BaudRateBPS = 0, .CharFormat = CDC_LINEENCODING_OneStopBit, .ParityType = CDC_PARITY_None, .DataBits = 8 };

/** Event handler for the USB_Connect event. This indicates that the device is enumerating via the status LEDs and
 *  starts the library USB task to begin the enumeration and USB management process.
 */
void EVENT_USB_Device_Connect( void )
{
    /* Indicate USB enumerating */

    // Future LED MSG could be good here
}

/** Event handler for the USB_Disconnect event. This indicates that the device is no longer connected to a host via
 *  the status LEDs and stops the USB management and CDC management tasks.
 */
void EVENT_USB_Device_Disconnect( void )
{
    /* Indicate USB not ready */

    // Future LED MSG could be good here
}

/** Event handler for the USB_ConfigurationChanged event. This is fired when the host set the current configuration
 *  of the USB device after enumeration - the device endpoints are configured and the CDC management tasks are started.
 */
void EVENT_USB_Device_ConfigurationChanged( void )
{
    bool ConfigSuccess = true;

    /* Setup first CDC Interface's Endpoints */
    ConfigSuccess &= Endpoint_ConfigureEndpoint( CDC_TX_EPADDR, EP_TYPE_BULK, CDC_TXRX_EPSIZE, 1 );
    ConfigSuccess &= Endpoint_ConfigureEndpoint( CDC_RX_EPADDR, EP_TYPE_BULK, CDC_TXRX_EPSIZE, 1 );
    ConfigSuccess &= Endpoint_ConfigureEndpoint( CDC_NOTIFICATION_EPADDR, EP_TYPE_INTERRUPT, CDC_NOTIFICATION_EPSIZE, 1 );

    /* Reset line encoding baud rates so that the host knows to send new values */
    LineEncoding1.BaudRateBPS = 0;
}

/** Event handler for the USB_ControlRequest event. This is used to catch and process control requests sent to
 *  the device from the USB host before passing along unhandled control requests to the library for processing
 *  internally.
 */
void EVENT_USB_Device_ControlRequest( void )
{
    /* Determine which interface's Line Coding data is being set from the wIndex parameter */
    void* LineEncodingData = &LineEncoding1;

    /* Process CDC specific control requests */
    switch( USB_ControlRequest.bRequest ) {
        case CDC_REQ_GetLineEncoding:
            if( USB_ControlRequest.bmRequestType == ( REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE ) ) {
                Endpoint_ClearSETUP();

                /* Write the line coding data to the control endpoint */
                Endpoint_Write_Control_Stream_LE( LineEncodingData, sizeof( CDC_LineEncoding_t ) );
                Endpoint_ClearOUT();
            }

            break;
        case CDC_REQ_SetLineEncoding:
            if( USB_ControlRequest.bmRequestType == ( REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE ) ) {
                Endpoint_ClearSETUP();

                /* Read the line coding data in from the host into the global struct */
                Endpoint_Read_Control_Stream_LE( LineEncodingData, sizeof( CDC_LineEncoding_t ) );
                Endpoint_ClearIN();
            }

            break;
        case CDC_REQ_SetControlLineState:
            if( USB_ControlRequest.bmRequestType == ( REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE ) ) {
                Endpoint_ClearSETUP();
                Endpoint_ClearStatusStage();
            }

            break;
    }
}
