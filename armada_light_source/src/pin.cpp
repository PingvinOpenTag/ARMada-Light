#include <stdio.h>
#include "pin.h"
#include <stdint.h>
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_exti.h"



Pin::Pin(uint8_t port, uint8_t pin){
m_port=port;
m_pin=pin;

// Enable GPIO Peripheral clock

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA << (port), ENABLE);


}


void Pin::switchToOutput(){
GPIO_InitTypeDef GPIO_InitStructure;

  // Configure pin in output push/pull mode
  GPIO_InitStructure.GPIO_Pin = (1 << m_pin);
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(((GPIO_TypeDef *)(GPIOA_BASE + (GPIOB_BASE-GPIOA_BASE)*(m_port))), &GPIO_InitStructure);

}

void Pin::switchToOutputOD(){
	GPIO_InitTypeDef GPIO_InitStructure;
	  // Configure pin in output push/pull mode
	  GPIO_InitStructure.GPIO_Pin = (1 << m_pin);
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	  GPIO_Init(((GPIO_TypeDef *)(GPIOA_BASE + (GPIOB_BASE-GPIOA_BASE)*(m_port))), &GPIO_InitStructure);


}
void Pin::switchToInput(){
GPIO_InitTypeDef GPIO_InitStructure;

  // Configure pin in output push/pull mode
  GPIO_InitStructure.GPIO_Pin = (1 << m_pin);
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
  GPIO_Init(((GPIO_TypeDef *)(GPIOA_BASE + (GPIOB_BASE-GPIOA_BASE)*(m_port))), &GPIO_InitStructure);

}

void Pin::switchToInputPU(){
GPIO_InitTypeDef GPIO_InitStructure;

  // Configure pin in output push/pull mode
  GPIO_InitStructure.GPIO_Pin = (1 << m_pin);
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(((GPIO_TypeDef *)(GPIOA_BASE + (GPIOB_BASE-GPIOA_BASE)*(m_port))), &GPIO_InitStructure);

}



void Pin::set(){
GPIO_SetBits(((GPIO_TypeDef *)(GPIOA_BASE + (GPIOB_BASE-GPIOA_BASE)*(m_port))),
        (1 << m_pin));
}

bool Pin::isSet(){
	return GPIO_ReadInputDataBit(((GPIO_TypeDef *)(GPIOA_BASE + (GPIOB_BASE-GPIOA_BASE)*(m_port))),
			        (1 << m_pin));

}

void Pin::reset(){
GPIO_ResetBits(((GPIO_TypeDef *)(GPIOA_BASE + (GPIOB_BASE-GPIOA_BASE)*(m_port))),
        (1 << m_pin));

}

void Pin::setCallback(TPIN_IRQ_CALLBACK/*ExtiCallbackFuncPointer*/ pointer){
  callbecksPointers[m_port][m_pin] = pointer;
}



TPIN_IRQ_CALLBACK/*ExtiCallbackFuncPointer*/ Pin::callbecksPointers[4][16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

void Pin::extiCallback(uint8_t line_number){

	for(uint8_t i=0; i<4;i++ ){
	if(callbecksPointers[i][line_number]!=0)
    //  return;
      //else {
              (callbecksPointers[i][line_number])->pin_irq_callback();
      //}
	}
};

#ifdef __cplusplus
 extern "C" {
  #endif


 void EXTI0_IRQHandler(void){
	 EXTI_ClearFlag(EXTI_Line0);
	 Pin::extiCallback(0);
 }

void  EXTI1_IRQHandler(void){
	EXTI_ClearFlag(EXTI_Line1);
	 Pin::extiCallback(1);
 }

void  EXTI2_IRQHandler(void){
	EXTI_ClearFlag(EXTI_Line2);
	 Pin::extiCallback(2);
 }

void  EXTI3_IRQHandler(void){
	EXTI_ClearFlag(EXTI_Line3);
	 Pin::extiCallback(3);
 }

void  EXTI4_IRQHandler(void){
	EXTI_ClearFlag(EXTI_Line4);
	 Pin::extiCallback(4);
 }



void  EXTI15_10_IRQHandler(){
	 if (EXTI_GetFlagStatus(EXTI_Line15) == SET)
	        {
	          EXTI_ClearFlag(EXTI_Line15);
	          Pin::extiCallback(15);
	        }
	        else if (EXTI_GetFlagStatus(EXTI_Line14) == SET)
	        {
	          EXTI_ClearFlag(EXTI_Line14);
	          Pin::extiCallback(14);
	        }
	        else if (EXTI_GetFlagStatus(EXTI_Line13) == SET)
	        {
	          EXTI_ClearFlag(EXTI_Line13);
	          Pin::extiCallback(13);
	        }
	        else if (EXTI_GetFlagStatus(EXTI_Line12) == SET)
	        {
	          EXTI_ClearFlag(EXTI_Line12);
	          Pin::extiCallback(12);
	        }
	        else if (EXTI_GetFlagStatus(EXTI_Line11) == SET)
	        {
	          EXTI_ClearFlag(EXTI_Line11);
	          Pin::extiCallback(11);
	        }
	        else if (EXTI_GetFlagStatus(EXTI_Line10) == SET)
	        {
	          EXTI_ClearFlag(EXTI_Line10);
	          Pin::extiCallback(10);
	        }


 }


void EXTI9_5_IRQHandler()//ïîìåíÿëñÿ ñòàòóñ áëþòóñ ñîåäèíåíèÿ
	{

        if (EXTI_GetFlagStatus(EXTI_Line5) == SET)
        {
          EXTI_ClearFlag(EXTI_Line5);
          Pin::extiCallback(5);
        }
        else if (EXTI_GetFlagStatus(EXTI_Line6) == SET)
        {
          EXTI_ClearFlag(EXTI_Line6);
          Pin::extiCallback(6);
        }
        else if (EXTI_GetFlagStatus(EXTI_Line7) == SET)
        {
          EXTI_ClearFlag(EXTI_Line7);
          Pin::extiCallback(7);
        }
        else if (EXTI_GetFlagStatus(EXTI_Line8) == SET)
        {
          EXTI_ClearFlag(EXTI_Line8);
          Pin::extiCallback(8);
        }
        else if (EXTI_GetFlagStatus(EXTI_Line9) == SET)
        {
          EXTI_ClearFlag(EXTI_Line9);
          Pin::extiCallback(9);
        }



	//static portBASE_TYPE xHigherPriorityTaskWoken;
	  //xHigherPriorityTaskWoken = pdFALSE;
	//static /*volatile*/ tsystem_event_type sys_event_tmp;
	//EXTI_ClearFlag(EXTI_Line7);
	//sys_event_tmp.event_source = BLUETOOTH;
	//xQueueSendToBackFromISR(xEventQueue, &sys_event_tmp, &xHigherPriorityTaskWoken);

	}




 #ifdef __cplusplus
   }
 #endif
