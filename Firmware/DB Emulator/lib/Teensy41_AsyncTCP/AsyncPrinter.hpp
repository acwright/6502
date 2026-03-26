/****************************************************************************************************************************
  AsyncPrinter.hpp
  
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

#ifndef _TEENSY41_ASYNC_PRINTER_HPP_
#define _TEENSY41_ASYNC_PRINTER_HPP_

#include "Arduino.h"

#include "Teensy41_AsyncTCP.hpp"

#include "cbuf.hpp"

/////////////////////////////////////////////////

class AsyncPrinter;

typedef std::function<void(void*, AsyncPrinter*, uint8_t*, size_t)> ApDataHandler;
typedef std::function<void(void*, AsyncPrinter*)> ApCloseHandler;

/////////////////////////////////////////////////

class AsyncPrinter: public Print
{
  private:
    AsyncClient *_client;
    ApDataHandler _data_cb;
    void *_data_arg;
    ApCloseHandler _close_cb;
    void *_close_arg;
    cbuf *_tx_buffer;
    size_t _tx_buffer_size;

    void _onConnect(AsyncClient *c);

  public:
    AsyncPrinter *next;

    AsyncPrinter();
    AsyncPrinter(AsyncClient *client, size_t txBufLen = TCP_MSS);

    virtual ~AsyncPrinter();

    int connect(IPAddress ip, uint16_t port);
    int connect(const char *host, uint16_t port);

    void onData(ApDataHandler cb, void *arg);
    void onClose(ApCloseHandler cb, void *arg);

    operator bool();
    AsyncPrinter & operator=(const AsyncPrinter &other);

    size_t write(uint8_t data);
    size_t write(const uint8_t *data, size_t len);

    bool connected();
    void close();

    size_t _sendBuffer();
    void _onData(void *data, size_t len);
    void _on_close();
    void _attachCallbacks();
};

/////////////////////////////////////////////////

#endif 	// _TEENSY41_ASYNC_PRINTER_HPP_
