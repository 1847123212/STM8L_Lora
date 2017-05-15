#include "ebox_pwm.h"

void pwm1_config(uint8_t prescaler,uint16_t period,uint16_t pulse)
{

    CLK->PCKENR1 |= CLK_PCKENR1_TIM2;
    GPIO_ExternalPullUpConfig(GPIOB, GPIO_Pin_0, ENABLE);


    TIM2->ARRH = (uint8_t)(period >> 8);
    TIM2->ARRL = (uint8_t)(period);
    TIM2->PSCR = (uint8_t)(prescaler );
    TIM2->CR1 = 0X80;/*�Զ�װ�أ����ض��룬���ϼ���������������������Ӱ�ӼĴ������رն�ʱ��*/
    
    //TIM2_TimeBaseInit(TIM2_Prescaler_1, TIM2_CounterMode_Up, 1000);
    // TIM2_OC1Init(TIM2_OCMode_PWM1, TIM2_OutputState_Enable, pulse, TIM2_OCPolarity_Low, TIM2_OCIdleState_Set);
    TIM2->CCMR1 |= (uint8_t)TIM2_OCMode_PWM1;

    TIM2->CCER1 |= TIM_CCER1_CC1E;//���ʹ��
    TIM2->CCER1 |= TIM_CCER1_CC1P;//���Կ���
    TIM2->OISR |= TIM_OISR_OIS1;//�������
    /* Set the Pulse value */
    TIM2->CCR1H = (uint8_t)(pulse >> 8);
    TIM2->CCR1L = (uint8_t)(pulse);

    
    TIM2->BKR |= TIM_BKR_MOE ;//output enable
    TIM2->CR1 |= 0X01;/*������ʱ��*/
}
void pwm2_config(uint8_t prescaler,uint16_t period,uint16_t pulse)
{
    CLK->PCKENR1 |= CLK_PCKENR1_TIM3;
    GPIO_ExternalPullUpConfig(GPIOD, GPIO_Pin_0, ENABLE);

    TIM3->ARRH = (uint8_t)(period >> 8);
    TIM3->ARRL = (uint8_t)(period);
    TIM3->PSCR = (uint8_t)(prescaler );
    TIM3->CR1 = 0X80;/*�Զ�װ�أ����ض��룬���ϼ���������������������Ӱ�ӼĴ������رն�ʱ��*/
    
    TIM3->CCMR2 |= (uint8_t)TIM2_OCMode_PWM1;

    TIM3->CCER1 |= TIM_CCER1_CC2E;//���ʹ��
    TIM3->CCER1 |= TIM_CCER1_CC2P;//���Կ���
    TIM3->OISR |= TIM_OISR_OIS1;//�������
    /* Set the Pulse value */
    TIM3->CCR2H = (uint8_t)(pulse >> 8);
    TIM3->CCR2L = (uint8_t)(pulse);
    
    TIM3->BKR |= TIM_BKR_MOE ;//output enable
    TIM3->CR1 |= 0X01;/*������ʱ��*/
}