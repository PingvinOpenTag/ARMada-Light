#include <stdint.h>
#include <stdlib.h>
#include "usart.h"
#include "stm32f10x.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_gpio.h"



// unsigned char Usart_1::TxBuf[USART1_TX_SIZE_BUF];
// unsigned char Usart_1::RxBuf[USART1_RX_SIZE_BUF];

 xSemaphoreHandle  Usart_1::xUsartSemaphore;

 	//���������� �����


 uint32_t Usart_1::maximum_bytes_in_buff = 0;

 Usart_1 *Usart_1::self_pointer=0;

 void Usart_1::PutChar(unsigned char sym) //�������� ������ � �����
 {

	 if (self_pointer==0) return;
	 if (/*(USART_GetITStatus(USART1, USART_IT_TXE) == SET)*/(USART1->SR/*USART1->SR*/ & USART_SR_TXE)&& (self_pointer->txCount == 0))
 	 {
 			USART_SendData(USART1/*USART1*/, sym);
 	 }
 	 else {
 		taskENTER_CRITICAL();
 		 if (self_pointer->txCount < USART1_TX_SIZE_BUF){    //���� � ������ ��� ���� �����
 			 self_pointer->TxBuf[self_pointer->txBufTail] = sym; //�������� � ���� ������
 			 self_pointer->txCount++;                   //�������������� ������� ��������
 			 self_pointer->txBufTail++;                 //� ������ ������ ������
 			 if (self_pointer->txBufTail == USART1_TX_SIZE_BUF) self_pointer->txBufTail = 0;
 			 }
 		 taskEXIT_CRITICAL();
 		 }

 }



Usart_1::Usart_1(uint32_t boudRate=9600){
	self_pointer = this;
	 txBufTail=0;
	 txBufHead=0;
	 txCount=0;


	 	//�������� �����

	 rxBufTail=0;
	 rxBufHead=0;
	 rxCount=0;




	vSemaphoreCreateBinary(xUsartSemaphore);
	GPIO_InitTypeDef port;
	USART_InitTypeDef usart;

//	rxBufTail = 0;
	//rxBufHead = 0;
//	rxCount = 0;

//	txBufTail = 0;
//	txBufHead = 0;
//	txCount = 0;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA| RCC_APB2Periph_AFIO, ENABLE);
    //���� PA9 � PA10 � ������ �������������� ������� �
         //Rx � Tx USART��
        GPIO_StructInit(&port);
        port.GPIO_Mode = GPIO_Mode_AF_PP;
        port.GPIO_Pin = GPIO_Pin_9;
        port.GPIO_Speed = GPIO_Speed_2MHz;
        GPIO_Init(GPIOA, &port);


        port.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        port.GPIO_Pin = GPIO_Pin_10;
        port.GPIO_Speed = GPIO_Speed_2MHz;
        GPIO_Init(GPIOA, &port);

        //��������� USART, ��� ���� ��������� ����������, ����� �������� ������
        USART_StructInit(&usart);
        usart.USART_BaudRate = boudRate;//BAUDRATE;
        USART_Init(USART1, &usart);
        USART_Cmd(USART1, ENABLE);

}





uint32_t Usart_1::GetTxCount(void)
{
	if(self_pointer==0) return 0;
 return self_pointer->txCount;
}

uint32_t Usart_1::GetRxCount(void)
{
	if(self_pointer==0) return 0;
  return self_pointer->rxCount;
}

unsigned char Usart_1::GetChar(void)
{
  unsigned char sym;
  if(self_pointer==0) return '\0';
  if (self_pointer->rxCount > 0){                     //���� �������� ����� �� ������
    sym = self_pointer->RxBuf[self_pointer->rxBufHead];        //��������� �� ���� ������
    self_pointer->rxCount--;                          //��������� ������� ��������
    self_pointer->rxBufHead++;                        //���������������� ������ ������ ������
    if (self_pointer->rxBufHead == USART1_RX_SIZE_BUF) self_pointer->rxBufHead = 0;
    return sym;                         //������� ����������� ������
  }
  return 0;
}
void Usart_1::setBoudRate(uint32_t boudRate){

	USART_InitTypeDef usart;

        //��������� USART, ��� ���� ��������� ����������, ����� �������� ������
        USART_StructInit(&usart);
        usart.USART_BaudRate = boudRate;//BAUDRATE;
        USART_Init(USART1, &usart);

}

void Usart_1::iqr_enable(void){
		//�������� ���������� �� ������ ����� � �� ��������� ��������
		    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
		    USART_ITConfig(USART1, USART_IT_TC, ENABLE);
		    NVIC_SetPriority(USART1_IRQn, 12);
		    NVIC_EnableIRQ(USART1_IRQn);


}

