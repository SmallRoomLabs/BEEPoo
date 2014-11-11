//
// BEEPoo - Standalone AY-3-8912 player with ATMEGA32U4 with Arduino bootloader
//
// Copyright (c) 2014 SmallRoomLabs / Mats Engstrom - mats.engstrom@gmail.com
// 
// Hardware and Firmware released under the MIT License (MIT) than can be
// found in full in the file named LICENCE in the git repository.
//
// https://github.com/SmallRoomLabs/BEEPoo
//

//
//   PB0 SD_CS       RXLED
//   PB1 SCK         SCK
//   PB2 MOSI        MOSI
//   PB3 MISO        MISO
//   PB4 Rotary Q1   Digital pin 8 
//   PB5 Rotary Q2   Digital Pin 9 (PWM)
//   PB6 AY_BC1      Digital Pin 10 (PWM)
//   PB7             Digital pin 11 (PWM)
//   
//   PC6 AY_RESET#   Digital Pin 5 (PWM)
//   PC7 AY_CLK      Digital Pin 13 (PWM)
//   
//   PD0 AY_Data0    Digital pin 3 (SCL)(PWM)
//   PD1 AY_Data1    Digital pin 2 (SDA)
//   PD2 AY_Data2    Digital pin 0 (RX)
//   PD3 AY_Data3    Digital pin 1 (TX)
//   PD4 AY_Data4    Digital pin 4
//   PD5 AY_Data5    TXLED
//   PD6 AY_Data6    Digital pin 12
//   PD7 AY_Data7    Digital Pin 6 (PWM)
// 
//   PE2 AY_Dir      HWB
//   PE6 LCD_CS      Digital pin 7
//   
//   PF0 Button      Analog In 5
//   PF1 Button      Analog In 4
//   PF4 Button      Analog In 3
//   PF5 Button      Analog In 2
//   PF6 Button      Analog In 1
//   PF7 Rotary But  Analog In 0
// 
// 

#include <stdint.h>
#include "spi.h"
#include "timer4.h"
#include "ay38912.h"
#include "nokia1202.h"
#include "pff.h"

int AY_RESET    = 5;        // D5
int AY_CLK      = 13;       // D13
int AY_BC1      = 10;       // D10
int DEBUG_LED   = 11;       // D11

#define DEBUGLED_ON  PORTB |= B10000000
#define DEBUGLED_OFF PORTB &= B01111111


//
//
//
void setup() {
  // PB0 o SD_CS       RXLED
  // PB1 o SCK         SCK
  // PB2 o MOSI        MOSI
  // PB3 i MISO        MISO
  // PB4 i Rotary Q1   Digital pin 8 
  // PB5 i Rotary Q2   Digital Pin 9 (PWM)
  // PB6 o AY_BC1      Digital Pin 10 (PWM)
  // PB7 o DEBUGLED    Digital pin 11 (PWM)
  DDRB=0x00 | (1<<0) | (1<<1) | (1<<2) | (1<<6) | (1<<7);
  PORTB=0x00 | (1<<3) | (1<<4) | (1<<5);  // Enable pullup

  // PC6 o AY_RESET#   Digital Pin 5 (PWM)
  // PC7 o AY_CLK      Digital Pin 13 (PWM)
  DDRC=0x00 | (1<<6) | (1<<7);
  PORTC=0x00;  

  // PD0 o AY_Data0    Digital pin 3 (SCL)(PWM)
  // PD1 o AY_Data1    Digital pin 2 (SDA)
  // PD2 o AY_Data2    Digital pin 0 (RX)
  // PD3 o AY_Data3    Digital pin 1 (TX)
  // PD4 o AY_Data4    Digital pin 4
  // PD5 o AY_Data5    TXLED
  // PD6 o AY_Data6    Digital pin 12
  // PD7 o AY_Data7    Digital Pin 6 (PWM)
  DDRD=0xFF;
  PORTD=0x00;

  // PE2 o AY_Dir      HWB
  // PE6 o LCD_CS      Digital pin 7
  DDRE=0x00 | (1<<2) | (1<<6);
  PORTE=0x00;  

  // PF0 Button      Analog In 5
  // PF1 Button      Analog In 4
  // PF4 Button      Analog In 3
  // PF5 Button      Analog In 2
  // PF6 Button      Analog In 1
  // PF7 Rotary But  Analog In 0
  DDRF=0x00;
  PORTF=0x00;  

  // Reset 
  digitalWrite(AY_RESET,LOW);
  digitalWrite(AY_CLK,LOW);
  digitalWrite(DEBUG_LED,LOW);

  // Start timer4 output to D13
  InitTimer4(1000000);
  SetTimer4Duty(512);
  Timer4Start();
  
  // Delay 1 ms before bringing AY out of reset
  delay(1);    
  digitalWrite(AY_RESET,HIGH);

  // Enable PORT A on the AY-3-8912 and turn off all leds
  AyRegister(AY_ENABLE,B01000000);
  AyPort(0x00); 

  DEBUGLED_ON;
}

FATFS  fs;

//
//
//
void loop() {
  int res;
  int i;
  char buf[16];
  unsigned int br;
  
  DEBUGLED_OFF;
  delay(500);
  DEBUGLED_ON;
  LcdInit();
  delay(500);

  LcdXY(0,0);LcdString("Mount=");
  res=pf_mount(&fs);
  LcdCharacter(48+res);

  LcdXY(0,1);LcdString("Open=");
  res=pf_open("ABCDEFGH.IJK");
  LcdCharacter(48+res);
  
  LcdXY(0,2);LcdString("Read=");
  for (i=0; i<16; i++) buf[i]='@';
  res=pf_read(buf, 16, &br);    /* Read data to the buff[] */
  LcdCharacter(48+res);

  LcdXY(0,3);
  for (i=0; i<16; i++) {
    LcdCharacter(buf[i]);
  }


  DEBUGLED_OFF;
  for (;;);
}
