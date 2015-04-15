//-----------------------------------------------------------------------------
// F3xx_USB_Main.c
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
//-----------------------------------------------------------------------------
// Header Files
//-----------------------------------------------------------------------------

#include "c8051f3xx.h"
#include "USB0_Register.h"
#include "USB0_InterruptServiceRoutine.h"
#include "USB0_Descriptor.h"
#include "USBINT1_Main.h"
#include "UART1_ATCommand.h"
#include "SPI0_TI.h"

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

// IN_PACKET and OUT_PACKET buffer bytes immediately before and
// after they are transferred across USB inside the report handlers

SEGMENT_VARIABLE(In_Packet[IN_EP1_PACKET_SIZE], U8, SEG_XDATA);
SEGMENT_VARIABLE(Out_Packet[OUT_EP1_PACKET_SIZE], U8, SEG_XDATA);
SEGMENT_VARIABLE(In3_Packet[IN_EP3_PACKET_SIZE], U8, SEG_XDATA);

// flowFlag and flowFlagC check for debug flow
U8 flowFlag = 0x00;
U8 flowFlagC = 0xFF;

//-----------------------------------------------------------------------------
// Main Routine
//-----------------------------------------------------------------------------
void main(void)
{
   U16 i;
   U32 buffer;

   // Initialize packets to 0
   for (i = 0; i < sizeof (In_Packet); i++)
      In_Packet[i] = 0;
   for (i = 0; i < sizeof (Out_Packet); i++)
      Out_Packet[i] = 0;
   for (i = 0; i < sizeof (In3_Packet); i++)
      In3_Packet[i] = 0;

   System_Init ();                     // Initialize Sysclk, Port IO, Timer2, ADC0
   USB0_Init ();                       // Initialize USB0
   UART1_Init ();                      // Initial UART1
   SPI0_Init();                        // Inital SPI0
   PCA0_Init();

   IE_EA = 1;                             // Enable global interrupts
   IE_EA = 1;

   //Init AFE4490 Board
   AFE4490Init();
   In_Packet[7]++;

   while (1)
   {
      if (readySPI & READY_ADC_RDY) {
         readySPI &= ~READY_ADC_RDY; // clear ADC_RDY flag
         if (countADC_RDY >= 4) {
            //SPI_FLUSH();
            buffer = AFE4490Read(LED2ABSVAL); // read LED2 val
            // store 22bit in Big-endian (MSB in Lowest address)
            In3_Packet[0] = (buffer & 0x3F0000);
            In3_Packet[1] = (buffer & 0x00FF00);
            In3_Packet[2] = (buffer & 0x0000FF);

            buffer = AFE4490Read(LED1ABSVAL); // read LED1 val
            In3_Packet[3] = (buffer & 0x3F0000);
            In3_Packet[4] = (buffer & 0x00FF00);
            In3_Packet[5] = (buffer & 0x0000FF);

            buffer = AFE4490Read(ALED2VAL); // read ambient LED2 val
            buffer += AFE4490Read(ALED1VAL); // read ambient LED1 val
            In_Packet[4] = (buffer & 0x3F0000);
            In_Packet[5] = (buffer & 0x00FF00);
            In_Packet[6] = (buffer & 0x0000FF);
            countADC_RDY = 0;
         }
      }
      if (readyUART1 & READY_READ)
      {
         //P0_B4 ^= 1;
         run_state_machine ();
      }
   }
}
