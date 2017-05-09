#include "sx1278.h"



#define XTAL_FREQ               26000000
#define FREQ_STEP               49.59106

/*!
 * Local RF buffer for communication support
 */
#define RF_BUFFER_SIZE          256
static uint8_t RFBuffer[RF_BUFFER_SIZE];


static uint32_t LastTxTime = 0;
static uint32_t LastRxTime = 0;
uint32_t TxPacketTime = 0;
uint32_t RxPacketTime = 0;


// Default settings
tLoRaSettings LoRaSettings =
{
    434000000,        // RFFrequency
    20,               // Power
    6,                // SignalBw [0: 7.8kHz, 1: 10.4 kHz, 2: 15.6 kHz, 3: 20.8 kHz, 4: 31.2 kHz,
                      // 5: 41.6 kHz, 6: 62.5 kHz, 7: 125 kHz, 8: 250 kHz, 9: 500 kHz, other: Reserved]
    10,                // SpreadingFactor [6: 64, 7: 128, 8: 256, 9: 512, 10: 1024, 11: 2048, 12: 4096  chips]
    1,                // ErrorCoding [1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8]
    TRUE,             // CrcOn [0: OFF, 1: ON]
    TRUE,            // ImplicitHeaderOn [0: OFF, 1: ON]
    TRUE,                // RxSingleOn [0: Continuous, 1 Single]
    FALSE,                // FreqHopOn [0: OFF, 1: ON]
    4,                // HopPeriod Hops every frequency hopping period symbols
    //100,              // TxPacketTimeout
    1500,              // RxPacketTimeout
    8,              // PayloadLength (used for implicit header mode)
    4,                  //preamble length(4-x)
};

/*!
 * RF state machine variable
 */
