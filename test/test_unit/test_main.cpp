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
#include <Shell.h>
#include "../TesterPrint.h"
#include <shell/Int32Parser.h>

ArgumentReader arg;
TesterPrint testout;

#define handler(C, HSTR) COMMAND_HANDLER(C, request, response, HSTR)

handler(VER, "Displays firmware version.")
{
    response.write('K');
    return 0;
}

DECLARE_SHELL_COMMANDS(user_commands){
    SHELL_COMMAND(VER),
    SHELL_COMMAND(HELP),
    END_SHELL_COMMANDS};

void setUp(void)
{
    testout.clear();
}

void tearDown(void)
{
    // clean stuff up here
}

void test_uint_conversion(void)
{
    uint32_t val = 999;
    TEST_ASSERT_FALSE(parsePositional32("", &val, DEC));
    TEST_ASSERT_FALSE(parsePositional32(" ", &val, DEC));
    TEST_ASSERT_FALSE(parsePositional32(".", &val, DEC));
    TEST_ASSERT_FALSE(parsePositional32("1.", &val, DEC));
    TEST_ASSERT_FALSE(parsePositional32(".1", &val, DEC));
    TEST_ASSERT_FALSE(parsePositional32("1 ", &val, DEC));
    TEST_ASSERT_FALSE(parsePositional32(" 1", &val, DEC));
    TEST_ASSERT_FALSE(parsePositional32("1B", &val, DEC));
    TEST_ASSERT_FALSE(parsePositional32("2", &val, BIN));
    TEST_ASSERT_FALSE(parsePositional32("8", &val, OCT));
    TEST_ASSERT_FALSE(parsePositional32(":", &val, DEC));
    TEST_ASSERT_FALSE(parsePositional32("A", &val, DEC));
    TEST_ASSERT_FALSE(parsePositional32("G", &val, HEX));
    TEST_ASSERT_FALSE(parsePositional32("0xAB", &val, HEX));
    TEST_ASSERT_FALSE(parsePositional32("0", &val, 0));
    TEST_ASSERT_FALSE(parsePositional32("0", &val, 1));
    TEST_ASSERT_FALSE(parsePositional32("0", &val, 37));
    // overflow
    TEST_ASSERT_FALSE(parsePositional32("100000000000000000000000000000000", &val, BIN));
    TEST_ASSERT_FALSE(parsePositional32("4294967296", &val, DEC));
    TEST_ASSERT_FALSE(parsePositional32("100000000", &val, HEX));
    // original value does not change when false
    TEST_ASSERT_EQUAL_UINT32(999, val);
    // minimum value
    TEST_ASSERT_TRUE(parsePositional32("0", &val, BIN));
    TEST_ASSERT_EQUAL_UINT32(0, val);
    TEST_ASSERT_TRUE(parsePositional32("0", &val, DEC));
    TEST_ASSERT_EQUAL_UINT32(0, val);
    TEST_ASSERT_TRUE(parsePositional32("0", &val, HEX));
    TEST_ASSERT_EQUAL_UINT32(0, val);
    // minimum nonzero
    TEST_ASSERT_TRUE(parsePositional32("1", &val, BIN));
    TEST_ASSERT_EQUAL_UINT32(1, val);
    TEST_ASSERT_TRUE(parsePositional32("1", &val, DEC));
    TEST_ASSERT_EQUAL_UINT32(1, val);
    TEST_ASSERT_TRUE(parsePositional32("1", &val, HEX));
    TEST_ASSERT_EQUAL_UINT32(1, val);
    // minimum len two
    TEST_ASSERT_TRUE(parsePositional32("10", &val, BIN));
    TEST_ASSERT_EQUAL_UINT32(2, val);
    TEST_ASSERT_TRUE(parsePositional32("10", &val, OCT));
    TEST_ASSERT_EQUAL_UINT32(8, val);
    TEST_ASSERT_TRUE(parsePositional32("10", &val, DEC));
    TEST_ASSERT_EQUAL_UINT32(10, val);
    TEST_ASSERT_TRUE(parsePositional32("10", &val, HEX));
    TEST_ASSERT_EQUAL_UINT32(16, val);
    TEST_ASSERT_TRUE(parsePositional32("10", &val, 36));
    TEST_ASSERT_EQUAL_UINT32(36, val);
    // maximum len one
    TEST_ASSERT_TRUE(parsePositional32("1", &val, BIN));
    TEST_ASSERT_EQUAL_UINT32(1, val);
    TEST_ASSERT_TRUE(parsePositional32("9", &val, DEC));
    TEST_ASSERT_EQUAL_UINT32(9, val);
    TEST_ASSERT_TRUE(parsePositional32("F", &val, HEX));
    TEST_ASSERT_EQUAL_UINT32(0xF, val);
    // maximum value
    TEST_ASSERT_TRUE(parsePositional32("11111111111111111111111111111111", &val, BIN));
    TEST_ASSERT_EQUAL_UINT32(0xffffffff, val);
    TEST_ASSERT_TRUE(parsePositional32("4294967295", &val, DEC));
    TEST_ASSERT_EQUAL_UINT32(0xffffffff, val);
    TEST_ASSERT_TRUE(parsePositional32("FFFFffff", &val, HEX));
    TEST_ASSERT_EQUAL_UINT32(0xffffffff, val);
    // nonstandard bases
    TEST_ASSERT_TRUE(parsePositional32("10", &val, 9));
    TEST_ASSERT_EQUAL_UINT32(9, val);
    TEST_ASSERT_TRUE(parsePositional32("10", &val, 11));
    TEST_ASSERT_EQUAL_UINT32(11, val);
    TEST_ASSERT_TRUE(parsePositional32("10", &val, 15));
    TEST_ASSERT_EQUAL_UINT32(15, val);
    TEST_ASSERT_TRUE(parsePositional32("111", &val, 3));
    TEST_ASSERT_EQUAL_UINT32(9 + 3 + 1, val);
    // misc cases
    TEST_ASSERT_TRUE(parsePositional32("012", &val, 10));
    TEST_ASSERT_EQUAL_UINT32(12, val);
}

