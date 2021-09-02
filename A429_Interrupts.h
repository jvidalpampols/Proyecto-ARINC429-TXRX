// My_Interrupts for DEI1016_Tools for A429_Test_hardware_6.ino
#ifndef MY_INTERRUPTS_H
#define MY_INTERRUPTS_H

#include "Arduino.h"
#include "A429_Define.h"
#include "A429_Pulses.h"
#include "DEI1016_Tools.h"

volatile unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled

///////////////////  ISR Interrupts Service Routines ////////////////////////

void ISR_Int_RX1();
void ISR_Int_RX2();
void ISR_TX_Buffer_Empty();

/*
void Do_Reset();

void Do_Reset() {
  asm volatile ("jmp 0"); // Do RESET from software
  }
*/

/////////////////////  ISR Interrupts Service Routines ///////////////////


void ISR_Int_RX1 () {
  INT_RX1_Ready  = true;
  ISR_RX1_Times++; // Flag for Auto Testing wrap around
  ISR_RX1_Count ++; // Debugging  Flag
}

void ISR_Int_RX2() {
  Int_RX2_Ready = true;
  ISR_RX2_Times++;        // Debugging Flag
}

void ISR_TX_Buffer_Empty() {
  INT_DEI_TX_Buffer_Empty_Flag = true; // When the Flag is true, Load_DEI_TX_Buffer() will be called in the loop()
  Buffer_Flag_Interrupts ++;  // To check  ISR the program
} //ISR_TX_Buffer_Empty

#endif
