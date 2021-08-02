//  My_Define for DEI1016_Tools for A429_Test_hardware_6
#ifndef MY_DEFINE_H
#define MY_DEFINE_H
//#include "Arduino.h"

///////////////////////////////// Define moved from DEI1016_Tools //////////////////////
#define PAREN     0x0010  // PARity ENable. CONTROL WORD_1
#define SLFTST    0x0000  // SeLF TeST. PARity ENable. CONTROL WORD_1
#define NOT_SLFTST 0x0020  // Not SeLF TeST. PARity ENable. CONTROL WORD_1
#define PAR_ODD   0x0000  // PARity ChecK. CONTROL WORD_2
#define PAR_EVEN  0x1000  // PARity ChecK. CONTROL WORD_2
#define TX_LO      0x2000  // TX (transmit). LOw speed (8-12,5 KHz). CONTROL WORD_2. 100usec Period
#define TX_HI      0x0000  // TX (transmit) HIgh speed (100 KHz). CONTROL WORD_2. 10usec Period
#define RX_LO      0x4000  // RX (receive)  LOw speed. CONTROL WORD_2
#define RX_HI      0x0000   // RX (receive)  HIgh speed. CONTROL WORD_2
#define WORD_32    0x0000   // 32 bits
#define WORD_25    0x8000   //25 bits
#define Max_A429_RX_Buffer 12    // Maximun Buffer RX. Should not be  > 64 because using "uint8_t" to scroll RX Buffer
#define Max_A429_TX_Buffer 8    // Maximun Buffer TX in DEI1016.  
 
/////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////// DEI 1016  1 MHz Clock ////////////////////////////////////////////////////
//Este programa genera dos salidas de Clock de Mhz, en los pines 11 y 12 del Arduino UNO, con un Duty Cycle del 50%, con el Timer1.
//Test MEGA 2560 for 1 Mhz clock DEI 1016 TX/RX ARINC 429 Project
#define pwmPin_11  11 // DI11 > PB5, For testing PWM. 1Mhz Clock for DEI 1016 PIN 37 MCK 
//#define pwmPin_12  12 // NOT USED,For Testing PWM. 1Mhz Clock to check without Trigger Schmitt.
#define MCK 11   //DI11 > PB5, PWM. 1Mhz Clock for DEI 1016 PIN 37 1MCK to Trigger Schmitt

/////////////////////////////////////////// INTERRUPTS ////////////////////////////////////////////////////
#define INT_RX1  2 // DI02 >>PE4, INT-0. -DR1 'LOW'. Interrupt when ARINC 420 word on RX1. ISR should be attached. Logic Output. Data Ready, Receiver 1. A Low output indicates valid data in receiver 1.
#define INT_RX2  3 // DI03 >> PE5, INT-1. -DR2. 'LOW' Interrupt when ARINC 420 word on RX2. ISR should be attached.Logic Output. Data Ready, Receiver 2. A Low output indicates valid data in receiver 2.
#define TXR_FIFO_EMPTY 18  //DI18 >> PD3,  INT-5. TXR 'HI'.  Interrupt when TX Buffer is empty 'HI'. ISR shuold be attached to flag for TX Buffer to be load again.
//#define SCROLL_INT 19 // ANULADO.  INT-4.'LOW'. External switches Scroll TX & RX. To manage scrolling on RX and TX displays.

//////////////////////////////////////////MEGA 2560 OUTPUTS to DEI 1016 ////////////////////////////////////////////////////
//#define SEL_WORD_1_2 22 //OUTPUT.DI22 >> PA0 >>  To select on DEI 1016 to load WORD_1 op WORD_2.
#define SEL_WORD_1_2 PA0 //OUTPUT.>> PA0 >>  SELECT, WD1/WD2 SEL, >> To select on DEI 1016 to load WORD_1 op WORD_2.

