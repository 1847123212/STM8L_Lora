#include "sx1278.h"

#define RF_BUFFER_SIZE 256


static uint32_t LastTime = 0;


// Default settings
tLoRaSettings LoRaSettings =
{
    434000000,        // RFFrequency
    20,               // Power
    6,                // SignalBw [0: 7.8kHz, 1: 10.4 kHz, 2: 15.6 kHz, 3: 20.8 kHz, 4: 31.2 kHz,
                      // 5: 41.6 kHz, 6: 62.5 kHz, 7: 125 kHz, 8: 250 kHz, 9: 500 kHz, other: Reserved]
    7,                // SpreadingFactor [6: 64, 7: 128, 8: 256, 9: 512, 10: 1024, 11: 2048, 12: 4096  chips]
    1,                // ErrorCoding [1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8]
    TRUE,             // CrcOn [0: OFF, 1: ON]
    FALSE,            // ImplicitHeaderOn [0: OFF, 1: ON]
    TRUE,                // RxSingleOn [0: Continuous, 1 Single]
    FALSE,                // FreqHopOn [0: OFF, 1: ON]
    4,                // HopPeriod Hops every frequency hopping period symbols
    100,              // TxPacketTimeout
    100,              // RxPacketTimeout
    8,              // PayloadLength (used for implicit header mode)
};
/*!
 * Local RF buffer for communication support
 */
static uint8_t RFBuffer[RF_BUFFER_SIZE];

/*!
 * RF state machine variable
 */
static uint8_t RFLRState = RFLR_STATE_IDLE;
/*!
 * Rx management support variables
 */
static uint16_t RxPacketSize = 0;
static int8_t RxPacketSnrEstimate;
//static double RxPacketRssiValue;
//static uint8_t RxGain = 1;
//static uint32_t RxTimeoutTimer = 0;
/*!
 * Tx management support variables
 */
