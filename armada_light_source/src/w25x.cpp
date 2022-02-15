#include <stdio.h>
#include <stdint.h>
#include "w25x.h"

#ifdef __cplusplus
 extern "C" {
  #endif


#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_spi.h"
#ifdef __cplusplus
  }
#endif


void SPI_FLASH_CS_H(void){
	GPIO_SetBits(GPIOB, GPIO_Pin_12);
}

void SPI_FLASH_CS_L(void){
//	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
	GPIO_ResetBits(GPIOB, GPIO_Pin_12);
}

uint8_t SPI_Read_Write(uint8_t byte){

	/* Loop while DR register in not emplty */
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
	/* Send byte through the SPI1 peripheral */
	////SPI_I2S_SendData16(SPI1, byte);
	*((__IO uint8_t *)&SPI2->DR) = byte;
	//SPI_I2S_SendData(SPI2, byte);
	//	SPI_SendData8(SPI1, byte);
	/* Wait to receive a byte */
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
	/* Return the byte read from the SPI bus */
	return SPI_I2S_ReceiveData(SPI2);
//	return (uint8_t)SPI_ReceiveData8(SPI1);

}

uint8_t SPI_Flash_ReadSR(void){
		uint8_t bteFF = 0xFF;
		uint8_t bteREC;
		SPI_FLASH_CS_L();
//		uint8_t bteTR = W25X_ReadStatusReg;
		/* Loop while DR register in not emplty */
////		while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
		/* Send byte through the SPI1 peripheral */
////		SPI_I2S_SendData16(SPI1,(uint8_t)W25X_ReadStatusReg);
////		while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
////		bteREC = SPI_I2S_ReceiveData16(SPI1);
		//HAL_SPI_Transmit(&hspi1, &bteTR, 1, 100);
		//HAL_SPI_TransmitReceive(&hspi1, &bteFF, &bteREC, 1, 100);
		bteREC=SPI_Read_Write((uint8_t)W25X_ReadStatusReg);
		if (bteREC!=0)
		{
			volatile int y;
			y++;
		}
		bteREC = SPI_Read_Write((uint8_t)bteFF);

		SPI_FLASH_CS_H();
		return bteREC;
}


uint8_t SPI_Flash_ReadSR2(void){
		uint8_t bteFF = 0xFF;
		uint8_t bteREC;
		SPI_FLASH_CS_L();
//		uint8_t bteTR = W25X_ReadStatusReg;
		/* Loop while DR register in not emplty */
////		while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
		/* Send byte through the SPI1 peripheral */
////		SPI_I2S_SendData16(SPI1,(uint8_t)W25X_ReadStatusReg);
////		while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
////		bteREC = SPI_I2S_ReceiveData16(SPI1);
		//HAL_SPI_Transmit(&hspi1, &bteTR, 1, 100);
		//HAL_SPI_TransmitReceive(&hspi1, &bteFF, &bteREC, 1, 100);
		bteREC=SPI_Read_Write((uint8_t)W25X_ReadStatusReg2);
		if (bteREC!=0)
		{
			volatile int y;
			y++;
		}
		bteREC = SPI_Read_Write((uint8_t)bteFF);
		SPI_FLASH_CS_H();
		return bteREC;
}


uint8_t SPI_Flash_ReadSR3(void){
		uint8_t bteFF = 0xFF;
		uint8_t bteREC;
		SPI_FLASH_CS_L();

		bteREC=SPI_Read_Write((uint8_t)W25X_ReadStatusReg3);
		if (bteREC!=0)
		{
			volatile int y;
			y++;
		}
		bteREC = SPI_Read_Write((uint8_t)bteFF);
		SPI_FLASH_CS_H();
		return bteREC;
}



