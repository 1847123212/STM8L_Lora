#ifndef __AT_H
#define __AT_H

typedef enum{
  at_statIdle,
  at_statRecving,
  at_statProcess,
  at_statIpSending,
  at_statIpSended,
  at_statIpTraning
}at_stateType;

typedef struct
{
    char *at_cmdName;
    int8_t at_cmdLen;
    void (*at_testCmd)();
    void (*at_queryCmd)();
    void (*at_setupCmd)(char *pPara);
    void (*at_exeCmd)();
}at_funcationType;

#endif