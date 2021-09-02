//DEI1016_Tools for A429_Test_hardware_6
#ifndef MY_DEI1016_TOOLS_H
#define MY_DEI1016_TOOLS_H
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include "Arduino.h"
#include "Delay.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "A429_Interrupts.h"
#include "A429_Display_Tools.h"
#include "A429_Define.h"
#include "A429_Pulses.h"
#include "A429_Menu.h"
#include "A429_ENG.h"


///////////////////////////////// Changed files XXX.h names ////////////////////
//#include "My_Interrupts.h"
//#include "My_TX_Edit.h"
//#include "My_Define.h"
//#include "My_Pulses.h"
//#include "My_Menu.h"

// Now define several timer values for communicating with the DEI 1016:
// These are in nano seconds.
const unsigned int TMR = 200;  //Master Reset
const unsigned int Tpwld = 130; // /LD1, /LD2 and /LDCW Pulse Width. 130nsec MIN
const unsigned int Tsdw = 110; // Data to /LD⇑ Set-Up Time. 110nsec MIN
const unsigned int Tddr = 200; //Delay /OE⇓ to valid data. 200nsec MAX
const unsigned int Tssel = 20; // SEL to /OE⇓ to valid data. 20nsec MIN
const unsigned int Thsel = 20; //SEL to /OE⇑ hold time. 20nsec. MIN
const unsigned int Tpwoe = 200; ///OE1 or /OE2 Pulse Width. 200nsec. MIN
const unsigned int Toeoe = 50; // Delay between consecutive /OE pulses. 50 nsec
const unsigned int Tdoedr = 200; //Delay, 2nd /OE⇑ to /DRn⇑. 200nsec MAX

/////////////////// CONTROL WORD DEFAULTS ////////////////////////
unsigned int Control_Word = 0x0000;
unsigned int TX_Parity_Enable_ON = 0x0000;  //Bit 4, PAREN "1" Parity Enable.
unsigned int Self_Test_Enable_OFF = 0x0000;  // Bit 5  SLFTST "0" Enables Wrap Around.
unsigned int RX1_SDI_Code_Check_ON = 0x0000; // Bit 6 SDEN1 "1" Enables Check RX1 SDI
unsigned int X1Y1_SDI_Codes_RX1_Check = 0x0000;  // SDI Bits to check X1 (D7): ARINC Bit 9, Y1(D8): ARINC Bit 10.
unsigned int RX2_SDI_Code__Check_ON = 0x0000; // Bit 9 SDEN1 "1" Enables Check RX2 SDI
unsigned int X2Y2_SDI_Codes_RX1_Check = 0x0000;  // SDI Bits to check X1 (D10): ARINC Bit 9, Y1(D11): ARINC Bit 10.
unsigned int Parity_Check = 0x0000; //  PARCK. Bit 12. Logic "0" ODD Parity (Normal), "1" EVEN Parity
unsigned int TX_Data_Rate = 0x0000; // TXSEL. Bit 13. Logic "0" High Rate 100KHz, Logic "1" Low Rate: 12,5 KHz
unsigned int RX_Data_Rate = 0x0000; // RCVSEL. Bit 14. Logic "0" High Rate 100KHz, Logic "1" Low Rate: 12,5 KHz
unsigned int Word_Lenght = 0x0000;  // WLSEL. Bit 15. Logic "0" 32 bits, Logic "1" 25 bits.

// Bits: B0 >> B3, Not used.
unsigned int Control_Word_Default = 0x6010; //LO. D4=1 TX Parity Enable, D5 = 0 Wrap Around Enable,D6 >> D11 = 0SDI Do not check , D12 = 0 TX Parity to ODD, D13 = 1 TX LO Rate 12,5 KHz,
//D14 = 1 RX LO Rate 12,5 KHz, D15 = 0 Word Lenght = 32.
//unsigned int Control_Word_Default = 0x0010; //HI. D4=1 TX Parity Enable, D5 = 0 Wrap Around Enable,D6 >> D11 = 0SDI Do not check , D12 = 0 TX Parity to ODD, D13 = 1 TX LO Rate 12,5 KHz,
//D14 = 1 RX LO Rate 12,5 KHz, D15 = 0 Word Lenght = 32.
//unsigned int Control_Word_Default = 0x1234; // D4=1 TX Parity Enable, D5 = 0 Wrap Around Enable,D6 >> D11 = 0SDI Do not check , D12 = 0 TX Parity to ODD, D13 = 1 TX LO Rate 12,5 KHz,
//D14 = 1 RX LO Rate 12,5 KHz, D15 = 0 Word Lenght = 32.

/////////////////// From A429_Interrupts //////////////////////
volatile byte INT_RX1_Ready = false; //Flag for INT_RX1. To be set 'true' for ISR and reset to 'false' when serviced into loop().
volatile byte Int_RX2_Ready = false; //Flag for INT_RX2. To be set 'true' for ISR and reset to 'false' when serviced into loop().
volatile int TX_Buffer_Busy = false; // Flag to not allow further Load_TX_Buffer(); while is loading the TX_Buffer;
volatile int RX1_Buffer_Busy = false; // Flag to not allow further RX_Buffer(); while is working in the RX1_Buffer;
volatile byte Flag_TX = false;
volatile byte Flag_RX = false;
volatile byte  TX_ON = false;  // On every 50msec.
volatile int  Buffer_Flag_Interrupts = 0;
volatile byte INT_DEI_TX_Buffer_Empty_Flag = false; //To Flag when TX Buffer is empty. This Flag in the loop(), will call to Load_DEI_TX_Buffer() if it not already busy: "TX_Buffer_Busy"
//To be set 'true' for ISR and reset to 'false' when serviced into loop().
volatile unsigned int Times_TX_Buffer_Busy = 0; //Debug how many times ISR TX Buffer empty while TX_Buffer still loading
volatile unsigned int ISR_RX1_Times = 0;   // Flag for Auto Testing wrap around FLAG Testing the TX/RX. After 20 RX1 INT's Buffer_TX[0]is increased by 1.
volatile unsigned int ISR_RX1_Count = 0;   // Debug FLAG, how many times ISR RX2 is called.
volatile unsigned int ISR_RX2_Times = 0;   // Flag for Auto Testing wrap around FLAG Testing the TX/RX. After 20 RX1 INT's Buffer_TX[0]is increased by 1.
volatile unsigned int ISR_RX2_Count = 0;   // Debug FLAG, how many times ISR RX2 is called.


uint8_t Menu_Test = false;// Flag to activate when Menu TEST is selected. TX_Buffer.data [0] will be incremented  every ISR_RX1_Times == 20


//////////////////// Delays Functions ///////////////////
static __inline__ void Delay_50nsec() __attribute__((always_inline));
static __inline__ void Delay_100nsec() __attribute__((always_inline));
static __inline__ void Delay_200nsec() __attribute__((always_inline));
static __inline__ void Delay_300nsec() __attribute__((always_inline));
//static __inline__ void _delay_loop_1_x( uint8_t) __attribute__((always_inline));
//asm volatile ("nop\n\t"); __attribute__((always_inline)

///////////// LCD Functions ////////////////////
void Print_Initial_Message();
void LCD_Clear_Line(uint8_t line);

/////////////////  RX Functions ///////////////////////////
void Display_RX_HEX_Header_Screen(); //Screen Fields layout for data.Shows SDI, Refresh time instead of Tms
void Display_RX_BIN_Header_Screen(); //Screen Fields layout for data.
void Display_RX_ENG_Header_Screen(); //Screen Fields layout for data.

void RX_Display_A429_HEX_Show_Data(); //Print Data of each label on screen. Taking in to account the fields set by Display_XX_YYY_Header_Screen().
void RX_Display_A429_BIN_Show_Data();//Print Data of each label on screen. Taking in to account the fields set by Display_XX_YYY_Header_Screen().
void RX_Display_A429_ENG_Show_Data();//Print Data of each label on screen. Taking in to account the fields set by Display_XX_YYY_Header_Screen().

void RX_Display_A429_HEX();//Draw Header_Screen, Show_data, Get_RX_Data when flag ISR_RX == true, Arrange_Data WORD_1/_2 into ARINC_429_STRUCT  fields:Data,SDI, SSM,
//Manage Scroll and exit.
void RX_Display_A429_BIN();//Draw Header_Screen, Show_data, Get_RX_Data when flag ISR_RX == true, Arrange_Data WORD_1/_2 into ARINC_429_STRUCT  fields:Data,SDI, SSM,
//Manage Scroll and exit.
void RX_Display_A429_ENG();//Draw Header_Screen, Show_data, Get_RX_Data when flag ISR_RX == true, Arrange_Data WORD_1/_2 into ARINC_429_STRUCT  fields:Data,SDI, SSM,
//Manage Scroll and exit.

/////////////////  TX Functions ///////////////////////////
// Not available TX for ENG.   Engineering data only available on RX.
void Display_TX_HEX_Header_Screen(); //Screen layout for data.
void Display_TX_BIN_Header_Screen(); //Screen layout for data.
void TX_Display_A429_HEX_Show_Data(); //Print Data of each label on screen. Taking in to account the fields set by Display_XX_YYY_Header_Screen()
void TX_Display_A429_BIN_Show_Data();//Print Data of each label on screen. Taking in to account the fields set by Display_XX_YYY_Header_Screen()
void TX_Display_A429_HEX(); //Draw Header_Screen, Show_data, Get_RX_Data when flag ISR_RX == true, Arrange_Data WORD_1/_2 into ARINC_429_STRUCT  fields:Data,SDI, SSM, manage Scroll and exit
void TX_Display_A429_BIN(); //Draw Header_Screen, Show_data, Get_RX_Data when flag ISR_RX == true, Arrange_Data WORD_1/_2 into ARINC_429_STRUCT  fields:Data,SDI, SSM, manage Scroll and exit

///////////////////  DEI1016 Tools prototypes ////////////////////////
void Init_Timers();
void Check_50msec_Timer();
void Data_Bus_Output();
void Data_Bus_Input();
unsigned int Read_Data_Word();
void Write_Data_Word(unsigned int data_word);
void Master_Reset_Default();
void Master_Reset();
void Select_Word_1_Read();
void Select_Word_2_Read();
void Select_Word_1_Load();
void Select_Word_2_Load();
void Deselect_Word_1_Load();
void Deselect_Word_2_Load();

///////////Control Register Functions//////////////
void Set_Parity_High();
void Set_Parity_Low();

void Load_Control_Register_ON();
void Load_Control_Register_OFF();
void Load_Control_Word (unsigned int Ctrl_Word);

///////////TX Functions//////////////
void Enable_TX();
void Disable_TX();
void Setup_TX_Buffer();
void Fill_Testing_TX_Array();
void Load_DEI_TX_Buffer();

///////////RX Functions//////////////
void Enable_RX_1();
void Enable_RX_2();
void Disable_RX_1();
void Disable_RX_2();
void Get_RX1_Data();
void Get_RX2_Data();
void Arrange_RX_Buffer();
void RX_Clear_Buffer();
void RX_Clear_Buffer_Position (uint8_t Buff_Index);

//void Do_Reset();

//storage variables for Timer1 (1 MHz) DEI 1016 Clock and Timer4 (50 msec) for label refresh time.
volatile uint32_t long count_50ms = 0;
uint32_t last_count_50ms = 0;
uint32_t last_millis = 0;
uint32_t actual_millis;

struct ARINC429 {
  uint8_t A429_Label; //Coded in octal
  uint8_t A429_SDI;
  uint32_t A429_Data;
  uint8_t A429_SSM;
  boolean A429_Parity;
  uint32_t A429_Word;  // Use not decided yet.
  uint16_t DEI1016_Word_1;  //To hold WORD_1.
  uint16_t DEI1016_Word_2;  //To hold WORD_2.
  uint8_t TX_Refresh_50ms = 0; //default value. How many blocks of 50ms have to be elapsed for label been xmit
  uint8_t Last_TX_Refresh_50ms = 0; // Last TX time. How many blocks of 50ms have been elapsed since last label xmit. When Last_TX_Refresh_50ms == TX_Refresh_50ms --> xmit Label
  uint16_t RX_Refresh_50ms = 0; // How many blocks 50ms have elapsed since the last time label have been received
  uint32_t Last_RX_count_50ms = 0;  // Updated to actual count_50ms, when Label is been received.
  //To calculate RX refresh time: RX_Refresh_50ms --> RX_Refresh_50ms = (count_50ms (actual value) - Last_RX_count_50ms) *50.
};

//ARINC429 A429_TX_Buffer [8]; // To keep the ARINC 429 words entered by keypad.
ARINC429 A429_TX_Buffer [Max_A429_TX_Buffer]; // To keep the ARINC 429 words entered by keypad
ARINC429 A429_RX_Buffer [Max_A429_RX_Buffer]; // To keep the ARINC 429 words received.

unsigned int A429_TX_Buffer_Write_Index = 0;
unsigned int A429_TX_Buffer_Read_Index = 0;
unsigned int A429_RX_Buffer_Write_Index = 0;// Keep track of where to store next label
//unsigned int A429_RX_Buffer_Read_Index = 0; // Not Used// Keep track of last label Read in order to show on LCD
unsigned int A429_RX_Buffer_Arrange_Index = 0;// Keep track of labels already arranged
//unsigned int A429_RX_Display_Index = 0;//Not used
uint8_t RX_Scroll_Index = 0;  //Keep track of what ARINC 429,data of RX Buffer,is been shown. To be increased by keys "1" FWD and "7" AFT and increase RX_Scroll_Index
uint8_t TX_Scroll_Index = 0;  //Keep track of what ARINC 429,data of TX Buffer,is been shown//To be increased by keys "1" FWD and "7" AFT and increase TX_Scroll_Index
uint8_t Header_A429_HEX = false; // To print Header only first time.
uint8_t Header_A429_ENG = false; // To print Header only first time.
uint8_t Header_A429_BIN = false; // To print Header only first time.

//////////////////  HELP  ////////////////////////////
int8_t Help_Index = 0; // Global pointer to keep track of Help strings.Note should be signed forboundaries checking
const uint8_t Help_Index_Max = 35; // Global pointe r tokeep track of Help strings.

char Help_Strings[36][20] = {
  //1º Menu page
  "HELP Keyboard   1/9",
  "Scroll keys: 1 & 7 ",
  "1>FWD, 7>AFT       ",
  "*>Exit Key         ",

  //2º Menu page
  "HELP Keyboard   2/9",
  "#>Enter key        ",
  "*>ALT Key -> LED ON",
  "ALT+C = E,ALT+D = F",

  //3º Menu page
  "HELP Home Menu  3/9",
  "1>TX Menu:         ",
  "Allows TX Label    ",
  "Editing and Display",

  //4º Menu page
  "HELP Home Menu  4/9",
  "4>TX Display:      ",
  "Actual TX Buffer   ",
  "Modes: HEX,BIN,ENG ",

  //5º Menu page
  "HELP Home Menu  5/9",
  "7>RX Display:      ",
  "Actual RX Buffer   ",
  "Modes: HEX,BIN,ENG ",

  //6º Menu page
  "Display on ENG  6/9",
  "Mode will show data",
  "in ENG units or HEX",
  "if Label not apply ",

  //7º Menu page
  "HELP Home Menu  7/9",
  "2>TEST:     ",
  "Wrap-Around Test   ",
  "TX[0] increases +1 ",

  //8º Menu page
  "HELP Home Menu  8/9",
  "Before select TEST ",
  "Disconnect any  ",
  "RX and TX hardware ",

  //9º Menu page
  "HELP Home Menu  9/9",
  "5>RESET:           ",
  "Software Reset     ",
  "IC A429 DEI-1016   ",

};

/*
  void Do_Reset() {
  asm volatile ("jmp 0"); // Do RESET from software
  }
*/

///////////// LCD Functions ////////////////////
void LCD_Clear_Line(uint8_t line) {
  uint8_t i = line;
  lcd.setCursor(0, i);
  lcd.print("                    ");
}
void Print_Initial_Message() {

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.home();
  lcd.setCursor(0, 0);
  lcd.print("-------CITHE--------");
  delay(500);
  lcd.setCursor(0, 1);
  lcd.print("ARINC 429 TX-RX");
  delay(500);
  lcd.setCursor(0, 2);
  lcd.print("Numero Serie:001");
  delay(500);
  lcd.setCursor(0, 3);
  lcd.print("SW Rev.0, 08/2021");
  delay (5000);
}

//////LCD TX FUNCTIONS///////////////

void Display_TX_HEX_Header_Screen() {
  // Print menu TX to the lcd.

  TX_Scroll_Index = 0; //Reset Scroll Index

  lcd.clear();
  lcd.home();
  lcd.setCursor(0, 0);
  lcd.print("TX");
  lcd.setCursor(3, 0);
  lcd.print("LAB");
  lcd.setCursor(7, 0);
  lcd.print("SDI");
  lcd.setCursor(11, 0);
  lcd.print("DATA");
  lcd.setCursor(17, 0);
  lcd.print("SSM");
  delay (1000);
}

