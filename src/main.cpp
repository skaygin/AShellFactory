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
#include "Arduino.h"
#include <Shell.h>
#include <ShellCmdRepository.h>

COMMAND_HANDLER(VER, request, response, "Displays firmware version.")
{
    response.print(F("Tester Version 1.0"));
    return 0;
}

DECLARE_SHELL_COMMANDS(user_commands){
    SHELL_COMMAND(VER),
    SHELL_COMMAND(PIN),
    SHELL_COMMAND(APIN),
    SHELL_COMMAND(EEREAD),
    SHELL_COMMAND(EEWRITE),
    SHELL_COMMAND(HELP),
    END_SHELL_COMMANDS};

void setup()
{
    Serial.begin(9600);
    Shell.begin(user_commands);
    Shell.addEndpoint(Serial);
    Shell.exec(F("VER"), Serial);
}

void loop()
{
    Shell.tick();
}

#ifdef ENV_NATIVE
#include <stdio.h>
class StdioPrint : public Print
{
public:
    virtual size_t write(uint8_t c)
    {
        putchar(c);
        return 1;
    }

    /* default implementation: may be overridden */
    size_t write(const uint8_t *buffer, size_t size)
    {
        size_t n = 0;
        while (size--)
        {
            if (write(*buffer++))
                n++;
            else
                break;
        }
        return n;
    }
};

char *cmdptr_;

void try_charptr(const char *cmdline)
{
    cmdptr_ = (char *)cmdline;
}

StdioPrint stdprint;

int main()
{
    try_charptr("hello world");
    stdprint.write('e');
    stdprint.write((uint8_t *)"abc", 3);
    putchar('h');
    // stdprint.print("xyz"); //this doesn't work because Print is from ArduinoFake and functions are all zero
    putchar(13);
    putchar(10);
    // printf("hello world\r\n");
    Shell.begin(user_commands);
    //   Shell.addEndpoint(stdout);
    Shell.exec(F("VER"), stdprint);
    printf("exiting\r\n");
    return 0;
}

#endif
