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
#ifndef _SHELL_H
#define _SHELL_H

// TODO:
// - Unit tests to be reviewed
// - Python terminal for acceptance tests
// - Otomatically generate deployments for Arduino and PlatformIO
/*
- Examples
    - Basic : all built-in commands with admin, plus LED command
    - Tiny : no help just a single command
    - DynamicCommands : login
    - MultiEndpoints : both serial and software serial
    - AdvancedMultiShell : two endpoint to distinct shells
    - Bluetooth : Two endpoints, one bluetooth, other serial, forwards AT commands
    - Telnet : Needs ethernet, both serial and telnet works
    - Advanced : Shell Framing Layer
*/
#include "ShellCore.h"
#include "shell/ShellCmdHELP.h"
#include "shell/ShellCmdFREEMEM.h"
#include "shell/ShellCmdRESET.h"
#endif //_SHELL_H
