
#include "ebox.h"
#include "stdio.h"
#include "sx1278.h"
#include "at_cmd.h"
#include "at_port.h"
#include "low_power.h"

uint8_t halt_mode = 0;
const uint8_t info[]="MODULE:XLoRa-01(V0.1)\r\nVendor:eBox&Widora\r\n";

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
    uart1_write_cstring(info);
    SX1278Init();

    while (1)
    {
        switch( SX1278Process( ) )
        {
        case RF_IDLE:
            break;
        case RF_RX_TIMEOUT:
            uart1_write_string("TimeOut\r\n");
            //printf("rx time:%ld\n",RxPacketTime);
            //SX1278SetRFState(RFLR_STATE_RX_INIT);
            //gpio_pb0_toggle();
            break;
        case RF_RX_DONE:
            SX1278ForwardPacket();
            //uart1_write_string("+IPD");
            //uart1_write(buffer,bufferLength);
            
            SX1278SetRFState(RFLR_STATE_RX_INIT);
            //SX1278SetTxPacket(buf,8);
           // gpio_pb0_toggle();
            break;
        case RF_TX_DONE:
            //RFLRState = RFLR_STATE_RX_INIT;
            SX1278SetRFState(RFLR_STATE_RX_INIT);
            //uart1_write_string("TX done\n");
            //SX1276LoRaSetRFState(RFLR_STATE_RX_INIT);
            uart1_write_string("OK\r\n");

            break;
        default:
            break;
        }
      
      at_process_loop();

    }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
