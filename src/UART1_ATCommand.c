/*
 * UART1_ATCommand.c
 *
 *  Created on: 21 Jan 2015
 *      Author: jirawat
 */

#include "c8051f3xx.h"
#include "USBINT1_Main.h"
#include "UART1_ATCommand.h"
#include "USB0_InterruptServiceRoutine.h"


//-----------------------------------------------------------------------------
// global variables
//-----------------------------------------------------------------------------
U16 uart_buffer_size = 0;  // DataSize for transfer data
U16 uart_buffer_index = 0; // current index of tx buffer
U8 rx_index = 0;           // current index of rx buffer
U8 *uart_buffer_ptr;       // pointer to outgoing-data Buffer
U8 state = 0;              // current state

//-----------------------------------------------------------------------------
// External global variables
//-----------------------------------------------------------------------------
U8 readByte;       // reading buffer
U8 readyUART1 = 0; // flag about UART1

U16 Tx_Buffer_Length = 0;
U16 Rx_Buffer_Length = 0;
SEGMENT_VARIABLE(Tx_Buffer[SIZE_TX_BUFFER], U8, SEG_XDATA);
SEGMENT_VARIABLE(Rx_Buffer[SIZE_RX_BUFFER], U8, SEG_XDATA);
//-----------------------------------------------------------------------------
// send_string
//-----------------------------------------------------------------------------
//
// send the next byte from data buffer
//
// Param:
//    int length: length of buffer for sending
//    uint8* buffer: pointer to buffer
//-----------------------------------------------------------------------------
void send_string (U16 length, U8 *buffer)
{
   while (SCON1 & SCON1_RI__SET); // wait until rx complete
   while (uart_buffer_size > 0); // wait for current transmition

   //append EOF to buffer
   //buffer[length]     = '\r';
   //buffer[length + 1] = '\n';
   if (length > 1)
   {
      buffer[length] = '\n';
      uart_buffer_size = length + 1;
   }
   else
   {
      uart_buffer_size = length;
   }
   //setting the buffer
   uart_buffer_ptr = buffer;
   uart_buffer_index = 0;

   send_next_char(); // sent the buffer to tx
}

//-----------------------------------------------------------------------------
// send_next_char
//-----------------------------------------------------------------------------
//
// send the next byte from data buffer
//
//-----------------------------------------------------------------------------
void send_next_char_isr ()
{
   if ( uart_buffer_size > 0 )
   {
      uart_buffer_size--;
      WRITE_CHAR(uart_buffer_ptr[uart_buffer_index++]);
   }
}
void send_next_char()
{
   if ( uart_buffer_size > 0 )
   {
      uart_buffer_size--;
      WRITE_CHAR(uart_buffer_ptr[uart_buffer_index++]);
   }
}

