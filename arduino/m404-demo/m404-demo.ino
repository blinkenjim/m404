//
// M404 - A vacuum fluorescent display monitor for Raspberry Pi.
//
// This is a demo program that display's Lewis Carroll's poem
// Jabberwocky on the M404SD01 Vacuum Fluorescent Display module.
//
// Copyright (c) 2017 James Thompson
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
// MA 02110-1301, USA.

#include <SoftwareSerial.h>

const unsigned int pin_blank_ = 3;
const unsigned int pin_test_ = 2;
const unsigned int pin_reset_ = 5;
const unsigned int pin_busy = 6;
const unsigned int pin_nc = 9;
const unsigned int pin_rxd = 10;

SoftwareSerial mySerial(pin_nc, pin_rxd);

// We must delay a short time after writing each character, or else
// or the module receives garbled text. 440 microseconds seems to be
// the minimum amount we can delay. Also, we must check the busy
// pin before writing the next character. This prevents garbling when
// the text wraps from the end of the display to the beginning.

#define INTERCHAR_DELAY_ONLY 0

#define BAUD 62500
#define DELAY 440

#if defined(INTERCHAR_DELAY_ONLY) && (INTERCHAR_DELAY_ONLY > 0)
#define INTERCHAR_DELAY delayMicroseconds(DELAY)
#else
#define WAIT for (int busy = HIGH; busy == HIGH; busy = digitalRead(pin_busy)) {} 
#define INTERCHAR_DELAY { WAIT; delayMicroseconds(DELAY); WAIT; }
#endif

void metered_print(const char *s) {
  while (*s) {
    mySerial.write(*s++);
    INTERCHAR_DELAY;
  }
}

void metered_write(const unsigned char *s, unsigned int len) {
  for(int i = 0; i < len; i++) {
    mySerial.write(*s++);
    INTERCHAR_DELAY;
  }
}

void metered_write(const unsigned char c) {
  mySerial.write(c);
  INTERCHAR_DELAY;
}

void setup(){
  // put your setup code here, to run once:
  pinMode(pin_busy, INPUT);

  pinMode(pin_blank_, OUTPUT);
  pinMode(pin_test_, OUTPUT);
  pinMode(pin_reset_, OUTPUT);
 
  digitalWrite(pin_blank_, HIGH);
  digitalWrite(pin_test_, HIGH);
  digitalWrite(pin_reset_, HIGH);

#if 0
  digitalWrite(pin_test_, LOW);
  while (true) {}
#endif

  mySerial.begin(BAUD);

  delay(1000);

  unsigned char dim_cmd[] = { 0x04, 0xff };
  metered_write(dim_cmd, 2);

#if 1
  unsigned char outchar = 32;
  for (int i = 0; i < 160; i++) {
    Serial.write(outchar++);
    INTERCHAR_DELAY;
  }
#endif
}

void loop() {

  const int d1 = 1000;
  const int d2 = 4000;

  metered_write(0x0b);
  metered_write(0x13);
  metered_print("'Twas brillig, and the slithy toves\r");
  delay(d1);
  metered_print("Did gyre and gimble in the wabe:\r");
  delay(d1);
  metered_print("All mimsy were the borogoves,\r");
  delay(d1);
  metered_print("And the mome raths outgrabe.");
  delay(d2);
  metered_write(0x0b);

  metered_print("\"Beware the Jabberwock, my son,\r");
  delay(d1);
  metered_print("The jaws that bite, the claws that catch\r");
  delay(d1);
  metered_write(0x1a);
  metered_print("Beware the Jubjub bird, and shun\r");
  delay(d1);
  metered_print("The frumious Bandersnatch!\"");
  delay(d2);
  metered_write(0x0b);

  metered_print("He took his vorpal sword in hand:\r");
  delay(d1);
  metered_print("Long time the manxome foe he sought--\r");
  delay(d1);
  metered_print("So rested he by the Tumtum tree,\r");
  delay(d1);
  metered_print("And stood a while in thought.");
  delay(d2);

  metered_write(0x0b);
  metered_print("And, as in uffish thought he stood,\r");
  delay(d1);
  metered_print("The Jabberwork, with eyes of flame,\r");
  delay(d1);
  metered_print("Came whiffling through the tulgey wood,\r");
  delay(d1);
  metered_print("And burbled as it came!");
  delay(d2);

  metered_write(0x0b);
  metered_print("One, two! One, two!\rAnd through and through\r");
  delay(d1);
//  metered_write(0x1a);
  metered_print("The vorpal blade went snicker-snack!\r");
  delay(d1);
  metered_print("He left it dead, and with its head\r");
  delay(d1);
  metered_print("He went galumphing back.");
  delay(d2);

  metered_write(0x0b);
  metered_print("\"And hast thou slain the Jabberwock?\r");
  delay(d1);
//  metered_write(0x1a);
  metered_print("Come to my arms, my beamish boy!\r");
  delay(d1);
  metered_print("O frabjous day! Callooh! Callay!\"\r");
  delay(d1);
  metered_print("He chortled in his joy.");
  delay(d2);

  delay(4000);
}
