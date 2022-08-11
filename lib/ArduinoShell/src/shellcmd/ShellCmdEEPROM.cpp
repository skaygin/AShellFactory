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
#include "ShellCmdEEPROM.h"
#include <EEPROM.h>

IMPLEMENT_COMMAND_HANDLER(EEREAD, request, response)
{
    int16_t address;
    if (request.readInt16(&address) <= 0)
        return SHELL_RESPONSE_ERR_BAD_ARGUMENT;
    if (address < 0)
        return SHELL_RESPONSE_ERR_BAD_ARGUMENT;
    int16_t count = 256;
    request.readInt16(&count);
    if (count <= 0 || count > 1024)
        count = 1024;
    while (count)
    {
        int value = EEPROM.read(address);
        if (value < 16)
            response.write('0');
        response.print(value, HEX);
        count--;
        address++;
    }
    return 0;
}

int8_t hexupcase2int(char c)
{
    if (c >= 'A')
        return (c - ('A' - 10)) & 0x0f;
    return (c - '0') & 0x0f;
}

IMPLEMENT_COMMAND_HANDLER(EEWRITE, request, response)
{
    int16_t address;
    if (request.readInt16(&address) <= 0)
        return SHELL_RESPONSE_ERR_BAD_ARGUMENT;
    if (address < 0)
        return SHELL_RESPONSE_ERR_BAD_ARGUMENT;
    char *hexstr;
    while (request.readString(&hexstr, true))
    {
        while (*hexstr)
        {
            int8_t value = hexupcase2int(*(hexstr++));
            if (*hexstr)
                value = (value << 4) | hexupcase2int(*(hexstr++));
            EEPROM.write(address, value);
            address++;
        }
    }
    return 0;
}
