#include "hd44780_driver.h"

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

extern unsigned char Decode2Rus[];

void lcd_delay(void) {
	//vTaskDelay(1);

	volatile uint32_t tmpvar;
	for (tmpvar=/*1333*/4000;tmpvar!=0;tmpvar--);

}

void lcd_alternative_init(){
	LCD_PORT->CRH |= LCD_PORT_CRH_S;
		LCD_PORT->CRL |= LCD_PORT_CRL_S;
		LCD_PORT->CRH &= ~(LCD_PORT_CRH_C);
		LCD_PORT->CRL &= ~(LCD_PORT_CRL_C);

		/*
		lcd_send(0x00,COMMAND);
		lcd_send(0x00,COMMAND);
		lcd_send(0x00,COMMAND);
		lcd_send(0x00,COMMAND);
		lcd_send(0x00,COMMAND);
*/

		lcd_send(0x33,COMMAND);



		lcd_send(0x32,COMMAND);//function set
		lcd_send(0x28,COMMAND);//function set - 2line 5x8dot 0x2a EN_RU 0x28 EN_JP
		lcd_send(0x08,COMMAND);//0x08 display off cursor off blink off
		lcd_send(0x01,COMMAND);//display clear;
		lcd_send(0x02,COMMAND);//return home
		lcd_send(0x06,COMMAND);//entry mode set
		lcd_send(0x0C,COMMAND);//0x0c display on cursor off blink off


}
void lcd_init() {




	LCD_PORT->CRH |= LCD_PORT_CRH_S;
	LCD_PORT->CRL |= LCD_PORT_CRL_S;
	LCD_PORT->CRH &= ~(LCD_PORT_CRH_C);
	LCD_PORT->CRL &= ~(LCD_PORT_CRL_C);

	lcd_send(0x00,COMMAND);
	lcd_send(0x00,COMMAND);
	lcd_send(0x00,COMMAND);
	lcd_send(0x00,COMMAND);
	lcd_send(0x00,COMMAND);



	lcd_set_4bit_mode();
	lcd_set_state(LCD_ENABLE,/*CURSOR_ENABLE*/CURSOR_DISABLE,NO_BLINK);
	lcd_clear();
	lcd_send(0x06,COMMAND);
}

void lcd_set_user_char(uint8_t char_num, uint8_t * char_data) {
	uint8_t i;
	lcd_send(((1<<6) | (char_num * 8) ), COMMAND);
	for (i=0;i<=7;i++) {
		lcd_send(char_data[i],DATA);
	}
	lcd_send((1<<7), COMMAND);
}

void lcd_set_xy(uint8_t x, uint8_t y)  {
	if (y==0) {
		lcd_send( ((1<<7) | (x)),COMMAND);
	} else {
		lcd_send( ((3<<6) | (x)),COMMAND);
	}
}


void lcd_out(char * txt) {
	while(*txt) {
	//	if(*txt>=192) *txt = Decode2Rus[*txt-192];

		lcd_send(*txt,DATA);
		txt++;
	}
}

void lcd_clear(void) {
	lcd_send(0x01,COMMAND);
}

void lcd_set_state(lcd_state state, cursor_state cur_state, cursor_mode cur_mode)  {
	if (state==LCD_DISABLE)  {
		lcd_send(0x08,COMMAND);
	} else {
		if (cur_state==CURSOR_DISABLE) {
			if (cur_mode==NO_BLINK)  {
				lcd_send(0x0C,COMMAND);
			} else {
				lcd_send(0x0D,COMMAND);
			}
		} else  {
			if (cur_mode==NO_BLINK)  {
				lcd_send(0x0E,COMMAND);
			} else {
				lcd_send(0x0F,COMMAND);
			}
		}
	}
}

