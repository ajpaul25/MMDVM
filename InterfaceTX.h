#ifndef INTERFACETX_H
#define INTERFACETX_H

#include "base.h"
#include "InterfaceRX.h"

class InterfaceTX {
public:
    InterfaceTX(void);
    virtual ~InterfaceTX(void);
    virtual void process();
    virtual uint8_t processMessage(uint8_t type, const uint8_t* buffer, uint16_t length);
    virtual uint8_t getSpace() const;
    virtual uint8_t getSpace(uint8_t index) const;
    virtual uint8_t setConfig(const uint8_t* data, uint16_t length);
    virtual void setStart(bool start);
    virtual uint8_t writeData(const uint8_t* data, uint16_t length);
    InterfaceRX rx;
};

#endif // INTERFACETX_H