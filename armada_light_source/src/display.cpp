
#include <bitmaps.h>
#include "display.h"
#include "system.h"
#ifdef __cplusplus
 extern "C" {
  #endif

#include "bitmap.h"
#include "ws2812b.h"
#ifdef __cplusplus
  }
#endif







//extern const uint16_t gerb_pic[0x4000];
extern	System sys;



const uint32_t* lic_key = (uint32_t*) 0x807F800;




OledDisplay::OledDisplay():power_pin(PORTC,PIN10){
//	init();
power_pin.set();
power_pin.switchToOutputOD();
}


extern RGB_t leds[];



const char hex_numbers[]={"0123456789ABCDEF"};



const char numbers[]={"0123456789"};

char* Display::int_to_str(uint8_t x, uint8_t digits){

	static volatile char str[6];



	volatile uint8_t celoe, ostatok;
	celoe=x;

	int digits_tmp;
	digits_tmp=digits;
	if (digits == 0) digits_tmp=3;
	      for (int i=(digits_tmp-1); i>=0; i--)
	      {

	      ostatok= celoe%10;
		  celoe  = celoe/10;
		  str[i]= numbers[ostatok];
	      }
	      str[digits_tmp]='\0';



	if (digits == 0)
	{
	        while ((str[0]=='0')&str[1] !='\0') for (int i=0; i < 6; i++) str[i]=str[i+1];
	}

	      return (char*)&str;

	}

	char* Display::int_to_str(uint16_t x, uint8_t digits){

	static volatile char str[6];



	volatile uint16_t celoe, ostatok;
	celoe=x;

	int digits_tmp;
	digits_tmp=digits;
	if (digits == 0) digits_tmp=5;
	      for (int i=(digits_tmp-1); i>=0; i--)
	      {

	      ostatok= celoe%10;
		  celoe  = celoe/10;
		  str[i]= numbers[ostatok];
	      }
	      str[digits_tmp]='\0';



	if (digits == 0)
	{
	        while ((str[0]=='0')&str[1] !='\0') for (int i=0; i < 6; i++) str[i]=str[i+1];
	}

	      return (char*)&str;

}






void OledDisplay::init(void){
	//lcd_init();
	lcd_alternative_init();
	lcd_generate_additional_symbols();
//	uint8_t user_char[8];

}

void OledDisplay::show_logo(void){
	go_to_xy(0,0);
	print((char*)"Firmware");
	go_to_xy(0,1);
	print((char*)"Ver. 0.1");

}
void OledDisplay::print(char* str){
	lcd_out(str);
}


void OledDisplay::go_to_xy(uint8_t x, uint8_t y){

	lcd_set_xy(x,y);
}


void OledDisplay::set_user_char(uint8_t char_num, uint8_t * char_data){
	lcd_set_user_char(char_num, char_data);
}

void OledDisplay::print_user_char(uint8_t byte, dat_or_comm dc){
	lcd_send(byte, dc);
}


void OledDisplay::update_battary_voltage(void){
int voltage;
uint8_t curr_batt_level; //������� ������ ��������� ������� (1 �� 6 ���������)
voltage = sys.adc->voltage();
go_to_xy(0,7);
if (voltage < LOW_VOLTAGE){
	lcd_generate_bat_level_symbols(0);
	return;
}
if (voltage > FULL_VOLTAGE){
	lcd_generate_bat_level_symbols(5);
	return;
}
curr_batt_level = (6*(voltage -LOW_VOLTAGE))/(FULL_VOLTAGE - LOW_VOLTAGE);
lcd_generate_bat_level_symbols(curr_batt_level);


}

void OledDisplay::update_health(void){
	go_to_xy(1,0);
	print((char*)int_to_str(sys.settings.player.health,3));
}


void OledDisplay::update_rounds(void){
	go_to_xy(1,1);
	print((char*)int_to_str(sys.settings.gun.rounds,3));
}

void OledDisplay::update_clips(void){
	go_to_xy(6,1);
	print((char*)int_to_str(sys.settings.gun.clips,2));
	print((char*)" ");

}

void OledDisplay::update_fire_mode(void){
	go_to_xy(5,0);

	switch(sys.settings.gun.fire_mode){
			case SINGLE:
			{
				print((char*)"S");
			}
			break;
			case SEMI_AUTO:
			{
				print((char*)"A");

			}
			break;
			case AUTO:
			{
				print((char*)"Q");
			}
			break;
			default:break;
			}

}



void OledDisplay::init_gui(bool game_status){

lcd_clear();
if (!game_status){
	go_to_xy(0,0);
	print((char*)"  GAME  ");
	go_to_xy(0,1);
	print((char*)"  OVER  ");
	return;

}

go_to_xy(0,0);
print_user_char(1,DATA);
update_health();

go_to_xy(7,0);
print_user_char(0,DATA);
update_battary_voltage();

go_to_xy(0,1);
print_user_char(2,DATA);
update_rounds();

go_to_xy(4,1);
print((char*)"/");
print_user_char(3,DATA);
update_clips();
update_fire_mode();
update_bluetooth();


}



void OledDisplay::update_bluetooth(void){

if (!sys.in_game()) return;
if(sys.bluetooth->connectStatus())
	{
		go_to_xy(6,0);
		print_user_char(6,DATA);

	}
else
	{
		go_to_xy(6,0);
		print((char*)" ");
	}

}

void OledDisplay::incorrect_lic_key(void){
	lcd_clear();
	go_to_xy(3,0);
	print((char*)"NO");
	go_to_xy(0,1);
	print((char*)"LICENSE!");

}

