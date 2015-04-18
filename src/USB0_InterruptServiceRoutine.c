//-----------------------------------------------------------------------------
// F3xx_USB0_InterruptServiceRoutine.c
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
// Release 1.0
//    -Initial Revision (PD)
//    -04 JUN 2008
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include "c8051f3xx.h"
#include "USB0_Register.h"
#include "USB0_InterruptServiceRoutine.h"
#include "USB0_Descriptor.h"
#include "USBINT1_Main.h"

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

U8 USB0_State;              // Holds the current USB State

Setup_Buffer Setup;         // Buffer for current device
                            // request information

U16 DataSize;               // Size of data to return
U16 DataSent;               // Amount of data sent so far
U8 const * DataPtr;         // Pointer to data to return

// Holds the status for each endpoint:
// EP0, IN_EP1, OUT_EP1, IN_EP3
U8 EP_Status[4] = {EP_IDLE, EP_HALT, EP_HALT, EP_IDLE};

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

void Usb_Resume (void);                // Resumes USB operation
void Usb_Reset (void);                 // Called after USB bus reset
void Handle_Control (void);            // Handle Setup packet on EP 0
void Handle_In1 (void);                // Handle in packet on EP 1
void Handle_Out1 (void);               // Handle out packet on EP 1
void Handle_In3 (void);                // Handle in packet on EP3

void Fifo_Read (U8, U16, U8*);         // Used for multiple byte reads
                                       // of Endpoint fifos
void Fifo_Write_InterruptServiceRoutine (U8, U16, U8 const *);
                                       // Used for multiple byte
                                       // writes of Endpoint fifos

//-----------------------------------------------------------------------------
// Usb_ISR
//-----------------------------------------------------------------------------
//
// Called after any USB type interrupt, this handler determines which type
// of interrupt occurred, and calls the specific routine to handle it.
//
// Top-level USB ISR
//
//-----------------------------------------------------------------------------
INTERRUPT (Usb_ISR, USB0_IRQn)
{
   U8 bCommon, bIn, bOut;

   POLL_READ_BYTE (CMINT, bCommon); // Read all interrupt registers
   POLL_READ_BYTE (IN1INT, bIn);    // this read also clears the register
   POLL_READ_BYTE (OUT1INT, bOut);

   if (bCommon & rbRSUINT)          // Handle Resume interrupt
   {
      Usb_Resume ();
   }
   if (bCommon & rbRSTINT)          // Handle Reset interrupt
   {
      Usb_Reset ();
   }
   if (bIn & rbEP0)                 // Handle Setup packet received
   {                                // or packet transmitted if Endpoint 0
      Handle_Control();             // is in transmit mode
   }
   if (bIn & rbIN1)                 // Handle In Packet sent, put new data
   {                                // on endpoint 1 fifo
      Handle_In1 ();
   }
   if (bIn & rbIN3)                 // Handle In packet Endpoint 3 sent, put
   {                                // new data on endpoint 3 fifo
      //flowFlag++;
      Handle_In3 ();
      //flowFlagC--;
   }
   if (bOut & rbOUT1)               // Handle Out packet received, take
   {                                // data off endpoint 1 fifo
      Handle_Out1 ();
   }
   if (bCommon & rbSUSINT)          // Handle Suspend interrupt
   {
      USB0_Suspend ();
   }
   if (bCommon & rbSOF)             // Handle SOF interrupt
   {
      //Send_Packet_ISR();
   }
}

//-----------------------------------------------------------------------------
// Support Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Usb_Reset
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// - Set state to default
// - Clear Usb Inhibit bit
//
//-----------------------------------------------------------------------------
void Usb_Reset (void)
{
   USB0_State = DEV_DEFAULT;           // Set device state to default

   POLL_WRITE_BYTE (POWER, 0x01);      // Clear usb inhibit bit to enable USB
                                       // suspend detection

   EP_Status[0] = EP_IDLE;             // Set default Endpoint Status
   EP_Status[1] = EP_HALT;
   EP_Status[2] = EP_HALT;
   EP_Status[3] = EP_HALT;
}

//-----------------------------------------------------------------------------
// Usb_Resume
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Resume normal USB operation
//
//-----------------------------------------------------------------------------
void Usb_Resume(void)
{
   volatile int k = 0;
   k++;
}

