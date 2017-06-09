#ifndef __AT_H
#define __AT_H

typedef enum{
  at_statIdle,
  at_statRecving,
  at_statProcess,
  at_statIpSending,
  at_statIpSended,
  at_statTranInit,
  at_statIpTraning
}at_stateType;

typedef struct
{
    char *at_cmdName;
    int8_t at_cmdLen;
    void (*at_Cmd)(char *pPara);
}at_funcationType;

#endif