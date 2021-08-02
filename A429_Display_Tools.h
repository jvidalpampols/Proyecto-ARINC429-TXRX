#ifndef MY_TX_EDIT_H
#define MY_TX_EDIT_H

#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include "Arduino.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

/*/////////////////////////////// Changed files XXX.h names //////////////////////
  #include "My_Interrupts.h"
  #include "My_TX_Edit.h"
  #include "My_Define.h"
  #include "My_Pulses.h"
  #include "My_Menu.h"
  #include "Delay.h"
  //////////////////////////////// Changed files XXX.h names /////////////////////*/

#include "A429_Interrupts.h"
#include "A429_Display_Tools.h"
#include "A429_Define.h"
#include "A429_Pulses.h"
#include "a429_Menu.h"
#include "Delay.h"

void Draw_Get_A429_Label_Screen();
int8_t Get_A429_Label();
void Show_Label_Error();
void Draw_Get_SDI_Screen();
int Get_SDI();
void Show_SDI_Error();
void Draw_Get_SSM_Screen();
int Get_SSM();
void Show_SSM_Error();
void Draw_Get_A429_Data_Screen();
int32_t  Get_A429_Data();
void Show_A429_Data_Error();
int32_t ASCII_Hex_to_Int(char *hex);
int8_t ASCII_key_to_SSM(char key);
int8_t ASCII_key_to_SDI(char key);
int16_t ASCII_Hex_to_Oct(char *hex);
int ASCII_Octal_to_Octal_Label(char *str);

//////////// GET ARINC 429 LABEL FUNCTIONS /////////////////////////////////
void Draw_Get_A429_Label_Screen() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter Label in Octal");
  lcd.setCursor(0, 1);
  lcd.print("Values: 000 to 377");
  lcd.setCursor(0, 2);
  lcd.print("Label[");
  lcd.print(TX_Buffer_Index + 1);
  lcd.print("]: ");
}

int8_t Get_A429_Label() {
  //String   keyBuffer[3];
  //String keyBuffer = "000";
  char key = 0;
  char keyBuffer[] = "000";
  int b_index = 0, fullBuff = 0, enter = 0;
  int label;

  Draw_Get_A429_Label_Screen();
  lcd.setCursor(9, 2);
  lcd.blink();

  //key = waitForKey();
  do {
    while (!(key = I2C_Keypad.getKey())) { }
    if (b_index == 3) {
      fullBuff = 1;
      switch (key) {
        case '#':
          if (ALT) {
            ALT = !ALT;
            //digitalWrite(ALT_KEY, LOW);
            PORTH &= ~(1 << PH5);
          }
          enter = 1;
          Serial.println(" Enter.");
          break;
        case '*':
          ALT = !ALT;
          if (ALT) {
            //digitalWrite(ALT_KEY, HIGH);
            PORTH |= (1 << PH5);
          }
          else {
            //digitalWrite(ALT_KEY, LOW);
            PORTH &= ~(1 << PH5);
          }
          break;
        case 'A':
          if (ALT) {
            key = ' ';
            b_index--;
            keyBuffer[b_index] = key;
            Serial.print(key);
            lcd.setCursor(9 + b_index, 2);
            lcd.print(key);
            lcd.setCursor(9 + b_index, 2);
          }
          break;
        default:
          break;
      }
    }
    else {
      switch (key) {
        case '*':
          ALT = !ALT;
          if (ALT) {
            //digitalWrite(ALT_KEY, HIGH);
            PORTH |= (1 << PH5);
          }
          else {
            //digitalWrite(ALT_KEY, LOW);
            PORTH &= ~(1 << PH5);
          }
          break;

        case 'A':
          if (ALT && (b_index > 0)) {
            key = ' ';
            b_index--;
            keyBuffer[b_index] = key;
            Serial.print(key);
            lcd.setCursor(9 + b_index, 2);
            lcd.print(key);
            lcd.setCursor(9 + b_index, 2);
          }
          break;

        case '#':
          /* Modify to be able to introduce only one or two numbers */
          if (b_index > 0){
            int moves = 3 - b_index;
            char first = keyBuffer[0];
            char second = keyBuffer[1];
            keyBuffer[0] = '0';
            for (int i=0; i<moves; i++){
              keyBuffer[1] = first;
              keyBuffer[2] = second;
              first = keyBuffer[0];
              second = keyBuffer[1];
            }
            if (ALT) {
              ALT = !ALT;
              //digitalWrite(ALT_KEY, LOW);
              PORTH &= ~(1 << PH5);
            }
            enter = 1;
          }
          break;

        case 'B':
          break;
        case 'C':
          break;
        case 'D':
          break;
        case '8':
          break;
        case '9':
          break;

        default:
          keyBuffer[b_index] = key;
          Serial.print(key);
          lcd.setCursor(9 + b_index, 2);
          lcd.print(key);
          b_index++;
          break;
      }
    }
  } while (enter == 0);

  //Serial.print('\n');
  Serial.print("Octal Label in Buffer: ");
  Serial.println(keyBuffer);

  //Convert octal string label to octal label.
  label = ASCII_Octal_to_Octal_Label(keyBuffer);
  Serial.print("Label in OCT: ");
  Serial.println(label, OCT);
  if (label == -1) {
    Show_Label_Error();
    lcd.noBlink();
    Serial.print("Error. ");
    Serial.println(label);
    return -1;
  }
  
 // if ((label > 0377) || (label < 0000)) {
  if (label > 0377) {
    Show_Label_Error();
    lcd.noBlink();
    Serial.print("Not in range. ");
    Serial.println(label);
    return (-1);
  }
  else {
    Serial.print("Label in Decimal Converted in Octal: ");
    Serial.print(label);
    Serial.print("  ");
    Serial.println(label, OCT);
    lcd.noBlink();
    return (label);
  }
}

