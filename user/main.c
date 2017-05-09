
#include "ebox.h"
#include "stdio.h"
#include "sx1278.h"
uint8_t buf[8]={'1','2','3','4','5','6','7','8'};

extern uint8_t RFLRState;

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
        //SX1278SetTxPacket(buf,8);
        SX1278SetRFState(RFLR_STATE_RX_INIT);
    while (1)
    {
        switch( SX1278Process( ) )
        {
        case RF_TX_TIMEOUT:
            gpio_pb0_toggle();
            printf("tx time:%ld\n",TxPacketTime);
            SX1278SetTxPacket(buf,8);
            break;    
          case RF_RX_TIMEOUT:
            printf("rx time:%ld\n",RxPacketTime);
            SX1278SetTxPacket(buf,8);
            break;
        case RF_RX_DONE:
            break;
        case RF_TX_DONE:
            //RFLRState = RFLR_STATE_RX_INIT;
            SX1278SetRFState(RFLR_STATE_RX_INIT);
            printf("tx time:%ld\n",TxPacketTime);
            //SX1276LoRaSetRFState(RFLR_STATE_RX_INIT);
            break;
        default:
            break;
        }
    }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
