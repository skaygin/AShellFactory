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

// local impl. of atol like functions saves program storage by not including unnecessary libs and improves functionality

bool baseToUInt32_(const char *str, uint32_t *result, int base, uint8_t fraction_places)
{
  if (base < 2 || base > 16)
    return false;
  char cur = *(str++);
  if (!cur)
    return false;
  if (cur == '.' && !*str)
    return false;
  uint8_t fracremain = fraction_places;
  bool frac = false;
  uint32_t val = 0;
  while (cur || fracremain)
  {
    if (!cur && fraction_places)
      frac = true;
    char c = cur ? cur : '0';
    if (c == '.')
    {
      if (frac || !fraction_places)
        return false;
      frac = true;
    }
    else if (c < '0')
      return false;
    else
    {
      if (frac)
        fracremain--;
      uint32_t oldval = val;
      uint8_t inc;
      val *= base;
      if ((val / base) != oldval)
        return false; // overflow
      oldval = val;
      inc = 0;
      char C = c & ~0x20; // to uppercase for hex
      if (c <= '9')
      {
        inc = c - '0';
        if (base < (inc + 1))
          return false;
      }
      else if (C >= 'A' && C <= 'F')
      {
        inc = C - ('A' - 10);
        if (base < (inc + 1))
          return false;
      }
      else
        return false;
      val += inc;
      if (val < oldval)
        return false; // overflow
    }
    if (cur)
      cur = *(str++);
  }
  *result = val;
  return true;
}

// Uses python convention, sign not allowed here
bool parseUInt32_(const char *str, uint32_t *result)
{
  char c = *(str++);
  int base = 10;
  if (!c)
    return false;
  if (c == '0')
  {
    c = *(str++) & ~0x20; // to uppercase, zero remains
    if (!c)
    {
      str -= 2;
    }
    else if (c == 'B')
    {
      base = 2;
    }
    else if (c == 'X')
    {
      base = 16;
    }
    else if (c == 'O')
    {
      base = 8;
    }
    else
    {
      // base = 10;
      str--;
    }
  }
  else
  {
    // base = 10;
    str--;
  }
  return baseToUInt32_(str, result, base, 0);
}

bool ArgumentReader::strToUInt32(const char *str, uint32_t *result, int base, uint8_t fraction_places)
{
  return baseToUInt32_(str, result, base, fraction_places);
}

bool ArgumentReader::parseUInt32(const char *str, uint32_t *result)
{
  if (*str == '+')
  {
    str++;
    return baseToUInt32_(str, result, DEC, 0);
  }
  return parseUInt32_(str, result);
}

bool parseInt32_(const char *str, int32_t *result, uint8_t decimal_places)
{
  char c = *str;
  bool isneg = false;
  uint32_t ul;
  bool parsed;
  if (c == '+' || c == '-')
  {
    str++;
    isneg = (c == '-');
    parsed = baseToUInt32_(str, &ul, DEC, decimal_places);
  }
  else
  {
    if (decimal_places)
      parsed = baseToUInt32_(str, &ul, DEC, decimal_places);
    else
      parsed = parseUInt32_(str, &ul);
  }
  if (!parsed)
    return false;
  if (isneg)
  {
    if (ul > 0x80000000)
      return false;
    int32_t lv = ul - 1;
    lv = -lv - 1;
    *result = lv;
  }
  else
  {
    if (ul > 0x7fffffff)
      return false;
    *result = ul;
  }
  return true;
}

bool ArgumentReader::parseInt32(const char *str, int32_t *result)
{
  return parseInt32_(str, result, 0);
}

bool ArgumentReader::parseDecimal32(const char *str, int32_t *result, uint8_t decimal_places)
{
  return parseInt32_(str, result, decimal_places);
}

ArgumentReader::ArgumentReader(char separator)
{
  separator_ = separator;
}

void ArgumentReader::begin(byte *cmdlinebuf)
{
  cmdptr_ = cmdlinebuf;
}

int8_t ArgumentReader::readDecimal32(int32_t *arg, uint8_t decimal_places, int32_t min, int32_t max)
{
  char *vstr;
  int8_t len = readString(&vstr, false, separator_);
  if (!len)
    return 0;
  int32_t lv;
  if (!parseDecimal32(vstr, &lv, decimal_places))
    return -len;
  if (lv < min || lv > max)
    return -len;
  *arg = lv;
  return len;
}

int8_t ArgumentReader::readUInt32(uint32_t *arg, uint32_t min, uint32_t max)
{
  char *vstr;
  int8_t len = readString(&vstr, false, separator_);
  if (!len)
    return 0;
  uint32_t ul;
  if (!parseUInt32(vstr, &ul))
    return -len;
  if (ul < min || ul > max)
    return -len;
  *arg = ul;
  return len;
}

int8_t ArgumentReader::readInt32(int32_t *arg, int32_t min, int32_t max)
{
  char *vstr;
  int8_t len = readString(&vstr, false, separator_);
  if (!len)
    return 0;
  int32_t lv;
  if (!parseInt32(vstr, &lv))
    return -len;
  if (lv < min || lv > max)
    return -len;
  *arg = lv;
  return len;
}

int8_t ArgumentReader::readInt16(int16_t *arg, int16_t min, int16_t max)
{
  char *intstr;
  int8_t len = readString(&intstr, false, separator_);
  if (!len)
    return 0;
  int32_t lv;
  if (!parseInt32(intstr, &lv))
    return -len;
  int I = lv;
  if (I < min || I > max)
    return -len;
  *arg = I;
  return len;
}

int8_t ArgumentReader::readHex(uint32_t *arg, uint32_t min, uint32_t max)
{
  char *hexstr;
  int8_t len = readString(&hexstr, false, separator_);
  if (!len)
    return 0;
  uint32_t ul;
  if (!strToUInt32(hexstr, &ul, HEX))
    return -len;
  if (ul < min || ul > max)
    return -len;
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
  char c = pgm_read_byte_near(end);
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
    c = pgm_read_byte_near(++end);
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
  char c = pgm_read_byte_near(end);
  while (c)
  {
    if (c == delimiter)
      index++;
    else if (index == value)
      output.write(c);
    c = pgm_read_byte_near(++end);
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