void test_fraction_conversion(void)
{
    uint32_t val = 999;
    TEST_ASSERT_FALSE(parsePositional32("", &val, DEC, 2));
    TEST_ASSERT_FALSE(parsePositional32(".", &val, DEC, 2));
    TEST_ASSERT_FALSE(parsePositional32("1..", &val, DEC, 2));
    TEST_ASSERT_FALSE(parsePositional32("1..", &val, DEC, 2));
    TEST_ASSERT_FALSE(parsePositional32("12.3x", &val, DEC, 1));
    TEST_ASSERT_FALSE(parsePositional32("12.3 ", &val, DEC, 1));
    //  original value does not change when false
    TEST_ASSERT_EQUAL_UINT32(999, val);

    TEST_ASSERT_TRUE(parsePositional32("1", &val, DEC, 2));
    TEST_ASSERT_EQUAL_UINT32(100, val);
    TEST_ASSERT_TRUE(parsePositional32("1.", &val, DEC, 2));
    TEST_ASSERT_EQUAL_UINT32(100, val);
    TEST_ASSERT_TRUE(parsePositional32(".1", &val, DEC, 2));
    TEST_ASSERT_EQUAL_UINT32(10, val);
    TEST_ASSERT_TRUE(parsePositional32("12.3", &val, DEC, 1));
    TEST_ASSERT_EQUAL_UINT32(123, val);
    TEST_ASSERT_TRUE(parsePositional32("12.34", &val, DEC, 2));
    TEST_ASSERT_EQUAL_UINT32(1234, val);
    TEST_ASSERT_TRUE(parsePositional32("12.39", &val, DEC, 1));
    TEST_ASSERT_EQUAL_UINT32(123, val);
    TEST_ASSERT_TRUE(parsePositional32("11.11", &val, BIN, 2));
    TEST_ASSERT_EQUAL_UINT32(0b1111, val);
    TEST_ASSERT_TRUE(parsePositional32("AB.CD", &val, HEX, 2));
    TEST_ASSERT_EQUAL_UINT32(0xABCD, val);
    TEST_ASSERT_TRUE(parsePositional32("12.3", &val, DEC, 2));
    TEST_ASSERT_EQUAL_UINT32(1230, val);
    TEST_ASSERT_TRUE(parsePositional32("11.1", &val, BIN, 2));
    TEST_ASSERT_EQUAL_UINT32(0b1110, val);
}

