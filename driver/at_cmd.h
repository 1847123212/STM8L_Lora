#ifndef __AT_CMD_H
#define __AT_CMD_H
#include "ebox.h"
#include "at.h"

#define at_backOk        uart1_write_string("\r\nOK\r\n")
#define at_backError     uart1_write_string("\r\nERROR\r\n")
#define at_backTeError   "+CTE ERROR: %d\r\n"

extern at_funcationType at_fun[];
void at_cmdProcess(uint8_t *pAtRcvData);

#endif