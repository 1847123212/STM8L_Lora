#include "at_port.h"
#include "at_cmd.h"
#include "at.h"
#include "sx1278.h"
#define at_cmdLenMax 256

at_stateType  at_state;

 uint8_t at_cmdLine[at_cmdLenMax];
//uint8_t at_dataLine[at_dataLenMax];/////
//uint8_t *pDataLine;

void at_recv_event(char temp)
{
    static uint8_t atHead[2];
    static uint8_t *pCmdLine;
    static uint16_t count = 0;

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
        uart1_write_string("ERR\r\n");
      }
      break;
      
      
    case at_statTranInit://no break;
        pCmdLine = at_cmdLine;
        at_state = at_statIpTraning;
    case at_statIpTraning:
      if(count  < (LoRaPacket.len - 4) )
      {
        *pCmdLine = temp;
        pCmdLine++;
        count++;
      }
      
      if(count >= (LoRaPacket.len - 4))
      {
            LoRaPacket.source.val = LoRaAddr;
            LoRaPacket.destination.val = DestAddr;
            LoRaPacket.data = (uint8_t *)at_cmdLine;
            SX1278SetTxPacket(&LoRaPacket);
            uart1_write_string("END\r\n");
            at_state = at_statIdle;
            count = 0;
      }
      break;
      /*
    case at_statIpSended: //send data
    if(temp == '\n')
    {
        //system_os_post(at_busyTaskPrio, 0, 2);
        uart1_write_string("busy s...\r\n");
    }
      break;*/     
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
       // SX1278SetTxPacket(at_dataLine,123);//UartDev.rcv_buff.pRcvMsgBuff);
    }
    else if(at_state == at_statIpTraning)
    {
        //at_ipDataSendNow();//UartDev.rcv_buff.pRcvMsgBuff);
    }
}