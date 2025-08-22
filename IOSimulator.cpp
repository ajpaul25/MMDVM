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

#include "Config.h"
#include "Globals.h"
#include "IO.h"

#if defined(SIMULATOR)

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

const uint16_t DC_OFFSET = 2048U;

extern "C" {
  void ADC_Handler()
  {
    io.interrupt();
  }
}

void CIO::initInt()
{
  debug("loading simulator io");
}

void CIO::startInt()
{
  debug("-");
}

void CIO::interrupt()
{
  debug("-");
}

bool CIO::getCOSInt()
{
    debug("-");
    return true;
}

void CIO::setLEDInt(bool on)
{
  debug( on ? "on" : "off" );
}

void CIO::setPTTInt(bool on)
{
  debug( on ? "on" : "off" );
}

void CIO::setCOSInt(bool on)
{
  debug( on ? "on" : "off" );
}

void CIO::setDStarInt(bool on)
{
  debug( on ? "on" : "off" );
}

void CIO::setDMRInt(bool on)
{
  debug( on ? "on" : "off" );
}

void CIO::setYSFInt(bool on)
{
  debug( on ? "on" : "off" );
}

void CIO::setP25Int(bool on)
{
  debug( on ? "on" : "off" );
}

void CIO::setNXDNInt(bool on)
{
  debug( on ? "on" : "off" );
}

void CIO::setM17Int(bool on)
{
  debug( on ? "on" : "off" );
}

void CIO::setPOCSAGInt(bool on)
{
  debug( on ? "on" : "off" );
}

void CIO::setFMInt(bool on)
{
  debug( on ? "on" : "off" );
}

void CIO::delayInt(unsigned int dly)
{
  char sdly[10];
  snprintf(sdly,10,"%u",dly);
  debug(sdly);
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

