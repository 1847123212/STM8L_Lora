#ifndef __AT_FUN1_H
#define __AT_FUN1_H

#include "ebox.h"

#define USE_REG 0

void at_CmdError();
void at_CmdNull(char *pPara);
void at_CmdReset(char *pPara);
void at_CmdSleep(char *pPara);
void at_CmdVersion(char *pPara);
#if USE_REG
void at_CmdReg(char *pPara);
#endif

void at_CmdPD0(char *pPara);
void at_CmdPB0(char *pPara);
void at_CmdPWM(char *pPara);
void at_CmdConfig(char *pPara);
void at_CmdRxMode(char *pPara);
void at_CmdTxPacket(char *pPara);

void at_CmdSaveConfig(char *pPara);

#endif
