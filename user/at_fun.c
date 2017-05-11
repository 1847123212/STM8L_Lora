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
    WWDG->CR &= 0xbf;//T6置0
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
 char C2D(
    uint8_t c	/**< is a character('0'-'F') to convert to HEX */
)
{
    if (c >= '0' && c <= '9')
        return c - '0';

    return (char)c;
}
uint32_t ATOI32(
    char *str	/**< is a pointer to convert */
)
{
    uint32_t num = 0;
    while (*str != 0)
        num = num * 10 + C2D(*str++);
    return num;
}
uint32_t getPara(char **pPara)
{
    char buf[10];
    int i = 0;
    if(**pPara == ',' || **pPara == '=' )(*pPara)++;//跳过前面非数字区域
    while(**pPara >= '0' && **pPara <= '9')
    {
        buf[i++] = **pPara;
        (*pPara)++;
    }
    buf[i++] = '\0';
    return  ATOI32((char *)buf);
}
void at_CmdConfig(char *pPara)
{

    if(*pPara++ == '=')
    {
        if(CheckPara(pPara) == FALSE) 
        {
            at_backError;
            at_state = at_statIdle;
            return ;
        }
        LoRaSettings.RFFrequency = getPara(&pPara);
        LoRaSettings.Power = getPara(&pPara);
        LoRaSettings.SignalBw = getPara(&pPara);
        LoRaSettings.SpreadingFactor = getPara(&pPara);
        LoRaSettings.ErrorCoding = getPara(&pPara);
        LoRaSettings.CrcOn = (bool)getPara(&pPara);
        LoRaSettings.ImplicitHeaderOn = (bool)getPara(&pPara);
        LoRaSettings.RxSingleOn = (bool)getPara(&pPara);
        LoRaSettings.FreqHopOn = (bool)getPara(&pPara);
        LoRaSettings.HopPeriod = getPara(&pPara);
        LoRaSettings.RxPacketTimeout = getPara(&pPara);
        LoRaSettings.PayloadLength = getPara(&pPara);
        LoRaSettings.PreambleLength = getPara(&pPara);

        /*
        i = 0;
        while(pPara[count] != ',')
        {
            buf[i++] = pPara[count++];
        }
        buf[i++] = '\0';
        LoRaSettings.RFFrequency = ATOI32((char *)buf);
        
        i = 0;
        count++;
        while(pPara[count] != ',')
        {
            buf[i++] = pPara[count++];
        }
        buf[i++] = '\0';
        LoRaSettings.Power = ATOI32((char *)buf);
        
        i = 0;
        count++;
        while(pPara[count] != ',')
        {
            buf[i++] = pPara[count++];
        }
        buf[i++] = '\0';
        LoRaSettings.SignalBw = ATOI32((char *)buf);
        
        i = 0;
        count++;
        while(pPara[count] != ',')
        {
            buf[i++] = pPara[count++];
        }
        buf[i++] = '\0';
        LoRaSettings.SpreadingFactor = ATOI32((char *)buf);
        
        i = 0;
        count++;
        while(pPara[count] != ',')
        {
            buf[i++] = pPara[count++];
        }
        buf[i++] = '\0';
        LoRaSettings.ErrorCoding = ATOI32((char *)buf);
        
        i = 0;
        count++;
        while(pPara[count] != ',')
        {
            buf[i++] = pPara[count++];
        }
        buf[i++] = '\0';
        LoRaSettings.CrcOn = (bool)ATOI32((char *)buf);
        
        i = 0;
        count++;
        while(pPara[count] != ',')
        {
            buf[i++] = pPara[count++];
        }
        buf[i++] = '\0';
        LoRaSettings.ImplicitHeaderOn = (bool)ATOI32((char *)buf);
        
        i = 0;
        count++;
        while(pPara[count] != ',')
        {
            buf[i++] = pPara[count++];
        }
        buf[i++] = '\0';
        LoRaSettings.RxSingleOn = (bool)ATOI32((char *)buf);
        
        i = 0;
        count++;
        while(pPara[count] != ',')
        {
            buf[i++] = pPara[count++];
        }
        buf[i++] = '\0';
        LoRaSettings.FreqHopOn = (bool)ATOI32((char *)buf);
        
        i = 0;
        count++;
        while(pPara[count] != ',')
        {
            buf[i++] = pPara[count++];
        }
        buf[i++] = '\0';
        LoRaSettings.HopPeriod = ATOI32((char *)buf);        
                
        i = 0;
        count++;
        while(pPara[count] != ',')
        {
            buf[i++] = pPara[count++];
        }
        buf[i++] = '\0';
        LoRaSettings.RxPacketTimeout = ATOI32((char *)buf);
        
        i = 0;
        count++;
        while(pPara[count] != ',')
        {
            buf[i++] = pPara[count++];
        }
        buf[i++] = '\0';
        LoRaSettings.PayloadLength = ATOI32((char *)buf);
        
        i = 0;
        count++;
        while(pPara[count] != '\r')
        {
            buf[i++] = pPara[count++];
        }
        buf[i++] = '\0';
        LoRaSettings.PreambleLength = ATOI32((char *)buf);
        */
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