#include <rf_modul.hpp>
#include "system.h"
#include "keyboard.h"
#include "bluetooth.h"
#include "w25x.h"
#include "wavplayer.h"
#include "ir.h"
#include "statistics.h"

#ifdef __cplusplus
 extern "C" {
  #endif
#include "../FreeRTOS/include/FreeRTOS.h"
#include "../FreeRTOS/include/task.h"
#include "../FreeRTOS/include/queue.h"
#include "../FreeRTOS/include/semphr.h"
#include "ws2812b.h"


#ifdef __cplusplus
  }
#endif


extern  RGB_t leds[];
extern RfModul<Spi_1> rf_modul;

char team_color_to_symbol[4] ={'r','b','y','g'};


xQueueHandle System::xEventQueue;
unsigned long *System::UID = (unsigned long *)0x1FFFF7E8;
uint32_t System::deviceUID[3];
tsettings System::settings;
Bluetooth* System::bluetooth;
bool System::game_status;
Keyboard* System::keyboard;
WavPlayer* System::wavPlayer;
IrTransceiver* System::ir;
//OledDisplay* System::display;
Display* System::display;
ADC* System::adc;
uint16_t System::bandana_led_on_timer;
uint16_t System::safe_counter;
uint16_t System::shock_counter;
bool System::matryoshka_enable = true;
bool System::now_zomby = false;
Pin System::second_weapon_pin(PORTC,PIN12);

void System::start_new_game(void){
shock_counter=50;//stop fire
now_zomby = false;
settings.player.health = settings.player.health_after_start;
settings.gun.clips = settings.gun.clips_after_start;
settings.gun.rounds = settings.gun.rounds_placed_in_clip;
wavPlayer->play_sound_now(1);

if(!matryoshka_enable) bluetooth->PutChar('n');
	else {
			//	bluetooth->PutChar('n');
				bluetooth->PutChar(team_color_to_symbol[settings.player.team_color]);
				bluetooth->PutChar('v');
		}





taskENTER_CRITICAL();
bandana_led_on_timer = 50;
taskEXIT_CRITICAL();

//vTaskDelay(configTICK_RATE_HZ/2);
//bluetooth->PutChar('f');
taskENTER_CRITICAL();
game_status = true;
taskEXIT_CRITICAL();

if (xSemaphoreTake(display->xDisplaySemaphore,configTICK_RATE_HZ)== pdTRUE)
	{
		display->init_gui(true);
		xSemaphoreGive(display->xDisplaySemaphore);
	}

//xSemaphoreGive(display->xGameOverSemaphore);


}

/*
void System::silence_game_over(void){
	settings.player.health = 0;
	if(!matryoshka_enable) bluetooth->PutChar('n');
	else {
			bluetooth->PutChar(team_color_to_symbol[settings.player.team_color]);
			bluetooth->PutChar('v');
	}
	taskENTER_CRITICAL();
	game_status = false;
	bandana_led_on_timer = 200;
	taskEXIT_CRITICAL();
	//if (xSemaphoreTake(display->xGameOverSemaphore,configTICK_RATE_HZ)== pdTRUE)
	{
		if (xSemaphoreTake(display->xDisplaySemaphore,configTICK_RATE_HZ)== pdTRUE)
			{
				display->init_gui(false);
				xSemaphoreGive(display->xDisplaySemaphore);
			}
		//xSemaphoreGive(display->xGameOverSemaphore);
	}
}
*/

void System::game_over(bool zomby){

settings.player.health = 0;

wavPlayer->play_sound_now(20);
if (settings.player.zomby)
{
	if (zomby)
	{
		now_zomby = true;
		wavPlayer->add_sound_to_play(21);
	}
}
if(!matryoshka_enable) bluetooth->PutChar('n');
else {
		bluetooth->PutChar(team_color_to_symbol[settings.player.team_color]);
		bluetooth->PutChar('v');
}
taskENTER_CRITICAL();
game_status = false;
bandana_led_on_timer = 200;
taskEXIT_CRITICAL();
//if (xSemaphoreTake(display->xGameOverSemaphore,configTICK_RATE_HZ)== pdTRUE)
{
	if (xSemaphoreTake(display->xDisplaySemaphore,configTICK_RATE_HZ)== pdTRUE)
		{
			display->init_gui(false);
			xSemaphoreGive(display->xDisplaySemaphore);
		}
	//xSemaphoreGive(display->xGameOverSemaphore);
}
//taskENTER_CRITICAL();

//taskEXIT_CRITICAL();

//vTaskDelay(configTICK_RATE_HZ*4);
//bluetooth->PutChar('f');
//taskEXIT_CRITICAL();
//if (zomby) now_zomby = true;
}


bool System::in_game(void){

	return game_status;

}


