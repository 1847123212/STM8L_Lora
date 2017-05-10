#include "at_fun.h"
#include "at.h"
#include "at_cmd.h"

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
    WWDG->CR |= 0x80;//WDGA=1
    WWDG->CR &= 0xbf;//T6ÖÃ0
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