#define ENABLE_RX1 23 // DI23 >> PA1, OUTPUT. OE1. Enable receiver RX_1 on DEI 1016.
#define ENABLE_RX2 24 // DI24 >> PA2, OUTPUT. OE2. Enable receiver RX_2 on DEI 1016.
#define LOAD_WORD_1 25 // DI25 > PA3, OUPUT Signal to ENABLE WORD_1 Load into DEI1016
#define LOAD_WORD_2 26 // DI26 > PA4, OUPUT Signal to ENABLE WORD_2 Load into DEI1016
#define ENABLE_TX 27 // DI 27 > PA5, OUTPUT to enable Transmit ARINC 429 LABELS from TX Buffer.
#define LOAD_CTRL_REG 28 // DI28 > PA6, OUTPUT 
#define MASTER_RESET 29 // DI29 > PA7, OUTPUT 
#define HI_OPEN_PARITY  07  //  DI07 > PH4, OUTPUT: /DBCEN.  To be checked behaviour during tests 
#define PARITY_ODD_LED  04  //  DI04 > PG5, OUTPUT: PARITY ODD LED
#define PARITY_EVEN_LED  05 //  DI05 > PE3, OUTPUT: PARITY EVEN LED
#define RX_HIGH_LED  12 //  DI12 > PB6, OUTPUT: RX_HIGH_LED
#define TX_HIGH_LED  13 //  DI13 > PB7, OUTPUT: TX_HIGH_LED. PB7
#define BD429_RELAY 9 //  DI09 > PH6, OUTPUT: RELAY ON for TX_LOW >> DI09 >> PH6
#define ALT_KEY    8  // DI08 > PH5, OUTPUT LED to Show when ALT key (*) has been pressed to make C = E and D = F, on Keypad so to have hex keypad. LED should  
//#define ALT_KEY 8  //LED para indicar que esta presionada la tecla "ALT"

////////////////////////////////////////// ARINC 429 //////////////////////////////////////////////////////////////////
////////////////////////////////////////// MEGA 2560 DATA to DEI 1016  WORD-1, WORD-2 /////////////////////////////////

#define DATA_0 49 // PORT_L0. IN/OUT Data Word Bit 0. For data interchange between MEGA 2560 & DEI 1016.
#define DATA_1 48 // PORT_L1. IN/OUT Data Word Bit 1. For data interchange between MEGA 2560 & DEI 1016.
#define DATA_2 47 // PORT_L2. IN/OUT Data Word Bit 2. For data interchange between MEGA 2560 & DEI 1016.
#define DATA_3 46 // PORT_L3. IN/OUT Data Word Bit 3. For data interchange between MEGA 2560 & DEI 1016.
#define DATA_4 45 // PORT_L4. IN/OUT Data Word Bit 4. For data interchange between MEGA 2560 & DEI 1016.
#define DATA_5 44 // PORT_L5. IN/OUT Data Word Bit 5. For data interchange between MEGA 2560 & DEI 1016.
#define DATA_6 43 // PORT_L6. IN/OUT Data Word Bit 6. For data interchange between MEGA 2560 & DEI 1016.
#define DATA_7 42 // PORT_L7. IN/OUT Data Word Bit 7. For data interchange between MEGA 2560 & DEI 1016.
#define DATA_8 37 // PORT_C0. IN/OUT Data Word Bit 8. For data interchange between MEGA 2560 & DEI 1016.
#define DATA_9 36 // PORT_C1. IN/OUT Data Word Bit 9. For data interchange between MEGA 2560 & DEI 1016.
#define DATA_10 35 // PORT_C2. IN/OUT Data Word Bit 10. For data interchange between MEGA 2560 & DEI 1016.
#define DATA_11 34 // PORT_C3. IN/OUT Data Word Bit 11. For data interchange between MEGA 2560 & DEI 1016.
#define DATA_12 33 // PORT_C4. IN/OUT Data Word Bit 12. For data interchange between MEGA 2560 & DEI 1016.
#define DATA_13 32 // PORT_C5. IN/OUT Data Word Bit 13. For data interchange between MEGA 2560 & DEI 1016.
#define DATA_14 31 // PORT_C6. IN/OUT Data Word Bit 14. For data interchange between MEGA 2560 & DEI 1016.
#define DATA_15 30 // PORT_C7. IN/OUT Data Word Bit 15. For data interchange between MEGA 2560 & DEI 1016.


//////////////////////////////////////////MEGA 2560 INPUTS ////////////////////////////////////////////////////

//#define TX_SCROLL  5  // INPUT (PULL-UP). To check for TX Display scrolling.
//#define RX_SCROLL  4  // INPUT (PULL-UP). To check for RX Display scrolling.

//#define Mask_Scroll_TX  0x00100000 // PG5
//#define Mask_Scroll_RX  0x00001000 // PE3

#define BounceDelay 1000

#endif
