/*
 *   Copyright (C) 2015,2016,2017,2018,2020,2021 by Jonathan Naylor G4KLX
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

#if !defined(IO_H)
#define  IO_H

#include "Globals.h"

#include "RingBuffer.h"

struct TSample {
  volatile uint16_t sample;
  volatile uint8_t control;
};

class CIO {
public:
  CIO();

  void start();

  void process();

  void write(MMDVM_STATE mode, q15_t* samples, uint16_t length, const uint8_t* control = NULL);

  uint16_t getSpace() const;

  void setDecode(bool dcd);
  void setADCDetection(bool detect);
  void setMode(MMDVM_STATE state);
  
  void interrupt();

  void getOverflow(bool& adcOverflow, bool& dacOverflow);

  bool hasTXOverflow();
  bool hasRXOverflow();

  bool hasLockout() const;

  void resetWatchdog();
  uint32_t getWatchdog();
  
  uint8_t getCPU() const;

  void getUDID(uint8_t* buffer);

  void selfTest();
  void setParameters(const uint8_t* data, uint16_t length);

private:
  bool                  m_started;

  CRingBuffer<TSample>  m_rxBuffer;
  CRingBuffer<TSample>  m_txBuffer;
  CRingBuffer<uint16_t> m_rssiBuffer;

#if defined(USE_DCBLOCKER)
  arm_biquad_casd_df1_inst_q31 m_dcFilter;
  q31_t                        m_dcState[4];
#endif

//todo: not generic
#if defined(MODE_NXDN)
#if !defined(USE_NXDN_BOXCAR)
  arm_fir_instance_q15 m_nxdnISincFilter;
  q15_t                m_nxdnISincState[60U];     // NoTaps + BlockSize - 1, 32 + 20 - 1 plus some spare
#endif
#endif

  bool                 m_pttInvert;
  q15_t                m_rxLevel;
  q15_t                m_cwIdTXLevel;

  uint16_t             m_rxDCOffset;
  uint16_t             m_txDCOffset;

  bool                 m_useCOSAsLockout;

  uint32_t             m_ledCount;
  bool                 m_ledValue;

  bool                 m_detect;

  uint16_t             m_adcOverflow;
  uint16_t             m_dacOverflow;

  volatile uint32_t    m_watchdog;

  bool                 m_lockout;

  // Hardware specific routines
  void initInt();
  void startInt();

  bool getCOSInt();

  void setLEDInt(bool on);
  void setPTTInt(bool on);
  void setCOSInt(bool on);

  //todo: not generic
  void setDStarInt(bool on);
  void setDMRInt(bool on);
  void setYSFInt(bool on);
  void setP25Int(bool on);
  void setNXDNInt(bool on);
  void setPOCSAGInt(bool on);
  void setM17Int(bool on);
  void setFMInt(bool on);
  
  void delayInt(unsigned int dly);
};

#endif
