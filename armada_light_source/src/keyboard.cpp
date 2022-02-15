#include <pin.h>
#include <stdint.h>
#include <stdlib.h>
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_tim.h"
#include "keyboard.h"
#include "system.h"
#include "bluetooth.h"

#ifdef __cplusplus
 extern "C" {
  #endif
#include "../FreeRTOS/include/FreeRTOS.h"
#include "../FreeRTOS/include/task.h"
#include "../FreeRTOS/include/queue.h"

#ifdef __cplusplus
  }
#endif




#ifdef __cplusplus
 extern "C" {
  #endif

 void TIM7_IRQHandler(void)
 {

	if (System::bandana_led_on_timer >0){
//		 static portBASE_TYPE xHigherPriorityTaskWoken;
//		 static tevents_source_type event_sourse;
//		 xHigherPriorityTaskWoken = pdFALSE;
		System::bandana_led_on_timer--;
		if(System::bandana_led_on_timer==0) {
		/*
			event_sourse = LED_OFF;
			xQueueSendToBackFromISR(System::xEventQueue, &event_sourse, &xHigherPriorityTaskWoken);

				 				if( xHigherPriorityTaskWoken == pdTRUE )
				 							                          {
				 							                                portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
				 							                          }
*/
			Bluetooth::PutChar('f');

		}
	}
	if(System::safe_counter>0)
	{
		System::safe_counter--;
	}
	if(System::shock_counter>0)
	{
		System::shock_counter--;
	}
	Keyboard::scanKeys();
	TIM_ClearITPendingBit(TIM7, TIM_IT_Update);

 }

#ifdef __cplusplus
  }