void Display_TX_BIN_Header_Screen() {
  // Print menu TX to the lcd.

  TX_Scroll_Index = 0; //Reset Scroll Index

  lcd.clear();
  lcd.home();
  lcd.setCursor(0, 0);
  lcd.print("TX:");
  lcd.setCursor(6, 0);
  lcd.print("Tms:");

  lcd.setCursor(0, 1);
  lcd.print("LB:    SDI:   SSM:");

  lcd.setCursor(0, 2);
  lcd.print("B29");
  //lcd.setCursor(7, 0);
  lcd.print(" -----------> ");
  //lcd.setCursor(11, 0);
  lcd.print("B11");
  //  lcd.setCursor(17, 0);
  //  lcd.print("SSM");
  delay (1000);
}

void TX_Display_A429_HEX_Show_Data() {
  for ( uint8_t i = 0; i <= 2; i++) {

    if ((TX_Scroll_Index + i) == 8) {
      LCD_Clear_Line(3);
      break;
      //continue;
    }
    //TX BUFFER
    //lcd.setCursor(0, 1 + i);
    //lcd.print("                    "); //Deleted to avoid flickering.
    lcd.setCursor(0, 1 + i);
    //lcd.setCursor(0, 3);
    if ((TX_Scroll_Index + (i + 1)) < 10) {
      lcd.print("0");
    }
    lcd.print(TX_Scroll_Index + (i + 1));

    //LABEL
    if (A429_TX_Buffer[TX_Scroll_Index + i].A429_Label > 0100) {
      lcd.setCursor(3, 1 + i);
      lcd.print(A429_TX_Buffer[TX_Scroll_Index + i].A429_Label, OCT);
    }
    else if ((A429_TX_Buffer[TX_Scroll_Index + i].A429_Label < 0100) && (A429_TX_Buffer[TX_Scroll_Index + i].A429_Label > 07)) {
      lcd.setCursor(3, 1 + i);
      lcd.print ("0");
      lcd.print(A429_TX_Buffer[TX_Scroll_Index + i].A429_Label, OCT);
    }
    else if (A429_TX_Buffer[TX_Scroll_Index + i].A429_Label < 010) {
      lcd.setCursor(3, 1 + i);
      lcd.print ("0");
      lcd.print ("0");
      lcd.print(A429_TX_Buffer[TX_Scroll_Index + i].A429_Label, OCT);
    }

    //SDI
    if (A429_TX_Buffer[TX_Scroll_Index + i].A429_SDI < 2) {
      lcd.setCursor(8, 1 + i);
      lcd.print("0");
      lcd.print(A429_TX_Buffer[TX_Scroll_Index + i].A429_SDI, BIN);
    }
    else {
      lcd.setCursor(8, 1 + i);
      lcd.print(A429_TX_Buffer[TX_Scroll_Index + i].A429_SDI, BIN);
    }

    //DATA
    lcd.setCursor(11, 1 + i);
    if (A429_TX_Buffer[TX_Scroll_Index + i].A429_Data == 0)
      lcd.print("00000");
    else
      lcd.print(A429_TX_Buffer[TX_Scroll_Index + i].A429_Data, HEX);

    //SSM
    if (A429_TX_Buffer[TX_Scroll_Index + i].A429_SSM < 2) {
      lcd.setCursor(18, 1 + i);
      lcd.print("0");
      lcd.print(A429_TX_Buffer[TX_Scroll_Index + i].A429_SSM, BIN);
    }
    else {
      lcd.setCursor(18, 1 + i);
      lcd.print(A429_TX_Buffer[TX_Scroll_Index + i].A429_SSM, BIN);
    }
  } // for (uint8_t i = 0; i <= 2; i++)

}//TX_Display_A429_HEX_Show_Data()

void TX_Display_A429_HEX()  {

  int key = 0;

  if (Header_A429_HEX == false) {
    Display_TX_HEX_Header_Screen();// Default
    Header_A429_HEX = true;
    TX_Scroll_Index = 0; //First Time reset Index
  }

  TX_Display_A429_HEX_Show_Data();

  do {

    while (!(key = I2C_Keypad.getKey())) {
    }

    switch (key) //Check for Digital Input
    {
      case '1':
        if ((TX_Scroll_Index < (Max_A429_TX_Buffer - 2)))  // Note: Max_A429_TX_Buffer 8 (0 >> 7)
          TX_Scroll_Index += 3;
        else if ((TX_Scroll_Index == (Max_A429_TX_Buffer - 2)))
          TX_Scroll_Index = 0;
        TX_Display_A429_HEX_Show_Data();
        break;

      case '7':
        if ((TX_Scroll_Index > 0))  // Note: Max_A429_TX_Buffer 8 (0 >> 7)
          TX_Scroll_Index -= 3;
        else if (TX_Scroll_Index ==  0)
          TX_Scroll_Index = 6;

        TX_Display_A429_HEX_Show_Data();
        break;

      default:
        break;
    }//switch(key)
  } while (key != '*');
}//void TX_Display_A429_HEX()


void TX_Display_A429_BIN_Show_Data() {
  uint8_t Data_MSB = 0;

  //TX BUFFER
  lcd.setCursor(3, 0);
  //lcd.print("  "); //Erase former TX
  lcd.setCursor(3, 0);
  lcd.print(TX_Scroll_Index + 1);

  //Serial.println("TX:  ");
  //delay(3000);

  //TX Tms (Refresh time)
  lcd.setCursor(10, 0);
  lcd.print("    "); //Erase former Tms
  lcd.setCursor(10, 0);
  lcd.print((A429_TX_Buffer[TX_Scroll_Index].TX_Refresh_50ms * 50));
  //lcd.print((A429_TX_Buffer[TX_Scroll_Index].TX_Refresh_50ms));
  //lcd.print("    "); //Erase former TX

  //Serial.println("Tms:  ");
  //delay(3000);


  //TX LABEL
  lcd.setCursor(3, 1);
  lcd.print("   "); //Erase former LABEL

  lcd.setCursor(3, 1);
  if (A429_TX_Buffer[TX_Scroll_Index].A429_Label > 0100) {
    lcd.print(A429_TX_Buffer[TX_Scroll_Index].A429_Label, OCT);
  }
  else if ((A429_TX_Buffer[TX_Scroll_Index].A429_Label < 0100) && (A429_TX_Buffer[TX_Scroll_Index].A429_Label > 07)) {
    lcd.setCursor(3, 1);
    lcd.print ("0");
    lcd.print(A429_TX_Buffer[TX_Scroll_Index].A429_Label, OCT);
  }
  else if (A429_TX_Buffer[TX_Scroll_Index].A429_Label < 010) {
    lcd.setCursor(3, 1);
    lcd.print ("0");
    lcd.print ("0");
    lcd.print(A429_TX_Buffer[TX_Scroll_Index].A429_Label, OCT);
  }

  //Serial.println("TX LABEL:  ");
  //delay(3000);

  //TX SDI
  lcd.setCursor(11, 1);
  lcd.print("   "); //Erase former SDI
  if (A429_TX_Buffer[TX_Scroll_Index].A429_SDI < 2) {
    lcd.setCursor(11, 1);
    lcd.print("0");
    lcd.print(A429_TX_Buffer[TX_Scroll_Index].A429_SDI, BIN);
    lcd.setCursor(13, 1);
    lcd.print(" ");
  }
  else {
    lcd.setCursor(11, 1);
    lcd.print(A429_TX_Buffer[TX_Scroll_Index].A429_SDI, BIN);
    lcd.print(" ");
  }

  //Serial.println(" SDI:  ");
  //delay(3000);

  //TX SSM
  lcd.setCursor(18, 1);
  lcd.print("  "); //Erase former SSM

  if (A429_TX_Buffer[TX_Scroll_Index].A429_SSM < 2) {
    lcd.setCursor(18, 1);
    lcd.print("0");
    lcd.print(A429_TX_Buffer[TX_Scroll_Index].A429_SSM, BIN);
  }
  else {
    lcd.setCursor(18, 1);
    lcd.print(A429_TX_Buffer[TX_Scroll_Index].A429_SSM, BIN);
  }

  //DATA BIN
  lcd.setCursor(0, 3);
  LCD_Clear_Line(3);

  //////////////////New/////////////////
  const uint32_t Data;

  uint8_t Data_0;
  uint16_t Data_1;
  uint16_t Data_bit;
  uint16_t Mask_Data = 0b0000000000000001;

  Data_0 = A429_TX_Buffer[TX_Scroll_Index].A429_Data >> 16;
  Data_1 = A429_TX_Buffer[TX_Scroll_Index].A429_Data & 0b0001111111111111111;


  //Serial.print("Data_0: ");
  int8_t cursor = 1;
  for (int8_t i = 2; i >= 0; i--) {
    Mask_Data = 0b0000000000000001;
    Mask_Data = Mask_Data << i;
    Data_bit = Data_0 & Mask_Data;
    lcd.setCursor(cursor++, 3);
    if (Data_bit)
      lcd.print("1");
    else
      lcd.print("0");
  } // for Data_0

  //Serial.print("\nData_1: ");

  for (int16_t i = 15; i >= 0; i--) {
    Mask_Data = 0b0000000000000001;
    Mask_Data = Mask_Data << i;
    Data_bit = Data_1 & Mask_Data;
    lcd.setCursor(cursor++, 3);
    if (Data_bit)
      lcd.print("1");
    else
      lcd.print("0");
  } // for Data_1

  /////////////////////////////////////


}//void TX_Display_A429_BIN_Show_Data()

void TX_Display_A429_BIN_Show_Data_Old() {
  uint8_t Data_MSB = 0;

  //TX BUFFER
  lcd.setCursor(3, 0);
  //lcd.print("  "); //Erase former TX
  lcd.setCursor(3, 0);
  lcd.print(TX_Scroll_Index + 1);

  //Serial.println("TX:  ");
  //delay(3000);

  //TX Tms (Refresh time)
  lcd.setCursor(10, 0);
  lcd.print("    "); //Erase former Tms
  lcd.setCursor(10, 0);
  lcd.print((A429_TX_Buffer[TX_Scroll_Index].TX_Refresh_50ms * 50));
  //lcd.print((A429_TX_Buffer[TX_Scroll_Index].TX_Refresh_50ms));
  //lcd.print("    "); //Erase former TX

  //Serial.println("Tms:  ");
  //delay(3000);


  //TX LABEL
  lcd.setCursor(3, 1);
  lcd.print("   "); //Erase former LABEL

  lcd.setCursor(3, 1);
  if (A429_TX_Buffer[TX_Scroll_Index].A429_Label > 0100) {
    lcd.print(A429_TX_Buffer[TX_Scroll_Index].A429_Label, OCT);
  }
  else if ((A429_TX_Buffer[TX_Scroll_Index].A429_Label < 0100) && (A429_TX_Buffer[TX_Scroll_Index].A429_Label > 07)) {
    lcd.setCursor(3, 1);
    lcd.print ("0");
    lcd.print(A429_TX_Buffer[TX_Scroll_Index].A429_Label, OCT);
  }
  else if (A429_TX_Buffer[TX_Scroll_Index].A429_Label < 010) {
    lcd.setCursor(3, 1);
    lcd.print ("0");
    lcd.print ("0");
    lcd.print(A429_TX_Buffer[TX_Scroll_Index].A429_Label, OCT);
  }

  //Serial.println("TX LABEL:  ");
  //delay(3000);

  //TX SDI
  lcd.setCursor(11, 1);
  lcd.print("   "); //Erase former SDI
  if (A429_TX_Buffer[TX_Scroll_Index].A429_SDI < 2) {
    lcd.setCursor(11, 1);
    lcd.print("0");
    lcd.print(A429_TX_Buffer[TX_Scroll_Index].A429_SDI, BIN);
    lcd.setCursor(13, 1);
    lcd.print(" ");
  }
  else {
    lcd.setCursor(11, 1);
    lcd.print(A429_TX_Buffer[TX_Scroll_Index].A429_SDI, BIN);
    lcd.print(" ");
  }

  //Serial.println(" SDI:  ");
  //delay(3000);

  //TX SSM
  lcd.setCursor(18, 1);
  lcd.print("  "); //Erase former SSM

  if (A429_TX_Buffer[TX_Scroll_Index].A429_SSM < 2) {
    lcd.setCursor(18, 1);
    lcd.print("0");
    lcd.print(A429_TX_Buffer[TX_Scroll_Index].A429_SSM, BIN);
  }
  else {
    lcd.setCursor(18, 1);
    lcd.print(A429_TX_Buffer[TX_Scroll_Index].A429_SSM, BIN);
  }

  //DATA BIN
  lcd.setCursor(0, 3);
  LCD_Clear_Line(3);
  uint32_t A429_Data_Mask = 0b0000000000000000000;

  uint32_t A429_Data = A429_TX_Buffer[TX_Scroll_Index].A429_Data;
  for (uint8_t i = 19; i >= 0; i--) {
    lcd.setCursor(i, 3);
    uint32_t A429_Data_Mask = 0b0000000000000000000;
    A429_Data_Mask &= ~( 1 >> i);
    if (A429_Data_Mask & A429_TX_Buffer[TX_Scroll_Index].A429_Data)
      lcd.print("1");
    else
      lcd.print("0");
  }

  lcd.setCursor(1, 3);
  lcd.print("0000000000000000000"); // Set default pattern
  if (A429_TX_Buffer[TX_Scroll_Index].A429_Data == 0) {
    //lcd.print("0000000000000000000");
    return;
  }

  if (Data_MSB >= 4) {
    lcd.print(A429_TX_Buffer[TX_Scroll_Index].A429_Data, BIN);
  }
  else if ((Data_MSB <= 3) || (Data_MSB >= 2)) {
    lcd.setCursor(1, 3);
    lcd.print("0");
    lcd.setCursor(2, 3);
    lcd.print(A429_TX_Buffer[TX_Scroll_Index].A429_Data, BIN);
  }
  else if ((Data_MSB <= 2) || (Data_MSB >= 1)) {
    lcd.setCursor(1, 3);
    lcd.print("00");
    lcd.print(A429_TX_Buffer[RX_Scroll_Index].A429_Data, BIN);
  }
  else {
    lcd.setCursor(1, 3);
    lcd.print("000");
    lcd.print(A429_TX_Buffer[RX_Scroll_Index].A429_Data, BIN);
  }

  //Serial.println("DATA:  ");
  //delay(3000);
} //void TX_Display_A429_BIN_Show_Data_Old()

void TX_Display_A429_BIN() {//First time: Set up BIN header and  reset "RX_Scroll_Index"
  int key = 0;
  Header_A429_HEX = false;
  Header_A429_ENG = false;

  if (Header_A429_BIN == false) {
    Display_TX_BIN_Header_Screen();// Default
    Header_A429_BIN = true;
  }

  TX_Display_A429_BIN_Show_Data();

  do {

    while (!(key = I2C_Keypad.getKey())) {
    }

    switch (key) //Check for Digital Input
    {
      case '1':
        if ((TX_Scroll_Index < (Max_A429_TX_Buffer - 1)))  // Note: Max_A429_TX_Buffer 8 (0 >> 7)
          TX_Scroll_Index += 1;
        else if ((TX_Scroll_Index >= (Max_A429_TX_Buffer - 1)))
          TX_Scroll_Index = 0;
        TX_Display_A429_BIN_Show_Data();
        break;

      case '7':
        if (TX_Scroll_Index == 0)  // Note: Max_A429_TX_Buffer 8 (0 >> 7)
          TX_Scroll_Index = 7;
        else if ((TX_Scroll_Index <= (Max_A429_TX_Buffer - 1)))
          TX_Scroll_Index -= 1;

        TX_Display_A429_BIN_Show_Data();
        break;

      default:
        break;
    }//switch(key)
  } while (key != '*');
}// TX_Display_A429_BIN

//////LCD RX FUNCTIONS///////////////

void Display_RX_HEX_Header_Screen() {
  // Print menu RX to the LCD.

  RX_Scroll_Index = 0; //Reset Scroll Index

  lcd.clear();
  lcd.home();
  lcd.setCursor(0, 0);
  lcd.print("RX");
  lcd.setCursor(3, 0);
  lcd.print("LAB");
  lcd.setCursor(7, 0);
  lcd.print("SDI");
  lcd.setCursor(11, 0);
  lcd.print("DATA");
  lcd.setCursor(17, 0);
  lcd.print("SSM");
  delay (100);
}//void Display_RX_HEX_Header_Screen_SDI()

void Display_RX_BIN_Header_Screen() {
  // Print menu RX to the LCD.

  RX_Scroll_Index = 0; //Reset Scroll Index

  lcd.clear();
  lcd.home();
  lcd.setCursor(0, 0);
  lcd.print("RX:");
  //lcd.setCursor(5, 0);
  //lcd.print("BINARY");
  lcd.setCursor(6, 0);
  lcd.print("Tms:         ");

  lcd.setCursor(0, 1);
  lcd.print("LB:    SDI:   SSM:");

  lcd.setCursor(0, 2);
  lcd.print("B29");
  lcd.print(" -----------> ");
  //lcd.setCursor(11, 0);
  lcd.print("B11");
  delay (1000);
}//void Display_RX_BIN_Header_Screen()