void Usart_1::RX_IQR_Callback(void){
	static portBASE_TYPE xHigherPriorityTaskWoken;
		  xHigherPriorityTaskWoken = pdFALSE;
		  if (self_pointer==0) {

			  USART_ClearITPendingBit(USART1, USART_IT_RXNE);
			  return;}

	  if (self_pointer->rxCount < USART1_RX_SIZE_BUF)  {              //���� � ������ ��� ���� �����
		  self_pointer->RxBuf[self_pointer->rxBufTail] = USART_ReceiveData(USART1);        //������� ������ �� UDR � �����
		  self_pointer->rxBufTail++;                             //��������� ������ ������ ��������� ������
	      if (self_pointer->rxBufTail == USART1_RX_SIZE_BUF) self_pointer->rxBufTail = 0;
	      self_pointer->rxCount++;                                 //��������� ������� �������� ��������
	      if (self_pointer->rxCount > maximum_bytes_in_buff) maximum_bytes_in_buff = self_pointer->rxCount;
		  xSemaphoreGiveFromISR( xUsartSemaphore, &xHigherPriorityTaskWoken );
		  if (xHigherPriorityTaskWoken != pdFALSE) {
		      portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
		    }


	  }
}

void Usart_1::TX_IQR_Callback(void){

	 if (self_pointer==0) return;
	if (self_pointer->txCount > 0){              //���� ����� �� ������
	//    	    UDR = usartTxBuf[txBufHead]; //���������� � UDR ������ �� ������
	    	    USART_SendData(USART1, self_pointer->TxBuf[self_pointer->txBufHead]);//���������� � UDR ������ �� ������
	    	    self_pointer->txCount--;                   //��������� ������� ��������
	    	    self_pointer->txBufHead++;                 //�������������� ������ ������ ������
	    	    if (self_pointer->txBufHead == USART1_TX_SIZE_BUF) self_pointer->txBufHead = 0;
	    	  }
}






xSemaphoreHandle  Usart_2::xUsartSemaphore;

	//���������� �����


uint32_t Usart_2::maximum_bytes_in_buff = 0;

Usart_2 *Usart_2::self_pointer=0;

void Usart_2::PutChar(unsigned char sym) //�������� ������ � �����
{

	 if (self_pointer==0) return;
	 if (/*(USART_GetITStatus(USART1, USART_IT_TXE) == SET)*/(USART2->SR/*USART1->SR*/ & USART_SR_TXE)&& (self_pointer->txCount == 0))
	 {
			USART_SendData(USART2/*USART1*/, sym);
	 }
	 else {
		taskENTER_CRITICAL();
		 if (self_pointer->txCount < USART2_TX_SIZE_BUF){    //���� � ������ ��� ���� �����
			 self_pointer->TxBuf[self_pointer->txBufTail] = sym; //�������� � ���� ������
			 self_pointer->txCount++;                   //�������������� ������� ��������
			 self_pointer->txBufTail++;                 //� ������ ������ ������
			 if (self_pointer->txBufTail == USART2_TX_SIZE_BUF) self_pointer->txBufTail = 0;
			 }
		 taskEXIT_CRITICAL();
		 }

}



Usart_2::Usart_2(uint32_t boudRate=9600){
	self_pointer = this;
	 txBufTail=0;
	 txBufHead=0;
	 txCount=0;


	 	//�������� �����

	 rxBufTail=0;
	 rxBufHead=0;
	 rxCount=0;




	vSemaphoreCreateBinary(xUsartSemaphore);
	GPIO_InitTypeDef port;
	USART_InitTypeDef usart;

//	rxBufTail = 0;
	//rxBufHead = 0;
//	rxCount = 0;

//	txBufTail = 0;
//	txBufHead = 0;
//	txCount = 0;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA| RCC_APB2Periph_AFIO, ENABLE);

    //���� PA2 � PA3 � ������ �������������� ������� �
     //Rx � Tx USART��
    GPIO_StructInit(&port);
    port.GPIO_Mode = GPIO_Mode_AF_PP;
    port.GPIO_Pin = GPIO_Pin_2;
    port.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOA, &port);


    port.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    port.GPIO_Pin = GPIO_Pin_3;
    port.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOA, &port);

    USART_StructInit(&usart);
    usart.USART_BaudRate = boudRate;//BAUDRATE;
    USART_Init(USART2, &usart);
    USART_Cmd(USART2, ENABLE);

}





uint32_t Usart_2::GetTxCount(void)
{
	if(self_pointer==0) return 0;
return self_pointer->txCount;
}

uint32_t Usart_2::GetRxCount(void)
{
	if(self_pointer==0) return 0;
 return self_pointer->rxCount;
}