//-----------------------------------------------------------------------------
// Handle_Control
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// - Decode Incoming Setup requests
// - Load data packets on fifo while in transmit mode
//
//-----------------------------------------------------------------------------
void Handle_Control (void)
{
   U8 controlReg;                      // Temporary storage for EP control
                                       // register

   POLL_WRITE_BYTE (INDEX, 0);         // Set Index to Endpoint Zero
   POLL_READ_BYTE (E0CSR, controlReg); // Read control register

   if (EP_Status[0] == EP_ADDRESS)     // Handle Status Phase of Set Address
                                       // command
   {
      POLL_WRITE_BYTE (FADDR, Setup.wValue.U8[LSB]);
      EP_Status[0] = EP_IDLE;
   }

   if (controlReg & rbSTSTL)           // If last packet was a sent stall,
   {                                   // reset STSTL bit and return EP0
                                       // to idle state
      POLL_WRITE_BYTE (E0CSR, 0);
      EP_Status[0] = EP_IDLE;
      return;
   }

   if (controlReg & rbSUEND)           // If last Setup transaction was
   {                                   // ended prematurely then set
      POLL_WRITE_BYTE (E0CSR, rbDATAEND);
      // Serviced Setup End bit and return EP0
      POLL_WRITE_BYTE (E0CSR, rbSSUEND);
      EP_Status[0] = EP_IDLE;          // To idle state
   }

   // If Endpoint 0 is in idle mode
   if (EP_Status[0] == EP_IDLE)
   {
      // Make sure that EP 0 has an Out Packet
      // ready from host although if EP0
      // is idle, this should always be the
      // case
      if (controlReg & rbOPRDY)
      {
         // Get Setup Packet off of Fifo,
         // it is currently Big-Endian
         Fifo_Read (FIFO_EP0, 8, (U8*) &Setup);

// If using a big-endian compiler
#if (MSB == 0)
         // Compiler Specific - these next three
         // statements swap the bytes of the
         // Setup packet words to Big Endian so
         // they can be compared to other 16-bit
         // values elsewhere properly
         Setup.wValue.U16 = Setup.wValue.U8[MSB] + (Setup.wValue.U8[LSB] << 8);
         Setup.wIndex.U16 = Setup.wIndex.U8[MSB] + (Setup.wIndex.U8[LSB] << 8);
         Setup.wLength.U16 = Setup.wLength.U8[MSB] + (Setup.wLength.U8[LSB] << 8);
#endif // (MSB == 0)

         switch (Setup.bRequest)       // Call correct subroutine to handle
         {                             // each kind of standard request
            case GET_STATUS:
               Get_Status ();
               break;
            case CLEAR_FEATURE:
               Clear_Feature ();
               break;
            case SET_FEATURE:
               Set_Feature ();
               break;
            case SET_ADDRESS:
               Set_Address ();
               break;
            case GET_DESCRIPTOR:
               Get_Descriptor ();
               break;
            case GET_CONFIGURATION:
               Get_Configuration ();
               break;
            case SET_CONFIGURATION:
               Set_Configuration ();
               break;
            case GET_INTERFACE:
               Get_Interface ();
               break;
            case SET_INTERFACE:
               Set_Interface ();
               break;
            default:
               Force_Stall ();         // Send stall to host if invalid request
               break;
         }
      }
   }

   if (EP_Status[0] == EP_TX)          // See if endpoint should transmit
   {
      if (!(controlReg & rbINPRDY) )   // Don't overwrite last packet
      {
         // Read control register
         POLL_READ_BYTE (E0CSR, controlReg);

         // Check to see if Setup End or Out Packet received, if so do not put
         // any new data on FIFO
         if ((!(controlReg & rbSUEND)) || (!(controlReg & rbOPRDY)))
         {
            // Add In Packet ready flag to E0CSR bitmask
            controlReg = rbINPRDY;

            if (DataSize >= EP0_PACKET_SIZE)
            {
               // Break Data into multiple packets if larger than Max Packet
               Fifo_Write_InterruptServiceRoutine (FIFO_EP0, EP0_PACKET_SIZE,
               									         (U8*)DataPtr);
               // Advance data pointer
               DataPtr  += EP0_PACKET_SIZE;
               // Decrement data size
               DataSize -= EP0_PACKET_SIZE;
               // Increment data sent counter
               DataSent += EP0_PACKET_SIZE;
            }
            else
            {
               // If data is less than Max Packet size or zero
               Fifo_Write_InterruptServiceRoutine (FIFO_EP0, DataSize,
                                                   (U8*)DataPtr);
               controlReg |= rbDATAEND;// Add Data End bit to bitmask
               EP_Status[0] = EP_IDLE; // Return EP 0 to idle state
            }
            if (DataSent == Setup.wLength.U16)
            {
               // This case exists when the host requests an even multiple of
               // your endpoint zero max packet size, and you need to exit
               // transmit mode without sending a zero length packet
               controlReg |= rbDATAEND;// Add Data End bit to mask
               EP_Status[0] = EP_IDLE; // Return EP 0 to idle state
            }
            // Write mask to E0CSR
            POLL_WRITE_BYTE(E0CSR, controlReg);
         }
      }
   }
}

