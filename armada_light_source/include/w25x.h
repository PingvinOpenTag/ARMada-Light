/*****************************************
 *  2048 sectors 4096 bytes per sector
 *
 */




#ifndef W25X_H
#define W25X_H

#include <stdio.h>
#include <stdint.h>

#ifdef __cplusplus
 extern "C" {
  #endif
#include "../FreeRTOS/include/FreeRTOS.h"
#include "../FreeRTOS/include/task.h"
#include "../FreeRTOS/include/queue.h"
#include "../FreeRTOS/include/semphr.h"


#ifdef __cplusplus
  }
#endif



#define W25X_WriteEnable		0x06
#define W25X_WriteDisable		0x04
#define W25X_ReadStatusReg		0x05
#define W25X_ReadStatusReg2		0x35
#define W25X_ReadStatusReg3		0x33
#define W25X_WriteStatusReg		0x01
#define W25X_ReadData			0x03
#define W25X_FastReadData		0x0B
#define W25X_FastReadDual		0x3B
#define W25X_PageProgram		0x02
#define W25X_BlockErase			0xD8
#define W25X_SectorErase		0x20
#define W25X_ChipErase			0xC7
#define W25X_PowerDown			0xB9
#define W25X_ReleasePowerDown	0xAB
#define W25X_DeviceID			0xAB
#define W25X_ManufactDeviceID	0x90
#define W25X_JedecDeviceID		0x9F

#define SPI_FLASH_SECTOR_SIZE				4096
#define SPI_FLASH_PAGE_SIZE					256

#define SECTORS_FOR_ONE_SOUND 32
#define NUMBER_OF_SOUNDS 61

 /*******************
  * 1952 - первый свободный сектор
  * 16 страниц в секторе
  * 31232 - первая свободная страница
  * 2048 - всего секторов в микросхеме
  * *******************/



//s25FL164k

void SPI_FLASH_CS_H(void);
void SPI_FLASH_CS_L(void);
uint8_t SPI_Flash_ReadSR(void);
uint8_t SPI_Flash_ReadSR2(void);
uint8_t SPI_Flash_ReadSR3(void);
void SPI_Flash_Read(uint8_t* pBuffer,uint32_t ReadAddr,uint16_t NumByteToRead);
void SPI_Flash_Read_With_DMA(uint8_t* pBuffer,uint32_t ReadAddr,uint16_t NumByteToRead);
void SPI_FLASH_Write_Enable(void);
void SPI_FLASH_Write_Disable(void);
void SPI_Flash_Write_Page( uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite);
void SPI_Flash_Erase_Sector(uint32_t Dst_Addr);
void SPI_Flash_Wait_Busy(void);
void SPI_Flash_WriteSR(uint8_t byte);

void init_SPI2(void);
uint8_t SPI_Read_Write(uint8_t byte);
uint32_t SPI_Flash_read_ID(void);
void SPI_Flash_Init_Dma_To_Tx( uint16_t len);
void SPI_Flash_Init_Dma_To_Rx( uint8_t* rx_buff_adr, uint16_t len);


#endif
