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

#if defined(STM32F4XX) || defined(STM32F7XX)

#include "Config.h"
#include "Globals.h"
#include "Filters.h"

// Global variables
MMDVM_STATE m_modemState = STATE_IDLE;

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

modeStruct m_mode[24];

#if defined(MODE_DSTAR)
CDStarRX dstarRX;
//CDStarTX dstarTX;

CCalDStarRX calDStarRX;
CCalDStarTX calDStarTX;
#endif

#if defined(MODE_DMR)
CDMRIdleRX dmrIdleRX;
CDMRRX dmrRX;
//CDMRTX dmrTX;

CDMRDMORX dmrDMORX;
//CDMRDMOTX dmrDMOTX;

CCalDMR calDMR;
#endif

#if defined(MODE_YSF)
CYSFRX ysfRX;
//CYSFTX ysfTX;
#endif

#if defined(MODE_P25)
CP25RX p25RX;
//CP25TX p25TX;

CCalP25 calP25;
#endif

#if defined(MODE_NXDN)
CNXDNRX nxdnRX;
//CNXDNTX nxdnTX;

CCalNXDN calNXDN;
#endif

#if defined(MODE_M17)
CM17RX m17RX;
//CM17TX m17TX;

CCalM17 calM17;
#endif

#if defined(MODE_POCSAG)
//CPOCSAGTX  pocsagTX;
CCalPOCSAG calPOCSAG;
#endif

#if defined(MODE_FM)
CFM    fm;
CCalFM calFM;
#endif

#if defined(MODE_AX25)
CAX25RX ax25RX;
//CAX25TX ax25TX;
#endif

CCalRSSI calRSSI;

CCWIdTX cwIdTX;

CSerialPort serial;
CIO io;

