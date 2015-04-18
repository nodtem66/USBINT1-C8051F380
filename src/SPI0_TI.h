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
// definitions for ADS1298
// from ADS1298 datasheet
//=============================================================================

// Commands
#define ADS_CMD_WAKEUP  0x02
#define ADS_CMD_STANDBY 0x04
#define ADS_CMD_RESET   0x06
#define ADS_CMD_START   0x08 // start data conversion
#define ADS_CMD_STOP    0x0a // stop data conversion
#define ADS_CMD_RDATAC  0x10 // Start read data continuous
#define ADS_CMD_SDATAC  0x11 // Stop read data continuous
#define ADS_CMD_RDATA   0x12 // read data
#define ADS_CMD_RREG    0x20 // read register
#define ADS_CMD_WREG    0x40 // write register

// register address
#define ADS_ID          0x00 // ID of ADS version
#define ADS_CONFIG1     0x01
#define ADS_CONFIG2     0x02
#define ADS_CONFIG3     0x03
#define ADS_LOFF        0x04 // Lead off
#define ADS_CH1SET      0x05
#define ADS_CH2SET      0x06
#define ADS_CH3SET      0x07
#define ADS_CH4SET      0x08
#define ADS_CH5SET      0x09
#define ADS_CH6SET      0x0A
#define ADS_CH7SET      0x0B
#define ADS_CH8SET      0x0C
#define ADS_RLD_SENSP   0x0D // Right leg driven
#define ADS_RLD_SENSN   0x0E // Right leg driven
#define ADS_LOFF_SENSP  0x0F // Lead off
#define ADS_LOFF_SENSN  0x10 // Lead off
#define ADS_LOFF_FLIP   0x11 // Lead off
#define ADS_LOFF_STATP  0x12 // Lead off status
#define ADS_LOFF_STATN  0x13 // Lead off status
#define ADS_GPIO        0x14 // general purpose input/output
#define ADS_PACE        0x15 // PACE detection
#define ADS_RESP        0x16 // respiratory
#define ADS_CONFIG4     0x17
#define ADS_WCT1        0x18 // Wilson central terminal
#define ADS_WCT2        0x19 // Wilson central terminal

// CONFIG1: Configuration Register 1
#define ADS_CONFIG1__HR       0x80
#define ADS_CONFIG1__DAISY_EN 0x40
#define ADS_CONFIG1__CLK_EN   0x20
#define ADS_CONFIG1__DR2      0x04
#define ADS_CONFIG1__DR1      0x02
#define ADS_CONFIG1__DR0      0x01
#define ADS_CONFIG1__HIGH_RES_32K_SPS 0x80
#define ADS_CONFIG1__HIGH_RES_16K_SPS 0x81
#define ADS_CONFIG1__HIGH_RES_8K_SPS  0x82
#define ADS_CONFIG1__HIGH_RES_4K_SPS  0x83
#define ADS_CONFIG1__HIGH_RES_2K_SPS  0x84
#define ADS_CONFIG1__HIGH_RES_1K_SPS  0x85
#define ADS_CONFIG1__HIGH_RES_500_SPS 0x86
#define ADS_CONFIG1__LOW_POWR_250_SPS 0x03

// CONFIG2: Configuration Register 2
#define ADS_CONFIG2__WCT_CHOP     0x20
#define ADS_CONFIG2__INT_TEST     0x10
#define ADS_CONFIG2__TEST_AMP     0x04
#define ADS_CONFIG2__TEST_FREQ1   0x02
#define ADS_CONFIG2__TEST_FREQ0   0x01
#define ADS_CONFIG2__INT_TEST_4HZ 0x10
#define ADS_CONFIG2__INT_TEST_8HZ 0x11
#define ADS_CONFIG2__INT_TEST_DC  0x13

// CONFIG3: Configuration Register 3
#define ADS_CONFIG3__DEFAULT       0x30
#define ADS_CONFIG3__PD_REFBUF     0x80
#define ADS_CONFIG3__VREF_4V       0x20
#define ADS_CONFIG3__RLD_MEAS      0x10
#define ADS_CONFIG3__RLDREF_INT    0x08
#define ADS_CONFIG3__PD_RLD        0x04
#define ADS_CONFIG3__RLD_LOFF_SENS 0x02
#define ADS_CONFIG3__RLD_STAT      0x01