void Display_RX_ENG_Header_Screen() {
  // Print menu RX to the LCD.

  RX_Scroll_Index = 0; //Reset Scroll Index

  lcd.clear();
  lcd.home();
  lcd.setCursor(0, 0);
  lcd.print("RX");
  lcd.setCursor(3, 0);
  lcd.print("LAB");
  lcd.setCursor(7, 0);
  lcd.print("ENG.");
  lcd.setCursor(15, 0);
  lcd.print("Unid.");
  //lcd.setCursor(18, 0);
  //lcd.print("ms");
  delay (1000);
}//void Display_RX_ENG_Header_Screen()


void RX_Display_A429_HEX_Show_Data() {

  for ( uint8_t i = 0; i <= 2; i++) {

    //RX BUFFER
    lcd.setCursor(0, 1 + i);
    //lcd.print("                    ");  //Deleted to avoid flickering.
    lcd.setCursor(0, 1 + i);
    if ((RX_Scroll_Index + (i + 1)) < 10) {
      lcd.print("0");
    }
    lcd.print(RX_Scroll_Index + (i + 1));

    //LABEL
    if (A429_RX_Buffer[RX_Scroll_Index + i].A429_Label > 0100) {
      lcd.setCursor(3, 1 + i);
      lcd.print(A429_RX_Buffer[RX_Scroll_Index + i].A429_Label, OCT);
    }
    else if ((A429_RX_Buffer[RX_Scroll_Index + i].A429_Label < 0100) && (A429_RX_Buffer[RX_Scroll_Index + i].A429_Label > 07)) {
      lcd.setCursor(3, 1 + i);
      lcd.print ("0");
      lcd.print(A429_RX_Buffer[RX_Scroll_Index + i].A429_Label, OCT);
    }
    else if (A429_RX_Buffer[RX_Scroll_Index + i].A429_Label < 010) {
      lcd.setCursor(3, 1 + i);
      lcd.print ("0");
      lcd.print ("0");
      lcd.print(A429_RX_Buffer[RX_Scroll_Index + i].A429_Label, OCT);
    }

    //SDI
    if (A429_RX_Buffer[RX_Scroll_Index + i].A429_SDI < 2) {
      lcd.setCursor(7, 1 + i);
      lcd.print("0");
      lcd.print(A429_RX_Buffer[RX_Scroll_Index + i].A429_SDI, BIN);
    }
    else {
      lcd.setCursor(7, 1 + i);
      lcd.print(A429_RX_Buffer[RX_Scroll_Index + i].A429_SDI, BIN);
    }

    //DATA
    lcd.setCursor(11, 1 + i);
    if (A429_RX_Buffer[RX_Scroll_Index + i].A429_Data == 0)
      lcd.print("00000");
    else
      lcd.print(A429_RX_Buffer[RX_Scroll_Index + i].A429_Data, HEX);

    //SSM
    if (A429_RX_Buffer[RX_Scroll_Index + i].A429_SSM < 2) {
      lcd.setCursor(17, 1 + i);
      lcd.print("0");
      lcd.print(A429_RX_Buffer[RX_Scroll_Index + i].A429_SSM, BIN);
    }
    else {
      lcd.setCursor(17, 1 + i);
      lcd.print(A429_RX_Buffer[RX_Scroll_Index + i].A429_SSM, BIN);
    }

  } // for (uint8_t i = 0; i <= 2; i++)

}//void RX_Display_A429_HEX_Show_Data()


void RX_Display_A429_HEX_Show_Data_Old_Tms() {

  for ( uint8_t i = 0; i <= 2; i++) {

    //RX BUFFER
    lcd.setCursor(0, 1 + i);
    //lcd.print("                    ");  //Deleted to avoid flickering.
    lcd.setCursor(0, 1 + i);
    if ((RX_Scroll_Index + (i + 1)) < 10) {
      lcd.print("0");
    }
    lcd.print(RX_Scroll_Index + (i + 1));

    //LABEL
    if (A429_RX_Buffer[RX_Scroll_Index + i].A429_Label > 0100) {
      lcd.setCursor(3, 1 + i);
      lcd.print(A429_RX_Buffer[RX_Scroll_Index + i].A429_Label, OCT);
    }
    else if ((A429_RX_Buffer[RX_Scroll_Index + i].A429_Label < 0100) && (A429_RX_Buffer[RX_Scroll_Index + i].A429_Label > 07)) {
      lcd.setCursor(3, 1 + i);
      lcd.print ("0");
      lcd.print(A429_RX_Buffer[RX_Scroll_Index + i].A429_Label, OCT);
    }
    else if (A429_RX_Buffer[RX_Scroll_Index + i].A429_Label < 010) {
      lcd.setCursor(3, 1 + i);
      lcd.print ("0");
      lcd.print ("0");
      lcd.print(A429_RX_Buffer[RX_Scroll_Index + i].A429_Label, OCT);
    }

    //DATA
    lcd.setCursor(7, 1 + i);
    if (A429_RX_Buffer[RX_Scroll_Index + i].A429_Data == 0)
      lcd.print("00000");
    else
      lcd.print(A429_RX_Buffer[RX_Scroll_Index + i].A429_Data, HEX);

    //SSM
    if (A429_RX_Buffer[RX_Scroll_Index + i].A429_SSM < 2) {
      lcd.setCursor(13, 1 + i);
      lcd.print("0");
      lcd.print(A429_RX_Buffer[RX_Scroll_Index + i].A429_SSM, BIN);
    }
    else {
      lcd.setCursor(13, 1 + i);
      lcd.print(A429_RX_Buffer[RX_Scroll_Index + i].A429_SSM, BIN);
    }

    //Tms
    //A429_RX_Buffer [RX_Scroll_Index + i].RX_Refresh_50ms
    uint16_t Tms = (A429_RX_Buffer [RX_Scroll_Index + i].RX_Refresh_50ms) * 50; // To get the refresh time in msec
    //uint8_t Tms = (A429_RX_Buffer [RX_Scroll_Index + i].RX_Refresh_50ms); // To get the refresh time in blocks of 50 msec
    lcd.setCursor(17, 1 + i);
    if (A429_RX_Buffer[RX_Scroll_Index + i].A429_Label == 000) {
      lcd.print("000");
      continue;
    }

    if (Tms >= 100) {
      lcd.print(Tms);
    }
    else if ((Tms < 100) && (Tms >= 10)) {
      lcd.print("0");
      lcd.print(Tms);
    }
    else {
      lcd.print(Tms);
    }

  } // for (uint8_t i = 0; i <= 2; i++)

}//void RX_Display_A429_HEX_Show_Data_Old_Tms()

void RX_Display_A429_ENG_Show_Data() {

  for ( uint8_t i = 0; i <= 2; i++) {

    //RX BUFFER
    lcd.setCursor(0, 1 + i);
    //lcd.print("                    ");  //Deleted to avoid flickering.
    lcd.setCursor(0, 1 + i);
    if ((RX_Scroll_Index + (i + 1)) < 10) {
      lcd.print("0");
    }
    lcd.print(RX_Scroll_Index + (i + 1));

    //LABEL
    if (A429_RX_Buffer[RX_Scroll_Index + i].A429_Label > 0100) {
      lcd.setCursor(3, 1 + i);
      lcd.print(A429_RX_Buffer[RX_Scroll_Index + i].A429_Label, OCT);
    }
    else if ((A429_RX_Buffer[RX_Scroll_Index + i].A429_Label < 0100) && (A429_RX_Buffer[RX_Scroll_Index + i].A429_Label > 07)) {
      lcd.setCursor(3, 1 + i);
      lcd.print ("0");
      lcd.print(A429_RX_Buffer[RX_Scroll_Index + i].A429_Label, OCT);
    }
    else if (A429_RX_Buffer[RX_Scroll_Index + i].A429_Label < 010) {
      lcd.setCursor(3, 1 + i);
      lcd.print ("0");
      lcd.print ("0");
      lcd.print(A429_RX_Buffer[RX_Scroll_Index + i].A429_Label, OCT);
    }

    // If Label decoded into ENGINEERING DATA
    if (A429_RX_Buffer[RX_Scroll_Index + i].A429_Label == 001) {
      lcd.setCursor(7, 1 + i);
      float Distance = Label_001_Distance_To_Go(A429_RX_Buffer[RX_Scroll_Index + i].A429_Data);
      lcd.print(Distance);
      lcd.setCursor(14, 1 + i);
      lcd.print("  ");
      lcd.setCursor(15, 1 + i);
      lcd.print("NM");
    }
    //Else show SDI--HEX-SSM

    else {
      //DATA
      lcd.setCursor(7, 1 + i);
      if (A429_RX_Buffer[RX_Scroll_Index + i].A429_Data == 0)
        lcd.print("            ");
      else {
        lcd.print(A429_RX_Buffer[RX_Scroll_Index + i].A429_Data, HEX);
        lcd.setCursor(12, 1 + i);
        lcd.print("        ");
      }
    }//Else show SDI--HEX-SSM
  } // for (uint8_t i = 0; i <= 2; i++)

}//void RX_Display_A429_ENG_Show_Data()

void RX_Display_A429_BIN_Show_Data() {
  uint8_t Data_MSB = 0;
  //RX BUFFER
  lcd.setCursor(3, 0);
  //lcd.print("  "); //Deleted to avoid flickering. Erase former RX
  lcd.setCursor(3, 0);
  //lcd.setCursor(0, 1 + i);
  lcd.print(RX_Scroll_Index + 1);
  if ((RX_Scroll_Index + 1) <= 9)
    lcd.print(' ');


  //Tms
  //A429_RX_Buffer [RX_Scroll_Index + i].RX_Refresh_50ms
  uint16_t Tms = (A429_RX_Buffer [RX_Scroll_Index].RX_Refresh_50ms) * 50; // To get the refresh time in msec
  //uint8_t Tms = (A429_RX_Buffer [RX_Scroll_Index].RX_Refresh_50ms); // To get the refresh time in blocks of 50 msec
  lcd.setCursor(10, 0);
  if (A429_RX_Buffer[RX_Scroll_Index].A429_Label == 000) {
    lcd.print("       ");
    //continue;
  }
  else if ((Tms >= 100) && (Tms <= 9999) ) {
    lcd.print(Tms);
    lcd.print("    ");
  }
  else if ((Tms < 100) && (Tms >= 10)) {
    lcd.print("0");
    lcd.print(Tms);
    lcd.print("    ");
  }
  else {
    lcd.print(Tms);
    lcd.print("    ");
  }

  //RX LABEL
  lcd.setCursor(3, 1);
  //lcd.print("   "); ////Deleted to avoid flickering.Erase former LABEL

  lcd.setCursor(3, 1);
  if (A429_RX_Buffer[RX_Scroll_Index].A429_Label > 0100) {
    lcd.print(A429_RX_Buffer[RX_Scroll_Index].A429_Label, OCT);
  }
  else if ((A429_RX_Buffer[RX_Scroll_Index].A429_Label < 0100) && (A429_RX_Buffer[RX_Scroll_Index].A429_Label > 07)) {
    lcd.setCursor(3, 1);
    lcd.print ("0");
    lcd.print(A429_RX_Buffer[RX_Scroll_Index].A429_Label, OCT);
  }
  else if (A429_RX_Buffer[RX_Scroll_Index].A429_Label < 010) {
    lcd.setCursor(3, 1);
    lcd.print ("0");
    lcd.print ("0");
    lcd.print(A429_RX_Buffer[RX_Scroll_Index].A429_Label, OCT);
  }

  //RX SDI
  lcd.setCursor(11, 1);
  //lcd.print("  "); //Deleted to avoid flickering.Erase former SDI
  if (A429_RX_Buffer[RX_Scroll_Index].A429_SDI < 2) {
    lcd.setCursor(11, 1);
    lcd.print("0");
    lcd.print(A429_RX_Buffer[RX_Scroll_Index].A429_SDI, BIN);
  }
  else {
    lcd.setCursor(11, 1);
    lcd.print(A429_RX_Buffer[RX_Scroll_Index].A429_SDI, BIN);
  }

  //RX SSM
  lcd.setCursor(18, 1);
  //lcd.print("  "); //Deleted to avoid flickering.Erase former SSM

  if (A429_RX_Buffer[RX_Scroll_Index].A429_SSM < 2) {
    lcd.setCursor(18, 1);
    lcd.print("0");
    lcd.print(A429_RX_Buffer[RX_Scroll_Index].A429_SSM, BIN);
  }
  else {
    lcd.setCursor(18, 1);
    lcd.print(A429_RX_Buffer[RX_Scroll_Index].A429_SSM, BIN);
  }

  //DATA BIN

  //////////////////////////////New //////////////////////

  //const uint32_t Data;

  uint8_t Data_0;
  uint16_t Data_1;
  uint16_t Data_bit;
  uint16_t Mask_Data = 0b0000000000000001;

  Data_0 = A429_RX_Buffer[RX_Scroll_Index].A429_Data >> 16;
  Data_1 = A429_RX_Buffer[RX_Scroll_Index].A429_Data & 0b0001111111111111111;


  //Serial.print("Data_0: ");
  int8_t cursor = 1;
  for (int8_t i = 2; i >= 0; i--) {
    Mask_Data = 0b0000000000000001;
    Mask_Data = Mask_Data << i;
    Data_bit = Data_0 & Mask_Data;
    lcd.setCursor(cursor++, 3);
    if (Data_bit)
      lcd.print("1");
    else
      lcd.print("0");
  } // for Data_0

  //Serial.print("\nData_1: ");

  for (int16_t i = 15; i >= 0; i--) {
    Mask_Data = 0b0000000000000001;
    Mask_Data = Mask_Data << i;
    Data_bit = Data_1 & Mask_Data;
    lcd.setCursor(cursor++, 3);
    if (Data_bit)
      lcd.print("1");
    else
      lcd.print("0");
  } // for Data_1

  /////////////////////////////////////

}//void RX_Display_A429_BIN_Show_Data();


void RX_Display_A429_BIN_Show_Data_Old() {
  uint8_t Data_MSB = 0;
  //RX BUFFER
  lcd.setCursor(3, 0);
  //lcd.print("  "); //Deleted to avoid flickering. Erase former RX
  lcd.setCursor(3, 0);
  //lcd.setCursor(0, 1 + i);
  lcd.print(RX_Scroll_Index + 1);
  if ((RX_Scroll_Index + 1) <= 9)
    lcd.print(' ');


  //Tms
  //A429_RX_Buffer [RX_Scroll_Index + i].RX_Refresh_50ms
  uint16_t Tms = (A429_RX_Buffer [RX_Scroll_Index].RX_Refresh_50ms) * 50; // To get the refresh time in msec
  //uint8_t Tms = (A429_RX_Buffer [RX_Scroll_Index].RX_Refresh_50ms); // To get the refresh time in blocks of 50 msec
  lcd.setCursor(10, 0);
  if (A429_RX_Buffer[RX_Scroll_Index].A429_Label == 000) {
    lcd.print("       ");
    //continue;
  }
  else if ((Tms >= 100) && (Tms <= 9999) ) {
    lcd.print(Tms);
    lcd.print("    ");
  }
  else if ((Tms < 100) && (Tms >= 10)) {
    lcd.print("0");
    lcd.print(Tms);
    lcd.print("    ");
  }
  else {
    lcd.print(Tms);
    lcd.print("    ");
  }

  //RX LABEL
  lcd.setCursor(3, 1);
  //lcd.print("   "); ////Deleted to avoid flickering.Erase former LABEL

  lcd.setCursor(3, 1);
  if (A429_RX_Buffer[RX_Scroll_Index].A429_Label > 0100) {
    lcd.print(A429_RX_Buffer[RX_Scroll_Index].A429_Label, OCT);
  }
  else if ((A429_RX_Buffer[RX_Scroll_Index].A429_Label < 0100) && (A429_RX_Buffer[RX_Scroll_Index].A429_Label > 07)) {
    lcd.setCursor(3, 1);
    lcd.print ("0");
    lcd.print(A429_RX_Buffer[RX_Scroll_Index].A429_Label, OCT);
  }
  else if (A429_RX_Buffer[RX_Scroll_Index].A429_Label < 010) {
    lcd.setCursor(3, 1);
    lcd.print ("0");
    lcd.print ("0");
    lcd.print(A429_RX_Buffer[RX_Scroll_Index].A429_Label, OCT);
  }

  //RX SDI
  lcd.setCursor(11, 1);
  //lcd.print("  "); //Deleted to avoid flickering.Erase former SDI
  if (A429_RX_Buffer[RX_Scroll_Index].A429_SDI < 2) {
    lcd.setCursor(11, 1);
    lcd.print("0");
    lcd.print(A429_RX_Buffer[RX_Scroll_Index].A429_SDI, BIN);
  }
  else {
    lcd.setCursor(11, 1);
    lcd.print(A429_RX_Buffer[RX_Scroll_Index].A429_SDI, BIN);
  }

  //RX SSM
  lcd.setCursor(18, 1);
  //lcd.print("  "); //Deleted to avoid flickering.Erase former SSM

  if (A429_RX_Buffer[RX_Scroll_Index].A429_SSM < 2) {
    lcd.setCursor(18, 1);
    lcd.print("0");
    lcd.print(A429_RX_Buffer[RX_Scroll_Index].A429_SSM, BIN);
  }
  else {
    lcd.setCursor(18, 1);
    lcd.print(A429_RX_Buffer[RX_Scroll_Index].A429_SSM, BIN);
  }

  //DATA BIN
  lcd.setCursor(0, 3);
  //LCD_Clear_Line(3);
  lcd.setCursor(1, 3);
  Data_MSB = A429_RX_Buffer[RX_Scroll_Index].A429_Data >> 16;
  if (A429_RX_Buffer[RX_Scroll_Index].A429_Data == 0) {
    lcd.print("0000000000000000000");
  }
  else if (Data_MSB >= 4) {
    lcd.print(A429_RX_Buffer[RX_Scroll_Index].A429_Data, BIN);
  }
  else if ((Data_MSB <= 3) && (Data_MSB >= 2)) {
    lcd.print("0");
    lcd.print(A429_RX_Buffer[RX_Scroll_Index].A429_Data, BIN);
  }
  else if ((Data_MSB >= 0) && (Data_MSB <= 1)) {
    lcd.print("0");
    lcd.print("0");
    lcd.print(A429_RX_Buffer[RX_Scroll_Index].A429_Data, BIN);
  }

}//void RX_Display_A429_BIN_Show_Data_Old();


