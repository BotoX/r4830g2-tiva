#include "driverlib/can.h"
#include "inc/hw_ints.h"

extern tCANMsgObject g_sCAN0RxMessage;
extern tCANMsgObject g_sCAN0TxMessage;

extern uint8_t g_ui8TXMsgData[8];
extern uint8_t g_ui8RXMsgData[32];

extern bool g_bTXFlag;
extern bool g_Debug;