//-----------------------------------------------------------------------------
// get_error_string
//-----------------------------------------------------------------------------
//
// generate string from error code
//
// Params:
//    int err_code: error code to convert to string
//    int* buffer: pointer of return string
//    int* length: pointer of length of return string
//
//-----------------------------------------------------------------------------
void get_error_string (U8 err_code, U8 *buffer, U16* length)
{
   switch (err_code)
   {
   case ERROR_TIMEOUT:
      buffer[0] = 'E';
      buffer[1] = 'R';
      buffer[2] = 'R';
      buffer[3] = 'O';
      buffer[4] = 'R';
      buffer[5] = ' ';
      buffer[6] = 'T';
      buffer[7] = 'I';
      buffer[8] = 'M';
      buffer[9] = 'E';
      buffer[10] = 'O';
      buffer[11] = 'U';
      buffer[12] = 'T';
      *length = 13;
      break;
   case ERROR_INVALID_COMMAND:
      buffer[0] = 'E';
      buffer[1] = 'R';
      buffer[2] = 'R';
      buffer[3] = 'O';
      buffer[4] = 'R';
      buffer[5] = ' ';
      buffer[6] = 'I';
      buffer[7] = 'N';
      buffer[8] = 'V';
      buffer[9] = 'A';
      buffer[10] = 'L';
      buffer[11] = 'I';
      buffer[12] = 'D';
      buffer[13] = ' ';
      buffer[14] = 'C';
      buffer[15] = 'O';
      buffer[16] = 'M';
      buffer[17] = 'M';
      buffer[18] = 'A';
      buffer[19] = 'N';
      buffer[20] = 'D';
      *length = 21;
      break;
   case ERROR_INVALID_PARAM:
      buffer[0] = 'E';
      buffer[1] = 'R';
      buffer[2] = 'R';
      buffer[3] = 'O';
      buffer[4] = 'R';
      buffer[5] = ' ';
      buffer[6] = 'I';
      buffer[7] = 'N';
      buffer[8] = 'V';
      buffer[9] = 'A';
      buffer[10] = 'L';
      buffer[11] = 'I';
      buffer[12] = 'D';
      buffer[13] = ' ';
      buffer[14] = 'P';
      buffer[15] = 'A';
      buffer[16] = 'R';
      buffer[17] = 'A';
      buffer[18] = 'M';
      *length = 19;
      break;
   case ERROR_NO_PARAM:
      buffer[0] = 'E';
      buffer[1] = 'R';
      buffer[2] = 'R';
      buffer[3] = 'O';
      buffer[4] = 'R';
      buffer[5] = ' ';
      buffer[6] = 'N';
      buffer[7] = 'O';
      buffer[8] = ' ';
      buffer[9] = 'P';
      buffer[10] = 'A';
      buffer[11] = 'R';
      buffer[12] = 'A';
      buffer[13] = 'M';
      *length = 14;
      break;
   case ERROR_PARITY:
      buffer[0] = 'E';
      buffer[1] = 'R';
      buffer[2] = 'R';
      buffer[3] = 'O';
      buffer[4] = 'R';
      buffer[5] = ' ';
      buffer[6] = 'P';
      buffer[7] = 'A';
      buffer[8] = 'R';
      buffer[9] = 'I';
      buffer[10] = 'T';
      buffer[11] = 'Y';
      *length = 12;
      break;
   case ERROR_FULL_FIFO:
      buffer[0] = 'E';
      buffer[1] = 'R';
      buffer[2] = 'R';
      buffer[3] = 'O';
      buffer[4] = 'R';
      buffer[5] = ' ';
      buffer[6] = 'F';
      buffer[7] = 'U';
      buffer[8] = 'L';
      buffer[9] = 'L';
      buffer[10] = ' ';
      buffer[11] = 'F';
      buffer[12] = 'I';
      buffer[13] = 'F';
      buffer[14] = 'O';
      *length = 15;
      break;
   default:
      buffer[0] = 0;
      *length = 0;
   }
}

//-----------------------------------------------------------------------------
// send_error
//-----------------------------------------------------------------------------
//
// send error message to tx
//
// Param:
//     int err_code: error code
//-----------------------------------------------------------------------------
void send_error (U8 err_code)
{
   WAIT_FOR_TX_COMPLETE();
   get_error_string(err_code, Tx_Buffer, &Tx_Buffer_Length);
   send_string(Tx_Buffer_Length,Tx_Buffer);
}