void RX_Display_A429_HEX() {
  int key = 0;

  /////// 23-01-2021 To fix ramdon barbage in display /////////////
  Header_A429_BIN = false;
  RX_Scroll_Index = 0;
  ////////////////////

  if (Header_A429_HEX == false) {
    Display_RX_HEX_Header_Screen();// Default
    Header_A429_HEX = true;
  }

  RX_Display_A429_HEX_Show_Data();

  while ((key = I2C_Keypad.getKey()) != '*') {

    RX_Display_A429_HEX_Show_Data();
    /////////////////////////  RX PROCEDURES /////////////////////////
    if (INT_RX1_Ready == true) { // RX1 Have received data
      RX1_Buffer_Busy = true;
      Get_RX1_Data();
      INT_RX1_Ready = false;

      //////////// Debugging //////////////
      ISR_RX1_Count--;
      //Serial.print("ISR_RX1_Count=");
      //Serial.print(ISR_RX1_Count);
      /////////////////////////////////////
      //INT_RX1_Ready = false;
      //RX1_Buffer_Busy = false;
      Arrange_RX_Buffer();
      RX1_Buffer_Busy = false;
      //RX_Display_A429_HEX_Show_Data(); // 23-JAN-2021 To avoid empty display until press '1' UP Buffer or '7' DOWN Buffer

    } //If INT_RX1 true

    switch (key) //Check for Digital Input
    {
      case '1':
        if ((RX_Scroll_Index < (Max_A429_RX_Buffer - 3)))  // Note: Max_A429_RX_Buffer 20 (0+1 >> 19+1)
          RX_Scroll_Index += 3;
        else if ((RX_Scroll_Index == (Max_A429_RX_Buffer - 3)))
          RX_Scroll_Index = 0;
        //RX_Display_A429_HEX_Show_Data();
        break;

      case '7':
        if (RX_Scroll_Index == 0)  // Note: Max_A429_TX_Buffer 8 (0 >> 7)
          RX_Scroll_Index = Max_A429_RX_Buffer - 3;
        else if (RX_Scroll_Index > 0)
          RX_Scroll_Index -= 3;
        //RX_Display_A429_HEX_Show_Data();
        break;

      default:
        //Menu Test code
        if (Menu_Test == true) {
          if (ISR_RX1_Times >= 20) {
            ISR_RX1_Times = 0;
            A429_TX_Buffer[0]. A429_Data ++;
            if ( A429_TX_Buffer[0]. A429_Data >= 0x3FFFF) {
              A429_TX_Buffer[0]. A429_Data = 0;
            }//ifA429_Data >= 0x3FFFF
            Setup_TX_Buffer();//For testing Dynamic changes on label Data
            Load_DEI_TX_Buffer();
          }//if ISR_RX1_Times
        }//if Menu_Test
        break;//default
    }//switch(key)
  }//  while ((key = I2C_Keypad.getKey()) != '*')
} //RX_Display_A429_HEX()


void RX_Display_A429_ENG() {
  int key = 0;

  /////// 23-01-2021 To fix ramdon barbage in display /////////////
  Header_A429_BIN = false;
  Header_A429_HEX = false;
  RX_Scroll_Index = 0;
  ////////////////////

  if (Header_A429_ENG == false) {
    Display_RX_ENG_Header_Screen();// Default
    Header_A429_ENG = true;
  }

  RX_Display_A429_ENG_Show_Data();

  while ((key = I2C_Keypad.getKey()) != '*') {

    RX_Display_A429_ENG_Show_Data();

    /////////////////////////  RX PROCEDURES /////////////////////////
    if (INT_RX1_Ready == true) { // RX1 Have received data
      RX1_Buffer_Busy = true;
      Get_RX1_Data();
      INT_RX1_Ready = false;

      //////////// Debugging //////////////
      ISR_RX1_Count--;
      //Serial.print("ISR_RX1_Count=");
      //Serial.print(ISR_RX1_Count);
      /////////////////////////////////////

      Arrange_RX_Buffer();
      RX1_Buffer_Busy = false;

      //RX_Display_A429_ENG_Show_Data(); // 23-JAN-2021 To avoid empty display until press '1' UP Buffer or '7' DOWN Buffer

    } //If INT_RX1 true

    switch (key) //Check for Digital Input
    {
      case '1':
        if ((RX_Scroll_Index < (Max_A429_RX_Buffer - 3)))  // Note: Max_A429_RX_Buffer 8 (0 >> 7)
          RX_Scroll_Index += 3;
        else if ((RX_Scroll_Index == (Max_A429_RX_Buffer - 3)))
          RX_Scroll_Index = 0;
        //RX_Display_A429_ENG_Show_Data();
        break;

      case '7':
        if (RX_Scroll_Index == 0)  // Note: Max_A429_TX_Buffer 8 (0 >> 7)
          RX_Scroll_Index = Max_A429_RX_Buffer - 3;
        else if (RX_Scroll_Index > 0)
          RX_Scroll_Index -= 3;
        //RX_Display_A429_ENG_Show_Data();
        break;

      default:
        //Menu Test code
        if (Menu_Test == true) {
          if (ISR_RX1_Times >= 20) {
            ISR_RX1_Times = 0;
            A429_TX_Buffer[0]. A429_Data ++;
            if ( A429_TX_Buffer[0]. A429_Data >= 0x3FFFF) {
              A429_TX_Buffer[0]. A429_Data = 0;
            }//ifA429_Data >= 0x3FFFF
            Setup_TX_Buffer();//For testing Dynamic changes on label Data
            Load_DEI_TX_Buffer();
          }//if ISR_RX1_Times
        }//if Menu_Test
        break;//default
    }//switch(key)
  }//while ((key = I2C_Keypad.getKey()) != '*')
} //RX_Display_A429_ENG()


void RX_Display_A429_BIN() {//First time: Set up BIN header and  reset "RX_Scroll_Index"
  int key = 0;
  Header_A429_HEX = false;
  Header_A429_ENG = false;
  RX_Scroll_Index = 0;

  if (Header_A429_BIN == false) {
    Display_RX_BIN_Header_Screen();// Default
    Header_A429_BIN = true;
  }

  RX_Display_A429_BIN_Show_Data();

  while ((key = I2C_Keypad.getKey()) != '*') {
    /////////////////////////  RX PROCEDURE /////////////////////////
    if (INT_RX1_Ready == true) { // RX1 Have received data
      RX1_Buffer_Busy = true;
      Get_RX1_Data();
      INT_RX1_Ready = false;

      //////////// Debugging //////////////
      ISR_RX1_Count--;
      //Serial.print("ISR_RX1_Count=");
      //Serial.print(ISR_RX1_Count);
      /////////////////////////////////////

      Arrange_RX_Buffer();
      RX1_Buffer_Busy = false;

      //Setup_TX_Buffer();
      RX_Display_A429_BIN_Show_Data();
    } //If INT_RX1 true

    switch (key) //Check for Digital Input
    {
      case '1':
        if ((RX_Scroll_Index < (Max_A429_RX_Buffer - 1)))  // Note: Max_A429_RX_Buffer 8 (0 >> 7)
          RX_Scroll_Index += 1;
        else if ((RX_Scroll_Index == (Max_A429_RX_Buffer - 1)))
          RX_Scroll_Index = 0;
        RX_Display_A429_BIN_Show_Data();
        break;

      case '7':
        if (RX_Scroll_Index == 0)  // Note: Max_A429_TX_Buffer 8 (0 >> 7)
          RX_Scroll_Index = Max_A429_RX_Buffer - 1;
        else if (RX_Scroll_Index > 0)
          RX_Scroll_Index -= 1;
        RX_Display_A429_BIN_Show_Data();
        break;

      default:
        //Menu Test code
        if (Menu_Test == true) {
          if (ISR_RX1_Times >= 20) {
            ISR_RX1_Times = 0;
            A429_TX_Buffer[0]. A429_Data ++;
            if ( A429_TX_Buffer[0]. A429_Data >= 0x3FFFF) {
              A429_TX_Buffer[0]. A429_Data = 0;
            }//ifA429_Data >= 0x3FFFF
            Setup_TX_Buffer();//For testing Dynamic changes on label Data
            Load_DEI_TX_Buffer();
          }//if ISR_RX1_Times
        }//if Menu_Test
        break;//default
    }//switch(key)
  }// while ((key = I2C_Keypad.getKey()) != '*')
}// RX_Display_A429_BIN


///////////////// TIMERS ///////////////////////
void Init_Timers() {

  /*Clock setup for Timer1 & Tmer4, MEGA 2560
    Program to check DEI1610 clock 1 MHz and 50msec clock to be used as refresh clock for ARINC 429 labels.
    Timer 1 Channel A: 1MHz DEI1016 Clock. Pin Digital 11.
    Timer 1 Channel B: Not used. Pin Digital 12.
    Timer 4 Channel A: 20Hz Interrupt for Labels refresh Time. Pin Digital 6.
    Timer 4 Channel B: Not Used. Pin Digital 7.
    Timer 4 Channel C: Not Used. Pin Digital 8.
  */

  //set pins as outputs
  DDRB |= (1 << PB5);  //DI11, PB5. Timer1A.1MHz DEI1016 Clock.
  DDRH |= (1 << PH3); //DI06, PH3. Timer4A. 20Hz Interrupt for labels refresh clock.
  //pinMode(12, OUTPUT); // Timer1B. Not used
  //pinMode(7, OUTPUT);  // Timer4B. Not used.

  cli();  //Disable interrupts

  /////////////  TIMER 1A for 1MHz Clock  //////////////
  /*Reset Registers TIMER 1*/
  TCCR1A = 0x00;// set entire TCCR1A register to 0
  TCCR1B = 0x00;// set entire TCCR1A register to 0
  TCCR1C = 0x00;// set entire TCCR1A register to 0
  TCNT1 = 0;//initialize counter value to 0
  OCR1A = 0; //On Compare Register.
  OCR1B = 0; //On Compare Register.

  /*Reset Registers TIMER 4*/
  TCCR4A = 0x00;// set entire TCCR1A register to 0
  TCCR4B = 0x00;// set entire TCCR1A register to 0
  TCCR4C = 0x00;// set entire TCCR1A register to 0
  TCNT4 = 0;//initialize counter value to 0
  OCR4A = 0; //On Compare Register.
  OCR4B = 0; //On Compare Register
  OCR4C = 0; //On Compare Register

  /***** SettingS TIMER1 Ch A 1MHz DEI1016Clock ******/
  TCCR1B |= (1 << CS11 ); // Clk/8 = 2 MHz. Set Prescaler to divide by 8. N = 8
  TCCR1B |= (1 << WGM12); // Mode 4 CTC Clear on Time Compare
  TCCR1A |= (1 << COM1A0);  // Compare Output Mode for channel A Timer 1 (pin: 11) to Toggle when CTC
  OCR1A = 0;//On compare Register >> Focr = Fclk/(2N(1+OCR1A)), N:8 Prescaler. >> Focr = 16MHz/(2x8) = 1 Mhz

  /***** Settings TIMER4 Ch C 20Hz Refresh ARINC 429 Labels  Clock ******/
  TCCR4B |= (1 << CS40 ); //  Clk/64. Set Prescaler to divide by 64. N = 64
  TCCR4B |= (1 << CS41 ); // Clk/64. Set Prescaler to divide by 64. N = 64
  TCCR4B |= (1 << WGM42); // Mode4 CTC Clear on Time Compare
  TCCR4A |= (1 << COM4A0);  // Compare Output Mode for channel A Timer 4 (pin: 6) to Toggle when CTC
  //OCR4A = 12499; // = 0x30D3; Focr = Fclk/(2N(1+OCR1A)), N:64 Prescaler. >> OCR = (16.10e6 - (20x2x64))/(20x2x64) = 20 Hz ( Refresh 50 msec)
  OCR4AH = 0x30;   //  First HI for Temporary Register.
  OCR4AL = 0xD3;   // Then LO

  TIMSK4 |= (1 << OCIE4A); //Enables INT when counter match OCR.
  last_millis = millis();
  sei(); //Enable interrupts
}// Init_Timers


void Check_50msec_Timer() {//To test 50msec Timer

  if (last_count_50ms != count_50ms) {
    //actual_millis = millis();
    //Serial.print("Miliseconds: ");
    //Serial.println (actual_millis - last_millis);
    //last_millis = actual_millis;
    //last_count_50ms = count_50ms ;
    TX_ON = true;
  }
}
//////////////////// Delays Functions ///////////////////
static __inline__ void Delay_50nsec() {
  asm volatile ("nop\n\t"); //62,5 nanoseconds
}

static __inline__ void Delay_100nsec() {
  asm volatile ("nop\n\t"); //62,5 nanoseconds
  asm volatile ("nop\n\t"); //62,5 nanoseconds
}
static __inline__ void Delay_200nsec() {
  asm volatile ("nop\n\t"); //62,5 nanoseconds
  asm volatile ("nop\n\t"); //62,5 nanoseconds
  asm volatile ("nop\n\t"); //62,5 nanoseconds
  asm volatile ("nop\n\t"); //62,5 nanoseconds
}
static __inline__ void Delay_300nsec() {
  asm volatile ("nop\n\t"); //62,5 nanoseconds
  asm volatile ("nop\n\t"); //62,5 nanoseconds
  asm volatile ("nop\n\t"); //62,5 nanoseconds
  asm volatile ("nop\n\t"); //62,5 nanoseconds
  asm volatile ("nop\n\t"); //62,5 nanoseconds
  asm volatile ("nop\n\t"); //62,5 nanoseconds
}
//

/////////////////////////////// ON DELAY.H /////////////////////////////
//_NOP1(); // 180 nsec
//_NOP2(); // 250 nsec
//_NOP3(); // 316 nsec

/*************************************************************************************************
  Data_Bus_Output()
  This module sets the bus pins to Outputs
**************************************************************************************************/
void Data_Bus_Output()
{
  DDRL = 0xFF; // PORT Output
  DDRC = 0xFF; // PORT Output
}

/*************************************************************************************************
  Data_Bus_Output()
  This module sets the bus pins to Inputs HI Z
**************************************************************************************************/
void Data_Bus_Input()
{
  PORTL = 0x00; //Inputs HI Z
  PORTC = 0x00;  //Inputs HI Z
  DDRL = 0x00; // PORT Input
  DDRC = 0x00;  // PORT Input
}

////////////////// Direct PORT Read & Write functions.

unsigned int Read_Data_Word() { // Read Word B0 --> B15

  unsigned int data_word;
  byte byte_low;
  byte byte_high;

  DDRC = 0x00;
  DDRL = 0x00;
  byte_low = PINL;
  byte_high = PINC;
  data_word = byte_high << 8;
  data_word = data_word | byte_low;

  return (data_word);
} //Read_Data_Word()


void Write_Data_Word(unsigned int data_word) { // Write Word B0 --> B15

  unsigned int word_1;
  byte byte_low;
  byte byte_high;

  byte_low = data_word & 0x00FF;
  word_1 = data_word >> 8;
  byte_high = word_1 & 0xFF;

  DDRC = 0xFF;
  DDRL = 0xFF;
  PORTC = byte_high;
  PORTL = byte_low;
}


