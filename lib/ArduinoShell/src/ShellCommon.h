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
#ifndef _SHELL_COMMON_H
#define _SHELL_COMMON_H

#include <Arduino.h>
#include "shell/ArgumentReader.h"

// zero returned on successful execution
#define SHELL_RESPONSE_OK 0

// These error codes should be sequential in ascending order
const int8_t SHELL_RESPONSE_ERR_CUSTOM_PREFIX = 0; // negative return values are displayed with this prefix
const int8_t SHELL_RESPONSE_ERR_BAD_ARGUMENT = 1;
const int8_t SHELL_RESPONSE_ERR_TIMEOUT = 2;
const int8_t SHELL_RESPONSE_ERR_ILLEGAL_STATE = 3;
const int8_t SHELL_RESPONSE_ERR_IO_ERROR = 4;
const int8_t SHELL_RESPONSE_ERR_COMMUNICATION_FAULT = 5;
const int8_t SHELL_RESPONSE_ERR_ILLEGAL_OPERATION = 6;

// Note for Developer: Remember to increment count if new response errors are added
#define SHELL_RESPONSE_ERROR_COUNT 7

typedef int8_t (*CommandHandlerFunc)(ArgumentReader &, Print &);

struct ShellCommandStruct
{
    PGM_P command;
    CommandHandlerFunc handler;
    PGM_P helptext;
};

#define DECLARE_COMMAND_HANDLER(C, HELPSTR)             \
    const char _shell_pstr_cmd_##C[] PROGMEM = #C;      \
    const char _shell_pstr_hlp_##C[] PROGMEM = HELPSTR; \
    extern int8_t _shell_handle_##C(ArgumentReader &, Print &)

#define IMPLEMENT_COMMAND_HANDLER(C, REQ, RESP) \
    int8_t _shell_handle_##C(ArgumentReader &REQ, Print &RESP)

#define COMMAND_HANDLER(C, REQ, RESP, HELPSTR)          \
    const char _shell_pstr_cmd_##C[] PROGMEM = #C;      \
    const char _shell_pstr_hlp_##C[] PROGMEM = HELPSTR; \
    int8_t _shell_handle_##C(ArgumentReader &REQ, Print &RESP)

#define DECLARE_SHELL_COMMANDS(name) \
    const ShellCommandStruct name[] PROGMEM

#define SHELL_COMMAND(C) \
    (ShellCommandStruct) { _shell_pstr_cmd_##C, &_shell_handle_##C, _shell_pstr_hlp_##C }

// this is not to store sizeof array in memory
#define END_SHELL_COMMANDS \
    (ShellCommandStruct){0, 0, 0},

#endif //_SHELL_COMMON_H