void setup()
{
  serial.start();
  int m=0;

  m_mode[m].idlerx = 0;
  m_mode[m].rx = new CDStarRX();
  m_mode[m].tx = new CDStarTX();
  m_mode[m].calrx = &calDStarRX;
  m_mode[m].caltx->tx = *m_mode[m].tx;//&calDStarTX;
  m_mode[m].orx = 0;
  m_mode[m].otx = 0;
  m_mode[m].condition = [](){ return m_dstarEnable && m_modemState == STATE_DSTAR; };
  m_mode[m].ocondition = [](){ return false; };
  m_mode[m].calcondition = [](){ return m_modemState == STATE_DSTARCAL; };
  m_mode[m].spacepos = 6U;
  m_mode[m].spacelen = 1U;
  m_mode[m].stateid = STATE_DSTAR;
  m_mode[m].enabled = &m_dstarEnable;
  m_mode[m].filtertaps = GAUSSIAN_0_5_FILTER;
  m_mode[m].filterlen = GAUSSIAN_0_5_FILTER_LEN;
  m_mode[m].filterStateSize = 40U;
  m++;

  m_mode[m].idlerx = &dmrIdleRX;
  m_mode[m].rx = &dmrRX;
  m_mode[m].tx = new CDMRTX();
  m_mode[m].calrx = 0;
  m_mode[m].caltx = &calDMR;
  m_mode[m].caltx->tx = *m_mode[m].tx;
  m_mode[m].orx = &dmrDMORX;
  m_mode[m].otx = new CDMRDMOTX();
  m_mode[m].caltx->otx = *m_mode[m].otx;
  m_mode[m].condition = [](){ return m_dmrEnable && m_modemState == STATE_DMR; };
  m_mode[m].ocondition = [](){ return m_duplex; };
  m_mode[m].calcondition = [](){ return m_modemState == STATE_DMRCAL || m_modemState == STATE_LFCAL || m_modemState == STATE_DMRCAL1K || m_modemState == STATE_DMRDMO1K; };
  m_mode[m].spacepos = 7U;
  m_mode[m].spacelen = 2U;
  m_mode[m].stateid = STATE_DMR;
  m_mode[m].enabled = &m_dmrEnable;
  m_mode[m].filtertaps = RRC_0_2_FILTER;
  m_mode[m].filterlen = RRC_0_2_FILTER_LEN;
  m_mode[m].filterStateSize = 70U;
  m++;

  m_mode[m].idlerx = 0;
  m_mode[m].rx = &ysfRX;
  m_mode[m].tx = new CYSFTX();
  m_mode[m].calrx = 0;
  m_mode[m].caltx = 0;
  m_mode[m].orx = 0;
  m_mode[m].otx = 0;
  m_mode[m].condition = [](){ return  m_ysfEnable && m_modemState == STATE_YSF; };
  m_mode[m].ocondition = [](){ return false; };
  m_mode[m].calcondition = [](){ return false; };
  m_mode[m].spacepos = 9U;
  m_mode[m].spacelen = 1U;
  m_mode[m].stateid = STATE_YSF;
  m_mode[m].enabled = &m_ysfEnable;
  m_mode[m].filtertaps = RRC_0_2_FILTER;
  m_mode[m].filterlen = RRC_0_2_FILTER_LEN;
  m_mode[m].filterStateSize = 70U;
  m++;

  m_mode[m].idlerx = 0;
  m_mode[m].rx = &p25RX;
  m_mode[m].tx = new CP25TX();
  m_mode[m].calrx = 0;
  m_mode[m].caltx = &calP25;
  m_mode[m].caltx->tx = *m_mode[m].tx;
  m_mode[m].orx = 0;
  m_mode[m].otx = 0;
  m_mode[m].condition = [](){ return m_p25Enable && m_modemState == STATE_P25; };
  m_mode[m].ocondition = [](){ return false; };
  m_mode[m].calcondition = [](){ return m_modemState == STATE_P25CAL1K; };
  m_mode[m].spacepos = 10U;
  m_mode[m].spacelen = 1U;
  m_mode[m].stateid = STATE_P25;
  m_mode[m].enabled = &m_p25Enable;
  m_mode[m].filtertaps = BOXCAR5_FILTER;
  m_mode[m].filterlen = BOXCAR5_FILTER_LEN;
  m_mode[m].filterStateSize = 30U;
  m++;

  m_mode[m].idlerx = 0;
  m_mode[m].rx = &nxdnRX;
  m_mode[m].tx = new CNXDNTX;
  m_mode[m].calrx = 0;
  m_mode[m].caltx = &calNXDN;
  m_mode[m].caltx->tx = *m_mode[m].tx;
  m_mode[m].orx = 0;
  m_mode[m].otx = 0;
  m_mode[m].condition = [](){ return m_nxdnEnable && m_modemState == STATE_NXDN; };
  m_mode[m].ocondition = [](){ return false; };
  m_mode[m].calcondition = [](){ return m_modemState == STATE_NXDNCAL1K; };
  m_mode[m].spacepos = 11U;
  m_mode[m].spacelen = 1U;
  m_mode[m].stateid = STATE_NXDN;
  m_mode[m].enabled = &m_nxdnEnable;
  #if defined(USE_NXDN_BOXCAR)
  m_mode[m].filtertaps = BOXCAR10_FILTER;
  m_mode[m].filterlen = BOXCAR10_FILTER_LEN;
  m_mode[m].filterStateSize = 40U;
  #else
  m_mode[m].filtertaps = NXDN_0_2_FILTER;
  m_mode[m].filterlen = NXDN_0_2_FILTER_LEN;
  m_mode[m].filterStateSize = 110U;
  #endif
  m++;

  m_mode[m].idlerx = 0;
  m_mode[m].rx = &m17RX;
  m_mode[m].tx = new CM17TX();
  m_mode[m].calrx = 0;
  m_mode[m].caltx = &calM17;
  m_mode[m].caltx->tx = *m_mode[m].tx;
  m_mode[m].orx = 0;
  m_mode[m].otx = 0;
  m_mode[m].condition = [](){ return m_m17Enable && m_modemState == STATE_M17; };
  m_mode[m].ocondition = [](){ return false; };
  m_mode[m].calcondition = [](){ return m_modemState == STATE_M17CAL; };
  m_mode[m].spacepos = 12U;
  m_mode[m].spacelen = 1U;
  m_mode[m].stateid = STATE_M17;
  m_mode[m].enabled = &m_m17Enable;
  m_mode[m].filtertaps = RRC_0_5_FILTER;
  m_mode[m].filterlen = RRC_0_5_FILTER_LEN;
  m_mode[m].filterStateSize = 70U;
  m++;

  m_mode[m].idlerx = 0;
  m_mode[m].rx = 0;
  CPOCSAGTX *ptx = new CPOCSAGTX();
  m_mode[m].tx = ptx;
  m_mode[m].calrx = 0;
  m_mode[m].caltx = &calPOCSAG;
  m_mode[m].caltx->tx = *m_mode[m].tx;
  m_mode[m].orx = 0;
  m_mode[m].otx = 0;
  //m_mode[m].condition = [ptx](){ return (m_pocsagEnable && (m_modemState == STATE_POCSAG || ptx->busy())); };
  //m_mode[m].condition = [&](){ return ptx->busy() == true; };
  m_mode[m].condition = [](){ return (m_pocsagEnable && (m_modemState == STATE_POCSAG)); };
  m_mode[m].ocondition = [](){ return false; };
  m_mode[m].calcondition = [](){ return m_modemState == STATE_POCSAGCAL; };
  m_mode[m].spacepos = 14U;
  m_mode[m].spacelen = 1U;
  m_mode[m].stateid = STATE_POCSAG;
  m_mode[m].enabled = &m_pocsagEnable;
  m++;

  m_mode[m].idlerx = 0;
  m_mode[m].rx = 0;
  m_mode[m].tx = &fm;
  m_mode[m].calrx = 0;
  m_mode[m].caltx = &calFM;
  m_mode[m].caltx->tx = *m_mode[m].tx;
  m_mode[m].orx = 0;
  m_mode[m].otx = 0;
  m_mode[m].condition = [](){ return m_fmEnable && m_modemState == STATE_FM; };
  m_mode[m].ocondition = [](){ return false; };
  m_mode[m].calcondition = [](){ return m_modemState == STATE_FMCAL10K || m_modemState == STATE_FMCAL12K || m_modemState == STATE_FMCAL15K || m_modemState == STATE_FMCAL20K || m_modemState == STATE_FMCAL25K || m_modemState == STATE_FMCAL30K; };
  m_mode[m].spacepos = 13U;
  m_mode[m].spacelen = 1U;
  m_mode[m].stateid = STATE_FM;
  m_mode[m].enabled = &m_fmEnable;
  m++;
  
  m_mode[m].idlerx = 0;
  m_mode[m].rx = &ax25RX;
  m_mode[m].tx = new CAX25TX();
  m_mode[m].calrx = 0;
  m_mode[m].caltx = 0;
  m_mode[m].orx = 0;
  m_mode[m].otx = 0;
  m_mode[m].condition = [](){ return m_ax25Enable && m_modemState == STATE_AX25; };
  m_mode[m].ocondition = [](){ return false; };
  m_mode[m].calcondition = [](){ return false; };
  m_mode[m].spacepos = 15U;
  m_mode[m].spacelen = 1U;
  m_mode[m].stateid = STATE_AX25;
  m_mode[m].enabled = &m_ax25Enable;
  m++;
}

void loop()
{
  serial.process();
  
  io.process();

  // The following is for transmitting
  for(int i=0; i<24; i++){
    if (m_mode[i].tx)
      if (m_mode[i].condition())
        if(m_mode[i].ocondition())
          m_mode[i].otx->process();
        else
          m_mode[i].tx->process();
  }
  for(int i=0; i<24; i++){
    if(m_mode[i].caltx)
      if (m_mode[i].calcondition())
        m_mode[i].caltx->process();
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