void Show_Label_Error() {
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Error Label value!!");
  Serial.println("Show Error");
  delay(2000);
}

//////////// GET ARINC 429 SDI FUNCTIONS /////////////////////////////////
void Draw_Get_SDI_Screen() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter SDI");
  lcd.setCursor(0, 1);
  lcd.print("Values: 0 to 3");
  lcd.setCursor(0, 2);
  lcd.print("SDI[");
  lcd.print(TX_Buffer_Index + 1);
  lcd.print("]: ");
}

int Get_SDI()
{
  char key;
  char keyBuffer = "0";
  int b_index = 0, enter = 0;
  int SDI;

  Draw_Get_SDI_Screen();
  lcd.setCursor(7, 2);
  lcd.blink();

  do {
    while (!(key = I2C_Keypad.getKey())) { }
    if (b_index == 1) {
      switch (key) {
        case '#':
          if (ALT) {
            ALT = !ALT;
            //digitalWrite(ALT_KEY, LOW);
            PORTH &= ~(1 << PH5);
          }
          enter = 1;;
          break;
        case '*':
          ALT = !ALT;
          if (ALT) {
            //digitalWrite(ALT_KEY, HIGH);
            PORTH |= (1 << PH5);
          }
          else {
            //digitalWrite(ALT_KEY, LOW);
            PORTH &= ~(1 << PH5);
          }
          break;
        case 'A':
          if (ALT) {
            key = ' ';
            b_index--;
            keyBuffer = key;
            Serial.print(key);
            lcd.setCursor(7, 2);
            lcd.print(key);
            lcd.setCursor(7, 2);
          }
          break;
        default:
          break;
      }
    }
    else {
      switch (key) {
        case '*':
          ALT = !ALT;
          if (ALT) {
            //digitalWrite(ALT_KEY, HIGH);
            PORTH |= (1 << PH5);
          }
          else {
            //digitalWrite(ALT_KEY, LOW);
            PORTH &= ~(1 << PH5);
          }
          break;

        case 'A':
          if (ALT && (b_index > 0)) {
            key = ' ';
            b_index--;
            keyBuffer = key;
            Serial.print(key);
            lcd.setCursor(7, 2);
            lcd.print(key);
            lcd.setCursor(7, 2);
          }
          break;

        case '0':
          keyBuffer = key;
          Serial.print(key);
          lcd.setCursor(7, 2);
          lcd.print(key);
          b_index++;
          break;
        case '1':
          keyBuffer = key;
          Serial.print(key);
          lcd.setCursor(7, 2);
          lcd.print(key);
          b_index++;
          break;
        case '2':
          keyBuffer = key;
          Serial.print(key);
          lcd.setCursor(7, 2);
          lcd.print(key);
          b_index++;
          break;
        case '3':
          keyBuffer = key;
          Serial.print(key);
          lcd.setCursor(7, 2);
          lcd.print(key);
          b_index++;
          break;

        default:
          break;
      }
    }
  } while (enter == 0);

  SDI =  ASCII_key_to_SDI(keyBuffer);

  Serial.print("SDI: ");
  Serial.println(SDI);
  //Check Label range
  lcd.noBlink();
  if (SDI == -1) {
    Show_SDI_Error();
    return -1;
  }
  else {
    return (SDI) ;
  }
}