static uint16_t TxPacketSize = 0;
//1,复位引脚复位
//2.进入sleepmode
//3.启动外部晶振
//4.进入lora模式
//5.设置载波频率
//6.输出功率设置    
void SX1278Init()
{    
    SX1278InitIo( );
    SX1278Reset();
    SX1278SetOpMode( SX1278_SLEEP );
    setRegValue(SX1278_REG_OP_MODE, SX1278_LORA, 7, 7);//lora mode on

    /*
    setRegValue(SX1278_REG_DIO_MAPPING_1, 
                SX1278_DIO0_RX_DONE | 
                SX1278_DIO1_RX_TIMEOUT | 
                SX1278_DIO2_FHSS_CHANGE_CHANNEL | 
                SX1278_DIO3_CAD_DONE , 
                7, 0);
    setRegValue(SX1278_REG_DIO_MAPPING_2, 
                SX1278_DIO4_CAD_DETECTED | 
                SX1278_DIO5_MODE_READY , 
                7, 4);
    */
    
    //5.设置载波频率
    setRegValue(SX1278_REG_FRF_MSB, SX1278_FRF_MSB,7,0);
    setRegValue(SX1278_REG_FRF_MID, SX1278_FRF_MID,7,0);
    setRegValue(SX1278_REG_FRF_LSB, SX1278_FRF_LSB,7,0);
    

 
    setRegValue(SX1278_REG_MODEM_CONFIG_1, (LoRaSettings.SignalBw<<4) ,7,4);//bw setting
    setRegValue(SX1278_REG_MODEM_CONFIG_1, (LoRaSettings.ErrorCoding<<1) ,3,1);//cr setting    
    if(LoRaSettings.ImplicitHeaderOn == TRUE)//head setting
    {
        //如果采用隐式报头，则只能发送或者接受特定的字节长度
        setRegValue(SX1278_REG_PAYLOAD_LENGTH,LoRaSettings.PayloadLength ,7,0);//set payload length
        setRegValue(SX1278_REG_MODEM_CONFIG_1, SX1278_HEADER_IMPL_MODE,0,0);
    }
    else
        setRegValue(SX1278_REG_MODEM_CONFIG_1, SX1278_HEADER_EXPL_MODE,0,0);
    
    setRegValue(SX1278_REG_MODEM_CONFIG_2, (LoRaSettings.SpreadingFactor<<4) ,7,4);//sf 
    setRegValue(SX1278_REG_MODEM_CONFIG_2, SX1278_TX_MODE_SINGLE,3,3);//set tx single mode
    setRegValue(SX1278_REG_MODEM_CONFIG_2, (LoRaSettings.CrcOn<<2) ,2,2);// set crc 
    
    //set rx time out counter
    setRegValue(SX1278_REG_MODEM_CONFIG_2, SX1278_RX_TIMEOUT_MSB|0X03,1,0);
    setRegValue(SX1278_REG_SYMB_TIMEOUT_LSB, SX1278_RX_TIMEOUT_LSB | 0XFF,7,0);

    
    if(LoRaSettings.SpreadingFactor == 6) {
        setRegValue(SX1278_REG_MODEM_CONFIG_1, SX1278_HEADER_IMPL_MODE,0,0);//if sf=6,mast set SX1278_HEADER_IMPL_MODE
        setRegValue(SX1278_REG_DETECT_OPTIMIZE, SX1278_DETECT_OPTIMIZE_SF_6, 2, 0);//有疑问
        setRegValue(SX1278_REG_DETECTION_THRESHOLD, SX1278_DETECTION_THRESHOLD_SF_6,7,0);
    } else {
        //setRegValue(SX1278_REG_DETECT_OPTIMIZE, SX1278_DETECT_OPTIMIZE_SF_7_12, 2, 0);
        //setRegValue(SX1278_REG_DETECTION_THRESHOLD, SX1278_DETECTION_THRESHOLD_SF_7_12,7,0);
    }
    
    

    //setRegValue(SX1278_REG_MODEM_CONFIG_3, SX1278_LOW_DATA_RATE_OPT_ON ,3,3);//Set Low Data rate Optimize

    
    
    //9.LNA 增益设置001，最大增益
    setRegValue(SX1278_REG_LNA, SX1278_LNA_GAIN_1 | SX1278_LNA_BOOST_HF_ON,7,0);
       setRegValue(SX1278_REG_SYMB_TIMEOUT_LSB, 0x0f,7,0);
    
    setRegValue(SX1278_REG_PREAMBLE_MSB, SX1278_PREAMBLE_LENGTH_MSB,7,0);//前导码长度高位
    setRegValue(SX1278_REG_PREAMBLE_LSB, SX1278_PREAMBLE_LENGTH_LSB,7,0);//前导码长度低位

    //6.输出功率设置    
    setRegValue(SX1278_REG_PA_CONFIG, SX1278_PA_SELECT_BOOST | SX1278_MAX_POWER |(LoRaSettings.Power - 5)&0x0f,7,0);
    setRegValue(SX1278_REG_PA_DAC, SX1278_PA_BOOST_ON, 2, 0);
    //7.设置PA的过流保护（关闭），电流微调默认0x0b
    setRegValue(SX1278_REG_OCP, SX1278_OCP_OFF | SX1278_OCP_TRIM, 5, 0);
    SX1278SetOpMode(SX1278_STANDBY);    
}
void SX1278TxMode() {
  //uint8_t temp;
  SX1278SetOpMode(SX1278_STANDBY);
 // temp = getRegValue(SX1278_REG_PA_DAC,7,0);
 // printf("temp = 0x%x\n",temp);
  setRegValue(SX1278_REG_HOP_PERIOD, SX1278_HOP_PERIOD_OFF,7,0);
  setRegValue(SX1278_REG_DIO_MAPPING_1, SX1278_DIO0_TX_DONE, 7, 6);
  //SX1278ClearIRQFlags();
  setRegValue(SX1278_REG_IRQ_FLAGS_MASK, SX1278_MASK_IRQ_FLAG_TX_DONE,7,0);

}
void SX1278RxMode(bool RxSingleOn)
{
    //set mode to standby
    SX1278SetOpMode(SX1278_STANDBY);
    //enable RX_TIMEOUT,RX_DONE interrupte
    setRegValue(SX1278_REG_IRQ_FLAGS_MASK, 
                (//SX1278_MASK_IRQ_FLAG_RX_TIMEOUT &
                 SX1278_MASK_IRQ_FLAG_RX_DONE),
                7,0);
    //config the interrupte pin 
    setRegValue(SX1278_REG_DIO_MAPPING_1, 
                SX1278_DIO0_RX_DONE | 
                SX1278_DIO1_RX_TIMEOUT | 
                SX1278_DIO2_FHSS_CHANGE_CHANNEL | 
                SX1278_DIO3_CAD_DONE , 
                7, 0);
    setRegValue(SX1278_REG_DIO_MAPPING_2, 
                SX1278_DIO4_CAD_DETECTED | 
                SX1278_DIO5_MODE_READY , 
                7, 4);
    
  
    //set single mode on or off
    if(RxSingleOn == TRUE)
    {
        //set OpMode receive
        SX1278SetOpMode(SX1278_RXSINGLE); 
    }
    else
    {
        //set fifo ptr address as fifo base address
        setRegValue(SX1278_REG_FIFO_RX_BASE_ADDR, SX1278_FIFO_RX_BASE_ADDR,7,0);
        setRegValue(SX1278_REG_FIFO_ADDR_PTR,     SX1278_FIFO_RX_BASE_ADDR,7,0);
        //set OpMode receive
        SX1278SetOpMode(SX1278_RXCONTINUOUS); 
    }
    
    //clear RF_BUFFER
    memset( RFBuffer, 0, ( size_t )RF_BUFFER_SIZE );
    


    
     //  SX1278ClearIRQFlags();
    
    //setRegValue(SX1278_REG_FIFO_RX_BASE_ADDR, SX1278_FIFO_RX_BASE_ADDR_MAX);
//    setRegValue(SX1278_REG_FIFO_RX_BASE_ADDR, SX1278_FIFO_RX_BASE_ADDR_MAX);
//    setRegValue(SX1278_REG_FIFO_ADDR_PTR, SX1278_FIFO_RX_BASE_ADDR_MAX);

    if( LoRaSettings.FreqHopOn == TRUE )
    {
       // SX1276LR->RegHopPeriod = LoRaSettings.HopPeriod;

        //SX1276Read( REG_LR_HOPCHANNEL, &SX1276LR->RegHopChannel );
       // SX1276LoRaSetRFFrequency( HoppingFrequencies[SX1276LR->RegHopChannel & RFLR_HOPCHANNEL_CHANNEL_MASK] );
        setRegValue(SX1278_REG_HOP_PERIOD, SX1278_HOP_PERIOD_MAX,7,0);
    }
    else
    {
       // SX1276LR->RegHopPeriod = 255;
        setRegValue(SX1278_REG_HOP_PERIOD, SX1278_HOP_PERIOD_MAX,7,0);
    }
        
    //SX1276Write( REG_LR_HOPPERIOD, SX1276LR->RegHopPeriod );
                
    //update enter rx mode time
    LastTime = millis();

}
void SX1278Send(uint8_t* pBuffer,uint8_t len)
{

  setRegValue(SX1278_REG_PAYLOAD_LENGTH, len,7,0);
  setRegValue(SX1278_REG_FIFO_TX_BASE_ADDR, SX1278_FIFO_TX_BASE_ADDR,7,0);
  setRegValue(SX1278_REG_FIFO_ADDR_PTR, SX1278_FIFO_TX_BASE_ADDR,7,0);

  SX1278WriteBuffer(SX1278_REG_FIFO, pBuffer, len);
  
  SX1278SetOpMode(SX1278_TX);
}

