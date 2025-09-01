#include "simulator.h"
#include "Globals.h"
#include <thread>

#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 

#include <cstring>
#include <iostream>
  
#define BUFSIZE 1024 

using namespace std;

char screenbuf[20*50];
char debugScreenbuf[40*100];

struct sockaddr_in ioremaddr;
socklen_t ioaddrlen;
int iofd;

struct sockaddr_in serialremaddr;
socklen_t serialaddrlen;
int serialfd;

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
  		this_thread::sleep_for(std::chrono::milliseconds(20));
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

void initIOSimulator(void(*rxcb)(char*,uint8_t))
{
#ifdef IO_UDP_PORT
	thread* t1 = new thread(createUdp,IO_UDP_PORT,&iofd,&ioremaddr,&ioaddrlen,rxcb);
#endif
}

void initSerialSimulator(void(*rxcb)(char*,uint8_t))
{
#ifdef SERIAL_UDP_PORT
	thread* t2 = new thread(createUdp,SERIAL_UDP_PORT,&serialfd,&serialremaddr,&serialaddrlen,rxcb);
#endif
}

void createUdp(int port, int *fd, struct sockaddr_in *remaddr, socklen_t *addrlen, void(*rxcb)(char*,uint8_t))
{
	struct sockaddr_in myaddr;
	*addrlen = sizeof(*remaddr);
	uint8_t recvlen;
	char buf[BUFSIZE];


	if ((*fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
			perror("error creating socket\n");
			return;
	}
	int option=1;
	setsockopt(*fd, SOL_SOCKET, SO_REUSEPORT, &option, sizeof(option));

	memset((char *)&myaddr, 0, sizeof(myaddr));
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = INADDR_ANY;
	myaddr.sin_port = htons(port);

	bind(*fd, (struct sockaddr*)&myaddr, sizeof(myaddr));

	while(true) {
			recvlen = recvfrom(*fd, buf, BUFSIZE, 0, (struct sockaddr *)remaddr, addrlen);
			if (recvlen > 0) {
					//buf[recvlen++] = '!';

					//char out[10];
					//for(int i=0; i<recvlen; i++)
					//{
					//	snprintf(out,10,"0x%02x",buf[i]);
					//	debug(out);
					//}
					//debug(buf);
					rxcb(buf, recvlen);
					//serialRXBuffer.put(buf);
					//m_rxBuffer.put(s);
					//char message[100];
					//snprintf(message,100,"fd %d\n", *fd);
					//sendto(*fd, message, strlen(message), 0, (struct sockaddr*)remaddr, *addrlen);
					//sendto(*fd, buf, strlen(buf), 0, (struct sockaddr*)remaddr, *addrlen);
			}
	}
}

void sendSerial(char *data, uint8_t length)
{
	char d[length*3+1];
	d[length*3] = 0;
	for(int i=0; i<length; i++)
	{
		char c[3];
		snprintf(c,3,"%02x",data[i]);
		d[i*3] = c[0];
		d[i*3+1] = c[1];
		d[i*3+2] = ' ';
	}
	debug(d);
	int ret = sendto(serialfd, data, length, 0, (struct sockaddr*)&serialremaddr, serialaddrlen);
	if(ret==-1)
	  debug("problem sending");
}

void sendIO(char *data, uint8_t length)
{
	sendto(iofd, data, length, 0, (struct sockaddr*)&ioremaddr, ioaddrlen);
}