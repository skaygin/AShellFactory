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
#include "ShellController.h"

#define F_P(x) (reinterpret_cast<const __FlashStringHelper *>(x))

// These error codes should be sequential in DESCENDING order, start should be 0x7f
const int8_t SHELL_RESPONSE_ERR_UNKNOWN_ERROR = 127;
const int8_t SHELL_RESPONSE_ERR_BAD_COMMAND = 126;
const int8_t SHELL_RESPONSE_ERR_COMMAND_TOO_LONG = 125;
const int8_t SHELL_RESPONSE_ERR_BAD_FRAME = 124;
// remember to increment count if new system commands are defined
#define SHELL_RESPONSE_SYSTEM_ERROR_COUNT 4

const char PSTR_SHELL_RESPONSE_ERR_UNKNOWN_ERROR[] PROGMEM = "Unknown error";
const char PSTR_SHELL_RESPONSE_ERR_BAD_COMMAND[] PROGMEM = "Unknown command";
const char PSTR_SHELL_RESPONSE_ERR_COMMAND_TOO_LONG[] PROGMEM = "Command too long";
const char PSTR_SHELL_RESPONSE_ERR_BAD_FRAME[] PROGMEM = "Bad frame";

// THIS ORDER IS IMPORTANT, MUST FOLLOW THE ORDER IN CORRESPONDING DEFINITIONS
PGM_P const system_error_strings[] PROGMEM =
    {
        PSTR_SHELL_RESPONSE_ERR_UNKNOWN_ERROR,
        PSTR_SHELL_RESPONSE_ERR_BAD_COMMAND,
        PSTR_SHELL_RESPONSE_ERR_COMMAND_TOO_LONG,
        PSTR_SHELL_RESPONSE_ERR_BAD_FRAME,
};

//*************** Framing **************

// ShellController inherits Print to optimize RAM space, has the following operation modes
const uint8_t PRINTMODE_IGNORE = 0;     // Ignores writes
const uint8_t PRINTMODE_REQUESTING = 1; // Forwarded to the request buffer
const uint8_t PRINTMODE_RESPONDING = 2; // Forwarded to requesting stream (CLI)
// Note: doing this using inner classes wasted 32 byte RAM

class DefaultFraming : public ShellFraming
{
private:
  char prompt_;

public:
  void begin(char prompt)
  {
    prompt_ = prompt;
  }
  virtual void endSend(Print *out)
  {
    out->println();
    if (prompt_)
      out->write(prompt_);
  }
  virtual int8_t receive(Print *in, char c)
  {
    if (c == 13)
      return 1;
    if (c != 10)
    {
      in->write(c & 0x7f);
    }
    return 0;
  }
};

//******************************Linked list node for registered endpoints (CLIs) ******
class EndpointNode
{
private:
public:
  Stream *stream; // endpoint, console
  EndpointNode *next;
  EndpointNode(Stream *endpoint_stream)
  {
    stream = endpoint_stream;
    next = 0;
  }
};

//******************* ShellController Implementation ****************************

ShellController *context_ = 0;

ShellController *ShellController::context()
{
  return context_;
}

ShellController::ShellController()
{
  user_command_defs_ = 0;
  head_node_ = 0;
  request_ = new ArgumentReader();
  default_cmd_framing_ = new DefaultFraming();
  framing_layer_ = default_cmd_framing_;
  pending_framing_ = 0;
  print_mode_ = PRINTMODE_IGNORE;
}

void ShellController::begin(const CommandMapping user_commands[], char prompt)
{
  ((DefaultFraming *)default_cmd_framing_)->begin(prompt);
  user_command_defs_ = (PGM_P)user_commands;
  admin_command_defs_ = 0;
  request_buf_ptr_ = &request_buf_[0];
}

void ShellController::setAdminCommands(const CommandMapping admin_commands[])
{
  admin_command_defs_ = (PGM_P)admin_commands;
}

void ShellController::setFraming(ShellFraming *framing)
{
  this->pending_framing_ = framing ? framing : default_cmd_framing_;
}

void ShellController::addEndpoint(Stream &stream)
{
  EndpointNode *newnode = new EndpointNode(&stream);
  if (!head_node_)
  {
    head_node_ = newnode;
    //_active_stream = &stream; // allows writes in setup() to be forwarded to the firstly registered console
  }
  else
  {
    EndpointNode *n = (EndpointNode *)head_node_;
    while (n->next)
      n = n->next;
    n->next = newnode;
  }
}

void ShellController::removeEndpoint(Stream &stream)
{
  EndpointNode *n = (EndpointNode *)head_node_;
  EndpointNode *prev = 0;
  while (n)
  {
    if (n->stream == &stream)
    {
      if (prev)
        prev->next = n->next;
      else
        head_node_ = n->next;
      n->next = 0;
      // if (_active_stream == &stream)
      //   _active_stream = _head_node ? ((StreamNode *)_head_node)->stream : 0;
      return;
    }
    prev = n;
    n = n->next;
  }
}

