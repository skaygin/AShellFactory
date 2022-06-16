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
#include "ArgumentReader.h"

// local implementation of atol saves program storage by not including unnecessary libs

// known issues/restrictions: 0x and 0b are considered as zero, overflows are ignored
// bool ArgumentReader::atol(const char *str, long *result)

bool ArgumentReader::atol(const char *str, long *result)
{
  char c = *(str++);
  uint8_t base = 0;
  uint8_t flags = 0; // bit 7 neg, bit 0 not-first
  long val = 0;
  while (c)
  {
    if (base)
    {
      // combined state, base is determined, at least second character
      char C = c & ~0x20; // uppercase
      val *= base;
      if (c < '0')
        return false;
      else if (c <= '1')
        val += c - '0';
      else if (c <= '9')
      {
        if (base < 10)
          return false;
        val += c - '0';
      }
      else if (C >= 'A' && C <= 'F')
      {
        if (base < 16)
          return false;
        val += C - ('A' - 10);
      }
      else
        return false;
    }
    else if (flags & 1)
    {
      // this is the second character, first was zero
      if (c == 'x')
        base = 16;
      else if (c == 'b')
        base = 2;
      else if (c >= '0' && c <= '9')
      {
        base = 10;
        val += c - '0';
      }
      else
        return false;
    }
    else
    {
      // this is the first character
      if (c == '-')
      {
        base = 10;
        flags |= 0x80;
      }
      else if (c >= '1' && c <= '9')
      {
        base = 10;
        val = c - '0';
      }
      else if (c != '0')
        return false;
    }
    c = *(str++);
    flags |= 1; // not first
  }
  if (!(flags & 1))
    return false;
  *result = flags & 0x80 ? -val : val;
  return true;
}

ArgumentReader::ArgumentReader(char separator)
{
  separator_ = separator;
}

void ArgumentReader::begin(byte *cmdlinebuf)
{
  cmdptr_ = cmdlinebuf;
}

int8_t ArgumentReader::readLong(int32_t *arg, int32_t min, int32_t max)
{
  char *longstr;
  int8_t len = readString(&longstr, false, separator_);
  if (!len)
    return 0;
  long L;
  if (!atol(longstr, &L))
    return -len;
  if (L < min || L > max)
    return -len;
  *arg = L;
  return len;
}

int8_t ArgumentReader::readInt(int16_t *arg, int16_t min, int16_t max)
{
  char *intstr;
  int8_t len = readString(&intstr, false, separator_);
  if (!len)
    return 0;
  long L;
  if (!atol(intstr, &L))
    return -len;
  int I = L;
  if (I < min || I > max)
    return -len;
  *arg = I;
  return len;
}

int8_t ArgumentReader::readEnum(uint8_t *arg, PGM_P options, char delimiter)
{
  char *enumstr;
  int8_t len = readString(&enumstr, false, separator_);
  if (!len)
    return 0;
  PGM_P start = options;
  PGM_P end = start;
  uint8_t index = 0;
  char c = pgm_read_byte(end);
  while (c)
  {
    if (c == delimiter)
    {
      int slen = end - start;
      if (len == slen && !strncasecmp_P(enumstr, start, slen))
      {
        *arg = index;
        return len;
      }
      start = end + 1;
      index++;
    }
    c = pgm_read_byte(++end);
  }
  if (strcasecmp_P(enumstr, start))
    return -len;
  *arg = index;
  return len;
}

void ArgumentReader::printEnum(Print &output, uint8_t value, PGM_P options, char delimiter)
{
  PGM_P end = options;
  uint8_t index = 0;
  char c = pgm_read_byte(end);
  while (c)
  {
    if (c == delimiter)
      index++;
    else if (index == value)
      output.write(c);
    c = pgm_read_byte(++end);
  }
}

// increments cmdptr so that points to null-terminator if no separators
// if there are separators, increments until space, makes all consecutive separators null, finally points non-space char (it might be null)
// returns length without separators
int16_t ArgumentReader::readString(char **arg, bool uppercase, char separator)
{
  *arg = (char *)cmdptr_;
  int len = 0;
  while (*cmdptr_)
  {
    if (*cmdptr_ == separator)
    {
      while (*cmdptr_ == separator)
      {
        *cmdptr_ = '\0';
        cmdptr_++;
      }
      break;
    }
    len++;
    if (uppercase && *cmdptr_ >= 'a' && *cmdptr_ <= 'z')
      *cmdptr_ = *cmdptr_ & ~0x20;
    cmdptr_++;
  }
  return len;
}

int16_t ArgumentReader::readString(char **arg, bool uppercase)
{
  return readString(arg, uppercase, separator_);
}

int16_t ArgumentReader::readToEnd(char **arg, bool uppercase)
{
  return readString(arg, uppercase, 0);
}

char *ArgumentReader::peek()
{
  return (char *)cmdptr_;
}
