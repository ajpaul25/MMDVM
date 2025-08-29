/*
 *   Copyright (C) 2015,2016,2017,2018,2020,2021 by Jonathan Naylor G4KLX
 *   Copyright (C) 2016 by Mathis Schmieder DB9MAT
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

#if defined(STM32F4XX) || defined(STM32F7XX) || defined(SIMULATOR)

#include "Config.h"
#include "Globals.h"
#include "Filters.h"

// Global variables
MMDVM_STATE m_modemState = STATE_IDLE;

//todo: not generic
bool m_dstarEnable  = true;
bool m_dmrEnable    = true;
bool m_ysfEnable    = true;
bool m_p25Enable    = true;
bool m_nxdnEnable   = true;
bool m_m17Enable    = true;
bool m_pocsagEnable = true;
bool m_fmEnable     = true;
bool m_ax25Enable   = true;

bool m_duplex = true;

bool m_tx  = false;
bool m_dcd = false;

AbstractMode* m_mode[24];
uint8_t m_mode_length;

CCalRSSI calRSSI;

CCWIdTX cwIdTX;

CSerialPort serial;
CIO io;

//todo: not generic
void setup()
{
  serial.start();
  int m=0;
#if defined(MODE_DSTAR)
  m_mode[m++] = new CDStarMode(&m_modemState);
#endif
#if defined(MODE_DMR)
  m_mode[m++] = new CDMRMode(&m_modemState);
#endif
#if defined(MODE_YSF)
  m_mode[m++] = new CYSFMode(&m_modemState);
#endif
#if defined(MODE_P25)
  m_mode[m++] = new CP25Mode(&m_modemState);
#endif
#if defined(MODE_NXDN)
  m_mode[m++] = new CNXDNMode(&m_modemState);
#endif
#if defined(MODE_M17)
  m_mode[m++] = new CM17Mode(&m_modemState);
#endif
#if defined(MODE_POCSAG)
  m_mode[m++] = new CPOCSAGMode(&m_modemState);
#endif
#if defined(MODE_FM)
  m_mode[m++] = new CFMMode(&m_modemState);
#if defined(MODE_AX25)
  m_mode[m++] = new CAX25Mode(&m_modemState);
#endif  //MODE_AX25
#endif  //MODE_FM
  
  m_mode_length = m;
}

void loop()
{
  serial.process();
  
  io.process();

  // The following is for transmitting
  for(int i=0; i<m_mode_length; i++){
    if (m_mode[i]->tx)
      if (m_mode[i]->condition())
        if(m_mode[i]->ocondition())
          m_mode[i]->otx->process();
        else
          m_mode[i]->tx->process();
  }
  for(int i=0; i<m_mode_length; i++){
    if(m_mode[i]->caltx)
      if (m_mode[i]->calcondition())
        m_mode[i]->caltx->process();
  }

  if (m_modemState == STATE_IDLE)
    cwIdTX.process();
}

int main()
{
  setup();

  for (;;)
    loop();
}

#endif
