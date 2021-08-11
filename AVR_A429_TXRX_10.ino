  // AVR_A429_TXRX_10
/*
  Make it compatible with ATMEL Studio.
  Do not use Arduino functions.
  Make Arduino libraries compatible with ATMEL Studio
  1º. Rename file to more meaningfull manes
  2º. Arrange #defines from Arduino pins to Mega 2560 Port Pins
  3º. Change Arduino functions for ATMEL "C" functions.
  4º. Checked TX all label one at a time OK.
  5º. Checked refresh time O.K.
  6º. Change symmetric keyboard by software.
  7º. ALT_KEY light stays on when is active.
  8º. Edition of Label.
  9º. Edition of SDI, SSM and Data.
  10º. Menu revision, ordering code. OK.
  11º. TX Programming mostly all okay. (Label 377 gives error)
  12º. When receives a label already stored into RX Buffer, over writes thenew data,SDI,SSM.So updates values instead  
  13º. Label"000",not available. Because labeland funtions returning label,are defined as uint8_t, return label=0,is used as error code.  
  14º.Programed refresh time at 50msec steps.
*/

/*  ///////////////////////////////// Changed files XXX.h names ////////////////////
  #include "My_Define.h"
  #include "My_Pulses.h"
  #include "My_Interrupts.h"
  #include "DEI1016_Tools.h"
  #include "My_Menu.h"
*///////////////////////////////////////////////////////////////////////

#include "A429_Define.h"
#include "A429_Pulses.h"
#include "A429_Interrupts.h"
#include "DEI1016_Tools.h"
#include "A429_Menu.h"

#include <stdio.h> // for function sprintf

unsigned long  My_Loop = 0;
unsigned long My_FIFO_Empty = 0;

ISR(TIMER4_COMPA_vect) //Timer4A interrupt 20Hz (50msec)
{
  count_50ms++;
  TX_ON = true;
  Load_DEI_TX_Buffer(); // Function to Fill DEI1016 TX Buffer
  TX_ON = false;
}

int Home_Menu_Times = 0; //For debugging

