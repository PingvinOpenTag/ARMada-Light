#ifndef _KEYBOARD_H
#define _KEYBOARD_H


#include <stdint.h>
#include <stdlib.h>
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "pin.h"


 constexpr static unsigned char SHORT_DURATION = 4;

 //Определим источники ситемных событий
 enum events_source_type {
 	BUTTON1,
 	BUTTON2,
 	BUTTON3,
 	BUTTON4,
 	BLUETOOTH,
 	LED_OFF,
 	NEW_GAME,
 	GAME_OVER,
 	CONFIG_BLUETOOTH,
 	NONE
 };
 typedef enum events_source_type tevents_source_type;

 enum type_button_status {
 			button_on = 0,
 			button_off = 1
 			};
 typedef enum type_button_status TBUTTON_STATUS;


 enum type_button_event  {
 			depressed=0,
 			pressed=1
 	        };
 typedef enum type_button_event TBUTTON_EVENT;


 enum type_button {
 	button1=0,
 	button2=1,
 	button3=2,
 	button4=3
 };
 typedef enum type_button TBUTTON;

 struct pressing_duration
 	{
 		uint16_t duration      ; //длительность непрерывного
 					    //нажатия кнопки
 		uint8_t inc; //байт, разрешающий
 					    //или запрещающий отсчет
 					    //длительности нажатия кнопки
 		};
 typedef struct pressing_duration TPRESSING_DURATION;



class Keyboard  {

public:

	Keyboard()/*:trigger(PORTA,PIN0), reload(PORTA,PIN1)*/{
	//	trigger.switchToInputPU();
	//	reload.switchToInputPU();
	//	init_timer();
	}
static void init(void);

static void scanKeys(void);

static bool triggerIsPressed(void);


static bool reloadIsPressed(void);

static bool fire_modeIsPressed(void);

static TBUTTON_EVENT buttons_events[4];
static 	Pin trigger;
static 	Pin reload;
static  Pin fire_mode;
private:

static void init_timer();
static TBUTTON_STATUS get_button_status(TBUTTON button);
static TBUTTON_EVENT test_button(TBUTTON button);

static TPRESSING_DURATION buttons_pressing_duratins[4];

protected:



};



#endif