void System::vMainLoopTask (void *pvParameters){

	xSemaphoreTake(display->xDisplaySemaphore,configTICK_RATE_HZ);
	vTaskDelay(configTICK_RATE_HZ/2);
	display->power_off();
	vTaskDelay(configTICK_RATE_HZ/2);
	display->power_on();
	vTaskDelay(configTICK_RATE_HZ/2);
	//lcd_alternative_init();
	display->init();
	check_battary();
	vTaskDelay(configTICK_RATE_HZ/2);





//	if(get_display_type(PORTB,PIN4)==OLED)
	{
		display->power_off();
		vTaskDelay(configTICK_RATE_HZ/2);
		display->power_on();
		vTaskDelay(configTICK_RATE_HZ/2);
	//lcd_alternative_init();
		display->init();
	}


	load_settings();

	if(settings.second_weapon_enable){
		second_weapon_pin.set();
		second_weapon_pin.switchToOutput();
	}

	//vTaskDelay(configTICK_RATE_HZ/2);
	////lcd_generate_additional_symbols();
/*
	display->go_to_xy(0,0);
	display->print((char*)"Firmware");
	display->go_to_xy(0,1);
	display->print((char*)"Ver. 0.1");
*/
	display->show_logo();
	vTaskDelay(configTICK_RATE_HZ*2);
	ws2812b_Init();

	display->init_gui(game_status);
	xSemaphoreGive(display->xDisplaySemaphore);
	//ws2812b_Init();




	switch(settings.player.team_color){
	case Red:
	{

		leds[0].r=255;
	   	leds[0].g=0;
	   	leds[0].b=0;
	   	while (!ws2812b_IsReady()); // wait
	   	ws2812b_SendRGB(leds, 1);
	   	wavPlayer->add_sound_to_play(46);
	}
	break;
	case Blue:
	{
	    leds[0].r=0;
	   	leds[0].g=0;
	   	leds[0].b=255;
	   	while (!ws2812b_IsReady()); // wait
	   	ws2812b_SendRGB(leds, 1);
	   	wavPlayer->add_sound_to_play(47);
	}
	break;

	case Yellow:
	{
	    leds[0].r=255;
	   	leds[0].g=255;
	   	leds[0].b=0;
	   	while (!ws2812b_IsReady()); // wait
	   	ws2812b_SendRGB(leds, 1);
	   	wavPlayer->add_sound_to_play(48);
	}
	break;
	case Green:
	{
	    leds[0].r=0;
	   	leds[0].g=255;
	   	leds[0].b=0;
	   	while (!ws2812b_IsReady()); // wait
	   	ws2812b_SendRGB(leds, 1);
	   	wavPlayer->add_sound_to_play(49);
	}
	break;


	default: break;



	}
/*
	if(!settings.autostart_game) wavPlayer->add_sound_to_play(20);
	else
	{
		silence_game_over();
	}
*/
	for (;;) {
		static volatile tevents_source_type sys_event_source_tmp;

if (xQueueReceive( xEventQueue, (void*)&sys_event_source_tmp, configTICK_RATE_HZ/*portMAX_DELAY*/)== pdTRUE)
	{
		switch (sys_event_source_tmp)
		  {
		  	  case BUTTON1:
		  	  {
		  		  on_trigger_pressed();
		  		  xQueueReset(xEventQueue);
		  		  Keyboard::buttons_events[button1]=depressed;
		  		  Keyboard::buttons_events[button2]=depressed;
		  		  Keyboard::buttons_events[button3]=depressed;
		  	  }
		  	  break;
		  	  case BUTTON2:
		  	  {
		  		on_reload_pressed();
		  		Keyboard::buttons_events[button2]=depressed;
		  	  }
		  	  break;
		  	  case BUTTON3:
		  	  {
		  		//Bluetooth::SendStr((char*)"Button_3 pressed\r\n");
		  		on_fire_mode_pressed();
		  		Keyboard::buttons_events[button3]=depressed;
		  	  }
		  	  break;
		  	  case BUTTON4:
		  	  {
		  		Bluetooth::SendStr((char*)"Button_4 pressed\r\n");

		  		Keyboard::buttons_events[button4]=depressed;
		  	  }
		  	  break;
		  	  case BLUETOOTH:
		  	  {


		  		//  if(in_game())
		  		//  {
					if (xSemaphoreTake(display->xDisplaySemaphore,configTICK_RATE_HZ)== pdTRUE)
					{
						display->update_bluetooth();
						xSemaphoreGive(display->xDisplaySemaphore);
					}
		  		 // }

		  		  /*
		  		  if (Bluetooth::connectStatus()){
		  			  static volatile int n=0;
		  			  n++;
		  		  }
		  		  else{
		  			  static volatile int m=0;
		  			  m++;

		  		  }
		  		  */
		  	  }
		  	  break;
		  	  case LED_OFF:{
		  	//	Bluetooth::PutChar('f');
		  	  }
		  	  break;
		  	  case NEW_GAME:
		  	  {
		  		//start_new_game();
		  	  }
		  	  break;
		  	  case GAME_OVER:
		  	  {
		  		 // game_over();
		  	  }
		  	  break;
		  	  case CONFIG_BLUETOOTH:
		  	  {
		  		  bluetooth->SendStr((char*)"Bluetooth connection will be broken.\r\nBue!\r\n");
		  		  vTaskDelay(configTICK_RATE_HZ/2);
		  		  if (!bluetooth->configure()) wavPlayer->play_sound_now(41);
		  		  else wavPlayer->play_sound_now(55);
		  	  }
		  	  break;
		  	  default:;

		  }//[switch]
	}//[if (xQueueReceive( xEventQueue, (void*)&sys_event_source_tmp, portMAX_DELAY)== pdTRUE);]
	else
		{
			static uint8_t prescaler_counter = 0;
			static uint8_t zomby_period_counter = 0;
			if (!now_zomby)
			{
			if((!in_game())&&(bluetooth->connectStatus())){
				if(!bandana_led_on_timer){


					if(!matryoshka_enable) bluetooth->PutChar('n');
					else {
					//	bluetooth->PutChar('n');
						bluetooth->PutChar(team_color_to_symbol[settings.player.team_color]);

					}
					taskENTER_CRITICAL();
					bandana_led_on_timer = 5;
					taskEXIT_CRITICAL();


					//vTaskDelay(configTICK_RATE_HZ/30);
					//bluetooth->PutChar('f');
					}//[if(!bandana_led_on_timer)]
				};//[if((!in_game())&&(bluetooth->connectStatus()))]
			}//[if (!now_zomby)]

if (prescaler_counter++%2==0)
	{
			if((!bluetooth->connectStatus())&&(wavPlayer->is_stop())) wavPlayer->add_sound_to_play(45);
	}//		[if (prescaler_counter%2)]

			if (xSemaphoreTake(display->xDisplaySemaphore,configTICK_RATE_HZ)== pdTRUE)
			{
				display->update_battary_voltage();
				xSemaphoreGive(display->xDisplaySemaphore);
			}
if (now_zomby/*(!in_game())&&(settings.player.zomby)*/)
	{
		static volatile uint8_t led_prescaler=0;
		bluetooth->PutChar(team_color_to_symbol[led_prescaler++%4]);
		if(zomby_period_counter++%10==0)
		{

			bluetooth->SendStr((char*)"h101000000000101011101000t");
			if (wavPlayer->is_stop()) wavPlayer->add_sound_to_play(36);

		}
	}

		}//[else]
	}

}


void System::incorrect_lic_key(void){
	volatile bool key_incorrect = true;
	display->incorrect_lic_key();
	do{
				leds[0].r=128;
				leds[0].g=128;
				leds[0].b=128;
				while (!ws2812b_IsReady()); // wait
				ws2812b_SendRGB(leds, 1);
				vTaskDelay(configTICK_RATE_HZ/2);
				leds[0].r=0;
				leds[0].g=0;
				leds[0].b=0;
				while (!ws2812b_IsReady()); // wait
				ws2812b_SendRGB(leds, 1);
				vTaskDelay(configTICK_RATE_HZ/2);

				taskENTER_CRITICAL();
				key_incorrect = !check_license();
				taskEXIT_CRITICAL();

			}while(key_incorrect);
}