void SX1278GetRxPacket( void *buffer, uint16_t *size )
{
    *size = RxPacketSize;
    RxPacketSize = 0;
    memcpy( ( void * )buffer, ( void * )RFBuffer, ( size_t )*size );
}
void SX1278SetTxPacket(uint8_t* pBuffer,uint8_t len)
{
    TxPacketSize = len;
    memcpy( ( void * )RFBuffer, pBuffer, ( size_t )TxPacketSize ); 

    RFLRState = RFLR_STATE_TX_INIT;
}
void SX1276LoRaSetRFState( uint8_t state )
{
    RFLRState = state;
}

uint8_t SX1278Process( void )
{
    uint8_t result = RF_BUSY;
    switch( RFLRState )
    {
        case RFLR_STATE_IDLE:
            break;
        case RFLR_STATE_RX_INIT:
                      printf("RX_INIT\n");
            SX1278RxMode(LoRaSettings.RxSingleOn);
            RFLRState = RFLR_STATE_RX_RUNNING;
            break;
        case RFLR_STATE_RX_RUNNING:
            printf("RX_RUNNING\n");

            if( DIO0_PIN_READ == 1 ) // RxDone
            {
                LastTime = millis( );
                // Clear Irq
                SX1278ClearIRQFlags(SX1278_IRQFLAGS_RXDONE);
                RFLRState = RFLR_STATE_RX_DONE;
            }
            if( DIO1_PIN_READ == 1 ) // RxTimeout
            {
                LastTime = millis( );

                // Clear Irq
                SX1278ClearIRQFlags(SX1278_IRQFLAGS_RXTIMEOUT);
                RFLRState = RFLR_STATE_RX_TIMEOUT;
                printf("RX TIME OUT(HARD)\n");
            }
            if( DIO2_PIN_READ == 1 ) // FHSS Changed Channel
            {
                LastTime = millis( );
                //if( LoRaSettings.FreqHopOn == true )
                //{
               //     SX1276Read( REG_LR_HOPCHANNEL, &SX1276LR->RegHopChannel );
                //    SX1276LoRaSetRFFrequency( HoppingFrequencies[SX1276LR->RegHopChannel & RFLR_HOPCHANNEL_CHANNEL_MASK] );
                //}
                // Clear Irq
                //SX1276Write( REG_LR_IRQFLAGS, RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL );
                // Debug
                //RxGain = SX1276LoRaReadRxGain( );
            }

            if( LoRaSettings.RxSingleOn == TRUE ) // Rx single mode
            {
                if( ( millis( ) - LastTime ) > LoRaSettings.RxPacketTimeout )
                {
                    RFLRState = RFLR_STATE_RX_TIMEOUT;
                    printf("RX TIME OUT");
                }
            }
            break;
        case RFLR_STATE_RX_DONE:
            printf("RX_DONE\n");
            if((getRegValue(SX1278_REG_IRQ_FLAGS,7,0)&SX1278_IRQFLAGS_PAYLOADCRCERROR) == SX1278_IRQFLAGS_PAYLOADCRCERROR)
            {
                SX1278ClearIRQFlags(SX1278_IRQFLAGS_PAYLOADCRCERROR);
                if( LoRaSettings.RxSingleOn == TRUE ) // Rx single mode
                {
                    RFLRState = RFLR_STATE_RX_INIT;
                }
                else
                {
                    RFLRState = RFLR_STATE_RX_RUNNING;
                }
                break;

            }
            {
                uint8_t rxSnrEstimate;
                rxSnrEstimate = getRegValue( SX1278_REG_PKT_SNR_VALUE, 7,0 );
                if( rxSnrEstimate & 0x80 ) // The SNR sign bit is 1
                {
                    // Invert and divide by 4
                    RxPacketSnrEstimate = ( ( ~rxSnrEstimate + 1 ) & 0xFF ) >> 2;
                    RxPacketSnrEstimate = -RxPacketSnrEstimate;
                }
                else
                {
                    // Divide by 4
                    RxPacketSnrEstimate = ( rxSnrEstimate & 0xFF ) >> 2;
                }
            }
            
            if( LoRaSettings.RxSingleOn == TRUE ) // Rx single mode
            {
                setRegValue( SX1278_REG_FIFO_ADDR_PTR, SX1278_FIFO_RX_BASE_ADDR,7,0 );

                if( LoRaSettings.ImplicitHeaderOn == TRUE )
                {
                    RxPacketSize = LoRaSettings.PayloadLength;
                }
                else
                {
                    RxPacketSize = getRegValue( SX1278_REG_RX_NB_BYTES,7,0 );
                }
                SX1278ReadBuffer(0, RFBuffer, RxPacketSize );

            }
            else // Rx continuous mode
            {
                //将最后一次接受到的数据包读取出来并传递给FIFO的读取指针头
                //用于读取FIFO中的数据
                uint8_t current_ptr;
                current_ptr = getRegValue( SX1278_REG_FIFO_RX_CURRENT_ADDR, 7,0 );
                setRegValue( SX1278_REG_FIFO_ADDR_PTR,current_ptr,7,0);                
                if( LoRaSettings.ImplicitHeaderOn == TRUE )
                {
                    RxPacketSize = LoRaSettings.PayloadLength;
                }
                else
                {
                    RxPacketSize = getRegValue( SX1278_REG_RX_NB_BYTES,7,0 );
                }
                SX1278ReadBuffer(0, RFBuffer, RxPacketSize );

            }
            if( LoRaSettings.RxSingleOn == TRUE ) // Rx single mode
            {
                RFLRState = RFLR_STATE_RX_INIT;
            }
            else // Rx continuous mode
            {
                RFLRState = RFLR_STATE_RX_RUNNING;
            }
            result = RF_RX_DONE;
              break;
        case RFLR_STATE_RX_TIMEOUT:
            printf("RX_TIMEOUT\n");
            //RFLRState = RFLR_STATE_RX_INIT;
            result = RF_RX_TIMEOUT;
            break;
        case RFLR_STATE_TX_INIT:
            printf("TX-INIT\n");
            SX1278TxMode();
            SX1278Send(RFBuffer,TxPacketSize);
            LastTime = millis( );

            RFLRState = RFLR_STATE_TX_RUNNING;
            break;
        case RFLR_STATE_TX_RUNNING:
            printf("TX-RUNNING\n");
            if( DIO0_PIN_READ == 1 ) // TxDone
            {
                // Clear Irq
                SX1278ClearIRQFlags(SX1278_IRQFLAGS_RXTIMEOUT);
                RFLRState = RFLR_STATE_TX_DONE;   
                printf("dio0\n");
            }
            if( DIO2_PIN_READ == 1 ) // FHSS Changed Channel
            {
                //if( LoRaSettings.FreqHopOn == true )
                //{
                //    SX1276Read( REG_LR_HOPCHANNEL, &SX1276LR->RegHopChannel );
                //    SX1276LoRaSetRFFrequency( HoppingFrequencies[SX1276LR->RegHopChannel & RFLR_HOPCHANNEL_CHANNEL_MASK] );
                //}
                // Clear Irq
                // SX1276Write( REG_LR_IRQFLAGS, RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL );
            }
            if(millis() - LastTime > LoRaSettings.TxPacketTimeout)
            {
                result = RF_TX_TIMEOUT;
            }
            break;
        case RFLR_STATE_TX_DONE:
            printf("RF-TX-DNOE \n");
            // optimize the power consumption by switching off the transmitter as soon as the packet has been sent
            SX1278SetOpMode( SX1278_STANDBY );
            RFLRState = RFLR_STATE_IDLE;
            result = RF_TX_DONE;
            break;
        case RFLR_STATE_CAD_INIT: 
            break;
        case RFLR_STATE_CAD_RUNNING:
        default:
            break;
    }
    return result;
}
