#include "gpio.h"

void gpio_pd0_init()
{
    GPIO_Init(GPIOD,GPIO_Pin_0,GPIO_Mode_Out_PP_Low_Fast);
}

void gpio_pd0_write(uint8_t GPIO_BitVal)
{
    GPIO_Init(GPIOD,GPIO_Pin_0,GPIO_Mode_Out_PP_Low_Fast);  
    if (GPIO_BitVal != RESET)
    {
        GPIOD->ODR |= GPIO_Pin_0;

    }
    else
    {
        GPIOD->ODR &= (uint8_t)(~GPIO_Pin_0);
    }
}

void gpio_pb0_init()
{
      GPIO_Init(GPIOB,GPIO_Pin_0,GPIO_Mode_Out_PP_Low_Fast);
}

void gpio_pb0_write(uint8_t GPIO_BitVal)
{
    GPIO_Init(GPIOB,GPIO_Pin_0,GPIO_Mode_Out_PP_Low_Fast);
    if (GPIO_BitVal != RESET)
    {
        GPIOB->ODR |= GPIO_Pin_0;
    }
    else
    {
        GPIOB->ODR &= (uint8_t)(~GPIO_Pin_0);
    }
}
void gpio_pb0_toggle()
{
    GPIO_ToggleBits(GPIOB,GPIO_Pin_0);
}