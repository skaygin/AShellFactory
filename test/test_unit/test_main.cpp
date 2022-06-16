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
#include <unity.h>
#include <TesterPrint.h>
#include <ShellFactory.h>

ArgumentReader arg;
TesterPrint testout;

#define handler(C, HSTR) COMMAND_HANDLER(C, request, response, HSTR)

handler(VER, "Displays firmware version.")
{
    response.write('K');
    return 0;
}

const CommandMapping PROGMEM user_commands[] =
    {
        MAP_HANDLER(VER),
        MAP_HANDLER(HELP),
        0 // must end with zero to indicate end of commands
};

void setUp(void)
{
    testout.clear();
}

void tearDown(void)
{
    // clean stuff up here
}

void test_read_line(void)
{
    byte cmdline[] = "txt 12  on";

    arg.begin(&cmdline[0]);
    // arg.begin("txt 12  on"); // native fails, mega causes next to fail??
    int8_t res;
    char *str;
    res = arg.readString(&str, true);
    TEST_ASSERT_EQUAL_INT8(3, res);
    TEST_ASSERT_EQUAL_STRING(str, ("TXT"));
    int16_t num;
    res = arg.readInt(&num, 1, 12);
    TEST_ASSERT_EQUAL_INT8(2, res);
    TEST_ASSERT_EQUAL_INT16(12, num);
    uint8_t enm;
    PGM_P options = PSTR("A|B|ON|OFF");
    res = arg.readEnum(&enm, options);
    TEST_ASSERT_EQUAL_INT8(2, res);
    TEST_ASSERT_EQUAL_UINT8(2, enm);

    arg.printEnum(testout, 0, options);
    TEST_ASSERT_EQUAL_STRING(testout.getPrinted(), ("A"));
    arg.printEnum(testout, 2, options);
    TEST_ASSERT_EQUAL_STRING(testout.getPrinted(), ("ON"));
    arg.printEnum(testout, 3, options);
    TEST_ASSERT_EQUAL_STRING(testout.getPrinted(), ("OFF"));
    arg.printEnum(testout, 4, options);
    TEST_ASSERT_EQUAL_STRING(testout.getPrinted(), (""));
}

void test_read_line_wrong_order(void)
{
    char *str;
    byte cmdline[] = "txt 12  on";
    arg.begin(&cmdline[0]);
    // arg.begin("txt 12  on"); // this fails with native, mega passes
    int16_t num = 99;
    int8_t res = arg.readInt(&num);
    TEST_ASSERT_EQUAL_INT8(-3, res);
    TEST_ASSERT_EQUAL_INT16(99, num);
    TEST_ASSERT_EQUAL_STRING(arg.peek(), ("12  on"));
    res = arg.readToEnd(&str, true);
    TEST_ASSERT_EQUAL_INT8(6, res);
    TEST_ASSERT_EQUAL_STRING(str, ("12  ON"));
}

void test_shell(void)
{
    // TEST_ASSERT_EQUAL_INT8(0, Shell.call("ver", testout));
    // Shell.run(F("VER"), testout);
}

int main(int argc, char **argv)
{
    Shell.begin(user_commands, '~');
    // Shell.addEndpoint(tester);
    UNITY_BEGIN();
    RUN_TEST(test_read_line);
    RUN_TEST(test_read_line_wrong_order);
    RUN_TEST(test_shell);
    UNITY_END();

    return 0;
}