//-----------------------------------------------------------------------------
// read_char
//-----------------------------------------------------------------------------
//
// read the current byte from data buffer
//
//-----------------------------------------------------------------------------
void read_char_isr ()
{
   readByte = SBUF1; // receive RX from FIFO
   SCON1 &= ~SCON1_RI__SET; // clear rx flag

   P0_B5 ^= 1;
   TMR3 = TMR3RL;    // reload new timer3; delay timeout 10 msec

   if (readByte == '\r' || readByte == '\n')
   {
      P0_B4 ^= 1;
      begin_state_machine();
   }
   else
   {
      Rx_Buffer[Rx_Buffer_Length++] = readByte;
      WAIT_FOR_TX_COMPLETE();
      Tx_Buffer[0] = readByte;
      send_string(1, Tx_Buffer);
   }
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// STATE MACHINE for AT COMMAND API
//    - IDLE
//    - A-STATE
//    - T-STATE
//    - COMMAND-STATE
//    - PARAM-STATE
// Variables:
//    - int state
//    - int[] Rx_Buffer
//    - int Rx_Buffer_Length
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Begin State Machine
//-----------------------------------------------------------------------------
//
// prepare the environmental variables to start state machine
// run before Run_State_Machine
//
//-----------------------------------------------------------------------------
void begin_state_machine ()
{
   readyUART1 |= READY_READ;      // start parsing AT Command
   rx_index = 0;                  // set index to the first offset in buffer
   state = STATE_IDLE;            // start with STATE_IDLE
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void run_state_machine ()
{
   if(rx_index < Rx_Buffer_Length)
   {
      if (Rx_Buffer[rx_index] == '\n' || Rx_Buffer[rx_index] == '\r')
      {
         state = STATE_IDLE;
      }
      else
      {
         switch (state)
         {
         case STATE_IDLE:
            if (Rx_Buffer[rx_index] == 'A')
               state = STATE_A;
            break;
         case STATE_A:
            if (Rx_Buffer[rx_index] == 'T')
            {
               if (rx_index + 1 >= Rx_Buffer_Length || Rx_Buffer[rx_index + 1] == '\r' || Rx_Buffer[rx_index + 1] == '\n')
               {
                  WAIT_FOR_TX_COMPLETE();
                  GET_OK_STR_BUFFER(Tx_Buffer);
                  send_string(2, Tx_Buffer);
                  state = STATE_IDLE;
               }
               else
               {
                  state = STATE_T;
               }
            }
            else
            {
               send_error(ERROR_INVALID_COMMAND);
            }
            break;
         case STATE_T:
            if (Rx_Buffer[rx_index] == 'V')
            {
               WAIT_FOR_TX_COMPLETE();
               GET_OK_STR_BUFFER(Tx_Buffer);
               Tx_Buffer[2] = '1';
               Tx_Buffer[3] = '.';
               Tx_Buffer[4] = '0';
               Tx_Buffer[5] = '0';
               send_string(6, Tx_Buffer);
               state = STATE_IDLE;
            }
            else if (Rx_Buffer[rx_index] == 'F')
            {
               WAIT_FOR_TX_COMPLETE();
               GET_OK_STR_BUFFER(Tx_Buffer);
               //WRITE_HEX_TO_BUFFER(Tx_Buffer[2], flowFlag >> 4);
               //WRITE_HEX_TO_BUFFER(Tx_Buffer[3], flowFlag & 0x0F);
               //WRITE_HEX_TO_BUFFER(Tx_Buffer[4], flowFlagC >> 4);
               //WRITE_HEX_TO_BUFFER(Tx_Buffer[5], flowFlagC & 0x0F);
               send_string(2, Tx_Buffer);
               state = STATE_IDLE;
            }
            else if (rx_index + 3 < Rx_Buffer_Length && Rx_Buffer[rx_index] == 'E' && Rx_Buffer[rx_index + 1] == 'P')
            {
               U8 endpoint = Rx_Buffer[rx_index + 2] - '0';
               if (Rx_Buffer[rx_index + 3] == '?')
               {
                  if (endpoint >= 0 && endpoint <= 3)
                  {
                     WAIT_FOR_TX_COMPLETE();
                     GET_OK_STR_BUFFER(Tx_Buffer);
                     Tx_Buffer[2] = EP_Status[endpoint] + '0';
                     send_string(3, Tx_Buffer);
                  }
                  state = STATE_IDLE;
               }
               else if (Rx_Buffer[rx_index + 3] == '=')
               {
                  if (endpoint >= 0 && endpoint <= 3)
                  {
                     EP_Status[endpoint] = (Rx_Buffer[rx_index + 4] - '0');
                     WAIT_FOR_TX_COMPLETE();
                     GET_OK_STR_BUFFER(Tx_Buffer);
                  send_string(2, Tx_Buffer);
                  }
                  state = STATE_IDLE;
               }
               else
               {
                  state = STATE_IDLE;
                  send_error(ERROR_INVALID_COMMAND);
               }
            }
            else
            {
               state = STATE_IDLE;
               send_error(ERROR_INVALID_COMMAND);
            }
            break;
         case STATE_COMMAND:
            break;
         case STATE_PARAMETER:
            break;
         default:
            break;
         }
      }
      rx_index++;
   }
   else
   {
      readyUART1 &= ~READY_READ;
      Rx_Buffer_Length = 0;
   }
}
