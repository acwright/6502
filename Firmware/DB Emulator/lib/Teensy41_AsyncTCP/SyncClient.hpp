/****************************************************************************************************************************
  SyncClient.hpp
  
  Asynchronous TCP library for Espressif MCUs

  Copyright (c) 2016 Hristo Gochkov. All rights reserved.
  This file is part of the esp8266 core for Arduino environment.
   
  Teensy41_AsyncTCP is a library for Teensy4.1 using LwIP-based QNEthernet
  
  Based on and modified from :
  
  1) ESPAsyncTCP    (https://github.com/me-no-dev/ESPAsyncTCP)
  2) AsyncTCP       (https://github.com/me-no-dev/AsyncTCP)
  
  Built by Khoi Hoang https://github.com/khoih-prog/Teensy41_AsyncTCP
  
  This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License 
  as published bythe Free Software Foundation, either version 3 of the License, or (at your option) any later version.
  This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 
  Version: 1.1.0
  
  Version Modified By   Date      Comments
  ------- -----------  ---------- -----------
  1.0.0    K Hoang     17/03/2022 Initial coding to support only Teensy4.1 using QNEthernet
  1.1.0    K Hoang     26/09/2022 Fix issue with slow browsers or network. Clean up. Remove hard-code if possible
 *****************************************************************************************************************************/

#pragma once

#ifndef _TEENSY41_ASYNC_TCP_SYNC_CLIENT_HPP_
#define _TEENSY41_ASYNC_TCP_SYNC_CLIENT_HPP_

/////////////////////////////////////////////////

#if defined(LWIP_NETIF_TX_SINGLE_PBUF)
	#undef LWIP_NETIF_TX_SINGLE_PBUF
	#define LWIP_NETIF_TX_SINGLE_PBUF      1
#endif

/////////////////////////////////////////////////

#include "Client.h"

// Needed for Arduino core releases prior to 2.5.0, because of changes
// made to accommodate Arduino core 2.5.0
// CONST was 1st defined in Core 2.5.0 in IPAddress.h
#ifndef CONST
  #define CONST
#endif

/////////////////////////////////////////////////

class cbuf;
class AsyncClient;

/////////////////////////////////////////////////

class SyncClient: public Client 
{
  private:
    AsyncClient *_client;
    cbuf *_tx_buffer;
    size_t _tx_buffer_size;
    cbuf *_rx_buffer;
    int *_ref;

    size_t _sendBuffer();
    void _onData(void *data, size_t len);
    void _onConnect(AsyncClient *c);
    void _onDisconnect();
    void _attachCallbacks();
    void _attachCallbacks_Disconnect();
    void _attachCallbacks_AfterConnected();
    void _release();

  public:
    SyncClient(size_t txBufLen = TCP_MSS);
    SyncClient(AsyncClient *client, size_t txBufLen = TCP_MSS);
    virtual ~SyncClient();

    int ref();
    int unref();
    
    operator bool() 
    {
      return connected();
    }
    
    SyncClient & operator=(const SyncClient &other);

#if ASYNC_TCP_SSL_ENABLED

    int _connect(const IPAddress& ip, uint16_t port, bool secure);
    
    int connect(CONST IPAddress& ip, uint16_t port, bool secure) 
    {
      return _connect(ip, port, secure);
    }
    
    int connect(IPAddress ip, uint16_t port, bool secure) 
    {
      return _connect(reinterpret_cast<const IPAddress&>(ip), port, secure);
    }
    
    int connect(const char *host, uint16_t port, bool secure);
    
    int connect(CONST IPAddress& ip, uint16_t port) 
    {
      return _connect(ip, port, false);
    }
    
    int connect(IPAddress ip, uint16_t port) 
    {
      return _connect(reinterpret_cast<const IPAddress&>(ip), port, false);
    }
    
    int connect(const char *host, uint16_t port) 
    {
      return connect(host, port, false);
    }
    
#else

    int _connect(const IPAddress& ip, uint16_t port);
    
    int connect(CONST IPAddress& ip, uint16_t port) 
    {
      return _connect(ip, port);
    }
    
    int connect(IPAddress ip, uint16_t port) 
    {
      return _connect(reinterpret_cast<const IPAddress&>(ip), port);
    }
    
    int connect(const char *host, uint16_t port);
    
#endif

    void setTimeout(uint32_t seconds);

    uint8_t status();
    uint8_t connected();

    bool stop(unsigned int maxWaitMs);
    bool flush(unsigned int maxWaitMs);
    
    void stop() 
    {
      (void)stop(0);
    }
    
    void flush() 
    {
      (void)flush(0);
    }
    
    size_t write(uint8_t data);
    size_t write(const uint8_t *data, size_t len);

    int available();
    int peek();
    int read();
    int read(uint8_t *data, size_t len);
};

/////////////////////////////////////////////////

#endif // _TEENSY41_ASYNC_TCP_SYNC_CLIENT_HPP_
