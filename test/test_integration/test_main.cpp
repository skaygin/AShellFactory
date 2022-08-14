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
#include <EEPROM.h>
#include <unity.h>
#include <Shell.h>
#include "../TesterStream.h"

TesterStream tester;
TesterStream tester2;
ArgumentReader arg;

// pio ci src/main.cpp  --lib="./lib/ArduinoShell/src" --board=nanoatmega168 --board=uno --board=megaatmega2560 --board=leonardo

#define handler(C, HSTR) COMMAND_HANDLER(C, request, response, HSTR)

handler(VER, "Displays firmware version.")
{
    response.print(F("Tester Version 1.0"));
    return 0;
}

handler(TEST, "Used for unit testing. <month>")
{
    int16_t month;
    int8_t res = request.readInt(&month, 1, 12);
    if (res <= 0)
        return SHELL_RESPONSE_ERR_BAD_ARGUMENT;
    char *str = request.peek();
    response.print(F("test executed with params:"));
    response.print(month);
    response.write(',');
    response.print(str);
    return 0;
}

// dot at the end is deliberately missing
handler(A, "Short command no dot")
{
    int16_t code;
    if (!request.readInt(&code))
        return 0;
    return code;
}

handler(WHO, "Displays name of the requesting endpoint.")
{
    ShellController *ctx = ShellController::context();
    Stream *ep = ctx->getRequestingEndpoint();
    if (ep == &Serial)
        response.print(F("Serial"));
#ifndef MEMORY_USAGE
    if (ep == &tester)
        response.print(F("Tester"));
    if (ep == &tester2)
        response.print(F("Tester2"));
#endif
    return 0;
}

/*
DECLARE_SHELL_COMMANDS(user_commands){
        SHELL_COMMAND(VER),
        SHELL_COMMAND(TEST),
        SHELL_COMMAND(A),
        SHELL_COMMAND(RESET),
        SHELL_COMMAND(MEM),
        SHELL_COMMAND(WHO),
        SHELL_COMMAND(DPIN),
        SHELL_COMMAND(APIN),
        SHELL_COMMAND(HELP),
        END_SHELL_COMMANDS
};

DECLARE_SHELL_COMMANDS(admin_commands){
        SHELL_COMMAND(EEREAD),
        SHELL_COMMAND(EEWRITE),
        SHELL_COMMAND(FRAMING),
        END_SHELL_COMMANDS
};
*/

DECLARE_SHELL_COMMANDS(user_commands){
    SHELL_COMMAND(VER),
    SHELL_COMMAND(TEST),
    SHELL_COMMAND(A),
    SHELL_COMMAND(WHO),
    SHELL_COMMAND(HELP),
    END_SHELL_COMMANDS};

void test_ver_command(void)
{
    tester.execute(F("VER\r"));
    TEST_ASSERT_EQUAL_STRING(("Tester Version 1.0\r\n~"), tester.response());
}

void test_all_commands()
{
    tester2.execute(F("VER\r"));
    TEST_ASSERT_EQUAL_STRING("Tester Version 1.0\r\n~", tester2.response());
    tester.execute(F("VER\r"));
    TEST_ASSERT_EQUAL_STRING(tester.response(), ("Tester Version 1.0\r\n~"));
    tester2.execute(F("WHO\r"));
    TEST_ASSERT_EQUAL_STRING(tester2.response(), ("Tester2\r\n~"));
    tester.execute(F("WHO\r"));
    TEST_ASSERT_EQUAL_STRING(tester.response(), ("Tester\r\n~"));
}

void test_run_externally()
{
    TEST_ASSERT_EQUAL_INT8(0, tester2.call(F("VER")));
    TEST_ASSERT_EQUAL_STRING(tester2.response(), ("Tester Version 1.0"));
    TEST_ASSERT_EQUAL_INT8(0, tester.call(F("VER")));
    TEST_ASSERT_EQUAL_STRING(tester.response(), ("Tester Version 1.0"));
}

