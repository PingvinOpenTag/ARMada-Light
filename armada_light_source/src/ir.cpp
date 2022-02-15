#include <stdint.h>
#include <stdlib.h>
#include "stm32f10x.h"
#include "ir.h"
#include "miles_protocol.h"
#include "stm32f10x_tim.h"
#include "system.h"



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

// extern const uint8_t damage_value[];
 extern System sys;



 uint8_t IrTransceiver::tx_buffer[TX_BUFFER_SIZE]; 	//������ ��-�����������
 tir_tx_buffer_cursor IrTransceiver::ir_tx_buffer_cursor; //������ ����������� �� ������
 TIM_OCInitTypeDef IrTransceiver::timer8_PWM;
 TRecStatus IrTransceiver::tim2_rec_state;
 uint16_t IrTransceiver::tim2_ir_rx_count;
 trx_event IrTransceiver::Zone2RxEvent;//������� 2-�� ����
 uint8_t IrTransceiver::zone2_rx_buffer[ZONE_RX_BUFFER_SIZE]; 	//������ ��-���������
 xSemaphoreHandle IrTransceiver::xZone2Semaphore;
 Pin IrTransceiver::flash_led_pin(PORTC,PIN8);
 Pin IrTransceiver::vibro_pin(PORTC,PIN1);
 IrTransceiver::IrTransceiver(): ir_power_switch_pin(PORTC,PIN6){
	 vSemaphoreCreateBinary(xZone2Semaphore);//������� 2-�� ���� ���������

		xTaskCreate( Zone2task,  "Zone2task", configMINIMAL_STACK_SIZE/2, NULL, 0,
					                        ( xTaskHandle * ) NULL);


	 init_f0_pwm();
	 init_sender_pwm();
	 init_receiver();
#ifndef BLACK_PCB
	 ir_power_switch_pin.switchToOutputOD();
#else
	 ir_power_switch_pin.switchToOutput();
#endif
	 flash_led_pin.switchToOutput();
	 vibro_pin.switchToOutput();

 }



 void  IrTransceiver::low_power(void){
#ifndef BLACK_PCB
	 ir_power_switch_pin.set();
#else
	 ir_power_switch_pin.reset();
#endif
 }

 void  IrTransceiver::hight_power(void){
#ifndef BLACK_PCB
	 ir_power_switch_pin.reset();
#else
	 ir_power_switch_pin.set();
#endif
 }

 void IrTransceiver::Zone2task(void *pvParameters) {
 	 for (;;) {
 		  xSemaphoreTake(xZone2Semaphore, portMAX_DELAY );//���� ������� �� ���������

 	       switch(Zone2RxEvent)
 	       {
 	        case RX_COMPLETE:
 	       	   {
 	       		  // hit_processing(zone_2);
 	       		   hitProcessing();
 	       		   Zone2RxEvent = NOT_EVENT;//���������� �������
 	               tim2_rec_state = REC_Idle;//��������� ����� �� 2-�� ����

 	           }
 	        break;
 	        case RX_ERROR:
 	           {
 	        	   Zone2RxEvent = NOT_EVENT;
 	       //      		 tim5_rec_state = REC_Idle;
 	           }
 	        break;
 	        case RX_MESSAGE_COMPLETE://������� ���������
 	        {
          	//	message_processing(zone_2);
 	        	Zone2RxEvent = NOT_EVENT;
 	            tim2_rec_state = REC_Idle;//��������� ����� �� 2-�� ����
 	        }
 	        break;
 	        default: break;
 	       }
 	 }
 }

 void IrTransceiver::init_receiver(void){
	 GPIO_InitTypeDef port;
	 volatile TIM_TimeBaseInitTypeDef timer2;
	 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);
	 	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	 	GPIO_StructInit(&port);
	 	port.GPIO_Mode = GPIO_Mode_IN_FLOATING;

	 	port.GPIO_Pin = GPIO_Pin_15;
	 //	port.GPIO_Speed = GPIO_Speed_2MHz;
	 	GPIO_Init(GPIOA, &port);

	 	/*Configure peripheral I/O remapping */
	 	GPIO_PinRemapConfig(GPIO_PartialRemap1_TIM2, ENABLE);

	 	TIM_TimeBaseStructInit((TIM_TimeBaseInitTypeDef*)&timer2);
	 	timer2.TIM_Prescaler = /*72*/(SystemCoreClock/1000000)-1;//������� ������������
	 ////	    timer3.TIM_Period = IR_START_BIT_DURATION + IR_SPACE_DURATION;
	 	    TIM_TimeBaseInit(TIM2, (TIM_TimeBaseInitTypeDef*)&timer2);

	 	    TIM_ICInitTypeDef TIM_ICStructure;


	 	    TIM_ICStructure.TIM_Channel = TIM_Channel_1;//������ �����
	 	    TIM_ICStructure.TIM_ICPolarity = TIM_ICPolarity_Falling; // �� ������� ������
	 	    TIM_ICStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; // ����� � �����
	 	    TIM_ICStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1; // ��� ��������
	 	    TIM_ICStructure.TIM_ICFilter = 0; // ��� �������

	 	    // ��� ������� � �������� ����� PWM input - ������������� ����������� ��������������� �����
	 	              // ������ � ����������� ���������� �������� �� 1 � 2 ������, �� 3 � 4 - �� �����
	         TIM_PWMIConfig(TIM2, &TIM_ICStructure);

	         /* �������� �������� ��� ��������: ���� 1 */
	                  TIM_SelectInputTrigger(TIM2, TIM_TS_TI1FP1);
	                  /* �� ������� �� �������� ������� ����� ������������. */
	                  TIM_SelectSlaveMode(TIM2, TIM_SlaveMode_Reset);
	                  /* �������� ������� �� �������� */
	                  TIM_SelectMasterSlaveMode(TIM2, TIM_MasterSlaveMode_Enable);


	                  // ��� ������ �����, ��� ��������. ������� 15��, ��������� ������������ ��� (�����) 13.5��
	                  TIM_OCInitTypeDef TIM_OCStructure;
	                  TIM_OCStructure.TIM_OCMode = TIM_OCMode_Timing;
	                  TIM_OCStructure.TIM_OutputState = TIM_OutputState_Disable;
	                  TIM_OCStructure.TIM_OutputNState = TIM_OutputNState_Disable;
	                  TIM_OCStructure.TIM_Pulse = 15000;
	                  TIM_OCStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	                  TIM_OC3Init(TIM2, &TIM_OCStructure);


	                  /* ��������� ������� ������������ ���������� �� ������� */
	                  TIM_ITConfig(TIM2, TIM_IT_CC1, ENABLE);
	                  // � �� �������� �������� ������
	                  TIM_ITConfig(TIM2, TIM_IT_CC3, DISABLE/*ENABLE*/);

	                  TIM_ClearFlag(TIM2, TIM_FLAG_CC1);
	                  TIM_ClearFlag(TIM2, TIM_FLAG_CC3);

	                  /* �������� ������ */
	                  TIM_Cmd(TIM2, ENABLE);
	                  // ��������� ����������
	                  //������������� ���������, ����� ���� ������ 11 ��� ���������� ������ FreeRTOS
	                  NVIC_SetPriority(TIM2_IRQn, 12);
	                  NVIC_EnableIRQ(TIM2_IRQn);


 }


 /**************************************************************************************
 * ��������� �������������� ������
 * � �������� ��������� ������� ����������� ����������������� ����� ������ (�� 1 �� 127)
 * � ���������� ���������� ������� � ���������� ���������� data_packet.player_id
 * ����� ��������������� ������� ������������  data_packet.player_id.(bit_0 ... bit_7)
 ***************************************************************************************/
 void IrTransceiver::set_player_id(uint8_t ID){
 tx_buffer[0]= ID;
 tx_buffer[0] &=~(1<<7);//������� ��� � �������� ������ ����� "0"

 }


 /**************************************************************************************
 * ��������� �������������� (�����) �������
 * � �������� ��������� ������� ����������� ����������������� ����� (����) ������� (�� 0 �� 3)
 * � ���������� ���������� ������� � ���������� ���������� data_packet.team_id
 * ����� ��������������� ������� ������������  data_packet.team_id.(bit_0 � bit_1)
 ***************************************************************************************/
 void  IrTransceiver::set_team_color(tteam_color  color){
 tx_buffer[1] &=~((1<<7)|(1<<6));//�������� ��� ������� ����
 tx_buffer[1] |=(color <<6);//������������� 6 � 7 ���� � ������������ � ������ �������
 }


 /**************************************************************************************
 * ��������� ��������� ��������� ������ ������ (��������� ����)
 * � �������� ��������� ������� ����������� ��������� ����
 * � ���������� ���������� ������� � ���������� ���������� data_packet.damage
 * ����� ��������������� ������� ������������  data_packet.damage.(bit_0 � bit_3)
 ***************************************************************************************/


 void IrTransceiver::set_gun_damage(tgun_damage damage){
 tx_buffer[1] &=~((1<<5)|(1<<4)|(1<<3)|(1<<2));//�������� ���� �����
 tx_buffer[1] |=(damage << 2);
 }




