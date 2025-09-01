/*
 *   Copyright (C) 2015,2016,2017,2018,2020 by Jonathan Naylor G4KLX
 *   Copyright (C) 2015 by Jim Mclaughlin KI6ZUM
 *   Copyright (C) 2016 by Colin Durbridge G4EML
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

#if defined(SIMULATOR)

#include "Config.h"
#include "Globals.h"
#include "IO.h"
#include <thread>
#include <string>
#include <cstdlib>
//#include <sys/socket.h>
//#include <sys/types.h> 
//#include <arpa/inet.h> 
#include <functional>
#include <netinet/in.h> 
#include "Simulator_Lib/simulator.h"

using namespace std;

#define PIN_COS                7
#define PIN_PTT                8
#define PIN_COSLED             11
#define PIN_DSTAR              16
#define PIN_DMR                17
#define PIN_YSF                18
#define PIN_P25                19
#define PIN_NXDN               20
#define PIN_POCSAG             4
#define PIN_FM                 5
#define ADC_CHER_Chan          (1<<7)                 // ADC on Due pin A0  - Due AD7 - (1 << 7)
#define ADC_ISR_EOC_Chan       ADC_ISR_EOC7
#define ADC_CDR_Chan           7
#define DACC_MR_USER_SEL_Chan  DACC_MR_USER_SEL_CHANNEL0 // DAC on Due DAC0
#define DACC_CHER_Chan         DACC_CHER_CH0

#define BUFSIZE 1024 

char cosInt[20], ledInt[20], pttInt[20], dStarInt[20], dmrInt[20], ysfInt[20], p25Int[20], nxdnInt[20], m17Int[20], pocsagInt[20], fmInt[20], dly[20];
status statArray[20];
uint8_t statArraySize;

CRingBuffer<TSample>  ioRXBuffer;

const uint16_t DC_OFFSET = 2048U;

extern "C" {
  void ADC_Handler()
  {
    io.interrupt();
  }
}

void rxIOCallback(char* buf, uint8_t length)
{
  //debug(buf);
  uint16_t s = buf[0] & 0xFF | (buf[1] & 0xFF) << 8;
  TSample rxsamp = {s,0};
  ioRXBuffer.put(rxsamp);
}

void CIO::initInt()
{
  initSimulator();
  initIOSimulator(&rxIOCallback);
  debug("loading simulator io");
  uint8_t i=0;
  statArray[i++]={"cos", cosInt};
  statArray[i++]={"led", ledInt};
  statArray[i++]={"ptt", pttInt};
  statArray[i++]={"dStar", dStarInt};
  statArray[i++]={"dmr", dmrInt};
  statArray[i++]={"ysf", ysfInt};
  statArray[i++]={"p25", p25Int};
  statArray[i++]={"nxdn", nxdnInt};
  statArray[i++]={"m17", m17Int};
  statArray[i++]={"pocsag", pocsagInt};
  statArray[i++]={"fm", fmInt};
  for(int j=0; j<i; j++)
    snprintf(statArray[j].status, 20, "off");
  statArray[i]={"dly", dly};
  snprintf(statArray[i++].status,20,"%u",dly);
  statArraySize=i;
  setIOStatus(statArray,statArraySize);
  thread* t1 = new thread(timerThread);
}

void CIO::startInt()
{
  debug("-");
}

void CIO::interrupt()
{
  //debug("-");
  TSample sample = {DC_OFFSET, MARK_NONE};

    m_txBuffer.get(sample);
    char d[2];
    d[0] = sample.sample & 0xFF;
    d[1] = (sample.sample >> 8) & 0xFF;
    sendIO(d, sizeof(sample.sample));

    TSample samp;
    ioRXBuffer.get(samp);
    m_rxBuffer.put(samp);

    //todo: create a way to input rssi data
#if defined(SEND_RSSI_DATA)
    //m_rssiBuffer.put(ADC->ADC_CDR[RSSI_CDR_Chan]);
#else
    m_rssiBuffer.put(0U);
#endif

    m_watchdog++;
    setIOStatus(statArray,statArraySize);

}

bool CIO::getCOSInt()
{
    debug("-");
    if(strcmp(cosInt,"on") == 0)
      return true;
    return false;
}

void CIO::setLEDInt(bool on)
{
  snprintf(ledInt, 20, on ? "on" : "off");
  debug(ledInt);
}

void CIO::setPTTInt(bool on)
{
  snprintf(pttInt, 20, on ? "on" : "off");
  debug(pttInt);
}

void CIO::setCOSInt(bool on)
{
  snprintf(cosInt, 20, on ? "on" : "off");
  debug(cosInt);
}

void CIO::setDStarInt(bool on)
{
  snprintf(dStarInt, 20, on ? "on" : "off");
  debug(dStarInt);
}

void CIO::setDMRInt(bool on)
{
  snprintf(dmrInt, 20, on ? "on" : "off");
  debug(dmrInt);
}

void CIO::setYSFInt(bool on)
{
  snprintf(ysfInt, 20, on ? "on" : "off");
  debug(ysfInt);
}

void CIO::setP25Int(bool on)
{
  snprintf(p25Int, 20, on ? "on" : "off");
  debug(p25Int);
}

void CIO::setNXDNInt(bool on)
{
  snprintf(nxdnInt, 20, on ? "on" : "off");
  debug(nxdnInt);
}

void CIO::setM17Int(bool on)
{
  snprintf(m17Int, 20, on ? "on" : "off");
  debug(m17Int);
}

void CIO::setPOCSAGInt(bool on)
{
  snprintf(pocsagInt, 20, on ? "on" : "off");
  debug(pocsagInt);
}

void CIO::setFMInt(bool on)
{
  snprintf(fmInt, 20, on ? "on" : "off");
  debug(fmInt);
}

void CIO::delayInt(unsigned int d)
{
  snprintf(dly,20,"%u",d);
  debug(dly);
}

uint8_t CIO::getCPU() const
{
  debug("");
  return 0U;
}

void CIO::getUDID(uint8_t* buffer)
{
  debug("");
}

#endif