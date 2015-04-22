/*
 * SPI0_TI.c
 *
 *  Created on: 18 Mar 2015
 *      Author: Jirawat I.
 */

#include "c8051f3xx.h"
#include "SPI0_TI.h"
#include "USBINT1_Main.h"
#include "USB0_InterruptServiceRoutine.h"

// SPI flag to trigger NSS
U8 readySPI;
U8 countADC_RDY=0;
U8 bufferSPI=0;

// AFE4490Write(address, data)
// write the 24bit data to 8bit address
void AFE4490Write(U8 Address, U32 Data)
{
   // Wait until the SPI is free

   while(!SPI0CN_NSSMD0);

   SPI_START(); // enable device

   WRITE_SPI(Address); // send address 8 bit
   // send data MSB first
   WRITE_SPI((Data >> 16) & 0xFF); // send top 8 bit
   WRITE_SPI((Data >> 8) & 0xFF); // send middle 8 bit
   WRITE_SPI(Data & 0xFF); // send bottom 8 bit
   SPI_END(); // disable device

   In_Packet[INEP0_SPI_TX_CNT]++;
}
// AF4490Read(address)
// read 24bit data in 8bit SPI address and return in 32bit
// Note: set SPI_READ=1 in CONTROL1 register before use this
U32 AFE4490Read(U8 Address)
{
   U32 Data=0;
   U8 buffer;
   // Wait until the SPI is free
   while(!SPI0CN_NSSMD0);
   SPI_START(); // enable device
   WRITE_SPI(Address); // send address 8 bit
   READ_SPI(buffer); // get first MSB 8bit
   Data = (buffer << 8);
   READ_SPI(buffer); // get middle 8 bit
   Data = (Data | buffer) << 8;
   READ_SPI(buffer); // get LSB 8 bit
   Data |= buffer;
   SPI_END(); // disable device
   In_Packet[INEP0_SPI_RX_CNT]++;
   return Data;
}

// AFE4490Init()
// initialize the AFE4490 function:
// * Reset Control0/1/2
// * Set Clock edge mapping to SPI register
// * Enable timer
// * Set Gain and Ambient cancellation
// * Set LED power control
void AFE4490Init() {
   // According to Example Clock Edge Mapping to SPI Registers
   // AFE4490 datasheet December, 2012 Revised October 2014 Page.36
   // This configuration set PRF 500Hz and duty cycle 25%
   // The sysclk of AFE4490 = 4MHz;
   // PRF 500Hz = 8000 cycles
   // Note: ALED2 = Ambient LED2; LED2 = RED LED
   //       ALED1 = Ambient LED1; LED1 = IR LED
   // the pulse cycle start from: ALED2 > LED1  > ALED1 > LED2
   // the conversion start from:  LED2  > ALED2 > LED1  > ALED1
   AFE4490Write(CONTROL0, 0);
   AFE4490Write(CONTROL2, 0);
   AFE4490Write(PRPCOUNT, 7999);

   // ADC Reset count cycle
   AFE4490Write(ADCRSTCNT0, 0);
   AFE4490Write(ADCRSTENDCT0, 3);
   AFE4490Write(ADCRSTCNT1, 2000);
   AFE4490Write(ADCRSTENDCT1, 2003);
   AFE4490Write(ADCRSTCNT2, 4000);
   AFE4490Write(ADCRSTENDCT2, 4003);
   AFE4490Write(ADCRSTCNT3, 6000);
   AFE4490Write(ADCRSTENDCT3, 6003);

   // ADC Conversion count cycle
   AFE4490Write(LED2CONVST, 4);
   AFE4490Write(LED2CONVEND, 1999);
   AFE4490Write(ALED2CONVST, 2004);
   AFE4490Write(ALED2CONVEND, 3999);
   AFE4490Write(LED1CONVST, 4004);
   AFE4490Write(LED1CONVEND, 5999);
   AFE4490Write(ALED1CONVST, 6004);
   AFE4490Write(ALED1CONVEND, 7999);

   // ALED2 sample count cycle
   AFE4490Write(ALED2STC, 50);
   AFE4490Write(ALED2ENDC, 1998);

   AFE4490Write(LED1LEDSTC, 2000);  // LED1 turn on
   AFE4490Write(LED1STC, 2050);     // LED1 sample
   AFE4490Write(LED1ENDC, 3998);    // LED1 stop sample
   AFE4490Write(LED1LEDENDC, 3999); // LED1 turn off

   // ALED1 sample count cycle
   AFE4490Write(ALED1STC, 4050);
   AFE4490Write(ALED1ENDC, 5998);

   AFE4490Write(LED2LEDSTC, 6000);  // LED2 turn on
   AFE4490Write(LED2STC, 6050);     // LED2 sample
   AFE4490Write(LED2ENDC, 7998);    // LED2 stop sample
   AFE4490Write(LED2LEDENDC, 7999); // LED2 turn off

   AFE4490Write(CONTROL1, CONTROL1_TIMEREN_SET | (CONTROL1_AVERAGE_MARK & 9));
   AFE4490Write(TIAGAIN, 0);
   AFE4490Write(TIA_AMB_GAIN, 0);
   AFE4490Write(LEDCNTRL, 0x0003030); // LED Bightness config

   AFE4490Write(CONTROL0, CONTROL0_SPI_READ); // switch to SPI_READ mode
}

