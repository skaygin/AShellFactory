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
#include <Arduino.h>

#include <Shell.h>
#include <ShellCmd.h>

DECLARE_COMMAND_HANDLER(LOGIN, "Logs in and enables user commands.");
DECLARE_COMMAND_HANDLER(LOGOUT, "Logs out and disables user commands.");

COMMAND_HANDLER(VER, request, response, "Displays firmware version.")
{
    response.print(F("Tester Version 1.0"));
    return 0;
}

DECLARE_SHELL_COMMANDS(user_commands){
    SHELL_COMMAND(VER),
    SHELL_COMMAND(PIN),
    SHELL_COMMAND(APIN),
    // SHELL_COMMAND(EEREAD),
    // SHELL_COMMAND(EEWRITE),
    SHELL_COMMAND(FREEMEM),
    SHELL_COMMAND(RESET),
    SHELL_COMMAND(LOGOUT),
    SHELL_COMMAND(HELP),
    END_SHELL_COMMANDS};

DECLARE_SHELL_COMMANDS(login_commands){
    SHELL_COMMAND(LOGIN),
    SHELL_COMMAND(HELP),
    END_SHELL_COMMANDS};

DECLARE_SHELL_COMMANDS(admin_commands){
    SHELL_COMMAND(EEREAD),
    SHELL_COMMAND(EEWRITE),
    END_SHELL_COMMANDS};

IMPLEMENT_COMMAND_HANDLER(LOGIN, request, response)
{
    Shell.setUserCommands(user_commands);
    Shell.setAdminCommands(admin_commands);
    response.print(F("User commands are available."));
    return 0;
}

IMPLEMENT_COMMAND_HANDLER(LOGOUT, request, response)
{
    Shell.setUserCommands(login_commands);
    Shell.setAdminCommands(0);
    response.print(F("Logged out."));
    return 0;
}

void setup()
{
    Serial.begin(9600);
#ifndef ENV_NATIVE
    while (!Serial)
        ; // wait for serial port to connect. Needed for native USB
#endif
    Shell.begin(login_commands, F(">>"));
    Shell.addEndpoint(Serial);
    Shell.removeEndpoint(Serial);
    Shell.addEndpoint(Serial);
    Shell.exec(F("VER"), Serial);
}

void loop()
{
    Shell.tick();
}

int main()
{
    setup();
    loop();
    return 0;
}
