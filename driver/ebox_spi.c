#include "ebox_spi.h"

void spi_config()
{
    /* Enable SPI clock */
    CLK_PeripheralClockConfig((CLK_Peripheral_TypeDef)CLK_Peripheral_SPI1, ENABLE);
    
    //PB5->SCK,PB6->MOSI,PB7->MISO
    /* Set the MOSI,MISO and SCK at high level */
    GPIO_ExternalPullUpConfig(GPIOB, GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7, ENABLE);    


    /* SPI configuration */
    SPI_Init(   SPI1, 
                SPI_FirstBit_MSB, 
                SPI_BaudRatePrescaler_4,
                SPI_Mode_Master,
                SPI_CPOL_High, 
                SPI_CPHA_2Edge, 
                SPI_Direction_2Lines_FullDuplex,
                SPI_NSS_Soft, 
                0x07);
    
    SPI_Cmd(SPI1,ENABLE);

    /* Configure LORA_CS as Output push-pull, used as Flash Chip select */
    GPIO_Init(GPIOB, GPIO_Pin_4, GPIO_Mode_Out_PP_High_Slow);
}