Stream *ShellController::getRequestingEndpoint()
{
  return requesting_stream_;
}

size_t ShellController::write(uint8_t c)
{
  byte *bufstart = &request_buf_[0];
  if (print_mode_ == PRINTMODE_REQUESTING)
  {
    if (!c) // NULL resets buffer
    {
      request_buf_ptr_ = &request_buf_[0];
    }
    else
    {
      // if (((int)request_buf_ptr_ - (int)bufstart) < SHELL_MAX_REQUEST_LEN)
      if ((request_buf_ptr_ - bufstart) < SHELL_MAX_REQUEST_LEN)
        *request_buf_ptr_ = c;
      request_buf_ptr_++; // increase cmdptr but do not alter buffer, this allows backspace
    }
  }
  else if (print_mode_ == PRINTMODE_RESPONDING)
  {
    if (requesting_stream_)
      framing_layer_->send(requesting_stream_, c);
  }
  return 1;
}

// assumes that bufptr point to the start char of command, spaces are converted to null, cmdptr points to the parameters
CommandMapping *ShellController::findCommandDef_(PGM_P pgmp, char *chptr)
{
  PGM_P cmd = pgmp;
  while (cmd)
  {
    PGM_P cmdp = (PGM_P)pgm_read_ptr(cmd);
    if (!cmdp)
      break;
    if (strcasecmp_P(chptr, cmdp) == 0)
      return (CommandMapping *)cmd;
    cmd += sizeof(CommandMapping);
  }
  return 0;
}

void ShellController::printHelp_(Print &out, PGM_P command_defs, const char *cmd)
{
  // cmd == 0 means ALL, help for SPECIFIC command otherwise
  if (command_defs) // admin commands might not be set
    while (true)
    {
      PGM_P cmdp = (PGM_P)pgm_read_ptr(command_defs);
      if (!cmdp)
        break;
      // traverses each command definition
      if (!cmd || strcasecmp_P(cmd, cmdp) == 0)
      {
        // enters here when command is found or help is for ALL commands
        if (!cmd) // ALL
        {
          // print command with spaces padded to the hend
          out.print(F_P(cmdp));
          int len = SHELL_HELP_ALIGN_MAX_COMMAND_LEN - strlen_P((PGM_P)cmdp);
          while (len-- > 0)
            out.write(' ');
          out.write(' ');
        }
        // Both ALL and SPECIFIC
        PGM_P hlptxtp = (PGM_P)pgm_read_ptr(command_defs + sizeof(PGM_P) + sizeof(CommandHandlerFunc));
        // write until dot (included)
        char c;
        do
        {
          c = pgm_read_byte(hlptxtp);
          if (c == 0)
            break;
          out.write(c);
          hlptxtp++;
        } while (c != '.');
        out.println();
        if (cmd) // SPECIFIC
        {
          out.print(F_P(cmdp));    // writes command (without spaces)
          out.print(F_P(hlptxtp)); // writes remaining chars after dot
          return;                  // SPECIFIC returns after found
        }
      }
      command_defs += sizeof(CommandMapping);
    }
  // ALL
  if (cmd)
    out.print(F_P(PSTR_SHELL_RESPONSE_ERR_BAD_COMMAND)); // Command not found
  else
    out.println(F("\r\nFor more information on commands use HELP <cmd>."));
}

void ShellController::printHelp(Print &out, bool admin, char *cmd)
{
  printHelp_(out, admin ? admin_command_defs_ : user_command_defs_, cmd);
}

void ShellController::printError_(Print &out, int8_t errorcode)
{
  out.print(F("ERR:"));
  int8_t index = errorcode;
  if (errorcode < 0)
    index = SHELL_RESPONSE_ERR_CUSTOM_PREFIX;
  else if (errorcode < SHELL_RESPONSE_ERROR_COUNT)
    index = errorcode;
  else
  {
    // system error
    index = 127 - errorcode;
    if (index >= SHELL_RESPONSE_SYSTEM_ERROR_COUNT)
      index = 127 - SHELL_RESPONSE_ERR_UNKNOWN_ERROR;
    out.print(F_P(pgm_read_ptr(&(system_error_strings[index]))));
    return;
  }
  out.print(F_P(pgm_read_ptr(&(response_error_strings[index]))));
  if (errorcode < 0)
    out.print(errorcode);
}

#define NUL 0
#define STX 2
#define ETX 3
#define CR 13
#define LF 10

