#ifndef _SYSTEM_H
#define _SYSTEM_H


#define rf_enable

#include <stdint.h>
#include <stdlib.h>
#include "stm32f10x.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_gpio.h"
#include "pin.h"
#include "usart.h"
#include "miles_protocol.h"
#include "bluetooth.h"
#include "adc.h"
#include "keyboard.h"
#include "ir.h"
#include "wavplayer.h"
#include "display.h"





#ifdef __cplusplus
 extern "C" {
  #endif
#include "../FreeRTOS/include/FreeRTOS.h"
#include "../FreeRTOS/include/task.h"
#include "../FreeRTOS/include/queue.h"


#ifdef __cplusplus
  }
#endif

 //��������� ������������� ��� ��� ������� ����
 enum FireMode_Types {
 	SINGLE=0,
 	SEMI_AUTO=1,
 	AUTO=2
 };

 typedef enum FireMode_Types tfire_mode;


 enum IR_Power_Mode{
	 INDOORS=0,
	 OUTDOORS
 };

 typedef enum IR_Power_Mode tir_power_mode;


 enum Display_Type{
	 OLED = 0,
	 COLOR_ILI9163 = 1
 };

 typedef enum Display_Type tdisplay_type;
 constexpr static uint8_t DEFAULT_PLAYER_ID = 1;
 constexpr static uint8_t DEFAULT_TEAM_COLOR = Red;
 constexpr static uint16_t DEFAULT_HEALTH_AFTER_START = 100;
 constexpr static uint16_t DEFAULT_CLIPS_AFTER_START = 15;
 constexpr static uint16_t DEFAULT_ROUNDS_PLACED_IN_CLIP = 30;
 constexpr static tgun_damage DEFAULT_DAMAGE = Damage_25;
 constexpr static tfire_mode DEFAULT_FIRE_MODE = SEMI_AUTO;
 constexpr static bool DEFAULT_AUTOSTART_GAME = true;
 constexpr static bool DEFAULT_FRIENDLY_FIRE = false;
 constexpr static   uint16_t DEFAULT_GUN_RATE = 400;
 constexpr static   uint16_t DEFAULT_GUN_RELOAD_TIME = 100;
 constexpr static   uint16_t DEFAULT_SAFE_TIME = 50;
 constexpr static   uint16_t DEFAULT_SHOCK_TIME = 100;
 constexpr static char default_bluetooth_name[16] = "ARMada_rifle";
 constexpr static tir_power_mode DEFAULT_IR_POWER_MODE = INDOORS;
 constexpr static uint8_t DEFAULT_IR_DUTY_CYCLE = 50;
 constexpr static bool DEFAULT_ZOMBI = false;
 constexpr static uint8_t DEFAULT_ZOMBI_PERIOD = 10;

 constexpr static   uint16_t DEFAULT_BATTARY_LOW_VOLTAGE = 3400;
 constexpr static   uint16_t DEFAULT_BATTARY_FULL_VOLTAGE = 4200;

 constexpr static uint8_t DEFAULT_ACTIVE_PRESET = 0;

 constexpr static uint16_t PRESET_0_DEFAULT_HEALTH_AFTER_START = 100;
 constexpr static uint16_t PRESET_0_DEFAULT_CLIPS_AFTER_START = 15;
 constexpr static uint16_t PRESET_0_DEFAULT_ROUNDS_PLACED_IN_CLIP = 30;
 constexpr static tfire_mode PRESET_0_DEFAULT_FIRE_MODE =  SEMI_AUTO ;
 constexpr static tgun_damage PRESET_0_DEFAULT_DAMAGE = Damage_25;
 constexpr static bool PRESET_0_DEFAULT_FRIENDLY_FIRE = false;
 constexpr static uint8_t PRESET_0_DEFAULT_SHOCK_TIME = 100;
 constexpr static uint16_t PRESET_0_DEFAULT_RELOAD_TIME = 100;
 constexpr static uint8_t PRESET_0_DEFAULT_IR_DUTY_CYCLE = 50;
 constexpr static bool PRESET_0_DEFAULT_ZOMBI = true;
 constexpr static uint8_t PRESET_0_DEFAULT_ZOMBI_PERIOD = 10;

 constexpr static uint16_t PRESET_1_DEFAULT_HEALTH_AFTER_START = 1;
 constexpr static uint16_t PRESET_1_DEFAULT_CLIPS_AFTER_START = 8;
 constexpr static uint16_t PRESET_1_DEFAULT_ROUNDS_PLACED_IN_CLIP = 30;
 constexpr static tfire_mode PRESET_1_DEFAULT_FIRE_MODE = SINGLE ;
 constexpr static tgun_damage PRESET_1_DEFAULT_DAMAGE = Damage_1;
 constexpr static bool PRESET_1_DEFAULT_FRIENDLY_FIRE = true;
 constexpr static uint8_t PRESET_1_DEFAULT_SHOCK_TIME = 5;
 constexpr static uint16_t PRESET_1_DEFAULT_RELOAD_TIME = 150;
 constexpr static uint8_t PRESET_1_DEFAULT_IR_DUTY_CYCLE = 1;
 constexpr static bool PRESET_1_DEFAULT_ZOMBI = true;
 constexpr static uint8_t PRESET_1_DEFAULT_ZOMBI_PERIOD = 10;

 constexpr static uint16_t PRESET_2_DEFAULT_HEALTH_AFTER_START = 1;
 constexpr static uint16_t PRESET_2_DEFAULT_CLIPS_AFTER_START = 8;
 constexpr static uint16_t PRESET_2_DEFAULT_ROUNDS_PLACED_IN_CLIP = 30;
 constexpr static tfire_mode PRESET_2_DEFAULT_FIRE_MODE = SINGLE ;
 constexpr static tgun_damage PRESET_2_DEFAULT_DAMAGE = Damage_1;
 constexpr static bool PRESET_2_DEFAULT_FRIENDLY_FIRE = true;
 constexpr static uint8_t PRESET_2_DEFAULT_SHOCK_TIME = 5;
 constexpr static uint16_t PRESET_2_DEFAULT_RELOAD_TIME = 150;
 constexpr static uint8_t PRESET_2_DEFAULT_IR_DUTY_CYCLE = 1;
 constexpr static bool PRESET_2_DEFAULT_ZOMBI = true;
 constexpr static uint8_t PRESET_2_DEFAULT_ZOMBI_PERIOD = 10;

 constexpr static uint16_t PRESET_3_DEFAULT_HEALTH_AFTER_START = 1;
 constexpr static uint16_t PRESET_3_DEFAULT_CLIPS_AFTER_START = 8;
 constexpr static uint16_t PRESET_3_DEFAULT_ROUNDS_PLACED_IN_CLIP = 30;
 constexpr static tfire_mode PRESET_3_DEFAULT_FIRE_MODE = SINGLE ;
 constexpr static tgun_damage PRESET_3_DEFAULT_DAMAGE = Damage_1;
 constexpr static bool PRESET_3_DEFAULT_FRIENDLY_FIRE = true;
 constexpr static uint8_t PRESET_3_DEFAULT_SHOCK_TIME = 5;
 constexpr static uint16_t PRESET_3_DEFAULT_RELOAD_TIME = 150;
 constexpr static uint8_t PRESET_3_DEFAULT_IR_DUTY_CYCLE = 1;
 constexpr static bool PRESET_3_DEFAULT_ZOMBI = true;
 constexpr static uint8_t PRESET_3_DEFAULT_ZOMBI_PERIOD = 10;

 constexpr static uint16_t PRESET_4_DEFAULT_HEALTH_AFTER_START = 1;
 constexpr static uint16_t PRESET_4_DEFAULT_CLIPS_AFTER_START = 8;
 constexpr static uint16_t PRESET_4_DEFAULT_ROUNDS_PLACED_IN_CLIP = 30;
 constexpr static tfire_mode PRESET_4_DEFAULT_FIRE_MODE = SINGLE ;
 constexpr static tgun_damage PRESET_4_DEFAULT_DAMAGE = Damage_1;
 constexpr static bool PRESET_4_DEFAULT_FRIENDLY_FIRE = true;
 constexpr static uint8_t PRESET_4_DEFAULT_SHOCK_TIME = 5;
 constexpr static uint16_t PRESET_4_DEFAULT_RELOAD_TIME = 150;
 constexpr static uint8_t PRESET_4_DEFAULT_IR_DUTY_CYCLE = 1;
 constexpr static bool PRESET_4_DEFAULT_ZOMBI = true;
 constexpr static uint8_t PRESET_4_DEFAULT_ZOMBI_PERIOD = 10;










 typedef struct  player_type{
 uint8_t id;	//������������� ��������� � ��� ������
 uint8_t team_color;	//������������� (����) ��� �������
 uint16_t health;		//������� �������� ��������
 uint16_t health_after_start;		//�������� �������� ����� ������ ����
 uint8_t shock_time;//����� ���� � 1/100 �������
 uint16_t safe_time;
 bool zomby;
 uint8_t zomby_period;
 } tplayer;

 typedef struct  pgun_type{
 	tgun_damage damage; //��������� ����
 	uint16_t  rounds_placed_in_clip; //������� ������
 	uint16_t  rounds; //���������� �������� � ������ � ������� ������
 	uint16_t clips_after_start;//���������� ����� ����� ������ ����
 	uint16_t clips; //���������� ���������� �����
 	uint16_t reload_time;//����� �����������
 	uint16_t rate;//���������������� (���������/���)
 	uint8_t ir_power;//�������� �� ���������
 	uint8_t ir_power_offset;//�������� �� ������������ ���������
 	uint8_t ir_duty_cycle;//���������� �� �������
 	tfire_mode fire_mode;
 } tgun;



 typedef struct battary_type{
 	uint16_t full_voltage;//���������� ��������� ���������� ������� (� ������������)
 	uint16_t low_voltage;//���������� ��������� ����������� �������
 } tbattary;




 typedef struct bluetooth_type{
 	uint32_t baudrate;//�������� ������ (�������)
 	char name[32];//��� ��������� � ����
 	char mac_adress_for_bind[32];//mac-����� ����������, � �������� ����� ������������� ����������� � ������ ������, ����� CMODE = 0 (connect fixed address)
 	char cmode[4];
 	char role[4];
 	char pswd[6];
 } tbluetooth;

 typedef struct  preset_type{
	 uint16_t health_after_start;
	 uint16_t clips_after_start;
	 uint16_t  rounds_placed_in_clip;
	 tfire_mode fire_mode;
	 tgun_damage damage;
	 bool friendly_fire;
	 uint8_t shock_time;
	 uint16_t reload_time;
	 uint8_t ir_duty_cycle;
	 bool zomby;
	 uint8_t zomby_period;
 } tpreset;



 typedef struct  system_type{
	uint32_t marker;
 	tplayer player;//������ ������
 	tgun gun;//�������������� ������
 	bool friendly_fire;//������������� �����
 	bool autostart_game;//��������� ���� ����� ��������� �������
 	tbluetooth bluetooth;//�������������� � ��������� ������ ������
 	tbattary battary;//�������������� �������
 	uint8_t backlight_level;//������� ������� ������� ���������
  	uint32_t game_session_ID;
//  	uint8_t shock_time;
//  	uint16_t safe_time;
  	char bluetooth_name[16];
  	tir_power_mode ir_power_mode;
  	bool second_weapon_enable = false;
  	tpreset presets[5];
  	uint8_t active_preset;
  	//uint32_t lic_key[4];
 } tsettings;





 class System {

 public:
	 System();
	 static xQueueHandle xEventQueue;
	 static unsigned long *UID;
static 	Bluetooth* bluetooth;
static ADC* adc;
static Keyboard* keyboard;
static IrTransceiver* ir;
static WavPlayer* wavPlayer;

//static OledDisplay* display;
static Display* display;
static bool check_license(void);
static void hitProcessing(void);
static void messageProcessing(void);
static tsettings settings;
static void load_settings(void);
static void save_settings(void);
static void reset_settings(void);
static void apply_preset(uint8_t preset);
static void on_trigger_pressed(void);
static void on_reload_pressed(void);
static void on_fire_mode_pressed(void);
static bool in_game(void);
static void start_new_game(void);
static void game_over(bool zomby = false);
//static void silence_game_over(void);
static void incorrect_lic_key(void);
static tdisplay_type get_display_type(uint8_t detect_port, uint8_t detect_pin);
static uint16_t bandana_led_on_timer;
static uint16_t safe_counter;
static uint16_t shock_counter;
static bool matryoshka_enable;
static bool now_zomby;
static Pin second_weapon_pin;
static bool update_statistics(trx_package& ir_package, bool is_fatal);
 private:
	static void vMainLoopTask (void *pvParameters);
	void get_uid(uint32_t *text)
	{
		text[0]= UID[0];
		text[1]= UID[1];
		text[2]= UID[2];
	}
	static uint32_t deviceUID[3];
	static bool game_status;
	static void check_battary(void);



 protected:

 };





#endif
