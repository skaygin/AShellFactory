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
#include "Int32Parser.h"

// fraction_places -128 means no dots expected, otherwise dots are allowed, e.g. 1.0 1. or hex A. A.0
bool exactBaseToUInt32_(const char *str, uint32_t *out, uint8_t base, int8_t fraction_places = -128)
{
    if (fraction_places == -128)
        fraction_places = 0;
    if (base < 2 || base > 36)
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
                return false; // already in fraction part or fraction not allowed
            frac = true;
        }
        else if (c < '0')
            return false; // illegal chars including 0x20
        else
        {
            uint8_t inc = 0;
            char C = c & ~0x20; // to uppercase for hex
            if (c <= '9')
            {
                inc = c - '0';
                if (base < (inc + 1))
                    return false;
            }
            else if (C >= 'A' && C <= 'Z')
            {
                inc = C - ('A' - 10);
                if (base < (inc + 1))
                    return false;
            }
            else
                return false;
            if (!frac || fracremain) // otherwise truncated (ignored)
            {
                uint32_t oldval = val;
                val *= base;
                if ((val / base) != oldval)
                    return false; // overflow
                oldval = val;
                val += inc;
                if (val < oldval)
                    return false; // overflow
            }
            if (frac && fracremain)
                fracremain--;
        }
        if (cur)
            cur = *(str++);
    }
    *out = val;
    return true;
}

// Uses python convention, sign not allowed here
bool parseUInt32_(const char *str, uint32_t *out, uint8_t base = 0)
{
    int b = 0;
    if (*str == '0')
    {
        char c2 = *(str + 1) & ~0x20; // to uppercase, zero remains
        if (c2 == 'X')
            b = 16;
        else if (c2 == 'B')
            b = 2;
        else if (c2 == 'O')
            b = 8;
    }
    if (!b)
        b = 10;
    else
        str += 2;
    if (!base)
        base = b;
    else if (base != b)
        return false;
    return exactBaseToUInt32_(str, out, base);
}

bool parseInt32_(const char *str, int32_t *out, uint8_t base, int8_t decimal_places = -128)
{
    char c = *str;
    bool isneg = false;
    uint32_t ul;
    bool parsed;
    if (c == '+' || c == '-')
    {
        // sign implies decimal
        str++;
        isneg = (c == '-');
        if (base && base != 10)
            return false;
        parsed = exactBaseToUInt32_(str, &ul, DEC, decimal_places);
    }
    else
    {
        if (decimal_places != -128)
        {
            if (base && base != 10)
                return false;
            parsed = exactBaseToUInt32_(str, &ul, 10, decimal_places);
        }
        else
            parsed = parseUInt32_(str, &ul, base);
    }
    if (!parsed)
        return false;
    if (isneg)
    {
        if (ul > 0x80000000)
            return false;
        int32_t lv = ul - 1;
        lv = -lv - 1;
        *out = lv;
    }
    else
    {
        if (ul > 0x7fffffff)
            return false;
        *out = ul;
    }
    return true;
}

bool strToUInt32(const char *str, uint32_t *out, uint8_t base, int8_t fraction_places)
{
    return exactBaseToUInt32_(str, out, base, fraction_places);
}

bool parseUInt32(const char *literal, uint32_t *out, uint8_t base)
{
    char c = *literal;
    if (c == '+' || c == '-')
    {
        literal++;
        uint32_t v;
        if (base && base != 10)
            return false;
        if (!exactBaseToUInt32_(literal, &v, 10))
            return false;
        if (c == '-' && v)
            return false;
        *out = v;
        return true;
    }
    return parseUInt32_(literal, out, base);
}

bool parseInt32(const char *literal, int32_t *out, uint8_t base)
{
    return parseInt32_(literal, out, base);
}

bool parseDecimal32(const char *literal, int32_t *out, uint8_t decimal_places)
{
    if (decimal_places > 127)
        decimal_places = 127;
    return parseInt32_(literal, out, 10, decimal_places);
}
