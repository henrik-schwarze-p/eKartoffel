/*
 This is a mockup of https://github.com/adafruit/DHT-sensor-library/blob/master/DHT.cpp
 */

#include "DHT.h"

DHT::DHT(uint8_t pin, uint8_t type, uint8_t count) {
}

void DHT::begin(uint8_t usec) {
}

float DHT::readTemperature(bool S, bool force) {
    return 30.0;
}

float DHT::convertCtoF(float c) {
    return c * 1.8 + 32;
}

float DHT::convertFtoC(float f) {
    return (f - 32) * 0.55555;
}

float DHT::readHumidity(bool force) {
    return 10;
}

float DHT::computeHeatIndex(bool isFahrenheit) {
    float hi = computeHeatIndex(readTemperature(isFahrenheit), readHumidity(), isFahrenheit);
    return hi;
}

float DHT::computeHeatIndex(float temperature, float percentHumidity, bool isFahrenheit) {
    return 15;
}

bool DHT::read(bool force) {
    return false;
}

uint32_t DHT::expectPulse(bool level) {
    return 1;
}
