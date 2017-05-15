#include "at_fun.h"
#include "at.h"
#include "at_cmd.h"
#include "sx1278.h"
#include "misc.h"

extern at_stateType  at_state;

uint32_t getPara(char **pPara)
{
    char buf[10];
    int i = 0;
    if(**pPara == ',' || **pPara == '=' )(*pPara)++;//跳过前面非数字区域
    while((**pPara >= '0' && **pPara <= '9') )
    {
        buf[i++] = **pPara;
        (*pPara)++;
    }
    buf[i++] = '\0';
    return  ATOI32((char *)buf,10);
}

void at_CmdNull(char *pPara)
{
    at_backOk;
    at_state = at_statIdle;
}
void at_CmdError()
{
    at_backOk;
    at_state = at_statIdle;
}
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
    }
    else if(*pPara++ == '?')
    {
        at_backOk;
    }
    at_state = at_statIdle;


}
#if USE_REG
void at_CmdReg(char *pPara)
{
    static uint8_t rawValue;
    uint8_t buf[6]={0};
    static uint8_t reg;
    if(*pPara++ == ':')
    reg = getPara(&pPara);

    if(*pPara == '=')
    {
        rawValue=getPara(&pPara);
        SX1278Write(reg,rawValue);
        at_backOk;
    }
    else if(*pPara++ == '?')
    {
        buf[0] = '(';
        buf[1] = '0';
        buf[2] = 'x';
        buf[3] = '0';
        buf[4] = '0';
        buf[5] = ')';
        rawValue = SX1278Read(reg);
        ultoa(rawValue,&buf[3],16);
        buf[5]=')';
        uart1_write(buf,6);
        at_backOk;
    }
    else
    {
        at_backError;
    }
    at_state = at_statIdle;
    
}
#endif
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
    }
    at_state = at_statIdle;

}


void at_CmdPWM1(char *pPara)
{
    uint8_t prescaler;
    uint16_t period;
    uint16_t pulse; 
    if(*pPara++ == '=')
    {
        prescaler = getPara(&pPara);
        period = getPara(&pPara);
        pulse = getPara(&pPara);
        pwm1_config(prescaler,period,pulse);
        at_backOk;
    }
    else
    {
        at_backError;
    }
    at_state = at_statIdle;
}
void at_CmdPWM2(char *pPara)
{

    uint8_t prescaler;
    uint16_t period;
    uint16_t pulse; 
    if(*pPara++ == '=')
    {
        prescaler = getPara(&pPara);
        period = getPara(&pPara);
        pulse = getPara(&pPara);
        pwm2_config(prescaler,period,pulse);
        at_backOk;
    }
    else
    {
        at_backError;
    }
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
        SX1278Init();
        SX1278SetRFState(RFLR_STATE_RX_INIT);
        at_backOk;
    }
    else
    {
        at_backError;
    }
    at_state = at_statIdle;
    
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
void at_CmdSaveConfig(char *pPara)
{
    SaveConfig();
    at_backOk;
    at_state = at_statIdle;
}