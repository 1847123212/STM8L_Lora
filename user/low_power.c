#include "low_power.h"
#include "usart.h"
void EXTI_InputPin_Init(GPIO_TypeDef* GPIOx, uint8_t GPIO_Pin)
{
    EXTI_Trigger_TypeDef EXTI_Trigger = EXTI_Trigger_Falling;
 
    GPIO_Init(GPIOx, GPIO_Pin, GPIO_Mode_In_PU_IT);
      

    EXTI->CR2 &=  (uint8_t)(~EXTI_CR2_P4IS);
    EXTI->CR2 |= (uint8_t)((uint8_t)(EXTI_Trigger) << ((uint8_t)EXTI_Pin_4 & (uint8_t)0xEF));

}

void GPIO_LowPower_Config(void)
{


// Port ABCDEFG in output push-pull 0 

 
  //设置S1,S2为中断唤醒引脚，下降沿有效
   EXTI_InputPin_Init(GPIOC, GPIO_Pin_4);
   enableInterrupts();
   
}
void EnterHalt(void)
{
    
    
 /* Set GPIO*/	
   
    GPIO_LowPower_Config();
 
   //CLK_LSICmd(DISABLE);
   //while ((CLK->ICKCR & 0x04) != 0x00);

    uart1_write_string("12");
    
   halt();
}

void ExitHalt(void)
{  
  
   //使用外部时钟
    disableInterrupts();
    CLK->CKDIVR = (uint8_t)(CLK_SYSCLKDiv_1);
    enableInterrupts();
    uart1_write_string("34");

   //  RestorePinSetting();

}
