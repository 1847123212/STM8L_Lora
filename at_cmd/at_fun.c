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
uint32_t getPara16(char **pPara)
{
    char buf[10];
    int i = 0;
    if(**pPara == ',' || **pPara == '=' )(*pPara)++;//跳过前面非数字区域
    while(
          (**pPara >= '0' && **pPara <= '9') ||
          (**pPara >= 'a' && **pPara <= 'f') || 
          (**pPara >= 'A' && **pPara <= 'F')
            )
    {
        buf[i++] = **pPara;
        (*pPara)++;
    }
    buf[i++] = '\0';
    return  ATOI32((char *)buf,16);
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

void at_CmdError()
{
    at_backErrorCode(AT_ERR_CMD);
    at_state = at_statIdle;
}
void at_CmdNull(char *pPara)
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
void at_CmdState(char *pPara)
{

   /*
    RFLR_STATE_RX_TIMEOUT,
    RFLR_STATE_TX_INIT,
    RFLR_STATE_TX_RUNNING,
    ,
    RFLR_STATE_TX_TIMEOUT,
    RFLR_STATE_CAD_INIT,
    RFLR_STATE_CAD_RUNNING,

*/
    uint8_t state;
    if(*pPara == '?')
    {
        state = SX1278GetRFState();
        switch(state)
        {
        case RFLR_STATE_IDLE:
            uart1_write_string("IDLE\r\n");
            break;
        case RFLR_STATE_RX_INIT:
            uart1_write_string("RX_INIT\r\n");
            break;
        case RFLR_STATE_RX_RUNNING:
            uart1_write_string("RX_RUNNING\r\n");
            break;
        case RFLR_STATE_RX_DONE:
            uart1_write_string("RX_DONE\r\n");
            break;
        case RFLR_STATE_TX_DONE:
            uart1_write_string("TX_DONE\r\n");
            break;
        default:
            uart1_write_string("XXX");
            break;
        }
        at_backOk;
    }
    else
    {
        at_backErrorCode(AT_ERR_SYMBLE);
    }
    at_state = at_statIdle;

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
        at_backErrorCode(AT_SYMBLE);
    }
    at_state = at_statIdle;
    
}
#endif
void at_CmdPB0(char *pPara)
{
    CLK->PCKENR1 &= ~CLK_PCKENR1_TIM2;//关闭pwm输出
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
        GPIO_Init(GPIOB,GPIO_Pin_0,GPIO_Mode_In_FL_No_IT);
        if(GPIOB->IDR & GPIO_Pin_0)
        {
            uart1_write_string("1\r\n");
        }
        else
            uart1_write_string("0\r\n");

        at_backOk;
    }
    else
    {
        at_backErrorCode(AT_ERR_SYMBLE);
    }
    at_state = at_statIdle;

}
void at_CmdPC4(char *pPara)
{
    if(*pPara == '=')
    {
        pPara++;
        if(*pPara == '0')
            gpio_pc4_write(0);
        else
            gpio_pc4_write(1);
        at_backOk;
    }
    else if(*pPara == '?')
    {
        GPIO_Init(GPIOC,GPIO_Pin_4,GPIO_Mode_In_FL_No_IT);
        if(GPIOC->IDR & GPIO_Pin_4)
        {
            
            uart1_write_string("1\r\n");
        }
        else
            uart1_write_string("0\r\n");
        at_backOk;
    }
    else
    {
        at_backErrorCode(AT_ERR_SYMBLE);
    }
    at_state = at_statIdle;
}
void at_CmdPD0(char *pPara)
{
    CLK->PCKENR1 &= ~CLK_PCKENR1_TIM3;//关闭定时器PWM输出
    if(*pPara == '=')
    {
        pPara++;
        if(*pPara == '0')
            gpio_pd0_write(0);
        else
            gpio_pd0_write(1);
        at_backOk;
    }
    else if(*pPara == '?')
    {
        GPIO_Init(GPIOD,GPIO_Pin_0,GPIO_Mode_In_FL_No_IT);
        if(GPIOD->IDR & GPIO_Pin_0)
        {
            uart1_write_string("1\r\n");
        }
        else
            uart1_write_string("0\r\n");
        at_backOk;
    }
    else
    {
        at_backErrorCode(AT_ERR_SYMBLE);
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
        at_backErrorCode(AT_ERR_SYMBLE);
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
        at_backErrorCode(AT_ERR_SYMBLE);
    }
    at_state = at_statIdle;
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
        at_backErrorCode(AT_ERR_SYMBLE);
    }
    at_state = at_statIdle;
    
}
void at_CmdSetAddres(char *pPara)
{

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
        if(SX1278GetRFState() != RFLR_STATE_TX_RUNNING)
        {
            SX1278SetTxPacket((uint8_t *)pPara,(len-1));
            at_backOk;
        }
        else
        {
            at_backErrorCode(AT_ERR_RF_BUSY);
        }

    }
    else
    {
        at_backErrorCode(AT_ERR_SYMBLE);
    }
    at_state = at_statIdle;
}
void at_CmdSaveConfig(char *pPara)
{
    SaveConfig();
    at_backOk;
    at_state = at_statIdle;
}
void at_CmdGetRssi(char *pPara)
{
    uart1_write_string("1");
    RxPacketRssiValue;
    at_backOk;
    at_state = at_statIdle;
}
void at_CmdAddr(char *pPara)
{
    uint8_t buf[4];

    if(*pPara == '=')
    {
        LoRaAddr = getPara16(&pPara);
        at_backOk;
    }
    else if(*pPara == '?')
    {
        buf[0] = D2C((LoRaAddr&0xF000) >> 12);
        buf[1] = D2C((LoRaAddr&0x0F00) >> 8);
        buf[2] = D2C((LoRaAddr&0x00F0) >> 4);
        buf[3] = D2C((LoRaAddr&0x000F) >> 0);
        
        uart1_write((uint8_t*)buf,4);
        uart1_write_string("\r\n");
        at_backOk;
    }
    else
    {
        at_backErrorCode(AT_ERR_SYMBLE);
    }
    at_state = at_statIdle;
}