#ifndef __SX1278_H
#define __SX1278_H
#include "ebox.h"
#include "sx1278-hal.h"
extern uint32_t RxTcPacketTime;

typedef enum
{
    RF_IDLE,
    RF_BUSY,
    RF_RX_DONE,
    RF_RX_TIMEOUT,
    RF_TX_DONE,
    RF_TX_TIMEOUT,
    RF_LEN_ERROR,
    RF_CHANNEL_EMPTY,
    RF_CHANNEL_ACTIVITY_DETECTED,
}tRFProcessReturnCodes;

//LoRa
typedef enum
{
    RFLR_STATE_IDLE,
    RFLR_STATE_RX_INIT,
    RFLR_STATE_RX_RUNNING,
    RFLR_STATE_RX_DONE,
    RFLR_STATE_RX_TIMEOUT,
    RFLR_STATE_TX_INIT,
    RFLR_STATE_TX_RUNNING,
    RFLR_STATE_TX_DONE,
    RFLR_STATE_TX_TIMEOUT,
    RFLR_STATE_CAD_INIT,
    RFLR_STATE_CAD_RUNNING,
}tRFLRStates;
    /*!
 * SX1276 LoRa General parameters definition
 */
typedef struct sLoRaSettings
{
    uint32_t RFFrequency;
    int8_t Power;
    uint8_t SignalBw;                   // LORA [0: 7.8 kHz, 1: 10.4 kHz, 2: 15.6 kHz, 3: 20.8 kHz, 4: 31.2 kHz,
                                        // 5: 41.6 kHz, 6: 62.5 kHz, 7: 125 kHz, 8: 250 kHz, 9: 500 kHz, other: Reserved]  
    uint8_t SpreadingFactor;            // LORA [6: 64, 7: 128, 8: 256, 9: 512, 10: 1024, 11: 2048, 12: 4096  chips]
    uint8_t ErrorCoding;                // LORA [1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8]
    bool CrcOn;                         // [0: OFF, 1: ON]
    bool ImplicitHeaderOn;              // [0: OFF, 1: ON]
    bool RxSingleOn;                    // [0: Continuous, 1 Single]
    bool FreqHopOn;                     // [0: OFF, 1: ON]
    uint8_t HopPeriod;                  // Hops every frequency hopping period symbols
    //uint32_t TxPacketTimeout;           //软件发送超时控制
    uint32_t RxPacketTimeout;           //软件接收超时控制
    uint8_t PayloadLength;              //用户数据长度：1-255
    uint16_t PreambleLength;            //前导码长度4-1024
}tLoRaSettings;

typedef union 
{
    uint16_t val;
    uint8_t byte[2];

}xuint16_t;

typedef struct 
{
    xuint16_t source;
    xuint16_t destination;
    uint8_t *data;
    uint8_t len;
}Packet_t;

extern tLoRaSettings LoRaSettings;
extern float RxPacketRssiValue;
extern uint16_t LoRaAddr;
extern Packet_t LoRaPacket;

void SX1278Init();
void SX1278Reset();
void SX1278SetOpMode(uint8_t mode);
void SX1278ClearIRQFlags(uint8_t IrqFlagMask);
void SX1278SetRFFrequency(uint32_t freq);
uint32_t SX1278GetRFFrequency( void );
void SX1278SetRFPower( int8_t power );

void SX1278TxMode() ;
void SX1278Send(uint8_t* pBuffer,uint8_t len);
void SX1278SetTxPacket(uint8_t* pBuffer,uint8_t len);
void SX1278SetTxPacket1(Packet_t* packet);

void SX1278RxMode(bool RxSingleOn);
void SX1278GetRxPacket( void *buffer, uint8_t *size );
void SX1278ForwardPacket();

void SX1278SetRFState( uint8_t state );
uint8_t SX1278GetRFState();
uint8_t SX1278Process( void );
void SaveConfig();
void LoadConfig();

#endif