void test_help_command()
{
    /*
    tester.input(F("HE"));
    Shell.tick(false);
    tester.execute(F("LP\r"));
    TEST_ASSERT_EQUAL_STRING(tester.response(), ("VER       Displays firmware version.\r\nTEST      Used for unit testing.\r\nA         Short command no dot\r\nRESET     Soft-resets the device.\r\nMEM       Displays free memory.\r\nWHO       Displays name of the requesting endpoint.\r\nDPIN      Performs R/W operation or configures digital pin.\r\nAPIN      Performs R/W on analog pin.\r\nHELP      Provides Help information for commands.\r\n\r\nFor more information on commands use HELP <cmd>.\r\n\r\n~"));
    tester.execute(F("HELP  -A \r"));
    TEST_ASSERT_EQUAL_STRING(tester.response(), ("EEREAD    Reads bytes from EEPROM.\r\nEEWRITE   Writes bytes to EEPROM.\r\nFRAMING   Changes framing mode.\r\n\r\nFor more information on commands use HELP <cmd>.\r\n\r\n~"));
    tester.execute(F("HELP TEST\r"));
    TEST_ASSERT_EQUAL_STRING(tester.response(), ("Used for unit testing.\r\nTEST <month>\r\n~"));
    tester.execute(F("HELP  HeLP\r"));
    TEST_ASSERT_EQUAL_STRING(tester.response(), ("Provides Help information for commands.\r\nHELP [-a] [<cmd>]\r\n~"));
    tester.execute(F("HELP eeread\r"));
    TEST_ASSERT_EQUAL_STRING(tester.response(), ("Unknown command\r\n~"));
    tester.execute(F("HELP -a eeread\r"));
    TEST_ASSERT_EQUAL_STRING(tester.response(), ("Reads bytes from EEPROM.\r\nEEREAD <addr> <count>\r\n~"));
    tester.execute(F("HELP -a HELP\r"));
    TEST_ASSERT_EQUAL_STRING(tester.response(), ("Unknown command\r\n~"));
    */
}

void test_user_commands()
{
    tester.execute(F("\r"));
    TEST_ASSERT_EQUAL_STRING(tester.response(), ("\r\n~"));
    tester.execute(F("A\r"));
    TEST_ASSERT_EQUAL_STRING(tester.response(), ("\r\n~"));
    tester.execute(F("TEST\r"));
    TEST_ASSERT_EQUAL_STRING(tester.response(), ("ERR:Bad or missing argument\r\n~"));
    tester.execute(F("tESt 1\r"));
    TEST_ASSERT_EQUAL_STRING(tester.response(), ("test executed with params:1,\r\n~"));
    tester.execute(F("TEST   5   A  B\r"));
    TEST_ASSERT_EQUAL_STRING(tester.response(), ("test executed with params:5,A  B\r\n~"));
}

void test_multiple_consoles()
{
    tester.input(F("V"));
    tester2.execute(F("ER\r"));
    TEST_ASSERT_EQUAL_STRING(tester2.response(), ("Tester Version 1.0\r\n~"));
    TEST_ASSERT_EQUAL_STRING(Shell.getRequestingEndpoint(), 0);
    tester.execute(F("VER\r"));
    TEST_ASSERT_EQUAL_STRING(tester.response(), ("Tester Version 1.0\r\n~"));
}

void test_break_down_tick()
{
    tester.execute(F("TeST 8\r"), false);
    char *str = Shell.available(true);
    TEST_ASSERT_EQUAL_STRING(str, ("TeST 8"));
    Shell.exec((byte *)str, tester);
    TEST_ASSERT_EQUAL_STRING(tester.response(), ("test executed with params:8,\r\n~"));
    // lower level
    byte cmd[] = "TeST 7";
    arg.begin(&cmd[0]);
    char *c;
    arg.readString(&c);
    // CommandMapping *def = Shell.findCommandDefinition(c); // todo
    CommandHandlerFunc func = Shell.findCommandFunction(c);
    TEST_ASSERT_EQUAL_INT8(0, func(arg, tester));
    TEST_ASSERT_EQUAL_STRING(tester.response(), ("test executed with params:7,"));
}

void test_external_executor()
{
    byte cmd[] = "TEsT";
    CommandHandlerFunc func = Shell.findCommandFunction((char *)&cmd[0]);
    byte cmdarg[] = "9";
    arg.begin(&cmdarg[0]);
    func(arg, tester); //__run__TEST(pr);
    TEST_ASSERT_EQUAL_STRING(tester.response(), ("test executed with params:9,"));
    Shell.exec(F("test 11"), tester);
    TEST_ASSERT_EQUAL_STRING(tester.response(), ("test executed with params:11,\r\n~"));
    Shell.exec(F("test 12"), tester);
    TEST_ASSERT_EQUAL_STRING(tester.response(), ("test executed with params:12,\r\n~"));
    Shell.exec(F("UNKNOWN"), tester);
    TEST_ASSERT_EQUAL_STRING(tester.response(), ("ERR:Unknown command\r\n~"));
}

