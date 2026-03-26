/****************************************************************************************************************************
  Teensy41_AsyncTCP.h
   
  Teensy41_AsyncTCP is a library for Teensy4.1 using LwIP-based QNEthernet
  
  Based on and modified from :
  
  1) ESPAsyncTCP    (https://github.com/me-no-dev/ESPAsyncTCP)
  2) AsyncTCP       (https://github.com/me-no-dev/AsyncTCP)
  
  Built by Khoi Hoang https://github.com/khoih-prog/Teensy41_AsyncTCP
  
  This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License 
  as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
  This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 
  Version: 1.1.0
  
  Version Modified By   Date      Comments
  ------- -----------  ---------- -----------
  1.0.0    K Hoang     17/03/2022 Initial coding to support only Teensy4.1 using QNEthernet
  1.1.0    K Hoang     26/09/2022 Fix issue with slow browsers or network. Clean up. Remove hard-code if possible
 *****************************************************************************************************************************/

/****************************************************************************************************************************
  Asynchronous TCP library for Espressif MCUs

  Copyright (c) 2016 Hristo Gochkov. All rights reserved.
  This file is part of the esp8266 core for Arduino environment.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *****************************************************************************************************************************/

#pragma once

#ifndef _TEENSY41_ASYNC_TCP_H_
#define _TEENSY41_ASYNC_TCP_H_

/////////////////////////////////////////////////

#if ( defined(CORE_TEENSY) && defined(__IMXRT1062__) && defined(ARDUINO_TEENSY41) )
  // For Teensy 4.1
  #define BOARD_NAME              "TEENSY 4.1"
  // Use true for NativeEthernet Library, false if using other Ethernet libraries
  #define USE_NATIVE_ETHERNET     false
  #define USE_QN_ETHERNET         true
#else
  #error Only Teensy 4.1 supported
#endif

/////////////////////////////////////////////////

#include <QNEthernet.h>

#ifndef ASYNC_TCP_SSL_ENABLED
  #define ASYNC_TCP_SSL_ENABLED       0
#endif

#include <Teensy41_AsyncTCP.hpp>
#include <Teensy41_AsyncTCP_Impl.h>

#include <AsyncPrinter.hpp>
#include <AsyncPrinter_Impl.h>

#include <cbuf.hpp>
#include <cbuf_Impl.h>

// KH, Not in use, keep for future
//////////////////////
//#include <SyncClient.hpp>
//#include <SyncClient_Impl.h>

//#include <Teensy41_AsyncTCP_Buffer.hpp>
//#include <Teensy41_AsyncTCP_Buffer_Impl.h>
//////////////////////


#endif /* _TEENSY41_ASYNC_TCP_H_ */
