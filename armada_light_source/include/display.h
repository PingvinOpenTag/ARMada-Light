#ifndef _DISPLAY_H
#define _DISPLAY_H

#include <stdint.h>
#include <stdlib.h>
#include "stm32f10x.h"
//#include "stm32f10x_usart.h"
#include "stm32f10x_gpio.h"
#include "pin.h"



#ifdef __cplusplus
 extern "C" {
  #endif
#include "../FreeRTOS/include/FreeRTOS.h"
#include "../FreeRTOS/include/task.h"
#include "../FreeRTOS/include/queue.h"
#include "../FreeRTOS/include/semphr.h"
#include "hd44780_driver.h"

#ifdef __cplusplus
  }
#endif

 //�������� ��� ������� �� ����  ����� �����

 constexpr static uint32_t FULL_VOLTAGE = 4200;
 constexpr static uint32_t LOW_VOLTAGE = 3400;


 //�������� ��� ������� �� �����  ����� �����
/*
 constexpr static uint32_t FULL_VOLTAGE = 4200*2;
 constexpr static uint32_t LOW_VOLTAGE = 3400*2;
*/


 extern const uint32_t* lic_key;

 class Display {

public:
	Display(){
		vSemaphoreCreateBinary(xDisplaySemaphore);
		//vSemaphoreCreateBinary(xGameOverSemaphore);
	};
virtual void init(void)=0;
virtual void print(char* str) = 0;
virtual void go_to_xy(uint8_t x, uint8_t y) = 0;
virtual void update_battary_voltage(void)=0;
virtual void update_health(void)=0;
virtual void update_rounds(void) = 0;
virtual void update_clips(void) = 0;
virtual void update_bluetooth(void) = 0;
virtual void init_gui(bool game_status)=0;
virtual void update_fire_mode(void)=0;
virtual void power_on(void) = 0;
virtual void power_off(void) = 0;
virtual void show_logo(void) = 0;
virtual void incorrect_lic_key(void) = 0;
xSemaphoreHandle xDisplaySemaphore;
//xSemaphoreHandle xGameOverSemaphore;






private:


protected:
char* int_to_str(uint8_t x, uint8_t digits);
char* int_to_str(uint16_t x, uint8_t digits);

 };



class OledDisplay:public Display
{
public:
	 OledDisplay();
void init(void) override;
void print (char* str) override;
void go_to_xy(uint8_t x, uint8_t y) override;
void update_battary_voltage(void) override;
void update_health(void) override;
void update_rounds(void)override;
void update_clips(void) override;
void update_bluetooth(void)override;
void update_fire_mode(void)override;
void init_gui(bool game_status) override;
void show_logo(void) override;
void set_user_char(uint8_t char_num, uint8_t * char_data);
void print_user_char(uint8_t byte, dat_or_comm dc);
void incorrect_lic_key(void) override;

void power_on(void) override{
	power_pin.reset();
}
void power_off(void)override{
	LCD_PORT->BSRR=(LCD_DB7_BC | LCD_DB6_BC | LCD_DB5_BC | LCD_DB4_BC | LCD_CD_BC | LCD_EN_BC);
	power_pin.set();
}
private:
Pin power_pin;


protected:



};



#endif