uint8_t RFLRState = RFLR_STATE_IDLE;
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
//1,��λ���Ÿ�λ
//2.����sleepmode
//3.�����ⲿ����
//4.����loraģʽ
//5.�����ز�Ƶ��
//6.�����������    
void SX1278Init()
{    
  uint8_t temp;
  uint16_t temp16;
    SX1278InitIo( );
    SX1278Reset();
  
    SX1278SetOpMode( SX1278_SLEEP );
  
    SX1278WriteBits(SX1278_REG_OP_MODE, SX1278_LORA, 7, 7);//lora mode on
  

    /*
    SX1278WriteBits(SX1278_REG_DIO_MAPPING_1, 
                SX1278_DIO0_RX_DONE | 
                SX1278_DIO1_RX_TIMEOUT | 
                SX1278_DIO2_FHSS_CHANGE_CHANNEL | 
                SX1278_DIO3_CAD_DONE , 
                7, 0);
    SX1278WriteBits(SX1278_REG_DIO_MAPPING_2, 
                SX1278_DIO4_CAD_DETECTED | 
                SX1278_DIO5_MODE_READY , 
                7, 4);
    */
    
    //5.�����ز�Ƶ��
    SX1278SetRFFrequency(LoRaSettings.RFFrequency);
    LORA_DBG("IRQ:%ld\n",SX1278GetRFFrequency());

  
    

    temp = (LoRaSettings.SignalBw<<4) | (LoRaSettings.ErrorCoding<<1) | LoRaSettings.ImplicitHeaderOn;
    SX1278Write(SX1278_REG_MODEM_CONFIG_1,temp);
    //���������ʽ��ͷ����ֻ�ܷ��ͻ��߽����ض����ֽڳ���
    //SX1278Write(SX1278_REG_PAYLOAD_LENGTH,LoRaSettings.PayloadLength);//set payload length

    
    if(LoRaSettings.SpreadingFactor == 6) {
        SX1278Write(SX1278_REG_PAYLOAD_LENGTH,LoRaSettings.PayloadLength );//set payload length
  
        SX1278WriteBits(SX1278_REG_MODEM_CONFIG_1, SX1278_HEADER_IMPL_MODE,0,0);//if sf=6,mast set SX1278_HEADER_IMPL_MODE
  
        SX1278WriteBits(SX1278_REG_DETECT_OPTIMIZE, SX1278_DETECT_OPTIMIZE_SF_6, 2, 0);//������
        SX1278Write(SX1278_REG_DETECTION_THRESHOLD, SX1278_DETECTION_THRESHOLD_SF_6);
    } else {
        SX1278WriteBits(SX1278_REG_DETECT_OPTIMIZE, SX1278_DETECT_OPTIMIZE_SF_7_12, 2, 0);
       SX1278Write(SX1278_REG_DETECTION_THRESHOLD, SX1278_DETECTION_THRESHOLD_SF_7_12);
    }    
    //sf 
    //set tx single mode
    // set crc 
    //set rx time out counter
    temp = (LoRaSettings.SpreadingFactor<<4) | SX1278_TX_MODE_SINGLE | (LoRaSettings.CrcOn<<2) | SX1278_RX_TIMEOUT_MSB;
    SX1278Write(SX1278_REG_MODEM_CONFIG_2, temp);
  
    SX1278Write(SX1278_REG_SYMB_TIMEOUT_LSB, SX1278_RX_TIMEOUT_LSB | 0XFF);

    
    SX1278WriteBits(SX1278_REG_MODEM_CONFIG_3, SX1278_LOW_DATA_RATE_OPT_ON ,3,3);//Set Low Data rate Optimize
  

    
    //LNA ������������001���������
    SX1278Write(SX1278_REG_LNA, SX1278_LNA_GAIN_1 | SX1278_LNA_BOOST_HF_ON);
  

    //�����������    
    SX1278WriteBits(SX1278_REG_PA_DAC, SX1278_PA_BOOST_ON, 2, 0);
    SX1278WriteBits(SX1278_REG_PA_CONFIG, SX1278_PA_SELECT_BOOST, 7, 7);
    SX1278SetRFPower(LoRaSettings.Power);
    LORA_DBG("PACFG2:0x%02x\n",SX1278Read(SX1278_REG_PA_CONFIG));

  
    //����PA�Ĺ����������رգ�������΢��Ĭ��0x0b
    SX1278WriteBits(SX1278_REG_OCP, SX1278_OCP_OFF | SX1278_OCP_TRIM, 5, 0);
  
    temp16 = LoRaSettings.PreambleLength;
    temp = temp16>>8;
    SX1278Write(SX1278_REG_PREAMBLE_MSB, temp);//ǰ���볤�ȸ�λ
    temp = temp16&0x00ff;
    SX1278Write(SX1278_REG_PREAMBLE_LSB, temp);//ǰ���볤�ȵ�λ
   
    SX1278SetOpMode(SX1278_STANDBY);    
  
    LORA_DBG("IRQ:0x%02x\n",SX1278Read(SX1278_REG_PREAMBLE_MSB));
    LORA_DBG("IRQ:0x%02x\n",SX1278Read(SX1278_REG_PREAMBLE_LSB));
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
void SX1278SetRFFrequency(uint32_t freq)
{
    LoRaSettings.RFFrequency = freq;
    freq = ( uint32_t )( ( double )freq / ( double )FREQ_STEP );
    SX1278Write(SX1278_REG_FRF_MSB, ( freq >> 16 ) & 0xFF );
    SX1278Write(SX1278_REG_FRF_MID, ( freq >> 8 ) & 0xFF );
    SX1278Write(SX1278_REG_FRF_LSB, freq & 0xFF);
}
uint32_t SX1278GetRFFrequency( void )
{
    uint8_t tempMSB,tempMID,tempLSB;
    uint32_t tempFrq;
    tempMSB = SX1278Read( SX1278_REG_FRF_MSB);
    tempMID = SX1278Read( SX1278_REG_FRF_MID);
    tempLSB = SX1278Read( SX1278_REG_FRF_LSB);
    tempFrq = ( ( uint32_t )tempMSB << 16 ) | ( ( uint32_t )tempMID << 8 ) | ( ( uint32_t )tempLSB );
    tempFrq = ( uint32_t )( ( double )tempFrq * ( double )FREQ_STEP );

    return tempFrq;
}
void SX1278SetRFPower( int8_t power )
{
    uint8_t PaConfig = SX1278Read( SX1278_REG_PA_CONFIG );
    ;
    
    if( ( PaConfig & SX1278_PA_SELECT_BOOST ) == SX1278_PA_SELECT_BOOST )
    {
        if( ( SX1278Read( SX1278_REG_PA_DAC ) & 0x87 ) == 0x87 )
        {
            if( power < 5 )
            {
                power = 5;
            }
            if( power > 20 )
            {
                power = 20;
            }
            PaConfig = ( PaConfig & SX1278_MAX_POWER | SX1278_PA_SELECT_BOOST ) | 0x70;
            PaConfig = ( PaConfig & SX1278_MAX_POWER | SX1278_PA_SELECT_BOOST) | ( uint8_t )( ( uint16_t )( power - 5 ) & 0x0F );
        }
        else
        {
            if( power < 2 )
            {
                power = 2;
            }
            if( power > 17 )
            {
                power = 17;
            }
            PaConfig = ( PaConfig & SX1278_MAX_POWER | SX1278_PA_SELECT_BOOST) | 0x70;
            PaConfig = ( PaConfig & SX1278_MAX_POWER | SX1278_PA_SELECT_BOOST) | ( uint8_t )( ( uint16_t )( power - 2 ) & 0x0F );
        }
    }
    else
    {
        if( power < -1 )
        {
            power = -1;
        }
        if( power > 14 )
        {
            power = 14;
        }
        PaConfig = ( PaConfig & SX1278_MAX_POWER ) | 0x70;
        PaConfig = ( PaConfig & SX1278_MAX_POWER ) | ( uint8_t )( ( uint16_t )( power + 1 ) & 0x0F );
    }
    SX1278Write( SX1278_REG_PA_CONFIG, PaConfig );
    LoRaSettings.Power = power;
}

void SX1278TxMode() 
{
    SX1278SetOpMode(SX1278_STANDBY);
    SX1278Write(SX1278_REG_HOP_PERIOD, SX1278_HOP_PERIOD_OFF);
    SX1278WriteBits(SX1278_REG_DIO_MAPPING_1, SX1278_DIO0_TX_DONE, 7, 6);
    SX1278Write(SX1278_REG_IRQ_FLAGS_MASK, SX1278_MASK_IRQ_FLAG_TX_DONE);
}
void SX1278Send(uint8_t* pBuffer,uint8_t len)
{
  SX1278Write(SX1278_REG_PAYLOAD_LENGTH, len);
  SX1278Write(SX1278_REG_FIFO_TX_BASE_ADDR, SX1278_FIFO_TX_BASE_ADDR_MAX);
  SX1278Write(SX1278_REG_FIFO_ADDR_PTR, SX1278_FIFO_TX_BASE_ADDR_MAX);
  SX1278WriteBuffer(SX1278_REG_FIFO, pBuffer, len);
  SX1278SetOpMode(SX1278_TX);
}
void SX1278SetTxPacket(uint8_t* pBuffer,uint8_t len)
{
    if(LoRaSettings.ImplicitHeaderOn == TRUE && LoRaSettings.PayloadLength < len)
    {
        TxPacketSize = LoRaSettings.PayloadLength;
    }
    else
    {
        TxPacketSize = len;
    }
    memcpy( ( void * )RFBuffer, pBuffer, ( size_t )TxPacketSize ); 
    RFLRState = RFLR_STATE_TX_INIT;
}

void SX1278RxMode(bool RxSingleOn)
{
    //set mode to standby
    SX1278SetOpMode(SX1278_STANDBY);
    //enable RX_TIMEOUT,RX_DONE interrupte
    SX1278Write(SX1278_REG_IRQ_FLAGS_MASK, 
                //SX1278_MASK_IRQ_FLAG_RX_TIMEOUT &
                 SX1278_MASK_IRQ_FLAG_RX_DONE
                 );
    //config the interrupte pin 
    SX1278Write(SX1278_REG_DIO_MAPPING_1, 
                SX1278_DIO0_RX_DONE | 
                SX1278_DIO1_RX_TIMEOUT | 
                SX1278_DIO2_FHSS_CHANGE_CHANNEL | 
                SX1278_DIO3_CAD_DONE 
                );
    SX1278WriteBits(SX1278_REG_DIO_MAPPING_2, 
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
      
      //SX1278_REG_FIFO_RX_BASE_ADDR�������ã�Ĭ�Ͼ���0�����ÿռ�Ϊ256�ֽ�
        //set fifo ptr address as fifo base address
        //SX1278Write(SX1278_REG_FIFO_RX_BASE_ADDR, SX1278_FIFO_RX_BASE_ADDR_MAX);
        SX1278Write(SX1278_REG_FIFO_ADDR_PTR,     SX1278_FIFO_RX_BASE_ADDR_MAX);
        //set OpMode receive
        SX1278SetOpMode(SX1278_RXCONTINUOUS); 
    }
    
    //clear RF_BUFFER
    memset( RFBuffer, 0, ( size_t )RF_BUFFER_SIZE );
    


    
     //  SX1278ClearIRQFlags();
    
    //SX1278WriteBits(SX1278_REG_FIFO_RX_BASE_ADDR, SX1278_FIFO_RX_BASE_ADDR_MAX);
//    SX1278WriteBits(SX1278_REG_FIFO_RX_BASE_ADDR, SX1278_FIFO_RX_BASE_ADDR_MAX);
//    SX1278WriteBits(SX1278_REG_FIFO_ADDR_PTR, SX1278_FIFO_RX_BASE_ADDR_MAX);

    //if( LoRaSettings.FreqHopOn == TRUE )
    //{
       // SX1276LR->RegHopPeriod = LoRaSettings.HopPeriod;

        //SX1276Read( REG_LR_HOPCHANNEL, &SX1276LR->RegHopChannel );
       // SX1276LoRaSetRFFrequency( HoppingFrequencies[SX1276LR->RegHopChannel & RFLR_HOPCHANNEL_CHANNEL_MASK] );
    //    SX1278Write(SX1278_REG_HOP_PERIOD, SX1278_HOP_PERIOD_MAX);
    //}
    //else
    //{
       // SX1276LR->RegHopPeriod = 255;
    //    SX1278Write(SX1278_REG_HOP_PERIOD, SX1278_HOP_PERIOD_MAX);
    //}
        
    //SX1276Write( REG_LR_HOPPERIOD, SX1276LR->RegHopPeriod );
                
    //update enter rx mode time
}

void SX1278GetRxPacket( void *buffer, uint16_t *size )
{
    *size = RxPacketSize;
    RxPacketSize = 0;
    memcpy( ( void * )buffer, ( void * )RFBuffer, ( size_t )*size );
}

void SX1278SetRFState( uint8_t state )
{
    RFLRState = state;
}
uint8_t SX1278Process( void )
{
    uint8_t result = RF_BUSY;
    switch( RFLRState )
    {
    case RFLR_STATE_IDLE:
        SX1278SetOpMode( SX1278_SLEEP );
        break;
    case RFLR_STATE_RX_INIT:
        LORA_DBG("RX_INIT");
        SX1278RxMode(LoRaSettings.RxSingleOn);
        RFLRState = RFLR_STATE_RX_RUNNING;
        LastRxTime  = millis();
        break;
    case RFLR_STATE_RX_RUNNING:
        //LORA_DBG("RX_RUNNING");

        if( DIO0_PIN_READ == 1 ) // RxDone
        {
          RxPacketTime = millis() - LastRxTime;
            // Clear Irq
            SX1278ClearIRQFlags(SX1278_CLEAR_IRQ_FLAG_RX_DONE);
            RFLRState = RFLR_STATE_RX_DONE;
        }
        /*
        ��ʹ��Ӳ���ĳ�ʱ�ж�
        if( DIO1_PIN_READ == 1 ) // RxTimeout
        {
            LastTime = millis( );

            // Clear Irq
            SX1278ClearIRQFlags(SX1278_CLEAR_IRQ_FLAG_RX_TIMEOUT);
            RFLRState = RFLR_STATE_RX_TIMEOUT;
            LORA_DBG("RX TIME OUT(HARD)\n");
        }
        */
        //if( DIO2_PIN_READ == 1 ) // FHSS Changed Channel
        //{
        //    LastTime = millis( );
            //if( LoRaSettings.FreqHopOn == true )
            //{
           //     SX1276Read( REG_LR_HOPCHANNEL, &SX1276LR->RegHopChannel );
            //    SX1276LoRaSetRFFrequency( HoppingFrequencies[SX1276LR->RegHopChannel & RFLR_HOPCHANNEL_CHANNEL_MASK] );
            //}
            // Clear Irq
            //SX1276Write( REG_LR_IRQFLAGS, RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL );
            // Debug
            //RxGain = SX1276LoRaReadRxGain( );
       // }

        if( LoRaSettings.RxSingleOn == TRUE ) // Rx single mode
        {
            if( ( millis( ) - LastRxTime ) > LoRaSettings.RxPacketTimeout )
            {
                RxPacketTime = millis() - LastRxTime;
                RFLRState = RFLR_STATE_RX_TIMEOUT;
                //LORA_DBG("RX TIME OUT(SOFT)\n");

            }
        }
        break;
    case RFLR_STATE_RX_DONE:
        LORA_DBG("RX_DONE");
        //
        if((SX1278Read(SX1278_REG_IRQ_FLAGS)&SX1278_CLEAR_IRQ_FLAG_PAYLOAD_CRC_ERROR) == SX1278_CLEAR_IRQ_FLAG_PAYLOAD_CRC_ERROR)
        {
          //���������CRC�������RX DONE�жϺ�CRC�ж�
            SX1278ClearIRQFlags(SX1278_CLEAR_IRQ_FLAG_RX_DONE | SX1278_CLEAR_IRQ_FLAG_PAYLOAD_CRC_ERROR);


        }
        else
        {
          //���û��CRC�������RX DONE�ж�
            SX1278ClearIRQFlags(SX1278_CLEAR_IRQ_FLAG_RX_DONE);
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
            rxSnrEstimate = SX1278Read( SX1278_REG_PKT_SNR_VALUE );
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
            SX1278Write( SX1278_REG_FIFO_ADDR_PTR, SX1278_FIFO_RX_BASE_ADDR_MAX );

            if( LoRaSettings.ImplicitHeaderOn == TRUE )
            {
                RxPacketSize = LoRaSettings.PayloadLength;
            }
            else
            {
                RxPacketSize = SX1278Read( SX1278_REG_RX_NB_BYTES );
            }
            SX1278ReadBuffer(0, RFBuffer, RxPacketSize );

        }
        else // Rx continuous mode
        {
            //�����һ�ν��ܵ������ݰ���ȡ���������ݸ�FIFO�Ķ�ȡָ��ͷ
            //���ڶ�ȡFIFO�е�����
            SX1278Write( SX1278_REG_FIFO_ADDR_PTR,SX1278Read( SX1278_REG_FIFO_RX_CURRENT_ADDR ));                
            if( LoRaSettings.ImplicitHeaderOn == TRUE )
            {
                RxPacketSize = LoRaSettings.PayloadLength;
            }
            else
            {
                RxPacketSize = SX1278Read( SX1278_REG_RX_NB_BYTES );
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
        LORA_DBG("RX_TIMEOUT");
        RFLRState = RFLR_STATE_RX_INIT;
        result = RF_RX_TIMEOUT;
        break;
    case RFLR_STATE_TX_INIT:
        LORA_DBG("TX-INIT");
        SX1278TxMode();
        SX1278Send(RFBuffer,TxPacketSize);
        RFLRState = RFLR_STATE_TX_RUNNING;
        LastTxTime = millis( );
        break;
    case RFLR_STATE_TX_RUNNING:
        //LORA_DBG("TX-RUNNING");
        if( DIO0_PIN_READ == 1 ) // TxDone
        {
            TxPacketTime = millis() - LastTxTime;
            // Clear Irq
            SX1278ClearIRQFlags(SX1278_CLEAR_IRQ_FLAG_TX_DONE);

            RFLRState = RFLR_STATE_TX_DONE;   
            //printf("dio0\n");
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
        /*��ʹ�÷��ͳ�ʱ
        if(millis() - LastTime > LoRaSettings.TxPacketTimeout)
        {
            TxPacketTime = millis() - LastTime;
            RFLRState = RFLR_STATE_TX_TIMEOUT;
        }*/
        break;
    case RFLR_STATE_TX_DONE:
        LORA_DBG("RF-TX-DNOE");
        // optimize the power consumption by switching off the transmitter as soon as the packet has been sent
        SX1278SetOpMode( SX1278_STANDBY );
        RFLRState = RFLR_STATE_IDLE;
        result = RF_TX_DONE;
        break;
    /*��ʹ�÷��ͳ�ʱ
    case RFLR_STATE_TX_TIMEOUT:
        LORA_DBG("RF-TX-TIMEOUT");
        result = RF_TX_TIMEOUT;
        break;
        */
    case RFLR_STATE_CAD_INIT: 
        break;
    case RFLR_STATE_CAD_RUNNING:
    default:
        break;
    }
    return result;
}