////////////////////// Output functions ///////////////////////////

void Select_Word_1_Read() { // Select Word_1 to be read SEL. Logic Input. Receiver word select. A Low input selects receiver Word 1; Hi selects Word 2 to be read on D[15:0] port.
  //PA0 = LOW >> Set Word_1. Pin: DI22  //PA0 = HIGH >> Set Word_2. Pin: DI22
  // Default SEL WORD_1
  cbi (PORTA, 0); //PA0 "LOW" /PA0 = LOW >> Set Word_1. Pin: DI22

}

void Select_Word_2_Read() { // Select Word_2 to be read SEL. Logic Input. Receiver word select. A Low input selects receiver Word 1; Hi selects Word 2 to be read on D[15:0] port.
  //PA0 = LOW >> Set Word_1. Pin: DI22  //PA0 = HIGH >> Set Word_2.Pin: DI22
  // Default is SEL WORD_1

  sbi (PORTA, 0); //PA0 "HI" // //PA0 = HIGH >> Set WORD_2.Pin: DI22
}

void Select_Word_1_Load() { // Select Word_1 to be load LD1. Logic Input. Load Transmitter Word 1. A Low input pulse loads Word 1 into the Transmitter FIFO from D[15:0].
  //PA3 = LOW >> Select Load Word_1. Pin: DI25  //PA3 = HIGH >> Deselect Word_1
  cbi (PORTA, 3); //PA3 "LOW" //
}

void Deselect_Word_1_Load() { // Deselect Word_1 to be load LD1
  //PA3 = LOW >> Select Load Word_1. Pin: DI25  //PA3 = HIGH >> Deselect Word_1
  // Default is SEL WORD_2
  sbi (PORTA, 3); //PA3 "HI" //
}

void Select_Word_2_Load() { // Select Word_2 to be load LD2. Logic Input. Load Transmitter Word 2. A Low input pulse loads Word 2 into the Transmitter FIFO from D[15:0].
  //PA4 = HIGH >> Select Load Word_2.Pin: DI26  //PA4 = LOW >> Deselect Load Word_2.Pin: DI26
  // Default is SEL WORD_2
  cbi (PORTA, 4); //PA4 "LOW" //
}

void Deselect_Word_2_Load() { // Deselect Word_2 to be load LD2
  //PA4 = HIGH >> Select Load Word_2.Pin: DI26  //PA4 = LOW >> Deselect Load Word_2.Pin: DI26
  // Default is SEL WORD_2
  sbi (PORTA, 4); //PA4 "HI" //
}

void Enable_RX_1() {
  //OE1 PA1 = LOW Enables RX 1. Pin: DI23  //OE1 PA1 = HIGH Disenables RX 1. Pin: DI23
  //sbi (PORTA, 1); //PA1 "HI" //
  cbi (PORTA, 1); //PA1 "LO" //  //OE1 PA1 = LOW Enables RX 1. Pin: DI23

}

void Disable_RX_1() {
  //PA1 = LOW Enables RX 1. Pin: DI23 OE1  //PA1 = HIGH Disenables RX 1.Pin: DI23. OE1
  //cbi (PORTA, 1); //PA1 "LOW"
  sbi (PORTA, 1); //PA1 "HI" //PA1 = HIGH Disenables RX 1.Pin: DI23. OE1
}

void Enable_RX_2() {
  //PA2 = LOW Enables RX 2. Pin: DI24. OE2  //PA2 = HIGH Disenables RX 2.Pin: DI24. OE2
  cbi (PORTA, 2); //PA2 "LOW"
}

void Disable_RX_2() {
  //PA2 = LOW Enables RX 1. Pin: DI24. OE2  //PA2 = HIGH Disenables RX 1.Pin: DI24
  sbi (PORTA, 2); //PA2 "HI" //
}

void Set_Parity_High() {
  //DBCEN. Pin: DI07 >> PH4
  //Parity HI forces to DEI1016 inserts parity bit into b32. Select PARCK D12 on Control Regiter to Logic "1" for EVEN parity or Logic "0" for ODD parity
  // Note: Normal parity is ODD >> D12 = "0".
  sbi (PORTH, 4); //PH4 "HI" //
}

void Set_Parity_Low() {
  //DBCEN. Pin: DI07 >> PH4
  //Parity HI forces to DEI1016 inserts parity bit into b32. Select PARCK D12 on Control Regiter to Logic "1" for EVEN parity or Logic "0" for ODD parity
  // Note: Normal parity is ODD >> D12 = "0".
  cbi (PORTH, 4); //PH4 "HI" //
}

void Master_Reset_Default() {
  //To make sure Reset is OFF, at any time. //MR.Pin DI29 >> PA7 to HI  //MR Default = HI
  //cbi (PORTA, 7); //PA7 "LOW" // To be inverted by Trigger Schmitt "LOW"
  sbi (PORTA, 7); //PA7 "HI" //
}

void Master_Reset() {
  // Master_Reset for DEI1016
  //MR.Pin DI29 >> PA7  // LO signal during MIN 200 nanoseconds, resets FIFO Transmitter, bit counters, gap timers, DRx, TXR (Transmitter Redy),
  // Control Register is not affected. Signal used on Power Up and systen Reset.
  cbi (PORTA, 7); //PA7 "LOW" //
  Delay_200nsec;
  sbi (PORTA, 7); //PA7 "HI"
}

void Load_Control_Register_ON() {
  //LDCW.Pin DI28 >> PA6.  //LDCW. Logic Input. Load Control Register. A LOW input pulse(MIN 130nsec) loads the Control Register from D[15:0].
  cbi (PORTA, 6); //PA6 "LOW" //
}

void Load_Control_Register_OFF() {
  //LDCW.Pin DI28 >> PA6.
  //LDCW. Logic Input. Load Control Register. A LOW input pulse(MIN 130nsec) loads the Control Register from D[15:0].
  //LDCW Default = HI
  sbi (PORTA, 6); //PA6 "HI"
}

void Enable_TX() {
  //ENTX. Pin DI27 >> PA5.
  //ENTX. Logic Input. Enable Transmitter. A Hi input enables the Transmitter to send data from the Transmitter FIFO.
  //ENTX. Logic must be LOW while writing data into Transmitter FIFO. Transmitter memory is cleared by HIGH-to-LOW transition.
  sbi (PORTA, 5); //PA5 "HI"
}

void Disable_TX() {
  //ENTX. Pin DI27 >> PA5.
  //ENTX. Logic Input. Enable Transmitter. A Hi input enables the Transmitter to send data from the Transmitter FIFO.
  //ENTX. Logic must be LOW while writing data into Transmitter FIFO. Transmitter memory is cleared by HIGH-to-LOW transition.
  cbi(PORTA, 5); //PA5 "LOW"
}

/////////////////////// Miscelaneous Functions /////////////////////////

void Fill_Testing_TX_Array() {

  //Dummy function to test the program.
  // Labels to be transmitted should be selected through Keypad and then call to Setup_TX_Buffer(); function to setup WORD_1, WORD_2 before to be sent to DEI1016

  //Load TX Buffer with A429 ARRAY[0]
  A429_TX_Buffer[0]. A429_Label = 001; // Label Octal coded.
  //A429_TX_Buffer[0]. A429_Label = 0000; // Test Label.Does work because isnot received ???.Check why
  A429_TX_Buffer[0]. A429_SDI = B00000010; // SDI Binary.
  A429_TX_Buffer[0]. A429_Data = 0x01234; // Data HEX coded. Distance to Go 2750,4NM
  A429_TX_Buffer[0]. A429_SSM = B00000011; // SSM Binary.
  A429_TX_Buffer[0]. TX_Refresh_50ms = 2; // 100ms;

  //Load TX Buffer with A429 ARRAY [1]
  A429_TX_Buffer[1]. A429_Label = 0377; // Label Octal coded.d
  A429_TX_Buffer[1]. A429_SDI = B00000001; // SDI Binary.
  A429_TX_Buffer[1]. A429_Data = 0x12345; // Data HEX coded.
  A429_TX_Buffer[1]. A429_SSM = B00000010; // SSM Binary.
  A429_TX_Buffer[1]. TX_Refresh_50ms = 3; //150ms;

  //Load TX Buffer with A429 ARRAY [2]
  A429_TX_Buffer[2]. A429_Label = 0321; // Label Octal coded.
  A429_TX_Buffer[2]. A429_SDI = B00000010; // SDI Binary.
  A429_TX_Buffer[2]. A429_Data = 0x3CDEF; // Data HEX coded.
  A429_TX_Buffer[2]. A429_SSM = B00000001; // SSM Binary.
  A429_TX_Buffer[2]. TX_Refresh_50ms = 5; //250ms; //default value

  //Load TX Buffer with A429 ARRAY [3]
  A429_TX_Buffer[3]. A429_Label = 0213; // Label Octal coded.
  A429_TX_Buffer[3]. A429_SDI = B00000001; // SDI Binary.
  A429_TX_Buffer[3]. A429_Data = 0x23579; // Data HEX coded.
  A429_TX_Buffer[3]. A429_SSM = B00000010; // SSM Binary.
  A429_TX_Buffer[3]. TX_Refresh_50ms = 7; //350ms;

  //Load TX Buffer with A429 ARRAY [4]
  A429_TX_Buffer[4]. A429_Label = 0103; // Label Octal coded.
  A429_TX_Buffer[4]. A429_SDI = B00000010; // SDI Binary.
  A429_TX_Buffer[4]. A429_Data = 0x1CE13; // Data HEX coded.
  A429_TX_Buffer[4]. A429_SSM = B00000001; // SSM Binary.
  A429_TX_Buffer[4]. TX_Refresh_50ms = 11; //550ms;

  //Load TX Buffer with A429 ARRAY [5]
  A429_TX_Buffer[5]. A429_Label = 031; // Label Octal coded.
  A429_TX_Buffer[5]. A429_SDI = B00000001; // SDI Binary.
  A429_TX_Buffer[5]. A429_Data = 0x21234; // Data HEX coded.
  A429_TX_Buffer[5]. A429_SSM = B00000010; // SSM Binary.
  A429_TX_Buffer[5]. TX_Refresh_50ms = 13; //650ms;

  //Load TX Buffer with A429 ARRAY [6]
  A429_TX_Buffer[6]. A429_Label = 0032; // Label Octal coded.
  A429_TX_Buffer[6]. A429_SDI = B00000010; // SDI Binary.
  A429_TX_Buffer[6]. A429_Data = 0x1BCDE; // Data HEX coded.
  A429_TX_Buffer[6]. A429_SSM = B00000001; // SSM Binary.
  A429_TX_Buffer[6]. TX_Refresh_50ms = 19; //950ms;

  //Load TX Buffer with A429 ARRAY [7]
  A429_TX_Buffer[7]. A429_Label = 0211; // Label Octal coded.
  A429_TX_Buffer[7]. A429_SDI = B00000001; // SDI Binary.
  A429_TX_Buffer[7]. A429_Data = 0x12345; // Data HEX coded.
  A429_TX_Buffer[7]. A429_SSM = B00000010; // SSM Binary.
  A429_TX_Buffer[7]. TX_Refresh_50ms = 17; //850ms;

} //void Fill_Testing_TX_Array()

void Setup_TX_Buffer() {

  //This function will setup WORD_1 and WORD_2, taking the data already stored in each A429 structure of the A429 Array.
  // Will be called allways after any label is loaded into A429_TX_Buffer

  uint32_t A429_Data_Mask = 0x00000000;  // Use to be decided ????
  uint16_t A429_Word_Mask = 0x0000;

  TX_Buffer_Busy = true;  // Flag for ISR Interrupt Buffer Empty no take action while in Load_DEI_TX_Buffer

  for (uint8_t i = 0; i <= 7; i++)
  {
    //Check for Label out of range. Actually shuold be checked while entering TX Labels.
    if ( (A429_TX_Buffer[i].A429_Label == 0) || (A429_TX_Buffer[i]. A429_Label > 0377)) {
      continue;
    }//if Label out of range

    //Reset DEI1016 Words: DEI1016_Word_1 =  0x0000 & DEI1016_Word_2 =  0x0000;
    A429_TX_Buffer[i].DEI1016_Word_1 = 0x0000;
    A429_TX_Buffer[i].DEI1016_Word_2 = 0x0000;

    // DEI1016_Word_1 Set Up
    A429_TX_Buffer[i].DEI1016_Word_1 = A429_TX_Buffer[i]. A429_Data << 13;  // Place B13, B12, B11 into Word_1: B15, B14, B13
    A429_Word_Mask = A429_Word_Mask | A429_TX_Buffer[i]. A429_SDI;
    A429_Word_Mask = A429_Word_Mask << 11; // << 11; //Load SDI into Mask
    A429_TX_Buffer[i].DEI1016_Word_1 = A429_TX_Buffer[i].DEI1016_Word_1 | A429_Word_Mask;// Mask into WORD_1
    A429_Word_Mask = 0x00000000; //Reset Mask.
    A429_Word_Mask = A429_TX_Buffer[i]. A429_SSM << 9; //Load SSM into Mask
    A429_TX_Buffer[i].DEI1016_Word_1 = A429_TX_Buffer[i].DEI1016_Word_1 | A429_Word_Mask;
    A429_TX_Buffer[i].DEI1016_Word_1 = A429_TX_Buffer[i].DEI1016_Word_1 & 0xFEFF; // B1111 1110 1111 1111;// Set Parity "0"
    A429_TX_Buffer[i].DEI1016_Word_1 = A429_TX_Buffer[i].DEI1016_Word_1 | A429_TX_Buffer[i]. A429_Label;//Load Label into DEI1016_Word_1

    //DEI1016_Word_2 Set Up
    A429_TX_Buffer[i].DEI1016_Word_2 = A429_TX_Buffer[i]. A429_Data  >> 3; //Get rid of B13, B12, B11.
  }//For

  TX_Buffer_Busy = false;  // Interrupt Buffer Empty Enable

}//void Setup_TX_Buffer()

void Load_DEI_TX_Buffer() { // This function is called every 50msec. Checks refresh times before load a label into DEI_TX_Buffer

  TX_Buffer_Busy = true;
  Disable_TX(); //ENTX to LOW. //Disable Transmition


  for (uint8_t i = 0; i <= 7 ; i++) {
    //for (uint8_t i = 0; i <= 1 ; i++) { // For testing only one label at a time
    if (A429_TX_Buffer[i].TX_Refresh_50ms == 0) {
      continue;
    }//if refresh time == 0 skip to next into buffer

    A429_TX_Buffer[i].Last_TX_Refresh_50ms++; //Update last_count_50ms

    if (A429_TX_Buffer[i].Last_TX_Refresh_50ms == A429_TX_Buffer[i].TX_Refresh_50ms) {
      Select_Word_1_Load(); //LD1 LOW. PA3 = LOW. Pin: DI25

      Write_Data_Word(A429_TX_Buffer[i].DEI1016_Word_1); // Write Word B0 --> B15
      Deselect_Word_1_Load();//LD1 HIGH //PA3 = HIGH >> Pin: DI25
      Select_Word_2_Load(); //LD2 LOW PA4 = LOW Pin: DI26
      Write_Data_Word(A429_TX_Buffer[i]. DEI1016_Word_2); // Write Word B0 --> B15
      Deselect_Word_2_Load();//LD2 HIGH. //PA4 = HIGH Pin: DI26
      A429_TX_Buffer[i].Last_TX_Refresh_50ms = 0; //Reset last_count
    } //if(A429_TX_Buffer[i].last_count == A429_TX_Buffer[i].TX_Refresh_50ms)
  } //for (int i = 0; i <= 7 ; i++)

  Enable_TX(); //ENTX to HIGH.

  INT_DEI_TX_Buffer_Empty_Flag = false;  // To Flag DEI0116 TX_Buffer has been filled out.
  TX_Buffer_Busy = false;
}// Load_DEI_TX_Buffer()


/////////////////// RX Functions //////////////////////

