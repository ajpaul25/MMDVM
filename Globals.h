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

#if !defined(GLOBALS_H)
#define  GLOBALS_H

#include "base.h"

#include "SerialPort.h"
//todo: not generic
#include "DMRIdleRX.h"
#include "DMRDMORX.h"
#include "DMRDMOTX.h"
#include "DStarRX.h"
#include "DStarTX.h"
#include "DStarMode.h"
#include "DMRRX.h"
#include "DMRTX.h"
#include "YSFRX.h"
#include "YSFTX.h"
#include "P25RX.h"
#include "P25TX.h"
#include "M17RX.h"
#include "M17TX.h"
#include "NXDNRX.h"
#include "NXDNTX.h"
#include "POCSAGTX.h"
#include "CalDStarRX.h"
#include "CalDStarTX.h"
#include "CalFM.h"
#include "CalDMR.h"
#include "CalP25.h"
#include "CalNXDN.h"
#include "CalPOCSAG.h"
#include "CalRSSI.h"
#include "CWIdTX.h"
#include "AX25RX.h"
#include "AX25TX.h"
#include "CalM17.h"
#include "Debug.h"
#include "IO.h"
#include "FM.h"
#include "InterfaceRX.h"
#include "InterfaceTX.h"
#include "InterfaceCal.h"
#include "AbstractMode.h"

const uint8_t  MARK_SLOT1 = 0x08U;
const uint8_t  MARK_SLOT2 = 0x04U;
const uint8_t  MARK_NONE  = 0x00U;

const uint16_t RX_BLOCK_SIZE = 2U;

const uint16_t TX_RINGBUFFER_SIZE = 500U;
const uint16_t RX_RINGBUFFER_SIZE = 600U;

#if defined(__MK20DX256__)
const uint16_t TX_BUFFER_LEN = 2000U;
#else
const uint16_t TX_BUFFER_LEN = 4000U;
#endif

extern MMDVM_STATE m_modemState;

//todo: not generic
extern bool m_dstarEnable;
extern bool m_dmrEnable;
extern bool m_ysfEnable;
extern bool m_p25Enable;
extern bool m_nxdnEnable;
extern bool m_pocsagEnable;
extern bool m_m17Enable;
extern bool m_fmEnable;
extern bool m_ax25Enable;

/*typedef struct {
  InterfaceRX* idlerx;
  InterfaceRX* rx;
  InterfaceTX* tx;
  InterfaceCal* calrx;
  InterfaceCal* caltx;
  InterfaceRX* orx;
  InterfaceTX* otx;
  bool (* condition)();
  bool (* ocondition)();
  bool (* calcondition)();
  uint8_t spacelen;
  uint8_t spacepos;
  uint8_t stateid;
  bool* enabled;
  q15_t *filtertaps;
  uint16_t filterlen;
  uint8_t filterStateSize;
  arm_fir_instance_q15 firFilter;
  q15_t txlevel;
}modeStruct;

extern modeStruct m_mode[24];*/

extern AbstractMode *m_mode[24];
extern uint8_t m_mode_length;

extern bool m_duplex;

extern bool m_tx;
extern bool m_dcd;

extern CSerialPort serial;
extern CIO io;

extern CCalRSSI calRSSI;

extern CCWIdTX cwIdTX;

#endif
