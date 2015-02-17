/*
 * UART1_ATCommand.h
 *
 *  Created on: 21 Jan 2015
 *      Author: jirawat
 */

#ifndef UART1_ATCOMMAND_H_
#define UART1_ATCOMMAND_H_

//-----------------------------------------------------------------------------
// Define State constant
//-----------------------------------------------------------------------------
#define STATE_IDLE      0
#define STATE_A         1
#define STATE_T         2
#define STATE_COMMAND   3
#define STATE_PARAMETER 4
#define STATE_TIMEOUT   5
//-----------------------------------------------------------------------------
// Define Error constant
//-----------------------------------------------------------------------------
#define ERROR_TIMEOUT                  0
#define ERROR_INVALID_COMMAND          1
#define ERROR_NO_PARAM                 2
#define ERROR_INVALID_PARAM            3
#define ERROR_PARITY                   4
#define ERROR_FULL_FIFO                5

//-----------------------------------------------------------------------------
// Define READY FLAG
//-----------------------------------------------------------------------------
#define READY_READ         0x01
#define READY_WRITE        0x02

//-----------------------------------------------------------------------------
// Define
//-----------------------------------------------------------------------------
#define SIZE_TX_BUFFER     64
#define SIZE_RX_BUFFER     64

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------
void get_error_string (U8 err_code, U8 *buffer, U16 *length);
void send_error (U8 err_code);
void send_string (U16 length, U8 *buffer);
void send_next_char_isr ();
void send_next_char ();
void read_char_isr ();
void run_state_machine ();
void begin_state_machine ();

//-----------------------------------------------------------------------------
// External Global variables
//-----------------------------------------------------------------------------
extern U8 readyUART1;
extern U8 rx_index;
extern U16 Tx_Buffer_Length;
extern U16 Rx_Buffer_Length;
extern SEGMENT_VARIABLE(Tx_Buffer[SIZE_TX_BUFFER], U8, SEG_XDATA);

//-----------------------------------------------------------------------------
// Define macro
//-----------------------------------------------------------------------------
// WRITE_CHAR(b)
//    wait for busy transmit holding register and write the data
//-----------------------------------------------------------------------------
#define WRITE_CHAR(b) while (!(SCON1 & SCON1_THRE__BMASK)); \
                      SBUF1 = b;
#define GET_OK_STR_BUFFER(b) b[0] = 'O'; b[1] = 'K';
#define GET_ERROR_STR_BUFFER(b) b[0] = 'E'; b[1] = 'R'; b[2] = 'R'; \
                               b[3] = 'O'; b[4] = 'R';
#define WAIT_FOR_TX_COMPLETE() while (uart_buffer_size != 0);
#define WRITE_HEX_TO_BUFFER(b, hex) b = hex; \
                                    b = (b > 9) ? (b + 'A' - 10) : b + '0';
#endif /* UART1_ATCOMMAND_H_ */
