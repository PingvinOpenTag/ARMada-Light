#ifndef _SPI_HPP
#define _SPI_HPP

#include "pin.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_spi.h"



class Spi_1 {


public:
	Spi_1(){

		init();
	};


void init(void){
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

	 SPI_Cmd(SPI1, DISABLE);
		/*Enable or disable APB2 peripheral clock */
	 SPI_I2S_DeInit(SPI1);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_SPI1 | RCC_APB2Periph_AFIO, ENABLE);
	/*Configure GPIO pin */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_6|GPIO_Pin_5;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	// ���������, ��� ���������� �� ������ �������� ������
	    SPIConf.SPI_Direction = SPI_Direction_2Lines_FullDuplex;// SPI_Direction_1Line_Tx;
	    // ���������, ��� ���� ���������� - Master
	    SPIConf.SPI_Mode = SPI_Mode_Master; //	S.SPI_Mode = SPI_Mode_Master;
	    // ���������� ����� �� 8 ��� (=1 ����)
	    SPIConf.SPI_DataSize = SPI_DataSize_8b; //	S.SPI_DataSize = SPI_DataSize_8b;
	    // ����� 00
	    SPIConf.SPI_CPOL = SPI_CPOL_Low;//	S.SPI_CPOL = SPI_CPOL_Low;
	    SPIConf.SPI_CPHA = SPI_CPHA_1Edge;//	S.SPI_CPHA = SPI_CPHA_1Edge;
	    SPIConf.SPI_NSS = SPI_NSS_Soft;//S.SPI_NSS = SPI_NSS_Soft;
	    // ��������� �������� �������� (������� ���� ��������, ��� ������� �� ��������� ����� ��������� ���)
	    SPIConf.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;//		 S.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;

	    // ������� ������ ������� ����� ����� (�.�. ����� �������)
	    SPIConf.SPI_FirstBit = SPI_FirstBit_MSB;//	S.SPI_FirstBit = SPI_FirstBit_MSB;
	    SPIConf.SPI_CRCPolynomial=10;
	// ����� ��������� � SPI
	    SPI_Init(SPI1, &SPIConf);
	    // �������  SPI1
	    SPI_Cmd(SPI1, ENABLE);

	    // ��������� ������ NSS �������������� ����������, ��������� ��� � �������
	    // ���� �������� ��� � ����, �� ��� SPI ������ ��������, ���
	    // � ��� ��������������� ��������� � ��� ������ ���������� �������.

	    // SS = 1
	    SPI_NSSInternalSoftwareConfig(SPI1, SPI_NSSInternalSoft_Set);
		SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, DISABLE);

}

/*******************************************************************************
* Function Name   : SPI_RW
* Description : Sends a byte through the SPI interface and return the byte
*                received from the SPI bus.
* Input       : byte : byte to send.
* Output       : None
* Return       : The value of the received byte.
*******************************************************************************/
u8 readWrite(u8 byte)
{
	/* Loop while DR register in not emplty */
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
	/* Send byte through the SPI1 peripheral */
	SPI_I2S_SendData(SPI1, byte);
	/* Wait to receive a byte */
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
	/* Return the byte read from the SPI bus */
	return SPI_I2S_ReceiveData(SPI1);
}


void Send(uint16_t data) {

	//GPIO_ResetBits(GPIOC, GPIO_Pin_3); // SS = 0
	SPI_I2S_SendData(SPI1, data);  // отправили данные
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET); // ждём, пока данные не отправятся
    //GPIO_SetBits(GPIOC, GPIO_Pin_3);
}


void wait_for_bus_free(void){
	while (!(SPI1->SR & SPI_SR_TXE)); // Wait for bus free
	while (SPI1->SR & SPI_SR_BSY);

}



private:



protected:





};





