#ifndef __TYPES_H
#define __TYPES_H

#include "stm32f10x.h"
#include "miles_protocol.h"

#define IN_GAME 				true
#define OUT_OF_GAME 			false

#define TAG 0
#define BANDANA 1


//���� ������ ������ �������� �����
//#define OLD_VERSION_PCB

//���� ����������: BANDANA ��� TAG
//#define DEVICE_ROLE BANDANA
#define DEVICE_ROLE TAG


#define DISPLAY_ENABLE
#define COLOR_LCD
#define LCD_NOT_STANDART


//#define RTC_Enable
#define LOW_DROP_ENABLE

#define SI4432_ENABLE

#define FULL_CLIP_AFTER_START


//����������� �����������, ������ �������� �����
//#define DIRECTION_CALCULATION
#define HIT_TIMEOUT 10
// 1 to v1.1 , 2 to v2.2
#define LCD_MODUL_VERSION 2


#define batt_text_x_pos 80
#define batt_text_y_pos 21


#define FLASH_LED_ON (GPIO_SetBits(GPIOB, GPIO_Pin_1))
#define FLASH_LED_OFF (GPIO_ResetBits(GPIOB, GPIO_Pin_1))

#ifndef bool

                        #define bool unsigned char

                        #define true 1

                        #define false 0

#endif
//������������ ������� ��������� ��������
//#define SENSORS_BECKLIGHT



#define NUMBER_OF_SENSORS_FRAMES 8

#define TX_BUFFER_SIZE   40 //������ ������� �����������
#define RX_BUFFER_SIZE   40 //������ ������� ���������
#define RX_BLUETOOTH_BUFFER_SIZE 40 //������ ������� ��������� (� ������)
#define ZONE_RX_BUFFER_SIZE   40 //������ ������� ��������� ��� ���������

#define IR_RAW_BUFFER_SIZE 256 //������ ������ ��-���������

#define SOUND_BUFFER_SIZE 512//512

#define HIGH_BRIDHTNESS (1<<4)
#ifdef SENSORS_BECKLIGHT
#define RED ((1<<2))//�������
#define BLUE ((1<<1))//�����
#define GREEN ((1<<0))//������
#else
#define RED ((1<<2)|HIGH_BRIDHTNESS)//�������
#define BLUE ((1<<1)|HIGH_BRIDHTNESS)//�����
#define GREEN ((1<<0)|HIGH_BRIDHTNESS)//������

#endif
#define CYAN (BLUE|GREEN)//�������
#define YELLOW (RED|GREEN)//������
#define MAGENTA (RED|BLUE)//���������
#define WHITE (RED|GREEN|BLUE)//�����
#define BLACK (0)
//((1<<4)|(1<<5)|(1<<6))//������
#define VIBRO_HP (1<<3)//��� �������������� (������� ��������)
#define VIBRO_LP (1<<3)//��� �������������� (����� ��������)
#define RED_WO_VIBRO RED
//(~((1<<6)| VIBRO_HP | VIBRO_LP))//������� ��� ��������


/*
#define Z1 3;//������ � ������� ������� ������ ����
#define Z2 2;//������ � ������� ������� ������ ����
#define Z3 1;//������ � ������� ������� ������� ����
#define Z4 0;//������ � ������� ������� ��������� ����
*/
//--------------------------------------------------

typedef struct IR_Tx_Buffer_Cursor {
uint8_t byte_pos; //����� ����� � ������ �����������
uint8_t bit_mask;//������� ����� ��� ��������� �������� ����
uint16_t bits_for_tx;//������� ��� ����� ��� ��������
bool heder_been_sent;//����, �����������, ��������� �� ��������� ������

} tir_tx_buffer_cursor;

typedef struct IR_RX_BIT {
uint16_t Pulse;
uint16_t Period;
} tir_rx_bit;





enum recStatus{
	REC_Idle,
	REC_Recording,
	REC_Captured
};
typedef enum recStatus TRecStatus;


enum damage_zone{
	zone_4,
	zone_3,
	zone_2,
	zone_1
};
typedef enum damage_zone TDamageZone;

struct pressing_duration
		{ uint16_t key_1      ; //������������ ������������
					    //������� ������ 1 ("������� ����")
		  unsigned int key_1_inc:1; //���, �����������
					    //��� ����������� ������
					    //������������ ������� ������ 1
		  uint16_t key_2      ; //������������ ������������
					    //������� ������ 2 ("������������")
		  unsigned int key_2_inc:1; //���, �����������
					    //��� ����������� ������
					    //������������ ������� ������ 2



		  unsigned int no_key   ;

		};



enum typkeyboard_status {
			no_key_pressed,
			key_pressed
			};
typedef enum typkeyboard_status TKEYBOARD_STATUS;

enum typkeyboard_event  {
			no_key_pressing,
			key_pressing
	        };
typedef enum typkeyboard_event TKEYBOARD_EVENT;


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

//������ ���������, � ������� ����� ������� ������, ��������� �� ����������� ������

#pragma pack(push, 1)
typedef struct RX_Packet {
uint8_t player_id;	//������������� ��������� � ��� ������
uint8_t team_id;	//������������� (����) ��� �������
uint8_t damage;		//���������� ��� ����
} trx_packet;
#pragma pack(pop)

