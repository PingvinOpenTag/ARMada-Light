
#include <pin.h>
#include "bluetooth.h"

#include <stdint.h>
#include <stdlib.h>
#include "stm32f10x.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_gpio.h"
#include "usart.h"
#include "system.h"
#include "keyboard.h"



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

/*
Bluetooth::Bluetooth(){

//at_mode_pin.setPort(PORTB,PIN10);
at_mode_pin.switchToOutput();
at_mode_pin.reset();

//state_pin.setPort(PORTC,PIN7);
state_pin.switchToInput();




}

*/

 extern System sys;

 const std::string symbols("AaBbCcDdEeFfGgHhIiJjKkLl");

 xSemaphoreHandle Bluetooth::xBluetoothSemaphore;
 xSemaphoreHandle  Bluetooth::xOkReceivedSemaphore;

 unsigned char Bluetooth::rx_buffer[BLUETOOTH_SIZE_BUF];


 Pin Bluetooth::state_pin(PORTC,PIN7);
 Bluetooth* Bluetooth::self_pointer=0;

 void Bluetooth::vListeningTask (void *pvParameters){
	 self_pointer->reset();

//std::string str;
	 for (;;) {

		 testBluetoothData();
	 }
 }

 void  Bluetooth::statePinEnable(void){
	 	//	state_pin.setCallback((TPIN_IRQ_CALLBACK/*ExtiCallbackFuncPointer*/) Bluetooth::status_has_changed);
	 		state_pin.enableExti(true);
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




 void Bluetooth::getPackageValue(trx_package &package){
	 package.damage = damage_value[rx_buffer[3]&0b00001111];
	 package.team_id = (rx_buffer[3]&0b00110000)>>4;
	 package.player_id = ((rx_buffer[3]&0b11000000)>>6)|((rx_buffer[2]&0b00011111)<<2);
 }


void Bluetooth::getMessageValue(tir_message& message){
	message.ID = rx_buffer[1];
	message.param = rx_buffer[2];
	message.control_byte = rx_buffer[3];
}

void Bluetooth::testBluetoothData(void){
unsigned char sym;
static TRecHeaderType recHeader = NO_HEADER;
static uint32_t byte_counter = 0;
uint32_t bytes_in_buff;
if (xSemaphoreTake(self_pointer->usart.xUsartSemaphore, portMAX_DELAY)== pdTRUE)//���� 2 ������� ���������� Si4432
	{

	do {
				 taskENTER_CRITICAL();
				 bytes_in_buff = self_pointer->usart.GetRxCount();
				 if (bytes_in_buff ){
					 sym = self_pointer->usart.GetChar();
					 taskEXIT_CRITICAL();

		if (sym == (unsigned char) 0x0E){
			if (recHeader == NO_HEADER){
				recHeader=SHOT_PACKAGE_HEADER;
				byte_counter = 0;
				rx_buffer[byte_counter++] = sym;
				taskENTER_CRITICAL();
				bytes_in_buff = self_pointer->usart.GetRxCount();
				taskEXIT_CRITICAL();
				continue;
			}
		}//[if (sym = (unsigned char) 0x0E)]
		if (recHeader == SHOT_PACKAGE_HEADER){
			if (byte_counter==1)
			{
				if (sym==0){ //������ ���� ������ ��������� 0
					rx_buffer[byte_counter++] = sym;
					taskENTER_CRITICAL();
					bytes_in_buff = self_pointer->usart.GetRxCount();
					taskEXIT_CRITICAL();
					continue;
				}
				else{
					recHeader = NO_HEADER;
					byte_counter = 0;
					taskENTER_CRITICAL();
					bytes_in_buff = self_pointer->usart.GetRxCount();
					taskEXIT_CRITICAL();
					continue;
				}
			}//[if (byte_counter==1)]
			if (byte_counter==2){
			if (sym<64){ //������ ���� ������ ��������� 0
					rx_buffer[byte_counter++] = sym;
					taskENTER_CRITICAL();
					bytes_in_buff = self_pointer->usart.GetRxCount();
					taskEXIT_CRITICAL();
					continue;
				}
			else{
					recHeader = NO_HEADER;
					byte_counter = 0;
					taskENTER_CRITICAL();
					bytes_in_buff = self_pointer->usart.GetRxCount();
					taskEXIT_CRITICAL();
					continue;
				}
			}//[(byte_counter==2)]
			if (byte_counter==3){
				rx_buffer[byte_counter] = sym;
				System::hitProcessing();
				recHeader = NO_HEADER;
				byte_counter = 0;
				taskENTER_CRITICAL();
				bytes_in_buff = self_pointer->usart.GetRxCount();
				taskEXIT_CRITICAL();
				continue;
			}//[if (byte_counter==3)]

			taskENTER_CRITICAL();
			bytes_in_buff = self_pointer->usart.GetRxCount();
			taskEXIT_CRITICAL();
			continue;
		}//[if (recHeader == SHOT_PACKAGE_HEADER)]




		if (sym == (unsigned char) 0x18){
			if (recHeader == NO_HEADER){
				recHeader = MESSAGE_HEADER;
				byte_counter = 0;
				rx_buffer[byte_counter++] = sym;
				taskENTER_CRITICAL();
				bytes_in_buff = self_pointer->usart.GetRxCount();
				taskEXIT_CRITICAL();
				continue;
			}


		}//[if (sym = (unsigned char) 0x18)]

		if (recHeader == MESSAGE_HEADER){
			if(byte_counter<3){
				rx_buffer[byte_counter++] = sym;
				taskENTER_CRITICAL();
				bytes_in_buff = self_pointer->usart.GetRxCount();
				taskEXIT_CRITICAL();
				continue;
			}
			else if (byte_counter==3){
				rx_buffer[byte_counter] = sym;
				System::messageProcessing();
				recHeader = NO_HEADER;
				byte_counter = 0;
				taskENTER_CRITICAL();
				bytes_in_buff = self_pointer->usart.GetRxCount();
				taskEXIT_CRITICAL();
				continue;
			}
			taskENTER_CRITICAL();
			bytes_in_buff = self_pointer->usart.GetRxCount();
			taskEXIT_CRITICAL();
			continue;
		}//[if (recHeader == MESSAGE_HEADER)]
		if (!(byte_counter<BLUETOOTH_SIZE_BUF-1)){
			recHeader = NO_HEADER;
			byte_counter = 0;
			taskENTER_CRITICAL();
			bytes_in_buff = self_pointer->usart.GetRxCount();
			taskEXIT_CRITICAL();
			continue;
		}

		else {
				rx_buffer[byte_counter++] = sym;
				if (sym =='\r'){
					rx_buffer[byte_counter]=0;
					std::string s;
					//s= (const char *)rx_buffer;
					s = reinterpret_cast <const char*>(rx_buffer);
					Parser::parse(s);

					volatile int i;
					i++;
					//const char *str = s.c_str();
					//char* ch_str;
					//ch_str = const_cast<char*>(s.c_str());
					//Bluetooth::SendStr((char*)rx_buffer);


					recHeader = NO_HEADER;
					byte_counter = 0;
				}
				taskENTER_CRITICAL();
				bytes_in_buff = self_pointer->usart.GetRxCount();
				taskEXIT_CRITICAL();
				continue;
		}

		//		 str.append(reinterpret_cast<const char*>(&sym));
			//	 if (sym=='+'){
				//	 volatile int i = 0;
					// i++;
				 //}

			}//[ if (bytes_in_buff )]
				 else taskEXIT_CRITICAL();
		 }while(bytes_in_buff);//[ while (Usart::GetRxCount())]
	}//[if (xSemaphoreTake(Usart::xUsartSemaphore, configTICK_RATE_HZ*4/*portMAX_DELAY*/ )== pdTRUE)]
 }


/*
 void Bluetooth::status_has_changed(void){
	// if(sys.in_game())
	//		  		  {
						if (xSemaphoreTake(sys.display->xDisplaySemaphore,configTICK_RATE_HZ)== pdTRUE)
						{
							sys.display->update_bluetooth();
							xSemaphoreGive(sys.display->xDisplaySemaphore);
						}
			  		//  }


 }
*/


 void	Bluetooth::pin_irq_callback(void){
	 static portBASE_TYPE xHigherPriorityTaskWoken;
	  xHigherPriorityTaskWoken = pdFALSE;
	 tevents_source_type sys_event_source_tmp;
	 sys_event_source_tmp = BLUETOOTH;
	 xQueueSendToBackFromISR(System::xEventQueue, &sys_event_source_tmp, &xHigherPriorityTaskWoken);

	 /*
	 if(sys.in_game())
			  		  {
						if (xSemaphoreTake(sys.display->xDisplaySemaphore,configTICK_RATE_HZ)== pdTRUE)
						{
							sys.display->update_bluetooth();
							xSemaphoreGive(sys.display->xDisplaySemaphore);
						}
			  		  }
*/

 }

 void Bluetooth::reset(void){
	 reset_pin.switchToOutput();
	 reset_pin.reset();
	 vTaskDelay(200);
	 reset_pin.set();
	 vTaskDelay(200);

	 reset_pin.switchToOutputOD();
	 reset_pin.set();
	 vTaskDelay(200);

 }

 void Bluetooth::set_at_commands_mode(bool mode)//��������� ������ ������ � ����� at-������
 {
 	if (mode)at_mode_pin.set();
 	else at_mode_pin.reset();
 }


 void Bluetooth::switch_to_at_mode(void){

	 usart.setBoudRate(38400);
	 set_at_commands_mode(true);
	 vTaskDelay(100);
	 reset();
	 vTaskDelay(100);
 }


 void Bluetooth::switch_to_data_mode(void){

	 usart.setBoudRate(9600);
	 set_at_commands_mode(false);
	 vTaskDelay(100);
	 reset();
	 vTaskDelay(100);
 }


 bool Bluetooth::send_test_at_command(void){
	 usart.FlushTxBuf();
	 usart.FlushRxBuf();
	 xSemaphoreTake(xOkReceivedSemaphore,configTICK_RATE_HZ/10);
	 usart.SendStr((char*)"AT");
	 usart.SendStr((char*)"\r\n");
	 if (xSemaphoreTake(xOkReceivedSemaphore,configTICK_RATE_HZ)== pdTRUE)
	 	{
	 		return true;
	 	}
	 else return false;


 }



 bool Bluetooth::send_set_at_command(char* cmd, char* param){
	 usart.FlushTxBuf();
	 usart.FlushRxBuf();
	 xSemaphoreTake(xOkReceivedSemaphore,configTICK_RATE_HZ/10);
	 usart.SendStr(cmd);
	 usart.SendStr((char*)"=");
	 usart.SendStr(param);
	 usart.SendStr((char*)"\r\n");
	 if (xSemaphoreTake(xOkReceivedSemaphore,configTICK_RATE_HZ*2)== pdTRUE)
	 	 	{
	 	 		return true;
	 	 	}
	 	 else return false;

 }


 bool Bluetooth::send_at_command(char* cmd){
	 usart.FlushTxBuf();
	 usart.FlushRxBuf();
	 xSemaphoreTake(xOkReceivedSemaphore,configTICK_RATE_HZ/10);
	 usart.SendStr(cmd);
	 usart.SendStr((char*)"\r\n");
	 if (xSemaphoreTake(xOkReceivedSemaphore,configTICK_RATE_HZ)== pdTRUE)
	 	 	{
	 	 		return true;
	 	 	}
	 	 else return false;

 }



bool Bluetooth::configure(void){
	 volatile bool at_res;
	 switch_to_at_mode();
	 at_res = send_test_at_command();
	 if (!at_res) {
		 switch_to_data_mode();
		 return false;
	 }
//	 at_res = send_at_command((char*)"AT+UART?\r\n");


	 at_res = send_set_at_command((char*)"AT+NAME", sys.settings.bluetooth_name);

/*
	 at_res = send_set_at_command((char*)"AT+UART",(char*)"9600,0,0");
	 if (at_res){
		 switch_to_data_mode();
		 usart.setBoudRate(9600);
		 return true;;
	 }
*/

	 switch_to_data_mode();
	 return at_res;
 }

bool Bluetooth::in_at_mode(void){

return at_mode_pin.isSet();

}
