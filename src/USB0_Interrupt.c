//-----------------------------------------------------------------------------
// F380_USB0Interrupt.c
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
// Target:         C8051F38x
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
//    -All changes by BW
//    -1 SEP 2010
//    -Updated USB0_Init() to write 0x89 to CLKREC instead of 0x80
// Release 1.0
//    -Initial Revision (PD)
//    -04 JUN 2008
//

//-----------------------------------------------------------------------------
// Header Files
//-----------------------------------------------------------------------------

#include "c8051f3xx.h"
#include "USB0_InterruptServiceRoutine.h"
#include "USB0_Register.h"
#include "USBINT1_Main.h"
#include "SPI0_TI.h"

#if TARGET_MCU != MCU_F380
#error Invalid TARGET_MCU definition
#endif // TARGET_MCU != MCU_F380
//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

#define SYSCLK             24000000    // SYSCLK frequency in Hz
// USB clock selections (SFR CLKSEL)
#define USB_4X_CLOCK       0x00        // Select 4x clock multiplier, for USB
#define USB_INT_OSC_DIV_2  0x10        // Full Speed
#define USB_EXT_OSC        0x20
#define USB_EXT_OSC_DIV_2  0x30
#define USB_EXT_OSC_DIV_3  0x40
#define USB_EXT_OSC_DIV_4  0x50

// System clock selections (SFR CLKSEL)
#define SYS_INT_OSC        0x00        // Select to use internal oscillator
#define SYS_EXT_OSC        0x01        // Select to use an external oscillator
#define SYS_4X_DIV_2       0x02

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

// Initialization Routines
void Sysclk_Init(void);               // Initialize the system clock
void Port_Init(void);                 // Configure ports
void Timer2_Init(void);               // Configure Timer2
void Timer3_Init(void);               // Configure Timer3
void ADC0_Init(void);                 // Configure ADC0
void USB0_Init(void);                 // Configure USB core
void USB0_Suspend(void);              // Suspend System
void Delay(void);                     // About 80us/1ms on Full/LowSpeed
void UART1_Init(void);                // configure UART1
void PCA0_Init(void);                 // configure PCA for external interrupts

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------
//SBIT (Led1, SFR_P2, 2);                // LED='1' means ON
//SBIT (Led2, SFR_P2, 3);
//SBIT (Sw1, SFR_P2, 0);                 // SW='0' means pressed
//SBIT (Sw2, SFR_P2, 1);
// This constant is added to Temperature
SEGMENT_VARIABLE (TEMP_ADD, const S16, SEG_CODE) = -231;

//-----------------------------------------------------------------------------
// External Global Variables
//-----------------------------------------------------------------------------

//U8 Switch1State = 0;                   // Indicate status of switch
//U8 Switch2State = 0;                   // starting at 0 == unpressed

//U8 Potentiometer = 0x00;               // Last read potentiometer value
U8 Temperature = 0x00;                   // Last read temperature sensor value

//-----------------------------------------------------------------------------
// Initialization Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// System_Init
//-----------------------------------------------------------------------------
// This function is declared in the header file F3xx_HIDtoUART.h and is
// called in the main(void) function.  It calls initialization routines
// local to this file.
//
//-----------------------------------------------------------------------------
void System_Init(void)
{
   PCA0MD &= ~0x40;                   // Disable watchdog timer

   Sysclk_Init();                     // Initialize oscillator
   Port_Init();                       // Initialize crossbar and GPIO
   Timer2_Init();                     // Initialize Timer2
   Timer3_Init();                     // Initialize Timer3
   //ADC0_Init ();                    // Initialize ADC0
}