class Spi_2 {


public:
	Spi_2(){

		init();
	};


void init(void){
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

	 SPI_Cmd(SPI2, DISABLE);
		/*Enable or disable APB2 peripheral clock */
	 SPI_I2S_DeInit(SPI2);
	 RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE );
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
	/*Configure GPIO pin */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	// ���������, ��� ���������� �� ������ �������� ������
	    SPIConf.SPI_Direction = SPI_Direction_2Lines_FullDuplex;// SPI_Direction_1Line_Tx;
	    // ���������, ��� ���� ���������� - Master
	    SPIConf.SPI_Mode = SPI_Mode_Master; //	S.SPI_Mode = SPI_Mode_Master;
	    // ���������� ����� �� 8 ��� (=1 ����)
	    SPIConf.SPI_DataSize = SPI_DataSize_8b; //	S.SPI_DataSize = SPI_DataSize_8b;
	    // ����� 00
	    SPIConf.SPI_CPOL = SPI_CPOL_Low;//	S.SPI_CPOL = SPI_CPOL_Low;
	    SPIConf.SPI_CPHA = SPI_CPHA_1Edge;//	S.SPI_CPHA = SPI_CPHA_1Edge;
	    SPIConf.SPI_NSS = SPI_NSS_Soft;//S.SPI_NSS = SPI_NSS_Soft;
	    // ��������� �������� �������� (������� ���� ��������, ��� ������� �� ��������� ����� ��������� ���)
	    SPIConf.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;//		 S.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;

	    // ������� ������ ������� ����� ����� (�.�. ����� �������)
	    SPIConf.SPI_FirstBit = SPI_FirstBit_MSB;//	S.SPI_FirstBit = SPI_FirstBit_MSB;
	    SPIConf.SPI_CRCPolynomial=10;
	// ����� ��������� � SPI
	    SPI_Init(SPI2, &SPIConf);
	    // �������  SPI1
	    SPI_Cmd(SPI2, ENABLE);

	    // ��������� ������ NSS �������������� ����������, ��������� ��� � �������
	    // ���� �������� ��� � ����, �� ��� SPI ������ ��������, ���
	    // � ��� ��������������� ��������� � ��� ������ ���������� �������.

	    // SS = 1
	    SPI_NSSInternalSoftwareConfig(SPI2, SPI_NSSInternalSoft_Set);
		SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Tx, DISABLE);

}

/*******************************************************************************
* Function Name   : SPI_RW
* Description : Sends a byte through the SPI interface and return the byte
*                received from the SPI bus.
* Input       : byte : byte to send.
* Output       : None
* Return       : The value of the received byte.
*******************************************************************************/
u8 readWrite(u8 byte)
{
	/* Loop while DR register in not emplty */
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
	/* Send byte through the SPI1 peripheral */
	SPI_I2S_SendData(SPI2, byte);
	/* Wait to receive a byte */
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
	/* Return the byte read from the SPI bus */
	return SPI_I2S_ReceiveData(SPI2);
}

void Send(uint16_t data) {

	//GPIO_ResetBits(GPIOC, GPIO_Pin_3); // SS = 0
	SPI_I2S_SendData(SPI2, data);  // отправили данные
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET); // ждём, пока данные не отправятся
    //GPIO_SetBits(GPIOC, GPIO_Pin_3);
}


void wait_for_bus_free(void){
	while (!(SPI2->SR & SPI_SR_TXE)); // Wait for bus free
	while (SPI2->SR & SPI_SR_BSY);

}





private:



protected:





};



class Spi_3 {


public:
	Spi_3(){

		init();
	};


void init(void){
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

	 SPI_Cmd(SPI3, DISABLE);
		/*Enable or disable APB2 peripheral clock */
	 SPI_I2S_DeInit(SPI3);
	 RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE );
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
	/*Configure GPIO pin */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	// ���������, ��� ���������� �� ������ �������� ������
	    SPIConf.SPI_Direction = SPI_Direction_2Lines_FullDuplex;// SPI_Direction_1Line_Tx;
	    // ���������, ��� ���� ���������� - Master
	    SPIConf.SPI_Mode = SPI_Mode_Master; //	S.SPI_Mode = SPI_Mode_Master;
	    // ���������� ����� �� 8 ��� (=1 ����)
	    SPIConf.SPI_DataSize = SPI_DataSize_8b; //	S.SPI_DataSize = SPI_DataSize_8b;
	    // ����� 00
	    SPIConf.SPI_CPOL = SPI_CPOL_Low;//	S.SPI_CPOL = SPI_CPOL_Low;
	    SPIConf.SPI_CPHA = SPI_CPHA_1Edge;//	S.SPI_CPHA = SPI_CPHA_1Edge;
	    SPIConf.SPI_NSS = SPI_NSS_Soft;//S.SPI_NSS = SPI_NSS_Soft;
	    // ��������� �������� �������� (������� ���� ��������, ��� ������� �� ��������� ����� ��������� ���)
	    SPIConf.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;//		 S.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;