void setup(void)
{
  Serial.begin(9600); //For Testing

  ///////////// IN/OUT Pins setup //////////////
  //pinMode(SEL_WORD_1_2, OUTPUT); // DI22 >> PA0
  DDRA |= (1 << PA0);  // OUTPUT;

  //pinMode(ENABLE_RX1, OUTPUT); //DI23 >> PA1
  DDRA |= (1 << PA1);  // OUTPUT;

  //pinMode(ENABLE_RX2, OUTPUT); //DI24 >> PA2
  DDRA |= (1 << PA2);  // OUTPUT;

  //pinMode(INT_RX1, INPUT_PULLUP); //DI02 >> PE4
  DDRE &= ~(1 << PE4);  // INPUT;
  PORTE |= (1 << PE4);  // PULLUP

  //pinMode(INT_RX2, INPUT_PULLUP);  //DI03 >> PE5
  DDRE &= ~(1 << PE5);  // INPUT;
  PORTE |= (1 << PE5);  // PULLUP

  //pinMode(ALT_KEY, OUTPUT); // DI08 >> PH5 .To signal LED ALT key for C=E, D=F
  DDRH |= (1 << PH5);  // OUTPUT;

  //pinMode(HI_OPEN_PARITY, OUTPUT); //DI07 >> PH4. -DBCEN, HI /OPEN Parity
  DDRH |= (1 << PH4);  // OUTPUT;

  //pinMode(MASTER_RESET, OUTPUT);  //DI29 >> PA7 MASTER RESET  -MR
  DDRA |= (1 << PA7);  // OUTPUT;

  //pinMode(LOAD_CTRL_REG, OUTPUT);  //DI28 >> PA6 . -LDCW, LOAD CONTROL REGISTER
  DDRA |= (1 << PA6);  // OUTPUT;

  //pinMode(ENABLE_TX, OUTPUT);  //DI27 >> PA5. ENTX,  ENABLE TX
  DDRA |= (1 << PA5);  // OUTPUT;

  //pinMode(TXR_FIFO_EMPTY, INPUT); //DI18 >> PD3, TXR,  FIFO EMPTY
  DDRD &= ~(1 << PD3);  // INPUT;

  //pinMode(LOAD_WORD_1, OUTPUT); //DI 25 >> PA3, -LD1 >> LOAD WORD 1
  DDRA |= (1 << PA3);  // OUTPUT;

  //pinMode(LOAD_WORD_2, OUTPUT);  //DI26 >> PA4, -LD2 >> LOAD WORD 2
  DDRA |= (1 << PA4);  // OUTPUT;

  //pinMode(PARITY_ODD_LED, OUTPUT);  // DI04 >> PG5,   OUTPUT: LED PARITY ODD LED
  DDRG |= (1 << PG5);  // OUTPUT;

  //pinMode(PARITY_EVEN_LED, OUTPUT); // DI05 >> PE3,  OUTPUT: LED PARITY EVEN LED
  DDRE |= (1 << PE3);  // OUTPUT;

  //pinMode(RX_HIGH_LED, OUTPUT);     // DI12 >> PB6 OUTPUT: LED RX_HIGH_LED
  DDRB |= (1 << PB6);  // OUTPUT;

  //pinMode(TX_HIGH_LED, OUTPUT);     // DI13 >>  PB7, OUTPUT: LED TX_HIGH_LED
  DDRB |= (1 << PB7);  // OUTPUT;

  //pinMode(BD429_RELAY, OUTPUT); // DI09 >> PH6, OUTPUT ON for RELAY 4066 to connect 470pF parallel with 75 pF for LOW Speed
  DDRH |= (1 << PH6);  // OUTPUT;

  //pinMode(scrollINT, INPUT_PULLUP); //  Not used

  /////////////////ISR Interrupts Service Routines Set Up///////////////////////
  attachInterrupt(digitalPinToInterrupt(INT_RX1), ISR_Int_RX1, FALLING);
  attachInterrupt(digitalPinToInterrupt(INT_RX2), ISR_Int_RX2, FALLING);
  //attachInterrupt(digitalPinToInterrupt(scrollINT), ISR_Scroll, FALLING); // Not used
  attachInterrupt(digitalPinToInterrupt(TXR_FIFO_EMPTY), ISR_TX_Buffer_Empty, RISING);

  delay(100);

  /////////////// Set UP Discretes Initial State /////////////////

  Master_Reset_Default(); //Make sure reset is OFF
  delay(100);
  Load_Control_Register_OFF();
  delay(100);
  //Select_Word_1_Read(); //SEL_LOW WORD_1. DI22
  Select_Word_2_Read(); //SEL_HIGH WORD_2. DI22
  delay(100);
  Disable_RX_1(); //OE1. Disable  OE1
  Disable_RX_2(); //OE2. Disable OE2
  Disable_TX(); //ENTX. Pin DI27 >> PA5.
  delay(100);
  Set_Parity_High(); //DBCEN. Pin: DI07 >> PH4
  Deselect_Word_1_Load(); // LD1
  Deselect_Word_2_Load(); //LD2

  /////////LOAD CONTROL REGISTER //////////////////////
  Control_Word = 0x0000; // Reset Control_Word;
  Control_Word = (PAREN | SLFTST | PAR_ODD | TX_LO | RX_LO | WORD_32); // Enables Wrap Around Selftest "SLFTST"
  //Control_Word = (PAREN | NOT_SLFTST | PAR_ODD | TX_LO | RX_LO | WORD_32); // Disenables Wrap Around Selftest "SLFTST"
  //Control_Word = (PAREN | NOT_SLFTST | PAR_ODD | TX_HI | RX_HI | WORD_32); // Disenables Wrap Around Selftest "SLFTST"

  Load_Control_Word(Control_Word);  //Load Control Word
  delay(100);

  //////////////////// Show settings set by Load_Control_Word(Control_Word)/////////////////////
  //digitalWrite(PARITY_ODD_LED, HIGH);   //LED PARITY ODD LED DI04 >> PG5,
  PORTG |= (1 << PG5);
  //digitalWrite(PARITY_EVEN_LED, LOW);   // LED PARITY EVEN LED DI05 >> PE3,
  PORTE &= ~(1 << PE3);
  
  //digitalWrite(TX_HIGH_LED, LOW); // DI13 >> PB7
  PORTB &= ~(1 << PB7);// TX HI LED OFF
  //digitalWrite(BD429_RELAY, HIGH); // Relay BD429 ON for TX_LOW;  DI09 >> PH6
  PORTH |= (1 << PH6); // Relay BD429 ON for TX_LOW;  DI09 >> PH6
  //PORTH &= ~(1 << PH6); //Prueba Carlos 03-08-2021
  
  ///////////////////////////////////////////////////////////////////////////////////////
  
  Display_Keypad_Setup(); //Functuion from Finite_State_Menu_A429_I2C_2

  delay(1000);

  // Print INITIAL  message to the LCD
  Print_Initial_Message();
  //Display_RX_HEX_Header_Screen();// Default

  ////////// TX BUFFER    //////////////////

  Fill_Testing_TX_Array(); // Load a dummy Labels to TX_Buffer nfor testing. Normal program labels will be loaded from Keypad entries.
  Setup_TX_Buffer();  //This function will setup WORD_1 and WORD_2, taking the data already stored in each A429 structure of the A429 Array.

  /////////////// SETUP: 1 Mhz DEI1016 Clock and Timer for A429 label refresh period //////////////////////////////////

  Init_Timers();
}

void loop()
{
  //Check RX Flag
  switch (State) //Check for Digital Input
  {
    case State_Home:
      Home_Menu();
      break;

    case State_TX_Display:
      TX_Display();
      break;
    
    case State_Test:
      Test();
      break;

    case State_Misc:
      Misc();
      break;

    case State_Help:
      Help();
      break;

    case State_TX_SPEED_PARITY_Programming:
      TX_SPEED_PARITY();
      break;

    case State_TX_Menu:
      TX_Menu();
      break;

    case State_Programming_TX:
      TX_Programming_Menu();
      break;

    case State_Programming_Label:
      Programming_Label();
      break;

    case State_Programming_SDI:
      Programming_SDI();
      break;

    case State_Programming_Data:
      Programming_Data();
      break;

    case State_Programming_SSM:
      Programming_SSM();
      break;

    case State_TX_Display_HEX:
      TX_Display_HEX();
      break;

    case State_TX_Display_BIN:
      TX_Display_BIN();
      break;

    case State_TX_Display_ENG:
      TX_Display_ENG();
      break;

    case State_RX_Display:
      RX_Display();
      break;

    case State_RX_SPEED_PARITY_Programming:
      RX_SPEED_PARITY();
      break;

    case State_RX_Display_HEX:
      RX_Display_HEX();
      break;

    case State_RX_Display_BIN:
      RX_Display_BIN();
      break;

    case State_RX_Display_ENG:
      RX_Display_ENG();
      break;

    default:
      lcd.clear();
      lcd.setCursor(0, 1);
      lcd.print(" INVALID ENTRY !!!!");
      delay(2000);
      Draw_Home_Menu();
      State = State_Home;

  }//switch
}//loop()


/////////////////// OLD CODE /////////////////////////////
/* UNUSED
case State_TX_Menu_Home:
  TX_Menu();
  break;

case State_RX_Menu_Home:
  RX_Display();
  break;
*/
