
#include "ebox.h"
#include "radio.h"
#include "sx1276-lora.h"
#include "sx1276-hal.h"
#include "stdio.h"
tRadioDriver *Radio;

uint8_t data;
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
    
    //pwm_config(1000,500);
    Radio = RadioDriverInit( );
    Radio->Init( );
   // Radio->StartRx( );
    
    SX1276InitIo();
    SX1276Reset( );

  //      GPIO_Init(GPIOB,GPIO_Pin_0,GPIO_Mode_Out_PP_Low_Fast);  

    while (1)
    {
     // SX1276Write(REG_LR_FRFMSB,data);
      SX1276Read(REG_LR_FRFMSB,&data);

      GPIO_ToggleBits(GPIOB,GPIO_Pin_0);
    //  printf("0x%x\r\n",data);
        delay_ms(100);
    }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