typedef struct tag_init_data_type{
	uint8_t player_id;
	uint8_t team_id;	//������������� (����) ��� �������
	bool game_status;
	uint8_t health;
	uint8_t command;
	uint32_t game_session_ID;//
	uint8_t Checksum;
} ttag_init_data;

//#pragma pack(push, 1)
typedef struct cloning_data_type{
/* Byte number*/
/*1*/	/*ID*/ 			//������������ ���������
/*2*/	/*param*/			//�������� ���������
/*3*/	/*control_byte*/	//����������� ����
//��������������� ������ ��� ������������
/*4*/	uint8_t reserved_1;		//���������������
/*5*/	uint8_t reserved_2;		//���������������
/*6*/	uint8_t reserved_3;		//���������������
/*7*/	uint8_t Team_ID; 		//������������� �������
/*8*/	uint8_t reserved_4;		//���������������
/*9*/ 	uint8_t Clips_added_by_picking_up_ammo_box;
/*10*/	uint8_t Health_added_by_picking_up_medic_box;
/*11*/ 	uint8_t reserved_5;		//���������������
/*12*/ 	uint8_t Hit_LAD_timeout;//������������ ������� ���������� ��� ���������, � ��������
/*13*/ 	uint8_t Sound_set;		//
/*14*/ 	uint8_t Overheat_limit;	//������������ ���������� ��������� � ������, ���������� �������� �������� � ��������� ������
/*15*/ 	uint8_t reserved_6;		//���������������
/*16*/ 	uint8_t reserved_7;		//���������������
/*17*/	uint8_t Damage_per_shot;//��������� ������� ���� �� ���� ���������
/*18*/	uint8_t Clip_size;		//������� ��������, 0xFF - �������������� ����������
/*19*/	uint8_t Number_of_clips;//���������� �����, 0xFF - �������������� ����������
/*20*/	uint8_t Fire_selector;	//
/*21*/	uint8_t Nonber_of_rounds_of_burst_mode;//���������� ��������� ��� ������������������ ��������
/*22*/	uint8_t	Cyclic_RPM;
/*23*/ 	uint8_t	Reload_delay;	//����� �����������, � ��������
/*24*/ 	uint8_t	IR_power;		//�������� �� �������
/*25*/ 	uint8_t IR_range;		//
/*26*/ 	uint8_t Tagger_on_off_settings;		//
/*27*/ 	uint8_t Respawn_health;
/*28*/ 	uint8_t reserved_8;		//���������������
/*29*/ 	uint8_t Respawn_delay_in_tens_of_seconds; //
/*30*/ 	uint8_t Armour_value;
/*31*/ 	uint8_t Game_on_off_settings_1_2;
/*32*/ 	uint8_t Game_on_off_settings_2_2;
/*33*/ 	uint8_t	Hit_delay;
/*34*/ 	uint8_t Start_delay_in_seconds; //� ��������
/*35*/ 	uint8_t Death_delay_in_seconds;//� ��������
/*36*/ 	uint8_t Time_limit_in_minutes;
/*37*/ 	uint8_t Maximum_respawns;
/*38*/ 	uint8_t reserved_9;		//���������������
/*39*/ 	uint8_t Checksum;

} tcloning_date_package;

//#pragma pack(pop)

typedef union clone_data_union{
	tcloning_date_package clone_data;
	uint8_t raw_data[sizeof(tcloning_date_package)];
	ttag_init_data tag_init_data;
	uint8_t raw_tag_init_data[sizeof(ttag_init_data)];
} tclone_data_union;

//��������� ��������� ��� �������� ����������� �� �� ���������

typedef struct IR_Message {
uint8_t ID; //������������ ���������
uint8_t param;//�������� ���������
uint8_t control_byte;//����������� ����
tclone_data_union clone_data_union;//������ ��� ������������
} tir_message;


typedef struct  player_type{
uint8_t id;	//������������� ��������� � ��� ������
uint8_t team_color;	//������������� (����) ��� �������
uint8_t health;		//������� �������� ��������
uint8_t health_after_start;		//�������� �������� ����� ������ ����
uint8_t shock_time;//����� ���� � 1/100 �������
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
	uint8_t ir_duty_ctcle;//���������� �� �������
} tgun;



typedef struct battary_type{
	uint16_t full_voltage;//���������� ��������� ���������� ������� (� ������������)
	uint16_t low_voltage;//���������� ��������� ����������� �������
} tbattary;

//��������� ������������� ��� ��������� ������
enum SystmSounds_Types {
	NOTHING,//������
	SHOT, 		//�������
	HIT, 	//�������
	START_GAME,//������ ����� ����
	GAME_OVER,//����� ��� ����
	EMPTY_CLIP,//������ ������
	RELOAD,//���������
	MESSAGE,		//��������� ��� ������
	SONAR,//���� ������
	NO_CLIPS,//����������� ������
	USER_SOUND //��� ������ ������
	};
typedef enum SystmSounds_Types tsysSoundType; //��������� ������������� ��� ��� ������� ��-���������

