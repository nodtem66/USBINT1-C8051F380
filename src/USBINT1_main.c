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
#include "SPI0_TI.h"

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

// IN_PACKET and OUT_PACKET buffer bytes immediately before and
// after they are transferred across USB inside the report handlers
SEGMENT_VARIABLE(In_Packet[IN_EP1_PACKET_SIZE], U8, SEG_XDATA);
SEGMENT_VARIABLE(Out_Packet[OUT_EP1_PACKET_SIZE], U8, SEG_XDATA);
SEGMENT_VARIABLE(In3_Packet[IN_EP3_PACKET_SIZE], U8, SEG_XDATA);

// atomic lock
volatile bit InPacketLock = 0;
volatile bit OutPacketLock = 0;
volatile bit In3PacketLock = 0;
SEGMENT_VARIABLE(ADS_Data_Out[ADS_DATA_OUT_LENGTH], U8, SEG_XDATA);
//-----------------------------------------------------------------------------
// Main Routine
//-----------------------------------------------------------------------------
void main(void)
{
   U16 i;
   U32 buffer = 0;

   // Initialize packets to 0
   for (i = 0; i < sizeof (In_Packet); i++)
      In_Packet[i] = 0;
   for (i = 0; i < sizeof (Out_Packet); i++)
      Out_Packet[i] = 0;
   for (i = 0; i < sizeof (In3_Packet); i++)
      In3_Packet[i] = 0;

   System_Init ();                     // Initialize Sysclk, Port IO, Timer2, ADC0
   USB0_Init ();                       // Initialize USB0
   SPI0_Init();                        // Inital SPI0
   PCA0_Init();

   IE_EA = 1;                          // Enable global interrupts
   IE_EA = 1;

   Delay();

   // wait at least 0.128 sec for ADS initialisation (ads1298 pp. 42)
   // wait 9us (18t_clk) for reset pulse
   ADSDelay(10);
   ADSCommand(ADS_CMD_SDATAC);
   Delay();
   ADSInit();                          // Init ADS1298 board
   In_Packet[4] = ADSRead(ADS_ID);
   Delay();
   ADSCommand(ADS_CMD_START);
   ADSCommand(ADS_CMD_RDATAC);
   Delay();
   In_Packet[7]++;
   while (1)
   {
      //ADSWrite(ADS_CONFIG1, ADS_CONFIG1__HIGH_RES_1K_SPS);
      //In_Packet[4] = ADSRead(ADS_ID);
      if (readySPI & READY_ADC_RDY)
      {
         // Start Read 27 Bytes from ADS1298
         LOCK(In3PacketLock)
         SPI_START()

         // Read 24bit Status register in Big-endian (MSB in Lowest Address)
         READ_SPI(ADS_Data_Out[0]);
         READ_SPI(ADS_Data_Out[1]);
         READ_SPI(ADS_Data_Out[2]);
         // Read channel 1
         READ_SPI(ADS_Data_Out[3]);
         READ_SPI(ADS_Data_Out[4]);
         READ_SPI(ADS_Data_Out[5]);
         // Read channel 2
         READ_SPI(ADS_Data_Out[6]);
         READ_SPI(ADS_Data_Out[7]);
         READ_SPI(ADS_Data_Out[8]);
         // Read channel 3
         READ_SPI(ADS_Data_Out[9]);
         READ_SPI(ADS_Data_Out[10]);
         READ_SPI(ADS_Data_Out[11]);
         // Read channel 4
         READ_SPI(ADS_Data_Out[12]);
         READ_SPI(ADS_Data_Out[13]);
         READ_SPI(ADS_Data_Out[14]);
         // Read channel 5
         READ_SPI(ADS_Data_Out[15]);
         READ_SPI(ADS_Data_Out[16]);
         READ_SPI(ADS_Data_Out[17]);
         // Read channel 6
         READ_SPI(ADS_Data_Out[18]);
         READ_SPI(ADS_Data_Out[19]);
         READ_SPI(ADS_Data_Out[20]);
         // Read channel 7
         READ_SPI(ADS_Data_Out[21]);
         READ_SPI(ADS_Data_Out[22]);
         READ_SPI(ADS_Data_Out[23]);
         // Read channel 8
         READ_SPI(ADS_Data_Out[24]);
         READ_SPI(ADS_Data_Out[25]);
         READ_SPI(ADS_Data_Out[26]);
         SPI_END()

         // End Read 27 Bytes from ADS1298
         UNLOCK(In3PacketLock)
         readySPI &= ~READY_ADC_RDY;
      }
   }
}