void test_command_errors()
{

    tester.execute(F("UNKNOWN\r"));
    TEST_ASSERT_EQUAL_STRING(tester.response(), ("ERR:Unknown command\r\n~"));
    tester.execute(F("ABCDEFGHI 0123456789012345678901234567890123456789012345678901234567890123456789\r")); // just fits
    TEST_ASSERT_EQUAL_STRING(tester.response(), ("ERR:Unknown command\r\n~"));
    tester.execute(F("ABCDEFGHI 0123456789012345678901234567890123456789012345678901234567890123456789x\r"));
    TEST_ASSERT_EQUAL_STRING(tester.response(), ("ERR:Command too long\r\n~"));
}

void test_error_messages()
{

    tester.execute(F("A\r"));
    TEST_ASSERT_EQUAL_STRING(tester.response(), ("\r\n~"));
    tester.execute(F("A 0\r"));
    TEST_ASSERT_EQUAL_STRING(tester.response(), ("\r\n~"));
    tester.execute(F("A 1\r"));
    TEST_ASSERT_EQUAL_STRING(tester.response(), ("ERR:Bad or missing argument\r\n~"));
    tester.execute(F("A 2\r"));
    TEST_ASSERT_EQUAL_STRING(tester.response(), ("ERR:Operation timed out\r\n~"));
    tester.execute(F("A 3\r"));
    TEST_ASSERT_EQUAL_STRING(tester.response(), ("ERR:Illegal state\r\n~"));
    tester.execute(F("A 4\r"));
    TEST_ASSERT_EQUAL_STRING(tester.response(), ("ERR:I/O error\r\n~"));
    tester.execute(F("A 5\r"));
    TEST_ASSERT_EQUAL_STRING(tester.response(), ("ERR:Communication fault\r\n~"));
    tester.execute(F("A 6\r"));
    TEST_ASSERT_EQUAL_STRING(tester.response(), ("ERR:Illegal operation\r\n~"));
    tester.execute(F("A 7\r"));
    TEST_ASSERT_EQUAL_STRING(tester.response(), ("ERR:Unknown error\r\n~"));
    tester.execute(F("A 123\r"));
    TEST_ASSERT_EQUAL_STRING(tester.response(), ("ERR:Unknown error\r\n~"));
    tester.execute(F("A -1\r"));
    TEST_ASSERT_EQUAL_STRING(tester.response(), ("ERR:Custom-Error-1\r\n~"));
    tester.execute(F("A -128\r"));
    TEST_ASSERT_EQUAL_STRING(tester.response(), ("ERR:Custom-Error-128\r\n~"));
}
/*
void test_frame_mode()
{
  tester.execute(F("FRAMING BIN\r"));
  assertEqual(tester.response(), F("BIN\r\n~"));
  tester.execute(F("\002BADFRAME\x03\x80\x80\r\n"));
  assertEqual(tester.response(), F("\002ERR:Bad frame\3\x9F\xD4\r\n"));
  tester.execute(F("\002FRAMING ASC\x03\xFB\xD4\r\n"));       //<STX>FRAMING ASC<ETX><xFB><xD4><CR><LF>
  assertEqual(tester.response(), F("\002ASC\3\xBF\xD7\r\n")); //<STX>ASC<ETX><xBF><xD7><CR><LF>
}
*/

void setUp(void)
{
    // set stuff up here
}

void tearDown(void)
{
    // clean stuff up here
}

int main(int argc, char **argv)
{
    // NOTE!!! Wait for >2 secs
    // if board doesn't support software reset via Serial.DTR/RTS
    // delay(2000);
    Serial.begin(115200);
    while (!Serial)
        ;
    Shell.begin(user_commands, F("~"));
    // Shell.addEndpoint(tester2);
    Shell.addEndpoint(tester);
    // Shell.removeEndpoint(tester2);
    Shell.addEndpoint(tester2);
    UNITY_BEGIN();
    RUN_TEST(test_ver_command);
    RUN_TEST(test_all_commands);
    RUN_TEST(test_run_externally);
    RUN_TEST(test_help_command);
    RUN_TEST(test_user_commands);
    RUN_TEST(test_multiple_consoles);
    RUN_TEST(test_break_down_tick);
    RUN_TEST(test_external_executor);
    // RUN_TEST(test_command_errors);
    //  RUN_TEST(test_error_messages);

    UNITY_END();
    Serial.flush();
    return 0;
}
