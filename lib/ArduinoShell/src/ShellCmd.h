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
#ifndef _SHELL_CMD_REPOSITORY_H
#define _SHELL_CMD_REPOSITORY_H

#include "shellcmd/ShellCmdPIN.h"
#include "shellcmd/ShellCmdEEPROM.h"

/*
//todo:
//reads atmel registers at memory addr range 0x00 to 0xff
int8_t __run__ATMELREG(ArgumentReader &request, Print &response)
{
  int16_t addr;
  if (!request.readInt(&addr))
    return SHELL_RESPONSE_ERR_BAD_ARGUMENT;
  uint8_t *regptr = (uint8_t *)addr;
  int16_t value;
  if (request.readInt(&value))
  {
    *regptr = value;
  }
  else
  {
    value = *regptr;
    response.print(value);
  }
  return 0;
}
*/

#endif //_SHELL_CMD_REPOSITORY_H
