#ifndef __USART_H
#define __USART_H
#include <stdint.h>
#include <stdlib.h>
#include "stm32f10x.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_gpio.h"

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


  constexpr static uint32_t USART1_RX_SIZE_BUF = 1024;
  constexpr static uint32_t USART1_TX_SIZE_BUF = 1024;
class Usart_1{
public:
	Usart_1(uint32_t boudRate);

	static uint32_t maximum_bytes_in_buff;

	void setBoudRate(uint32_t);
	void iqr_enable(void);

static void RX_IQR_Callback(void);
static void TX_IQR_Callback(void);
//���������� ����������� �������� ����������� ������

static uint32_t GetTxCount(void);

//______________________________________________________________________________
//���������� ����������� �������� ����������� � �������� ������
static uint32_t GetRxCount(void);


static void DisableRxInt(void)  {

	USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);

};

static void EnableRxInt(void)   {

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

};


static void DisableTxInt(void)  {

	USART_ITConfig(USART1, USART_IT_TC, DISABLE);

};

static void EnableTxInt(void)   {

	USART_ITConfig(USART1, USART_IT_TC, ENABLE);

};

//"�������" ���������� �����
static void FlushTxBuf(void)
{
	if (self_pointer==0) return;
	self_pointer->txBufTail = 0;
	self_pointer->txBufHead = 0;
	self_pointer->txCount = 0;

}

//"�������" �������� �����
static void FlushRxBuf(void)
{
	if (self_pointer==0) return;
	self_pointer->rxBufTail = 0;
	self_pointer->rxBufHead = 0;
	self_pointer->rxCount = 0;

}

static void PutChar(unsigned char sym); //�������� ������ � �����

static void SendStr(char * data){
	volatile  unsigned char sym;
	//  while(*data)
	  while((sym = *data++)){

	    PutChar(sym);
	  }


}

static void SlowSendStr(char * data){
	volatile  unsigned char sym;
	//  while(*data)
	  while((sym = *data++)){
		while( GetTxCount());
	    PutChar(sym);
	  }


}


//������ �� ������
static unsigned char GetChar(void);

static xSemaphoreHandle xUsartSemaphore;





private:
	//static USART_TypeDef* pUsart;

	static Usart_1 *self_pointer;
	//���������� �����
	unsigned char TxBuf[USART1_TX_SIZE_BUF];
	/*static*/ uint32_t txBufTail;
	/*static*/ uint32_t txBufHead;
	/*static*/ uint32_t txCount;


	//�������� �����
	unsigned char RxBuf[USART1_RX_SIZE_BUF];
	/*static*/ uint32_t rxBufTail;
	/*static*/ uint32_t rxBufHead;
	/*static*/ uint32_t rxCount;




protected:
};





constexpr static uint32_t USART2_RX_SIZE_BUF = 1024;
constexpr static uint32_t USART2_TX_SIZE_BUF = 1024;
class Usart_2{
public:
	Usart_2(uint32_t boudRate);

	static uint32_t maximum_bytes_in_buff;

	void setBoudRate(uint32_t);
	void iqr_enable(void);

static void RX_IQR_Callback(void);
static void TX_IQR_Callback(void);
//���������� ����������� �������� ����������� ������

static uint32_t GetTxCount(void);

//______________________________________________________________________________
//���������� ����������� �������� ����������� � �������� ������
static uint32_t GetRxCount(void);


static void DisableRxInt(void)  {

	USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);

};

static void EnableRxInt(void)   {

	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

};


static void DisableTxInt(void)  {

	USART_ITConfig(USART2, USART_IT_TC, DISABLE);

};

static void EnableTxInt(void)   {

	USART_ITConfig(USART2, USART_IT_TC, ENABLE);

};

//"�������" ���������� �����
static void FlushTxBuf(void)
{
	if (self_pointer==0) return;
	self_pointer->txBufTail = 0;
	self_pointer->txBufHead = 0;
	self_pointer->txCount = 0;

}

//"�������" �������� �����
static void FlushRxBuf(void)
{
	if (self_pointer==0) return;
	self_pointer->rxBufTail = 0;
	self_pointer->rxBufHead = 0;
	self_pointer->rxCount = 0;

}

static void PutChar(unsigned char sym); //�������� ������ � �����

static void SendStr(char * data){
	volatile  unsigned char sym;
	//  while(*data)
	  while((sym = *data++)){

	    PutChar(sym);
	  }


}

//������ �� ������
static unsigned char GetChar(void);

static xSemaphoreHandle xUsartSemaphore;





private:
	//static USART_TypeDef* pUsart;

	static Usart_2 *self_pointer;
	//���������� �����
	unsigned char TxBuf[USART2_TX_SIZE_BUF];
	/*static*/ uint32_t txBufTail;
	/*static*/ uint32_t txBufHead;
	/*static*/ uint32_t txCount;


	//�������� �����
	unsigned char RxBuf[USART2_RX_SIZE_BUF];
	/*static*/ uint32_t rxBufTail;
	/*static*/ uint32_t rxBufHead;
	/*static*/ uint32_t rxCount;




protected:
};







#endif
