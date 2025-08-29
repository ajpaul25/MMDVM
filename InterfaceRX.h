#ifndef INTERFACERX_H
#define INTERFACERX_H

#include "base.h"

class InterfaceRX {
public:
    InterfaceRX(void);
    virtual ~InterfaceRX(void);
    virtual uint8_t setConfig(const uint8_t* data, uint16_t length);
    virtual void samples(const q15_t* samples, const uint16_t* rssi, uint8_t length);
    virtual void reset();
protected:
    virtual void writeData(const uint8_t* data, uint8_t length);
    virtual void writeEOT();
    virtual void writeLost();
};

#endif // INTERFACERX_H