//LOFF: Lead-Off Control Register
#define ADS_LOFF__COMP_TH2         0x80
#define ADS_LOFF__COMP_TH1         0x40
#define ADS_LOFF__COMP_TH0         0x20
#define ADS_LOFF__VLEAD_OFF_EN     0x10
#define ADS_LOFF__ILEAD_OFF1       0x08
#define ADS_LOFF__ILEAD_OFF0       0x04
#define ADS_LOFF__FLEAD_OFF1       0x02
#define ADS_LOFF__FLEAD_OFF0       0x01
#define ADS_LOFF__COMP_TH_95       0x00
#define ADS_LOFF__COMP_TH_92_5     0x20
#define ADS_LOFF__COMP_TH_90       0x40
#define ADS_LOFF__COMP_TH_87_5     0x60
#define ADS_LOFF__COMP_TH_85       0x80
#define ADS_LOFF__COMP_TH_80       0xA0
#define ADS_LOFF__COMP_TH_75       0xC0
#define ADS_LOFF__COMP_TH_70       0xE0
#define ADS_LOFF__ILEAD_OFF_6nA    0x00
#define ADS_LOFF__ILEAD_OFF_12nA   0x04
#define ADS_LOFF__ILEAD_OFF_18nA   0x08
#define ADS_LOFF__ILEAD_OFF_24nA   0x0C
#define ADS_LOFF__FLEAD_OFF_AC     0x01
#define ADS_LOFF__FLEAD_OFF_DC     0x03

//CHnSET: Individual Channel Settings (n = 1 : 8)
#define ADS_CHSET__PD              0x80
#define ADS_CHSET__GAIN2           0x40
#define ADS_CHSET__GAIN1           0x20
#define ADS_CHSET__GAIN0           0x10
#define ADS_CHSET__MUX2            0x04
#define ADS_CHSET__MUX1            0x02
#define ADS_CHSET__MUX0            0x01
#define ADS_CHSET__GAIN_1X         0x10
#define ADS_CHSET__GAIN_2X         0x20
#define ADS_CHSET__GAIN_3X         0x30
#define ADS_CHSET__GAIN_4X         0x40
#define ADS_CHSET__GAIN_6X         0x00
#define ADS_CHSET__GAIN_8X         0x50
#define ADS_CHSET__GAIN_12X        0x60
#define ADS_CHSET__ELECTRODE_INPUT 0x00
#define ADS_CHSET__SHORTED         0x01
#define ADS_CHSET__RLD_INPUT       0x02
#define ADS_CHSET__MVDD            0x03
#define ADS_CHSET__TEMP            0x04
#define ADS_CHSET__TEST_SIGNAL     0x05
#define ADS_CHSET__RLD_DRP         0x06
#define ADS_CHSET__RLD_DRN         0x07

//PACE: PACE Detect Register
#define ADS_PACE__PACEE1            0x10
#define ADS_PACE__PACEE0            0x08
#define ADS_PACE__PACEO1            0x04
#define ADS_PACE__PACEO0            0x02
#define ADS_PACE__PD_PACE           0x01
#define ADS_PACE__PACEE_CHAN2       0x00
#define ADS_PACE__PACEE_CHAN4       0x08
#define ADS_PACE__PACEE_CHAN6       0x10
#define ADS_PACE__PACEE_CHAN8       0x18
#define ADS_PACE__PACEO_CHAN1       0x00
#define ADS_PACE__PACEO_CHAN3       0x02
#define ADS_PACE__PACEO_CHAN5       0x04
#define ADS_PACE__PACEO_CHAN7       0x06

//RESP: Respiration Control Register
#define ADS_RESP__DEFAULT           0x20
#define ADS_RESP__RESP_DEMOD_EN1    0x80
#define ADS_RESP__RESP_MOD_EN1      0x40
#define ADS_RESP__RESP_PH2          0x10
#define ADS_RESP__RESP_PH1          0x08
#define ADS_RESP__RESP_PH0          0x04
#define ADS_RESP__RESP_CTRL1        0x02
#define ADS_RESP__RESP_CTRL0        0x01
#define ADS_RESP__RESP_PH_22_5      0x00
#define ADS_RESP__RESP_PH_45        0x04
#define ADS_RESP__RESP_PH_67_5      0x08
#define ADS_RESP__RESP_PH_90        0x0C
#define ADS_RESP__RESP_PH_112_5     0x10
#define ADS_RESP__RESP_PH_135       0x14
#define ADS_RESP__RESP_PH_157_5     0x18
#define ADS_RESP__RESP_NONE         0x00
#define ADS_RESP__RESP_EXT          0x01
#define ADS_RESP__RESP_INT_SIG_INT  0x02
#define ADS_RESP__RESP_INT_SIG_EXT  0x03

//CONFIG4: Configuration Register 4
#define ADS_CONFIG4__RESP_FREQ2     0x80
#define ADS_CONFIG4__RESP_FREQ1     0x40
#define ADS_CONFIG4__RESP_FREQ0     0x20
#define ADS_CONFIG4__SINGLE_SHOT    0x08
#define ADS_CONFIG4__WCT_TO_RLD     0x04
#define ADS_CONFIG4__PD_LOFF_COMP   0x02
#define ADS_CONFIG4__RESP_FREQ_64k_Hz     0x00
#define ADS_CONFIG4__RESP_FREQ_32k_Hz     0x20
#define ADS_CONFIG4__RESP_FREQ_16k_Hz     0x40
#define ADS_CONFIG4__RESP_FREQ_8k_Hz      0x60
#define ADS_CONFIG4__RESP_FREQ_4k_Hz      0x80
#define ADS_CONFIG4__RESP_FREQ_2k_Hz      0xA0
#define ADS_CONFIG4__RESP_FREQ_1k_Hz      0xC0
#define ADS_CONFIG4__RESP_FREQ_500_Hz     0xE0

