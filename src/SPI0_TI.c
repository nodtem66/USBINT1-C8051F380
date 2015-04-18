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
U8 bufferSPI;
U8 countADC_RDY = 0;
volatile U8 countDelay = 0;

// ADSDelay(count)
// delay count*31.25 msec
// this function set global counter `countDelay` and
// enable Timer3 reloaded at 32Hz
void ADSDelay(U8 count)
{
   countDelay = count;
   TMR3CN |= TMR3CN_TR3__RUN; // start timer3 at 32Hz
   while(countDelay);
}
// ADSCommand(command)
// send command to ADS. normally use with definition ADS_CMD_*
void ADSCommand(U8 cmd)
{
   // Wait until the SPI is free
   while(!SPI0CN_NSSMD0);

   SPI_START()
   WRITE_SPI(cmd)
   DelayForADS();
   SPI_END()

   In_Packet[INEP0_SPI_TX_CNT]++;
}

// ADSWrite(address, value)
// write one byte to register
void ADSWrite(U8 Address, U8 Data)
{
   // Wait until the SPI is free
   while(!SPI0CN_NSSMD0);

   SPI_START()
   WRITE_SPI(ADS_CMD_WREG | (Address & 0x1F)) // write address
   DelayForADS();
   WRITE_SPI(0x00) // write <number of register - 1> to write
   DelayForADS();
   WRITE_SPI(Data & 0xFF) // write value to address
   SPI_END()

   In_Packet[INEP0_SPI_TX_CNT]++;
}

// ADSRead(address, value)
// read one byte from register
U8 ADSRead(U8 Address)
{
   U8 buffer = 0;
   // Wait until the SPI is free
   while(!SPI0CN_NSSMD0);

   SPI_START()
   WRITE_SPI(ADS_CMD_RREG | (Address & 0x1F)) // write address
   DelayForADS();
   WRITE_SPI(0x00) // write <number of register - 1> to read
   DelayForADS();
   READ_SPI(buffer)
   SPI_END()
   In_Packet[INEP0_SPI_RX_CNT]++;
   return buffer;
}
void ADSInit()
{
   ADSWrite(ADS_CONFIG1, ADS_CONFIG1__HIGH_RES_1K_SPS);
   // power down REF buffer for internal reference
   ADSWrite(ADS_CONFIG3, ADS_CONFIG3__DEFAULT | ADS_CONFIG3__PD_REFBUF
         | ADS_CONFIG3__VREF_4V | ADS_CONFIG3__RLDREF_INT | ADS_CONFIG3__PD_RLD);
   ADSWrite(ADS_CONFIG4, 0x00);

   ADSWrite(ADS_RLD_SENSP, 0x06);
   ADSWrite(ADS_RLD_SENSN, 0x06);


   // ECG Normal mode
   ADSWrite(ADS_CONFIG2, 0x00);
   ADSWrite(ADS_WCT1, ADS_WCT1__PD_WCTA | ADS_WCT1__WCTA_CH2N);
   ADSWrite(ADS_WCT2, ADS_WCT2__PD_WCTB | ADS_WCT2__PD_WCTC
         | ADS_WCT2__WCTB_CH2P | ADS_WCT2__WCTC_CH3P);
   ADSWrite(ADS_CH1SET, ADS_CHSET__GAIN_8X);
   ADSWrite(ADS_CH2SET, ADS_CHSET__GAIN_8X);
   ADSWrite(ADS_CH3SET, ADS_CHSET__GAIN_8X);
   ADSWrite(ADS_CH4SET, ADS_CHSET__GAIN_8X);
   ADSWrite(ADS_CH5SET, ADS_CHSET__GAIN_8X);
   ADSWrite(ADS_CH6SET, ADS_CHSET__GAIN_8X);
   ADSWrite(ADS_CH7SET, ADS_CHSET__GAIN_8X);
   ADSWrite(ADS_CH8SET, ADS_CHSET__GAIN_8X);

   // ECG Test mode
   /*
   ADSWrite(ADS_CONFIG2, ADS_CONFIG2__INT_TEST_4HZ);
   ADSWrite(ADS_WCT1, 0x00);
   ADSWrite(ADS_WCT2, 0x00);
   ADSWrite(ADS_CH1SET, ADS_CHSET__TEST_SIGNAL);
   ADSWrite(ADS_CH1SET, ADS_CHSET__TEST_SIGNAL);
   ADSWrite(ADS_CH1SET, ADS_CHSET__TEST_SIGNAL);
   ADSWrite(ADS_CH1SET, ADS_CHSET__TEST_SIGNAL);
   ADSWrite(ADS_CH1SET, ADS_CHSET__TEST_SIGNAL);
   ADSWrite(ADS_CH1SET, ADS_CHSET__TEST_SIGNAL);
   ADSWrite(ADS_CH1SET, ADS_CHSET__TEST_SIGNAL);
   ADSWrite(ADS_CH1SET, ADS_CHSET__TEST_SIGNAL);
   */
}
