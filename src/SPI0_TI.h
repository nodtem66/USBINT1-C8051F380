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

//=============================================================================
// Register definitions for AFE4490
// from AFE4490 datasheet
// note:
// start/end count use from timer schedule
//=============================================================================
#define CONTROL0     0x00 // control register 0 D[23:4] = 0, D3 SW_RESET
#define CONTROL0_SW_RST      1 << 3 // software reset
#define CONTROL0_DIAG_EN     1 << 2 // diagnotic enable
#define CONTROL0_TIM_CNT_RST 1 << 1 // timer counter reset
#define CONTROL0_SPI_READ    1      // SPI Read (1 enable)
#define LED2STC      0x01 // sample LED2 start count
#define LED2ENDC     0x02 // sample LED2 end count
#define LED2LEDSTC   0x03 // LED2 LED start count (LED turn on)
#define LED2LEDENDC  0x04 // LED2 LED end count (LED turn off)
#define ALED2STC     0x05 // sample ambient LED2 start count
#define ALED2ENDC    0x06 // sample ambient LED2 end count
#define LED1STC      0x07 // sample LED1 start count
#define LED1ENDC     0x08 // sample LED1 end count
#define LED1LEDSTC   0x09 // LED1 LED start count (turn on)
#define LED1LEDENDC  0x0a // LED1 LED end count (turn off)
#define ALED1STC     0x0b // sample ambient LED1 start count
#define ALED1ENDC    0x0c // sample ambient LEF1 end count
#define LED2CONVST   0x0d // LED2 convert start count
#define LED2CONVEND  0x0e // LED2 convert end count
#define ALED2CONVST  0x0f // ambient LED2 convert start count
#define ALED2CONVEND 0x10 // ambient LED2 convert end count
#define LED1CONVST   0x11 // LED1 convert start count
#define LED1CONVEND  0x12 // LED1 convert end count
#define ALED1CONVST  0x13 // ambient LED2 convert start count
#define ALED1CONVEND 0x14 // ambient LED2 convert end count
#define ADCRSTCNT0   0x15 // ADC0 reset start count
#define ADCRSTENDCT0 0x16 // ADC0 reset end count
#define ADCRSTCNT1   0x17 // ADC1 reset start count
#define ADCRSTENDCT1 0x18 // ADC1 reset end count
#define ADCRSTCNT2   0x19 // ADC2 reset start count
#define ADCRSTENDCT2 0x1a // ADC2 reset end count
#define ADCRSTCNT3   0x1b // ADC3 reset start count
#define ADCRSTENDCT3 0x1c // ADC3 reset end count
#define PRPCOUNT     0x1d // Pulse Repetition Period count
#define CONTROL1     0x1e // Control register 1
#define CONTROL1_CLKALMPIN_SHIFT 9 // clock on ALM pins PD_ALM LED_ALM
#define CONTROL1_CLKALMPIN_ON_SAMPLE_PULSE    0 << 9
#define CONTROL1_CLKALMPIN_ON_LED_PULSE       1 << 9
#define CONTROL1_CLKALMPIN_ON_AMBIENT_PULSE   2 << 9
#define CONTROL1_CLKALMPIN_ON_CONVERT_PULSE   3 << 9
#define CONTROL1_CLKALMPIN_ON_AMBIENT_CONVERT 4 << 9
#define CONTROL1_TIMEREN_SET   0x000100 // timer enable
#define CONTROL1_AVERAGE_MARK  0x0000FF // Number of ADC average - 1
#define SPARE1       0x1f // Reserved
#define TIAGAIN      0x20 // Transimpedence Amp Gain LED1
#define TIA_AMB_GAIN 0x21 // Transimpedence Amp Gain LED2 and Ambient Cancellation
#define LEDCNTRL     0x22 // LED Control register
#define CONTROL2     0x23 // Control 2 register
#define CONTROL2_TX_REF_SHIFT 17       // Tx reference voltage 0 - 3
#define CONTROL2_RST_ON_PD_ALM   1 << 16 // reset clock onto PD_ALM pin
#define CONTROL2_EN_ADC_BYP      1 << 15 // enable ADC bypass
#define CONTROL2_TXBRGMOD        1 << 11 // enable tx bridge mode (push-pull)
#define CONTROL2_DIGOUT_TRISTATE 1 << 10 // enable digital 3-state mode
#define CONTROL2_XTALDIS         1 << 9  // use external 8MHz clock
#define CONTROL2_EN_SLOW_DIAG    1 << 8  // use slow mode diagnostic
#define CONTROL2_PDN_TX          1 << 2  // Tx is powered down
#define CONTROL2_PDN_RX          1 << 1  // Rx is powered down
#define CONTROL2_PDN_AFE         1       // AFE is powered down
#define SPARE2       0x24 // Reserved
#define SPARE3       0x25 // Reserved
#define SPARE4       0x26 // Reserved
#define RESERVED1    0x27 // Reserved
#define RESERVED2    0x28 // Reserver
#define ALARM        0x29 // Alarm register
#define ALARM_ALMPINCLKEN_SET 1 << 7 // enable Alarm pin clock
#define LED2VAL      0x2a // LED2 sample value
#define ALED2VAL     0x2b // Ambient LED2 value
#define LED1VAL      0x2c // LED1 sample value
#define ALED1VAL     0x2d // Ambient LED1 value
#define LED2ABSVAL   0x2e // (LED2 - Ambient) sample value
#define LED1ABSVAL   0x2f // (LED1 - Ambient) sample value
#define DIAG         0x30 // Diagnostic flag register
#define DIAG_PD_ALM   1 << 12 // power-down alarm status
#define DIAG_LED_ALM  1 << 11 // LED alarm
#define DIAG_LED1OPEN 1 << 10 // LED1 Open
#define DIAG_LED2OPEN 1 << 9  // LED2 Open
#define DIAG_LEDSC    1 << 8  // LED Short
#define DIAG_OUTPSHGND 1 << 7 // OUTP short to ground
#define DIAG_OUTNSHGND 1 << 6 // OUTN short to groud
#define DIAG_PDOC      1 << 5 // PD Open circuit
#define DIAG_PDSC      1 << 4 // PD Short circuit
#define DIAG_INNSCGND  1 << 3 // INN to GND
#define DIAG_INPSCGND  1 << 2 // INP to GND
#define DIAG_INNSCLED  1 << 1 // INN to LED
#define DIAG_INPSCLED  1      // INP to LED
//=============================================================================
// AFE4490 Helper
//=============================================================================
void AFE4490Write(U8, U32);
U32 AFE4490Read(U8);
void AFE4490Init(void);
//=============================================================================
// SPI Helper
//=============================================================================

