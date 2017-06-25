#ifndef __AT_CMD_H
#define __AT_CMD_H
#include "ebox.h"
#include "at.h"
#define VERSION "V0.3"

#define AT_ERR_CMD      "ERR:CMD\r\n"
#define AT_ERR_CPU_BUSY  "ERR:CPU_BUSY\r\n"
#define AT_ERR_RF_BUSY  "ERR:RF_BUSY\r\n"
#define AT_ERR_SYMBLE   "ERR:SYMBLE\r\n"
#define AT_ERR_PARA     "ERR:PARA\r\n"
#define AT_ERR_NONE     "ERR:NONE\r\n"

#define at_backHead     uart1_write_string("AT,")
#define at_backOk       uart1_write_string("OK\r\n")
#define at_backErr      uart1_write_string("ERR\r\n")
#define at_backErrorCode(x)     uart1_write_string(x)
//#define at_backTeError   "+CTE ERROR: %d\r\n"

extern at_funcationType at_fun[];
void at_cmdProcess(uint8_t *pAtRcvData);

#endif