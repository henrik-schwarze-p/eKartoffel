//
//  Arduino.h
//  AquaOS
//
//  Created by Kurt Schwarze on 16.03.19.
//  Copyright © 2019 Kurt Schwarze. All rights reserved.
//

#ifndef Arduino_h
#define Arduino_h
#define A0 0
#define A1 1
#define A2 2
#define A3 3
#define A4 4
#define A5 5
#define HIGH 1
#define LOW 0
#define INPUT 1
#define OUTPUT 1
#define PROGMEM

#include <stdint.h>

int          analogRead(uint8_t p);
int          digitalRead(uint8_t pin);
void         pinMode(uint8_t pin, uint8_t mode);
unsigned int millis();
void         digitalWrite(uint8_t pin, uint8_t value);
void         analogWrite(uint8_t pin, int value);

#endif /* Arduino_h */
