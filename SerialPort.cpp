/*
 *   Copyright (C) 2013,2015-2021 by Jonathan Naylor G4KLX
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

#if defined(MADEBYMAKEFILE)
#include "GitVersion.h"
#endif

#include "SerialPort.h"
#include "Version.h"

const uint8_t MMDVM_FRAME_START  = 0xE0U;

const uint8_t MMDVM_GET_VERSION  = 0x00U;
const uint8_t MMDVM_GET_STATUS   = 0x01U;
const uint8_t MMDVM_SET_CONFIG   = 0x02U;
const uint8_t MMDVM_SET_MODE     = 0x03U;
const uint8_t MMDVM_SET_FREQ     = 0x04U;

const uint8_t MMDVM_CAL_DATA     = 0x08U;
const uint8_t MMDVM_RSSI_DATA    = 0x09U;

const uint8_t MMDVM_SEND_CWID    = 0x0AU;

const uint8_t MMDVM_ACK          = 0x70U;
const uint8_t MMDVM_NAK          = 0x7FU;

const uint8_t MMDVM_SERIAL_DATA  = 0x80U;
const uint8_t MMDVM_I2C_DATA     = 0x81U;

const uint8_t MMDVM_TRANSPARENT  = 0x90U;
const uint8_t MMDVM_QSO_INFO     = 0x91U;

const uint8_t MMDVM_DEBUG1       = 0xF1U;
const uint8_t MMDVM_DEBUG2       = 0xF2U;
const uint8_t MMDVM_DEBUG3       = 0xF3U;
const uint8_t MMDVM_DEBUG4       = 0xF4U;
const uint8_t MMDVM_DEBUG5       = 0xF5U;
const uint8_t MMDVM_DEBUG_DUMP   = 0xFAU;

#if EXTERNAL_OSC == 12000000
#define TCXO "12.0000 MHz"
#elif EXTERNAL_OSC == 12288000
#define TCXO "12.2880 MHz"
#elif EXTERNAL_OSC == 14400000
#define TCXO "14.4000 MHz"
#elif EXTERNAL_OSC == 19200000
#define TCXO "19.2000 Mhz"
#else
#define TCXO "NO TCXO"
#endif

#if defined(DRCC_DVM_NQF)
#define	HW_TYPE	"MMDVM DRCC_DVM_NQF"
#elif defined(DRCC_DVM_HHP446)
#define	HW_TYPE	"MMDVM DRCC_DVM_HHP(446)"
#elif defined(DRCC_DVM_722)
#define HW_TYPE "MMDVM RB_STM32_DVM(722)"
#elif defined(DRCC_DVM_446)
#define HW_TYPE "MMDVM RB_STM32_DVM(446)"
#else
#define	HW_TYPE	"MMDVM"
#endif

#if defined(GITVERSION)
#define concat(h, a, b, c) h " " a " " b " GitID #" c ""
const char HARDWARE[] = concat(HW_TYPE, VERSION, TCXO, GITVERSION);
#else
#define concat(h, a, b, c, d) h " " a " " b " (Build: " c " " d ")"
const char HARDWARE[] = concat(HW_TYPE, VERSION, TCXO, __TIME__, __DATE__);
#endif

const uint8_t PROTOCOL_VERSION   = 2U;

// Parameters for batching serial data
const int      MAX_SERIAL_DATA  = 250;
const uint16_t MAX_SERIAL_COUNT = 100U;

CSerialPort::CSerialPort() :
m_buffer(),
m_ptr(0U),
m_len(0U),
m_debug(false),
m_serialData(),
m_lastSerialAvail(0),
m_lastSerialAvailCount(0U),
m_i2CData()
{
}

void CSerialPort::sendACK(uint8_t type)
{
  uint8_t reply[4U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 4U;
  reply[2U] = MMDVM_ACK;
  reply[3U] = type;

  writeInt(1U, reply, 4);
}

void CSerialPort::sendNAK(uint8_t type, uint8_t err)
{
  uint8_t reply[5U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 5U;
  reply[2U] = MMDVM_NAK;
  reply[3U] = type;
  reply[4U] = err;

  writeInt(1U, reply, 5);
}

void CSerialPort::getStatus()
{
  io.resetWatchdog();

  uint8_t reply[30U] = {0};

  // Send all sorts of interesting internal values
  reply[0U]  = MMDVM_FRAME_START;
  reply[1U]  = 20U;
  reply[2U]  = MMDVM_GET_STATUS;

  reply[3U]  = uint8_t(m_modemState);

  reply[4U]  = m_tx  ? 0x01U : 0x00U;

  bool adcOverflow;
  bool dacOverflow;
  io.getOverflow(adcOverflow, dacOverflow);

  if (adcOverflow)
    reply[4U] |= 0x02U;

  if (io.hasRXOverflow())
    reply[4U] |= 0x04U;

  if (io.hasTXOverflow())
    reply[4U] |= 0x08U;

  if (io.hasLockout())
    reply[4U] |= 0x10U;

  if (dacOverflow)
    reply[4U] |= 0x20U;
    
  reply[4U] |= m_dcd ? 0x40U : 0x00U;

  reply[5U] = 0x00U;

  for(int i=0; i<24; i++)
  {
    if (m_mode[i]->spacelen > 0)
    {
      InterfaceTX tx = m_mode[i]->ocondition() ? *m_mode[i]->otx : *m_mode[i]->tx;
      if (m_mode[i]->spacelen > 1 )
        for(uint8_t j = 0; j < m_mode[i]->spacelen; j++)
          reply[m_mode[i]->spacepos+j] = tx.getSpace(j);
      else
        reply[m_mode[i]->spacepos] = tx.getSpace();
    }
  }

  writeInt(1U, reply, 20);
}

void CSerialPort::getVersion()
{
  uint8_t reply[200U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 0U;
  reply[2U] = MMDVM_GET_VERSION;

  reply[3U] = PROTOCOL_VERSION;

  //todo: not generic
  // Return two bytes of mode capabilities
  reply[4U] = 0x00U;
#if defined(MODE_DSTAR)
  reply[4U] |= 0x01U;
#endif
#if defined(MODE_DMR)
  reply[4U] |= 0x02U;
#endif
#if defined(MODE_YSF)
  reply[4U] |= 0x04U;
#endif
#if defined(MODE_P25)
  reply[4U] |= 0x08U;
#endif
#if defined(MODE_NXDN)
  reply[4U] |= 0x10U;
#endif
#if defined(MODE_M17)
  reply[4U] |= 0x20U;
#endif
#if defined(MODE_FM)
  reply[4U] |= 0x40U;
#endif

  reply[5U] = 0x00U;
#if defined(MODE_POCSAG)
  reply[5U] |= 0x01U;
#endif
#if defined(MODE_AX25)
  reply[5U] |= 0x02U;
#endif

  // CPU type/manufacturer. 0=Atmel ARM, 1=NXP ARM, 2=St-Micro ARM
  reply[6U] = io.getCPU();

  // Reserve 16 bytes for the UDID
  ::memset(reply + 7U, 0x00U, 16U);
  io.getUDID(reply + 7U);

  uint8_t count = 23U;
  for (uint8_t i = 0U; HARDWARE[i] != 0x00U; i++, count++)
    reply[count] = HARDWARE[i];

  reply[1U] = count;

  writeInt(1U, reply, count);
}

uint8_t CSerialPort::setConfig(const uint8_t* data, uint16_t length)
{
  if (length < 37U)
    return 4U;

  m_debug = (data[0U] & 0x10U) == 0x10U;

  uint8_t txDelay = data[3U];
  if (txDelay > 50U)
    return 4U;

  MMDVM_STATE modemState = MMDVM_STATE(data[4U]);

  bool matched = false;
  for( int i=0; i<m_mode_length; i++)
  {
    m_mode[i]->setConfig(data, length);
    if( m_mode[i]->hasState(modemState) )
    {
      matched = true;
      if( !m_mode[i]->enabled )
        return 4U;
    }
  }
  switch(modemState){
    case STATE_IDLE:
    case STATE_RSSICAL:
      matched = true;
  }
  if(!matched)
    return 4U;


  setMode(modemState);

  bool simplex         = (data[0U] & 0x80U) == 0x80U;
  m_duplex       = !simplex;

  for(int i=0; i<m_mode_length; i++)
  {
    if (m_mode[i]->tx)
      m_mode[i]->tx->setConfig(data,length);
    if (m_mode[i]->rx)
      m_mode[i]->rx->setConfig(data,length);
    if (m_mode[i]->otx)
      m_mode[i]->otx->setConfig(data,length);
    if (m_mode[i]->orx)
      m_mode[i]->orx->setConfig(data,length);
    if (m_mode[i]->idlerx)
      m_mode[i]->idlerx->setConfig(data,length);
  }

  io.setParameters(data, length);

  io.start();

  return 0U;
}

uint8_t CSerialPort::setMode(const uint8_t* data, uint16_t length)
{
  if (length < 1U)
    return 4U;

  MMDVM_STATE modemState = MMDVM_STATE(data[0U]);

  if (modemState == m_modemState)
    return 0U;
  
  bool matched = false;
  for( int i=0; i<m_mode_length; i++)
  {
    m_mode[i]->setConfig(data, length);
    if( m_mode[i]->hasState(modemState) )
    {
      matched = true;
      if( !m_mode[i]->enabled )
        return 4U;
    }
  }
  switch(modemState){
    case STATE_IDLE:
    case STATE_RSSICAL:
      matched = true;
  }
  if(!matched)
    return 4U;

  setMode(modemState);

  return 0U;
}

//todo: not generic
void CSerialPort::setMode(MMDVM_STATE modemState)
{
  switch (modemState) {
    case STATE_DSTAR:
      DEBUG1("Mode set to D-Star");
      break;
    case STATE_DMR:
      DEBUG1("Mode set to DMR");
      break;
    case STATE_YSF:
      DEBUG1("Mode set to System Fusion");
      break;
    case STATE_P25:
      DEBUG1("Mode set to P25");
      break;
    case STATE_NXDN:
      DEBUG1("Mode set to NXDN");
      break;
    case STATE_M17:
      DEBUG1("Mode set to M17");
      break;
    case STATE_POCSAG:
      DEBUG1("Mode set to POCSAG");
      break;
    case STATE_FM:
      DEBUG1("Mode set to FM");
      break;
    case STATE_DSTARCAL:
      DEBUG1("Mode set to D-Star Calibrate");
      break;
    case STATE_DMRCAL:
      DEBUG1("Mode set to DMR Calibrate");
      break;
    case STATE_RSSICAL:
      DEBUG1("Mode set to RSSI Calibrate");
      break;
    case STATE_LFCAL:
      DEBUG1("Mode set to 80 Hz Calibrate");
      break;
    case STATE_FMCAL10K:
      DEBUG1("Mode set to FM 10Khz Calibrate");
      break;
    case STATE_FMCAL12K:
      DEBUG1("Mode set to FM 12.5Khz Calibrate");
      break;
    case STATE_FMCAL15K:
      DEBUG1("Mode set to FM 15Khz Calibrate");
      break;
    case STATE_FMCAL20K:
      DEBUG1("Mode set to FM 20Khz Calibrate");
      break;
    case STATE_FMCAL25K:
      DEBUG1("Mode set to FM 10Khz Calibrate");
      break;
    case STATE_FMCAL30K:
      DEBUG1("Mode set to FM 30Khz Calibrate");
      break;
    case STATE_P25CAL1K:
      DEBUG1("Mode set to P25 1011 Hz Calibrate");
      break;
    case STATE_DMRDMO1K:
      DEBUG1("Mode set to DMR MS 1031 Hz Calibrate");
      break;
    case STATE_NXDNCAL1K:
      DEBUG1("Mode set to NXDN 1031 Hz Calibrate");
      break;
    case STATE_POCSAGCAL:
      DEBUG1("Mode set to POCSAG Calibrate");
      break;
    case STATE_M17CAL:
      DEBUG1("Mode set to M17 Calibrate");
      break;
    default:        // STATE_IDLE
      DEBUG1("Mode set to Idle");
      break;
  }

  for(int i=0; i<24; i++)
  {
    if(modemState != m_mode[i]->stateid)
    {
      if (m_mode[i]->rx)
        m_mode[i]->rx->reset();
      if (m_mode[i]->idlerx)
        m_mode[i]->idlerx->reset();
      if (m_mode[i]->orx)
        m_mode[i]->orx->reset();
#if defined(MODE_FM)
      if (m_mode[i]->stateid == STATE_FM)
      {
        CFM fm = *(static_cast<CFM*>(m_mode[i]->tx));
        fm.reset();
      }
#endif
    }
  }

  cwIdTX.reset();

  io.setMode(modemState);
}

void CSerialPort::start()
{
  beginInt(1U, SERIAL_SPEED);

#if defined(SERIAL_REPEATER)
  #if defined(SERIAL_REPEATER_BAUD_RATE)
    beginInt(3U, SERIAL_REPEATER_BAUD_RATE);
  #else
    beginInt(3U, 9600);
  #endif
#endif
#if defined(I2C_REPEATER)
  beginInt(10U, 9600);
#endif
}

void CSerialPort::process()
{
  while (availableForReadInt(1U)) {
    uint8_t c = readInt(1U);

    if (m_ptr == 0U) {
      if (c == MMDVM_FRAME_START) {
        // Handle the frame start correctly
        m_buffer[0U] = c;
        m_ptr = 1U;
        m_len = 0U;
      } else {
        m_ptr = 0U;
        m_len = 0U;
      }
    } else if (m_ptr == 1U) {
      // Handle the frame length, 1/2
      m_len = m_buffer[m_ptr] = c;
      m_ptr = 2U;
    } else if (m_ptr == 2U) {
      // Handle the frame length, 2/2
      m_buffer[m_ptr] = c;
      m_ptr = 3U;

      if (m_len == 0U)
        m_len = c + 255U;

      // The full packet has been received, process it
      if (m_ptr == m_len)
        processMessage(m_buffer[2U], m_buffer + 3U, m_len - 3U);
    } else {
      // Any other bytes are added to the buffer
      m_buffer[m_ptr] = c;
      m_ptr++;

      // The full packet has been received, process it
      if (m_ptr == m_len) {
        if (m_len > 255U)
          processMessage(m_buffer[3U], m_buffer + 4U, m_len - 4U);
        else
          processMessage(m_buffer[2U], m_buffer + 3U, m_len - 3U);
      }
    }
  }

  if (io.getWatchdog() >= 48000U) {
    m_ptr = 0U;
    m_len = 0U;
  }

#if defined(SERIAL_REPEATER)
  // Write any outgoing serial data
  uint16_t serialSpace = m_serialData.getData();
  if (serialSpace > 0U) {
    int avail = availableForWriteInt(3U);
    if (avail < serialSpace)
      serialSpace = avail;

    for (uint16_t i = 0U; i < serialSpace; i++) {
      uint8_t c = 0U;
      m_serialData.get(c);
      writeInt(3U, &c, 1U);
    }
  }

  // Read any incoming serial data, and send out in batches
  int serialAvail = availableForReadInt(3U);
  if ((serialAvail > 0 && serialAvail == m_lastSerialAvail && m_lastSerialAvailCount >= MAX_SERIAL_COUNT) || (serialAvail >= MAX_SERIAL_DATA)) {
    uint8_t buffer[MAX_SERIAL_DATA];
    for (int i = 0; i < serialAvail && i < MAX_SERIAL_DATA; i++) {
      buffer[i] = readInt(3U);
      m_lastSerialAvail--;
    }
    writeSerialData(buffer, serialAvail - m_lastSerialAvail);
    m_lastSerialAvailCount = 0U;
  } else if (serialAvail > 0 && serialAvail == m_lastSerialAvail) {
    m_lastSerialAvailCount++;
  } else {
    m_lastSerialAvail      = serialAvail;
    m_lastSerialAvailCount = 0U;
  }
#endif

#if defined(I2C_REPEATER)
  // Write any outgoing serial data
  uint16_t i2CSpace = m_i2CData.getData();
  if (i2CSpace > 0U) {
    int avail = availableForWriteInt(10U);
    if (avail < i2CSpace)
      i2CSpace = avail;

    for (uint16_t i = 0U; i < i2CSpace; i++) {
      uint8_t c = 0U;
      m_i2CData.get(c);
      writeInt(10U, &c, 1U);
    }
  }
#endif
}

void CSerialPort::processMessage(uint8_t type, const uint8_t* buffer, uint16_t length)
{
  uint8_t err = 2U;

  switch (type) {
    case MMDVM_GET_STATUS:
      getStatus();
      break;

    case MMDVM_GET_VERSION:
      getVersion();
      break;

    case MMDVM_SET_CONFIG:
      err = setConfig(buffer, length);
      if (err == 0U)
        sendACK(type);
      else
        sendNAK(type, err);
      break;

    case MMDVM_SET_MODE:
      err = setMode(buffer, length);
      if (err == 0U)
        sendACK(type);
      else
        sendNAK(type, err);
      break;

    case MMDVM_SET_FREQ:
      sendACK(type);
      break;

//todo: not generic
/*#if !defined(MODE_FM)
    case MMDVM_FM_PARAMS1:
    case MMDVM_FM_PARAMS2:
    case MMDVM_FM_PARAMS3:
    case MMDVM_FM_PARAMS4:
      sendACK(type);
      break;
#endif*/

    case MMDVM_CAL_DATA:
      for (int i=0; i<24; i++) //step through all of our mode structs
        if( m_mode[i]->caltx )
          err = m_mode[i]->caltx->processMessage( type, buffer, length );

      if (err == 0U) {
        sendACK(type);
      } else {
        DEBUG2("Received invalid calibration data", err);
        sendNAK(type, err);
      }
      break;

    case MMDVM_SEND_CWID:
      err = 5U;
      if (m_modemState == STATE_IDLE)
        err = cwIdTX.write(buffer, length);
      if (err != 0U) {
        DEBUG2("Invalid CW Id data", err);
        sendNAK(type, err);
      }
      break;

    case MMDVM_TRANSPARENT:
    case MMDVM_QSO_INFO:
      // Do nothing on the MMDVM.
      break;