void System::on_trigger_pressed(void){
	portTickType xLastWakeTime;

	if(!bluetooth->connectStatus())
	{
		wavPlayer->play_sound_now(41);
		return;
	}
	if (!in_game()){
		//	wavPlayer->play_sound_now(41);
			if(bluetooth->connectStatus()){

			}
			return;
		}
	if(shock_counter) return;

		if (!settings.gun.rounds){
			if(settings.gun.clips==0)wavPlayer->play_sound_now(9);
			else wavPlayer->play_sound_now(27);
			//if()
			return;
		}

		switch(settings.gun.fire_mode){
		case SINGLE:
		{
			wavPlayer->play_sound_now(6);
			ir->send_shot_package();
			settings.gun.rounds--;
			if (xSemaphoreTake(display->xDisplaySemaphore,configTICK_RATE_HZ)== pdTRUE)
			{
				display->update_rounds();
				xSemaphoreGive(display->xDisplaySemaphore);
			}


		}
		break;
		case SEMI_AUTO:
		{
			for (int i=3;i>0;i--)
			{
				if (!keyboard->triggerIsPressed()||(!bluetooth->connectStatus())) break;
				if(settings.gun.rounds)
					{
						xLastWakeTime = xTaskGetTickCount();
						ir->send_shot_package();
						settings.gun.rounds--;

						wavPlayer->play_sound_now(6);
						if (xSemaphoreTake(display->xDisplaySemaphore,configTICK_RATE_HZ)== pdTRUE)
						{
							display->update_rounds();
							xSemaphoreGive(display->xDisplaySemaphore);
						}
						if(i>1)
							//vTaskDelay((portTickType)(configTICK_RATE_HZ*60)/settings.gun.rate);
							vTaskDelayUntil( &xLastWakeTime,(portTickType)(configTICK_RATE_HZ*60)/settings.gun.rate);
					}
				else
				{
					wavPlayer->play_sound_now(27);
					break;
				}

			}//[for]



		}
		break;
		case AUTO:
		{

			while (keyboard->triggerIsPressed()&&(bluetooth->connectStatus()))
			{

				if((!game_status)||(shock_counter)) break;
				if(settings.gun.rounds)
					{
						xLastWakeTime = xTaskGetTickCount();
						ir->send_shot_package();
						settings.gun.rounds--;
						wavPlayer->play_sound_now(6);
					////	if (xSemaphoreTake(display->xGameOverSemaphore,configTICK_RATE_HZ/3)== pdTRUE)
						{
						if (xSemaphoreTake(display->xDisplaySemaphore,configTICK_RATE_HZ)== pdTRUE)
						{
							display->update_rounds();
							xSemaphoreGive(display->xDisplaySemaphore);
						}
						//vTaskDelay((portTickType)(configTICK_RATE_HZ*60)/settings.gun.rate);
						////xSemaphoreGive(display->xGameOverSemaphore);
						}

						vTaskDelayUntil( &xLastWakeTime,(portTickType)(configTICK_RATE_HZ*60)/settings.gun.rate);
					}
				else
				{
					wavPlayer->play_sound_now(27);
					break;
				}

			}//[for]

		}
		break;
		default:break;
		}

}


void System::on_reload_pressed(void){
	if(!bluetooth->connectStatus())
	{
		wavPlayer->play_sound_now(41);
		return;
	}
	if (!in_game())
	{
		//		wavPlayer->play_sound_now(41);
				return;
	}
	if(!bluetooth->connectStatus())
	{
		//return;
	}
	if (settings.gun.clips >0){
		settings.gun.rounds = settings.gun.rounds_placed_in_clip;
		settings.gun.clips--;
		wavPlayer->play_sound_now(12);
		vTaskDelay((portTickType)settings.gun.reload_time*3);
		wavPlayer->add_sound_to_play(13);
		display->update_rounds();
		display->update_clips();
		if(settings.gun.clips ==0)wavPlayer->add_sound_to_play(14);
	}
	else
	{
		wavPlayer->play_sound_now(27);
		return;
	}

}



void  System::on_fire_mode_pressed(void){
	if(!bluetooth->connectStatus())
	{
		wavPlayer->play_sound_now(41);
		return;
	}
	if (!in_game())
	{
			//	wavPlayer->play_sound_now(41);
				return;
	}
	switch (settings.gun.fire_mode)
			  		{
			  			case SINGLE:{

			  				settings.gun.fire_mode = (tfire_mode)SEMI_AUTO;
			  				save_settings();
			  				if (xSemaphoreTake(display->xDisplaySemaphore,configTICK_RATE_HZ)== pdTRUE)
			  				{
			  					display->update_fire_mode();
			  					xSemaphoreGive(display->xDisplaySemaphore);
			  				}
			  				wavPlayer->play_sound_now(11);
			  				vTaskDelay(configTICK_RATE_HZ/3);


			  			//	while(uxQueueMessagesWaiting(wavPlayer->xSoundsToPlayQueue));
			  				while(!wavPlayer->is_stop());

			  			}
			  			break;
			  			case 	SEMI_AUTO:{

			  				settings.gun.fire_mode = (tfire_mode)AUTO;
			  				save_settings();
			  				if (xSemaphoreTake(display->xDisplaySemaphore,configTICK_RATE_HZ)== pdTRUE)
			  				{
			  					display->update_fire_mode();
			  					xSemaphoreGive(display->xDisplaySemaphore);
			  				}
			  				wavPlayer->play_sound_now(11);
			  				vTaskDelay(configTICK_RATE_HZ/2);
			  				while(!wavPlayer->is_stop());
			  				//while(uxQueueMessagesWaiting(wavPlayer->xSoundsToPlayQueue));
			  			}
			  			break;

			  			case AUTO:{

			  				settings.gun.fire_mode = (tfire_mode)SINGLE;
			  				save_settings();
			  				if (xSemaphoreTake(display->xDisplaySemaphore,configTICK_RATE_HZ)== pdTRUE)
			  				{
			  					display->update_fire_mode();
			  					xSemaphoreGive(display->xDisplaySemaphore);
			  				}
			  				wavPlayer->play_sound_now(11);
			  				vTaskDelay(configTICK_RATE_HZ/2);
			  				while(!wavPlayer->is_stop());
			  				//while(uxQueueMessagesWaiting(wavPlayer->xSoundsToPlayQueue));
			  			}
			  			break;
			  			default: break;
			  		}




}

