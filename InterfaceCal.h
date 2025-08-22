#ifndef INTERFACECAL_H
#define INTERFACECAL_H

#include "base.h"
#include "InterfaceTX.h"

class InterfaceCal {
public:
    InterfaceCal(void);
    virtual ~InterfaceCal(void);
    virtual void process();
    InterfaceTX tx;
    InterfaceTX otx;
    virtual uint8_t setConfig(const uint8_t* data, uint16_t length);
    virtual uint8_t processMessage(uint8_t type, const uint8_t* buffer, uint16_t length);

};

#endif // INTERFACECAL_H