unsigned char Usart_2::GetChar(void)
{
 unsigned char sym;
 if(self_pointer==0) return '\0';
 if (self_pointer->rxCount > 0){                     //���� �������� ����� �� ������
   sym = self_pointer->RxBuf[self_pointer->rxBufHead];        //��������� �� ���� ������
   self_pointer->rxCount--;                          //��������� ������� ��������
   self_pointer->rxBufHead++;                        //���������������� ������ ������ ������
   if (self_pointer->rxBufHead == USART2_RX_SIZE_BUF) self_pointer->rxBufHead = 0;
   return sym;                         //������� ����������� ������
 }
 return 0;
}
void Usart_2::setBoudRate(uint32_t boudRate){

	USART_InitTypeDef usart;

       //��������� USART, ��� ���� ��������� ����������, ����� �������� ������
       USART_StructInit(&usart);
       usart.USART_BaudRate = boudRate;//BAUDRATE;
       USART_Init(USART2, &usart);

}

void Usart_2::iqr_enable(void){
		//�������� ���������� �� ������ ����� � �� ��������� ��������
		    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
		    USART_ITConfig(USART2, USART_IT_TC, ENABLE);
		    NVIC_SetPriority(USART2_IRQn, 12);
		    NVIC_EnableIRQ(USART2_IRQn);


}

void Usart_2::RX_IQR_Callback(void){
	static portBASE_TYPE xHigherPriorityTaskWoken;
		  xHigherPriorityTaskWoken = pdFALSE;
		  if (self_pointer==0) {

			  USART_ClearITPendingBit(USART2, USART_IT_RXNE);
			  return;}

	  if (self_pointer->rxCount < USART1_RX_SIZE_BUF)  {              //���� � ������ ��� ���� �����
		  self_pointer->RxBuf[self_pointer->rxBufTail] = USART_ReceiveData(USART2);        //������� ������ �� UDR � �����
		  self_pointer->rxBufTail++;                             //��������� ������ ������ ��������� ������
	      if (self_pointer->rxBufTail == USART2_RX_SIZE_BUF) self_pointer->rxBufTail = 0;
	      self_pointer->rxCount++;                                 //��������� ������� �������� ��������
	      if (self_pointer->rxCount > maximum_bytes_in_buff) maximum_bytes_in_buff = self_pointer->rxCount;
		  xSemaphoreGiveFromISR( xUsartSemaphore, &xHigherPriorityTaskWoken );
		  if (xHigherPriorityTaskWoken != pdFALSE) {
		      portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
		    }


	  }
}

void Usart_2::TX_IQR_Callback(void){

	 if (self_pointer==0) return;
	if (self_pointer->txCount > 0){              //���� ����� �� ������
	//    	    UDR = usartTxBuf[txBufHead]; //���������� � UDR ������ �� ������
	    	    USART_SendData(USART2, self_pointer->TxBuf[self_pointer->txBufHead]);//���������� � UDR ������ �� ������
	    	    self_pointer->txCount--;                   //��������� ������� ��������
	    	    self_pointer->txBufHead++;                 //�������������� ������ ������ ������
	    	    if (self_pointer->txBufHead == USART2_TX_SIZE_BUF) self_pointer->txBufHead = 0;
	    	  }
}



#ifdef __cplusplus
 extern "C" {
  #endif
void USART1_IRQHandler()
{

	if (USART_GetITStatus(USART1, USART_IT_TC) != RESET)
	    {
			USART_ClearITPendingBit(USART1, USART_IT_TC);
			Usart_1::TX_IQR_Callback();

	    }
	//���������, ������������� �� ���������� ������� ������� ������ �����
	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	    {
		 Usart_1::RX_IQR_Callback();
	    }
	if (USART1->SR & USART_SR_ORE)
	    {
	    	USART_ClearITPendingBit(USART1, USART_IT_ORE);
	    }



}



void USART3_IRQHandler()
{
	if (USART_GetITStatus(USART3, USART_IT_TC) != RESET)
		    {
				USART_ClearITPendingBit(USART3, USART_IT_TC);
				//Usart::TX_IQR_Callback();

		    }
	if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
	    {
		 //Usart::RX_IQR_Callback();
	    }
	if (USART2->SR & USART_SR_ORE)
	    {
	    	USART_ClearITPendingBit(USART3, USART_IT_ORE);
	    }


}

void USART2_IRQHandler()
{
	 if (USART_GetITStatus(USART2, USART_IT_TC) != RESET)
	    {
		 Usart_2::TX_IQR_Callback();
		 USART_ClearITPendingBit(USART2, USART_IT_TC);

	    }
	//���������, ������������� �� ���������� ������� ������� ������ �����
	    if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	    {
	    	Usart_2::RX_IQR_Callback();
	    	//USART_ClearITPendingBit(USART2, USART_IT_RXNE);
	    }
	    if (USART2->SR & USART_SR_ORE)
        {
        	USART_ClearITPendingBit(USART2, USART_IT_ORE);
        }

}




#ifdef __cplusplus
  }
#endif