void SPI_Flash_Read(uint8_t* pBuffer,uint32_t ReadAddr,uint16_t NumByteToRead){

	uint8_t bteTR;
	uint8_t bteFF = 0xFF;
//	uint8_t bteREC;
	SPI_FLASH_CS_L();
////	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
//	bteTR = W25X_ReadData;
////	SPI_I2S_SendData16(SPI1,W25X_ReadData);
////	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
//	HAL_SPI_Transmit(&hspi1, &bteTR, 1, 100);

	SPI_Read_Write(W25X_ReadData);

	bteTR = (uint8_t)((ReadAddr) >> 16);
	SPI_Read_Write(bteTR);

////	SPI_I2S_SendData16(SPI1,bteTR);
////	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
//	HAL_SPI_Transmit(&hspi1, &bteTR, 1, 100);
	bteTR = (uint8_t)((ReadAddr) >> 8);
	SPI_Read_Write(bteTR);
////	SPI_I2S_SendData16(SPI1,bteTR);
////	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
//	HAL_SPI_Transmit(&hspi1, &bteTR, 1, 100);
	bteTR = (uint8_t) ReadAddr;
	SPI_Read_Write(bteTR);
////	SPI_I2S_SendData16(SPI1,bteTR);
////	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
	//HAL_SPI_Transmit(&hspi1, &bteTR, 1, 100);

	uint16_t i;

	for (i = 0; i < NumByteToRead; i++)
	{


		////	pBuffer[i]=SPI2_ReadWriteByte(0XFF);   //????
//		HAL_SPI_TransmitReceive(&hspi1, &bteFF, &bteREC, 1, 100);
//		pBuffer[i] = bteREC;
		////SPI_I2S_SendData16(SPI1,bteFF);
		////while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
		////bteREC = (uint8_t)SPI_I2S_ReceiveData16(SPI1);
		////pBuffer[i]=bteREC;
		pBuffer[i]=SPI_Read_Write(bteFF);
	}


/*
	while (NumByteToRead){
		bteREC = SPI_Read_Write(bteFF);
		if (NumByteToRead >1){




		}
		else{
			SPI1->CR2 |=
		}



	}
*/

	SPI_FLASH_CS_H();
}



void SPI_Flash_Read_With_DMA(uint8_t* pBuffer,uint32_t ReadAddr,uint16_t NumByteToRead){

	uint8_t bteTR;
	//uint8_t bteFF = 0xFF;

	SPI_FLASH_CS_L();

	SPI_Read_Write(W25X_ReadData);

	bteTR = (uint8_t)((ReadAddr) >> 16);
	SPI_Read_Write(bteTR);

	bteTR = (uint8_t)((ReadAddr) >> 8);
	SPI_Read_Write(bteTR);
	bteTR = (uint8_t) ReadAddr;
	SPI_Read_Write(bteTR);

	DMA_Cmd(DMA1_Channel4, DISABLE);
	DMA_Cmd(DMA1_Channel5, DISABLE);

	SPI_Flash_Init_Dma_To_Tx(NumByteToRead);
	SPI_Flash_Init_Dma_To_Rx(pBuffer,NumByteToRead);

//	SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Rx, ENABLE);
//	SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Tx, ENABLE);

	DMA_Cmd(DMA1_Channel4, ENABLE);
	DMA_Cmd(DMA1_Channel5, ENABLE);

	while (!DMA_GetFlagStatus(DMA1_FLAG_TC4));

	SPI_FLASH_CS_H();

}



void SPI_FLASH_Write_Enable(void){
	//	uint8_t bteTR;
		SPI_FLASH_CS_L();
	//	bteTR = W25X_WriteEnable;
		SPI_Read_Write((uint8_t)W25X_WriteEnable);
	//	SPI_Read_Write(bteTR);
		SPI_FLASH_CS_H();
}

void SPI_FLASH_Write_Disable(void){
	//uint8_t bteTR;
	SPI_FLASH_CS_L();
	//bteTR = W25X_WriteEnable;
	SPI_Read_Write((uint8_t)W25X_WriteDisable);
	SPI_FLASH_CS_H();

}

