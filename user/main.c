
#include "ebox.h"
#include "stdio.h"
#include "sx1278.h"
#include "at_cmd.h"
#include "at_port.h"
uint8_t buf[8]={'1','2','3','4','5','6','7','8'};
uint8_t buffer[8];
uint8_t bufferLength;
extern uint8_t RFLRState;
uint8_t masterOn = 0;
const uint8_t info[]="MODULE:LoRa-King\nVendor:eBox&Widora\nVersion:V0.1\nWeb:www.widora.org\n";
void master()
{
    switch( SX1278Process( ) )
    {

    case RF_TX_DONE:
        //RFLRState = RFLR_STATE_RX_INIT;
      //delay_ms(500);
        SX1278SetTxPacket(buf,8);
        //printf("tx time:%ld\n",TxPacketTime);
        //SX1276LoRaSetRFState(RFLR_STATE_RX_INIT);
        break;
    default:
        break;
    }
}
void slave()
{
    switch( SX1278Process( ) )
    {
    case RF_RX_TIMEOUT:
        //printf("rx time:%ld\n",RxPacketTime);
        SX1278SetRFState(RFLR_STATE_RX_INIT);
        gpio_pb0_toggle();
        break;
    case RF_RX_DONE:
        SX1278GetRxPacket(buffer,&bufferLength);
        //uart1_write_string("+IPD");
        uart1_write(buffer,bufferLength);
        
        memset(buffer,0,8);
        SX1278SetRFState(RFLR_STATE_RX_INIT);
        //SX1278SetTxPacket(buf,8);
        gpio_pb0_toggle();
        break;
    case RF_TX_DONE:
        //RFLRState = RFLR_STATE_RX_INIT;
        SX1278SetRFState(RFLR_STATE_RX_INIT);
        //uart1_write_string("TX done\n");
        //SX1276LoRaSetRFState(RFLR_STATE_RX_INIT);
        break;
    default:
        break;
    }
}
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
    uart1_write_string("a");
    uart1_write_cstring(info);
    SX1278Init();
    gpio_pb0_init();
    gpio_pd0_init();
    gpio_pc4_init();
    gpio_pc4_write(1);
    pwm1_config(0,59999,3999);
    pwm2_config(0,39999,19999);
    //SX1278SetTxPacket(buf,8);
        if(masterOn == 1)
        {
                SX1278SetTxPacket(buf,8);

        }
        else
        {
                SX1278SetRFState(RFLR_STATE_RX_INIT);
        }
          
    while (1)
    {
      if(masterOn == 1)
      {
        master();
      }
      else
      {
        slave();
      }
      
      at_process_loop();

    }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
