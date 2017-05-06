#include "at_cmd.h"
#include "at_fun.h"
#include "string.h"

#define at_cmdNum   32

at_funcationType at_fun[at_cmdNum]={
  
{"+RST",    4,      NULL,           NULL,             NULL,                 at_exe_cmd_rst  },
{"+PB0",    4,      NULL,           NULL,             at_setup_cmd_pb0,     NULL            },
{"+PD0",    4,      NULL,           NULL,             at_setup_cmd_pd0,     NULL            },
{"+PWM1",   4,      NULL,           NULL,             at_setup_cmd_pwm1,    NULL            },
{"+PWM2",   4,      NULL,           NULL,             at_setup_cmd_pwm1,    NULL            },

};
/**
  * @brief  Query and localization one commad.
  * @param  cmdLen: received length of command
  * @param  pCmd: point to received command 
  * @retval the id of command
  *   @arg -1: failure
  */
static int16_t at_cmdSearch(int8_t cmdLen, uint8_t *pCmd)
{
  int16_t i;

  if(cmdLen == 0)
  {
    return 0;
  }
  else if(cmdLen > 0)
  {
    for(i=1; i<at_cmdNum; i++)
    {
//      os_printf("%d len %d\r\n", cmdLen, at_fun[i].at_cmdLen);
      if(cmdLen == at_fun[i].at_cmdLen)
      {
//        os_printf("%s cmp %s\r\n", pCmd, at_fun[i].at_cmdName);
        if(memcmp(pCmd, at_fun[i].at_cmdName, cmdLen) == 0) //think add cmp len first
        {
          return i;
        }
      }
    }
  }
  return -1;
}
static int8_t at_getCmdLen(uint8_t *pCmd)
{
  uint8_t n,i;

  n = 0;
  i = 128;

  while(i--)
  {
    if((*pCmd == '\r') || (*pCmd == '=') || (*pCmd == '?') || ((*pCmd >= '0')&&(*pCmd <= '9')))
    {
      return n;
    }
    else
    {
      pCmd++;
      n++;
    }
  }
  return -1;
}
void at_cmdProcess(uint8_t *pAtRcvData)
{

  int16_t cmdId;
  int8_t cmdLen;

  cmdLen = at_getCmdLen(pAtRcvData);
  if(cmdLen != -1)
  {
    cmdId = at_cmdSearch(cmdLen, pAtRcvData);
  }
  else 
  {
  	cmdId = -1;
  }
  if(cmdId != -1)
  {
//    os_printf("cmd id: %d\r\n", cmdId);
    pAtRcvData += cmdLen;
    if(*pAtRcvData == '\r')
    {
      if(at_fun[cmdId].at_exeCmd)
      {
        at_fun[cmdId].at_exeCmd(cmdId);
      }
      else
      {
        at_backError;
      }
    }
    else if(*pAtRcvData == '?' && (pAtRcvData[1] == '\r'))
    {
      if(at_fun[cmdId].at_queryCmd)
      {
        at_fun[cmdId].at_queryCmd(cmdId);
      }
      else
      {
        at_backError;
      }
    }
    else if((*pAtRcvData == '=') && (pAtRcvData[1] == '?') && (pAtRcvData[2] == '\r'))
    {
      if(at_fun[cmdId].at_testCmd)
      {
        at_fun[cmdId].at_testCmd(cmdId);
      }
      else
      {
        at_backError;
      }
    }
    else if((*pAtRcvData >= '0') && (*pAtRcvData <= '9') || (*pAtRcvData == '='))
    {
      if(at_fun[cmdId].at_setupCmd)
      {
        at_fun[cmdId].at_setupCmd((char *)pAtRcvData);
      }
      else
      {
//        uart0_sendStr("no this fun\r\n"); //Relax, it's just a code.
        at_backError;
      }
    }
    else
    {
      at_backError;
    }
  }
  else 
  {
  	at_backError;
  }

}