void Get_RX1_Data() {
  // Gets data from RX_1 after DR1 interrupt.
  // Data should be stored into Data_Word_1 and Data_Word_1, fields of "A429 struct" array using the RX buffer global index

  uint8_t Label = 0; //Coded in octal to check for label
  unsigned int Data_Word_1;
  unsigned int Data_Word_2;

  // Reset DATA BUS as INPUT
  //Data_Bus_Input(); //Bus to Read
  PORTL = 0x00; //Inputs HI Z
  PORTC = 0x00;  //Inputs HI Z
  DDRL = 0x00; // PORT Input
  DDRC = 0x00;  // PORT Input

  //////////////////////// GET WORD_1  ////////////////

  //SEL WORD1 to be read
  cbi (PORTA, 0); //SEL WORD1 >> PA0 "LOW" /PA0 = LOW >> Set Word_1. Pin: DI22

  //Enable_RX_1();//OE1 LOW, DI23
  cbi (PORTA, 1); //PA1 "LO" //  //OE1 PA1 = LOW Enables RX 1. Pin: DI23
  // _delayNanoseconds()Tpwoe); //Delay Tpwoe 200nsec

  //Read WORD_1
  Data_Word_1 = Read_Data_Word();//  Read WORD_1
  //_NOP1();

  //Disable_RX_1();//OE1 HI, DI23
  sbi (PORTA, 1); //OE1 HIGH, DI23//PA1 "HI" //PA1 = HIGH Disenables RX 1.Pin: DI23. OE1

  //////////////////////// GET WORD_2  ////////////////
  //Select_Word_2_Read(); //SEL HI WORD_2, DI22
  sbi (PORTA, 0); //SEL WORD2 >> PA0 "HI" // //PA0 = HIGH >> Set WORD_2.Pin: DI22
  //_delayNanoseconds (Toeoe); //Delay Toeoe 50nsec
  _NOP1();

  //Enable_RX_1();//OE1 LOW, DI23
  cbi (PORTA, 1); //PA1 "LO" //  //OE1 PA1 = LOW Enables RX 1. Pin: DI23
  //  _delayNanoseconds (Tdoedr); //Delay Tdoedr 200nsec

  Data_Word_2 = Read_Data_Word();  //Read WORD_2
  //Disable_RX_1();//OE1 HIGH, DI23
  sbi (PORTA, 1); //PA1 "HI" //PA1 = HIGH Disenables RX 1.Pin: DI23. OE1

  // Reset DATA BUS as OUTPUT
  //Data_Bus_Output();//Bus to Write
  DDRL = 0xFF; // PORT Output
  DDRC = 0xFF; // PORT Output

  /////////////////////////////////////////CHECKS//////////////////

  // Debugging ISR_RX1_Times
  //Serial.print("\nISR_RX1_Times: ");
  //Serial.println(ISR_RX1_Times);
  //Serial.print("A429_TX_Buffer[0]. A429_Data:");
  //Serial.println(A429_TX_Buffer[0]. A429_Data, HEX);



  //If Transmition interval > 1500 mseconds, Delete Label from the Buffer
  /*
    if(A429_RX_Buffer [i].RX_Refresh_50ms > 30) {
    RX_Clear_Buffer_Position(i);
    }
  */

  /////////////////// Checking ALL Buffer for unique Label & Data Pair ///////////////////////////////////////////

  //Check if all data Data_Word_1 and Data_Word_2 already in the RX_Buffer. Same Label, same Data. Update refresh Time and >> Return
  for (uint8_t i = 0; i < Max_A429_RX_Buffer; i++) {

    if ((Data_Word_1 == A429_RX_Buffer [i].DEI1016_Word_1) && (Data_Word_2 == A429_RX_Buffer [i].DEI1016_Word_2)) {

      //Update refresh Time. Note that calculates blocks of 50msec.Real Refresh time in msec is calculated at Show routine by multiplying by 50
      A429_RX_Buffer [i].RX_Refresh_50ms = ((count_50ms - A429_RX_Buffer [i].Last_RX_count_50ms));// Calculate and Update Refresh Time msec.
      A429_RX_Buffer [i].Last_RX_count_50ms = count_50ms; //Updated Last time to calculate RX Refresh Time.

      //If Transmition interval > 1500 mseconds, Delete Label from the Buffer
      if (A429_RX_Buffer [i].RX_Refresh_50ms > 30) {
        RX_Clear_Buffer_Position(i);
      }
    }

    return;
  }//if Same WORD_1 and WORD_2

  //}//For


  //Get A429 Label
  Label = Data_Word_1 & 0x00FF;

  //Check ALL Buffer if label already in the RX_Buffer. If so over write data and >> Return
  //Scroll A429_RX_Buffer for same label.Update WORD_1 and WORD_2 on RX_Buffer same Label
  for (uint8_t i = 0; i < Max_A429_RX_Buffer; i++) {
    if (Label == A429_RX_Buffer [i].A429_Label) { //Over write A429 WORD_1 and WORD_2
      A429_RX_Buffer [i].DEI1016_Word_1 = Data_Word_1;
      A429_RX_Buffer [i].DEI1016_Word_2 = Data_Word_2;

      // To extract information from: DEI1016_Word_1, DEI1016_Word_2, and get the A429 fields: Label, SDI, Data, SSM

      //Update refresh Time.Note that calculates blocks of 50msec.Real Refresh time in msec is calculated at Show routine by multiplying by 50
      A429_RX_Buffer [i].RX_Refresh_50ms = ((count_50ms - A429_RX_Buffer [i].Last_RX_count_50ms));// Calculate and Update Refresh Time msec.
      A429_RX_Buffer [i].Last_RX_count_50ms = count_50ms; //Updated Last time to calculate RX Refresh Time.

      //If Transmition interval > 1500 mseconds, Delete Label from the Buffer
      if (A429_RX_Buffer [i].RX_Refresh_50ms > 30) {
        RX_Clear_Buffer_Position(i);
      }
      return;
    }
  }//For

  ///////////////////////// Store Data_Vord_1 and Data_Word_2 into RX_Buffer /////////////////
  //Check if Write_Index < Max_A429_RX_Buffer
  if (A429_RX_Buffer_Write_Index < Max_A429_RX_Buffer) {
    A429_RX_Buffer [A429_RX_Buffer_Write_Index].DEI1016_Word_1 = Data_Word_1;
    A429_RX_Buffer [A429_RX_Buffer_Write_Index].DEI1016_Word_2 = Data_Word_2;
  }
  else {
    A429_RX_Buffer_Write_Index = 0;//Reset Index to 0
    A429_RX_Buffer[A429_RX_Buffer_Write_Index].DEI1016_Word_1 = Data_Word_1;
    A429_RX_Buffer[A429_RX_Buffer_Write_Index].DEI1016_Word_2 = Data_Word_2;
    //Serial.println("Write Index to Reset");
  }

  //Update refresh Time
  //Update refresh Time.Note that calculates blocks of 50msec.Real Refresh time in msec is calculated at Show routine by multiplying by 50
  A429_RX_Buffer [A429_RX_Buffer_Write_Index].RX_Refresh_50ms = ((count_50ms - A429_RX_Buffer [A429_RX_Buffer_Write_Index].Last_RX_count_50ms));// Calculate and Update Refresh Time msec.
  A429_RX_Buffer [A429_RX_Buffer_Write_Index].Last_RX_count_50ms = count_50ms; //Updated Last time to calculate RX Refresh Time.

  //If Transmition interval > 1500 mseconds, Delete Label from the Buffer
  if (A429_RX_Buffer [A429_RX_Buffer_Write_Index].RX_Refresh_50ms > 30) {
    RX_Clear_Buffer_Position(A429_RX_Buffer_Write_Index);
  }

  //Increase RX_Buffer Pointer
  A429_RX_Buffer_Write_Index++;//Increase Pointer

  //Check RX_Buffer Limit
  if (A429_RX_Buffer_Write_Index >= Max_A429_RX_Buffer) {
    A429_RX_Buffer_Write_Index = 0; // Reset Write Pointer
  }

} //Get_RX1_Data()

void Arrange_RX_Buffer() {

  // From already stores DEI1016_Word_1 and DEI1016_Word_2, get Label, SDI, DATA, SSM, and store into the fields of each struct of RX_Buffer[]
  // To extract information from: DEI1016_Word_1, DEI1016_Word_2, and get the A429 fields: Label, SDI, Data, SSM

  unsigned int Buffer_Index = 0; //Not Used
  uint8_t Label = 0; //Coded in octal
  uint8_t SDI = 0 ;
  uint32_t Data = 0;
  uint8_t SSM = 0;
  boolean Parity;
  uint32_t A429_Word = 0;  // Use not decided yet.
  uint16_t DEI1016_Word_1 = 0;  //To hold WORD_1.
  uint16_t DEI1016_Word_2 = 0;  //To hold WORD_2.
  uint32_t Mask_Data = 0x0000E000; // Mask for b13, b12, b11

  //Buffer_Index = A429_RX_Buffer_Write_Index;

  //while ((A429_RX_Buffer_Arrange_Index != A429_RX_Buffer_Write_Index))
  // {
  //Get A429 WORD_1 , WORD_2

  DEI1016_Word_1 = A429_RX_Buffer[A429_RX_Buffer_Arrange_Index].DEI1016_Word_1;
  DEI1016_Word_2 = A429_RX_Buffer[A429_RX_Buffer_Arrange_Index].DEI1016_Word_2;

  //Get A429 Label
  Label = DEI1016_Word_1 & 0x00FF;
  A429_RX_Buffer[A429_RX_Buffer_Arrange_Index].A429_Label = Label;

  //Get A429 SSM
  SSM = DEI1016_Word_1 >> 9;
  SSM = SSM & 0x03;
  A429_RX_Buffer[A429_RX_Buffer_Arrange_Index].A429_SSM = SSM;

  //Get A429 SDI
  SDI = DEI1016_Word_1 >> 11;
  SDI = SDI & 0x03;
  A429_RX_Buffer[A429_RX_Buffer_Arrange_Index].A429_SDI = SDI;

  //Get A429 Data.  //Note 'Data' is 32 bits word. We need 19 bits to hols A429 data field
  Data = DEI1016_Word_2;
  Data = Data << 3; // 16 MSB A429 b29 ---- b14 and reserve place for b13, b12, b11
  A429_RX_Buffer[A429_RX_Buffer_Arrange_Index].A429_Data = Data;
  Data = 0; // Reset Data
  Data = DEI1016_Word_1 >> 13; //
  A429_RX_Buffer[A429_RX_Buffer_Arrange_Index].A429_Data =  A429_RX_Buffer[A429_RX_Buffer_Arrange_Index].A429_Data | Data; // Build Up A429 Data

  //Serial.print("A429_RX_Buffer [A429_RX_Buffer_Arrange_Index].RX_Refresh_50ms: ");
  //Serial.println(A429_RX_Buffer [A429_RX_Buffer_Arrange_Index].RX_Refresh_50ms);

  A429_RX_Buffer_Arrange_Index ++;

  // Check RX Buffer Limit
  if (A429_RX_Buffer_Arrange_Index >= Max_A429_RX_Buffer) {
    A429_RX_Buffer_Arrange_Index = 0;
  } //End Buffer Limit
  // } //while((A429_RX_Buffer_Arrange_Index != A429_RX_Buffer_Write_Index))

  /*DEBUGGING
    Serial.print("\nA429_RX_Buffer_Arrange_Index: ");
    Serial.println(A429_RX_Buffer_Arrange_Index);
    Serial.print("A429_RX_Buffer_Write_Index: ");
    Serial.println(A429_RX_Buffer_Write_Index);
  */
}//Arrange Buffer

void RX_Clear_Buffer_Position (uint8_t Buff_Index) {

  A429_RX_Buffer[Buff_Index].A429_Label = 0;
  A429_RX_Buffer[Buff_Index].A429_SDI = 0;
  A429_RX_Buffer[Buff_Index].A429_Data = 0;
  A429_RX_Buffer[Buff_Index].A429_SSM = 0;
  A429_RX_Buffer[Buff_Index].A429_Parity = 0;
  A429_RX_Buffer[Buff_Index].A429_Word = 0;
  A429_RX_Buffer[Buff_Index].DEI1016_Word_1 = 0;
  A429_RX_Buffer[Buff_Index].DEI1016_Word_2 = 0;
  A429_RX_Buffer[Buff_Index].TX_Refresh_50ms = 0;
  A429_RX_Buffer[Buff_Index].Last_TX_Refresh_50ms = 0;
}//void RX_Clear_Buffer_Position (uint8_t Buff_Index)

void RX_Clear_Buffer() {
  for ( uint8_t i = 0; i < Max_A429_RX_Buffer; i++) {
    A429_RX_Buffer[i].A429_Label = 0;
    A429_RX_Buffer[i].A429_SDI = 0;
    A429_RX_Buffer[i].A429_Data = 0;
    A429_RX_Buffer[i].A429_SSM = 0;
    A429_RX_Buffer[i].A429_Parity = 0;
    A429_RX_Buffer[i].A429_Word = 0;
    A429_RX_Buffer[i].DEI1016_Word_1 = 0;
    A429_RX_Buffer[i].DEI1016_Word_2 = 0;
    A429_RX_Buffer[i].TX_Refresh_50ms = 0;
    A429_RX_Buffer[i].Last_TX_Refresh_50ms = 0;
    //New
    A429_RX_Buffer_Write_Index = 0;// Keep track of where to store next label
    A429_RX_Buffer_Arrange_Index = 0;// Keep track of labels already arranged
  }
}// RX_Clear_Buffer()

/////////////////// Load Control Word //////////////////////

void Load_Control_Word (unsigned int Ctrl_Word) {

  unsigned int Control_Data;
  byte Word_Low;
  byte Word_High;

  // Set Up Word_1 and Word_2
  Word_High = Ctrl_Word >> 8;
  Word_Low = Ctrl_Word & 0x00FF;

  //Set Up Bus for Output
  Data_Bus_Output();
  delayMicroseconds(1);

  ///////// MASTER RESET //////////////
  //Start Load Control Word Procedure.
  //Master_Reset TMR = 200ns MIN

  cbi (PORTA, 7); //PA7 "LOW" RESET ON
  delayMicroseconds(10);
  sbi (PORTA, 7); //PA7 "HI" RESET OFF

  delayMicroseconds(10);

  //LDCW  LOAD CONTROL WORD
  cbi (PORTA, 6); //PA6 LDCW "LOW". Load_Control_Register_ON(); // LDCW LOW
  delayMicroseconds(2);
  PORTL = Word_Low;
  PORTC = Word_High;
  delayMicroseconds(2);  // let them settle a bit
  sbi (PORTA, 6); //LDCW OFF PA6 "HI". //Load_Control_Register_OFF(); // LDCW HI
  delayMicroseconds(1);
}

/////////////FROM My_Menu////////////

void Display_Keypad_Setup() {
  lcd.init(); // initialize the lcd
  I2C_Keypad.begin();
  Serial.begin(9600);
  delay(100);

  //pinMode(ALT_KEY, OUTPUT);
  //digitalWrite(ALT_KEY, LOW);
  PORTH &= ~(1 << PH5);

  // LCD Backlight ON
  lcd.setBacklight(HIGH);
  lcd.clear(); // Clear the LCD screen
  Create_Custom_Characters();
  // Write_Custom_Characters();
  delay(100);
  State = State_Home; // Default state when the program starts
}

void Create_Custom_Characters() {  // Creates the byte for the 3 custom characters
  lcd.createChar(0, menuCursor);
  lcd.createChar(1, upArrow);
  lcd.createChar(2, downArrow);
}//Create_Custom_Characters()


void Write_Custom_Characters() { //Test function for Custom characters
  lcd.clear();
  for (int i = 0; i <= 2; i++) {
    lcd.setCursor(0, i);
    lcd.write(i);
  }
} //Write_Custom_Characters()

void Draw_Home_Menu() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("---- Home  Menu ----");
  lcd.setCursor(0, 1);
  lcd.print("1>TX Menu"); // finishes at 9
  lcd.setCursor(0, 2);
  lcd.print("4>TX Display");
  lcd.setCursor(0, 3);
  lcd.print("7>RX Display");
  lcd.setCursor(13, 1);
  lcd.print("2>Test");
  lcd.setCursor(13, 2);
  lcd.print("5>Reset");
  lcd.setCursor(13, 3);
  lcd.print("8>Help");
}//Draw_Home_Menu

void Home_Menu() {
  uint8_t key = 0;
  Draw_Home_Menu();
  //State = State_Home;
  while (!(key = I2C_Keypad.getKey())) { }
  switch (key) { //Check for Digital Input
    case '1':
      TX_SPEED_PARITY();
      //State = State_Home;
      //State isset into TX_SPEED_PARITY()function.
      break;

    case '4':
      TX_Display();
      break;

    case '7':
      RX_SPEED_PARITY();
      break;

    case '2':
      Test();
      delay(2000);
      break;

    case '5':
      Software_Reset();
      delay(2000);
      // Do_Reset();
      break;

    case '8':
      Help();
      //delay(2000);
      break;

    default:
      lcd.clear();
      lcd.setCursor(0, 1);
      lcd.print(" INVALID ENTRY !!!!");
      delay(2000);
      Draw_Home_Menu();
      State = State_Home;
  }
  State = State_Home;
}//Home_Menu()

