#include "at_fun.h"
#include "at.h"
#include "at_cmd.h"
#include "sx1278.h"
#include "misc.h"

extern at_stateType  at_state;


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
        GPIOB->DDR |= GPIO_Pin_0;//OUTPUT
        GPIOB->CR1 |= GPIO_Pin_0;//PP
        //GPIOB->CR2 |= GPIO_Pin_0;//低速模式,默认值
        pPara++;
        if(*pPara == '0')
            GPIOB->ODR &= (uint8_t)(~GPIO_Pin_0);
        else
            GPIOB->ODR |= GPIO_Pin_0;
        at_backOk;
    }
    else if(*pPara == '?')
    {
        GPIOB->DDR &= (uint8_t)(~(GPIO_Pin_0));//INPUT
        GPIOB->CR1 &= (uint8_t)(~(GPIO_Pin_0));//FLOAT
        GPIOB->CR2 &= (uint8_t)(~(GPIO_Pin_0));//无中断,默认值
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
        GPIOC->DDR |= GPIO_Pin_4;//OUTPUT
        GPIOC->CR1 |= GPIO_Pin_4;//PP
        //GPIOC->CR2 |= GPIO_Pin_4;//低速模式,默认值

        pPara++;
        if(*pPara == '0')
            GPIOC->ODR &= (uint8_t)(~GPIO_Pin_4);
        else
            GPIOC->ODR |= GPIO_Pin_4;
        at_backOk;
    }
    else if(*pPara == '?')
    {
        GPIOC->DDR &= (uint8_t)(~(GPIO_Pin_4));//INPUT
        GPIOC->CR1 &= (uint8_t)(~(GPIO_Pin_4));//FLOAT
        GPIOC->CR2 &= (uint8_t)(~(GPIO_Pin_4));//无中断,默认值

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
        GPIOD->DDR |= GPIO_Pin_0;//OUTPUT
        GPIOD->CR1 |= GPIO_Pin_0;//PP
        pPara++;
        if(*pPara == '0')
            GPIOD->ODR &= (uint8_t)(~GPIO_Pin_0);
        else
            GPIOD->ODR |= GPIO_Pin_0;
        at_backOk;
    }
    else if(*pPara == '?')
    {
        GPIOD->DDR &= (uint8_t)(~(GPIO_Pin_0));//INPUT
        GPIOD->CR1 &= (uint8_t)(~(GPIO_Pin_0));//FLOAT
        GPIOD->CR2 &= (uint8_t)(~(GPIO_Pin_0));//无中断,默认值
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
        prescaler = getPara(&pPara,10);
        period = getPara(&pPara,10);
        pulse = getPara(&pPara,10);
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
        prescaler = getPara(&pPara,10);
        period = getPara(&pPara,10);
        pulse = getPara(&pPara,10);
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
#if USE_READ_STATE
    uint8_t buf[8];
    uint8_t len;
#endif

    if(*pPara == '=')
    {
        pPara++;
        if(CheckPara(pPara) == FALSE) 
        {
            at_backErr;
            at_state = at_statIdle;
            return ;
        }
        LoRaSettings.RFFrequency = getPara(&pPara,10);
        LoRaSettings.Power = getPara(&pPara,10);
        LoRaSettings.SignalBw = getPara(&pPara,10);
        LoRaSettings.SpreadingFactor = getPara(&pPara,10);
        LoRaSettings.ErrorCoding = getPara(&pPara,10);
        LoRaSettings.CrcOn = (bool)getPara(&pPara,10);
        LoRaSettings.ImplicitHeaderOn = (bool)getPara(&pPara,10);
        LoRaSettings.RxSingleOn = (bool)getPara(&pPara,10);
        LoRaSettings.FreqHopOn = (bool)getPara(&pPara,10);
        LoRaSettings.HopPeriod = getPara(&pPara,10);
        LoRaSettings.RxPacketTimeout = getPara(&pPara,10);
        LoRaSettings.PayloadLength = getPara(&pPara,10);
        LoRaSettings.PreambleLength = getPara(&pPara,10);
        SX1278Init();
        SX1278SetRFState(RFLR_STATE_RX_INIT);
        at_backOk;
    }
    else if(*pPara == '?')
    {
#if USE_READ_STATE
        len = digital2HexString(LoRaSettings.RFFrequency,buf);
        uart1_write((uint8_t*)buf,len);
        uart1_write_string(",");
        len = digital2HexString(LoRaSettings.Power,buf);
        uart1_write((uint8_t*)buf,len);
        uart1_write_string(",");
        len = digital2HexString(LoRaSettings.SignalBw,buf);
        uart1_write((uint8_t*)buf,len);
        uart1_write_string(",");
        len = digital2HexString(LoRaSettings.SpreadingFactor,buf);
        uart1_write((uint8_t*)buf,len);
        uart1_write_string(",");
        len = digital2HexString(LoRaSettings.ErrorCoding,buf);
        uart1_write((uint8_t*)buf,len);
        uart1_write_string(",");
        len = digital2HexString(LoRaSettings.CrcOn,buf);
        uart1_write((uint8_t*)buf,len);
        uart1_write_string(",");
        len = digital2HexString(LoRaSettings.ImplicitHeaderOn,buf);
        uart1_write((uint8_t*)buf,len);
        uart1_write_string(",");
        len = digital2HexString(LoRaSettings.RxSingleOn,buf);
        uart1_write((uint8_t*)buf,len);
        uart1_write_string(",");
        len = digital2HexString(LoRaSettings.HopPeriod,buf);
        uart1_write((uint8_t*)buf,len);
        uart1_write_string(",");
        len = digital2HexString(LoRaSettings.RxPacketTimeout,buf);
        uart1_write((uint8_t*)buf,len);
        uart1_write_string(",");        
        len = digital2HexString(LoRaSettings.PayloadLength,buf);
        uart1_write((uint8_t*)buf,len);
        uart1_write_string(",");        
        len = digital2HexString(LoRaSettings.PreambleLength,buf);
        uart1_write((uint8_t*)buf,len);
        uart1_write_string("\r\n");
        at_backOk;
#else
        at_backErr;
#endif
        
        
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
        len -=2;//删除\r\n
        if(SX1278GetRFState() != RFLR_STATE_TX_RUNNING)
        {
            LoRaPacket.source.val = LoRaAddr;
            LoRaPacket.destination.val = LoRaAddr;
            LoRaPacket.data = (uint8_t *)pPara;
            LoRaPacket.len = len+4;
            

            SX1278SetTxPacket1(&LoRaPacket);
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
    uint8_t buf[8];
    uint8_t len;

    if(*pPara == '=')
    {
        LoRaAddr = 0;
        /*
        *pPara++;
        
        LoRaAddr |= (C2D(*pPara++) ) << 12;
        LoRaAddr |= (C2D(*pPara++) ) << 8;
        LoRaAddr |= (C2D(*pPara++) ) << 4;
        LoRaAddr |= (C2D(*pPara++) ) << 0;
        */  
        LoRaAddr = getPara(&pPara,16);
        at_backOk;
    }
    else if(*pPara == '?')
    {
        
        buf[0] = D2C((LoRaAddr&0xF000) >> 12);
        buf[1] = D2C((LoRaAddr&0x0F00) >> 8);
        buf[2] = D2C((LoRaAddr&0x00F0) >> 4);
        buf[3] = D2C((LoRaAddr&0x000F) >> 0);
        
        //len = digital2HexString(LoRaAddr,buf);
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