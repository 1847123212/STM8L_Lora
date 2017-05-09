
#include "ebox.h"
#include "stdio.h"
#include "sx1278.h"
uint8_t buf[8]={'1','2','3','4','5','6','7','8'};

uint8_t data;
uint32_t last;
uint16_t xx;
void main(void)
{
    ebox_init();
    tim4_config();//初始化1ms定时器


    usart_init( USART1, 
                115200,
                USART_WordLength_8b,
                USART_StopBits_1,
                USART_Parity_No,
                (USART_Mode_TypeDef)(USART_Mode_Tx | USART_Mode_Rx));

    SX1278Init();
    gpio_pb0_init();
    //SX1278SetTxPacket(buf,8);
        SX1278SetTxPacket(buf,8);
    while (1)
    {
      //if(DIO0_PIN_READ != 0)
      {
      //      printf("dio0 = %d\n",DIO0_PIN_READ);
      }
      //SX1278Send(buf,8);
      //data = getRegValue(SX1278_REG_FRF_MSB,7,0);
     // GPIO_ToggleBits(GPIOB,GPIO_Pin_0);
      //printf("0x%x\r\n",data);
      //uart1_write("123\r\n",5);
        //delay_ms(1000);
switch( SX1278Process( ) )
{

    case RF_TX_TIMEOUT:
        gpio_pb0_toggle();
      printf("TX TIMEOUT\n");
        SX1278SetTxPacket(buf,8);
        break;    
      case RF_RX_TIMEOUT:
       // SX1278SetTxPacket(buf,8);
        break;
    case RF_RX_DONE:
        break;
    case RF_TX_DONE:
xx = millis() - last;
        printf("time:%d\n",xx);
        SX1278SetTxPacket(buf,8);
        last = millis();
        //SX1276LoRaSetRFState(RFLR_STATE_RX_INIT);
        break;
    default:
        break;
}
    }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