void Show_SDI_Error() {
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Error SDI value!!");
  delay(2000);
}

//////////// GET ARINC 429 DATA FUNCTIONS /////////////////////////////////
void Draw_Get_A429_Data_Screen() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter Data");
  lcd.setCursor(0, 1);
  lcd.print("Values between:");
  lcd.setCursor(0, 2);
  lcd.print("00000 to 7FFFF");
  lcd.setCursor(0, 3);
  lcd.print("Data[");
  lcd.print(TX_Buffer_Index + 1);
  lcd.print("]: ");
}

int32_t  Get_A429_Data()
{
  char keyBuffer[] = "00000";
  int b_index = 0, fullBuff = 0, enter = 0;
  int32_t data; //Because data have 3 bytes (19 bits). int32_t to allow -1 for errors
  char key;

  Draw_Get_A429_Data_Screen();

  lcd.setCursor(8, 3);
  lcd.blink();

  //key = waitForKey();
  do {
    while (!(key = I2C_Keypad.getKey())) { }
    if (b_index == 5) {
      fullBuff = 1;
      switch (key) {
        case '#':
          if (ALT) {
            ALT = !ALT;
            //digitalWrite(ALT_KEY, LOW);
            PORTH &= ~(1 << PH5);
          }
          enter = 1;;
          break;
        case '*':
          ALT = !ALT;
          if (ALT) {
            //digitalWrite(ALT_KEY, HIGH);
            PORTH |= (1 << PH5);
          }
          else {
            //digitalWrite(ALT_KEY, LOW);
            PORTH &= ~(1 << PH5);
          }
          break;
        case 'A':
          if (ALT) {
            key = ' ';
            b_index--;
            keyBuffer[b_index] = key;
            Serial.print(key);
            lcd.setCursor(8 + b_index, 3);
            lcd.print(key);
            lcd.setCursor(8 + b_index, 3);
          }
          break;
        default:
          break;
      }
    }
    else {
      switch (key) {
        case '*':
          ALT = !ALT;
          if (ALT) {
            //digitalWrite(ALT_KEY, HIGH);
            PORTH |= (1 << PH5);
          }
          else {
            //digitalWrite(ALT_KEY, LOW);
            PORTH &= ~(1 << PH5);
          }
          break;

        case 'A':
          if (ALT && (b_index > 0)) {
            key = ' ';
            b_index--;
            keyBuffer[b_index] = key;
            Serial.print(key);
            lcd.setCursor(8 + b_index, 3);
            lcd.print(key);
            lcd.setCursor(8 + b_index, 3);
          }
          else {
            keyBuffer[b_index] = key;
            Serial.print(key);
            lcd.setCursor(8 + b_index, 3);
            lcd.print(key);
            b_index++;
          }
          break;

        case 'C':
          if (ALT) {
            key = 'E';
          }
          keyBuffer[b_index] = key;
          Serial.print(key);
          lcd.setCursor(8 + b_index, 3);
          lcd.print(key);
          b_index++;
          break;
        case 'D':
          if (ALT) {
            key = 'F';
          }
          keyBuffer[b_index] = key;
          Serial.print(key);
          lcd.setCursor(8 + b_index, 3);
          lcd.print(key);
          b_index++;
          break;

        case '#':
          /* Modify to be able to introduce only one, two or three numbers */
          if (b_index > 0){
            int moves = 5 - b_index;
            char first = keyBuffer[0];
            char second = keyBuffer[1];
            char third = keyBuffer[2];
            char fourth = keyBuffer[3];
            keyBuffer[0] = '0';
            for (int i=0; i<moves; i++){
              keyBuffer[1] = first;
              keyBuffer[2] = second;
              keyBuffer[3] = third;
              keyBuffer[4] = fourth;
              first = keyBuffer[0];
              second = keyBuffer[1];
              third = keyBuffer[2];
              fourth = keyBuffer[3];
            }
            if (ALT) {
              ALT = !ALT;
              //digitalWrite(ALT_KEY, LOW);
              PORTH &= ~(1 << PH5);
            }
            enter = 1;
          }
          break;

        default:
          keyBuffer[b_index] = key;
          Serial.print(key);
          lcd.setCursor(8 + b_index, 3);
          lcd.print(key);
          b_index++;
          break;
      }
    }
  } while (enter == 0);

  data = ASCII_Hex_to_Int (keyBuffer);
  if (data == -1) {
    Show_A429_Data_Error();
    lcd.noBlink();
    return -1;
  }

  //Check Data range
  if ((data > 0x7FFFF) || (data < 0x0)) {
    Show_A429_Data_Error();
    lcd.noBlink();
    return -1;
  }
  else {
    //delay(2000);
    //Test Data
    Serial.println("");
    Serial.print("Data: ");
    Serial.println(data);
    Serial.print("Data +1 = ");
    Serial.println(data + 1, HEX);
    lcd.noBlink();
    return (data) ;
  }
}