void SPI_Flash_WriteSR(uint8_t byte){
	SPI_FLASH_Write_Enable();
	SPI_FLASH_CS_L();
	SPI_Read_Write((uint8_t)W25X_WriteStatusReg);
	SPI_Read_Write(byte);
	SPI_FLASH_CS_H();
	SPI_Flash_Wait_Busy();

}


void SPI_Flash_Write_Page( uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite){

	SPI_FLASH_Write_Enable();

		SPI_FLASH_CS_L();

		uint8_t bteTR;
//		HAL_SPI_Transmit(&hspi1, &bteTR, 1, 100);
		SPI_Read_Write((uint8_t)W25X_PageProgram);

/*
		bteTR = (uint8_t)((WriteAddr) >> 16);
		HAL_SPI_Transmit(&hspi1, &bteTR, 1, 100);
		bteTR = (uint8_t)((WriteAddr) >> 8);
		HAL_SPI_Transmit(&hspi1, &bteTR, 1, 100);
		bteTR = (uint8_t) WriteAddr;
		HAL_SPI_Transmit(&hspi1, &bteTR, 1, 100);
*/

		bteTR = (uint8_t)((WriteAddr) >> 16);
		SPI_Read_Write(bteTR);
		bteTR = (uint8_t)((WriteAddr) >> 8);
		SPI_Read_Write(bteTR);
		bteTR = (uint8_t) WriteAddr;
		SPI_Read_Write(bteTR);

		//HAL_SPI_Transmit(&hspi1, pBuffer, NumByteToWrite, 100);
		uint16_t i;
		for (i = 0; i < NumByteToWrite; i++)
		{
//			HAL_SPI_Transmit(&hspi1, pBuffer + i, 1, 100);
			bteTR = *(pBuffer + i);
			SPI_Read_Write(bteTR);
		}

		SPI_FLASH_CS_H();
		SPI_Flash_Wait_Busy();



}

void SPI_Flash_Erase_Sector(uint32_t Dst_Addr){
	Dst_Addr *= 4096;
	SPI_FLASH_Write_Enable(); //SET WEL
	SPI_Flash_Wait_Busy();
	SPI_FLASH_CS_L();

	uint8_t bteTR = W25X_SectorErase;
//	HAL_SPI_Transmit(&hspi1, &bteTR, 1, 100);
	SPI_Read_Write(bteTR);
	bteTR = (uint8_t)((Dst_Addr) >> 16);
//	HAL_SPI_Transmit(&hspi1, &bteTR, 1, 100);
	SPI_Read_Write(bteTR);
	bteTR = (uint8_t)((Dst_Addr) >> 8);
	SPI_Read_Write(bteTR);
//	HAL_SPI_Transmit(&hspi1, &bteTR, 1, 100);
	bteTR = (uint8_t) Dst_Addr;
	SPI_Read_Write(bteTR);
//	HAL_SPI_Transmit(&hspi1, &bteTR, 1, 100);

	SPI_FLASH_CS_H();

	SPI_Flash_Wait_Busy();
}

void SPI_Flash_Wait_Busy(void){
	while ((SPI_Flash_ReadSR() & 0x01) == 0x01);
}

