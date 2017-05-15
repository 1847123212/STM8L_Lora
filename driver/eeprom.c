#include "eeprom.h"

uint8_t EEPROM_Read(uint16_t Addr,uint8_t *RxBuffer,uint8_t Length)  
{  
    uint16_t T=0;  
    uint8_t *EEP;  
    EEP=(uint8_t *)(EEPROM_STARTADDR+Addr);  
    //FLASH->CR1 &= (uint8_t)(~FLASH_CR1_FIX);
    //FLASH->CR1 |= (uint8_t)FLASH_ProgramTime_Standard;
    FLASH->DUKR = FLASH_RASS_KEY2; /* Warning: keys are reversed on data memory !!! */
    FLASH->DUKR = FLASH_RASS_KEY1;
    
    while((FLASH->IAPSR  & (uint8_t)FLASH_FLAG_DUL) == (uint8_t)RESET)
    //while (FLASH_GetFlagStatus(FLASH_FLAG_DUL) == RESET)
    {nop();}    
    //FLASH->CR2=0x01;  //Standard programming mode
    //if(T==0xFFFF)return 0;  
    while(Length--){  
        *RxBuffer++=*EEP++;  
    }  
    return 1;  
}  

uint8_t EEPROM_Write(uint16_t Addr,uint8_t *RxBuffer,uint8_t Lenth)  
{  
    uint16_t T=0;  
    uint8_t *EEP;  
    EEP=(uint8_t *)(EEPROM_STARTADDR+Addr);  
    FLASH->DUKR = FLASH_RASS_KEY2; /* Warning: keys are reversed on data memory !!! */
    FLASH->DUKR = FLASH_RASS_KEY1;
    /* Wait until Data EEPROM area unlocked flag is set*/
    while((FLASH->IAPSR  & (uint8_t)FLASH_FLAG_DUL) == (uint8_t)RESET)
    {nop();}
    while(Lenth--){  
        *EEP++=*RxBuffer++;  
        //T=0;  
        //if(T==0xFFFF)return 0;  
    }  
    while((!(FLASH->IAPSR&0x04))&&(T<0xFFFF))T++;//Wait until End of high voltage flag is set  
    //FLASH->IAPSR&=~S3;  //硬件自动完成
    return 1;  
}  