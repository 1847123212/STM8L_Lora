
#include "ebox.h"
#include "radio.h"

tRadioDriver *Radio;


void main(void)
{
    ebox_init();
    tim4_config();//初始化1ms定时器

    /*High speed internal clock prescaler: 1*/

  
    /* EVAL COM (USARTx) configuration -----------------------------------------*/

    usart_init( USART1, 
                115200,
                USART_WordLength_8b,
                USART_StopBits_1,
                USART_Parity_No,
                (USART_Mode_TypeDef)(USART_Mode_Tx | USART_Mode_Rx));
    pwm_config(1000,500);
    Radio = RadioDriverInit( );

    
    while (1)
    {
        delay_ms(100);
    }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
