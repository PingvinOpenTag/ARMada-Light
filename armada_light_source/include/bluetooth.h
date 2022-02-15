#ifndef _BLUETOOTH_H
#define _BLUETOOTH_H


#include <stdint.h>
#include <stdlib.h>
#include <string>
#include "stm32f10x.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_gpio.h"
#include "pin.h"
#include "usart.h"
#include "parser.h"


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


 constexpr static uint32_t BLUETOOTH_SIZE_BUF = 64;


 enum recHeaderType{
 	NO_HEADER=0,
 	SHOT_PACKAGE_HEADER=1,
 	MESSAGE_HEADER=2
 };
 typedef enum recHeaderType TRecHeaderType;

#pragma pack(push, 1)
typedef struct RX_Packet {
uint8_t player_id;	//������������� ��������� � ��� ������
uint8_t team_id;	//������������� (����) ��� �������
uint8_t damage;		//���������� ��� ����
} trx_package;
#pragma pack(pop)


//��������� ��������� ��� �������� ����������� �� �� ���������
#pragma pack(push, 1)
typedef struct IR_Message {
uint8_t ID; //������������ ���������
uint8_t param;//�������� ���������
uint8_t control_byte;//����������� ����
} tir_message;
#pragma pack(pop)



class Bluetooth;

class Bluetooth: public PinIRQ{

public:
	Bluetooth(): at_mode_pin(PORTB,PIN10), /*state_pin(PORTC,PIN7),*/ reset_pin(PORTC,PIN4), usart(9600){
		self_pointer = this;
		vSemaphoreCreateBinary(xBluetoothSemaphore);
		vSemaphoreCreateBinary(xOkReceivedSemaphore);

		xTaskCreate( vListeningTask,/* ( signed char * )*/ "ListeningTask", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2,
		                            ( xTaskHandle * ) NULL);


		at_mode_pin.switchToOutput();
		at_mode_pin.reset();
		state_pin.switchToInput();
//		state_pin.setCallback((ExtiCallbackFuncPointer) Bluetooth::status_has_changed);
		state_pin.setCallback((TPIN_IRQ_CALLBACK) this);
		state_pin.enableExti(true);
		reset_pin.switchToOutput();
		reset_pin.set();
		usart.iqr_enable();

};
	void	pin_irq_callback(void) override;

	//static void status_has_changed(void);
	void reset(void);
	static void PutChar(unsigned char sym){
		Usart_1::PutChar(sym);
	}


	//������� ���������� ������ �� usart`�
	static void SendStr(char * data){
		Usart_1::SendStr(data);
	}
	static void SlowSendStr(char * data){
		Usart_1::SlowSendStr(data);
	}

	static bool connectStatus(void){
		return state_pin.isSet();
	}
	static xSemaphoreHandle xBluetoothSemaphore;
	static xSemaphoreHandle xOkReceivedSemaphore;
	static void getPackageValue(trx_package&);
	static void getMessageValue(tir_message&);
	static void statePinEnable(void);
	void set_at_commands_mode(bool mode);
	void switch_to_at_mode(void);
	void switch_to_data_mode(void);
	bool send_test_at_command(void);
	bool configure(void);
	bool in_at_mode(void);
	bool send_set_at_command(char* cmd, char* param);
	bool send_at_command(char* cmd);
private:
	static Pin state_pin;
	Pin at_mode_pin;

	Pin reset_pin;

	Usart_1 usart;
	static Bluetooth* self_pointer;
	static void vListeningTask (void *pvParameters);
	static void testBluetoothData(void);
	static unsigned char rx_buffer[64];

protected:



};




#endif