// In_packet 8 bytes
// -------------------------------------------------------------
// |  1 byte     | 1 byte     | 1 byte     | ...
// -------------------------------------------------------------
// | AFE_RDY_CNT | SPI_RX_CNT | SPI_TX_CNT | ...
// --------------------------------------------------------------
#define INEP0_AFE_RDY_CNT 0
#define INEP0_SPI_RX_CNT  1
#define INEP0_SPI_TX_CNT  2

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
extern U8 readySPI;      // flag 8 bit for SPI Communication
extern U8 countADC_RDY;  // count for ADC_RDY triggers
#define READY_SPI_END 1  // flag to trigger NSS
#define READY_ADC_RDY 2  // flag to trigger Send SPI Read to AFE
//-----------------------------------------------------------------------------
// Define macro
//-----------------------------------------------------------------------------
// WRITE_SPI(b)
//    wait for busy transmit holding register and write a byte to SPI
//-----------------------------------------------------------------------------
#define WRITE_SPI(b) while(!SPI0CN_TXBMT); SPI0DAT = (b);
#define READ_SPI(b) while(!SPI0CN_TXBMT); \
      while((SPI0CFG & SPI0CFG_SPIBSY__SET)); \
      SPI0DAT = 0; b = SPI0DAT;
#define SPI_START() readySPI &= ~READY_SPI_END; SPI0CN_NSSMD0 = 0;
//#define SPI_END() while(!SPI0CN_TXBMT); readySPI |= READY_SPI_END;
#define SPI_END() while(!SPI0CN_TXBMT); \
   while((SPI0CFG & SPI0CFG_SPIBSY__SET)); \
   readySPI |= READY_SPI_END; SPI0CN_NSSMD0 = 1;
#endif /* SPI0_TI_H_ */