//extern GPIO_InitTypeDef port;
void init_SPI2(void){

	GPIO_InitTypeDef GPIO_InitStruct;
	SPI_InitTypeDef SPIConf;

	/** SPI2 GPIO Configuration
		 PB13	 ------> SPI2_SCK
		 PB15	 ------> SPI2_MOSI
		 PB14	 ------> SPI2_MISO
	*/
	/** SPI3 GPIO Configuration
			 PB3	 ------> SPI2_SCK
			 PB5	 ------> SPI2_MOSI
			 PB4	 ------> SPI2_MISO
		*/
	/** SPI2 GPIO Configuration
				 PA5	 ------> SPI1_SCK
				 PA6	 ------> SPI1_MISO
				 PA7	 ------> SPI1_MOSI
	*/


    SPI_Cmd(SPI2, DISABLE);
    SPI_I2S_DeInit(SPI2);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB |/* RCC_APB2Periph_GPIOC |*/ RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
    /*Configure GPIO pin */
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStruct);


    // указываем, что используем мы только передачу данных
    SPIConf.SPI_Direction = SPI_Direction_2Lines_FullDuplex;// SPI_Direction_1Line_Tx;
    // указываем, что наше устройство - Master
    SPIConf.SPI_Mode = SPI_Mode_Master; //	S.SPI_Mode = SPI_Mode_Master;
    // передавать будем по 8 бит (=1 байт)
    SPIConf.SPI_DataSize = SPI_DataSize_8b; //	S.SPI_DataSize = SPI_DataSize_8b;
    // режим 00
    SPIConf.SPI_CPOL = SPI_CPOL_Low;//	S.SPI_CPOL = SPI_CPOL_Low;
    SPIConf.SPI_CPHA = SPI_CPHA_1Edge;//	S.SPI_CPHA = SPI_CPHA_1Edge;
    SPIConf.SPI_NSS = SPI_NSS_Soft;//S.SPI_NSS = SPI_NSS_Soft;
    // установим скорость передачи (опытным путём выяснили, что разницы от изменения этого параметра нет)
    SPIConf.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;//		 S.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;

    // передаём данные старшим битом вперёд (т.е. слева направо)
    SPIConf.SPI_FirstBit = SPI_FirstBit_MSB;//	S.SPI_FirstBit = SPI_FirstBit_MSB;
    SPIConf.SPI_CRCPolynomial=7;//10;
    // внесём настройки в SPI
    SPI_Init(SPI2, &SPIConf);
    // включим  SPI2
    SPI_Cmd(SPI2, ENABLE);

    // Поскольку сигнал NSS контролируется программно, установим его в единицу
    // Если сбросить его в ноль, то наш SPI модуль подумает, что
    // у нас мультимастерная топология и его лишили полномочий мастера.
    // SS = 1
    SPI_NSSInternalSoftwareConfig(SPI2, SPI_NSSInternalSoft_Set);
    //SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Tx, DISABLE);

	SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Rx, ENABLE);
	SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Tx, ENABLE);

			/*Enable or disable APB2 peripheral clock */
//			GPIO_InitTypeDef GPIO_InitStruct;
//			SPI_InitTypeDef SPIConf;

/*
			RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
			RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
*/
			  /*!< Connect SPI pins to AF*/
/*			GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_0);
			GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_0);
			GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_0);
*/
			/*Configure GPIO pin */
/*			port.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
			port.GPIO_Mode = GPIO_Mode_AF;
			port.GPIO_Speed = GPIO_Speed_50MHz;
			port.GPIO_OType=  GPIO_OType_PP;
			port.GPIO_PuPd=GPIO_PuPd_DOWN;

			GPIO_Init(GPIOA, &port);
*/
			/* CS pin*/
	/*		port.GPIO_Pin = GPIO_Pin_0;
			port.GPIO_Mode = GPIO_Mode_OUT;
			port.GPIO_OType=  GPIO_OType_PP;
			port.GPIO_Speed = GPIO_Speed_50MHz;
			port.GPIO_PuPd=GPIO_PuPd_NOPULL;

			GPIO_Init(GPIOB, &port);
			SPI_FLASH_CS_H();

//			GPIO_SetBits(GPIOC, GPIO_Pin_3);

			// указываем, что используем приём и передачу данных
			    SPIConf.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
			    // указываем, что наше устройство - Master
			    SPIConf.SPI_Mode = SPI_Mode_Master;
			    // передавать будем по 8 бит (=1 байт)
			    SPIConf.SPI_DataSize = SPI_DataSize_8b;

			    // режим 00
			    SPIConf.SPI_CPOL = SPI_CPOL_Low;
			    SPIConf.SPI_CPHA = SPI_CPHA_1Edge;
			    SPIConf.SPI_NSS = SPI_NSS_Soft;
			    // установим скорость передачи (опытным путём выяснили, что разницы от изменения этого параметра нет)
			    SPIConf.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
			    // передаём данные старшим битом вперёд (т.е. слева направо)
			    SPIConf.SPI_FirstBit = SPI_FirstBit_MSB;
			    SPIConf.SPI_CRCPolynomial=7;
			    // внесём настройки в SPI
			    SPI_Init(SPI1, &SPIConf);
			    // включим  SPI1
			    SPI_Cmd(SPI1, ENABLE);
			    // SS = 1
			    SPI1->CR2|=SPI_RxFIFOThreshold_QF;
			    SPI_NSSInternalSoftwareConfig(SPI1, SPI_NSSInternalSoft_Set);
				SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Tx, DISABLE);
*/
}