void Show_A429_Data_Error() {
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Error A429 Data value!!");
  delay(3000);
}

//////////// GET ARINC 429 SSM FUNCTIONS /////////////////////////////////
void Draw_Get_SSM_Screen() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter SSM");
  lcd.setCursor(0, 1);
  lcd.print("Values: 0 to 3");
  lcd.setCursor(0, 2);
  lcd.print("SSM[");
  lcd.print(TX_Buffer_Index + 1);
  lcd.print("]: ");
}

int Get_SSM()
{
  char key;
  char keyBuffer = "0";
  int b_index = 0, enter = 0;
  int SSM;
  Draw_Get_SSM_Screen();
  lcd.setCursor(7, 2);
  lcd.blink();

  do {
    while (!(key = I2C_Keypad.getKey())) { }
    if (b_index == 1) {
      switch (key) {
        case '#':
          if (ALT) {
            ALT = !ALT;
            //digitalWrite(ALT_KEY, LOW);
            PORTH &= ~(1 << PH5);
          }
          enter = 1;;
          break;
        case '*':
          ALT = !ALT;
          if (ALT) {
            //digitalWrite(ALT_KEY, HIGH);
            PORTH |= (1 << PH5);
          }
          else {
            //digitalWrite(ALT_KEY, LOW);
            PORTH &= ~(1 << PH5);
          }
          break;
        case 'A':
          if (ALT) {
            key = ' ';
            b_index--;
            keyBuffer = key;
            Serial.print(key);
            lcd.setCursor(7, 2);
            lcd.print(key);
            lcd.setCursor(7, 2);
          }
          break;
        default:
          break;
      }
    }
    else {
      switch (key) {
        case '*':
          ALT = !ALT;
          if (ALT) {
            //digitalWrite(ALT_KEY, HIGH);
            PORTH |= (1 << PH5);
          }
          else {
            //digitalWrite(ALT_KEY, LOW);
            PORTH &= ~(1 << PH5);
          }
          break;

        case 'A':
          if (ALT && (b_index > 0)) {
            key = ' ';
            b_index--;
            keyBuffer = key;
            Serial.print(key);
            lcd.setCursor(7, 2);
            lcd.print(key);
            lcd.setCursor(7, 2);
          }
          break;

        case '0':
          keyBuffer = key;
          Serial.print(key);
          lcd.setCursor(7, 2);
          lcd.print(key);
          b_index++;
          break;
        case '1':
          keyBuffer = key;
          Serial.print(key);
          lcd.setCursor(7, 2);
          lcd.print(key);
          b_index++;
          break;
        case '2':
          keyBuffer = key;
          Serial.print(key);
          lcd.setCursor(7, 2);
          lcd.print(key);
          b_index++;
          break;
        case '3':
          keyBuffer = key;
          Serial.print(key);
          lcd.setCursor(7, 2);
          lcd.print(key);
          b_index++;
          break;

        default:
          break;
      }
    }
  } while (enter == 0);

  SSM =  ASCII_key_to_SSM(keyBuffer);
  lcd.noBlink();
  //Check Label range
  if (SSM == -1) {
    Show_SSM_Error();
    return -1;
  }
  else
    return (SSM) ;
}

void Show_SSM_Error() {
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Error SSM value!!");
  delay(2000);
}