//-----------------------------------------------------------------------------
// Handle_In1
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This routine loads the current value from In_Packet on the Endpoint 1 fifo,
// after  an interrupt is received from the last packet being transmitted
//
//-----------------------------------------------------------------------------
void Handle_In1 ()
{
   U8 controlReg;

   POLL_WRITE_BYTE(INDEX, 1);           // Set index to endpoint 1 registers
   POLL_READ_BYTE(EINCSR1, controlReg); // Read contol register for EP 1

   // If endpoint is currently halted, send a stall
   if (EP_Status[1] == EP_HALT)
   {
      POLL_WRITE_BYTE(EINCSR1, rbInSDSTL);
   }
   // Otherwise send last updated
   else
   {
      // Clear sent stall if last packet returned a stall
      if (controlReg & rbInSTSTL)
      {
         POLL_WRITE_BYTE(EINCSR1, rbInCLRDT);
      }

      // Put new data on Fifo
      Fifo_Write_InterruptServiceRoutine(FIFO_EP1, IN_EP1_PACKET_SIZE, In_Packet);
      
      // Set In Packet ready bit, indicating 
      POLL_WRITE_BYTE(EINCSR1, rbInINPRDY);
   }
}

//-----------------------------------------------------------------------------
// Handle_In3
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This routine loads the current value from In_Packet on the Endpoint 3 fifo,
// after  an interrupt is received from the last packet being transmitted
//
//-----------------------------------------------------------------------------
void Handle_In3 ()
{
   U8 controlReg;
   POLL_WRITE_BYTE(INDEX, 3);           // Select index to endpoint 3 register
   POLL_READ_BYTE(EINCSRL, controlReg); // Read control register for EP3

   // If endpoint is currently halted, send a stall
   if (EP_Status[3] == EP_HALT)
   {
      POLL_WRITE_BYTE(EINCSRL, rbInSDSTL);
   }
   else
   {
      // Clear sent stall if last packet returned a stall
      if (controlReg & rbInSTSTL)
      {
         POLL_WRITE_BYTE(EINCSR1, rbInCLRDT);
      }
   }

   Send_Packet_ISR();
}

//-----------------------------------------------------------------------------
// Handle_Out1
//-----------------------------------------------------------------------------
// Take the received packet from the host off the fifo and put it into
// the OUT_PACKET array.
//
//-----------------------------------------------------------------------------
void Handle_Out1 ()
{
   U8 count = 0;
   U8 controlReg;

   // Set index to endpoint 1 registers
   POLL_WRITE_BYTE(INDEX, 1);
   POLL_READ_BYTE(EOUTCSR1, controlReg);

   // If endpoint is halted, send a stall
   if (EP_Status[2] == EP_HALT)
   {
      POLL_WRITE_BYTE(EOUTCSR1, rbOutSDSTL);
   }
   // Otherwise read packet from host
   else
   {
      // Clear sent stall bit if last packet was a stall
      if (controlReg & rbOutSTSTL)
      {
         POLL_WRITE_BYTE(EOUTCSR1, rbOutCLRDT);
      }

      POLL_READ_BYTE(EOUTCNTL, count);

      // If host did not send correct packet size, flush buffer
      if (count != OUT_EP1_PACKET_SIZE)
      {
         POLL_WRITE_BYTE(EOUTCSR1, rbOutFLUSH);
      }
      // Otherwise get the data packet
      else
      {
         Fifo_Read(FIFO_EP1, OUT_EP1_PACKET_SIZE, (U8*)Out_Packet);
      }

      // Clear Out Packet ready bit
      POLL_WRITE_BYTE(EOUTCSR1, 0);
   }
}

