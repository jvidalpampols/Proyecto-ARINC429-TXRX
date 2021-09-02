#ifndef MY_MENU_H
#define MY_MENU_H
#include <Keypad_I2C.h>
#include <LiquidCrystal_I2C.h>  // I2C LCD Library by Francisco Malpartida https://bitbucket.org/fmalpartida/new-liquidcrystal/wiki/Home

#include "DEI1016_Tools.h"
#include "A429_Interrupts.h"
#include "A429_Define.h"
#include "A429_Pulses.h"
#include "Delay.h"

//PCF8574A  I2C LCD
//#define lcd_addr 0x27    // PCF8574P I2C address of typical I2C LCD Backpack. Para PCF8574P >> 0x27 
#define lcd_addr 0x3F     // PCF8574A I2C address of typical I2C LCD Backpack. Para PCF8574 >> 0x3F

//PCF8574A  I2C KEYPAD
//#define keypad_addr 0x24 // PCF8574P I2C address of I2C Expander module (A0-A1-A2 dip switch to off position). Para PCF8574 >> 0x24 
#define keypad_addr 0x3C  // PCF8574A I2C address of I2C Expander module (A0-A1-A2 dip switch to off position). Para PCF8574A >> 0x3C


//define LCD
#define LCD_ROWS 4
#define LCD_COLS 20

enum {State_Home, State_TX_Display, State_Test, State_Software_Reset, State_Help, State_TX_SPEED_PARITY_Programming, State_TX_Menu,
      State_Programming_TX, State_Programming_Label, State_Programming_SDI, State_Programming_Data, State_Programming_SSM, State_TX_Display_HEX, State_TX_Display_BIN,
      State_TX_Display_ENG,  State_RX_Display, State_RX_SPEED_PARITY_Programming, State_RX_Display_HEX, State_RX_Display_BIN, State_RX_Display_ENG
     };

uint8_t State = State_Home;
//#define ALT_KEY 8  //LED para indicar que esta presionada la tecla "ALT"
#define MaxKeys 32 // Buffer for keyboard

bool ALT = false;     //Variable Booleana para identificar si esta presoionada la tecla "ALT", que corresponde a la tecla '*'

// Define the keypad Rows & Columns
const byte ROWS = 4;
const byte COLS = 4;

//Define Keypad Matrix Prototype

char keys[ROWS][COLS] = 
  {{'1', '4', '7', '*'},
  {'2', '5', '8', '0'},
  {'3', '6', '9', '#'},
  {'A', 'B', 'C', 'D'}
};



//Define Keypad Matrix BreadOut Panel
/*
char keys[ROWS][COLS] = 
  {{'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
*/

// Keypad pins connected to the I2C-Expander pins P0-P6
byte rowPins[ROWS] = {0, 1, 2, 3}; // connect to the row pinouts of the keypad
byte colPins[COLS] = {4, 5, 6, 7};   // connect to the column pinouts of the keypad

//Custom characters

// Creates 3 custom characters for the menu display
byte downArrow[8] = {
  0b00100, //   *
  0b00100, //   *
  0b00100, //   *
  0b00100, //   *
  0b00100, //   *
  0b10101, // * * *
  0b01110, //  ***
  0b00100  //   *
};

byte upArrow[8] = {
  0b00100, //   *
  0b01110, //  ***
  0b10101, // * * *
  0b00100, //   *
  0b00100, //   *
  0b00100, //   *
  0b00100, //   *
  0b00100  //   *
};

byte menuCursor[8] = {
  B01000, //  *
  B00100, //   *
  B00010, //    *
  B11111, // *****
  B00010, //    *
  B00100, //   *
  B01000, //  *
  B00000  //
};

//#define Custom Chars
#define CharCURSOR 0
#define CharUP 1
#define CharDOWN 2

//#define Max_A429_RX_Buffer 12    // Maximun Buffer RX. Should not be  > 64 because using "uint8_t" to scroll RX Buffer
//#define Max_A429_TX_Buffer 8    // Maximun Buffer TX in DEI1016.   
#define LOW_SPEED  0
#define HIGH_SPEED  1
#define ODD_PARITY  0
#define EVEN_PARITY 1

uint8_t key;

int8_t TX_Buffer_Index = 0;   // Index for lower display. Index should be signed otherwise instead of -1 it will be 255
int8_t RX_Buffer_Index = 0;  // Index for lower display.Index should be signed otherwise instead of -1 it will be 255
uint8_t TX_Speed = LOW_SPEED; //Default Value
uint8_t RX_Speed = LOW_SPEED; //Default Value
uint8_t TX_Parity = ODD_PARITY; //Default Value
uint8_t RX_Parity = ODD_PARITY; //Default Value

// ARINC 429 Buffer.

// Create instance for LCD called: lcd
LiquidCrystal_I2C lcd(lcd_addr, LCD_COLS, LCD_ROWS); // set the LCD address to 0x27 for a 20 chars and 4 line display

// Create instance of the Keypad name I2C_Keypad and using the PCF8574 chip
Keypad_I2C I2C_Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS, keypad_addr, PCF8574 );

///////////////////Functions //////////////////////////
void Load_Control_Word (unsigned int Ctrl_Word);
static __inline__ void Delay_200nsec() __attribute__((always_inline));
void RX_Display_A429_BIN();

///////////////////////////////
void Display_Keypad_Setup();
void Create_Custom_Characters();
void Write_Custom_Characters();
void Draw_Home_Menu();
void Home_Menu();

void Draw_TX_Display();
void TX_Display();
void Draw_TX_Display_HEX();
void Draw_TX_Display_BIN();
void Draw_TX_Display_ENG();
void TX_Display_HEX();
void TX_Display_BIN();
void TX_Display_ENG();
//void TX_Display_HEX();

void Draw_RX_Display();
void RX_Display();
void Draw_RX_Display_HEX();
void Draw_RX_Display_BIN();
void Draw_RX_Display_ENG();
void RX_Display_HEX();
void RX_Display_BIN();
void RX_Display_ENG();
void Draw_RX_SPEED_PARITY();
void RX_SPEED_PARITY();

void Draw_TX_SPEED_PARITY();
void TX_SPEED_PARITY();
void Draw_TX_Menu();
void Draw_TX_Menu_Data();
void TX_Menu();
void Draw_TX_Programming_Menu();
void TX_Programming_Menu();
void Draw_Programming_Label();
uint8_t Programming_Label();
void Draw_Programming_SDI();
int8_t  Programming_SDI();
void Draw_Programming_Data();
int32_t Programming_Data();
void Draw_Programming_SSM();
int8_t Programming_SSM();

///////Refresh Time
//void Draw_Programming_A429_Refresh(void);
int8_t Programming_A429_Refresh(void);
void Draw_Get_Refresh_Time_Screen();
int8_t Get_Refresh_Time(void);

void Show_Refresh_Time_Error(void);

/////////////////////////////

void Draw_Software_Reset();
void Software_Reset();
void Draw_Test();
void Test();
void Draw_Help();
void Help();

#endif
