#include "at_cmd.h"
#include "at_fun.h"
#include "string.h"

#define at_cmdNum   32
at_funcationType at_fun[at_cmdNum]={
  
{NULL,          0,  at_CmdNull,     },
{"+RST",        4,  at_CmdReset,    },
{"+SLEEP",      6,  at_CmdSleep,    },
{"+RX",         3,  at_CmdRxMode,   },
{"+TX",         3,  at_CmdTxPacket,   },
{"+VERSION",    8,  at_CmdVersion,  },
{"+PB0",        4,  at_CmdPB0,      },
{"+PD0",        4,  at_CmdPD0,      },
{"+PWM1",       4,  at_CmdPWM,      },
{"+CFG",        4,  at_CmdConfig,   },

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
    if((*pCmd == '\r') || (*pCmd == '=') || (*pCmd == '?'))
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
        pAtRcvData += at_fun[cmdId].at_cmdLen;
        if(at_fun[cmdId].at_Cmd)
        {
            at_fun[cmdId].at_Cmd((char *)pAtRcvData);
        }
        else
        {
//        uart0_sendStr("no this fun\r\n"); //Relax, it's just a code.
            at_backError;
        }
      /*
//    os_printf("cmd id: %d\r\n", cmdId);
    pAtRcvData += at_fun[cmdId].at_cmdLen;
    if(*pAtRcvData == '\r')
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
    else if(*pAtRcvData == '?' && (pAtRcvData[1] == '\r'))
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
    }*/
  }
  else 
  {
  	at_backError;
  }

}