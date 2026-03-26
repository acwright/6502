/****************************************************************************************************************************
  cbuf.hpp
  
  cbuf.hpp - Circular buffer implementation
  Copyright (c) 2014 Ivan Grokhotkov. All rights reserved.
  This file is part of the esp8266 core for Arduino environment.
   
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

#ifndef _TEENSY41_ASYNC_TCP_CBUF_HPP_
#define _TEENSY41_ASYNC_TCP_CBUF_HPP_

#include <stddef.h>
#include <stdint.h>
#include <string.h>

/////////////////////////////////////////////////

class cbuf 
{
  public:
    cbuf(size_t size);
    ~cbuf();

    size_t resizeAdd(size_t addSize);
    size_t resize(size_t newSize);
    size_t available() const;
    size_t size();

    size_t room() const;

    inline bool empty() const 
    {
      return _begin == _end;
    }

    inline bool full() const 
    {
      return wrap_if_bufend(_end + 1) == _begin;
    }

    int peek();
    size_t peek(char *dst, size_t size);

    int read();
    size_t read(char* dst, size_t size);

    size_t write(char c);
    size_t write(const char* src, size_t size);

    void flush();
    size_t remove(size_t size);

    cbuf *next;

  private:
    inline char* wrap_if_bufend(char* ptr) const 
    {
      return (ptr == _bufend) ? _buf : ptr;
    }

    size_t _size;
    char* _buf;
    const char* _bufend;
    char* _begin;
    char* _end;

};

/////////////////////////////////////////////////

#endif		// _TEENSY41_ASYNC_TCP_CBUF_HPP_