uint32_t SPI_Flash_read_ID(void){
	 uint32_t Temp = 0, Temp0 = 0, Temp1 = 0, Temp2 = 0;
	 uint8_t bteFF = 0xFF;
	 SPI_FLASH_CS_L();
	 SPI_Read_Write(0x9F);
	 Temp0 = SPI_Read_Write(bteFF);
	 Temp1 = SPI_Read_Write(bteFF);
	 Temp2 = SPI_Read_Write(bteFF);
	 SPI_FLASH_CS_H();
	 Temp = (Temp0 << 16) | (Temp1 << 8) | Temp2;
	 return Temp;
}


void SPI_Flash_Init_Dma_To_Tx(uint16_t len){
	static uint8_t _filler = 0xFF;
	DMA_InitTypeDef  DMA_to_SPI_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

//	DMA_Cmd(DMA1_Channel5, DISABLE);

	DMA_DeInit ( DMA1_Channel5 );
	DMA_to_SPI_InitStructure.DMA_PeripheralBaseAddr =(uint32_t)&(SPI2->DR);
	DMA_to_SPI_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&_filler;
	DMA_to_SPI_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_to_SPI_InitStructure.DMA_BufferSize = len;
	DMA_to_SPI_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_to_SPI_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
	DMA_to_SPI_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_to_SPI_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_to_SPI_InitStructure.DMA_Mode = DMA_Mode_Normal;//DMA_Mode_Circular;

	DMA_to_SPI_InitStructure.DMA_Priority = DMA_Priority_High;/*DMA_Priority_Low*/;
	DMA_to_SPI_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel5, &DMA_to_SPI_InitStructure);

	//NVIC_EnableIRQ(DMA1_Channel5_IRQn);
}

void SPI_Flash_Init_Dma_To_Rx( uint8_t* rx_buff_adr, uint16_t len){
	DMA_InitTypeDef  DMA_to_SPI_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

//	DMA_Cmd(DMA1_Channel4, DISABLE);

	DMA_DeInit ( DMA1_Channel4 );
	DMA_to_SPI_InitStructure.DMA_PeripheralBaseAddr =(uint32_t)&(SPI2->DR);
	DMA_to_SPI_InitStructure.DMA_MemoryBaseAddr = (uint32_t) rx_buff_adr;
	DMA_to_SPI_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_to_SPI_InitStructure.DMA_BufferSize = len;
	DMA_to_SPI_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_to_SPI_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_to_SPI_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_to_SPI_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_to_SPI_InitStructure.DMA_Mode = DMA_Mode_Normal;//DMA_Mode_Circular;

	DMA_to_SPI_InitStructure.DMA_Priority = DMA_Priority_High;/*DMA_Priority_Low*/;
	DMA_to_SPI_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel4, &DMA_to_SPI_InitStructure);
	//NVIC_EnableIRQ(DMA1_Channel4_IRQn);

}