void System::apply_preset(uint8_t preset){
	if (xSemaphoreTake(wavPlayer->xSpiFlashSemaphore,configTICK_RATE_HZ)== pdTRUE)
	{
		SPI_Flash_Read((uint8_t*)&settings,SECTORS_FOR_ONE_SOUND*SPI_FLASH_SECTOR_SIZE*NUMBER_OF_SOUNDS,sizeof(settings));
			 xSemaphoreGive(WavPlayer::xSpiFlashSemaphore);

			//	settings.player.health = settings.player.health_after_start;
			 if (preset>4) preset = DEFAULT_ACTIVE_PRESET;

			 	settings.player.zomby = settings.presets[preset].zomby;
			 	settings.player.zomby_period = settings.presets[preset].zomby_period;
			 //	settings.player.health_after_start = DEFAULT_HEALTH_AFTER_START;
			 	settings.player.health_after_start = settings.presets[preset].health_after_start;
			 //	settings.gun.clips_after_start = DEFAULT_CLIPS_AFTER_START;
			 	settings.gun.clips_after_start = settings.presets[preset].clips_after_start;
			 //	settings.gun.rounds_placed_in_clip = DEFAULT_ROUNDS_PLACED_IN_CLIP;
			 	settings.gun.rounds_placed_in_clip = settings.presets[preset].rounds_placed_in_clip;
			 //	settings.gun.damage = DEFAULT_DAMAGE;
			 	settings.gun.damage = settings.presets[preset].damage;
			 //	settings.gun.fire_mode = DEFAULT_FIRE_MODE;
			 // settings.gun.fire_mode =  settings.presets[settings.active_preset].fire_mode;
			 //	settings.gun.reload_time = DEFAULT_GUN_RELOAD_TIME;
			 	settings.gun.reload_time = settings.presets[preset].reload_time;
			 //	settings.friendly_fire = DEFAULT_FRIENDLY_FIRE;
			 	settings.friendly_fire = settings.presets[preset].friendly_fire;
			 //	settings.player.shock_time = DEFAULT_SHOCK_TIME;
			 //	settings.gun.ir_duty_cycle = DEFAULT_IR_DUTY_CYCLE;
			 //	settings.gun.ir_duty_cycle = settings.presets[settings.active_preset].ir_duty_cycle;

			 	settings.player.health = settings.player.health_after_start;
			 	settings.gun.clips = settings.gun.clips_after_start;
			 	settings.gun.rounds = settings.gun.rounds_placed_in_clip;


			 	ir->set_player_id(settings.player.id);
				ir->set_team_color((tteam_color)settings.player.team_color);
				ir->set_gun_damage(settings.gun.damage);
	//			game_status = settings.autostart_game;
				if(settings.ir_power_mode == INDOORS) ir->low_power();
				else ir->hight_power();
				display->init_gui(game_status);

				settings.player.health = settings.player.health_after_start;
				settings.gun.clips = settings.gun.clips_after_start;
				settings.gun.rounds = settings.gun.rounds_placed_in_clip;
				if(settings.ir_power_mode == INDOORS) ir->low_power();
				else ir->hight_power();




	}
	else return;

}

/*
void System::load_settings(void){
	if (xSemaphoreTake(wavPlayer->xSpiFlashSemaphore,configTICK_RATE_HZ)== pdTRUE)
	{
		SPI_Flash_Read((uint8_t*)&settings,SECTORS_FOR_ONE_SOUND*SPI_FLASH_SECTOR_SIZE*NUMBER_OF_SOUNDS,sizeof(settings));
			 xSemaphoreGive(WavPlayer::xSpiFlashSemaphore);

			//	settings.player.health = settings.player.health_after_start;
			 if (settings.active_preset>4) settings.active_preset = DEFAULT_ACTIVE_PRESET;

			 //	settings.player.health_after_start = DEFAULT_HEALTH_AFTER_START;
			 	settings.player.health_after_start = settings.presets[settings.active_preset].health_after_start;
			 //	settings.gun.clips_after_start = DEFAULT_CLIPS_AFTER_START;
			 	settings.gun.clips_after_start = settings.presets[settings.active_preset].clips_after_start;
			 //	settings.gun.rounds_placed_in_clip = DEFAULT_ROUNDS_PLACED_IN_CLIP;
			 	settings.gun.rounds_placed_in_clip = settings.presets[settings.active_preset].rounds_placed_in_clip;
			 //	settings.gun.damage = DEFAULT_DAMAGE;
			 	settings.gun.damage = settings.presets[settings.active_preset].damage;
			 	settings.gun.fire_mode = DEFAULT_FIRE_MODE;
			 // settings.gun.fire_mode =  settings.presets[settings.active_preset].fire_mode;
			 //	settings.gun.reload_time = DEFAULT_GUN_RELOAD_TIME;
			 	settings.gun.reload_time = settings.presets[settings.active_preset].reload_time;
			 //	settings.friendly_fire = DEFAULT_FRIENDLY_FIRE;
			 	settings.friendly_fire = settings.presets[settings.active_preset].friendly_fire;
			 //	settings.player.shock_time = DEFAULT_SHOCK_TIME;
			 	settings.gun.ir_duty_cycle = DEFAULT_IR_DUTY_CYCLE;
			 //	settings.gun.ir_duty_cycle = settings.presets[settings.active_preset].ir_duty_cycle;

			 	settings.player.health = settings.player.health_after_start;
			 	settings.gun.clips = settings.gun.clips_after_start;
			 	settings.gun.rounds = settings.gun.rounds_placed_in_clip;


			 	ir->set_player_id(settings.player.id);
				ir->set_team_color((tteam_color)settings.player.team_color);
				ir->set_gun_damage(settings.gun.damage);
				game_status = settings.autostart_game;
				if(settings.ir_power_mode == INDOORS) ir->low_power();
				else ir->hight_power();






	}
	else return;

	if (settings.marker != 0x12345680){
		reset_settings();
		save_settings();

	}
}
*/

#define MARKER (0x12345688)

void System::load_settings(void){
	if (xSemaphoreTake(wavPlayer->xSpiFlashSemaphore,configTICK_RATE_HZ)== pdTRUE)
	{
		SPI_Flash_Read((uint8_t*)&settings,SECTORS_FOR_ONE_SOUND*SPI_FLASH_SECTOR_SIZE*NUMBER_OF_SOUNDS,sizeof(settings));
			 xSemaphoreGive(WavPlayer::xSpiFlashSemaphore);

				settings.player.health = settings.player.health_after_start;
				settings.gun.clips = settings.gun.clips_after_start;
				settings.gun.rounds = settings.gun.rounds_placed_in_clip;
				ir->set_player_id(settings.player.id);
				ir->set_team_color((tteam_color)settings.player.team_color);
				ir->set_gun_damage(settings.gun.damage);
				game_status = settings.autostart_game;
				if(settings.ir_power_mode == INDOORS) ir->low_power();
				else ir->hight_power();






	}
	else return;

	if (settings.marker != /*0x12345681*/MARKER){
		reset_settings();
		save_settings();

	}
}