//-----------------------------------------------------------------------------
// USB0_Init
//-----------------------------------------------------------------------------
// USB Initialization performs the following:
// - Initialize USB0
// - Enable USB0 interrupts
// - Enable USB0 transceiver
// - Enable USB0 with suspend detection
//
//-----------------------------------------------------------------------------
void USB0_Init(void)
{
   POLL_WRITE_BYTE(POWER, 0x08);    // Force Asynchronous USB Reset
   POLL_WRITE_BYTE(IN1IE, 0x0F);    // Enable Endpoint 0,1, and 3 in interrupts
   POLL_WRITE_BYTE(OUT1IE, 0x02);   // Enable Endpoint 1 out interrupts
   POLL_WRITE_BYTE(CMIE, 0x0F);     // Enable SOF, Reset, Resume, and Suspend
                                    // interrupts

#ifdef _USB_LOW_SPEED_
   USB0XCN = 0xC0;                  // Enable transceiver; select low speed
   POLL_WRITE_BYTE (CLKREC, 0xA9);  // Enable clock recovery, single-step
                                    // mode disabled
#else
   USB0XCN = 0xE0;                  // Enable transceiver; select full speed
   POLL_WRITE_BYTE(CLKREC, 0x89);   // Enable clock recovery, single-step
                                    // mode disabled
#endif // _USB_LOW_SPEED_
   EIE1 |= 0x02;                    // Enable USB0 Interrupts

   // Enable USB0 by clearing the USB
   POLL_WRITE_BYTE(POWER, POWER_ISOUD__SOF_TOKEN | POWER_SUSEN__ENABLED);
}

//-----------------------------------------------------------------------------
// UART1_Init
//-----------------------------------------------------------------------------
// UART1 Initialize following step
// - enable interrupts
// - configure 9600/8E1
void UART1_Init(void)
{
   EIE2 |= EIE2_ES1__ENABLED;          //enable UART1 Interrupt
   SCON1 = SCON1_REN__RECEIVE_ENABLED; //enable UART1 reception
   SMOD1 = SMOD1_PE__PARITY_ENABLED    //configure 8 bit even parity
         | SMOD1_SDL__8_BITS | SMOD1_SPT__EVEN_PARITY;

   SBRL1 = 0xFB1E;                     //configure buad rate for 9600
   SBCON1 = SBCON1_SBRUN__ENABLED      //enable baud rate generator
         | SBCON1_SBPS__DIV_BY_1;

   SCON1 |= SCON1_TI__SET;             //set ready to TX
}

//-----------------------------------------------------------------------------
// SPI_Init
//-----------------------------------------------------------------------------
// SPI0 initialled by following step
// - disable SPI0
// - set MASTER MODE
// - set 4-wire configuration
// - set CLKPOL and CLKPHA
// - enable SPI0
//-----------------------------------------------------------------------------
void SPI0_Init(void)
{
   SPI0CN_SPIEN = SPI0CN_SPIEN__DISABLED;

   // set 4-wire single master; NSS signal is an active-low output
   SPI0CN |= SPI0CN_NSSMD__4_WIRE_MASTER_NSS_LOW;
   SPI0CFG = SPI0CFG_CKPHA__DATA_CENTERED_SECOND | SPI0CFG_CKPOL__IDLE_LOW
         | SPI0CFG_MSTEN__MASTER_ENABLED;
   SPI0CKR = SYSCLK / (2 * 500000) - 1;
   SPI0CN_SPIEN = SPI0CN_SPIEN__ENABLED;
   IE_ESPI0 = 1;  // enable SPI interrupt
   readySPI = 0;  // reset SPI flag
}
//-----------------------------------------------------------------------------
// USB0_Suspend
//-----------------------------------------------------------------------------
// Enter suspend mode after suspend signalling is present on the bus
//
void USB0_Suspend(void)
{
   U8 ADC0CN_Val = ADC0CN;    // Store the current value of ADC0CN
   U8 REF0CN_Val = REF0CN;    // Store the current value of REF0CN

   // Power Down

   ADC0CN = 0x00;             // Disable ADC0
   IE_EA = 0;                 // Disable global interrupts
   REF0CN = 0x00;             // Disable voltage references
   XBR1 &= ~0x40;             // Disengage the crossbar

   USB0XCN &= ~0x40;          // Suspend USB Transceiver
   CLKSEL = 0x10;             // USB Clock now uses int osc / 2
   CLKMUL = 0x00;             // Disable Clock Multiplier
   REG01CN |= 0x10;           // Place voltage regulator in low power mode
   OSCICN |= 0x20;            // Force internal oscillator suspend

   NOP ();

   // Power Up
   // Code execution begins here when resume signaling is received.

   REG01CN &= ~0x10;          // Place voltage regulator in full power mode
   Sysclk_Init();
   USB0XCN |= 0x40;           // Enable USB Transceiver

   XBR1 |= 0x40;              // Engage the crossbar
   ADC0CN = ADC0CN_Val;       // Place ADC0 in its previous state
   REF0CN = REF0CN_Val;       // Place REF0CN in its previous state
   IE_EA = 1;
}