#if defined(SERIAL_REPEATER)
    case MMDVM_SERIAL_DATA: {
      for (uint16_t i = 0U; i < length; i++)
        m_serialData.put(buffer[i]);
      }
      break;
#endif

#if defined(I2C_REPEATER)
    case MMDVM_I2C_DATA: {
      for (uint16_t i = 0U; i < length; i++)
        m_i2CData.put(buffer[i]);
      }
      break;
#endif

    default:
      bool success = false;
      for (int i=0; i<24; i++) //step through all of our mode structs
        if(m_mode[i]->tx)
          if (m_modemState == STATE_IDLE || m_mode[i]->condition())
          {
            if (m_mode[i]->ocondition())
              err = m_mode[i]->otx->processMessage( type, buffer, length);
            else
              err = m_mode[i]->tx->processMessage( type, buffer, length);
            if (err == 0 or err == 255) // 255 is also success but no subsequent state change
            {
              success = true;
              if (m_modemState == STATE_IDLE and err == 0)
                setMode(MMDVM_STATE(m_mode[i]->stateid));
            }
          }
      if( !success ) // Handle this, send a NAK back
          sendNAK(type, 1U);
      break;
  }

  m_ptr = 0U;
  m_len = 0U;
}

//todo: not generic
#if defined(MODE_DSTAR)
void CSerialPort::writeDStarHeader(const uint8_t* header, uint8_t length)
{
  if (m_modemState != STATE_DSTAR && m_modemState != STATE_IDLE)
    return;

  if (!m_dstarEnable)
    return;

  uint8_t reply[50U];
  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 0U;
  reply[2U] = MMDVM_DSTAR_HEADER;

  uint8_t count = 3U;
  for (uint8_t i = 0U; i < length; i++, count++)
    reply[count] = header[i];

  reply[1U] = count;

  writeInt(1U, reply, count);
}

