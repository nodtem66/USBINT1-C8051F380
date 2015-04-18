//-----------------------------------------------------------------------------
// F3xx_USB0_Main.h
//-----------------------------------------------------------------------------
// Copyright 2014 Silicon Laboratories, Inc.
// http://developer.silabs.com/legal/version/v11/Silicon_Labs_Software_License_Agreement.txt
//
// Program Description:
//
// Stub file for Firmware Template.
//
//
// How To Test:    See Readme.txt
//
//
// Target:         C8051F320/1,
//                 C8051F326/7,
//                 C8051F34x,
//                 C8051F38x,
//                 C8051T620/1/6/7, C8051T320/3,
//                 C8051T622/3, C8051T326/7
//
// Tool chain:     Keil C51 7.50 / Keil EVAL C51
//                 Silicon Laboratories IDE version 2.6
// Command Line:   See Readme.txt
// Project Name:   USB0_Interrupt
//
// Release 2.0
//    -Rewrite (CM)
//    -02 NOV 2012
//
// Release 1.1
//    -Changed "Usb_Init" to "USB_Init" (TP)
//    -07 OCT 2010
//
// Release 1.0
//    -Initial Revision (PD)
//    -04 JUN 2008
//

#ifndef _USBTOUART_H_
#define _USBTOUART_H_

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

#define SYSCLOCK           24000000

//-----------------------------------------------------------------------------
// External Global Variables
//-----------------------------------------------------------------------------
extern bit InPacketLock;
extern bit OutPacketLock;
extern bit In3PacketLock;
//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

void System_Init (void);
void USB0_Init (void);
void UART1_Init (void);
void SPI0_Init (void);
void PCA0_Init (void);
void Delay (void);
void DelayForADS (void);

//-----------------------------------------------------------------------------
// Interrupt Prototypes
//-----------------------------------------------------------------------------

INTERRUPT_PROTO (Timer2_ISR, TIMER2_IRQn);
INTERRUPT_PROTO (Timer3_ISR, TIMER3_IRQn);
INTERRUPT_PROTO (ADC0_ConvComplete_ISR, ADC0EOC_IRQn);
INTERRUPT_PROTO (Usb_ISR, USB0_IRQn);
INTERRUPT_PROTO (UART1_ISR, UART1_IRQn);
INTERRUPT_PROTO (SPI0_ISR, SPI0_IRQn);
INTERRUPT_PROTO (PCA0_ISR, PCA0_IRQn);

// Lock define macros
#define LOCK(l) while(l); l = 1;
#define UNLOCK(l) l = 0;
#endif // _USB_DESC_H_
