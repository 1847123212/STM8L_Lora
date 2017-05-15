#include "usart.h"

void usart_init(USART_TypeDef* USARTx,
                uint32_t BaudRate,
                USART_WordLength_TypeDef
                USART_WordLength,
                USART_StopBits_TypeDef USART_StopBits,
                USART_Parity_TypeDef USART_Parity,
                USART_Mode_TypeDef USART_Mode)
{
  uint32_t BaudRate_Mantissa = 0;

  //CLK_PeripheralClockConfig((CLK_Peripheral_TypeDef)CLK_Peripheral_USART1, ENABLE);
    CLK->PCKENR1 |= CLK_PCKENR1_USART1;//

  /* Configure USART Tx as alternate function push-pull  (software pull up)*/
  GPIO_ExternalPullUpConfig(GPIOC, GPIO_Pin_3, ENABLE);

  /* Configure USART Rx as alternate function push-pull  (software pull up)*/
  GPIO_ExternalPullUpConfig(GPIOC, GPIO_Pin_2, ENABLE);

  /* USART configuration 
  USART_Init(USARTx, 
             BaudRate,
             USART_WordLength,
             USART_StopBits,
             USART_Parity,
             USART_Mode);*/
  
  /* Clear the word length and Parity Control bits */
  USARTx->CR1 &= (uint8_t)(~(USART_CR1_PCEN | USART_CR1_PS | USART_CR1_M));
  /* Set the word length bit according to USART_WordLength value */
  /* Set the Parity Control bit to USART_Parity value */
  USARTx->CR1 |= (uint8_t)((uint8_t)USART_WordLength | (uint8_t)USART_Parity);
  /* Clear the STOP bits */
  USARTx->CR3 &= (uint8_t)(~USART_CR3_STOP);
  /* Set the STOP bits number according to USART_StopBits value */
  USARTx->CR3 |= (uint8_t)USART_StopBits;
  
  /* Clear the LSB mantissa of USARTDIV */
  USARTx->BRR1 &= (uint8_t)(~USART_BRR1_DIVM);
  /* Clear the MSB mantissa of USARTDIV */
  USARTx->BRR2 &= (uint8_t)(~USART_BRR2_DIVM);
  /* Clear the Fraction bits of USARTDIV */
  USARTx->BRR2 &= (uint8_t)(~USART_BRR2_DIVF);
  
  BaudRate_Mantissa  = (uint32_t)(CLK_GetClockFreq() / BaudRate );
  /* Set the fraction of USARTDIV */
  USARTx->BRR2 = (uint8_t)((BaudRate_Mantissa >> (uint8_t)8) & (uint8_t)0xF0);
  /* Set the MSB mantissa of USARTDIV */
  USARTx->BRR2 |= (uint8_t)(BaudRate_Mantissa & (uint8_t)0x0F);
  /* Set the LSB mantissa of USARTDIV */
  USARTx->BRR1 = (uint8_t)(BaudRate_Mantissa >> (uint8_t)4);

  /* Disable the Transmitter and Receiver */
  USARTx->CR2 &= (uint8_t)~(USART_CR2_TEN | USART_CR2_REN);
  /* Set TEN and REN bits according to USART_Mode value */
  USARTx->CR2 |= (uint8_t)USART_Mode;
  
  enableInterrupts();

  /* Enable the USART Receive interrupt: this interrupt is generated when the USART
    receive data register is not empty */
  //USART_ITConfig(USARTx, USART_IT_RXNE, ENABLE);
    USARTx->CR2 |= 0X20;

  /* Enable the USART Transmit complete interrupt: this interrupt is generated when the USART
    transmit Shift Register is empty */
  //USART_ITConfig(USARTx, USART_IT_TC, ENABLE);

  /* Enable USART */
    //USART_Cmd(USARTx, ENABLE);
    USARTx->CR1 &= (uint8_t)(0XDF); /**< USART Enable */
}
void uart1_write(uint8_t *buf,uint16_t len)
{

    while(len--)
    {
        //USART_SendData8(USART1, *buf++);
          USART1->DR = *buf++;
        while ((USART1->SR & (uint8_t)USART_FLAG_TC) != (uint8_t)0x00);
    }
}
void uart1_write_string(uint8_t *buf)
{

    while(*buf != '\0')
    {
                  USART1->DR = *buf++;
//USART_SendData8(USART1, *buf++);
        while ((USART1->SR & (uint8_t)USART_FLAG_TC) != (uint8_t)0x00);
    }
}
void uart1_write_cstring(const uint8_t *buf)
{

    while(*buf != '\0')
    {
        //USART_SendData8(USART1, *buf++);
        //while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
        USART1->DR = *buf++;
        while ((USART1->SR & (uint8_t)USART_FLAG_TC) != (uint8_t)0x00);
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
  //USART_SendData8(USART1, c);
  /* Loop until the end of transmission */
  //while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
    USART1->DR = c;
    while ((USART1->SR & (uint8_t)USART_FLAG_TC) != (uint8_t)0x00);

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
  //while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
    while ((USART1->SR & (uint8_t)USART_FLAG_RXNE) != (uint8_t)0x00);
    //c = USART_ReceiveData8(USART1);
    c = USART1->DR;
    return (c);
  }