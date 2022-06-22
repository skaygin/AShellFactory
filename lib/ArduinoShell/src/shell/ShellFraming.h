/*
MIT License

Copyright (c) 2022 Serkan KAYGIN

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef _SHELL_FRAMING_H_
#define _SHELL_FRAMING_H_

#include <Arduino.h>
//********************************
// Two way command framing for Command Shell Library
// Version 1.0
//
//
// By Serkan KAYGIN
//********************************

// notes to implementers:
//   in receive function, writing 0 (zero) to input Print resets the receive buffer
//   designed to be used with SOH (start of header)

// receive function returns one of these results
const int8_t SHELL_FRAME_NOT_RECEIVED = 0;
const int8_t SHELL_FRAME_RECEIVED = 1;
const int8_t SHELL_BAD_FRAME_RECEIVED = -1;

class ShellFraming
{
public:
    // REQUEST
    virtual void resetReceive() {}            // clear buffers, reset checksum
    virtual int8_t receive(Print *in, char c) // handle receive, return frame status
    {
        // default implementation receives nothing
        return SHELL_FRAME_NOT_RECEIVED;
    }
    // RESPONSE
    virtual void beginSend(Print *out) {} // send frame header
    virtual void send(Print *out, char c) // send data, updates checksum
    {
        // default implementation just forwards
        out->write(c);
    }
    virtual void endSend(Print *out) {} // send frame trailer, checksum etc.
};

#endif //_SHELL_FRAMING_H_