void ShellController::beginResponse_(Print *out)
{
  context_ = this;
  print_mode_ = PRINTMODE_RESPONDING;
  framing_layer_->beginSend(out); //&_response_out
}

void ShellController::endResponse_(Print *out, int8_t error_code)
{
  if (error_code)
    printError_(*out, error_code);
  request_buf_ptr_ = &request_buf_[0];
  framing_layer_->endSend(out); //&_response_out
  if (requesting_stream_)
    requesting_stream_->flush(); // flush after command is executed, useful for buffered streams
  requesting_stream_ = 0;
  context_ = 0;
  print_mode_ = PRINTMODE_IGNORE;
  if (pending_framing_)
  {
    framing_layer_ = pending_framing_;
    pending_framing_ = 0;
  }
}

CommandMapping *ShellController::findCommandDefinition(char *command)
{
  CommandMapping *cmddef = findCommandDef_((PGM_P)user_command_defs_, command);
  if (!cmddef)
    cmddef = findCommandDef_((PGM_P)admin_command_defs_, command);
  return cmddef;
}

CommandHandlerFunc ShellController::findCommandFunction(char *command)
{
  CommandMapping *cmddef = findCommandDefinition(command);
  return cmddef ? getFunctionByCommandDef_(cmddef) : 0;
}

CommandHandlerFunc ShellController::getFunctionByCommandDef_(CommandMapping *cmd)
{
  return (CommandHandlerFunc)((PGM_P)pgm_read_ptr(&(cmd->handler)));
}

int8_t ShellController::call(byte *command, Print &response)
{
  char *cmdstart; // = command;
  request_->begin(command);
  request_->readString(&cmdstart, true);
  // _request_buf_ptr points the first parameter (or null)
  CommandMapping *cmd = findCommandDefinition(cmdstart);
  if (!cmd)
    return SHELL_RESPONSE_ERR_BAD_COMMAND;
  else
  {
    CommandHandlerFunc func = getFunctionByCommandDef_(cmd);
    int8_t ret = func(*request_, response);
    if (ret >= SHELL_RESPONSE_ERROR_COUNT)
      ret = SHELL_RESPONSE_ERR_UNKNOWN_ERROR;
    return ret;
  }
}

void ShellController::run(byte *command, Print &out)
{
  beginResponse_(&out);
  int8_t errcode = call(command, out);
  endResponse_(&out, errcode);
}

void ShellController::run(const __FlashStringHelper *command, Print &out)
{
  byte *start = &request_buf_[0];
  strncpy_P((char *)start, (PGM_P)command, SHELL_MAX_REQUEST_LEN);
  run(start, out);
}

char *ShellController::available(bool greedy)
{
  byte *bufstart = &request_buf_[0];
  print_mode_ = PRINTMODE_REQUESTING;
  EndpointNode *n = (EndpointNode *)head_node_;
  while (n)
  {
    Stream *s = n->stream;
    while (s->available())
    {
      int8_t rcvres = framing_layer_->receive(this, s->read());
      if (rcvres)
      {
        requesting_stream_ = s;
        int8_t errcode = 0;
        if (rcvres < 0)
        {
          errcode = SHELL_RESPONSE_ERR_BAD_FRAME;
        }
        // else if (((int)request_buf_ptr_ - (int)bufstart) > SHELL_MAX_REQUEST_LEN)
        else if ((request_buf_ptr_ - bufstart) > SHELL_MAX_REQUEST_LEN)
        {
          errcode = SHELL_RESPONSE_ERR_COMMAND_TOO_LONG;
        }
        else
        {
          *request_buf_ptr_ = '\0'; // null termination
          if (request_buf_ptr_ > bufstart)
          { // empty command does not raise error
            print_mode_ = PRINTMODE_IGNORE;
            return (char *)bufstart;
          }
        }
        beginResponse_(this);
        endResponse_(this, errcode);
      }
      if (!greedy)
        break;
    }
    n = n->next;
  }
  print_mode_ = PRINTMODE_IGNORE;
  return 0;
}

void ShellController::tick(bool greedy)
{
  byte *cmdp = (byte *)available(greedy); // sets _requesting_stream internally
  if (cmdp)
  {
    run(cmdp, *this); //_response_out
  }
}

ShellController Shell; // create object

// *************** BUILT-IN COMMANDS *************************

int8_t __run__HELP(ArgumentReader &request, Print &response)
{
  char *cmd;
  bool isadmin = false;
  if (!request.readString(&cmd))
  {
    cmd = 0;
  }
  else
  {
    if (strcasecmp_P(cmd, PSTR("-A")) == 0)
    {
      isadmin = true;
      if (!request.readString(&cmd))
      {
        cmd = 0;
      }
    }
  }
  if (context_)
    context_->printHelp(response, isadmin, cmd);
  return 0;
}