void IrTransceiver::send_shot_package(void){
	flash_led_pin.set();
	vibro_pin.set();
	ir_tx_buffer_cursor.byte_pos = 0;
		ir_tx_buffer_cursor.bit_mask = (1<<7);
		ir_tx_buffer_cursor.bits_for_tx=14;//"�������" ������� �� 14 ���
		ir_tx_buffer_cursor.heder_been_sent = false;
		TIM_DeInit(TIM8);//���������� ��������� ������� �� ����������
		init_sender_pwm();
}


void  IrTransceiver::bit_send_done_callback(){



		//f_status = TIM_GetFlagStatus(TIM8, TIM_SR_UIF /*TIM_DIER_UIE*//*TIM_FLAG_CC1OF*/);




		if (ir_tx_buffer_cursor.bits_for_tx>0) //���� ��������� ��������� �� �� ������ ������
 		{
 			if(sys.settings.gun.ir_duty_cycle > 50)TIM1->CCR1 = 50; // �������� ������� ��
 			else TIM1->CCR1 = sys.settings.gun.ir_duty_cycle;// 50; // �������� ������� ��
 			//TIM1->CCR1 = (uint16_t)armadaSystem.gun.ir_duty_ctcle;
 			if(!ir_tx_buffer_cursor.heder_been_sent)
 			{
 				ir_tx_buffer_cursor.heder_been_sent = true;
 				return;
 			}
 			if(ir_tx_buffer_cursor.bit_mask == 0)//��� ���� �������� ����� ��� ��������
 			{
 				ir_tx_buffer_cursor.byte_pos++;//��������� � ���������� �����
 				ir_tx_buffer_cursor.bit_mask = (1<<7); //������� ��� ������ ������

 			}
 			if (tx_buffer[ir_tx_buffer_cursor.byte_pos]&ir_tx_buffer_cursor.bit_mask)//���� ������� ��� ����� "1"
 			{
 				//timer3.TIM_Period = IR_ONE_BIT_DURATION + IR_SPACE_DURATION;
 				TIM8->ARR = IR_ONE_BIT_DURATION + IR_SPACE_DURATION;
 				timer8_PWM.TIM_Pulse = IR_ONE_BIT_DURATION;
 				//ir_pulse_counter = IR_ONE;//�������� "1" (�������� 1200 �����������)
 			}
 			else //������� ��� ����� "0"
 			{
 				//timer3.TIM_Period = IR_ZERO_BIT_DURATION + IR_SPACE_DURATION;
 				TIM8->ARR = IR_ZERO_BIT_DURATION + IR_SPACE_DURATION;
 				timer8_PWM.TIM_Pulse = IR_ZERO_BIT_DURATION;
 				//				ir_pulse_counter = IR_ZERO;//�������� "0" (�������� 600 �����������)
 			}
 //			TIM_TimeBaseInit(TIM3, &timer3);
 			TIM_OC1Init(TIM8, (TIM_OCInitTypeDef*)&timer8_PWM);//��������� ����� ��������� ������� � ������
 //			ir_space_counter = IR_SPACE;      //� ��� ����� �� �������
 			ir_tx_buffer_cursor.bit_mask = (ir_tx_buffer_cursor.bit_mask >> 1); //��������� ���
 			ir_tx_buffer_cursor.bits_for_tx--; //��������� ���������� ���������� ���




 			//					ir_pulse_counter =data_packet.data[cursor_position++] ; //��������� ������� ��������� �������������

 		}
 		else //��� ���� ��������
 		{
 			TIM1->CCR1 = 0; // �������� ������� ��
 			TIM_ClearFlag(TIM8, TIM_SR_UIF/*TIM_DIER_UIE*//*TIM_FLAG_CC1OF*/);
 			TIM_Cmd(TIM8, DISABLE);//��������� ���������� (��������)
 			flash_led_pin.reset();
 			vibro_pin.reset();
 		////	FLASH_LED_OFF;

 		}




}
void  IrTransceiver::init_f0_pwm(){
	//TIM1 setup
	GPIO_InitTypeDef port;
		TIM_TimeBaseInitTypeDef timer;
		TIM_OCInitTypeDef timerPWM;
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);


		GPIO_StructInit(&port);
		port.GPIO_Mode = GPIO_Mode_AF_PP;
		port.GPIO_Pin = GPIO_Pin_8;
		port.GPIO_Speed = GPIO_Speed_2MHz;
		GPIO_Init(GPIOA, &port);

		//GPIO_ResetBits(GPIOA, GPIO_Pin_8);
	//	GPIO_SetBits(GPIOA, GPIO_Pin_8);

		TIM_TimeBaseStructInit(&timer);
		timer.TIM_Prescaler = /*7200-1;*/(SystemCoreClock/(/*36000*/56000*100))-1;
		    timer.TIM_Period = 100;
		    TIM_TimeBaseInit(TIM1, &timer);

		    TIM_OCStructInit(&timerPWM);
		    timerPWM.TIM_Pulse = 0;//50;
		    timerPWM.TIM_OCMode = TIM_OCMode_PWM1;
		    timerPWM.TIM_OutputState = TIM_OutputState_Enable;
		    TIM_OC1Init(TIM1, &timerPWM);

		    TIM_BDTRInitTypeDef bdtr;
		      TIM_BDTRStructInit(&bdtr);
		      bdtr.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;
		      TIM_BDTRConfig(TIM1, &bdtr);


		    TIM_Cmd(TIM1, ENABLE);







}