//��������� ������������� ��� ��� ������� ����
enum FireMode_Types {
	SINGLE,
	SEMI_AUTO,
	AUTO
};

typedef enum FireMode_Types tfire_mode;

typedef struct wav_player_type{
	bool stop;
	tsysSoundType type_of_reproduced_sound;//��� ���������������� � ������ ������ �����
	tsysSoundType  type_of_sound_to_play;//��� �����, ������� ����� �������������
	char shot_sound_file_name[32];
	char hit_sound_file_name[32];
	char start_game_sound_file_name[32];
	char game_over_sound_file_name[32];
	char empty_clip_sound_file_name[32];
	char reload_sound_file_name[32];
	char message_sound_file_name[32];
	char sonar_sound_file_name[32];
} twav_player;

//��������� ��������� �������� �������
enum events_source_type {
	UNKNOWN,
	TRIGGER_KEY,
	RELOAD_KEY,
	FIRE_MODE_SWITCH,
	BLUETOOTH
};

typedef enum events_source_type tevents_source_type;

typedef struct system_event_type{
	tevents_source_type event_source;
	char event_code;
} tsystem_event_type;

/*
//��������� ������������� ��� ��� ������� ������ ������
enum BT_Cmode_Types {
	connect_fixed_address = 0,//���������� � ���������� �����������
	connect_any_address, 	//���������� � ����� �����������
	slave_loop //�������� �����
	};
typedef enum  BT_Cmode_Types BT_Cmode ; //��������� ������������� ��� ��� ������� ��-���������

//��������� ������������� ��� ��� ����� ������
enum BT_Role_Types {
	Slave = 0,//���������� � ���������� �����������
	Master, 	//���������� � ����� �����������
	Slave_Loop //�������� �����
	};
typedef enum  BT_Role_Types BT_Role ; //��������� ������������� ��� ��� ������� ��-���������
*/



typedef struct bluetooth_type{
	uint32_t baudrate;//�������� ������ (�������)
	char name[32];//��� ��������� � ����
	char mac_adress_for_bind[32];//mac-����� ����������, � �������� ����� ������������� ����������� � ������ ������, ����� CMODE = 0 (connect fixed address)
	char cmode[4];
	char role[4];
	char pswd[6];
} tbluetooth;


typedef struct  system_type{
	tplayer player;//������ ������
	tgun gun;//�������������� ������
	twav_player wav_player;//�������������� � ������ wav ������
	bool friendly_fire;//������������� �����
	bool autostart_game;//��������� ���� ����� ��������� �������
	tbluetooth bluetooth;//�������������� � ��������� ������ ������
	tbattary battary;//�������������� �������
	uint8_t backlight_level;//������� ������� ������� ���������
#if DEVICE_ROLE==TAG
	uint32_t game_session_ID;
#endif
	uint8_t shock_time;
} tarmada_system;






#define JD0 2451911 // ���� �� 01 ��� 2001 ��
typedef struct{
uint16_t year;
uint8_t month;
uint8_t day;
uint8_t hour;
uint8_t minute;
uint8_t second;
} ftime_t;


//��������� ��� ��� ������ ������������� ����
enum tag_init_command_type {
	NEW_CONNECT,
	NEW_GAME,
	STOP_GAME,
	HEALTH_UPDATE,
	PLAYER_TEAM_ID_UPDATE
};

typedef enum tag_init_command_type ttag_init_command;



enum picture_on_the_screen{
	NO_PICTURE,
	MAIN_GUI,
	GUNS_PIC,
	GAME_OVER_PIC,
	CHITER_PIC
};

typedef enum picture_on_the_screen tpicture_on_the_screen;


enum rf_data_type{
	PING,
	STATUS_DATA,
	NICKNAME
};

typedef enum rf_data_type trf_data_type;


#pragma pack(push, 1)
typedef struct hit_confirmation{
	uint8_t need_to_send:1;//����� �� ���������� �������������
	uint8_t in_game :1;//� ���� ��� ��� ����
	uint8_t team_color:2; //���� �������
	uint8_t gun_damage:4;//��������� ����
	uint8_t player_id;//������������� ������
}thit_confirmation;

#pragma pack(pop)

#pragma pack(push, 1)

typedef struct  status_data_payload{
	 trf_data_type type; //��� ������������ ������						8 ���
	 uint8_t team_color :2;//���� �������								2 ����
	 uint8_t gun_damage:4;//����, ������� ������� ������				4 ����
	 uint8_t in_game :1;//� ���� ��� ��� ����							1 ���
	 uint8_t bluetooth_is_connected:1;//���������� �� ������-�������	1 ���
	 uint8_t player_id;//������������� ������							8 ���
	 uint8_t health;//��������											8 ���
	 uint16_t battery_voltage;//���������� �� ������� (� ������������)	16 ���
	 uint16_t clips;//���������� �����									16 ���
	 unsigned char nickname[15];//�������								15*8 ���
//	 trx_packet hit_data;//												3*8 ���
	 thit_confirmation  hit_data;//										2*8 ���

}  tstatus_payload;

#pragma pack(pop)


#endif /* __TYPES_H */
