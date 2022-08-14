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
#ifndef _INT32_PARSER_H_
#define _INT32_PARSER_H_

#include <Arduino.h>

// Positional numeral with a specified base(radix) 2-36,
// Has fractions after radix point, -128 means no radix point allowed
// Negative fraction_places are reserved for now, only positive ones are implemented
bool parsePositional32(const char *str, uint32_t *out, uint8_t base, int8_t fraction_places = -128);

bool parseUInt32(const char *str_literal, uint32_t *out, uint8_t base = 0);
bool parseInt32(const char *str_literal, int32_t *out, uint8_t base = 0);
bool parseDecimal32(const char *str_literal, int32_t *out, uint8_t decimal_places);

#endif //_INT32_PARSER_H_