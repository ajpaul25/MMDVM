/*
 *   Copyright (C) 2016,2017 by Jonathan Naylor G4KLX
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "Config.h"
#include "Globals.h"
#include "SerialPort.h"
#include <thread>

#if defined(SIMULATOR)

CRingBuffer<uint8_t>  serialRXBuffer(4096);

void rxSerialCallback(char* buf, uint8_t length)
{
  debug("");
  //char out[1024];
  for(int i=0; i<length; i++)
  {
    //if(buf[i] == '!')
      //break;
    //snprintf(out,110,"put 0x%02X",buf[i]);
    //debug(out);
    serialRXBuffer.put(buf[i]);
  }
}

void CSerialPort::beginInt(uint8_t n, int speed)
{
    char msg[50];
    snprintf(msg, 50, "parameters %u and %d", n, speed);
    initSerialSimulator(&rxSerialCallback);
    debug(msg);
}

int CSerialPort::availableForReadInt(uint8_t n)
{
  std::this_thread::sleep_for(std::chrono::milliseconds(20));
  int ret = serialRXBuffer.getData();
  //int ret = serialRXBuffer.getSpace() < 370U;
  char out[10];
  snprintf(out,10,"%u",ret);
  debug(out);
  return ret;
}

int CSerialPort::availableForWriteInt(uint8_t n)
{
  debug("-");
  return true;
}

uint8_t CSerialPort::readInt(uint8_t n)
{
  debug("-");
  uint8_t s;
  serialRXBuffer.get(s);
  return s;
}

void CSerialPort::writeInt(uint8_t n, const uint8_t* data, uint16_t length, bool flush)
{
  char d[length*3+1];
  snprintf(d, 128, "port: %d, data: %d, length: %d", n, *data, length);
  debug(d);
  char val[length];
  d[length*3]=0;
  for(int i=0; i<length; i++)
  {
    val[i] = data[i];
    char c[3];
    snprintf(c,3,"%02x",data[i]);
		d[i*3] = c[0];
		d[i*3+1] = c[1];
		d[i*3+2] = ' ';
  }
  debug(d);
  sendSerial(val,length);
}

#endif

