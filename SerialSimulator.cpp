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

void CSerialPort::beginInt(uint8_t n, int speed)
{
    char msg[50];
    snprintf(msg, 50, "parameters %u and %d", n, speed);
    debug(msg);
}

int CSerialPort::availableForReadInt(uint8_t n)
{
  debug("-");
  std::this_thread::sleep_for(std::chrono::milliseconds(200));
  return false;
}

int CSerialPort::availableForWriteInt(uint8_t n)
{
  debug("-");
  return true;
}

uint8_t CSerialPort::readInt(uint8_t n)
{
  debug("-");
  return 0U;
}

void CSerialPort::writeInt(uint8_t n, const uint8_t* data, uint16_t length, bool flush)
{
  char port[128];
  snprintf(port, 128, "port: %d, data: %d", n, *data);
  debug(port);
}

#endif

