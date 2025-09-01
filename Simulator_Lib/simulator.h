#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <cstdint>
#include <cstring>
#include <cstdio>
#include <iostream>
#include <sys/socket.h>
#include "base.h"
#include <functional>



typedef int16_t 	q15_t;
typedef int32_t 	q31_t;
typedef float 	    float32_t;
typedef int64_t     q63_t;

/**
  \brief   Signed Saturate
  \details Saturates a signed value.
  \param [in]  value  Value to be saturated
  \param [in]    sat  Bit position to saturate to (1..32)
  \return             Saturated value
 */
/*#define __SSAT(ARG1,ARG2) \
({                          \
  uint32_t __RES, __ARG1 = (ARG1); \
  __ASM ("ssat %0, %1, %2" : "=r" (__RES) :  "I" (ARG2), "r" (__ARG1) ); \
  __RES; \
 })*/

#define __SSAT(ARG1,ARG2) \
({                       \
  uint32_t min = -2^((ARG2)-1); \
  uint32_t max = 2^((ARG2)-1)-1; \
  uint32_t res = (ARG1); \
  ((ARG1) > max) ? max : ((ARG1) < min) ? min : (res); \
})

struct arm_fir_instance_q15{
    uint16_t numTaps;
    q15_t *pState;
    const q15_t *pCoeffs;
};

struct arm_fir_instance_f32{
    uint16_t numTaps;
    float32_t *pState;
    const float32_t *pCoeffs;
};

struct arm_fir_interpolate_instance_q15{
    uint8_t L;
    uint16_t phaseLength;
    const q15_t *pCoeffs;
    q15_t *pState;
};

struct arm_biquad_casd_df1_inst_q31{
    uint32_t numStages;
    q31_t *pState;
    const q31_t *pCoeffs;
    uint8_t postShift;
};

void arm_fir_fast_q15 	( 	const arm_fir_instance_q15 *  	S,
		const q15_t *  	pSrc,
		q15_t *  	pDst,
		uint32_t  	blockSize 
	);

void arm_fir_f32 	( 	const arm_fir_instance_f32 *  	S,
		const float32_t *  	pSrc,
		float32_t *  	pDst,
		uint32_t  	blockSize 
	);

q31_t 	arm_sin_q31 (q31_t x);

void 	arm_fir_interpolate_q15 (const arm_fir_interpolate_instance_q15 *S, const q15_t *pSrc, q15_t *pDst, uint32_t blockSize);

void 	arm_q15_to_q31 (const q15_t *pSrc, q31_t *pDst, uint32_t blockSize);

void 	arm_biquad_cascade_df1_q31 (const arm_biquad_casd_df1_inst_q31 *S, const q31_t *pSrc, q31_t *pDst, uint32_t blockSize);

q31_t 	arm_sin_q31 (q31_t x);



void debug(const char* msg, const char* fname = __builtin_FUNCTION(), const char* file = __builtin_FILE(), const int line = __builtin_LINE());

void timerThread();

void printAt(char* str, uint8_t top, uint8_t left, uint8_t height, uint8_t width, uint8_t border);

struct status
{
  const char* name;
  char* status;
};

void setIOStatus(status s[20], uint8_t size);

void initSimulator();
void initIOSimulator(void(*rxcb)(char*,uint8_t));
void initSerialSimulator(void(*rxcb)(char*,uint8_t));

void createUdp(int port, int *fd, struct sockaddr_in *remaddr, socklen_t *addrlen, void(*rxcb)(char*,uint8_t));

void sendSerial(char *data, uint8_t length);
void sendIO(char *data, uint8_t length);


#endif // SIMULATOR_H
