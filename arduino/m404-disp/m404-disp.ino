
// M404 - A vacuum fluorescent display monitor for Raspberry Pi.
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

void setup() {
  // put your setup code here, to run once:
  pinMode(pin_busy, INPUT);

  pinMode(pin_blank_, OUTPUT);
  pinMode(pin_test_, OUTPUT);  
  pinMode(pin_reset_, OUTPUT);

  digitalWrite(pin_blank_, HIGH);
  digitalWrite(pin_test_, HIGH);
  digitalWrite(pin_reset_, HIGH);

  delay(1);

  mySerial.begin(BAUD);
  Serial.begin(115200, SERIAL_8N1);

  delay(1000);

  unsigned char dim_cmd[] = { 0x04, 0xff };
  metered_write(dim_cmd, 2);
}

void loop() {
  metered_print("M404 monitor starting...\n\n");

  unsigned long last_millis = millis();
  unsigned long now_millis = last_millis;
  
  while(true) {
    while (Serial.available() > 0) {
      unsigned char c = Serial.read();
      metered_write(c);
      last_millis = millis();
    }
    
    now_millis = millis();
    
    if (now_millis < last_millis) {
      // Handle wraparound
      last_millis = now_millis;
    }
    
    if ((now_millis - last_millis) > 10000) // More than ten seconds since we heard from host?
    {
      metered_write(0x0b);
      metered_write(0x13);
      metered_print("\r\r      *** HOST NOT RESPONDING ***");
      
      while (Serial.available() == 0) {
      }
    }
  }
}