void IrTransceiver::init_sender_pwm(void){

	  //	TIM8 setup
			 volatile TIM_TimeBaseInitTypeDef timer8;

			    	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);



			    	TIM_TimeBaseStructInit((TIM_TimeBaseInitTypeDef*)&timer8);
			    	timer8.TIM_Prescaler = /*72*/(SystemCoreClock/1000000)-1;//������� ������������
			    	    timer8.TIM_Period = IR_START_BIT_DURATION + IR_SPACE_DURATION;
			    	    TIM_TimeBaseInit(TIM8, (TIM_TimeBaseInitTypeDef*)&timer8);

			    	    TIM_OCStructInit((TIM_OCInitTypeDef *)&timer8_PWM);
			    	    timer8_PWM.TIM_Pulse = IR_START_BIT_DURATION;
			    	    timer8_PWM.TIM_OCMode = TIM_OCMode_PWM1;
			    	    timer8_PWM.TIM_OutputState = TIM_OutputState_Disable;
			    	    TIM_OC1Init(TIM8, (TIM_OCInitTypeDef *)&timer8_PWM);
			    	    TIM_ITConfig(TIM8, TIM_IT_CC1, ENABLE);//��������� ���������� �� �������/���������
			    //	    TIM_ITConfig(TIM3, TIM_FLAG_CC1OF, ENABLE);//��������� ���������� �� �������/���������
			    	    TIM_ITConfig(TIM8, TIM_DIER_UIE, ENABLE);//��������� ���������� �� �������/���������

			    	    TIM_Cmd(TIM8, ENABLE);
			    	    NVIC_EnableIRQ( TIM8_UP_IRQn);
			    	    NVIC_EnableIRQ( TIM8_CC_IRQn);




}


