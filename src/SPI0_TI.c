/*
 * SPI0_TI.c
 *
 *  Created on: 18 Mar 2015
 *      Author: jirawat
 */

#include "c8051f3xx.h"
#include "SPI0_TI.h"
#include "USBINT1_Main.h"

// SPI flag to triggle NSS
U8 readySPIEnd=0;