//todo: not generic
void CSerialPort::writeDStarData(const uint8_t* data, uint8_t length)
{
  if (m_modemState != STATE_DSTAR && m_modemState != STATE_IDLE)
    return;

  if (!m_dstarEnable)
    return;

  uint8_t reply[20U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 0U;
  reply[2U] = MMDVM_DSTAR_DATA;

  uint8_t count = 3U;
  for (uint8_t i = 0U; i < length; i++, count++)
    reply[count] = data[i];

  reply[1U] = count;

  writeInt(1U, reply, count);
}

//todo: not generic
void CSerialPort::writeDStarLost()
{
  if (m_modemState != STATE_DSTAR && m_modemState != STATE_IDLE)
    return;

  if (!m_dstarEnable)
    return;

  uint8_t reply[3U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 3U;
  reply[2U] = MMDVM_DSTAR_LOST;

  writeInt(1U, reply, 3);
}

//todo: not generic
void CSerialPort::writeDStarEOT()
{
  if (m_modemState != STATE_DSTAR && m_modemState != STATE_IDLE)
    return;

  if (!m_dstarEnable)
    return;

  uint8_t reply[3U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 3U;
  reply[2U] = MMDVM_DSTAR_EOT;

  writeInt(1U, reply, 3);
}
#endif

//todo: not generic
#if defined(MODE_DMR)
void CSerialPort::writeDMRData(bool slot, const uint8_t* data, uint8_t length)
{
  if (m_modemState != STATE_DMR && m_modemState != STATE_IDLE)
    return;

  if (!m_dmrEnable)
    return;

  uint8_t reply[40U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 0U;
  reply[2U] = slot ? MMDVM_DMR_DATA2 : MMDVM_DMR_DATA1;

  uint8_t count = 3U;
  for (uint8_t i = 0U; i < length; i++, count++)
    reply[count] = data[i];

  reply[1U] = count;

  writeInt(1U, reply, count);
}

void CSerialPort::writeDMRLost(bool slot)
{
  if (m_modemState != STATE_DMR && m_modemState != STATE_IDLE)
    return;

  if (!m_dmrEnable)
    return;

  uint8_t reply[3U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 3U;
  reply[2U] = slot ? MMDVM_DMR_LOST2 : MMDVM_DMR_LOST1;

  writeInt(1U, reply, 3);
}
#endif

//todo: not generic
#if defined(MODE_YSF)
void CSerialPort::writeYSFData(const uint8_t* data, uint8_t length)
{
  if (m_modemState != STATE_YSF && m_modemState != STATE_IDLE)
    return;

  if (!m_ysfEnable)
    return;

  uint8_t reply[130U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 0U;
  reply[2U] = MMDVM_YSF_DATA;

  uint8_t count = 3U;
  for (uint8_t i = 0U; i < length; i++, count++)
    reply[count] = data[i];

  reply[1U] = count;

  writeInt(1U, reply, count);
}

void CSerialPort::writeYSFLost()
{
  if (m_modemState != STATE_YSF && m_modemState != STATE_IDLE)
    return;

  if (!m_ysfEnable)
    return;

  uint8_t reply[3U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 3U;
  reply[2U] = MMDVM_YSF_LOST;

  writeInt(1U, reply, 3);
}
#endif

//todo: not generic
#if defined(MODE_P25)
void CSerialPort::writeP25Hdr(const uint8_t* data, uint8_t length)
{
  if (m_modemState != STATE_P25 && m_modemState != STATE_IDLE)
    return;

  if (!m_p25Enable)
    return;

  uint8_t reply[120U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 0U;
  reply[2U] = MMDVM_P25_HDR;

  uint8_t count = 3U;
  for (uint8_t i = 0U; i < length; i++, count++)
    reply[count] = data[i];

  reply[1U] = count;

  writeInt(1U, reply, count);
}

//todo: not generic
void CSerialPort::writeP25Ldu(const uint8_t* data, uint8_t length)
{
  if (m_modemState != STATE_P25 && m_modemState != STATE_IDLE)
    return;

  if (!m_p25Enable)
    return;

  uint8_t reply[250U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 0U;
  reply[2U] = MMDVM_P25_LDU;

  uint8_t count = 3U;
  for (uint8_t i = 0U; i < length; i++, count++)
    reply[count] = data[i];

  reply[1U] = count;

  writeInt(1U, reply, count);
}

//todo: not generic
void CSerialPort::writeP25Lost()
{
  if (m_modemState != STATE_P25 && m_modemState != STATE_IDLE)
    return;

  if (!m_p25Enable)
    return;

  uint8_t reply[3U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 3U;
  reply[2U] = MMDVM_P25_LOST;

  writeInt(1U, reply, 3);
}
#endif

//todo: not generic
#if defined(MODE_NXDN)
void CSerialPort::writeNXDNData(const uint8_t* data, uint8_t length)
{
  if (m_modemState != STATE_NXDN && m_modemState != STATE_IDLE)
    return;

  if (!m_nxdnEnable)
    return;

  uint8_t reply[130U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 0U;
  reply[2U] = MMDVM_NXDN_DATA;

  uint8_t count = 3U;
  for (uint8_t i = 0U; i < length; i++, count++)
    reply[count] = data[i];

  reply[1U] = count;

  writeInt(1U, reply, count);
}

//todo: not generic
void CSerialPort::writeNXDNLost()
{
  if (m_modemState != STATE_NXDN && m_modemState != STATE_IDLE)
    return;

  if (!m_nxdnEnable)
    return;

  uint8_t reply[3U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 3U;
  reply[2U] = MMDVM_NXDN_LOST;

  writeInt(1U, reply, 3);
}
#endif

//todo: not generic
#if defined(MODE_M17)
void CSerialPort::writeM17LinkSetup(const uint8_t* data, uint8_t length)
{
  if (m_modemState != STATE_M17 && m_modemState != STATE_IDLE)
    return;

  if (!m_m17Enable)
    return;

  uint8_t reply[130U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 0U;
  reply[2U] = MMDVM_M17_LINK_SETUP;

  uint8_t count = 3U;
  for (uint8_t i = 0U; i < length; i++, count++)
    reply[count] = data[i];

  reply[1U] = count;

  writeInt(1U, reply, count);
}

//todo: not generic
void CSerialPort::writeM17Stream(const uint8_t* data, uint8_t length)
{
  if (m_modemState != STATE_M17 && m_modemState != STATE_IDLE)
    return;

  if (!m_m17Enable)
    return;

  uint8_t reply[130U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 0U;
  reply[2U] = MMDVM_M17_STREAM;

  uint8_t count = 3U;
  for (uint8_t i = 0U; i < length; i++, count++)
    reply[count] = data[i];

  reply[1U] = count;

  writeInt(1U, reply, count);
}

//todo: not generic
void CSerialPort::writeM17EOT()
{
  if (m_modemState != STATE_M17 && m_modemState != STATE_IDLE)
    return;

  if (!m_m17Enable)
    return;

  uint8_t reply[3U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 3U;
  reply[2U] = MMDVM_M17_EOT;

  writeInt(1U, reply, 3);
}

//todo: not generic
void CSerialPort::writeM17Lost()
{
  if (m_modemState != STATE_M17 && m_modemState != STATE_IDLE)
    return;

  if (!m_m17Enable)
    return;

  uint8_t reply[3U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 3U;
  reply[2U] = MMDVM_M17_LOST;

  writeInt(1U, reply, 3);
}
#endif

//todo: not generic
#if defined(MODE_FM)
void CSerialPort::writeFMData(const uint8_t* data, uint16_t length)
{
  if (m_modemState != STATE_FM && m_modemState != STATE_IDLE)
    return;

  if (!m_fmEnable)
    return;

  uint8_t reply[512U];

  reply[0U] = MMDVM_FRAME_START;

  if (length > 252U) {
    reply[1U] = 0U;
    reply[2U] = (length + 4U) - 255U;
    reply[3U] = MMDVM_FM_DATA;

    for (uint16_t i = 0U; i < length; i++)
      reply[i + 4U] = data[i];

    writeInt(1U, reply, length + 4U);
  } else {
    reply[1U] = length + 3U;
    reply[2U] = MMDVM_FM_DATA;

    for (uint16_t i = 0U; i < length; i++)
      reply[i + 3U] = data[i];

    writeInt(1U, reply, length + 3U);
  }
}

//todo: not generic
void CSerialPort::writeFMStatus(uint8_t status)
{
  if (m_modemState != STATE_FM && m_modemState != STATE_IDLE)
    return;

  if (!m_fmEnable)
    return;

  uint8_t reply[10U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 4U;
  reply[2U] = MMDVM_FM_STATUS;
  reply[3U] = status;

  writeInt(1U, reply, 4U);
}

//todo: not generic
void CSerialPort::writeFMEOT()
{
  if (m_modemState != STATE_FM && m_modemState != STATE_IDLE)
    return;

  if (!m_fmEnable)
    return;

  uint8_t reply[10U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 3U;
  reply[2U] = MMDVM_FM_EOT;

  writeInt(1U, reply, 3U);
}
#endif

//todo: not generic
#if defined(MODE_AX25)
void CSerialPort::writeAX25Data(const uint8_t* data, uint16_t length)
{
  if (m_modemState != STATE_FM && m_modemState != STATE_IDLE)
    return;

  if (!m_ax25Enable)
    return;

  uint8_t reply[512U];

  reply[0U] = MMDVM_FRAME_START;

  if (length > 252U) {
    reply[1U] = 0U;
    reply[2U] = (length + 4U) - 255U;
    reply[3U] = MMDVM_AX25_DATA;

    for (uint16_t i = 0U; i < length; i++)
      reply[i + 4U] = data[i];

    writeInt(1U, reply, length + 4U);
  } else {
    reply[1U] = length + 3U;
    reply[2U] = MMDVM_AX25_DATA;

    for (uint16_t i = 0U; i < length; i++)
      reply[i + 3U] = data[i];

    writeInt(1U, reply, length + 3U);
  }
}
#endif

#if defined(SERIAL_REPEATER)
void CSerialPort::writeSerialData(const uint8_t* data, uint8_t length)
{
  uint8_t reply[255U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 0U;
  reply[2U] = MMDVM_SERIAL_DATA;

  uint8_t count = 3U;
  for (uint8_t i = 0U; i < length; i++, count++)
    reply[count] = data[i];

  reply[1U] = count;

  writeInt(1U, reply, count);
}
#endif

#if defined(I2C_REPEATER)
void CSerialPort::writeI2CData(const uint8_t* data, uint8_t length)
{
  uint8_t reply[255U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 0U;
  reply[2U] = MMDVM_I2C_DATA;

  uint8_t count = 3U;
  for (uint8_t i = 0U; i < length; i++, count++)
    reply[count] = data[i];

  reply[1U] = count;

  writeInt(1U, reply, count);
}
#endif

//todo: not generic
void CSerialPort::writeCalData(const uint8_t* data, uint8_t length)
{
  if (m_modemState != STATE_DSTARCAL)
    return;

  uint8_t reply[130U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 0U;
  reply[2U] = MMDVM_CAL_DATA;

  uint8_t count = 3U;
  for (uint8_t i = 0U; i < length; i++, count++)
    reply[count] = data[i];

  reply[1U] = count;

  writeInt(1U, reply, count);
}

void CSerialPort::writeRSSIData(const uint8_t* data, uint8_t length)
{
  if (m_modemState != STATE_RSSICAL)
    return;

  uint8_t reply[30U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 0U;
  reply[2U] = MMDVM_RSSI_DATA;

  uint8_t count = 3U;
  for (uint8_t i = 0U; i < length; i++, count++)
    reply[count] = data[i];

  reply[1U] = count;

  writeInt(1U, reply, count);
}

void CSerialPort::writeDebug(const char* text)
{
  if (!m_debug)
    return;

  uint8_t reply[130U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 0U;
  reply[2U] = MMDVM_DEBUG1;

  uint8_t count = 3U;
  for (uint8_t i = 0U; text[i] != '\0'; i++, count++)
    reply[count] = text[i];

  reply[1U] = count;

  writeInt(1U, reply, count, true);
}

void CSerialPort::writeDebug(const char* text, int16_t n1)
{
  if (!m_debug)
    return;

  uint8_t reply[130U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 0U;
  reply[2U] = MMDVM_DEBUG2;

  uint8_t count = 3U;
  for (uint8_t i = 0U; text[i] != '\0'; i++, count++)
    reply[count] = text[i];

  reply[count++] = (n1 >> 8) & 0xFF;
  reply[count++] = (n1 >> 0) & 0xFF;

  reply[1U] = count;

  writeInt(1U, reply, count, true);
}

void CSerialPort::writeDebug(const char* text, int16_t n1, int16_t n2)
{
  if (!m_debug)
    return;

  uint8_t reply[130U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 0U;
  reply[2U] = MMDVM_DEBUG3;

  uint8_t count = 3U;
  for (uint8_t i = 0U; text[i] != '\0'; i++, count++)
    reply[count] = text[i];

  reply[count++] = (n1 >> 8) & 0xFF;
  reply[count++] = (n1 >> 0) & 0xFF;

  reply[count++] = (n2 >> 8) & 0xFF;
  reply[count++] = (n2 >> 0) & 0xFF;

  reply[1U] = count;

  writeInt(1U, reply, count, true);
}

void CSerialPort::writeDebug(const char* text, int16_t n1, int16_t n2, int16_t n3)
{
  if (!m_debug)
    return;

  uint8_t reply[130U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 0U;
  reply[2U] = MMDVM_DEBUG4;

  uint8_t count = 3U;
  for (uint8_t i = 0U; text[i] != '\0'; i++, count++)
    reply[count] = text[i];

  reply[count++] = (n1 >> 8) & 0xFF;
  reply[count++] = (n1 >> 0) & 0xFF;

  reply[count++] = (n2 >> 8) & 0xFF;
  reply[count++] = (n2 >> 0) & 0xFF;

  reply[count++] = (n3 >> 8) & 0xFF;
  reply[count++] = (n3 >> 0) & 0xFF;

  reply[1U] = count;

  writeInt(1U, reply, count, true);
}

void CSerialPort::writeDebug(const char* text, int16_t n1, int16_t n2, int16_t n3, int16_t n4)
{
  if (!m_debug)
    return;

  uint8_t reply[130U];

  reply[0U] = MMDVM_FRAME_START;
  reply[1U] = 0U;
  reply[2U] = MMDVM_DEBUG5;

  uint8_t count = 3U;
  for (uint8_t i = 0U; text[i] != '\0'; i++, count++)
    reply[count] = text[i];

  reply[count++] = (n1 >> 8) & 0xFF;
  reply[count++] = (n1 >> 0) & 0xFF;

  reply[count++] = (n2 >> 8) & 0xFF;
  reply[count++] = (n2 >> 0) & 0xFF;

  reply[count++] = (n3 >> 8) & 0xFF;
  reply[count++] = (n3 >> 0) & 0xFF;

  reply[count++] = (n4 >> 8) & 0xFF;
  reply[count++] = (n4 >> 0) & 0xFF;

  reply[1U] = count;

  writeInt(1U, reply, count, true);
}

void CSerialPort::writeDebugDump(const uint8_t* data, uint16_t length)
{
  uint8_t reply[512U];

  reply[0U] = MMDVM_FRAME_START;

  if (length > 252U) {
    reply[1U] = 0U;
    reply[2U] = (length + 4U) - 255U;
    reply[3U] = MMDVM_DEBUG_DUMP;

    for (uint16_t i = 0U; i < length; i++)
      reply[i + 4U] = data[i];

    writeInt(1U, reply, length + 4U);
  } else {
    reply[1U] = length + 3U;
    reply[2U] = MMDVM_DEBUG_DUMP;

    for (uint16_t i = 0U; i < length; i++)
      reply[i + 3U] = data[i];

    writeInt(1U, reply, length + 3U);
  }
}
