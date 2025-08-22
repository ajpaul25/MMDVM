#include "simulator.h"
using namespace std;

void arm_fir_fast_q15 	( 	const arm_fir_instance_q15 *  	S,
		const q15_t *  	pSrc,
		q15_t *  	pDst,
		uint32_t  	blockSize 
	)
{

}

void arm_fir_f32 	( 	const arm_fir_instance_f32 *  	S,
		const float32_t *  	pSrc,
		float32_t *  	pDst,
		uint32_t  	blockSize 
	)
{
    
}

void arm_fir_interpolate_q15 (const arm_fir_interpolate_instance_q15 *S, const q15_t *pSrc, q15_t *pDst, uint32_t blockSize)
{

}

void arm_q15_to_q31 (const q15_t *pSrc, q31_t *pDst, uint32_t blockSize)
{

}

void arm_biquad_cascade_df1_q31 (const arm_biquad_casd_df1_inst_q31 *S, const q31_t *pSrc, q31_t *pDst, uint32_t blockSize)
{

}

q31_t arm_sin_q31 (q31_t x)
{

}

void debug(const char* msg, const char* fname, const char* file, const int line )
{
  cout << "DEBUG " << fname << "(): "
    << msg
	<< " (" << file
	<< ", line " << line
	<< ")\n";
}
