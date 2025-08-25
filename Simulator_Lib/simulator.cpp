#include "simulator.h"
#include "Globals.h"
#include <thread>

using namespace std;

char screenbuf[20*50];
char debugScreenbuf[40*100];

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

void setIOStatus(status s[20], uint8_t size)
{
	const int cols=2;
	for( int i=0; i<size; i++ )
	{
		int j=0;
		for( ; j<strlen(s[i].name); j++ )
			screenbuf[i*50/cols+j] = s[i].name[j];
		if( j<48 )
		{
			screenbuf[i*50/cols+j++] = ':';
			screenbuf[i*50/cols+j++] = ' ';
		}
		for( int k=j; j<49 && j-k<strlen(s[i].status); j++)
			screenbuf[i*50/cols+j] = s[i].status[j-k];
		for( ; j<49; j++)
			screenbuf[i*50/cols+j] = ' ';

	}
}

void debug(const char* msg, const char* fname, const char* file, const int line )
{
	for(int i=0; i<39; i++)
		for(int j=0; j<100; j++)
			debugScreenbuf[i*100+j] = debugScreenbuf[(i+1)*100+j];
	char row[100];
	snprintf(row,100,"DEBUG %s (): %s (%s, line %d)",fname, msg, file, line);
	for(int i=strlen(row); i<100; i++)
		row[i] = ' ';
	for(int i=0; i<100; i++)
		debugScreenbuf[3900+i] = row[i];
}

void timerThread()
{
	for(;;)
	{
  		io.interrupt();
		printAt(screenbuf,1,120,20,50,1);
		printAt(debugScreenbuf,1,1,40,100,1);
  		this_thread::sleep_for(std::chrono::milliseconds(200));
	}
}

void printAt(char* str, uint8_t top, uint8_t left, uint8_t height, uint8_t width, uint8_t border)
{
	printf("\033[?25l");
	printf("\033[7");
	for(int row=0; row<height+border*2; row++)
	{
		for( int column=0; column<width+border*2; column++)
		{
			char c;
			if(row<border || row>height+border-1 || column<border || column>width+border-1 )
			  c=' ';
			else
			  c=str[width*(row-border)+column-border];
			printf("\033[%d;%dH%c", row+top, column+left, c);
		}
	}
	printf("\033[%d;%dH", 200, 0);
	printf("\033[?25h");\
	printf("\033[8");
	printf("\033#B");
}

void initSimulator()
{
	//clear the screen
	printf ("\033[2J");
}