void System::save_settings(void){
/*
	volatile uint16_t settings_size;
	settings_size = sizeof(settings);
	uint8_t* data = (uint8_t* )&settings;
*/
	if (xSemaphoreTake(wavPlayer->xSpiFlashSemaphore,configTICK_RATE_HZ)== pdTRUE){
		settings.marker = MARKER /*0x12345678*/;
		SPI_Flash_Erase_Sector(SECTORS_FOR_ONE_SOUND*NUMBER_OF_SOUNDS);
		SPI_Flash_Write_Page((uint8_t*)&settings,SECTORS_FOR_ONE_SOUND*SPI_FLASH_SECTOR_SIZE*NUMBER_OF_SOUNDS,sizeof(settings));
			 xSemaphoreGive(WavPlayer::xSpiFlashSemaphore);
			}
	else return;

}

void System::reset_settings(void){



	settings.presets[0].health_after_start = PRESET_0_DEFAULT_HEALTH_AFTER_START;
	settings.presets[0].clips_after_start = PRESET_0_DEFAULT_CLIPS_AFTER_START;
	settings.presets[0].rounds_placed_in_clip = PRESET_0_DEFAULT_ROUNDS_PLACED_IN_CLIP;
	settings.presets[0].fire_mode = PRESET_0_DEFAULT_FIRE_MODE;
	settings.presets[0].damage = PRESET_0_DEFAULT_DAMAGE;
	settings.presets[0].friendly_fire = PRESET_0_DEFAULT_FRIENDLY_FIRE;
	settings.presets[0].shock_time = PRESET_0_DEFAULT_SHOCK_TIME;
	settings.presets[0].reload_time = PRESET_0_DEFAULT_RELOAD_TIME;
	settings.presets[0].ir_duty_cycle = PRESET_0_DEFAULT_IR_DUTY_CYCLE;
	settings.presets[0].zomby = PRESET_0_DEFAULT_ZOMBI;
	settings.presets[0].zomby_period = PRESET_0_DEFAULT_ZOMBI_PERIOD;

	settings.presets[1].health_after_start = PRESET_1_DEFAULT_HEALTH_AFTER_START;
	settings.presets[1].clips_after_start = PRESET_1_DEFAULT_CLIPS_AFTER_START;
	settings.presets[1].rounds_placed_in_clip = PRESET_1_DEFAULT_ROUNDS_PLACED_IN_CLIP;
	settings.presets[1].fire_mode = PRESET_1_DEFAULT_FIRE_MODE;
	settings.presets[1].damage = PRESET_1_DEFAULT_DAMAGE;
	settings.presets[1].friendly_fire = PRESET_1_DEFAULT_FRIENDLY_FIRE;
	settings.presets[1].shock_time = PRESET_1_DEFAULT_SHOCK_TIME;
	settings.presets[1].reload_time = PRESET_1_DEFAULT_RELOAD_TIME;
	settings.presets[1].ir_duty_cycle = PRESET_1_DEFAULT_IR_DUTY_CYCLE;
	settings.presets[1].zomby = PRESET_1_DEFAULT_ZOMBI;
	settings.presets[1].zomby_period = PRESET_1_DEFAULT_ZOMBI_PERIOD;


	settings.presets[2].health_after_start = PRESET_2_DEFAULT_HEALTH_AFTER_START;
	settings.presets[2].clips_after_start = PRESET_2_DEFAULT_CLIPS_AFTER_START;
	settings.presets[2].rounds_placed_in_clip = PRESET_2_DEFAULT_ROUNDS_PLACED_IN_CLIP;
	settings.presets[2].fire_mode = PRESET_2_DEFAULT_FIRE_MODE;
	settings.presets[2].damage = PRESET_2_DEFAULT_DAMAGE;
	settings.presets[2].friendly_fire = PRESET_2_DEFAULT_FRIENDLY_FIRE;
	settings.presets[2].shock_time = PRESET_2_DEFAULT_SHOCK_TIME;
	settings.presets[2].reload_time = PRESET_2_DEFAULT_RELOAD_TIME;
	settings.presets[2].ir_duty_cycle = PRESET_2_DEFAULT_IR_DUTY_CYCLE;
	settings.presets[2].zomby = PRESET_2_DEFAULT_ZOMBI;
	settings.presets[2].zomby_period = PRESET_2_DEFAULT_ZOMBI_PERIOD;

	settings.presets[3].health_after_start = PRESET_3_DEFAULT_HEALTH_AFTER_START;
	settings.presets[3].clips_after_start = PRESET_3_DEFAULT_CLIPS_AFTER_START;
	settings.presets[3].rounds_placed_in_clip = PRESET_3_DEFAULT_ROUNDS_PLACED_IN_CLIP;
	settings.presets[3].fire_mode = PRESET_3_DEFAULT_FIRE_MODE;
	settings.presets[3].damage = PRESET_3_DEFAULT_DAMAGE;
	settings.presets[3].friendly_fire = PRESET_3_DEFAULT_FRIENDLY_FIRE;
	settings.presets[3].shock_time = PRESET_3_DEFAULT_SHOCK_TIME;
	settings.presets[3].reload_time = PRESET_3_DEFAULT_RELOAD_TIME;
	settings.presets[3].ir_duty_cycle = PRESET_3_DEFAULT_IR_DUTY_CYCLE;
	settings.presets[3].zomby = PRESET_3_DEFAULT_ZOMBI;
	settings.presets[3].zomby_period = PRESET_3_DEFAULT_ZOMBI_PERIOD;


	settings.presets[4].health_after_start = PRESET_4_DEFAULT_HEALTH_AFTER_START;
	settings.presets[4].clips_after_start = PRESET_4_DEFAULT_CLIPS_AFTER_START;
	settings.presets[4].rounds_placed_in_clip = PRESET_4_DEFAULT_ROUNDS_PLACED_IN_CLIP;
	settings.presets[4].fire_mode = PRESET_4_DEFAULT_FIRE_MODE;
	settings.presets[4].damage = PRESET_4_DEFAULT_DAMAGE;
	settings.presets[4].friendly_fire = PRESET_4_DEFAULT_FRIENDLY_FIRE;
	settings.presets[4].shock_time = PRESET_4_DEFAULT_SHOCK_TIME;
	settings.presets[4].reload_time = PRESET_4_DEFAULT_RELOAD_TIME;
	settings.presets[4].ir_duty_cycle = PRESET_4_DEFAULT_IR_DUTY_CYCLE;
	settings.presets[4].zomby = PRESET_4_DEFAULT_ZOMBI;
	settings.presets[4].zomby_period = PRESET_4_DEFAULT_ZOMBI_PERIOD;

	settings.active_preset = DEFAULT_ACTIVE_PRESET;

	settings.autostart_game = DEFAULT_AUTOSTART_GAME;
	settings.gun.rate = DEFAULT_GUN_RATE;
	settings.battary.low_voltage = DEFAULT_BATTARY_LOW_VOLTAGE;
	settings.battary.full_voltage = DEFAULT_BATTARY_FULL_VOLTAGE;
	settings.player.safe_time = DEFAULT_SAFE_TIME;
	settings.player.id = DEFAULT_PLAYER_ID;
	settings.player.team_color = DEFAULT_TEAM_COLOR;
	settings.gun.fire_mode = DEFAULT_FIRE_MODE;
	settings.gun.ir_duty_cycle = DEFAULT_IR_DUTY_CYCLE;

	settings.player.zomby = DEFAULT_ZOMBI;
	settings.player.zomby_period = DEFAULT_ZOMBI_PERIOD;
	settings.player.health_after_start = DEFAULT_HEALTH_AFTER_START;
//-//	settings.player.health_after_start = settings.presets[settings.active_preset].health_after_start;
	settings.gun.clips_after_start = DEFAULT_CLIPS_AFTER_START;
//-//	settings.gun.clips_after_start = settings.presets[settings.active_preset].clips_after_start;
	settings.gun.rounds_placed_in_clip = DEFAULT_ROUNDS_PLACED_IN_CLIP;
//-//settings.gun.rounds_placed_in_clip = settings.presets[settings.active_preset].rounds_placed_in_clip;
	settings.gun.damage = DEFAULT_DAMAGE;
//-//settings.gun.damage = settings.presets[settings.active_preset].damage;

	settings.gun.fire_mode =  DEFAULT_FIRE_MODE ;
//-//settings.gun.fire_mode = settings.presets[settings.active_preset].fire_mode;
	settings.gun.reload_time = DEFAULT_GUN_RELOAD_TIME;
//-//settings.gun.reload_time = settings.presets[settings.active_preset].reload_time;
	settings.friendly_fire = DEFAULT_FRIENDLY_FIRE;
//-//settings.friendly_fire = settings.presets[settings.active_preset].friendly_fire;
	settings.player.shock_time = DEFAULT_SHOCK_TIME;

//	settings.gun.ir_duty_cycle = settings.presets[settings.active_preset].ir_duty_cycle;


	game_status = settings.autostart_game;

	for (int i=0;i<16;i++){
		settings.bluetooth_name[i] = default_bluetooth_name[i];
	}
	settings.ir_power_mode=DEFAULT_IR_POWER_MODE;
	settings.second_weapon_enable = false;
	second_weapon_pin.reset();
}


