/*
 * THE FOLLOWING FIRMWARE IS PROVIDED: (1) "AS IS" WITH NO WARRANTY; AND 
 * (2)TO ENABLE ACCESS TO CODING INFORMATION TO GUIDE AND FACILITATE CUSTOMER.
 * CONSEQUENTLY, SEMTECH SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT OR
 * CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT
 * OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION
 * CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 * 
 * Copyright (C) SEMTECH S.A.
 */
/*! 
 * \file       sx1276-Hal.c
 * \brief      SX1276 Hardware Abstraction Layer
 *
 * \version    2.0.B2 
 * \date       Nov 21 2012
 * \author     Miguel Luis
 *
 * Last modified by Miguel Luis on Jun 19 2013
 */
#include "ebox.h"
#include "sx1278-Hal.h"



#define RST_PIN_RESET   (GPIOA->ODR &= B11111011)
#define NSS_PIN_RESET   (GPIOB->ODR &= B11101111)
#define DIO0_PIN_RESET  (GPIOA->ODR &= B11110111)
#define DIO1_PIN_RESET  (GPIOB->ODR &= B11111101)
#define DIO2_PIN_RESET  (GPIOB->ODR &= B11111011)
#define DIO3_PIN_RESET  (GPIOB->ODR &= B11110111)
#define DIO4_PIN_RESET  (GPIOC->ODR &= B11111110)
#define DIO5_PIN_RESET  (GPIOC->ODR &= B11111101)

#define RST_PIN_SET     (GPIOA->ODR |= GPIO_Pin_2)
#define NSS_PIN_SET     (GPIOB->ODR |= GPIO_Pin_4)
#define DIO0_PIN_SET    (GPIOA->ODR |= GPIO_Pin_3)
#define DIO1_PIN_SET    (GPIOB->ODR |= GPIO_Pin_1)
#define DIO2_PIN_SET    (GPIOB->ODR |= GPIO_Pin_2)
#define DIO3_PIN_SET    (GPIOB->ODR |= GPIO_Pin_3)
#define DIO4_PIN_SET    (GPIOC->ODR |= GPIO_Pin_0)
#define DIO5_PIN_SET    (GPIOC->ODR |= GPIO_Pin_1)




void SX1278InitIo( void )
{
    // Configure RESET PIN as output    
    GPIO_Init(GPIOA, GPIO_Pin_2, GPIO_Mode_Out_PP_Low_Slow);
    
    // Configure DIO0
    //DIO0_PIN.mode(INPUT);
    GPIO_Init(GPIOA, GPIO_Pin_3, GPIO_Mode_In_FL_No_IT);

    // Configure DIO1
    GPIO_Init(GPIOB, GPIO_Pin_1, GPIO_Mode_In_FL_No_IT);

    // Configure DIO2
    GPIO_Init(GPIOB, GPIO_Pin_2, GPIO_Mode_In_FL_No_IT);

    // REAMARK: DIO3/4/5 configured are connected to IO expander

    // Configure DIO3 as input
    GPIO_Init(GPIOB, GPIO_Pin_3, GPIO_Mode_In_FL_No_IT);

    // Configure DIO4 as input
    
    // Configure DIO5 as input
    
    //init spi
    spi_config();
}


void SX1278Reset()
{
    RST_PIN_RESET;
    delay_ms(1);
    RST_PIN_SET;
    delay_ms(6);
}
void SX1278SetOpMode(uint8_t mode)
{
  SX1278WriteBits(SX1278_REG_OP_MODE, mode, 2, 0);
}

void SX1278ClearIRQFlags(uint8_t IrqFlagMask) 
{
  uint8_t NewFlags;
  NewFlags =   SX1278Read(SX1278_REG_IRQ_FLAGS) | IrqFlagMask;
  SX1278Write(SX1278_REG_IRQ_FLAGS,NewFlags);
}
uint8_t SX1278WriteBits(uint8_t reg, uint8_t value, uint8_t msb, uint8_t lsb) 
{
    uint8_t currentValue;
    uint8_t newValue;
    if((msb > 7) || (lsb > 7)) {
        return 0xFF;
    }
    currentValue = SX1278Read(reg);
    newValue = currentValue & ((0xff << (msb + 1)) | (0xff >> (8 - lsb)));
    SX1278Write(reg, newValue | value);
    return 0;
}

uint8_t SX1278ReadBits(uint8_t reg, uint8_t msb, uint8_t lsb) {
    uint8_t rawValue;
    uint8_t maskedValue;
    if((msb > 7) || (lsb > 7)) {
        return 0xFF;
    }
    rawValue = SX1278Read(reg);
    maskedValue = rawValue & ((0xff << lsb) & (0xff >> (7 - msb)));
    return(maskedValue);
}
uint8_t SX1278Write( uint8_t addr, uint8_t data )
{
    SX1278WriteBuffer( addr, &data, 1 );
    return data;
}

uint8_t SX1278Read( uint8_t addr)
{
    uint8_t data;
    SX1278ReadBuffer( addr, &data, 1 );
    return data;
}

void SX1278WriteBuffer( uint8_t addr, uint8_t *buffer, uint8_t size )
{
    uint8_t i;

    //NSS = 0;
    //spi1.transfer( addr | 0x80 );
    NSS_PIN_RESET;
    spi1_transfer( addr | 0x80);
    for( i = 0; i < size; i++ )
    {
        spi1_transfer( buffer[i]) ;
   }
    //NSS = 1;
    NSS_PIN_SET;

}

void SX1278ReadBuffer( uint8_t addr, uint8_t *buffer, uint8_t size )
{
    uint8_t i;

    //NSS = 0;
    NSS_PIN_RESET;
    spi1_transfer(addr & 0x7F);
    for( i = 0; i < size; i++ )
    {
        buffer[i] = spi1_transfer( 0xff );
    }
    //NSS = 1;
    NSS_PIN_SET;
}



/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////