////////////////////// TX ///////////////////////////////
void Draw_TX_SPEED_PARITY() {
  lcd.clear();
  lcd.setCursor(0, 0); // For TX shown UP/DWN
  lcd.print(" TX SPEED & PARITY  ");
  lcd.setCursor(0, 1); // For TX shown UP/DWN
  lcd.print("SPD: ");
  lcd.setCursor(5, 1);

  if (TX_Speed) // TX_Speed = 255 (HIGH)
    lcd.print("HIGH");  //Default Value
  else // TX_Speed = 0(LOW)
    lcd.print("LOW ");  //Default Value

  lcd.setCursor(10, 1); // For TX shown UP/DWN
  lcd.print("PAR: ");

  lcd.setCursor(15, 1); // For TX shown UP/DWN
  if (TX_Parity) // TX_Parity = 1 (HIGH)
    lcd.print("EVEN");  //Default Value
  else // TX_Parity = 0(LOW)
    lcd.print("ODD ");  //Default Value

  lcd.setCursor(0, 2);
  lcd.print("TOGGLE:");
  lcd.setCursor(7, 2);
  lcd.print('1');
  lcd.setCursor(10, 2);
  lcd.print("TOGGLE:");
  lcd.setCursor(17, 2);
  lcd.print('7');
  lcd.setCursor(0, 3);
  lcd.write('*');
  lcd.setCursor(1, 3);
  lcd.write('>');
  lcd.setCursor(3, 3);
  lcd.printstr("Home");
  lcd.setCursor(13, 3);
  lcd.write('4');
  lcd.write('>');
  lcd.setCursor(16, 3);
  lcd.printstr("Next");
}//Draw_TX_SPEED_PARITY()

void TX_SPEED_PARITY() {
  Draw_TX_SPEED_PARITY();
  do {
    while (!(key = I2C_Keypad.getKey())) {
      ; //Wait for a key been pressed
    }

    switch (key) { //Check for Digital Input
      case '1':
        TX_Speed = ~TX_Speed;
        if (TX_Speed) {
          lcd.setCursor(5, 1);
          lcd.print("HIGH");
          //digitalWrite(TX_HIGH_LED, HIGH); //DI13 >> PB7
          PORTB |= (1 << PB7);//TX_HIGH_LED ON
          //digitalWrite(BD429_RELAY, LOW); // Relay BD429 OFF for TX HIGH. DI09 >> PH6
          PORTH &= ~(1 << PH6); // Relay OFF
          //PORTH |= (1 << PH6); //Cambio Prueba Carlos 03-08-2021
        }
        else {
          lcd.setCursor(5, 1);
          lcd.print("LOW ");
          //digitalWrite(TX_HIGH_LED, LOW); //DI13 >> PB7
          PORTB &= ~(1 << PB7); //LED TX HI OFF
          //digitalWrite(BD429_RELAY, HIGH); // Relay BD429 ON for TX LOW. DI09 >> PH6
          PORTH |= (1 << PH6); //Relay ON
          //PORTH &= ~(1 << PH6); //Relay OFF Cambio Prueba Carlos 03-08-2021
        }
        //Serial.print("TX_Speed: ");
        //Serial.println(TX_Speed);
        break;

      case '7':
        TX_Parity = ~TX_Parity;  // D12 >> Flip "1" = EVEN; "0" = ODD
        if (TX_Parity) {
          lcd.setCursor(15, 1);
          lcd.print("EVEN");
          //digitalWrite(PARITY_ODD_LED, LOW);//LED PARITY ODD LED DI04 >> PG5,
          PORTG &= ~(1 << PG5);
          //digitalWrite(PARITY_EVEN_LED, HIGH); // LED PARITY EVEN LED DI05 >> PE3,
          PORTE |= (1 << PE3);
        }
        else {
          lcd.setCursor(15, 1);
          lcd.print("ODD ");
          //digitalWrite(PARITY_EVEN_LED, LOW); // LED PARITY EVEN LED DI05 >> PE3,
          PORTE &= ~(1 << PE3);
          //digitalWrite(PARITY_ODD_LED, HIGH); //LED PARITY ODD LED DI04 >> PG5,
          PORTG |= (1 << PG5);
        }
        break;

      case '4':
        break;
      case '*':
        break;

      default:
        lcd.clear();
        lcd.setCursor(0, 1);
        lcd.print(" INVALID ENTRY !!!!");
        delay(2000);
        Draw_TX_SPEED_PARITY();
    }//Switch
  }  while ((key != '*') && (key != '4')); //Do until exit from TX_Menu to go back to Home_Menu

  //Set Speed
  if (TX_Speed)
    Control_Word = (Control_Word & 0xDFFF); // TX_HI Bit 13 = '0'
  else
    Control_Word |= 0x2000; // TX_LO Bit 13 = '1'

  //Set Parity
  if (TX_Parity) //D12 = 1 EVEN
    Control_Word |= 0x1000; // EVEN Bit 12 = '1'
  else
    Control_Word = (Control_Word & 0xEFFF); // ODD  Bit 12 = '0'

  /*******************/
  while (INT_DEI_TX_Buffer_Empty_Flag != true) {
    ;//Wait until TX Buffer is empty to set the new Control Word.
    //Serial.println("Set Speed & Parity.Wating for TX Buffer Empty!!");; // Wait for TX Buffer Empty
  }
  /***********************/

  Load_Control_Word (Control_Word);

  RX_Clear_Buffer(); // Start from scratch, because RX Speed has changed.

  if (key == '4') {
    State = State_TX_Menu; //Go to TX_Menu
    TX_Menu();
  }
  else if (key == '*') {
    State = State_Home; //Back to Home_Menu
    Home_Menu();
  }
  key = 0;
} // TX_SPEED_PARITY()

void Draw_TX_Menu_Data() {
  // Print Label
  if (A429_TX_Buffer[TX_Buffer_Index].A429_Label > 0100) {
    lcd.setCursor(3, 2);
    lcd.print(A429_TX_Buffer[TX_Buffer_Index].A429_Label, OCT);
  }
  else if ((A429_TX_Buffer[TX_Buffer_Index].A429_Label < 0100) && (A429_TX_Buffer[TX_Buffer_Index].A429_Label > 07)) {
    lcd.setCursor(3, 2);
    lcd.print ("0");
    lcd.print(A429_TX_Buffer[TX_Buffer_Index].A429_Label, OCT);
  }
  else if (A429_TX_Buffer[TX_Buffer_Index].A429_Label < 010) {
    lcd.setCursor(3, 2);
    lcd.print ("0");
    lcd.print ("0");
    lcd.print(A429_TX_Buffer[TX_Buffer_Index].A429_Label, OCT);
  }

  // Print SDI
  if (A429_TX_Buffer[TX_Buffer_Index].A429_SDI <= 1) {
    lcd.setCursor(7, 2);
    lcd.print('0');
    lcd.print(A429_TX_Buffer[TX_Buffer_Index].A429_SDI, BIN);
  }
  else {
    lcd.setCursor(7, 2);
    lcd.print(A429_TX_Buffer[TX_Buffer_Index].A429_SDI, BIN);
  }

  // Print Data
  lcd.setCursor(11, 2);
  lcd.print(A429_TX_Buffer[TX_Buffer_Index].A429_Data, HEX);

  // Print SSM
  if (A429_TX_Buffer[TX_Buffer_Index].A429_SSM <= 1) {
    lcd.setCursor(17, 2);
    lcd.print('0');
    lcd.print(A429_TX_Buffer[TX_Buffer_Index].A429_SSM, BIN);
  }
  else {
    lcd.setCursor(17, 2);
    lcd.print(A429_TX_Buffer[TX_Buffer_Index].A429_SSM, BIN);
  }
}//Draw_TX_Menu_Data

void Draw_TX_Menu() {
  lcd.clear();
  lcd.setCursor(0, 0); // For Header
  lcd.print(" ----- TX Menu ------");
  lcd.setCursor(0, 1);
  lcd.printstr("TX LAB SDI DATA  SSM ");
  //  lcd.setCursor(0, 2); // For TX shown UP/DWN
  //  lcd.print(TX_Buffer_Index);
  lcd.setCursor(0, 2); // For TX shown UP/DWN
  lcd.print(TX_Buffer_Index + 1); // So as not to start with "0"
  lcd.setCursor(0, 3);
  lcd.write('1');
  lcd.setCursor(1, 3);
  lcd.write(CharUP);
  lcd.setCursor(3, 3);
  lcd.write('7');
  lcd.setCursor(4, 3);
  lcd.write(CharDOWN);
  lcd.setCursor(6, 3);
  lcd.print("4>Prog");
  lcd.setCursor(13, 3);
  lcd.print("*>Home");

  Draw_TX_Menu_Data();
}//Draw_TX_Menu

void TX_Menu() {
  Draw_TX_Menu();
  do {
    while (!(key = I2C_Keypad.getKey())) { }
    switch (key) { //Check for Digital Input
      case '1':
        TX_Buffer_Index ++;
        if (TX_Buffer_Index <= (Max_A429_TX_Buffer - 1)) {
          lcd.setCursor(0, 2); // For TX shown UP/DWN
          lcd.print("  "); // Clear last index.
          lcd.setCursor(0, 2); // For TX shown UP/DWN
          lcd.print(TX_Buffer_Index + 1);
        }
        else {
          TX_Buffer_Index = 0; //Reset TX Buffer
          lcd.setCursor(0, 2); // For TX shown UP/DWN
          lcd.print("  "); // Clear last index.
          lcd.setCursor(0, 2); // For TX shown UP/DWN
          lcd.print(TX_Buffer_Index + 1);
        }

        // Print Label
        if (A429_TX_Buffer[TX_Buffer_Index].A429_Label > 0100) {
          lcd.setCursor(3, 2);
          lcd.print(A429_TX_Buffer[TX_Buffer_Index].A429_Label, OCT);
        }
        else if ((A429_TX_Buffer[TX_Buffer_Index].A429_Label < 0100) && (A429_TX_Buffer[TX_Buffer_Index].A429_Label > 07)) {
          lcd.setCursor(3, 2);
          lcd.print ("0");
          lcd.print(A429_TX_Buffer[TX_Buffer_Index].A429_Label, OCT);
        }
        else if (A429_TX_Buffer[TX_Buffer_Index].A429_Label < 010) {
          lcd.setCursor(3, 2);
          lcd.print ("0");
          lcd.print ("0");
          lcd.print(A429_TX_Buffer[TX_Buffer_Index].A429_Label, OCT);
        }

        // Print SDI
        if (A429_TX_Buffer[TX_Buffer_Index].A429_SDI <= 1) {
          lcd.setCursor(7, 2);
          lcd.print('0');
          lcd.print(A429_TX_Buffer[TX_Buffer_Index].A429_SDI, BIN);
        }
        else {
          lcd.setCursor(7, 2);
          lcd.print(A429_TX_Buffer[TX_Buffer_Index].A429_SDI, BIN);
        }

        // Print Data
        lcd.setCursor(11, 2);
        lcd.print(A429_TX_Buffer[TX_Buffer_Index].A429_Data, HEX);

        // Print SSM
        if (A429_TX_Buffer[TX_Buffer_Index].A429_SSM <= 1) {
          lcd.setCursor(17, 2);
          lcd.print('0');
          lcd.print(A429_TX_Buffer[TX_Buffer_Index].A429_SSM, BIN);
        }
        else {
          lcd.setCursor(17, 2);
          lcd.print(A429_TX_Buffer[TX_Buffer_Index].A429_SSM, BIN);
        }
        break;

      case '7':
        TX_Buffer_Index --;
        if (TX_Buffer_Index >= 0) {
          lcd.setCursor(0, 2); // For TX shown UP/DWN
          lcd.print("  "); // Clear last index.
          lcd.setCursor(0, 2); // For TX shown UP/DWN
          lcd.print(TX_Buffer_Index + 1);
        }
        else {
          TX_Buffer_Index = (Max_A429_TX_Buffer - 1); //Reset TX Buffer
          lcd.setCursor(0, 2); // For TX shown UP/DWN
          lcd.print("  "); // Clear last index.
          lcd.setCursor(0, 2); // For TX shown UP/DWN
          lcd.print(TX_Buffer_Index + 1);
        }

        // Print Label
        if (A429_TX_Buffer[TX_Buffer_Index].A429_Label > 0100) {
          lcd.setCursor(3, 2);
          lcd.print(A429_TX_Buffer[TX_Buffer_Index].A429_Label, OCT);
        }
        else if ((A429_TX_Buffer[TX_Buffer_Index].A429_Label < 0100) && (A429_TX_Buffer[TX_Buffer_Index].A429_Label > 07)) {
          lcd.setCursor(3, 2);
          lcd.print ("0");
          lcd.print(A429_TX_Buffer[TX_Buffer_Index].A429_Label, OCT);
        }
        else if (A429_TX_Buffer[TX_Buffer_Index].A429_Label < 010) {
          lcd.setCursor(3, 2);
          lcd.print ("0");
          lcd.print ("0");
          lcd.print(A429_TX_Buffer[TX_Buffer_Index].A429_Label, OCT);
        }

        // Print SDI
        if (A429_TX_Buffer[TX_Buffer_Index].A429_SDI <= 1) {
          lcd.setCursor(7, 2);
          lcd.print('0');
          lcd.print(A429_TX_Buffer[TX_Buffer_Index].A429_SDI, BIN);
        }
        else {
          lcd.setCursor(7, 2);
          lcd.print(A429_TX_Buffer[TX_Buffer_Index].A429_SDI, BIN);
        }

        // Print Data
        lcd.setCursor(11, 2);
        lcd.print(A429_TX_Buffer[TX_Buffer_Index].A429_Data, HEX);

        // Print SSM
        if (A429_TX_Buffer[TX_Buffer_Index].A429_SSM <= 1) {
          lcd.setCursor(17, 2);
          lcd.print('0');
          lcd.print(A429_TX_Buffer[TX_Buffer_Index].A429_SSM, BIN);
        }
        else {
          lcd.setCursor(17, 2);
          lcd.print(A429_TX_Buffer[TX_Buffer_Index].A429_SSM, BIN);
        }
        break;

      case '4':
        Draw_TX_Programming_Menu();
        TX_Programming_Menu();
        //Serial.print("\nTX Programming");
        Draw_TX_Menu();
        break;

      case '*':
        //Exit
        break;

      default:
        lcd.clear();
        lcd.setCursor(0, 1);
        lcd.print(" INVALID ENTRY !!!!");
        delay(2000);
        Draw_TX_Menu();
    }//Switch
  }  while (key != '*'); //Do until exit from TX_Menu to go back to Home_Menu
  State = State_Home; //Back to Home_Menu
} //TX_Menu()

void Draw_TX_Programming_Menu() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.printstr("TX:");
  //lcd.setCursor(3, 0);
  lcd.print(TX_Buffer_Index + 1);
  lcd.printstr(" Programming");
  lcd.setCursor(0, 1);
  lcd.printstr("1>Label");
  lcd.setCursor(8, 1);
  lcd.printstr("2>SDI");
  lcd.setCursor(14, 1);
  lcd.printstr("3>Data");
  lcd.setCursor(0, 2);
  lcd.printstr("4>SSM");

  // Add Refresh Time
  lcd.setCursor(8, 2);
  lcd.printstr("5>Time");
  //
  lcd.setCursor(0, 3);
  lcd.printstr("*>Back to TX Menu");
}//Draw_TX_Programming_Menu

uint8_t Programming_Label() {
  uint8_t A429_Label = 0;
  //Serial.println("Menu Programm Label");
  /*
    while ( A429_Label == -1 ) {
    Draw_Get_A429_Label_Screen();
    A429_Label = Get_A429_Label();
    }
  */
  Draw_Get_A429_Label_Screen();
  A429_Label = Get_A429_Label();

  State = State_Programming_TX; //Back to Programming TX
  return A429_Label;
} // Programming_Label()

int8_t  Programming_SDI() {
  int8_t SDI = -1; // Set SDI error to force while loop to Get_SDI()
  //Serial.println("Menu Programm SDI");
  while ( SDI == -1) {
    Draw_Get_SDI_Screen();
    SDI = Get_SDI();
  }
  State = State_Programming_TX; //Back to Programming TX
  return SDI;
} // Programming_SDI()

int32_t Programming_Data() {
  int32_t A429_Data = -1; // Set Data error to force while loop to Get_A429_Data()
  //Serial.println("Menu Programm Data");
  while ( A429_Data == -1) {
    Draw_Get_A429_Data_Screen();
    A429_Data = Get_A429_Data();
  }
  State = State_Programming_TX; //Back to Programming TX
  return A429_Data;
} // Prorgamming_Data()

//int Programming_SSM() {
int8_t Programming_SSM() {
  //State = State_Programming_SSM;
  //Draw_Get_SSM_Screen();
  int8_t SSM = -1; // Set SSM error to force while loop to Get_SSM()
  //Serial.println("Menu Programm SSM");
  while ( SSM == -1) {
    Draw_Get_SSM_Screen();
    SSM = Get_SSM();
  }
  ; //Back to Programming TX
  return SSM;
} // Programming_SSM()

///////////////////////////////  Refresh Time  ////////////////

int8_t Programming_A429_Refresh() {
  //State = State_Programming_SSM;
  //Draw_Get_SSM_Screen();
  int8_t Refresh_Time = -1; // Set SSM error to force while loop to Get_SSM()
  //Serial.println("Menu Programm Refresh_Time");
  while (Refresh_Time == -1) {
    Draw_Get_Refresh_Time_Screen();
    Refresh_Time  = Get_Refresh_Time();
  }
  State = State_Programming_TX; //Back to Programming TX
  return Refresh_Time;
} // Programming_SSM()