void lcd_set_4bit_mode(void) {

	LCD_PORT->BSRR=(LCD_DB7_BC | LCD_DB6_BC | LCD_DB5_BC | LCD_DB4_BC | LCD_CD_BC | LCD_EN_BC);
	LCD_PORT->BSRR=(LCD_DB5_BS);

	LCD_PORT->BSRR=LCD_EN_BS;
	lcd_delay();
	LCD_PORT->BSRR=LCD_EN_BC;
	lcd_delay();

	LCD_PORT->BSRR=(LCD_DB7_BC | LCD_DB6_BC | LCD_DB5_BC | LCD_DB4_BC | LCD_CD_BC | LCD_EN_BC);
	LCD_PORT->BSRR=(LCD_DB5_BS);

	LCD_PORT->BSRR=LCD_EN_BS;
	lcd_delay();
	LCD_PORT->BSRR=LCD_EN_BC;
	lcd_delay();

	LCD_PORT->BSRR=(LCD_DB7_BC | LCD_DB6_BC | LCD_DB5_BC | LCD_DB4_BC | LCD_CD_BC | LCD_EN_BC);
	LCD_PORT->BSRR=(LCD_DB7_BS);
	LCD_PORT->BSRR=(LCD_DB5_BS);//its my

	LCD_PORT->BSRR=LCD_EN_BS;
	lcd_delay();
	LCD_PORT->BSRR=LCD_EN_BC;
	lcd_delay();

}

void lcd_send(uint8_t byte, dat_or_comm dc)  {

	LCD_PORT->BSRR=(LCD_DB7_BC | LCD_DB6_BC | LCD_DB5_BC | LCD_DB4_BC | LCD_CD_BC | LCD_EN_BC);

	if (dc) {
		LCD_PORT->BSRR=LCD_CD_BS;
	}

	if (byte & 0x10) {
		LCD_PORT->BSRR=LCD_DB4_BS;
	}
	if (byte & 0x20) {
		LCD_PORT->BSRR=LCD_DB5_BS;
	}
	if (byte & 0x40) {
		LCD_PORT->BSRR=LCD_DB6_BS;
	}
	if (byte & 0x80) {
		LCD_PORT->BSRR=LCD_DB7_BS;
	}

	LCD_PORT->BSRR=LCD_EN_BS;
	lcd_delay();
	LCD_PORT->BSRR=LCD_EN_BC;
	lcd_delay();


	LCD_PORT->BSRR=(LCD_DB7_BC | LCD_DB6_BC | LCD_DB5_BC | LCD_DB4_BC );

	if (byte & 0x01) {
		LCD_PORT->BSRR=LCD_DB4_BS;
	}
	if (byte & 0x02) {
		LCD_PORT->BSRR=LCD_DB5_BS;
	}
	if (byte & 0x04) {
		LCD_PORT->BSRR=LCD_DB6_BS;
	}
	if (byte & 0x08) {
		LCD_PORT->BSRR=LCD_DB7_BS;
	}

	LCD_PORT->BSRR=LCD_EN_BS;
	lcd_delay();
	LCD_PORT->BSRR=LCD_EN_BC;
	lcd_delay();

}