void test_parse_uint32(void)
{
    uint32_t val = 999;
    TEST_ASSERT_FALSE(parseUInt32("", &val));
    TEST_ASSERT_FALSE(parseUInt32("AB", &val));
    TEST_ASSERT_FALSE(parseUInt32("AB", &val));
    TEST_ASSERT_FALSE(parseUInt32("0o78", &val));
    TEST_ASSERT_FALSE(parseUInt32("0b12", &val));
    TEST_ASSERT_FALSE(parseUInt32("12A", &val));
    TEST_ASSERT_FALSE(parseUInt32("+", &val));
    TEST_ASSERT_FALSE(parseUInt32("++", &val));
    TEST_ASSERT_FALSE(parseUInt32("++1", &val));
    TEST_ASSERT_FALSE(parseUInt32("-", &val));
    TEST_ASSERT_FALSE(parseUInt32("-1", &val));
    TEST_ASSERT_FALSE(parseUInt32("x1a", &val));
    TEST_ASSERT_FALSE(parseUInt32("0x", &val));
    TEST_ASSERT_FALSE(parseUInt32("1.", &val));
    TEST_ASSERT_FALSE(parseUInt32("0x1234", &val, 10));
    TEST_ASSERT_FALSE(parseUInt32("1.", &val, 10));
    TEST_ASSERT_EQUAL_UINT32(999, val);

    TEST_ASSERT_TRUE(parseUInt32("0", &val));
    TEST_ASSERT_EQUAL_UINT32(0, val);
    TEST_ASSERT_TRUE(parseUInt32("02", &val));
    TEST_ASSERT_EQUAL_UINT32(2, val);
    TEST_ASSERT_TRUE(parseUInt32("-0", &val));
    TEST_ASSERT_EQUAL_UINT32(0, val);
    TEST_ASSERT_TRUE(parseUInt32("3", &val));
    TEST_ASSERT_EQUAL_UINT32(3, val);
    TEST_ASSERT_TRUE(parseUInt32("+0", &val));
    TEST_ASSERT_EQUAL_UINT32(0, val);
    TEST_ASSERT_TRUE(parseUInt32("+1", &val));
    TEST_ASSERT_EQUAL_UINT32(1, val);
    TEST_ASSERT_TRUE(parseUInt32("+0x12", &val));
    TEST_ASSERT_EQUAL_UINT32(0x12, val);
    TEST_ASSERT_TRUE(parseUInt32("4294967295", &val));
    TEST_ASSERT_EQUAL_UINT32(0xffffffff, val);
    TEST_ASSERT_TRUE(parseUInt32("0XfFffffFF", &val));
    TEST_ASSERT_EQUAL_UINT32(0xffffffff, val);
    TEST_ASSERT_TRUE(parseUInt32("0x12ABcDEf", &val));
    TEST_ASSERT_EQUAL_UINT32(0x12ABCDEF, val);
    TEST_ASSERT_TRUE(parseUInt32("1ABcDEf", &val, 16));
    TEST_ASSERT_EQUAL_UINT32(0x1ABCDEF, val);
}

void test_parse_int32(void)
{
    int32_t val = 999;
    TEST_ASSERT_FALSE(parseInt32("", &val));
    TEST_ASSERT_FALSE(parseInt32("AB", &val));
    TEST_ASSERT_FALSE(parseInt32("AB", &val));
    TEST_ASSERT_FALSE(parseInt32("0o78", &val));
    TEST_ASSERT_FALSE(parseInt32("0b12", &val));
    TEST_ASSERT_FALSE(parseInt32("12A", &val));
    TEST_ASSERT_FALSE(parseInt32("+", &val));
    TEST_ASSERT_FALSE(parseInt32("++", &val));
    TEST_ASSERT_FALSE(parseInt32("++1", &val));
    TEST_ASSERT_FALSE(parseInt32("-", &val));
    TEST_ASSERT_FALSE(parseInt32("x1a", &val));
    TEST_ASSERT_FALSE(parseInt32("0x80000000", &val));
    TEST_ASSERT_FALSE(parseInt32("0x", &val));
    TEST_ASSERT_FALSE(parseInt32("1.", &val));
    TEST_ASSERT_FALSE(parseInt32("0x1234", &val, 10));
    TEST_ASSERT_FALSE(parseInt32("1.", &val, 10));
    TEST_ASSERT_EQUAL_INT32(999, val);

    TEST_ASSERT_TRUE(parseInt32("0", &val));
    TEST_ASSERT_EQUAL_INT32(0, val);
    TEST_ASSERT_TRUE(parseInt32("02", &val));
    TEST_ASSERT_EQUAL_INT32(2, val);
    TEST_ASSERT_TRUE(parseInt32("1", &val));
    TEST_ASSERT_EQUAL_INT32(1, val);
    TEST_ASSERT_TRUE(parseInt32("-1", &val));
    TEST_ASSERT_EQUAL_INT32(-1, val);
    TEST_ASSERT_TRUE(parseInt32("+1", &val));
    TEST_ASSERT_EQUAL_INT32(1, val);
    TEST_ASSERT_TRUE(parseInt32("+0x12", &val));
    TEST_ASSERT_EQUAL_INT32(0x12, val);
    TEST_ASSERT_TRUE(parseInt32("-0x12", &val));
    TEST_ASSERT_EQUAL_INT32(-0x12, val);
    TEST_ASSERT_TRUE(parseInt32("2147483647", &val));
    TEST_ASSERT_EQUAL_INT32(0x7fffffff, val);
    TEST_ASSERT_TRUE(parseInt32("-2147483648", &val));
    TEST_ASSERT_EQUAL_INT32(0x80000000, val);
    TEST_ASSERT_TRUE(parseInt32("+2147483647", &val));
    TEST_ASSERT_EQUAL_INT32(0x7fffffff, val);
    TEST_ASSERT_TRUE(parseInt32("0X7FffffFF", &val));
    TEST_ASSERT_EQUAL_INT32(0x7fffffff, val);
    TEST_ASSERT_TRUE(parseInt32("0x12ABcDEf", &val));
    TEST_ASSERT_EQUAL_INT32(0x12ABCDEF, val);
    TEST_ASSERT_TRUE(parseInt32("0b1111111111111111111111111111111", &val));
    TEST_ASSERT_EQUAL_INT32(0x7fffffff, val);
    TEST_ASSERT_TRUE(parseInt32("0b10", &val));
    TEST_ASSERT_EQUAL_INT32(2, val);
    TEST_ASSERT_TRUE(parseInt32("0B10", &val));
    TEST_ASSERT_EQUAL_INT32(2, val);
    TEST_ASSERT_TRUE(parseInt32("0o10", &val));
    TEST_ASSERT_EQUAL_INT32(8, val);
    TEST_ASSERT_TRUE(parseInt32("0O10", &val));
    TEST_ASSERT_EQUAL_INT32(8, val);
    TEST_ASSERT_TRUE(parseInt32("0x10", &val));
    TEST_ASSERT_EQUAL_INT32(16, val);
    TEST_ASSERT_TRUE(parseInt32("0X10", &val));
    TEST_ASSERT_EQUAL_INT32(16, val);
}