void IrTransceiver::set_zone_buffer_bit (TDamageZone zone, uint8_t index, bool value){
	uint8_t byte_index;
		uint8_t bit_index;
		byte_index = index/8; //����������, � ����� ����� ��������� ������ ���
		bit_index = index - (byte_index*8);//���������� ����� ���� � �����


					switch(zone)
					{

					case zone_1:
						{
							/*
							if(value) zone1_rx_buffer[byte_index]|= (1<<(7-bit_index));

							else zone1_rx_buffer[byte_index] &= ~(1<<(7-bit_index));
							 */
						}
						break;


					case zone_2:
						{

							if(value) zone2_rx_buffer[byte_index]|= (1<<(7-bit_index));

							else zone2_rx_buffer[byte_index] &= ~(1<<(7-bit_index));
						}
						break;
						case zone_3:
						{
							/*
							if(value) zone3_rx_buffer[byte_index]|= (1<<(7-bit_index));

							else zone3_rx_buffer[byte_index] &= ~(1<<(7-bit_index));
							*/
						}
						break;
						case zone_4:
						{
							/*
							if(value) zone4_rx_buffer[byte_index]|= (1<<(7-bit_index));

							else zone4_rx_buffer[byte_index] &= ~(1<<(7-bit_index));
							*/
						}
						break;
					}


}


