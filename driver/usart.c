#include "usart.h"

void usart_init(USART_TypeDef* USARTx,
                uint32_t BaudRate,
                USART_WordLength_TypeDef
                USART_WordLength,
                USART_StopBits_TypeDef USART_StopBits,
                USART_Parity_TypeDef USART_Parity,
                USART_Mode_TypeDef USART_Mode)
{

  CLK_PeripheralClockConfig((CLK_Peripheral_TypeDef)CLK_Peripheral_USART1, ENABLE);

  /* Configure USART Tx as alternate function push-pull  (software pull up)*/
  GPIO_ExternalPullUpConfig(GPIOC, GPIO_Pin_3, ENABLE);

  /* Configure USART Rx as alternate function push-pull  (software pull up)*/
  GPIO_ExternalPullUpConfig(GPIOC, GPIO_Pin_2, ENABLE);

  /* USART configuration */
  USART_Init(USARTx, 
             BaudRate,
             USART_WordLength,
             USART_StopBits,
             USART_Parity,
             USART_Mode);
    enableInterrupts();

  /* Enable the USART Receive interrupt: this interrupt is generated when the USART
    receive data register is not empty */
  USART_ITConfig(USARTx, USART_IT_RXNE, ENABLE);
  /* Enable the USART Transmit complete interrupt: this interrupt is generated when the USART
    transmit Shift Register is empty */
  USART_ITConfig(USARTx, USART_IT_TC, ENABLE);

  /* Enable USART */
  USART_Cmd(USARTx, ENABLE);
}
void uart1_write(uint8_t *buf,uint16_t len)
{

    while(len--)
    {
        USART_SendData8(USART1, *buf++);
        while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
    }
}
void uart1_write_string(uint8_t *buf)
{

    while(*buf != '\0')
    {
        USART_SendData8(USART1, *buf++);
        while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
    }
}
uint8_t uart1_read()
{
    return USART1->DR;
}
/**
  * @brief Retargets the C library printf function to the USART.
  * @param[in] c Character to send
  * @retval char Character sent
  * @par Required preconditions:
  * - None
  */
int putchar (int c)
{
  /* Write a character to the USART */
  USART_SendData8(USART1, c);
  /* Loop until the end of transmission */
  while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);

  return (c);
}
/**
  * @brief Retargets the C library scanf function to the USART.
  * @param[in] None
  * @retval char Character to Read
  * @par Required preconditions:
  * - None
  */
int getchar (void)
{
  int c = 0;
  /* Loop until the Read data register flag is SET */
  while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
    c = USART_ReceiveData8(USART1);
    return (c);
  }