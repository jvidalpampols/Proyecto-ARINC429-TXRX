// My_Interrupts for DEI1016_Tools for A429_Test_hardware_6.ino
#ifndef MY_INTERRUPTS_H
#define MY_INTERRUPTS_H

///////////////////////////////// Changed files XXX.h names ////////////////////
//#include "My_Define.h"
//#include "My_Pulses.h"
/////////////////////////////////////////////////////////////////////////////////

#include "Arduino.h"
#include "A429_Define.h"
#include "A429_Pulses.h"
#include "DEI1016_Tools.h"


//////////////////////////////////////////ISR flags ///////////////////////////////////////////////////
//volatile byte Int_RX1_Ready = false; //Flag for INT_RX1. To be set 'true' for ISR and reset to 'false' when serviced into loop().
//volatile byte Int_RX2_Ready = false; //Flag for INT_RX2. To be set 'true' for ISR and reset to 'false' when serviced into loop().

/////////////////////////////////////ISR_Scroll, is not used anymore. /////////////////////////////////////
//volatile byte Int_Scroll = false; //Flag for Push Buttom "Scroll" TX or RX. To be set 'true' for ISR and reset to 'false' when serviced into loop().
//volatile byte Int_Scroll_TX = false; //Flag for Push Buttom "Scroll". To be set 'true' when checked during routine to check wheather RX or TX have been pressed.
//volatile byte Int_Scroll_RX = false; //Flag for Push Buttom "Scroll". To be set 'true' when checked during routine to check wheather Scroll RX or Scroll TX have been pressed.
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

//volatile byte Int_DEI_TX_Buffer_Empty_Flag = false; //To Flag when TX Buffer is empty. This Flag in the loop(), will call to Load_DEI_TX_Buffer() if it notalready busy: "TX_Buffer_Busy"
//To be set 'true' for ISR and reset to 'false' when serviced into loop().
volatile unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled

//volatile byte Flag_TX = false;
//volatile byte Flag_RX = false;
//volatile byte  TX_ON = false;  // On every 50msec.
//volatile int  Buffer_Flag_Interrupts = 0;
//volatile int TX_Buffer_Busy = false; // Flag to not allow further Load_TX_Buffer(); while is loading the TX_Buffer;
//volatile int RX1_Buffer_Busy = false; // Flag to not allow further RX_Buffer(); while is working in the RX1_Buffer;
//volatile unsigned int Times_TX_Buffer_Busy = 0; //Debug how many times ISR TX Buffer empty while TX_Buffer still loading
//volatile unsigned int ISR_RX1_Times = 0;   // Debug FLAG, how many times ISR RX1 is called.
//volatile unsigned int ISR_RX2_Times = 0;   // Debug FLAG, how many times ISR RX2 is called.



///////////////////  ISR Interrupts Service Routines ////////////////////////

void ISR_Int_RX1();
void ISR_Int_RX2();
void ISR_TX_Buffer_Empty();
//void ISR_Scroll(); // Deleted Scroll by Push Buttons
//void Do_Reset();

/*
  void Do_Reset() {
  asm volatile ("jmp 0"); // Do RESET from software
  }
*/

/////////////////////  ISR Interrupts Service Routines ///////////////////


void ISR_Int_RX1 () {
  Int_RX1_Ready  = true;
  ISR_RX1_Times++; // Debugging Flag
  ISR_RX1_Count ++; // Debugging  Flag
}

void ISR_Int_RX2() {
  Int_RX2_Ready = true;
  ISR_RX2_Times++;        // Debugging Flag
}

void ISR_TX_Buffer_Empty() {
  Int_DEI_TX_Buffer_Empty_Flag = true; // When the Flag is true, Load_DEI_TX_Buffer() will be called in the loop()
  Buffer_Flag_Interrupts ++;  // To check  ISR the program
} //ISR_TX_Buffer_Empty


////////////////////////////  Deleted Scroll by Push Buttons
/*
  void ISR_Scroll() {
  //  Debounce code only for Testing
  if ((millis() - lastDebounceTime) > BounceDelay) {
    Int_Scroll = true;
    lastDebounceTime = millis();
    if (digitalRead(TX_SCROLL) == LOW) {
      Int_Scroll_TX = true;
      return;
    }

    else if (digitalRead(RX_SCROLL) == LOW) {
      Int_Scroll_RX = true;
      return;
    }
    else
      ;
  }//if millis()
  }
*/
////////////////////////////////////////


#endif
