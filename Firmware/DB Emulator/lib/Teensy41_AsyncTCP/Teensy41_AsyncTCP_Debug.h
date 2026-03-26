/****************************************************************************************************************************
  Teensy41_AsyncTCP_Debug.h
  
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

#pragma once

#ifndef _TEENSY41_ASYNC_TCP_DEBUG_H
#define _TEENSY41_ASYNC_TCP_DEBUG_H

#ifdef TEENSY41_ASYNC_TCP_DEBUG_PORT
  #define DBG_PORT_T41_ASYNC      TEENSY41_ASYNC_TCP_DEBUG_PORT
#else
  #define DBG_PORT_T41_ASYNC      Serial
#endif

// Change _TEENSY41_ASYNC_TCP_LOGLEVEL_ to set tracing and logging verbosity
// 0: DISABLED: no logging
// 1: ERROR: errors
// 2: WARN: errors and warnings
// 3: INFO: errors, warnings and informational (default)
// 4: DEBUG: errors, warnings, informational and debug

#ifndef _TEENSY41_ASYNC_TCP_LOGLEVEL_
  #define _TEENSY41_ASYNC_TCP_LOGLEVEL_       1
#endif

/////////////////////////////////////////////////////////

#define T41_ASYNC_PRINT_MARK      T41_ASYNC_PRINT("[T41_ASYNC] ")
#define T41_ASYNC_PRINT_SP        DBG_PORT_T41_ASYNC.print(" ")
#define T41_ASYNC_PRINT_SP0X      DBG_PORT_T41_ASYNC.print(" 0x")

#define T41_ASYNC_PRINT           DBG_PORT_T41_ASYNC.print
#define T41_ASYNC_PRINTLN         DBG_PORT_T41_ASYNC.println
#define T41_ASYNC_PRINTF          DBG_PORT_T41_ASYNC.printf

/////////////////////////////////////////////////////////

#define ASYNC_TCP_DEBUG(...) 				if(_TEENSY41_ASYNC_TCP_LOGLEVEL_>3) { T41_ASYNC_PRINTF(__VA_ARGS__); }
#define ASYNC_TCP_SSL_DEBUG(...) 		if(_TEENSY41_ASYNC_TCP_LOGLEVEL_>3) { T41_ASYNC_PRINTF(__VA_ARGS__); }

#define ASYNC_TCP_ASSERT( a ) do{ if(!(a)){T41_ASYNC_PRINTF("ASSERT: %s %u \n", __FILE__, __LINE__);}}while(0)

/////////////////////////////////////////////////////////

#define ATCP_LOGERROR(x)         if(_TEENSY41_ASYNC_TCP_LOGLEVEL_>0) { T41_ASYNC_PRINT_MARK; T41_ASYNC_PRINTLN(x); }
#define ATCP_LOGERROR0(x)        if(_TEENSY41_ASYNC_TCP_LOGLEVEL_>0) { T41_ASYNC_PRINT(x); }
#define ATCP_LOGERROR1(x,y)      if(_TEENSY41_ASYNC_TCP_LOGLEVEL_>0) { T41_ASYNC_PRINT_MARK; T41_ASYNC_PRINT(x); T41_ASYNC_PRINT_SP; T41_ASYNC_PRINTLN(y); }
#define T41_ASYNC_HEXLOGERROR1(x,y)   if(_TEENSY41_ASYNC_TCP_LOGLEVEL_>0) { T41_ASYNC_PRINT_MARK; T41_ASYNC_PRINT(x); T41_ASYNC_PRINT_SP0X; T41_ASYNC_PRINTLN(y, HEX); }
#define ATCP_LOGERROR2(x,y,z)    if(_TEENSY41_ASYNC_TCP_LOGLEVEL_>0) { T41_ASYNC_PRINT_MARK; T41_ASYNC_PRINT(x); T41_ASYNC_PRINT_SP; T41_ASYNC_PRINT(y); T41_ASYNC_PRINT_SP; T41_ASYNC_PRINTLN(z); }
#define T41_ASYNC_HEXLOGERROR2(x,y,z) if(_TEENSY41_ASYNC_TCP_LOGLEVEL_>0) { T41_ASYNC_PRINT_MARK; T41_ASYNC_PRINT(x); T41_ASYNC_PRINT_SP0X; T41_ASYNC_PRINT(y, HEX); T41_ASYNC_PRINT_SP0X; T41_ASYNC_PRINTLN(z, HEX); }
#define ATCP_LOGERROR3(x,y,z,w)  if(_TEENSY41_ASYNC_TCP_LOGLEVEL_>0) { T41_ASYNC_PRINT_MARK; T41_ASYNC_PRINT(x); T41_ASYNC_PRINT_SP; T41_ASYNC_PRINT(y); T41_ASYNC_PRINT_SP; T41_ASYNC_PRINT(z); T41_ASYNC_PRINT_SP; T41_ASYNC_PRINTLN(w); }

/////////////////////////////////////////////////////////

#define ATCP_LOGWARN(x)          if(_TEENSY41_ASYNC_TCP_LOGLEVEL_>1) { T41_ASYNC_PRINT_MARK; T41_ASYNC_PRINTLN(x); }
#define ATCP_LOGWARN0(x)         if(_TEENSY41_ASYNC_TCP_LOGLEVEL_>1) { T41_ASYNC_PRINT(x); }
#define ATCP_LOGWARN1(x,y)       if(_TEENSY41_ASYNC_TCP_LOGLEVEL_>1) { T41_ASYNC_PRINT_MARK; T41_ASYNC_PRINT(x); T41_ASYNC_PRINT_SP; T41_ASYNC_PRINTLN(y); }
#define T41_ASYNC_HEXLOGWARN1(x,y)    if(_TEENSY41_ASYNC_TCP_LOGLEVEL_>1) { T41_ASYNC_PRINT_MARK; T41_ASYNC_PRINT(x); T41_ASYNC_PRINT_SP0X; T41_ASYNC_PRINTLN(y, HEX); }
#define ATCP_LOGWARN2(x,y,z)     if(_TEENSY41_ASYNC_TCP_LOGLEVEL_>1) { T41_ASYNC_PRINT_MARK; T41_ASYNC_PRINT(x); T41_ASYNC_PRINT_SP; T41_ASYNC_PRINT(y); T41_ASYNC_PRINT_SP; T41_ASYNC_PRINTLN(z); }
#define T41_ASYNC_HEXLOGWARN2(x,y,z)  if(_TEENSY41_ASYNC_TCP_LOGLEVEL_>1) { T41_ASYNC_PRINT_MARK; T41_ASYNC_PRINT(x); T41_ASYNC_PRINT_SP0X; T41_ASYNC_PRINT(y, HEX); T41_ASYNC_PRINT_SP0X; T41_ASYNC_PRINTLN(z, HEX); }
#define ATCP_LOGWARN3(x,y,z,w)   if(_TEENSY41_ASYNC_TCP_LOGLEVEL_>1) { T41_ASYNC_PRINT_MARK; T41_ASYNC_PRINT(x); T41_ASYNC_PRINT_SP; T41_ASYNC_PRINT(y); T41_ASYNC_PRINT_SP; T41_ASYNC_PRINT(z); T41_ASYNC_PRINT_SP; T41_ASYNC_PRINTLN(w); }

/////////////////////////////////////////////////////////

#define ATCP_LOGINFO(x)          if(_TEENSY41_ASYNC_TCP_LOGLEVEL_>2) { T41_ASYNC_PRINT_MARK; T41_ASYNC_PRINTLN(x); }
#define ATCP_LOGINFO0(x)         if(_TEENSY41_ASYNC_TCP_LOGLEVEL_>2) { T41_ASYNC_PRINT(x); }
#define ATCP_LOGINFO1(x,y)       if(_TEENSY41_ASYNC_TCP_LOGLEVEL_>2) { T41_ASYNC_PRINT_MARK; T41_ASYNC_PRINT(x); T41_ASYNC_PRINT_SP; T41_ASYNC_PRINTLN(y); }
#define T41_ASYNC_HEXLOGINFO1(x,y)    if(_TEENSY41_ASYNC_TCP_LOGLEVEL_>2) { T41_ASYNC_PRINT_MARK; T41_ASYNC_PRINT(x); T41_ASYNC_PRINT_SP0X; T41_ASYNC_PRINTLN(y, HEX); }
#define ATCP_LOGINFO2(x,y,z)     if(_TEENSY41_ASYNC_TCP_LOGLEVEL_>2) { T41_ASYNC_PRINT_MARK; T41_ASYNC_PRINT(x); T41_ASYNC_PRINT_SP; T41_ASYNC_PRINT(y); T41_ASYNC_PRINT_SP; T41_ASYNC_PRINTLN(z); }
#define T41_ASYNC_HEXLOGINFO2(x,y,z)  if(_TEENSY41_ASYNC_TCP_LOGLEVEL_>2) { T41_ASYNC_PRINT_MARK; T41_ASYNC_PRINT(x); T41_ASYNC_PRINT_SP0X; T41_ASYNC_PRINT(y, HEX); T41_ASYNC_PRINT_SP0X; T41_ASYNC_PRINTLN(z, HEX); }
#define ATCP_LOGINFO3(x,y,z,w)   if(_TEENSY41_ASYNC_TCP_LOGLEVEL_>2) { T41_ASYNC_PRINT_MARK; T41_ASYNC_PRINT(x); T41_ASYNC_PRINT_SP; T41_ASYNC_PRINT(y); T41_ASYNC_PRINT_SP; T41_ASYNC_PRINT(z); T41_ASYNC_PRINT_SP; T41_ASYNC_PRINTLN(w); }

/////////////////////////////////////////////////////////

#define ATCP_LOGDEBUG(x)         if(_TEENSY41_ASYNC_TCP_LOGLEVEL_>3) { T41_ASYNC_PRINT_MARK; T41_ASYNC_PRINTLN(x); }
#define ATCP_LOGDEBUG0(x)        if(_TEENSY41_ASYNC_TCP_LOGLEVEL_>3) { T41_ASYNC_PRINT(x); }
#define ATCP_LOGDEBUG1(x,y)      if(_TEENSY41_ASYNC_TCP_LOGLEVEL_>3) { T41_ASYNC_PRINT_MARK; T41_ASYNC_PRINT(x); T41_ASYNC_PRINT_SP; T41_ASYNC_PRINTLN(y); }
#define T41_ASYNC_HEXLOGDEBUG1(x,y)   if(_TEENSY41_ASYNC_TCP_LOGLEVEL_>3) { T41_ASYNC_PRINT_MARK; T41_ASYNC_PRINT(x); T41_ASYNC_PRINT_SP0X; T41_ASYNC_PRINTLN(y, HEX); }
#define ATCP_LOGDEBUG2(x,y,z)    if(_TEENSY41_ASYNC_TCP_LOGLEVEL_>3) { T41_ASYNC_PRINT_MARK; T41_ASYNC_PRINT(x); T41_ASYNC_PRINT_SP; T41_ASYNC_PRINT(y); T41_ASYNC_PRINT_SP; T41_ASYNC_PRINTLN(z); }
#define T41_ASYNC_HEXLOGDEBUG2(x,y,z) if(_TEENSY41_ASYNC_TCP_LOGLEVEL_>3) { T41_ASYNC_PRINT_MARK; T41_ASYNC_PRINT(x); T41_ASYNC_PRINT_SP0X; T41_ASYNC_PRINT(y, HEX); T41_ASYNC_PRINT_SP0X; T41_ASYNC_PRINTLN(z, HEX); }
#define ATCP_LOGDEBUG3(x,y,z,w)  if(_TEENSY41_ASYNC_TCP_LOGLEVEL_>3) { T41_ASYNC_PRINT_MARK; T41_ASYNC_PRINT(x); T41_ASYNC_PRINT_SP; T41_ASYNC_PRINT(y); T41_ASYNC_PRINT_SP; T41_ASYNC_PRINT(z); T41_ASYNC_PRINT_SP; T41_ASYNC_PRINTLN(w); }

/////////////////////////////////////////////////////////

#endif    // _TEENSY41_ASYNC_TCP_DEBUG_H
