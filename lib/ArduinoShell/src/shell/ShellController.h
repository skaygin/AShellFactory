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

#include <Arduino.h>
#include <ShellCommon.h>
#include "ShellFraming.h"

#if !defined(SHELL_MAX_REQUEST_LEN)
#define SHELL_MAX_REQUEST_LEN 80
#endif

#if !defined(SHELL_HELP_ALIGN_MAX_COMMAND_LEN)
#define SHELL_HELP_ALIGN_MAX_COMMAND_LEN 9
#endif

#if !defined(SHELL_MAX_ENDPOINTS)
#define SHELL_MAX_ENDPOINTS 4
#endif

// By default backspace is defined as BS=8 character, make it NUL=0 to disable
#if !defined(SHELL_BACKSPACE_CHAR)
#define SHELL_BACKSPACE_CHAR 8
#endif

const char PSTR_SHELL_RESPONSE_ERR_PREFIX[] PROGMEM = "ERR:";

const char PSTR_SHELL_RESPONSE_ERR_CUSTOM_PREFIX[] PROGMEM = "Custom-Error";
const char PSTR_SHELL_RESPONSE_ERR_BAD_ARGUMENT[] PROGMEM = "Bad or missing argument";
const char PSTR_SHELL_RESPONSE_ERR_TIMEOUT[] PROGMEM = "Operation timed out";
const char PSTR_SHELL_RESPONSE_ILLEGAL_STATE[] PROGMEM = "Illegal state";
const char PSTR_SHELL_RESPONSE_ERR_IO_ERROR[] PROGMEM = "I/O error";
const char PSTR_SHELL_RESPONSE_ERR_COMMUNICATION_FAULT[] PROGMEM = "Communication fault";
const char PSTR_SHELL_RESPONSE_ERR_ILLEGAL_OPERATION[] PROGMEM = "Illegal operation";

// THIS ORDER IS IMPORTANT, MUST FOLLOW THE ORDER IN CORRESPONDING DEFINITIONS
const char *const response_error_strings[SHELL_RESPONSE_ERROR_COUNT] PROGMEM =
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
    uint8_t print_mode_; // share this instance of ShellController as Print for RAM optimization
    ShellFraming *framing_layer_;
    ShellFraming *default_cmd_framing_;
    ShellFraming *pending_framing_;
    ArgumentReader *request_;
    Stream *endpoints_[SHELL_MAX_ENDPOINTS];
    Stream *requesting_endpoint_;
    PGM_P user_command_start_P_;
    PGM_P admin_command_start_P_;
    byte request_buf_[SHELL_MAX_REQUEST_LEN + 1]; //+1 for null termination
    byte *request_buf_ptr_;
    static ShellCommandStruct *findCommandStruct_P_(PGM_P command_start_P, char *cmd);
    static CommandHandlerFunc getFunctionByCommandStruct_P_(ShellCommandStruct *);
    static void printHelp_(Print &out, PGM_P command_start_P, const char *cmd);
    void printError_(Print &out, int8_t errorcode);
    void endExecute_();
    void beginResponse_(Print *out);
    void endResponse_(Print *out, int8_t error_code = SHELL_RESPONSE_OK);

public:
    static ShellController *context();
    ShellController();
    void begin(const ShellCommandStruct user_commands[], const __FlashStringHelper *prompt = 0);
    void setAdminCommands(const ShellCommandStruct admin_commands[]);
    void addEndpoint(Stream &stream);
    void removeEndpoint(Stream &stream);
    Stream *getRequestingEndpoint();
    void printHelp(Print &out, bool admin, char *cmd = 0);

    void tick(bool greedy = true);
    int8_t call(byte *command_line, Print &response);
    void exec(byte *command_line, Print &out);
    void exec(const __FlashStringHelper *command_line, Print &out);

    char *available(bool greedy);
    ShellCommandStruct *findCommandDefinition(char *command);
    CommandHandlerFunc findCommandFunction(char *command);
    void setFraming(ShellFraming *framing);
    virtual size_t write(uint8_t c);
};

extern ShellController Shell;

#endif //_SHELL_CONTROLLER_H
