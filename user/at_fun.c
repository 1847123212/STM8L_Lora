#include "at_fun.h"
#include "at.h"
#include "at_cmd.h"

extern at_stateType  at_state;

void at_exe_cmd_rst()
{
    WWDG->CR |= 0x80;//WDGA=1
    WWDG->CR &= 0xbf;//T6÷√0
    at_backOk;
    at_state = at_statIdle;
}
void at_setup_cmd_pd0(char *pPara)
{
    pPara++;
    if(*pPara == '0')
        gpio_pd0_write(0);
    else
        gpio_pd0_write(1);
    at_backOk;
    at_state = at_statIdle;

}

void at_setup_cmd_pb0(char *pPara)
{
    pPara++;
    if(*pPara == '0')
        gpio_pb0_write(0);
    else
        gpio_pb0_write(1);
    at_backOk;
    at_state = at_statIdle;
}

void at_setup_cmd_pwm1(char *pPara)
{

    at_backOk;
    at_state = at_statIdle;
}

void at_setup_cmd_pwm2(char *pPara)
{

    at_backOk;
    at_state = at_statIdle;
}
void at_exeCmdNull(char *pPara)
{
    at_backOk;
    at_state = at_statIdle;
}
