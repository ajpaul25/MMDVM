#ifndef INTERFACERX_H
#define INTERFACERX_H

#if defined(STM32F4XX)
#include "stm32f4xx.h"
#elif defined(STM32F7XX)
#include "stm32f7xx.h"
#else
#include <Arduino.h>
#undef PI //Undefine PI to get rid of annoying warning as it is also defined in arm_math.h.
#endif

#if defined(__SAM3X8E__)
#define  ARM_MATH_CM3
#elif defined(STM32F7XX)
#define  ARM_MATH_CM7
#elif defined(STM32F4XX) || defined(__MK20DX256__) || defined(__MK64FX512__) || defined(__MK66FX1M0__)
#define  ARM_MATH_CM4
#else
#error "Unknown processor type"
#endif

#include <arm_math.h>
class InterfaceRX {
public:
    InterfaceRX(void);
    virtual ~InterfaceRX(void);
    virtual uint8_t setConfig(const uint8_t* data, uint16_t length);
    virtual void samples(const q15_t* samples, const uint16_t* rssi, uint8_t length);
    virtual void reset();
};

#endif // INTERFACERX_H