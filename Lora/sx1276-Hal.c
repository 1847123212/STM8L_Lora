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
#include "platform.h"
#include "sx1276-Hal.h"


#if defined( USE_SX1276_RADIO )



#define RST_PIN_RESET   GPIO_WriteBit(GPIOA,GPIO_Pin_2,RESET)
#define NSS_PIN_RESET   GPIO_WriteBit(GPIOB,GPIO_Pin_4,RESET)
#define DIO0_PIN_RESET  GPIO_WriteBit(GPIOA,GPIO_Pin_3,RESET)
#define DIO1_PIN_RESET  GPIO_WriteBit(GPIOB,GPIO_Pin_1,RESET)
#define DIO2_PIN_RESET  GPIO_WriteBit(GPIOB,GPIO_Pin_2,RESET)
#define DIO3_PIN_RESET  GPIO_WriteBit(GPIOB,GPIO_Pin_3,RESET)
#define DIO4_PIN_RESET  GPIO_WriteBit(GPIOC,GPIO_Pin_0,RESET)
#define DIO5_PIN_RESET  GPIO_WriteBit(GPIOC,GPIO_Pin_1,RESET)

#define RST_PIN_SET     GPIO_WriteBit(GPIOA,GPIO_Pin_2,SET)
#define NSS_PIN_SET     GPIO_WriteBit(GPIOB,GPIO_Pin_4,SET)
#define DIO0_PIN_SET    GPIO_WriteBit(GPIOA,GPIO_Pin_3,SET)
#define DIO1_PIN_SET    GPIO_WriteBit(GPIOB,GPIO_Pin_1,SET)
#define DIO2_PIN_SET    GPIO_WriteBit(GPIOB,GPIO_Pin_2,SET)
#define DIO3_PIN_SET    GPIO_WriteBit(GPIOB,GPIO_Pin_3,SET)
#define DIO4_PIN_SET    GPIO_WriteBit(GPIOC,GPIO_Pin_0,SET)
#define DIO5_PIN_SET    GPIO_WriteBit(GPIOC,GPIO_Pin_1,SET)

#define DIO0_PIN_READ   GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_3)
#define DIO1_PIN_READ   GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1)
#define DIO2_PIN_READ   GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_2)
#define DIO3_PIN_READ   GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_3)
#define DIO4_PIN_READ   GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_0)
#define DIO5_PIN_READ   GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_1)



void SX1276InitIo( void )
{
    
    // Configure NSS as output
    //NSS_PIN.mode(OUTPUT_PP);

    
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

void SX1276SetReset( uint8_t state )
{

    if( state == RADIO_RESET_ON )
    {
        // Set RESET pin to 0
        //RST_PIN.mode(OUTPUT_PP);
        //RST_PIN.reset();
        GPIO_Init(GPIOA, GPIO_Pin_2, GPIO_Mode_Out_PP_Low_Slow);
        GPIO_WriteBit(GPIOA,GPIO_Pin_2,RESET);
    }
    else
    {
        // Set RESET pin as input pull down
        //RST_PIN.mode(INPUT_PD);     
        GPIO_Init(GPIOA, GPIO_Pin_2, GPIO_Mode_In_FL_No_IT);
    }
}

void SX1276Write( uint8_t addr, uint8_t data )
{
    SX1276WriteBuffer( addr, &data, 1 );
}

void SX1276Read( uint8_t addr, uint8_t *data )
{
    SX1276ReadBuffer( addr, data, 1 );
}

void SX1276WriteBuffer( uint8_t addr, uint8_t *buffer, uint8_t size )
{
    uint8_t i;

    //NSS = 0;
    //spi1.transfer( addr | 0x80 );
    NSS_PIN_RESET;
    spi1_trans( addr | 0x80);
    for( i = 0; i < size; i++ )
    {
        //spi1.transfer( buffer[i] );
        spi1_trans( buffer[i]) ;
   }
    //NSS = 1;
    NSS_PIN_SET;

}

void SX1276ReadBuffer( uint8_t addr, uint8_t *buffer, uint8_t size )
{
    uint8_t i;

    //NSS = 0;
    NSS_PIN_RESET;
    spi1_trans(addr & 0x7F);
    for( i = 0; i < size; i++ )
    {
        buffer[i] = spi1_trans( 0xff );
    }
    //NSS = 1;
    NSS_PIN_SET;
}

void SX1276WriteFifo( uint8_t *buffer, uint8_t size )
{
    SX1276WriteBuffer( 0, buffer, size );
}

void SX1276ReadFifo( uint8_t *buffer, uint8_t size )
{
    SX1276ReadBuffer( 0, buffer, size );
}

inline uint8_t SX1276ReadDio0( void )
{
    return DIO0_PIN_READ;
}

inline uint8_t SX1276ReadDio1( void )
{
    return DIO1_PIN_READ;
}

inline uint8_t SX1276ReadDio2( void )
{
    return DIO2_PIN_READ;
}

inline uint8_t SX1276ReadDio3( void )
{
    return DIO3_PIN_READ;
}

inline uint8_t SX1276ReadDio4( void )
{
    return DIO4_PIN_READ;
}

inline uint8_t SX1276ReadDio5( void )
{
    return DIO5_PIN_READ;
}

inline void SX1276WriteRxTx( uint8_t txEnable )
{
    if( txEnable != 0 )
    {
//        IoePinOn( FEM_CTX_PIN );
//        IoePinOff( FEM_CPS_PIN );
    }
    else
    {
//        IoePinOff( FEM_CTX_PIN );
//        IoePinOn( FEM_CPS_PIN );
    }
}

#endif // USE_SX1276_RADIO
