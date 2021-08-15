//
//  Arduino.cpp
//  AquaOS
//
//  Created by Synium on 04.09.19.
//  Copyright © 2019 Synium. All rights reserved.
//

#include <stdio.h>
#include "BoardLib.h"

#include <SDL2/SDL.h>

#define VELOCITY 1

int analogRead(uint8_t p) {
    return boardReadAnalog(p);
}

int digitalRead(uint8_t pin) {
    return boardReadDigital(pin);
}

void digitalWrite(uint8_t pin, uint8_t value) {
    boardWriteDigital(pin, value);
}

void analogWrite(uint8_t pin, int value) {
    boardWriteAnalog(pin, value);
}

void pinMode(uint8_t pin, uint8_t mode) {
}

unsigned long _micros = 0;

unsigned long micros() {
    _micros += 10;
    return _micros;
}

void delay(unsigned long d) {

}

void delayMicroseconds(unsigned long d) {
}

unsigned int millis() {
    return SDL_GetTicks() * VELOCITY;
}