void IrTransceiver::receiver_IQR_callback(void){

	 static portBASE_TYPE xHigherPriorityTaskWoken;
	  xHigherPriorityTaskWoken = pdFALSE;
	uint16_t cnt1, cnt2;
	        if (TIM_GetITStatus(TIM2, TIM_IT_CC1) != RESET) {
	                TIM_ClearITPendingBit(TIM2, TIM_IT_CC1);
	                TIM_ClearITPendingBit(TIM2, TIM_IT_Update);

	                cnt1 = TIM_GetCapture1(TIM2);
	                cnt2 = TIM_GetCapture2(TIM2);

	                if (tim2_rec_state == REC_Recording) {

	                	if (tim2_ir_rx_count  < IR_RAW_BUFFER_SIZE) {
	                		if(tim2_ir_rx_count==0)//������ ������ ���� ���������
	                		{
	                			if(checkVal(cnt2,IR_START_BIT_DURATION,IR_TOL))//��������� ��������� �� ������
	                			{
	                				tim2_ir_rx_count++;
	                			}
	                			else //��������� "�����"
	                			{
	                				// ��������� ���������� �� ������������ �������� ������
	                				TIM_ITConfig(TIM2, TIM_IT_CC3, /*ENABLE*/DISABLE);
	                				tim2_rec_state = REC_Idle;
	                				tim2_ir_rx_count=0;
								}
	                		}
	                		else //��������� ��� �������, ���� ����� ������
	                		{
//	                			TIM2_IR_RX_RAW_BUFFER[tim2_ir_rx_count-1].Period=cnt1;
//	                	        TIM2_IR_RX_RAW_BUFFER[tim2_ir_rx_count-1].Pulse=cnt2;

	                			if(checkVal(cnt2,IR_ONE_BIT_DURATION,IR_TOL)&&checkVal(cnt1,IR_ONE_BIT_DURATION+IR_SPACE_DURATION, IR_TOL))
	                			{
	                				set_zone_buffer_bit(zone_2,tim2_ir_rx_count-1,true);//��� �������
	                				tim2_ir_rx_count++;
	                			}
	                			else if (checkVal(cnt2, IR_ZERO_BIT_DURATION,IR_TOL)&&checkVal(cnt1,IR_ZERO_BIT_DURATION+IR_ZERO_BIT_DURATION,IR_TOL))
	                			{
	                				set_zone_buffer_bit(zone_2,tim2_ir_rx_count-1,false);//��� ����
	                				tim2_ir_rx_count++;
	                			} else {//������
	                				TIM_ITConfig(TIM2, TIM_IT_CC3, /*ENABLE*/DISABLE);
	                				tim2_rec_state = REC_Idle;
	                				tim2_ir_rx_count=0;
								}



	                		}
	                        }
	                	else {//������� ������� ����� - ������
	                		 // ��������� ���������� �� ������������ �������� ������
	                		  	TIM_ITConfig(TIM2, TIM_IT_CC3, /*ENABLE*/DISABLE);
	                		   	tim2_rec_state = REC_Idle;
	                		   	tim2_ir_rx_count=0;
						}

	                }


	                if (tim2_rec_state == REC_Idle) {
	                        //������ ������ �����, �������� ������
	                	//��������� ���������� �� 3 ������, �������������� ������� ����
	                	TIM_ClearITPendingBit(TIM2, TIM_IT_CC3);
	                	TIM_ITConfig(TIM2, TIM_IT_CC3, ENABLE);
	                	tim2_rec_state = REC_Recording;
	//                        captCount = 0;

	                }

	        }

	        if (TIM_GetITStatus(TIM2, TIM_IT_CC3) != RESET) {//���� ������� �� �����
	                TIM_ClearITPendingBit(TIM2, TIM_IT_CC3);
	                if (tim2_rec_state == REC_Recording) {

	                	if((tim2_ir_rx_count==14)||(tim2_ir_rx_count==24))
	                	{
	                		cnt2 = TIM_GetCapture2(TIM2);
	                		if(checkVal(cnt2,IR_ONE_BIT_DURATION,IR_TOL))
	                		{
	                			set_zone_buffer_bit(zone_2,tim2_ir_rx_count-1,true);//��� �������
//	                			tim2_ir_rx_count++;
	                		}
	                		else if (checkVal(cnt2, IR_ZERO_BIT_DURATION,IR_TOL))
	                		{
	                		set_zone_buffer_bit(zone_2,tim2_ir_rx_count-1,false);//��� ����
	            //    		tim2_ir_rx_count++;
	                		} else {//������
	                	   				TIM_ITConfig(TIM2, TIM_IT_CC3, /*ENABLE*/DISABLE);
	                	   				tim2_rec_state = REC_Idle;
	                            		tim2_ir_rx_count=0;
	        	                		Zone2RxEvent=RX_ERROR;
	        	                		xSemaphoreGiveFromISR( xZone2Semaphore, &xHigherPriorityTaskWoken);
	        	                		return;
	                				}

	                		tim2_rec_state = REC_Captured;
	                		if(tim2_ir_rx_count==14)Zone2RxEvent=RX_COMPLETE;
	                		else Zone2RxEvent=RX_MESSAGE_COMPLETE;
//	                		xTaskGetTickCount();
		                	xSemaphoreGiveFromISR( xZone2Semaphore, &xHigherPriorityTaskWoken );

	                	}
	                	else {//������
	                		tim2_rec_state = REC_Idle;
	                		Zone2RxEvent=RX_ERROR;
	                		xSemaphoreGiveFromISR( xZone2Semaphore, &xHigherPriorityTaskWoken );
						}

	                	// ��������� ���������� �� ������������ �������� ������
	                	TIM_ITConfig(TIM2, TIM_IT_CC3, /*ENABLE*/DISABLE);
	   //             	tim5_rec_state = REC_Idle;

	                	tim2_ir_rx_count=0;
	                	//xSemaphoreGiveFromISR( xZone4Semaphore, &xHigherPriorityTaskWoken );
	                }

	        }


}