#endif


 TPRESSING_DURATION Keyboard::buttons_pressing_duratins[4] ={{0,1},{0,1},{0,1},{0,1}};
 TBUTTON_EVENT Keyboard::buttons_events[4]={depressed,depressed, depressed, depressed};



 bool Keyboard::triggerIsPressed(void){
 	return (!trigger.isSet());
 }


 bool Keyboard::reloadIsPressed(void){
 	return (!reload.isSet());
 }


 bool Keyboard::fire_modeIsPressed(void){
 	return (!fire_mode.isSet());
 }



 void Keyboard::scanKeys(void){
	 portBASE_TYPE xHigherPriorityTaskWoken;
	 tevents_source_type event_sourse;
	 xHigherPriorityTaskWoken = pdFALSE;


	 switch (buttons_events[button1])
	 	{
	 		case depressed:
	 		{
	 			buttons_events[button1] = test_button(button1);
	 			if(buttons_events[button1]==pressed){
	 //				TIM_Cmd(TIM15, DISABLE);
	 				event_sourse = BUTTON1;

	 				xQueueSendToBackFromISR(System::xEventQueue, &event_sourse, &xHigherPriorityTaskWoken);

	 				if( xHigherPriorityTaskWoken == pdTRUE )
	 							                          {
	 							                                portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
	 							                          }

	 			//	button1_event=depressed;
	 			}

	 		}
	 		break;
	 		default:;
	 	}

	 switch (buttons_events[button2])
	 	{
	 		case depressed:
	 		{
	 			buttons_events[button2] = test_button(button2);
	 			if(buttons_events[button2]==pressed){
	 				event_sourse = BUTTON2;

	 				xQueueSendToBackFromISR(System::xEventQueue, &event_sourse, &xHigherPriorityTaskWoken);
	 						if( xHigherPriorityTaskWoken == pdTRUE )
	 							                          {
	 							                                portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
	 							                          }



	 //				button2_event=depressed;
	 			}
	 		}
	 		break;
	 		default:;
	 	}


	 switch (buttons_events[button3])
	 	{
	 		case depressed:
	 		{
	 			buttons_events[button3] = test_button(button3);
	 			if(buttons_events[button3]==pressed){
	 				event_sourse = BUTTON3;

	 				xQueueSendToBackFromISR(System::xEventQueue, &event_sourse, &xHigherPriorityTaskWoken);
	 							if( xHigherPriorityTaskWoken == pdTRUE )
	 							                          {
	 							                                portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
	 							                          }






	 //				button3_event=depressed;
	 			}
	 		}
	 		break;
	 		default:;
	 	}

	 switch (buttons_events[button4])
	 	{
	 		case depressed:
	 		{
	 			buttons_events[button4] = test_button(button4);
	 			if(buttons_events[button4]==pressed){
	 				event_sourse = BUTTON4;

	 				xQueueSendToBackFromISR(System::xEventQueue, &event_sourse, &xHigherPriorityTaskWoken);
	 							if( xHigherPriorityTaskWoken == pdTRUE )
	 							                          {
	 							                                portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
	 							                          }




	 //				button4_event=depressed;
	 			}
	 		}
	 		break;
	 		default:;
	 	}








	 static int i;
	 i++;

 }

  	Pin Keyboard::trigger(PORTA,PIN0);
  	Pin Keyboard::reload(PORTA,PIN1);
	Pin Keyboard::fire_mode(PORTA,PIN14);

 TBUTTON_STATUS Keyboard::get_button_status(TBUTTON button){
 	switch(button)
 	{
 	case button1:
 		{
 			if (Keyboard::triggerIsPressed())return button_on;
 			return button_off;
 		}
 	break;

 	case button2:
 		{
 			if (Keyboard::reloadIsPressed()) return button_on;
 			return button_off;
 		}
 	break;

 	case button3:
 		{
 			if (Keyboard::fire_modeIsPressed()) return button_on;
 			return button_off;//(TBUTTON_STATUS)GPIO_ReadInputDataBit(Button3_GPIO_Port, Button3_Pin);
 		}
 	break;

 	case button4:
 		{
 			return button_off;//(TBUTTON_STATUS)GPIO_ReadInputDataBit(Button4_GPIO_Port, Button4_Pin);
 		}
 	break;
 	default: return  button_off;
 	break;
 	}
 	return  button_off;

 }


 void Keyboard::init(void){
			trigger.switchToInputPU();
			reload.switchToInputPU();
			fire_mode.switchToInputPU();
			Keyboard::init_timer();
 }

 TBUTTON_EVENT Keyboard::test_button(TBUTTON button){
 	TBUTTON_EVENT ret_ev;
 //	TBUTTON_STATUS btn_status;
 //	btn_status = (TBUTTON_STATUS) get_button1_status();
 	switch(get_button_status(button))
 	{
 		case button_off://������ �� ������
 		{
 			if (buttons_pressing_duratins[button].duration >= SHORT_DURATION){
 				ret_ev = pressed;
 				buttons_pressing_duratins[button].duration=0;
 				buttons_pressing_duratins[button].inc=0;
 				return ret_ev;
 			}
 			else{
 				ret_ev=depressed;
 				buttons_pressing_duratins[button].duration=0;
 				buttons_pressing_duratins[button].inc=1;
 			}

 		}
 		break;

 		case button_on://������ ������
 		{
 			if (buttons_pressing_duratins[button].duration >= SHORT_DURATION){
 				ret_ev = pressed;
 				buttons_pressing_duratins[button].duration=0;
 				buttons_pressing_duratins[button].inc=0;
 			}
 			else{
 				ret_ev=buttons_events[button];
 				buttons_pressing_duratins[button].duration+=buttons_pressing_duratins[button].inc;

 			}
 		}
 		break;

 	}

 return   ret_ev;
 }





 void Keyboard::init_timer(){
	 volatile TIM_TimeBaseInitTypeDef timer7;
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);
		//��������� ���� ��������� ���������� ����������
		TIM_TimeBaseStructInit((TIM_TimeBaseInitTypeDef*)&timer7);
		 //���������� ������������
		timer7.TIM_Prescaler = 2*/*7200*/(SystemCoreClock/10000)-1;
		//��� ��������, �������� �� �������� ������ ����������� ����������
			timer7.TIM_Period = 100;
			//�������������� TIM4 ������ ����������
			TIM_TimeBaseInit(TIM7, (TIM_TimeBaseInitTypeDef*)&timer7);
			//����������� ������ ��� ��������� ���������� �� ���������� (������������)
			TIM_ITConfig(TIM7, TIM_IT_Update, ENABLE);
		    //������������� ���������, ����� ���� ������ 11 ��� ���������� ������ FreeRTOS
		    NVIC_SetPriority(TIM7_IRQn, 14);
			//��������� ������
			TIM_Cmd(TIM7, ENABLE);
			//��������� ��������������� ����������
			NVIC_EnableIRQ(TIM7_IRQn);
 }
