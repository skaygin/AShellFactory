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
#ifndef _SHELL_CONTROLLER_H
#define _SHELL_CONTROLLER_H

//********************************
// Shell Factory for Projects based on Arduino compatible boards
// Version 1.0
//
//
// By Serkan KAYGIN
//********************************

// VERSION HISTORY:
// v1.0
// First open source version

#include <Arduino.h>
#include "ShellFraming.h"
#include <shell/ArgumentReader.h>

#if !defined(SHELL_MAX_REQUEST_LEN)
#define SHELL_MAX_REQUEST_LEN 80
#endif

#if !defined(SHELL_HELP_ALIGN_MAX_COMMAND_LEN)
#define SHELL_HELP_ALIGN_MAX_COMMAND_LEN 9
#endif

typedef int8_t (*CommandHandlerFunc)(ArgumentReader &, Print &);

struct CommandMapping
{
    PGM_P command;
    CommandHandlerFunc handler;
    PGM_P helptext;
};

#define COMMAND_HANDLER(C, REQ, RESP, HELPSTR) \
    const char __cmd__##C[] PROGMEM = #C;      \
    const char __hlp__##C[] PROGMEM = HELPSTR; \
    int8_t __run__##C(ArgumentReader &REQ, Print &RESP)

#define EXTERN_COMMAND_HANDLER(C, REQ, RESP, HELPSTR) \
    const char __cmd__##C[] PROGMEM = #C;             \
    const char __hlp__##C[] PROGMEM = HELPSTR;        \
    extern int8_t __run__##C(ArgumentReader &REQ, Print &RESP)

#define MAP_HANDLER(C) \
    (CommandMapping) { __cmd__##C, &__run__##C, __hlp__##C }

// zero returned on successful execution
#define SHELL_RESPONSE_OK 0

const char PSTR_SHELL_RESPONSE_ERR_PREFIX[] PROGMEM = "ERR:";

// These error codes should be sequential in ascending order
const int8_t SHELL_RESPONSE_ERR_CUSTOM_PREFIX = 0; // negative return values are displayed with this prefix
const int8_t SHELL_RESPONSE_ERR_BAD_ARGUMENT = 1;
const int8_t SHELL_RESPONSE_ERR_TIMEOUT = 2;
const int8_t SHELL_RESPONSE_ERR_ILLEGAL_STATE = 3;
const int8_t SHELL_RESPONSE_ERR_IO_ERROR = 4;
const int8_t SHELL_RESPONSE_ERR_COMMUNICATION_FAULT = 5;
const int8_t SHELL_RESPONSE_ERR_ILLEGAL_OPERATION = 6;

// remember to increment count if new response errors are added
#define SHELL_RESPONSE_ERROR_COUNT 7

const char PSTR_SHELL_RESPONSE_ERR_CUSTOM_PREFIX[] PROGMEM = "Custom-Error";
const char PSTR_SHELL_RESPONSE_ERR_BAD_ARGUMENT[] PROGMEM = "Bad or missing argument";
const char PSTR_SHELL_RESPONSE_ERR_TIMEOUT[] PROGMEM = "Operation timed out";
const char PSTR_SHELL_RESPONSE_ILLEGAL_STATE[] PROGMEM = "Illegal state";
const char PSTR_SHELL_RESPONSE_ERR_IO_ERROR[] PROGMEM = "I/O error";
const char PSTR_SHELL_RESPONSE_ERR_COMMUNICATION_FAULT[] PROGMEM = "Communication fault";
const char PSTR_SHELL_RESPONSE_ERR_ILLEGAL_OPERATION[] PROGMEM = "Illegal operation";

// THIS ORDER IS IMPORTANT, MUST FOLLOW THE ORDER IN CORRESPONDING DEFINITIONS
PGM_P const response_error_strings[] PROGMEM =
    {
        PSTR_SHELL_RESPONSE_ERR_CUSTOM_PREFIX,
        PSTR_SHELL_RESPONSE_ERR_BAD_ARGUMENT,
        PSTR_SHELL_RESPONSE_ERR_TIMEOUT,
        PSTR_SHELL_RESPONSE_ILLEGAL_STATE,
        PSTR_SHELL_RESPONSE_ERR_IO_ERROR,
        PSTR_SHELL_RESPONSE_ERR_COMMUNICATION_FAULT,
        PSTR_SHELL_RESPONSE_ERR_ILLEGAL_OPERATION,
};

class ShellController : public Print
{
private:
    uint8_t print_mode_; // share this as Print to save RAM
    ShellFraming *framing_layer_;
    ShellFraming *default_cmd_framing_;
    ShellFraming *pending_framing_;
    ArgumentReader *request_;
    Stream *requesting_stream_;
    PGM_P user_command_defs_;
    PGM_P admin_command_defs_;
    void *head_node_;
    byte request_buf_[SHELL_MAX_REQUEST_LEN + 1]; //+1 for null termination
    byte *request_buf_ptr_;
    static CommandHandlerFunc getFunctionByCommandDef_(CommandMapping *);
    static CommandMapping *findCommandDef_(PGM_P pgmp, char *cmd);
    static void printHelp_(Print &out, PGM_P command_defs, const char *cmd);
    void printError_(Print &out, int8_t errorcode);
    void endExecute_();
    void beginResponse_(Print *out);
    void endResponse_(Print *out, int8_t error_code = SHELL_RESPONSE_OK);

public:
    static ShellController *context();
    ShellController();
    void begin(const CommandMapping user_commands[], char prompt = '>');
    void setAdminCommands(const CommandMapping admin_commands[]);
    void addEndpoint(Stream &stream);
    void removeEndpoint(Stream &stream);
    Stream *getRequestingEndpoint();
    void printHelp(Print &out, bool admin, char *cmd = 0);

    void tick(bool greedy = true);
    int8_t call(byte *command, Print &response);
    void run(byte *command, Print &out);
    void run(const __FlashStringHelper *command, Print &out);

    char *available(bool greedy);
    CommandMapping *findCommandDefinition(char *command);
    CommandHandlerFunc findCommandFunction(char *command);
    void setFraming(ShellFraming *framing);
    virtual size_t write(uint8_t c);
};

extern ShellController Shell;

//***************************** ESSENTIAL/COMMON COMMANDS *****************

EXTERN_COMMAND_HANDLER(HELP, request, response, "Provides Help information for commands. [-a] [<cmd>]");

#endif //_SHELL_CONTROLLER_H