void lcd_generate_additional_symbols(void){
	lcd_send(((1<<6)|0), COMMAND);


	lcd_send(0b00001110, DATA);
	lcd_send(0b00011111, DATA);
	lcd_send(0b00011111, DATA);
	lcd_send(0b00011111, DATA);
	lcd_send(0b00011111, DATA);
	lcd_send(0b00011111, DATA);
	lcd_send(0b00011111, DATA);
	lcd_send(0b00000000, DATA);

	lcd_send(0b00000000, DATA);
	lcd_send(0b00000000, DATA);
	lcd_send(0b00001010, DATA);
	lcd_send(0b00011111, DATA);
	lcd_send(0b00001110, DATA);
	lcd_send(0b00000100, DATA);
	lcd_send(0b00000000, DATA);
	lcd_send(0b00000000, DATA);

	lcd_send(0b00000110, DATA);
	lcd_send(0b00001111, DATA);
	lcd_send(0b00001111, DATA);
	lcd_send(0b00001111, DATA);
	lcd_send(0b00001111, DATA);
	lcd_send(0b00000110, DATA);
	lcd_send(0b00001111, DATA);
	lcd_send(0b00000000, DATA);


	lcd_send(0b00001110, DATA);
	lcd_send(0b00011010, DATA);
	lcd_send(0b00011110, DATA);
	lcd_send(0b00011010, DATA);
	lcd_send(0b00011110, DATA);
	lcd_send(0b00011010, DATA);
	lcd_send(0b00001111, DATA);
	lcd_send(0b00000000, DATA);


	lcd_send(0b00000110, DATA);
	lcd_send(0b00001001, DATA);
	lcd_send(0b00000110, DATA);
	lcd_send(0b00001111, DATA);
	lcd_send(0b01001111, DATA);
	lcd_send(0b00001111, DATA);
	lcd_send(0b00000110, DATA);
	lcd_send(0b00000110, DATA);



	lcd_send(0b00010000, DATA);
	lcd_send(0b00011111, DATA);
	lcd_send(0b00011111, DATA);
	lcd_send(0b00011111, DATA);
	lcd_send(0b00010000, DATA);
	lcd_send(0b00010000, DATA);
	lcd_send(0b00010000, DATA);
	lcd_send(0b00010000, DATA);

	lcd_send(0b00001100, DATA);
	lcd_send(0b00001010, DATA);
	lcd_send(0b00011010, DATA);
	lcd_send(0b00001100, DATA);
	lcd_send(0b00011010, DATA);
	lcd_send(0b00001001, DATA);
	lcd_send(0b00001010, DATA);
	lcd_send(0b00001100, DATA);




	lcd_send((1<<7), COMMAND);
}


void lcd_generate_bat_level_symbols(uint8_t bt_level){
	lcd_send(((1<<6)|0), COMMAND);
	switch(bt_level)
	{
		case 0:
		{
			lcd_send(0b00001110, DATA);
			lcd_send(0b00010001, DATA);
			lcd_send(0b00010001, DATA);
			lcd_send(0b00010001, DATA);
			lcd_send(0b00010001, DATA);
			lcd_send(0b00010001, DATA);
			lcd_send(0b00011111, DATA);
			lcd_send(0b00000000, DATA);
		}
		break;
		case 1:
		{
			lcd_send(0b00001110, DATA);
			lcd_send(0b00010001, DATA);
			lcd_send(0b00010001, DATA);
			lcd_send(0b00010001, DATA);
			lcd_send(0b00010001, DATA);
			lcd_send(0b00011111, DATA);
			lcd_send(0b00011111, DATA);
			lcd_send(0b00000000, DATA);
		}
		break;
		case 2:
		{
			lcd_send(0b00001110, DATA);
			lcd_send(0b00010001, DATA);
			lcd_send(0b00010001, DATA);
			lcd_send(0b00010001, DATA);
			lcd_send(0b00011111, DATA);
			lcd_send(0b00011111, DATA);
			lcd_send(0b00011111, DATA);
			lcd_send(0b00000000, DATA);
		}
		break;
		case 3:
		{
			lcd_send(0b00001110, DATA);
			lcd_send(0b00010001, DATA);
			lcd_send(0b00010001, DATA);
			lcd_send(0b00011111, DATA);
			lcd_send(0b00011111, DATA);
			lcd_send(0b00011111, DATA);
			lcd_send(0b00011111, DATA);
			lcd_send(0b00000000, DATA);
		}
		break;

		case 4:
		{
			lcd_send(0b00001110, DATA);
			lcd_send(0b00010001, DATA);
			lcd_send(0b00011111, DATA);
			lcd_send(0b00011111, DATA);
			lcd_send(0b00011111, DATA);
			lcd_send(0b00011111, DATA);
			lcd_send(0b00011111, DATA);
			lcd_send(0b00000000, DATA);

		}
		break;
		case 5:
		{	lcd_send(0b00001110, DATA);
			lcd_send(0b00011111, DATA);
			lcd_send(0b00011111, DATA);
			lcd_send(0b00011111, DATA);
			lcd_send(0b00011111, DATA);
			lcd_send(0b00011111, DATA);
			lcd_send(0b00011111, DATA);
			lcd_send(0b00000000, DATA);


		}
		break;
		default: break;
	}

	lcd_send((1<<7), COMMAND);
}
