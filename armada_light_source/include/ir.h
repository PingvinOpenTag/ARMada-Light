#ifndef _IR_H
#define _IR_H
#include <stdint.h>
#include <stdlib.h>
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_tim.h"
#include "pin.h"
#include "miles_protocol.h"
#include "bluetooth.h"

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

//���� ��� ����� ������� (������� �����) �� ����� ������, ���� ������ - ����������������
#define BLACK_PCB


#define TX_BUFFER_SIZE   40 //������ ������� �����������
#define RX_BUFFER_SIZE   40 //������ ������� ���������
#define RX_BLUETOOTH_BUFFER_SIZE 40 //������ ������� ��������� (� ������)
#define ZONE_RX_BUFFER_SIZE   40 //������ ������� ��������� ��� ���������
#define IR_RAW_BUFFER_SIZE 256 //������ ������ ��-���������



typedef struct IR_Tx_Buffer_Cursor {
uint8_t byte_pos; //����� ����� � ������ �����������
uint8_t bit_mask;//������� ����� ��� ��������� �������� ����
uint16_t bits_for_tx;//������� ��� ����� ��� ��������
bool heder_been_sent;//����, �����������, ��������� �� ��������� ������

} tir_tx_buffer_cursor;


enum recStatus{
	REC_Idle,
	REC_Recording,
	REC_Captured
};
typedef enum recStatus TRecStatus;


//��������� ������������� ��� ��� ������� ��-���������
enum Rx_Event {
				NOT_EVENT, 		//��� �������
				RX_COMPLETE, 	//������ �����
				RX_MESSAGE_COMPLETE,//������� ���������
				RX_ERROR,		//������ ������ ������
				CHITER_DETECTED, //������-������� ����������� �� ����� ����
				BANDANA_BATTARY_LOW //�� ������-������� ��������� �������
			};
typedef enum Rx_Event trx_event; //��������� ������������� ��� ��� ������� ��-���������



enum damage_zone{
	zone_4,
	zone_3,
	zone_2,
	zone_1
};
typedef enum damage_zone TDamageZone;


class IrTransceiver{

public:
	IrTransceiver();
	static void init_f0_pwm(void);
	static void init_sender_pwm(void);

	static tir_tx_buffer_cursor ir_tx_buffer_cursor; //������ ����������� �� ������
static void bit_send_done_callback(void);
static uint8_t tx_buffer[TX_BUFFER_SIZE]; 	//������ ��-�����������
static void send_shot_package(void);
void set_player_id(uint8_t ID);
void set_team_color(tteam_color  color);
void set_gun_damage(tgun_damage damage);
void low_power(void);
void hight_power(void);
void init_receiver(void);
static void receiver_IQR_callback(void);
static void getPackageValue(trx_package&);
static void hitProcessing(void);
private:
static TIM_OCInitTypeDef timer8_PWM;
static TRecStatus tim2_rec_state;
static uint16_t tim2_ir_rx_count;
Pin ir_power_switch_pin;
static Pin flash_led_pin;
static Pin vibro_pin;
static trx_event Zone2RxEvent;//������� 2-�� ����
static void set_zone_buffer_bit (TDamageZone zone, uint8_t index, bool value);
static uint8_t zone2_rx_buffer[ZONE_RX_BUFFER_SIZE]; 	//������ ��-���������
static xSemaphoreHandle xZone2Semaphore;
static void Zone2task(void *pvParameters);
protected:

};






#endif