void test_parse_decimal32(void)
{
    int32_t val = 999;
    TEST_ASSERT_FALSE(parseDecimal32("", &val, 2));
    TEST_ASSERT_FALSE(parseDecimal32("AB", &val, 2));
    TEST_ASSERT_FALSE(parseDecimal32("+", &val, 2));
    TEST_ASSERT_FALSE(parseDecimal32("++1", &val, 2));
    TEST_ASSERT_FALSE(parseDecimal32("-", &val, 2));
    TEST_ASSERT_FALSE(parseDecimal32("0x12", &val, 2));
    TEST_ASSERT_EQUAL_INT32(999, val);

    TEST_ASSERT_TRUE(parseDecimal32("0", &val, 2));
    TEST_ASSERT_EQUAL_INT32(0, val);
    TEST_ASSERT_TRUE(parseDecimal32("1", &val, 2));
    TEST_ASSERT_EQUAL_INT32(100, val);
    /*
        // TEST_ASSERT_TRUE(parseDecimal32("2.", &val, 0));
        // TEST_ASSERT_EQUAL_INT32(2, val);
        TEST_ASSERT_TRUE(parseDecimal32("1.0", &val, 0));
        TEST_ASSERT_EQUAL_INT32(1, val);
        //*/
    TEST_ASSERT_TRUE(parseDecimal32("21474836.47", &val, 2));
    TEST_ASSERT_EQUAL_INT32(0x7fffffff, val);
    TEST_ASSERT_TRUE(parseDecimal32("-21474836.48", &val, 2));
    TEST_ASSERT_EQUAL_INT32(0x80000000, val);
    TEST_ASSERT_TRUE(parseDecimal32("+21474836.47", &val, 2));
    TEST_ASSERT_EQUAL_INT32(0x7fffffff, val);
}

void test_read_cmdline(void)
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

void test_read_cmdline_wrong_order(void)
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
    // Shell.exec(F("VER"), testout);
}

int main(int argc, char **argv)
{
    Shell.begin(user_commands, F("~"));
    // Shell.addEndpoint(tester);
    UNITY_BEGIN();
    RUN_TEST(test_uint_conversion);
    RUN_TEST(test_fraction_conversion);
    RUN_TEST(test_parse_uint32);
    RUN_TEST(test_parse_int32);
    RUN_TEST(test_parse_decimal32);
    RUN_TEST(test_read_cmdline);
    RUN_TEST(test_read_cmdline_wrong_order);
    RUN_TEST(test_shell);
    UNITY_END();

    return 0;
}
