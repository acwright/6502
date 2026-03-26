/****************************************************************************************************************************
  Teensy41_AsyncTCP_Buffer.hpp

  @file Teensy41_AsyncTCPbuffer.h
  @date  22.01.2016
  @author Markus Sattler

  Copyright (c) 2015 Markus Sattler. All rights reserved.
  This file is part of the Async TCP for ESP.

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

#ifndef _TEENSY41_ASYNC_TCP_BUFFER_HPP_
#define _TEENSY41_ASYNC_TCP_BUFFER_HPP_

/////////////////////////////////////////////////

#include <Arduino.h>
#include <cbuf.hpp>

#include "Teensy41_AsyncTCP.hpp"

#include "Teensy41_AsyncTCP_Debug.h"

/////////////////////////////////////////////////

typedef enum 
{
  ATB_RX_MODE_NONE,
  ATB_RX_MODE_FREE,
  ATB_RX_MODE_READ_BYTES,
  ATB_RX_MODE_TERMINATOR,
  ATB_RX_MODE_TERMINATOR_STRING
} atbRxMode_t;

/////////////////////////////////////////////////

class AsyncTCPbuffer: public Print 
{
  public:

    typedef std::function<size_t(uint8_t * payload, size_t length)> AsyncTCPbufferDataCb;
    typedef std::function<void(bool ok, void * ret)> AsyncTCPbufferDoneCb;
    typedef std::function<bool(AsyncTCPbuffer * obj)> AsyncTCPbufferDisconnectCb;

    AsyncTCPbuffer(AsyncClient* c);
    virtual ~AsyncTCPbuffer();

    size_t write(String & data);
    size_t write(uint8_t data);
    size_t write(const char* data);
    size_t write(const char *data, size_t len);
    size_t write(const uint8_t *data, size_t len);

    void flush();

    void noCallback();

    void readStringUntil(char terminator, String * str, AsyncTCPbufferDoneCb done);

    // TODO implement read terminator non string
    //void readBytesUntil(char terminator, char *buffer, size_t length, AsyncTCPbufferDoneCb done);
    //void readBytesUntil(char terminator, uint8_t *buffer, size_t length, AsyncTCPbufferDoneCb done);

    void readBytes(char *buffer, size_t length, AsyncTCPbufferDoneCb done);
    void readBytes(uint8_t *buffer, size_t length, AsyncTCPbufferDoneCb done);

    // TODO implement
    // void setTimeout(size_t timeout);

    void onData(AsyncTCPbufferDataCb cb);
    void onDisconnect(AsyncTCPbufferDisconnectCb cb);

    IPAddress remoteIP();
    uint16_t  remotePort();
    IPAddress localIP();
    uint16_t  localPort();

    bool connected();

    void stop();
    void close();

  protected:
    AsyncClient* _client;
    cbuf * _TXbufferRead;
    cbuf * _TXbufferWrite;
    cbuf * _RXbuffer;
    atbRxMode_t _RXmode;
    size_t _rxSize;
    char _rxTerminator;
    uint8_t * _rxReadBytesPtr;
    String * _rxReadStringPtr;

    AsyncTCPbufferDataCb _cbRX;
    AsyncTCPbufferDoneCb _cbDone;
    AsyncTCPbufferDisconnectCb _cbDisconnect;

    void _attachCallbacks();
    void _sendBuffer();
    void _on_close();
    void _rxData(uint8_t *buf, size_t len);
    size_t _handleRxBuffer(uint8_t *buf, size_t len);
};

/////////////////////////////////////////////////

#endif // _TEENSY41_ASYNC_TCP_BUFFER_HPP_