//-----------------------------------------------------------------------------
// Sysclk_Init
//-----------------------------------------------------------------------------
// This function initializes the system clock and the USB clock.
// Low-speed System Clock:  12 MHz  USB Clock: 12 MHz
// Full-speed System Clock: 24 MHz  USB Clock: 48 MHz
//
//-----------------------------------------------------------------------------
void Sysclk_Init(void)
{
#ifdef _USB_LOW_SPEED_

   OSCICN |= 0x03;                     // Configure internal oscillator for
                                       // its maximum frequency and enable
                                       // missing clock detector

   CLKSEL = SYS_INT_OSC;// Select System clock
   CLKSEL |= USB_INT_OSC_DIV_2;// Select USB clock
#else
   OSCICN |= 0x03;                     // Configure internal oscillator for
                                       // its maximum frequency and enable
                                       // missing clock detector
   /*
    CLKMUL  = 0x00;                     // Select internal oscillator as
    // input to clock multiplier

    CLKMUL |= 0x80;                     // Enable clock multiplier
    Delay();                            // Delay for clock multiplier to begin
    CLKMUL |= 0xC0;                     // Initialize the clock multiplier
    Delay();                            // Delay for clock multiplier to begin

    while(!(CLKMUL & 0x20));            // Wait for multiplier to lock
    */
   CLKSEL = 0x02;                     // Set sys clock to clkmul / 2
#endif  // _USB_LOW_SPEED_
}

//-----------------------------------------------------------------------------
// Port_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function configures the crossbar and GPIO ports.
//
// P0.0  -  Skipped,     Push-Pull,  Digital }
// P0.1  -  Skipped,     Push-Pull,  Digital |
// P0.2  -  Skipped,     Push-Pull,  Digital
// P0.3  -  Skipped,     Push-Pull,  Digital
// P0.4  -  Skipped,     Push-Pull,  Digital debugging
// P0.5  -  Skipped,     Push-Pull,  Digital  /
// P0.6  -  Skipped,     Push-Pull,  Digital |
// P0.7  -  Skipped,     Push-Pull,  Digital }

// P1.0  -  Skipped,     Open-Drain, Digital
// P1.1  -  Skipped,     Open-Drain, Digital
// P1.2  -  Skipped,     Open-Drain, Digital
// P1.3  -  Skipped,     Open-Drain, Digital
// P1.4  -  Skipped,     Open-Drain, Digital
// P1.5  -  Skipped,     Open-Drain, Digital
// P1.6  -  Skipped,     Open-Drain, Digital
// P1.7  -  Skipped,     Open-Drain, Digital

// P2.0  -  Skipped,     Open-Drain, Digital
// P2.1  -  Skipped,     Open-Drain, Digital
// P2.2  -  SCK  (SPI0), Push-Pull,  Digital -- SCLK
// P2.3  -  MISO (SPI0), Open-Drain, Digital -- SPIOUT
// P2.4  -  MOSI (SPI0), Push-Pull,  Digital -- SPIIN
// P2.5  -  NSS  (SPI0), Push-Pull,  Digital -- SPICS
// P2.6  -  CEX0  (PCA), Open-Drain, Digital -- DRDY
// P2.7  -  Skipped,     Open-Drain, Digital