bool System::update_statistics(trx_package& ir_package, bool is_fatal){
	uint16_t free_cell_index;
	StatItem statistics_item_tmp;
	if (xSemaphoreTake(wavPlayer->xSpiFlashSemaphore,configTICK_RATE_HZ)== pdTRUE){
		free_cell_index = get_first_free_cell();
		 xSemaphoreGive(wavPlayer->xSpiFlashSemaphore);
	}
	else return false;
	if(free_cell_index==0xffff) return false;
	statistics_item_tmp.player_id = ir_package.player_id;
	statistics_item_tmp.team_color = ir_package.team_id;
	statistics_item_tmp.damage = ir_package.damage;
	statistics_item_tmp.is_fatal = is_fatal;
	statistics_item_tmp.is_delivery = false;
	statistics_item_tmp.marker = 0x55;
	if (xSemaphoreTake(wavPlayer->xSpiFlashSemaphore,configTICK_RATE_HZ)== pdTRUE){
		 write_stat_item( statistics_item_tmp,free_cell_index );
		 xSemaphoreGive(wavPlayer->xSpiFlashSemaphore);
		 return true;
	}
	else return false;
}


void System::hitProcessing(void){
	//static volatile int i;
	trx_package ir_package;
	 Bluetooth::getPackageValue(ir_package);
	 if (!in_game()) return;
	 if((settings.player.team_color == ir_package.team_id)&&(settings.friendly_fire==false)) return;
	 if((settings.player.team_color== ir_package.team_id)&&(settings.player.id == ir_package.player_id)) return;
	 if(safe_counter==0)
	 {
		 safe_counter = settings.player.safe_time;
		 shock_counter = settings.player.shock_time;//��������
		 if (settings.player.health > ir_package.damage)
		 {
			 settings.player.health -= ir_package.damage;
			 wavPlayer->play_sound_now(17);

			 if(!matryoshka_enable)
			 {
				 bluetooth->PutChar('n');
				 bandana_led_on_timer = 100;
			 }
			 else
			 {
				 bluetooth->PutChar(team_color_to_symbol[/*ir_package.team_id*/settings.player.team_color]);
				 bluetooth->PutChar('v');
			 }
#ifdef rf_enable
			 rf_modul.send_status_package(true,ir_package);
			 update_statistics(ir_package, false);
#endif
		 }
		 else
		 {
#ifdef rf_enable
			 rf_modul.send_status_package(true,ir_package);
			 update_statistics(ir_package, true);
#endif
			 //if(matryoshka_enable) bluetooth->PutChar(team_color_to_symbol[ir_package.team_id]);
			 if (settings.player.zomby) game_over(true);
			 else game_over();
		 }

		 if (in_game()){
			 if (xSemaphoreTake(display->xDisplaySemaphore,configTICK_RATE_HZ)== pdTRUE)
			 			{
			 				display->update_health();
			 				xSemaphoreGive(display->xDisplaySemaphore);
			 			}

		 }

	 }
	 //	 i++;

}


const uint8_t presets_sounds[]={4,18,25,34,35,36};

