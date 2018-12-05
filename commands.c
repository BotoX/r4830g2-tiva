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
#include "project0.h"
#include "drivers/rgb.h"
#include "inc/hw_types.h"
#include "utils/ustdlib.h"
#include "utils/uartstdio.h"
#include "utils/cmdline.h"
#include "utils.h"
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

    // calibration, non-linearity measured on my own PSU
    u += (u - 49.0) / 110.0;

    uint16_t hex = u * 1020;
    if(hex < 0xA600)
        hex = 0xA600;
    if(hex > 0xEA00)
        hex = 0xEA00;

    uint8_t perm = 0x00;
    if(argc == 3) {
        end = argv[2];
        if(ustrtoul(end, &end, 10))
            perm = 0x01;
    }

    if(perm) {
        if(hex < 0xC000)
            hex = 0xC000;
        if(hex > 0xE99A)
            hex = 0xE99A;
    }

    g_ui8TXMsgData[0] = 0x01;
    g_ui8TXMsgData[1] = perm;
    g_ui8TXMsgData[2] = 0x00;
    g_ui8TXMsgData[3] = 0x00;
    g_ui8TXMsgData[4] = 0x00;
    g_ui8TXMsgData[5] = 0x00;
    g_ui8TXMsgData[6] = (hex >> 8) & 0xFF;
    g_ui8TXMsgData[7] = hex & 0xFF;

    g_sCAN0TxMessage.ui32MsgID = 0x108180fe;
    g_sCAN0TxMessage.ui32MsgIDMask = 0;
    g_sCAN0TxMessage.ui32Flags = MSG_OBJ_TX_INT_ENABLE | MSG_OBJ_EXTENDED_ID;
    g_sCAN0TxMessage.ui32MsgLen = sizeof(g_ui8TXMsgData);
    g_sCAN0TxMessage.pui8MsgData = (uint8_t *)&g_ui8TXMsgData;

    g_bTXFlag = 1;

    return 0;
}

int CMD_raw(int argc, char **argv)
{
    if(argc != 3) {
        UARTprintf("Usage: raw <canid> <hex>\n");
        return 1;
    }

    const char *end = argv[1];
    uint32_t canid = ustrtoul(end, &end, 16);

    int len = hex2bytes(argv[2], g_ui8TXMsgData, sizeof(g_ui8TXMsgData));
    if(len <= 0)
        return 1;

    g_sCAN0TxMessage.ui32MsgID = canid;
    g_sCAN0TxMessage.ui32MsgIDMask = 0;
    g_sCAN0TxMessage.ui32Flags = MSG_OBJ_TX_INT_ENABLE | MSG_OBJ_EXTENDED_ID;
    g_sCAN0TxMessage.ui32MsgLen = len;
    g_sCAN0TxMessage.pui8MsgData = (uint8_t *)&g_ui8TXMsgData;

    g_bTXFlag = 1;

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
    {"raw",      CMD_raw,       " : raw <canid> <hex>"},
    { 0, 0, 0 }
};