// P3.0  -  Unassigned,  Open-Drain, Digital -- GPIO4
// P3.1  -  Unassigned,  Open-Drain, Digital -- GPIO3
// P3.2  -  Unassigned,  Open-Drain, Digital -- GPIO2
// P3.3  -  Unassigned,  Open-Drain, Digital -- GPIO1
// P3.4  -  Unassigned,  Open-Drain, Digital -- START
// P3.5  -  Unassigned,  Open-Drain, Digital
// P3.6  -  Unassigned,  Open-Drain, Digital
// P3.7  -  Unassigned,  Open-Drain, Digital
//-----------------------------------------------------------------------------
void Port_Init(void)
{
   //P0MDOUT = 0xFB; // 1111 1011 P0.7-P0.0
   P2MDOUT = 0x34; // for Push-Pull 0011 0100 (0x34)

   P0SKIP = 0xFF; // Skip all Port0 pin
   P1SKIP = 0xFF; // Skip all Port1 pin
   P2SKIP = 0x83; // Skip 1000 0011
   XBR0 = XBR0_SPI0E__ENABLED;        // Enable SPI0
   XBR1 = XBR1_XBARE__ENABLED | 0x01; // Enable the crossbar and PCA0 (CEX0)
   //XBR2 = XBR2_URT1E__ENABLED;        // Enable UART1

   P0_B3 = 0;    // Reset P0.3 to 0
   P0_B4 = 0;    // Reset P0.4 to 0
   P0_B5 = 0;    // Reset P0.5 to 0
}

//-----------------------------------------------------------------------------
// Timer2_Init
//-----------------------------------------------------------------------------
// Timer 2 reload, used to delay SPI NSS hold active more than 4*T_CLK_ADS
// Full-speed (SYSCLK: 24 MHz) Based Timer frquency SYSCLK/12 : 2 MHz
// Note: 4*T_CLK = 1.9532125us (512KHz)
// This timer reload with 2MHz / 4 = 500KHz
//-----------------------------------------------------------------------------
void Timer2_Init(void)
{
   TMR2CN = 0x00;           // Stop Timer2; Clear TF2;
   CKCON &= ~0xF0;          // Timer2 clocked based on TMR2CN_T2XCLK (SYSCLK/12)
   TMR2RL = 0x10000 - 4;    // Initialize reload value
   TMR2 = 0xffff;           // Set to reload immediately
   IE_ET2 = 1;              // Enable Timer2 interrupts
   TMR2CN_TR2 = 1;          // Start Timer2
}

//-----------------------------------------------------------------------------
// Timer3_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Timer 3 reload, used to delay
// Timeout 0.03125 second (32Hz)
//
// Full-speed (SYSCLK: 24 MHz) reload rate: 32 Hz
//
//-----------------------------------------------------------------------------
void Timer3_Init(void)
{
   TMR3CN = 0x00;            // Stop Timer3; Clear TF3;
   TMR3RL = 0x10000 - 62500; // Timer3 clocked based on TMR3CN_T2XCLK (SYSCLK/12)
   TMR3 = TMR3RL;            // set to reload immediately
   EIE1 |= EIE1_ET3__ENABLED;// enable Timer3 interrupts
   TMR3CN = TMR3CN_TR3__RUN; // start Timer3
}

//-----------------------------------------------------------------------------
// ADC0_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
// 
// Configures ADC for single ended continuous conversion or Timer2
//-----------------------------------------------------------------------------
void ADC0_Init(void)
{
   REF0CN = 0x0E; // Enable voltage reference VREF
   AMX0P = 0x1E;  // Positive input starts as temp sensor
   AMX0N = 0x1F;  // Single ended mode(neginput = gnd)

   ADC0CF = 0xF8; // SAR Period 0x1F, Right adjusted

   ADC0CN = 0xC2; // Continuous converion on timer 2
                  // overflow; low power tracking mode on

   EIE1 |= 0x08;  // Enable conversion complete interrupt
}