void System::messageProcessing(void){
	//static volatile int i;
	tir_message message;
	Bluetooth::getMessageValue(message);

	switch(message.ID)//���� ��� �������
		{
		case SET_IR_DUTY_CYCLE:
		{
			if(message.param > 50) message.param = 50;
			settings.gun.ir_duty_cycle = message.param;
			save_settings();
			wavPlayer->play_sound_now(55);
			if(!matryoshka_enable)
			{
				 bluetooth->PutChar('n');
				 bandana_led_on_timer = 25;
			 }
			 else
			 {
				 bluetooth->PutChar(team_color_to_symbol[settings.player.team_color]);
			 }

			break;
		}

		case   SET_ACTIVE_PRESET:
		{
			//if (!in_game()) return;
			if (message.param > 4) return;
			//settings.active_preset = message.param;
			apply_preset(message.param);
			save_settings();
//			load_settings();
			wavPlayer->play_sound_now(55);
			wavPlayer->add_sound_to_play((unsigned char)presets_sounds[message.param]);
			 if(!matryoshka_enable)
			 {
				 bluetooth->PutChar('n');
				 bandana_led_on_timer = 25;
			 }
			 else
			 {
				 bluetooth->PutChar(team_color_to_symbol[settings.player.team_color]);
			 }
			for (int i=0; i<message.param; i++)
			{
				vTaskDelay(configTICK_RATE_HZ/2);
				 if(!matryoshka_enable)
				 {
					 bluetooth->PutChar('n');
					 bandana_led_on_timer = 25;
				 }
				 else
				 {
					 bluetooth->PutChar(team_color_to_symbol[settings.player.team_color]);
					 bluetooth->PutChar('v');
				 }
			}

			break;
		}



		case ANOMALY:
		{
			if (!in_game()) return;
			bluetooth->PutChar('n');
			 if(!matryoshka_enable)
			 {
				 bluetooth->PutChar('n');
				 bandana_led_on_timer = 100;
			 }
			 else
			 {
				 bluetooth->PutChar(team_color_to_symbol[settings.player.team_color]);
				 bluetooth->PutChar('v');
			 }


			settings.player.health+=message.param;
			if (settings.player.health > settings.player.health_after_start)
				settings.player.health = settings.player.health_after_start;
			if (xSemaphoreTake(display->xDisplaySemaphore,configTICK_RATE_HZ)== pdTRUE)
			{
				display->update_health();
				xSemaphoreGive(display->xDisplaySemaphore);
			}
			wavPlayer->play_sound_now(30);
			break;
		}
		case RADIATION:
		{
			if (!in_game()) return;
			bluetooth->PutChar('n');
			 if(!matryoshka_enable)
			 {
				 bluetooth->PutChar('n');
				 bandana_led_on_timer = 100;
			 }
			 else
			 {
				 bluetooth->PutChar(team_color_to_symbol[settings.player.team_color]);
				 bluetooth->PutChar('v');
			 }

			 if (settings.player.health > message.param)
			 {
				 settings.player.health-=message.param;
				 wavPlayer->play_sound_now(31);
			 }
			 else
			 {
			 	 game_over();
			 }

			 if (in_game()){
				 if (xSemaphoreTake(display->xDisplaySemaphore,configTICK_RATE_HZ)== pdTRUE)
				 			{
				 				display->update_health();
				 				xSemaphoreGive(display->xDisplaySemaphore);
				 			}

			 }
			break;
		}

		case Add_Health:
   		{

   			if (!in_game()) return;
   			if(!matryoshka_enable)
   			{
   				bluetooth->PutChar('n');
   				bandana_led_on_timer = 100;
   			}
   			else
   			{
   				bluetooth->PutChar(team_color_to_symbol[settings.player.team_color]);
   			}

   			if ((settings.player.health + message.param) >= settings.player.health_after_start) settings.player.health = settings.player.health_after_start;
   			else settings.player.health += message.param;
   			wavPlayer->play_sound_now(23);
			 if (in_game()){
				 if (xSemaphoreTake(display->xDisplaySemaphore,configTICK_RATE_HZ)== pdTRUE)
				 			{
				 				display->update_health();
				 				xSemaphoreGive(display->xDisplaySemaphore);
				 			}

			 }
   			break;
   		}
	    case Add_Rounds:
	    {
	    	if (!in_game()) return;
	    	bluetooth->PutChar('n');
	    				 if(!matryoshka_enable)
	    				 {
	    					 bluetooth->PutChar('n');
	    					 bandana_led_on_timer = 100;
	    				 }
	    				 else
	    				 {
	    					 bluetooth->PutChar(team_color_to_symbol[settings.player.team_color]);
	    				 }
	    	if ((settings.gun.clips+message.param/settings.gun.rounds_placed_in_clip)>=settings.gun.clips_after_start) settings.gun.clips = settings.gun.clips_after_start;
	    	else  settings.gun.clips += message.param/settings.gun.rounds_placed_in_clip;
   			wavPlayer->play_sound_now(26);
			 if (in_game()){
				 if (xSemaphoreTake(display->xDisplaySemaphore,configTICK_RATE_HZ)== pdTRUE)
				 			{
				 				display->update_clips();
				 				xSemaphoreGive(display->xDisplaySemaphore);
				 			}
			}
	    	break;
   		}
   		case Change_color:
		{
			if((message.param>=0)&&(message.param<=3))
			{

				taskENTER_CRITICAL();
				bandana_led_on_timer = 100;
				taskEXIT_CRITICAL();
				settings.player.team_color = message.param;
				ir->set_team_color((Team_Color)settings.player.team_color);
				if(!matryoshka_enable)
				{
					bluetooth->PutChar('n');
				}
				else
				{
					bluetooth->PutChar(team_color_to_symbol[settings.player.team_color]);
					bluetooth->PutChar('v');
				}
				switch(settings.player.team_color)
				{
				case Red:
				{

					leds[0].r=255;
				   	leds[0].g=0;
				   	leds[0].b=0;
				   	while (!ws2812b_IsReady()); // wait
				   	ws2812b_SendRGB(leds, 1);
				   	wavPlayer->play_sound_now(46);
				}
				break;
				case Blue:
				{
				    leds[0].r=0;
				   	leds[0].g=0;
				   	leds[0].b=255;
				   	while (!ws2812b_IsReady()); // wait
				   	ws2812b_SendRGB(leds, 1);
				   	wavPlayer->play_sound_now(47);
				}
				break;

				case Yellow:
				{
				    leds[0].r=255;
				   	leds[0].g=255;
				   	leds[0].b=0;
				   	while (!ws2812b_IsReady()); // wait
				   	ws2812b_SendRGB(leds, 1);
				   	wavPlayer->play_sound_now(48);
				}
				break;
				case Green:
				{
				    leds[0].r=0;
				   	leds[0].g=255;
				   	leds[0].b=0;
				   	while (!ws2812b_IsReady()); // wait
				   	ws2812b_SendRGB(leds, 1);
				   	wavPlayer->play_sound_now(49);
				}
				break;


				default: break;


				}

				save_settings();
			}//[if((message.param>=0)&&(message.param<=3))]

			break;
		}
   		case Command://����� �� �������������� �������
   		{
   			switch(message.param)//�������, ����� ��� �������
   			{
			case 0x05://������ ����� ���� ����������
			{

			//	 tevents_source_type sys_event_source_tmp;
			//	 sys_event_source_tmp = NEW_GAME;
			//	 xQueueSendToBack(System::xEventQueue, &sys_event_source_tmp, configTICK_RATE_HZ);
			//	taskEXIT_CRITICAL();
				start_new_game();
			//	taskENTER_CRITICAL();
				break;
			}
			case 0x06:
			{
				if (!in_game()) return;
				bluetooth->PutChar('n');
				if(!matryoshka_enable)
				{
					bluetooth->PutChar('n');
					bandana_led_on_timer = 100;
				}
				else
				{
					bluetooth->PutChar(team_color_to_symbol[settings.player.team_color]);
				}
				settings.gun.clips = settings.gun.clips_after_start;
				settings.gun.rounds = settings.gun.rounds_placed_in_clip;
				if (xSemaphoreTake(display->xDisplaySemaphore,configTICK_RATE_HZ)== pdTRUE)
				{
					display->update_clips();
					display->update_rounds();
				 	xSemaphoreGive(display->xDisplaySemaphore);
				}
				wavPlayer->play_sound_now(26);


				break;
			}
			case 0x00://"���������" ������
			{
				if (!in_game()) return;
				//taskEXIT_CRITICAL();
			//	if(in_game())//���� � ����
				//{
				// tevents_source_type sys_event_source_tmp;
			//	 sys_event_source_tmp = GAME_OVER;
			//	 xQueueSendToBack(System::xEventQueue, &sys_event_source_tmp, configTICK_RATE_HZ);
					game_over();

				//}
				//else /*if (armadaSystem.wav_player.type_of_sound_to_play!=GAME_OVER)*/
				//{
				//	wavPlayer->play_sound_now(20);
					//armadaSystem.wav_player.type_of_sound_to_play = GAME_OVER;//���� ������������� ���� "���� ���������"
					//xSemaphoreGive(xWavPlayerManagerSemaphore);
				//}


				//taskENTER_CRITICAL();


				break;
			}

			case IR_POWER_INCREMENT:
			{

				if(!matryoshka_enable)
				{
					bluetooth->PutChar('n');
					taskENTER_CRITICAL();
					bandana_led_on_timer = 100;
					taskEXIT_CRITICAL();
				}
				else
				{
					bluetooth->PutChar(team_color_to_symbol[settings.player.team_color]);
				}


				settings.ir_power_mode = OUTDOORS;
				ir->hight_power();
				save_settings();
			 	wavPlayer->play_sound_now(43);
				break;
			}//[case IR_POWER_INCREMENT]
			case IR_POWER_DECREMENT:
			{
				if(!matryoshka_enable)
				{
					bluetooth->PutChar('n');
					taskENTER_CRITICAL();
					bandana_led_on_timer = 100;
					taskEXIT_CRITICAL();
				}
				else
				{
					bluetooth->PutChar(team_color_to_symbol[settings.player.team_color]);
				}
				settings.ir_power_mode = INDOORS;
				ir->low_power();
				save_settings();
			 	wavPlayer->play_sound_now(44);
				break;
			}//[case IR_POWER_DECREMENT]

			case IR_DUTY_CYCLE_INCREMENT:{
				bluetooth->PutChar('n');
				taskENTER_CRITICAL();
				bandana_led_on_timer = 100;
				taskEXIT_CRITICAL();
				settings.gun.ir_duty_cycle = (settings.gun.ir_duty_cycle/10)*10;
				if(settings.gun.ir_duty_cycle < 50)
				{
					settings.gun.ir_duty_cycle +=10;
					wavPlayer->play_sound_now(55);
				}
				else
				{
					settings.gun.ir_duty_cycle = 50;
					wavPlayer->play_sound_now(41);
				}


			}//[case IR_DUTY_CYCLE_INCREMENT]
			break;
			case IR_DUTY_CYCLE_DECREMENT:{
				bluetooth->PutChar('n');
				taskENTER_CRITICAL();
				bandana_led_on_timer = 100;
				taskEXIT_CRITICAL();
				if(settings.gun.ir_duty_cycle == 1)
					{
						wavPlayer->play_sound_now(41);
						return;
					}

				if (settings.gun.ir_duty_cycle > 10)
				{
					settings.gun.ir_duty_cycle = (settings.gun.ir_duty_cycle/10)*10;
					settings.gun.ir_duty_cycle -=10;
				}
				else
				{
					settings.gun.ir_duty_cycle =1;


				}

				wavPlayer->play_sound_now(55);

			}//[IR_DUTY_CYCLE_DECREMENT]
			break;


   			}//[switch(message.param)]



   			break;
   		}//[case Command]


	}//[switch(cap_message.rx_message.ID)//���� ��� �������]

}