///////////////  ASCII from keyboard input Turn into values  ////////////////
/**
   hex2int
   take a hex string and convert it to a 32bit number (max 8 hex digits)
*/
int32_t ASCII_Hex_to_Int(char *hex) {
  uint32_t val = 0;
  while (*hex) {
    // get current character from the "hex string" and  then increment
    char byte = *hex++;
    // transform hex character to the 4bit equivalent number, using the ascii table indexes
    if (byte >= '0' && byte <= '9')
      byte = byte - '0';
    else if ((byte >= 'a') && (byte <= 'f'))
      byte = byte - 'a' + 10;
    else if ((byte >= 'A') && (byte <= 'F'))
      byte = byte - 'A' + 10;
    else
      return -1; //Error!!!!
    // shift 4 to make space for new digit, and add the 4 bits of the new digit
    val = (val << 4) | (byte & 0x0F);
  }
  return val;
}

int8_t ASCII_key_to_SSM(char key) {
  int8_t val = 0;

  // get current ASCII character. Transform hex character to the 4bit equivalent number, using the ascii table indexes
  if (key >= '0' && key <= '3')
    val = key - '0';
  else
    return -1; // SDI Error
  return val;
}

int8_t ASCII_key_to_SDI(char key) {
  int8_t val = 0;

  // get current ASCII character. Transform hex character to the 4bit equivalent number, using the ascii table indexes
  if (key >= '0' && key <= '3')
    val = key - '0';
  else
    return -1; // SDI Error

  return val;
}

int16_t ASCII_Hex_to_Oct(char *hex) {
  int16_t val = 0;
  while (*hex) {
    // get current character from the "hex string" and  then increment
    char byte = *hex++;
    // transform hex character to the 4bit equivalent number, using the ascii table indexes
    if (byte >= '0' && byte <= '9') byte = byte - '0';
    else if (byte >= 'a' && byte <= 'f') byte = byte - 'a' + 10;
    else if (byte >= 'A' && byte <= 'F') byte = byte - 'A' + 10;
    // shift 4 to make space for new digit, and add the 4 bits of the new digit
    val = (val << 3) | (byte & 0x07);
  }
  return val;
}

/*
   ASCII_Octal_to_Octal_Label
   take an octal string and convert it into an int number that is the label intered in Octal)
*/
int ASCII_Octal_to_Octal_Label(char *str)
{
  int label = 0;
  char number;

  Serial.print("String to be converted: ");
  Serial.println(str);
  int ct = 0;
  while (*str) {
    // get current character from the "hex string" and  then increment
    number = *str++;
    Serial.print("Label number pos: ");
    Serial.println(ct);
    // transform hex character to the 4bit equivalent number, using the ascii table indexes

    //Check for Errors:
    // '0' <----> '7'
    if ((number >= '0') && (number <= '7'))
    {
      number = number - '0';
      //break;
      //continue;
    }
    else
      return -1; // Error!!!!

    // shift 3 to make space for new digit, and add the 3 bits of the new octal digit
    label = (label << 3);
    label = label | (number & 0x07);
    Serial.print("Octal: ");
    Serial.println(label, OCT);
    ct++;
  }
  Serial.print("Converted: ");
  Serial.println(label);
  return label;
}//ASCII_Octal_to_Octal_Label(char *str)

#endif


/////////////////// OLD CODE /////////////////////////////
/* UNUSED
  Get_SDI() {
  while (!(key = I2C_Keypad.getKey())) {
  }
  Serial.print("key: ");
  Serial.println(key);
  lcd.print(key);
  }

  Get_Data() {
  for (int i = 0; i <= 4; i++) {
    while (!(key = I2C_Keypad.getKey())) {
    }
    if (key == '*') {
      i--; //Do not take into account
      ALT = !ALT; //Change last state
      if (ALT) {
        digitalWrite(ALT_KEY, HIGH);
        continue;
      }
      else {
        digitalWrite(ALT_KEY, LOW);
        continue;
      }
    }

    if (ALT) {
      switch (key) {
        case ('C'):
          key = 'E';
          break;

        case ('D'):
          key = 'F';
          break;
        default:
          ;
      }//switch
    }//if ALT

    keyBuffer[i] = key;
    Serial.print(key);
    lcd.setCursor(8 + i, 3);
    lcd.print(key);
    //Not useful, ALT_KEY pin is set LOW after one single key is pushed
    //digitalWrite(ALT_KEY, LOW);
  } //for(
  }

*/