void PCA0_Init(void)
{
   PCA0CN = 0; // reset and stop PCA0 counter/timer
   EIE1 |= EIE1_EPCA0__ENABLED;       // enable pca0 interrupts
   PCA0MD = PCA0MD_ECF__OVF_INT_DISABLED // disable overwrite counter/timer interrupt
            | PCA0MD_CPS__SYSCLK_DIV_12; // use SYSCLK to PCA0 based clock
   PCA0CPM0 = PCA0CPM0_CAPN__ENABLED  // enable Capture at falling edge
            | PCA0CPM0_ECCF__ENABLED; // enable capture/compare flag interrupt
   PCA0CN = PCA0CN_CR__RUN;           // run PCA counter/timer
}
//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Timer2_ISR
//-----------------------------------------------------------------------------
//
// Called when timer 2 overflows
//
//-----------------------------------------------------------------------------
INTERRUPT (Timer2_ISR, TIMER2_IRQn)
{
   TMR2CN_TF2H = 0; // clear timer interrupt flag
   TMR2CN_TR2 = 0;  // stop timer
   //TODO: use #3 to debug
   In_Packet[3]++;
   SPI0CN_NSSMD0 = 1; //active NSS (deselect device)
}
//-----------------------------------------------------------------------------
// Timer2_ISR
//-----------------------------------------------------------------------------
//
// Called when timer 2 overflows
//
//-----------------------------------------------------------------------------
INTERRUPT (Timer3_ISR, TIMER3_IRQn)
{
   TMR3CN &= ~TMR3CN_TF3H__SET;   // reset pending flag
   //TODO: use #3 to debug
   In_Packet[7]++;
   countDelay--;
   if (countDelay <= 0)
   {
      TMR3CN &= ~TMR3CN_TR3__RUN; // stop timer
   }
}
//-----------------------------------------------------------------------------
// ADC0_ConvComplete_ISR
//-----------------------------------------------------------------------------
//
// Called after a conversion of the ADC has finished
// Updates the appropriate variable for potentiometer or temperature sensor
// Switches the ADC multiplexor value to switch between the potentiometer 
// and temp sensor
//
//-----------------------------------------------------------------------------
INTERRUPT (ADC0_ConvComplete_ISR, ADC0EOC_IRQn)
{
   S16 temp;

   // This switches the AMUX between the temperature sensor and the
   // potentiometer pin after conversion
   if (AMX0P == 0x1E)
   {
      // Add offset to Temperature
      temp = (ADC0H << 8) + ADC0L;
      temp += TEMP_ADD;
      Temperature = (U8) temp;
   }

   ADC0CN_ADINT = 0;
}

//-----------------------------------------------------------------------------
// SPI0_ISR
//-----------------------------------------------------------------------------
//
// called when byte transferred (both transmitted and received)
//
//-----------------------------------------------------------------------------
INTERRUPT (SPI0_ISR, SPI0_IRQn)
{
   // write collision interrupt
   if (SPI0CN_WCOL)
   {
      SPI0CN_WCOL = SPI0CN_WCOL__NOT_SET;
   }
   // SPI0 transfer interrupt
   if (SPI0CN_SPIF)
   {
      SPI0CN_SPIF = 0;
      SPI0CN_MODF = 0;
      // if read spi is running
      if (readySPI & READY_SPI_READ)
      {
         bufferSPI = SPI0DAT;
         readySPI &= ~READY_SPI_READ;
      }
      // if last byte to transfer
      if (readySPI & READY_SPI_END)
      {
         readySPI &= ~READY_SPI_END; // reset flag
         TMR2CN_TR2 = 1; // delay before deselect device
      }
   }
}
//-----------------------------------------------------------------------------
// PCA0_ISR
//-----------------------------------------------------------------------------
//
// called when ADC_READY ON CEX0
//
//-----------------------------------------------------------------------------
INTERRUPT (PCA0_ISR, PCA0_IRQn)
{
   if (PCA0CN_CCF0)
   {
      PCA0CN_CCF0 = 0;
      In_Packet[0]++;
      //countADC_RDY++;
      readySPI |= (READY_ADC_RDY);
   } else {
      PCA0CN_CF &= ~0x8e;
   }
}

//-----------------------------------------------------------------------------
// Helper Functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Delay
//-----------------------------------------------------------------------------
// Used for a small pause, approximately 0.24 ms in Full Speed,
// and 1 ms when clock is configured for Low Speed
//
//-----------------------------------------------------------------------------
void Delay(void)
{
   int x;
   for (x = 0; x < 500; x)
      x++;
}
//-----------------------------------------------------------------------------
// DelayForADS
//-----------------------------------------------------------------------------
// used to delay SPI NSS hold active more than 4*T_CLK_ADS
// Note: 4*T_CLK = 1.9532125us (512KHz)
// 50 NOP for SYSCLK = 24MHz
//-----------------------------------------------------------------------------
void DelayForADS(void)
{
   while(SPI0CFG & SPI0CFG_SPIBSY__SET);
}
