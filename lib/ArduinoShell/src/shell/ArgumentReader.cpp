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
#include "Int32Parser.h"

ArgumentReader::ArgumentReader(char separator)
{
  separator_ = separator;
}

void ArgumentReader::begin(byte *cmdlinebuf)
{
  cmdptr_ = cmdlinebuf;
}

int8_t ArgumentReader::readDecimal(uint8_t decimal_places, int32_t *arg, int32_t min, int32_t max)
{
  char *vstr;
  int8_t len = readString(&vstr, false);
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

int8_t ArgumentReader::readInt_(void *arg, uint8_t byte_count, int32_t min, int32_t max)
{
  char *vstr;
  int8_t len = readString(&vstr, false);
  if (!len)
    return 0;
  int32_t lv;
  if (!parseInt32(vstr, &lv))
    return -len;
  if (lv < min || lv > max)
    return -len;
  if (byte_count & 1)
    (*(uint8_t *)arg) = lv;
  else if (byte_count & 2)
    (*(uint16_t *)arg) = lv;
  else if (byte_count & 4)
    (*(uint32_t *)arg) = lv;
  return len;
}

int8_t ArgumentReader::readInt(uint32_t *arg, uint32_t min, uint32_t max)
{
  char *vstr;
  int8_t len = readString(&vstr, false);
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

int8_t ArgumentReader::readInt(int32_t *arg, int32_t min, int32_t max)
{
  return readInt_(arg, 4, min, max);
}

int8_t ArgumentReader::readInt(uint16_t *arg, uint16_t min, uint16_t max)
{
  return readInt_(arg, 2, min, max);
}

int8_t ArgumentReader::readInt(int16_t *arg, int16_t min, int16_t max)
{
  return readInt_(arg, 2, min, max);
}

int8_t ArgumentReader::readInt(uint8_t *arg, uint8_t min, uint8_t max)
{
  return readInt_(arg, 1, min, max);
}

int8_t ArgumentReader::readInt(int8_t *arg, int8_t min, int8_t max)
{
  return readInt_(arg, 1, min, max);
}

int8_t ArgumentReader::readHex(uint32_t *arg, uint32_t min, uint32_t max)
{
  char *hexstr;
  int8_t len = readString(&hexstr, false);
  if (!len)
    return 0;
  uint32_t ul;
  if (!parseUInt32(hexstr, &ul, 16))
    return -len;
  if (ul < min || ul > max)
    return -len;
  return len;
}

int8_t ArgumentReader::readEnum(uint8_t *arg, PGM_P options, char delimiter)
{
  char *enumstr;
  int8_t len = readString(&enumstr, false);
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
int ArgumentReader::readString(char **arg, bool uppercase, char separator)
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
  // most of other read functions assume max 127 length, except for readToEnd and readString
  // ensure maxlen is 127 to fit into uint8_t
  // return len > 127 ? 127 : len;
}

int ArgumentReader::readString(char **arg, bool uppercase)
{
  return readString(arg, uppercase, separator_);
}

int ArgumentReader::readToEnd(char **arg, bool uppercase)
{
  return readString(arg, uppercase, 0);
}

char *ArgumentReader::peek()
{
  return (char *)cmdptr_;
}
