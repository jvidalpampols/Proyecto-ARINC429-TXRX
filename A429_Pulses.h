#ifndef MY_PULSES_H
#define MY_PULSES_H
#include "Arduino.h"

/*
  Otro ejemplo es el uso de los métodos AVR cbi y sbi que se usan para hacer set o clear de bits el PORT y otras variables.
  Puedes encontrar estos métodos de código AVR en muchos sitios y ambos deben ser definidos en el sketch de Arduino para ser usados.
  Pegar este código al inicio del sketch y para usarlo simplemente poner una variable del puerto y el pin para hacer set o clear.
*/
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#endif