void System::check_battary(void){
if(adc->voltage()<LOW_VOLTAGE){
	wavPlayer->play_sound_now(39);
}
else{
	wavPlayer->play_sound_now(38);
}
}

tdisplay_type System::get_display_type(uint8_t detect_port, uint8_t detect_pin){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); // SWD Remap (освобождаем PA15, PB3, PB4)
	tdisplay_type result;
	Pin dt_pin(detect_port, detect_pin);
	dt_pin.switchToInput();
	if (dt_pin.isSet()) result = COLOR_ILI9163;
	else result = OLED;
	return result;
}



System::System(){

	System::xEventQueue = xQueueCreate( 4, sizeof(tevents_source_type));
	 xTaskCreate( vMainLoopTask,/* ( signed char * )*/ "MainLoopTask", configMINIMAL_STACK_SIZE*2, NULL, tskIDLE_PRIORITY + 2,
	                            ( xTaskHandle * ) NULL);

	 get_uid(deviceUID);
	 adc = new ADC();
	 keyboard = new Keyboard();
	 keyboard->init();
	 bluetooth = new Bluetooth();
	 bluetooth->statePinEnable();
	 wavPlayer = new WavPlayer();
	 wavPlayer->initTask();
	 ir = new IrTransceiver();
	 display = new OledDisplay();
//	 display = new ColorDisplay(PORTB,PIN9,PORTB,PIN8,PORTB,PIN7, PORTC, PIN10);
	/*
	switch (get_display_type(PORTB,PIN4)){

	case COLOR_ILI9163:
	{
		 display = new ColorDisplay(PORTB,PIN9,PORTB,PIN8,PORTB,PIN7, PORTC, PIN10);
	}
	break;
	case OLED:
	default:
	{
		display = new OledDisplay();
	}
	break;
	}

*/


}
