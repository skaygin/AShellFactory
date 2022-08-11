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
#include "ShellCmdPIN.h"

IMPLEMENT_COMMAND_HANDLER(PIN, request, response)
{
    int16_t pin;
    if (!request.readInt16(&pin))
        return SHELL_RESPONSE_ERR_BAD_ARGUMENT;
    uint8_t value;
    PGM_P options = PSTR("LOW|HIGH|INPUT|OUTPUT|PULLUP");
    int8_t ret = request.readEnum(&value, options);
    if (ret < 0)
        return SHELL_RESPONSE_ERR_BAD_ARGUMENT;
    if (ret)
    {
        if (value < 2) // low,high
            digitalWrite(pin, value);
        else // input,output,pullup
            pinMode(pin, value - 2);
    }
    else
        value = digitalRead(pin);
    response.print(pin);
    response.write(' ');
    ArgumentReader::printEnum(response, value, options);
    return 0;
}

IMPLEMENT_COMMAND_HANDLER(APIN, request, response)
{
    int16_t pin;
    if (!request.readInt16(&pin))
        return SHELL_RESPONSE_ERR_BAD_ARGUMENT;
    int16_t value;
    if (!request.readInt16(&value))
        value = analogRead(pin);
    else
        analogWrite(pin, value);
    response.print(pin);
    response.write(' ');
    response.print(value);
    return 0;
}