//-----------------------------------------------------------------------------
// Fifo_Read
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   :
//                1) U8 addr : target address
//                2) U16 uNumBytes : number of bytes to unload
//                3) U8* pData : read data destination
//
// Read from the selected endpoint FIFO
//
//-----------------------------------------------------------------------------
void Fifo_Read (U8 addr, U16 uNumBytes, U8* pData)
{
   U16 i;

   if (uNumBytes)                      // Check if >0 bytes requested,
   {
      USB0ADR = addr;                  // Set address
      USB0ADR |= 0xC0;                 // Set auto-read and initiate
                                       // first read

      // Unload <NumBytes> from the selected FIFO
      for(i = 0; i < uNumBytes; i++)
      {
         while (USB0ADR & 0x80);       // Wait for BUSY->'0' (data ready)
         pData[i] = USB0DAT;           // Copy data byte
      }

      //while(USB0ADR & 0x80);         // Wait for BUSY->'0' (data ready)
      USB0ADR = 0;                     // Clear auto-read
   }
}

//-----------------------------------------------------------------------------
// Fifo_Write_InterruptServiceRoutine
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   :
//                1) U8 addr : target address
//                2) U16 uNumBytes : number of bytes to unload
//                3) U8* pData : location of source data
//
// Write to the selected endpoint FIFO
//
// Fifo_Write_InterruptServiceRoutine is used for calls made in an ISR.
//
//-----------------------------------------------------------------------------
void Fifo_Write_InterruptServiceRoutine (U8 addr, U16 uNumBytes, U8 const * pData)
{
   U16 i;

   // If >0 bytes requested,
   if (uNumBytes)
   {
      while (USB0ADR & 0x80);          // Wait for BUSY->'0'
                                       // (register available)
      USB0ADR = addr;                  // Set address (mask out bits7-6)

      // Write <NumBytes> to the selected FIFO
      for (i = 0; i < uNumBytes; i++)
      {
         USB0DAT = pData[i];
         while (USB0ADR & 0x80);       // Wait for BUSY->'0' (data ready)
      }
   }
}

//-----------------------------------------------------------------------------
// Force_Stall
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Force a procedural stall to be sent to the host
//
//-----------------------------------------------------------------------------
void Force_Stall (void)
{
   POLL_WRITE_BYTE (INDEX, 0);
   POLL_WRITE_BYTE (E0CSR, rbSDSTL);   // Set the send stall bit
   EP_Status[0] = EP_STALL;            // Put the endpoint in stall status
}

//-----------------------------------------------------------------------------
// Send_Packet
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Send the packet in In_Packet using the USB IN FIFO if available.
//
// - Copy a packet from In3_Packet to IN FIFO if available and not halted
//   and set IN packet ready to eventually transmit the packet to the host
//
// If FIFO has a room
static void Send_Packet_ISR()
{
   U8 controlReg;

   POLL_WRITE_BYTE(INDEX, 3);
   POLL_READ_BYTE(EINCSRL, controlReg);

   if ( EP_Status[3] != EP_HALT )
   {
      //If FIFO available
      if ( !(controlReg & rbINPRDY) )
      {
         // Put new data to FIFO
         //In3_Packet[1]++;
         //Fifo_Write_InterruptServiceRoutine(FIFO_EP3, IN_EP3_PACKET_SIZE, In3_Packet);
         In3PacketLock = 1;
         Fifo_Write_InterruptServiceRoutine(FIFO_EP3, ADS_DATA_OUT_LENGTH, ADS_Data_Out);
         In3PacketLock = 0;
         // Set In Packet ready bit, indicating a packet is ready
         // to send to the host
         POLL_WRITE_BYTE(EINCSRL, rbInINPRDY);
      }
   }
}

void Send_Packet_Foreground()
{
   U8 controlReg;

   POLL_WRITE_BYTE(INDEX, 3);
   POLL_READ_BYTE(EINCSRL, controlReg);

   if ( EP_Status[3] != EP_HALT )
   {
      //If FIFO available
      if ( !(controlReg & rbINPRDY) )
      {
         // Put new data to FIFO
         Fifo_Write_InterruptServiceRoutine(FIFO_EP3, IN_EP3_PACKET_SIZE, In3_Packet);

         // Set In Packet ready bit, indicating a packet is ready
         // to send to the host
         POLL_WRITE_BYTE(EINCSRL, rbInINPRDY);
      }
   }
}