void AFE4490Init2()
{
   AFE4490Write(CONTROL0, 0x000000);
   AFE4490Write(CONTROL2, 0x000000);
   AFE4490Write(LED2STC, 0X001770); //timer control
   AFE4490Write(LED2ENDC,0X001DAF); //timer control
   AFE4490Write(LED2LEDSTC,0X001770); //timer control
   AFE4490Write(LED2LEDENDC,0X001DAF); //timer control
   AFE4490Write(ALED2STC, 0X000000); //timer control
   AFE4490Write(ALED2ENDC, 0X00063F); //timer control
   AFE4490Write(LED1STC, 0X0007D0); //timer control
   AFE4490Write(LED1ENDC, 0X000E0F); //timer control
   AFE4490Write(LED1LEDSTC, 0X0007D0); //timer control
   AFE4490Write(LED1LEDENDC, 0X000E0F); //timer control
   AFE4490Write(ALED1STC, 0X000FA0); //timer control
   AFE4490Write(ALED1ENDC, 0X0015DF); //timer control
   AFE4490Write(LED2CONVST,0X000002); //timer control
   AFE4490Write(LED2CONVEND, 0X0007CF); //timer control
   AFE4490Write(ALED2CONVST, 0X0007D2); //timer control
   AFE4490Write(ALED2CONVEND,0X000F9F); //timer control
   AFE4490Write(LED1CONVST, 0X000FA2); //timer control
   AFE4490Write(LED1CONVEND, 0X001F3F); //timer control
   AFE4490Write(ALED1CONVST, 0X001772); //timer control
   AFE4490Write(ALED1CONVEND, 0X001F3F); //timer control
   AFE4490Write(ADCRSTCNT0, 0X000000); //timer control
   AFE4490Write(ADCRSTENDCT0,0X000000); //timer control
   AFE4490Write(ADCRSTCNT1, 0X0007D0); //timer control
   AFE4490Write(ADCRSTENDCT1, 0X0007D0); //timer control
   AFE4490Write(ADCRSTCNT2, 0X000FA0); //timer control
   AFE4490Write(ADCRSTENDCT2, 0X000FA0); //timer control
   AFE4490Write(ADCRSTCNT3, 0X001770); //timer control
   AFE4490Write(ADCRSTENDCT3, 0X001770);//timer control
   AFE4490Write(PRPCOUNT, 0X001F3F);//timer control
   AFE4490Write(CONTROL1,0x000107);      // Timers ON, average 3 samples
   AFE4490Write(TIAGAIN,0x000000);
   AFE4490Write(TIA_AMB_GAIN,0x000000);
   AFE4490Write(LEDCNTRL,0x0018080);     // LED_RANGE=100mA, LED=50mA
}
