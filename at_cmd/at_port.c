#include "at_port.h"
#include "at_cmd.h"
#include "at.h"
#include "sx1278.h"
#define at_cmdLenMax 64
#define at_dataLenMax 128

at_stateType  at_state;

static uint8_t at_cmdLine[at_cmdLenMax];
uint8_t at_dataLine[at_dataLenMax];/////
//uint8_t *pDataLine;

void at_recv_event(char temp)
{
    static uint8_t atHead[2];
    static uint8_t *pCmdLine;

    switch(at_state)
    {
    case at_statIdle:
        atHead[0] = atHead[1];
        atHead[1] = temp;
        if((memcmp(atHead, "AT", 2) == 0) || (memcmp(atHead, "at", 2) == 0))
        {
            at_state = at_statRecving;
            pCmdLine = at_cmdLine;
            atHead[1] = 0x00;
        }

      break;
    case at_statRecving: //push receive data to cmd line
      *pCmdLine = temp;
      if(temp == '\n')
      {
        pCmdLine++;
        *pCmdLine = '\0';
        at_state = at_statProcess;

      }
      else if(pCmdLine >= &at_cmdLine[at_cmdLenMax - 1])
      {
        at_state = at_statIdle;
      }
      pCmdLine++;
      break;      
    case at_statProcess: //process data
      if(temp == '\n')
      {
//      system_os_post(at_busyTaskPrio, 0, 1);
        uart1_write_string("\r\nbusy p...\r\n");
      }
      break;
      /*
    case at_statIpSending:
        *pDataLine = temp;
        if( (pDataLine >= &at_dataLine[at_dataLenMax - 1]))//应该加上超时判断
        {
            //system_os_post(at_procTaskPrio, 0, 0);
            at_state = at_statIpSended;
        }
        else
        {
            pDataLine++;
        }
      break;
    case at_statIpSended: //send data
    if(temp == '\n')
    {
        //system_os_post(at_busyTaskPrio, 0, 2);
        uart1_write_string("busy s...\r\n");
    }
      break;  */   
    }
}
void at_process_loop()
{
    if(at_state == at_statProcess)
    {
        at_cmdProcess(at_cmdLine);
    }
    else if(at_state == at_statIpSended)
    {
        SX1278SetTxPacket(at_dataLine,123);//UartDev.rcv_buff.pRcvMsgBuff);
    }
    else if(at_state == at_statIpTraning)
    {
        //at_ipDataSendNow();//UartDev.rcv_buff.pRcvMsgBuff);
    }
}