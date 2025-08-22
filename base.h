#ifndef BASE_H
#define BASE_H

#if defined(STM32F4XX)
#include "stm32f4xx.h"
#elif defined(STM32F7XX)
#include "stm32f7xx.h"
#elif defined(SIMULATOR)
#include "simulator.h"
#else
#include <Arduino.h>
#undef PI
#endif

#if defined(__SAM3X8E__)
#define  ARM_MATH_CM3
#elif defined(STM32F7XX)
#define  ARM_MATH_CM7
#elif defined(STM32F4XX) || defined(__MK20DX256__) || defined(__MK64FX512__) || defined(__MK66FX1M0__)
#define  ARM_MATH_CM4
#elif defined(SIMULATOR)
#else
#error "Unknown processor type"
#endif

#if !defined(SIMULATOR)
#include <arm_math.h>
#endif

#endif // BASE_H