/////////////////////////////////
void TX_Programming_Menu() {
  //Local A429 parameters.
  int8_t  A429_Label;
  int8_t A429_SDI;
  int8_t A429_SSM;
  int32_t A429_Data;
  uint8_t A429_Time;

  Draw_TX_Programming_Menu();
  State = State_Programming_TX;

  do {
    while (!(key = I2C_Keypad.getKey())) { }
    switch (key) {
      case '1':
        A429_Label = Programming_Label();
        // Store A429_Label
        A429_TX_Buffer[TX_Buffer_Index].A429_Label = A429_Label;
        Draw_TX_Programming_Menu(); // Redraw TX Programming Menu
        break;

      case '2':
        A429_SDI = Programming_SDI();
        // Store SDI
        A429_TX_Buffer[TX_Buffer_Index].A429_SDI = A429_SDI;
        Draw_TX_Programming_Menu(); // Redraw TX Programming Menu
        break;

      case '3':
        A429_Data = Programming_Data();
        // Store A429_Data
        A429_TX_Buffer[TX_Buffer_Index].A429_Data = A429_Data;
        Draw_TX_Programming_Menu(); // Redraw TX Programming Menu
        break;

      case '4':
        A429_SSM = Programming_SSM();
        // Store SSM
        A429_TX_Buffer[TX_Buffer_Index].A429_SSM = A429_SSM;
        Draw_TX_Programming_Menu(); // Redraw TX Programming Menu
        break;

      //////////// Refresh Time /////////////
      case '5':
        A429_Time = Programming_A429_Refresh();
        // Store TX_Refresh_50ms
        A429_TX_Buffer[TX_Buffer_Index].TX_Refresh_50ms = A429_Time;
        Draw_TX_Programming_Menu(); // Redraw TX Programming Menu
        break;
      //////////////////////////////////
      case '*':
        break;

      default:
        lcd.clear();
        lcd.setCursor(0, 1);
        lcd.print(" INVALID ENTRY !!!!");
        delay(2000);
        Draw_TX_Programming_Menu();
        break;
        //State = State_Programming_TX;
    }//Switch
  }  while (key != '*'); //Do until exit from TX_Menu to go back to Home_Menu
  Setup_TX_Buffer();   // 23-01-2021 New to correct TX label editing
  State = State_TX_Menu; //Back to TX_Menu
  key = 0;
}//TX_Programming_Menu()

void Draw_TX_Display() {
  lcd.clear();
  lcd.setCursor(0, 0); // For TX shown UP/DWN
  lcd.print(" - TX DISPLAY MENU -");
  lcd.setCursor(0, 1); // For TX shown UP/DWN
  lcd.print("1>Display HEX");
  lcd.setCursor(0, 2); // For TX shown UP/DWN
  lcd.print("4>Display BIN");
  lcd.setCursor(0, 3); // For TX shown UP/DWN
  lcd.print("7>Display ENG");
  lcd.setCursor(14, 3); // For TX shown UP/DWN
  lcd.print("*>Home");
} // Draw_TX_Display()

void TX_Display_HEX() {
  Header_A429_HEX = false;
  TX_Display_A429_HEX();
  State = State_TX_Display;
} // TE_Display_HEX()

void TX_Display_BIN() {
  TX_Display_A429_BIN();
  Header_A429_BIN = false;
  State = State_TX_Display;
} // TX_Display_BIN()

void TX_Display_ENG() {
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print(" Show_TX_Display_ENG!!!!");
  delay(2000);
  State = TX_Display;
} // TX_Display_ENG()

void TX_Display() {
  Draw_TX_Display();
  uint8_t key = 0;
  do {
    while (!(key = I2C_Keypad.getKey())) { }
    switch (key) { //Check for Digital Input
      case '1':
        //Serial.println("TX_Display_HEX");
        TX_Display_HEX();
        Draw_TX_Display();
        break;

      case '4':
        //Serial.println("TX_Display_BIN");
        TX_Display_BIN();
        Draw_TX_Display();
        break;

      case '7':
        //Serial.println("TX_Display_ENG");
        TX_Display_ENG();
        Draw_TX_Display();
        break;

      case '*':
        break;

      default:
        lcd.clear();
        lcd.setCursor(0, 1);
        lcd.print(" INVALID ENTRY !!!!");
        delay(2000);
        Draw_TX_Display();
        break;
        ;
    }
  } while ( key != '*');
  //Serial.println("Back to Home");
  //delay(2000);
  //State = State_Home; //Back to Home_Menu
  State = State_Home;
} // TX_Display()

/////////////////////// RX ///////////////////////////////
void Draw_RX_SPEED_PARITY() {
  lcd.clear();
  lcd.setCursor(0, 0); // For TX shown UP/DWN
  lcd.print(" RX SPEED & PARITY  ");
  lcd.setCursor(0, 1); // For TX shown UP/DWN
  lcd.print("SPD: ");
  lcd.setCursor(5, 1);

  if (RX_Speed) // RX_Speed = 255 (HIGH)
    lcd.print("HIGH");  //Default Value
  else // RX_Speed = 0(LOW)
    lcd.print("LOW ");  //Default Value

  lcd.setCursor(10, 1); // For TX shown UP/DWN
  lcd.print("PAR: ");
  lcd.setCursor(15, 1); // For TX shown UP/DWN
  lcd.print("ODD "); //Default
  lcd.setCursor(0, 2);
  lcd.print("TOGGLE:");
  lcd.setCursor(7, 2);
  lcd.print('1');
  lcd.setCursor(10, 2);
  lcd.print("TOGGLE:");
  lcd.setCursor(17, 2);
  lcd.print('7');
  lcd.setCursor(0, 3);
  lcd.write('*');
  lcd.setCursor(1, 3);
  lcd.write('>');
  lcd.setCursor(3, 3);
  lcd.printstr("Home");
  lcd.setCursor(13, 3);
  lcd.write('4');
  lcd.write('>');
  lcd.setCursor(16, 3);
  lcd.printstr("Next");
}//Draw_RX_SPEED_PARITY()

void RX_SPEED_PARITY() {
  Draw_RX_SPEED_PARITY();
  do {
    while (!(key = I2C_Keypad.getKey())) { }
    switch (key) { //Check for Digital Input
      case '1':
        RX_Speed = ~RX_Speed;
        if (RX_Speed) {
          lcd.setCursor(5, 1);
          lcd.print("HIGH");
          //digitalWrite(RX_HIGH_LED, HIGH);
          PORTB |= (1 << PB6);
        }
        else {
          lcd.setCursor(5, 1);
          lcd.print("LOW ");
          //digitalWrite(RX_HIGH_LED, LOW);
          PORTB &= ~(1 << PB6);
        }
        //Serial.print("RX_Speed: ");
        //Serial.println(RX_Speed);
        break;

      case '7':
        RX_Parity = ~RX_Parity;
        if (RX_Parity) {
          lcd.setCursor(15, 1);
          lcd.print("EVEN");
        }
        else {
          lcd.setCursor(15, 1);
          lcd.print("ODD ");
        }
        //Serial.print("RX_Parity: ");
        //Serial.println(RX_Parity);
        break;

      case '4':
        break;
      case '*':
        break;

      default:
        lcd.clear();
        lcd.setCursor(0, 1);
        lcd.print(" INVALID ENTRY !!!!");
        delay(2000);
        Draw_RX_SPEED_PARITY();
    }//Switch
  }  while ((key != '*') & (key != '4') ); //Do until exit from TX_Menu to go back to Home_Menu

  //Set Speed
  if (RX_Speed)
    Control_Word = (Control_Word & 0xBFFF); // TX_HI Bit 14 = '0'
  else
    Control_Word |= 0x4000; // RX_LO Bit 14 = '1'
  //Master Reset.Pin DI29 >> PA7  // LO signal during MIN 200 nanoseconds, resets FIFO Transmitter, bit counters, gap timers, DRx, TXR (Transmitter Redy),
  // RX Parameters should be changed during Master Reset Pulse.
  cbi (PORTA, 7); //PA7 "LOW" //
  Delay_200nsec;
  Load_Control_Word (Control_Word);
  Delay_200nsec;
  sbi (PORTA, 7); //PA7 "HI"

  RX_Clear_Buffer(); // start from scratch, because RX Speed has changed

  if (key == '4') {
    State  = State_RX_Display; //Go to RX_Menu
    RX_Display();
  }
  else if (key == '*') {
    State = State_Home; //Back to Home_Menu
    Home_Menu();
  }
  key = 0;
} // RX_SPEED_PARITY()

void Draw_RX_Display() {
  lcd.clear();
  lcd.setCursor(0, 0); // For RX shown UP/DWN
  lcd.print(" - RX DISPLAY MENU -");
  lcd.setCursor(0, 1); // For RX shown UP/DWN
  lcd.print("1>Display HEX");
  lcd.setCursor(0, 2); // For RX shown UP/DWN
  lcd.print("4>Display BIN");
  lcd.setCursor(0, 3); // For RX shown UP/DWN
  lcd.print("7>Display ENG");
  lcd.setCursor(14, 3); // For RX shown UP/DWN
  lcd.print("*>Home");
} // Draw_RX_Display()

void RX_Display_HEX() {
  RX_Display_A429_HEX();
  Header_A429_HEX = false;
  State = State_RX_Display;
}// RX_Display_HEX()

void RX_Display_BIN() {
  RX_Display_A429_BIN();
  Header_A429_BIN = false;
  State = State_RX_Display;
} // RX_Display_BIN()

void RX_Display_ENG() {
  RX_Display_A429_ENG();
  Header_A429_ENG = false;
  State = State_RX_Display;
} // RX_Display_ENG()

void RX_Display_ENG_Old() {
  float Distance;
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print(" Show_RX_Display_ENG!!!!");
  delay(2000);
  lcd.setCursor(0, 1);
  lcd.print(" Show Distance To Go:   ");
  Distance = Label_001_Distance_To_Go(0x27504);
  lcd.print( Distance);

  delay(4000);
  State = State_RX_Display;
} // RX_Display_ENG_Old()


void RX_Display() {
  Draw_RX_Display();
  uint8_t key = 0;
  do {
    while (!(key = I2C_Keypad.getKey())) {
    }
    switch (key) //Check for Digital Input
    {
      case '1':
        //Serial.println("RX_Display_HEX");
        RX_Display_HEX();
        Draw_RX_Display();
        break;

      case '4':
        //Serial.println("RX_Display_BIN");
        RX_Display_BIN();
        Draw_RX_Display();
        break;

      case '7':
        //Serial.println("RX_Display_ENG");
        RX_Display_ENG();
        Draw_RX_Display();
        break;

      case '*':
        break;

      default:
        lcd.clear();
        lcd.setCursor(0, 1);
        lcd.print(" INVALID ENTRY !!!!");
        delay(2000);
        Draw_RX_Display();
        break;
        ;
    }
  } while ( key != '*');
  //Serial.println("Back to Home");
  //delay(2000);
  //State = State_RX_Display;
  State = State_Home; //Back to Home_Menu
  key = 0;
} // RX_Display()

/////////////////////// TEST ///////////////////////////////
void Draw_Test() {
  lcd.clear();
  lcd.setCursor(7, 0);
  if (Menu_Test == true) {
    Menu_Test = false; // to activate scrooling TX_Buffer[0].Data every ISR_INT = 10
    lcd.print("TEST OFF");
    delay(3000);
    lcd.setCursor(0, 1);
    lcd.print("Allowed Hardware");
    lcd.setCursor(0, 2);
    lcd.print("IN's & OUT's");
    lcd.setCursor(0, 3);
    lcd.print("Connections");
    delay(5000);
    PORTB &= ~(1 << PB4); // LED BAT/TEST OFF
  }
  else {
    Menu_Test = true; // to activate scrooling TX_Buffer[0].Data every ISR_INT = 10
    lcd.print("TEST ON");
    delay(3000);
    lcd.setCursor(0, 1);
    lcd.print("Disconnect hardware");
    lcd.setCursor(0, 2);
    lcd.print("IN's and OUT's Ports");
    lcd.setCursor(0, 3);
    lcd.print("Connections");
    delay(5000);
    lcd.clear();
    lcd.home();
    lcd.setCursor(7, 0);
    lcd.print("TEST ON");
    lcd.setCursor(0, 2);
    lcd.print("Press any key when");
    lcd.setCursor(0, 3);
    lcd.print("HARDWARE OFF!!");
    while (!(key = I2C_Keypad.getKey())) {
    }
    PORTB |= 1 << PB4; // LED BAT/TEST ON
  }
} // Draw_Test()

void Test() {
  Draw_Test();
  //delay(5000);
  //Serial.println("Menu Programm TEST");
  Control_Word = 0x0000; // Reset Control_Word;

  if (Menu_Test == true) {
    while (INT_DEI_TX_Buffer_Empty_Flag != true) {
      ;//Wait until TX Buffer is empty to set the new Control Word.
      //Serial.println("Test ON. Wating for TX Buffer Empty!!");; // Wait for TX Buffer Empty
    }
    Control_Word = (PAREN | SLFTST | PAR_ODD | TX_LO | RX_LO | WORD_32); // Enables Wrap Around Selftest "SLFTST"
    Load_Control_Word(Control_Word);  //Load Control Word
    delay(100);
    RX_Clear_Buffer();
  }
  else {
    while (INT_DEI_TX_Buffer_Empty_Flag != true) {
      ;//Wait until TX Buffer is empty to set the new Control Word.
      //Serial.println("TestOFF. Wating for TX Buffer Empty!!");; // Wait for TX Buffer Empty
    }
    Control_Word = (PAREN | NOT_SLFTST | PAR_ODD | TX_LO | RX_LO | WORD_32); // Disenables Wrap Around Selftest "SLFTST"
    Load_Control_Word(Control_Word);  //Load Control Word
    delay(100);
    RX_Clear_Buffer();
  }

  //Check Diodes for TX/RX Speed LOW & Parity ODD
  // Put code here
  //
  //Fill_Testing_TX_Array(); // Load a dummy Labels to TX_Buffer nfor testing. Normal program labels will be loaded from Keypad entries.
  //Setup_TX_Buffer();  //This function will setup WORD_1 and WORD_2, taking the data already stored in each A429 structure of the A429 Array.

  State = State_Home; //Back to Home_Menu
} // Test()

/////////////////////// MISC ///////////////////////////////
void Draw_Software_Reset() {
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("Software Reset");
  lcd.setCursor(1, 2);
  lcd.print("Reset IC DEI1016");
} // Draw_Software_Reset()

void Software_Reset() {
  Draw_Software_Reset();
  //Serial.println("Menu Programm MISC");
  //Serial.println("Reset is comming!!!");
  Master_Reset();
  State = State_Home; //Back to Home_Menu
} // Software_Reset()

/////////////////////// HELP ///////////////////////////////
void Draw_Help_First_Page() {

  lcd.clear();

  //Draw the first help page
  for (uint8_t i = 0; i <= 3; i++) {
    lcd.setCursor(0, i);
    lcd.print(Help_Strings[i]);
  }
  Help_Index = 0; // Upadte Help_Index
  //Serial.print("Help_Index First Page:");
  //Serial.println(Help_Index);

} // Draw_Help_First_Page()

void Draw_Help() {
  lcd.clear();
  for (uint8_t i = 0; i <= 3; i++) {
    lcd.setCursor(0, i);// (colunm,file)
    lcd.print(Help_Strings[Help_Index + i]);
  }

  //Serial.print("Help_Index Draw_Help:");
  //Serial.println(Help_Index);
} // Draw_Help()



void Help() {

  //uint8_t Help_Index = 3;
  uint8_t Help_key = 0;

  Draw_Help_First_Page();

  do {
    while (!(Help_key = I2C_Keypad.getKey())) {
      ; //Do Nothing
    }

    switch (Help_key) { //Check for Digital Input
      case '1':
        Help_Index += 4; // Upadte Help_Index
        if (Help_Index >= Help_Index_Max)
          Help_Index = 0; // Reset Help_Index

        ////////////////   Debugging ////////////////////
        //Serial.print("Case '7' Help Index:");
        //Serial.println(Help_Index);
        /////////////////////////////////////////////////

        Draw_Help();
        break;

      case '7':
        if (Help_Index == 0)
          Help_Index = Help_Index_Max - 3;
        else Help_Index -= 4;

        ////////////////   Debugging ////////////////////
        //Serial.print("Case '7' Help Index:");
        //Serial.println(Help_Index);
        ///////////////////////////////////////////////

        Draw_Help();
        break;

      case '*':
        break;

      default:
        lcd.clear();
        lcd.setCursor(0, 1);
        lcd.print(" INVALID ENTRY !!!!");
        delay(2000);
        Draw_Help_First_Page();
    }//Switch
  }  while (Help_key != '*'); //Do until exit from TX_Menu to go back to Home_Menu

  State = State_Home; //Back to Home_Menu
} // Help()


#endif
