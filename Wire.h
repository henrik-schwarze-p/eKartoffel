#ifndef Wire_h
#define Wire_h

#include <inttypes.h>
#include <stddef.h>

class TwoWire {
   public:
    TwoWire() {
    }
    void begin(int sda, int scl);
    void begin(int sda, int scl, uint8_t address);
    void pins(int sda, int scl) __attribute__((deprecated));  // use begin(sda, scl) in new code
    void begin() {
    }
    void begin(uint8_t) {
    }
    void begin(int) {
    }
    void setClock(uint32_t) {
    }
    void setClockStretchLimit(uint32_t) {
    }
    void beginTransmission(uint8_t) {
    }
    void beginTransmission(int) {
    }
    uint8_t endTransmission(void) {
        return 0;
    }
    uint8_t endTransmission(uint8_t) {
        return 0;
    }
    size_t requestFrom(uint8_t address, size_t size, bool sendStop) {
        return 0;
    }
    uint8_t status() {
        return 0;
    }

    uint8_t requestFrom(uint8_t, uint8_t) {
        return 0;
    }
    uint8_t requestFrom(uint8_t, uint8_t, uint8_t) {
        return 0;
    }
    uint8_t requestFrom(int, int) {
        return 0;
    }
    uint8_t requestFrom(int, int, int) {
        return 0;
    }

    virtual size_t write(uint8_t) {
        return 1;
    }
    virtual size_t write(const uint8_t*, size_t) {
        return 1;
    }
    virtual int available(void) {
        return 1;
    }
    virtual int read(void) {
        return 1;
    }
    virtual int peek(void) {
        return 1;
    }
    virtual void flush(void){};
    void         onReceive(void (*)(int));     // arduino api
    void         onReceive(void (*)(size_t));  // legacy esp8266 backward compatibility
    void         onRequest(void (*)(void)) {
    }
};

extern TwoWire Wire;

#endif /* Wire_h */