//������ ��� ��������� ��������� �������� �����. �������� ���� = damage_value[Damage_xx]
const uint8_t damage_value [] =
{
	1,   // 0000 = 1   (Damage_1)
	2,   // 0001 = 2   (Damage_2)
	4,   // 0010 = 4   (Damage_4)
	5,   // 0011 = 5   (Damage_5)
	7,   // 0100 = 7   (Damage_7)
	10,  // 0101 = 10  (Damage_10)
	15,  // 0110 = 15  (Damage_15)
	17,  // 0111 = 17  (Damage_17)
	20,  // 1000 = 20  (Damage_20)
	25,  // 1001 = 25  (Damage_25)
	30,  // 1010 = 30  (Damage_30)
	35,  // 1011 = 35  (Damage_35)
	40,  // 1100 = 40  (Damage_40)
	50,  // 1101 = 50  (Damage_50)
	75,  // 1110 = 75  (Damage_75)
	100, // 1111 = 100 (Damage_100)
};



void IrTransceiver::getPackageValue(trx_package &package){
	uint8_t byte_tmp;
	package.player_id = zone2_rx_buffer[1];
	byte_tmp = zone2_rx_buffer[1];
	byte_tmp = byte_tmp << 2; //����������� �� ��� ����� �������
	byte_tmp = byte_tmp >> 4;
	package.damage = damage_value[byte_tmp];
	package.team_id = zone2_rx_buffer[1]>>6;

}

void IrTransceiver::hitProcessing(void){
	//static volatile int i;
	trx_package ir_package;
	 getPackageValue(ir_package);
	 if (ir_package.team_id==sys.settings.player.team_color) return;
	 sys.wavPlayer->play_sound_now(8);
}


#ifdef __cplusplus
extern "C" {
#endif

 void TIM8_CC_IRQHandler(void){
//	ITStatus stat_tmp;
//		stat_tmp = TIM_GetITStatus(TIM8, TIM_IT_CC1);
		if (TIM_GetITStatus(TIM8, TIM_IT_CC1) != RESET)
	  {
			/* ��� �����, ��� ���������� ���������� */
	    TIM_ClearITPendingBit(TIM8, TIM_IT_CC1);
	   	TIM1->CCR1 = 0;
	  }
}




 void TIM8_UP_IRQHandler(void)
 {

     /* ��� ���-������ ������������ ������� over-capture, ���� ���������� */

 	if (TIM_GetFlagStatus(TIM8, TIM_SR_UIF /*TIM_DIER_UIE*//*TIM_FLAG_CC1OF*/) != RESET)
     {
 		TIM_ClearFlag(TIM8, TIM_SR_UIF/*TIM_DIER_UIE*//*TIM_FLAG_CC1OF*/);
 		IrTransceiver::bit_send_done_callback();



     }
 }


 void TIM2_IRQHandler(void)//2-� ����
 {

	 IrTransceiver::receiver_IQR_callback();

 }



#ifdef __cplusplus
  }
#endif
