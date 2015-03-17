/*
 * SPI0_TI.h
 *
 *  Created on: 18 Mar 2015
 *      Author: jirawat
 */

#ifndef SPI0_TI_H_
#define SPI0_TI_H_

// SPI CLOCK 500k
#define SPICLK 500000

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
extern U8 readySPIEnd; // flag to triggle NSS

//-----------------------------------------------------------------------------
// Define macro
//-----------------------------------------------------------------------------
// WRITE_SPI(b)
//    wait for busy transmit holding register and write a byte to SPI
//-----------------------------------------------------------------------------
#define WRITE_SPI(b) while(!SPI0CN_TXBMT); SPI0DAT = (b);
#define SPI_START() readySPIEnd=0; SPI0CN_NSSMD0 = 0;
#define SPI_END() while(!SPI0CN_TXBMT); readySPIEnd=1;

#endif /* SPI0_TI_H_ */
