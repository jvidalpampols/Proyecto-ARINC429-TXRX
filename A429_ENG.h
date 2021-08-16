// ARINC 429 Labels decoding for Engineering Units
#ifndef MY_A429_ENG_H
#define MY_A429_ENG_H

float Label_211_TAT (uint32_t A429_data);
float Label_001_Distance_To_Go (uint32_t A429_data);


float Label_211_TAT (uint32_t A429_data) {
  /*
     Label:211
     Coded:BNR
     ID: 0AD
     Parameter:Total Air Temperature Indicated
     Units:Degree C
     Range:+/-512
     Bits (sign) 12: b29(sign) b28(256) b27(128) b26(64) b25(32) b24(16) b23(8) b22(4) b21(2) b20(1) b19(0,5) b18(0,25)>> 1 bit sign + 11 bits number
     Resolution: 0.125
     Time Refresh Minimun: 250msec.
     Time Refresh Minimun: 500msec.
     Example-25ºC: 1b29 (-) 1(b28) 1(b27) 1(b26) 1(b25) 0(b24) 0(b23) 1b(22) 1(b21) 1(b20) 0(b19) 0(b18)
  */

  uint32_t My_data = 0;
  uint32_t Mask = 0b10000000000;
  float TAT = 0;
  int16_t Power_2 = 1;
  uint8_t My_sign = 0;
  uint8_t My_bit = 0;

  My_sign = A429_data >> 11;// Get sign bit +(0)/-(1)
  My_data = A429_data >> 1;// Get rid of sign bit
  My_data = (~My_data) + 1; // Complemento a 2

  for (uint8_t i = 0; i <= 10; i++) {
    Power_2 = Power_2 * 2;
    Mask = Mask >> 1;
    /*
      Serial.print("i");
      Serial.print(i);
      Serial.print("   Mask: ");
      Serial.println(Mask, BIN);
    */
    if (My_data & Mask) {
      TAT = (TAT + (512 / Power_2));
    }
    /*
      Serial.print("My_data&Mask: ");
      Serial.println(My_data & Mask);
      Serial.print("TAT:");
      Serial.println(TAT);
    */
  }

  //Sign
  if (My_sign) {
    TAT = TAT * (-1);
  }
}//float Label_211_Distance_TAT (uint32_t A429_data)


float Label_001_Distance_To_Go (uint32_t A429_data) {

  float My_Data = 0;
  uint32_t BCD_1; //Millares
  uint32_t BCD_2; //Centenas
  uint32_t BCD_3; //Decenas
  uint32_t BCD_4; //Unidades
  uint32_t BCD_5; //Decimal

  BCD_1 =  A429_data >> 16;
  //Serial.println(BCD_1, DEC);
  BCD_2 =  A429_data & 0b0001111111111111111;
  BCD_2 = BCD_2 >> 12;
  //Serial.println(BCD_2, DEC);
  BCD_3 = A429_data & 0b0000000111111111111;
  BCD_3 = BCD_3 >> 8;
  //Serial.println(BCD_3, DEC);
  BCD_4 = A429_data & 0b0000000000011111111;
  BCD_4 = BCD_4 >> 4;
  //Serial.println(BCD_4, DEC);
  BCD_5 = A429_data & 0b0000000000000001111;
  //Serial.println(BCD_5, DEC);

  /*
    Serial.print(BCD_1);
    Serial.print(BCD_2);
    Serial.print(BCD_3);
    Serial.print(BCD_4);
    Serial.print(",");
    Serial.print(BCD_5);
  */

  My_Data = (float)(BCD_5) / 10; //Tof orce type convertion (BCD) to float
  My_Data = My_Data + BCD_4;
  My_Data = My_Data + BCD_3 * 10;
  My_Data = My_Data + BCD_2 * 100;
  My_Data = My_Data + BCD_1 * 1000;
  return My_Data;
}//float Label_001_Distance_To_Go (uint32_t A429_data)

#endif // MY_A429_ENG_H
