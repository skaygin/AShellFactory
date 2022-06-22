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
#ifndef _ARGUMENT_READER_H_
#define _ARGUMENT_READER_H_

//********************************
// Argument Reader for Command Shell Library
// Version 1.0
//
// Modifies the input buffer by setting null terminations to the end of each argument
// Begins with a char* and reads first argument incrementing the pointer
// Long,Int,Enum and String arguments are supported
// Multiple separators (like spaces) are counted as one, however ',' s also
//
// By Serkan KAYGIN
//********************************

#include <Arduino.h>

/**
 * @brief Command line parser which reads arguments sequentially.
 * Supports strings, numbers, enumerations.
 * Decimal, binary and hexadecimal integers are accepted.
 */
class ArgumentReader
{
private:
    byte *cmdptr_;
    char separator_;

public:
    static bool atol(const char *str, long *result);
    ArgumentReader(char separator = ' ');
    void begin(byte *cmdlinebuf);
    int8_t readLong(int32_t *arg, int32_t min = (long)0x80000000, int32_t max = 0x7fffffff);
    int8_t readInt(int16_t *arg, int16_t min = (int)0x8000, int16_t max = 0x7fff);
    int8_t readEnum(uint8_t *arg, PGM_P options, char delimiter = '|');
    int16_t readString(char **arg, bool uppercase, char separator);
    int16_t readString(char **arg, bool uppercase = false);
    int16_t readToEnd(char **arg, bool uppercase = false);
    static void printEnum(Print &output, uint8_t value, PGM_P options, char delimiter = '|');
    char *peek();
};

#endif //_ARGUMENT_READER_H_