	    // ������� ������ ������� ����� ����� (�.�. ����� �������)
	    SPIConf.SPI_FirstBit = SPI_FirstBit_MSB;//	S.SPI_FirstBit = SPI_FirstBit_MSB;
	    SPIConf.SPI_CRCPolynomial=10;
	// ����� ��������� � SPI
	    SPI_Init(SPI3, &SPIConf);
	    // �������  SPI1
	    SPI_Cmd(SPI3, ENABLE);

	    // ��������� ������ NSS �������������� ����������, ��������� ��� � �������
	    // ���� �������� ��� � ����, �� ��� SPI ������ ��������, ���
	    // � ��� ��������������� ��������� � ��� ������ ���������� �������.

	    // SS = 1
	    SPI_NSSInternalSoftwareConfig(SPI3, SPI_NSSInternalSoft_Set);
		SPI_I2S_DMACmd(SPI3, SPI_I2S_DMAReq_Tx, DISABLE);

}



void init_to_16_bit(void)//настройка SPI3
{
	GPIO_InitTypeDef GPIO_InitStruct;
	SPI_InitTypeDef SPIConf;

	SPI_I2S_DeInit(SPI3);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);

	/*Configure GPIO pin */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3|/*GPIO_Pin_4|*/GPIO_Pin_5;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);


	// указываем, что используем мы только передачу данных
	    SPIConf.SPI_Direction = SPI_Direction_1Line_Tx;
	    // указываем, что наше устройство - Master
	    SPIConf.SPI_Mode = SPI_Mode_Master; //	S.SPI_Mode = SPI_Mode_Master;
	    // передавать будем по 8 бит (=1 байт)
	    SPIConf.SPI_DataSize = SPI_DataSize_16b; //	S.SPI_DataSize = SPI_DataSize_8b;
	    // режим 00
	    SPIConf.SPI_CPOL = SPI_CPOL_Low;//	S.SPI_CPOL = SPI_CPOL_Low;
	    SPIConf.SPI_CPHA = SPI_CPHA_1Edge;//	S.SPI_CPHA = SPI_CPHA_1Edge;
	    SPIConf.SPI_NSS = SPI_NSS_Soft;//S.SPI_NSS = SPI_NSS_Soft;
	    // установим скорость передачи (опытным путём выяснили, что разницы от изменения этого параметра нет)
	    SPIConf.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;//		 S.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;

	    // передаём данные старшим битом вперёд (т.е. слева направо)
	    SPIConf.SPI_FirstBit = SPI_FirstBit_MSB;//	S.SPI_FirstBit = SPI_FirstBit_MSB;
	    // внесём настройки в SPI
	    SPI_Init(SPI3, &SPIConf);//SPI_Init(SPI1, &S);
	    // включим  SPI1
	    SPI_Cmd(SPI3, ENABLE); //SPI_Cmd(SPI1, ENABLE);
	    // SS = 1
	    SPI_NSSInternalSoftwareConfig(SPI3, SPI_NSSInternalSoft_Set);

		SPI_I2S_DMACmd(SPI3, SPI_I2S_DMAReq_Tx, DISABLE);

}



/*******************************************************************************
* Function Name   : SPI_RW
* Description : Sends a byte through the SPI interface and return the byte
*                received from the SPI bus.
* Input       : byte : byte to send.
* Output       : None
* Return       : The value of the received byte.
*******************************************************************************/
u8 readWrite(u8 byte)
{
	/* Loop while DR register in not emplty */
	while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) == RESET);
	/* Send byte through the SPI1 peripheral */
	SPI_I2S_SendData(SPI3, byte);
	/* Wait to receive a byte */
	while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_RXNE) == RESET);
	/* Return the byte read from the SPI bus */
	return SPI_I2S_ReceiveData(SPI3);
}

void Send(uint16_t data) {

	//GPIO_ResetBits(GPIOC, GPIO_Pin_3); // SS = 0
	SPI_I2S_SendData(SPI3, data);  // отправили данные
    while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) == RESET); // ждём, пока данные не отправятся
    //GPIO_SetBits(GPIOC, GPIO_Pin_3);
}


void wait_for_bus_free(void){
	while (!(SPI3->SR & SPI_SR_TXE)); // Wait for bus free
	while (SPI3->SR & SPI_SR_BSY);

}


private:



protected:





};







#endif
