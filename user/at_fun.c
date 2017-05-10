#include "at_fun.h"
#include "at.h"
#include "at_cmd.h"
#include "sx1278.h"
#include <stdlib.h>
extern at_stateType  at_state;

void at_CmdReset(char *pPara)
{
    at_backOk;
    at_state = at_statIdle;
    WWDG->CR |= 0x80;//WDGA=1
    WWDG->CR &= 0xbf;//T6ÖÃ0
}
void at_CmdSleep(char *pPara)
{
    at_backOk;
    at_state = at_statIdle;
}
void at_CmdVersion(char *pPara)
{
    uart1_write_string(VERSION);
    at_backOk;
    at_state = at_statIdle;
}
void at_CmdPD0(char *pPara)
{
    if(*pPara++ == '=')
    {
        if(*pPara == '0')
            gpio_pd0_write(0);
        else
            gpio_pd0_write(1);
        at_backOk;
        at_state = at_statIdle;
    }
    else if(*pPara++ == '?')
    {
        at_backOk;
        at_state = at_statIdle;
    }

}

void at_CmdPB0(char *pPara)
{
    if(*pPara == '=')
    {
        pPara++;
        if(*pPara == '0')
            gpio_pb0_write(0);
        else
            gpio_pb0_write(1);
        at_backOk;
        at_state = at_statIdle;
    }
    else if(*pPara == '?')
    {
        pPara++;
        GPIO_Init(GPIOB,GPIO_Pin_0,GPIO_Mode_In_FL_No_IT);
        if(GPIOB->IDR & GPIO_Pin_0)
        {
            
            uart1_write_string("1");
        }
        else
            uart1_write_string("0");

        at_backOk;
        at_state = at_statIdle;
    }
}

void at_CmdPWM(char *pPara)
{

    at_backOk;
    at_state = at_statIdle;
}

void at_CmdNull(char *pPara)
{
    at_backOk;
    at_state = at_statIdle;
}
bool CheckPara(char *pPara)
{
    uint8_t dot=0,err = 0;
    while(*pPara != '\r')
    {
        if(*pPara == ',' )
        {
            dot++;
        }
        else if(*pPara < '0' || *pPara > '9')
        {
            err = 1;
        }
        pPara++;
          
    }
    if(dot == 12 && err == 0)
        return TRUE;
    else
        return FALSE;
}
void at_CmdConfig(char *pPara)
{
    uint8_t buf[20];
    int i = 0;
    uint8_t count = 0;
    if(pPara[count++] == '=')
    {
        if(CheckPara(&pPara[count]) == FALSE) 
        {
            at_backError;
            at_state = at_statIdle;
            return ;
        }
        i = 0;
        while(pPara[count] != ',')
        {
            buf[i++] = pPara[count++];
        }
        buf[i++] = '\0';
        LoRaSettings.RFFrequency = atol((const char*)buf);
        
        i = 0;
        count++;
        while(pPara[count] != ',')
        {
            buf[i++] = pPara[count++];
        }
        buf[i++] = '\0';
        LoRaSettings.Power = atoi((const char*)buf);
        
        i = 0;
        count++;
        while(pPara[count] != ',')
        {
            buf[i++] = pPara[count++];
        }
        buf[i++] = '\0';
        LoRaSettings.SignalBw = atoi((const char*)buf);
        
        i = 0;
        count++;
        while(pPara[count] != ',')
        {
            buf[i++] = pPara[count++];
        }
        buf[i++] = '\0';
        LoRaSettings.SpreadingFactor = atoi((const char*)buf);
        
        i = 0;
        count++;
        while(pPara[count] != ',')
        {
            buf[i++] = pPara[count++];
        }
        buf[i++] = '\0';
        LoRaSettings.ErrorCoding = atoi((const char*)buf);
        
        i = 0;
        count++;
        while(pPara[count] != ',')
        {
            buf[i++] = pPara[count++];
        }
        buf[i++] = '\0';
        LoRaSettings.CrcOn = (bool)atoi((const char*)buf);
        
        i = 0;
        count++;
        while(pPara[count] != ',')
        {
            buf[i++] = pPara[count++];
        }
        buf[i++] = '\0';
        LoRaSettings.ImplicitHeaderOn = (bool)atoi((const char*)buf);
        
        i = 0;
        count++;
        while(pPara[count] != ',')
        {
            buf[i++] = pPara[count++];
        }
        buf[i++] = '\0';
        LoRaSettings.RxSingleOn = (bool)atoi((const char*)buf);
        
        i = 0;
        count++;
        while(pPara[count] != ',')
        {
            buf[i++] = pPara[count++];
        }
        buf[i++] = '\0';
        LoRaSettings.FreqHopOn = (bool)atoi((const char*)buf);
        
        i = 0;
        count++;
        while(pPara[count] != ',')
        {
            buf[i++] = pPara[count++];
        }
        buf[i++] = '\0';
        LoRaSettings.HopPeriod = (bool)atoi((const char*)buf);        
                
        i = 0;
        count++;
        while(pPara[count] != ',')
        {
            buf[i++] = pPara[count++];
        }
        buf[i++] = '\0';
        LoRaSettings.RxPacketTimeout = atoi((const char*)buf);
        
        i = 0;
        count++;
        while(pPara[count] != ',')
        {
            buf[i++] = pPara[count++];
        }
        buf[i++] = '\0';
        LoRaSettings.PayloadLength = atoi((const char*)buf);
        
        i = 0;
        count++;
        while(pPara[count] != '\r')
        {
            buf[i++] = pPara[count++];
        }
        buf[i++] = '\0';
        LoRaSettings.PreambleLength = atoi((const char*)buf);
        
        SX1278Init();


        at_backOk;
        at_state = at_statIdle;
    }
}
void at_CmdRxMode(char *pPara)
{
    SX1278SetRFState(RFLR_STATE_RX_INIT);
    at_backOk;
    at_state = at_statIdle;
}
void at_CmdTxPacket(char *pPara)
{
    uint8_t len = 0;
    char *p = pPara;
    if(*pPara++ == '=')
    {
        while(*p++ != '\n')
        {
            len++;
        }
        SX1278SetTxPacket((uint8_t *)pPara,(len-1));
    }
    at_backOk;
    at_state = at_statIdle;
}