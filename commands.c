//*****************************************************************************
//
// rgb_commands.c - Command line functionality implementation
//
// Copyright (c) 2012-2014 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 2.1.0.12573 of the EK-TM4C123GXL Firmware Package.
//
//*****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "utils/ustdlib.h"
#include "utils/uartstdio.h"
#include "utils/cmdline.h"
#include "utils.h"
#include "huawei.h"
#include "project0.h"


int CMD_help(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    UARTprintf("\nAvailable Commands\n------------------\n\n");

    int32_t i32Index = 0;
    while(g_psCmdTable[i32Index].pcCmd)
    {
        UARTprintf("%17s %s\n", g_psCmdTable[i32Index].pcCmd, g_psCmdTable[i32Index].pcHelp);
        i32Index++;
    }
    UARTprintf("\n");

    return 0;
}

int CMD_debug(int argc, char **argv)
{
    if(argc < 2) {
        UARTprintf("Usage: debug <0|1>\n");
        return 1;
    }

    const char *end = argv[1];
    g_Debug = ustrtoul(end, &end, 10);

    return 0;
}

int CMD_voltage(int argc, char **argv)
{
    if(argc < 2 || argc > 3) {
        UARTprintf("Usage: voltage <volts> [perm]\n");
        return 1;
    }

    const char *end = argv[1];
    float u = ustrtof(end, &end);

    bool perm = false;
    if(argc == 3) {
        end = argv[2];
        if(ustrtoul(end, &end, 10))
            perm = true;
    }

    SetVoltage(u, perm);

    return 0;
}

int CMD_current(int argc, char **argv)
{
    if(argc < 2 || argc > 3) {
        UARTprintf("Usage: current <amps> [perm]\n");
        return 1;
    }

    const char *end = argv[1];
    float i = ustrtof(end, &end);

    bool perm = false;
    if(argc == 3) {
        end = argv[2];
        if(ustrtoul(end, &end, 10))
            perm = true;
    }

    SetCurrent(i, perm);

    return 0;
}

int CMD_can(int argc, char **argv)
{
    if(argc != 3) {
        UARTprintf("Usage: can <msgid> <hex>\n");
        return 1;
    }

    const char *end = argv[1];
    uint32_t msgid = ustrtoul(end, &end, 16);
    uint8_t data[8];

    int len = hex2bytes(argv[2], data, sizeof(data));
    if(len <= 0)
        return 1;

    SendCAN(msgid, data, len);

    return 0;
}



//*****************************************************************************
//
// Table of valid command strings, callback functions and help messages.  This
// is used by the cmdline module.
//
//*****************************************************************************
tCmdLineEntry g_psCmdTable[] =
{
    {"help",     CMD_help,      " : Display list of commands" },
    {"debug",    CMD_debug,     " : Debug <0|1>" },
    {"voltage",  CMD_voltage,   " : voltage <volts> [perm]"},
    {"current",  CMD_current,   " : current <amps> [perm]"},
    {"can",      CMD_can,       " : can <msgid> <hex>"},
    { 0, 0, 0 }
};
