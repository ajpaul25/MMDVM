#ifndef INTERFACECAL_H
#define INTERFACECAL_H

#include "InterfaceTX.h"

class InterfaceCal {
public:
    InterfaceCal(void);
    virtual ~InterfaceCal(void);
    virtual void process();
    InterfaceTX tx;
    virtual uint8_t setConfig(const uint8_t* data, uint16_t length);
};

#endif // INTERFACECAL_H