//WCT1: Wilson Central Terminal and Augmented Lead Control Register
#define ADS_WCT1__aVF_CH6           0x80
#define ADS_WCT1__aVL_CH5           0x40
#define ADS_WCT1__aVR_CH7           0x20
#define ADS_WCT1__avR_CH4           0x10
#define ADS_WCT1__PD_WCTA           0x08
#define ADS_WCT1__WCTA2             0x04
#define ADS_WCT1__WCTA1             0x02
#define ADS_WCT1__WCTA0             0x01
#define ADS_WCT1__WCTA_CH1P         0x00
#define ADS_WCT1__WCTA_CH1N         0x01
#define ADS_WCT1__WCTA_CH2P         0x02
#define ADS_WCT1__WCTA_CH2N         0x03
#define ADS_WCT1__WCTA_CH3P         0x04
#define ADS_WCT1__WCTA_CH3N         0x05
#define ADS_WCT1__WCTA_CH4P         0x06
#define ADS_WCT1__WCTA_CH4N         0x07

//WCT2: Wilson Central Terminal Control Register
#define ADS_WCT2__PD_WCTC           0x80
#define ADS_WCT2__PD_WCTB           0x40
#define ADS_WCT2__WCTB2             0x20
#define ADS_WCT2__WCTB1             0x10
#define ADS_WCT2__WCTB0             0x08
#define ADS_WCT2__WCTC2             0x04
#define ADS_WCT2__WCTC1             0x02
#define ADS_WCT2__WCTC0             0x01
#define ADS_WCT2__WCTB_CH1P         0x00
#define ADS_WCT2__WCTB_CH1N         0x08
#define ADS_WCT2__WCTB_CH2P         0x10
#define ADS_WCT2__WCTB_CH2N         0x18
#define ADS_WCT2__WCTB_CH3P         0x20
#define ADS_WCT2__WCTB_CH3N         0x28
#define ADS_WCT2__WCTB_CH4P         0x30
#define ADS_WCT2__WCTB_CH4N         0x38

#define ADS_WCT2__WCTC_CH1P         0x00
#define ADS_WCT2__WCTC_CH1N         0x01
#define ADS_WCT2__WCTC_CH2P         0x02
#define ADS_WCT2__WCTC_CH2N         0x03
#define ADS_WCT2__WCTC_CH3P         0x04
#define ADS_WCT2__WCTC_CH3N         0x05
#define ADS_WCT2__WCTC_CH4P         0x06
#define ADS_WCT2__WCTC_CH4N         0x07
//=============================================================================
// ADS1298 Helper
//=============================================================================
void ADSDelay(U8);
void ADSInit();
void ADSCommand(U8);
void ADSWrite(U8, U8);
U8 ADSRead(U8);
//=============================================================================
// SPI Helper
//=============================================================================

// In_packet 8 bytes
// -------------------------------------------------------------
// |  1 byte     | 1 byte     | 1 byte     | ...
// -------------------------------------------------------------
// | CEX0_INT    | SPI_RX_CNT | SPI_TX_CNT | ...
// -------------------------------------------------------------
#define INEP0_ADC_RDY_CNT 0
#define INEP0_SPI_RX_CNT  1
#define INEP0_SPI_TX_CNT  2

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
extern U8 readySPI;             // flag 8 bit for SPI Communication
extern U8 countADC_RDY;         // count for ADC_RDY triggers
extern U8 bufferSPI;
extern volatile U8 countDelay;  // count for timer3 interrupts
#define READY_SPI_END  1        // flag to trigger NSS
#define READY_ADC_RDY  2        // flag to trigger Send SPI Read to AFE
#define READY_SPI_READ 4        // flag to trigger SPI0_ISR read SPI0DAT

//-----------------------------------------------------------------------------
// Define macro
//-----------------------------------------------------------------------------
// WRITE_SPI(b)
//    wait for busy transmit holding register and write a byte to SPI
//-----------------------------------------------------------------------------
#define WRITE_SPI(b) while(!SPI0CN_TXBMT); SPI0DAT = (b);
#define READ_SPI(b) while(!SPI0CN_TXBMT); \
      while((SPI0CFG & SPI0CFG_SPIBSY__SET)); readySPI |= READY_SPI_READ; SPI0DAT = 0; \
      while(readySPI & READY_SPI_READ); b = bufferSPI;
#define SPI_START() SPI0CN_NSSMD0 = 0;
#define SPI_END() while(!SPI0CN_TXBMT); while(SPI0CFG & SPI0CFG_SPIBSY__SET); TMR2CN_TR2 = 1;

#endif /* SPI0_TI_H_ */
