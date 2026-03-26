/****************************************************************************************************************************
  AsyncPrinter_Impl.h
  
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

#ifndef _TEENSY41_ASYNC_PRINTER_IMPL_H_
#define _TEENSY41_ASYNC_PRINTER_IMPL_H_

/////////////////////////////////////////////////

AsyncPrinter::AsyncPrinter()
  : _client(NULL)
  , _data_cb(NULL)
  , _data_arg(NULL)
  , _close_cb(NULL)
  , _close_arg(NULL)
  , _tx_buffer(NULL)
  , _tx_buffer_size(TCP_MSS)
  , next(NULL)
{}

/////////////////////////////////////////////////

AsyncPrinter::AsyncPrinter(AsyncClient *client, size_t txBufLen)
  : _client(client)
  , _data_cb(NULL)
  , _data_arg(NULL)
  , _close_cb(NULL)
  , _close_arg(NULL)
  , _tx_buffer(NULL)
  , _tx_buffer_size(txBufLen)
  , next(NULL)
{
  _attachCallbacks();
  _tx_buffer = new (std::nothrow) cbuf(_tx_buffer_size);

  if (_tx_buffer == NULL)
  {
    //What should we do?
  }
}

/////////////////////////////////////////////////

AsyncPrinter::~AsyncPrinter() 
{
  _on_close();
}

/////////////////////////////////////////////////

void AsyncPrinter::onData(ApDataHandler cb, void *arg) 
{
  _data_cb = cb;
  _data_arg = arg;
}

/////////////////////////////////////////////////

void AsyncPrinter::onClose(ApCloseHandler cb, void *arg) 
{
  _close_cb = cb;
  _close_arg = arg;
}

/////////////////////////////////////////////////

int AsyncPrinter::connect(IPAddress ip, uint16_t port) 
{
  if (_client != NULL && connected())
    return 0;
    
  _client = new (std::nothrow) AsyncClient();

  if (_client == NULL)
  {
    ATCP_LOGERROR("AsyncPrinter::connect(ip): Error NULL _client");
  }

  _client->onConnect([](void *obj, AsyncClient * c) {
    ((AsyncPrinter*)(obj))->_onConnect(c);
  }, this);

  
  if (_client->connect(ip, port)) 
  {
    while (_client && _client->state() < 4)
      delay(1);
      
    return connected();
  }
  
  return 0;
}

/////////////////////////////////////////////////

int AsyncPrinter::connect(const char *host, uint16_t port) 
{
  if (_client != NULL && connected())
    return 0;
    
  _client = new (std::nothrow) AsyncClient();
  
  if (_client == NULL)
  {
    ATCP_LOGERROR("AsyncPrinter::connect(host): Error NULL _client");
  }

  _client->onConnect([](void *obj, AsyncClient * c) 
  {
    ((AsyncPrinter*)(obj))->_onConnect(c);
  }, this);
  
  if (_client->connect(host, port)) 
  {
    while (_client && _client->state() < 4)
      delay(1);
      
    return connected();
  }
  
  return 0;
}

/////////////////////////////////////////////////

void AsyncPrinter::_onConnect(AsyncClient *c) 
{
  (void) c;
  
  if (_tx_buffer != NULL) 
  {
    cbuf *b = _tx_buffer;
    _tx_buffer = NULL;
    delete b;
  }
  
  _tx_buffer = new (std::nothrow) cbuf(_tx_buffer_size);

  if (_tx_buffer)
  {
    ATCP_LOGERROR("AsyncPrinter::_onConnect: Error NULL _tx_buffer");
  }

  _attachCallbacks();
}

/////////////////////////////////////////////////

AsyncPrinter::operator bool() 
{
  return connected();
}

AsyncPrinter & AsyncPrinter::operator=(const AsyncPrinter &other) 
{
  if (_client != NULL) 
  {
    _client->close(true);
    _client = NULL;
  }
  
  _tx_buffer_size = other._tx_buffer_size;
  
  if (_tx_buffer != NULL) 
  {
    cbuf *b = _tx_buffer;
    _tx_buffer = NULL;
    delete b;
  }
  
  _tx_buffer = new (std::nothrow) cbuf(other._tx_buffer_size);

  if (_tx_buffer == NULL)
  {
    ATCP_LOGERROR("AsyncPrinter:operator=: Error NULL _tx_buffer");
  }

  _client = other._client;
  _attachCallbacks();
  
  return *this;
}

/////////////////////////////////////////////////

size_t AsyncPrinter::write(uint8_t data) 
{
  return write(&data, 1);
}

/////////////////////////////////////////////////

size_t AsyncPrinter::write(const uint8_t *data, size_t len) 
{
  if (_tx_buffer == NULL || !connected())
    return 0;
    
  size_t toWrite = 0;
  size_t toSend = len;
  
  while (_tx_buffer->room() < toSend) 
  {
    toWrite = _tx_buffer->room();
    _tx_buffer->write((const char*)data, toWrite);
    
    while (connected() && !_client->canSend())
      delay(0);
      
    if (!connected())
      return 0; // or len - toSend;
      
    _sendBuffer();
    toSend -= toWrite;
  }
  
  _tx_buffer->write((const char*)(data + (len - toSend)), toSend);
  
  while (connected() && !_client->canSend()) 
    delay(0);
    
  if (!connected()) 
    return 0; // or len - toSend;
    
  _sendBuffer();
  
  return len;
}

/////////////////////////////////////////////////

bool AsyncPrinter::connected() 
{
  return (_client != NULL && _client->connected());
}

/////////////////////////////////////////////////

void AsyncPrinter::close() 
{
  if (_client != NULL)
    _client->close(true);
}

/////////////////////////////////////////////////

size_t AsyncPrinter::_sendBuffer()
{
  size_t available = _tx_buffer->available();
  
  if (!connected() || !_client->canSend() || available == 0)
    return 0;
    
  size_t sendable = _client->space();
  
  if (sendable < available)
    available = sendable;
    
  char *out = new (std::nothrow) char[available];

  if (out == NULL)
  {
    // Connection should be aborted instead
    ATCP_LOGERROR("AsyncPrinter:_sendBuffer: Error NULL out");
  }

  _tx_buffer->read(out, available);
  size_t sent = _client->write(out, available);
  delete out;
  
  return sent;
}

/////////////////////////////////////////////////

void AsyncPrinter::_onData(void *data, size_t len) 
{
  if (_data_cb)
    _data_cb(_data_arg, this, (uint8_t*)data, len);
}

/////////////////////////////////////////////////

void AsyncPrinter::_on_close() 
{
  if (_client != NULL) 
  {
    _client = NULL;
  }
  
  if (_tx_buffer != NULL) 
  {
    cbuf *b = _tx_buffer;
    _tx_buffer = NULL;
    delete b;
  }
  
  if (_close_cb)
    _close_cb(_close_arg, this);
}

/////////////////////////////////////////////////

void AsyncPrinter::_attachCallbacks() 
{
  _client->onPoll([](void *obj, AsyncClient * c) 
  {
    (void) c;
    ((AsyncPrinter*)(obj))->_sendBuffer();
  }, this);
  
  _client->onAck([](void *obj, AsyncClient * c, size_t len, uint32_t time) 
  {
    (void) c;
    (void) len;
    (void) time;
    ((AsyncPrinter*)(obj))->_sendBuffer();
  }, this);
  
  _client->onDisconnect([](void *obj, AsyncClient * c) 
  {
    ((AsyncPrinter*)(obj))->_on_close();
    delete c;
  }, this);
  
  _client->onData([](void *obj, AsyncClient * c, void *data, size_t len) 
  {
    (void) c;
    ((AsyncPrinter*)(obj))->_onData(data, len);
  }, this);
}

/////////////////////////////////////////////////

#endif		// _TEENSY41_ASYNC_PRINTER_IMPL_H_
