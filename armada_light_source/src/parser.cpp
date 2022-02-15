#include <rf_modul.hpp>
#include "parser.h"
#include "bluetooth.h"
#include "w25x.h"
#include "wavplayer.h"
#include "system.h"
#include "flash.h"

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

extern	System sys;
extern   RfModul<Spi_1> rf_modul;


const unsigned char unknown_command_error[] = "ERROR:unknown command\r\n";
const unsigned char command_error[] = "ERROR\r\n";
const unsigned char parameter_empty_error[] = "ERROR:parameter is not specified\r\n";
const unsigned char parameter_out_of_range_error[] = "ERROR:parameter out of range\r\n";
const unsigned char parameter_invalid_error[]  = "ERROR:invalid parameter\r\n";
const unsigned char ok_string[] = "OK\r\n";
const unsigned char protocol[] =
		"Bullets in clip;int(0,100);bullets_in_clip?;bullets_in_clip=\r\n"
		"Clips;int(0,100);clips?;clips=\r\n"
		"Rate;int(30,800);cyclic_rate?;cyclic_rate=\r\n"
		"Damage;enum(1%,2%,4%,5%,7%,10%,15%,17%,20%,25%,30%,35%,40%,50%,75%,100%);damage_index?;damage_index=\r\n"
		"Player ID;int(0,127);player_id?;player_id=\r\n"
		"Team Color;enum(red,blue,yellow,green);team_id?;team_id=\r\n"
		"Friendly fire;enum(no,yes);friendly_fire?;friendly_fire=\r\n"
		"Health;int(0,999);life_after_start?;life_after_start=\r\n"
		"Safe_duration(in 1/50 sec.);int(0,250);safe_duration?;safe_duration=\r\n"
		"IR power;enum(indoors,outdoors);ir_power?;ir_power=\r\n"
		"IR duty cycle;int(0,50);ir_duty_cycle?;ir_duty_cycle=\r\n"
		"Shock duration (in 1/50 sec.);int(0,255);shock_time?;shock_time=\r\n"
		"Reload duration;int(0,800);reload_duration?;reload_duration=\r\n"
		"Fire mode;enum(single,semi-auto,queue);fire_mode?;fire_mode=\r\n"
		"Second weapon;enum(no,yes);second_weapon_enable?;second_weapon_enable=\r\n"
		"Autostart game;enum(no,yes);autostart_game?;autostart_game=\r\n"
		"Zomby;enum(no,yes);zomby?;zomby=\r\n"
	//	"Active preset;int(0,4);active_preset?;active_preset=\r\n"
		"Preset0 Health;int(0,999);health_p0?;health_p0=\r\n"
		"Preset0 Clips;int(0,100);clips_p0?;clips_p0=\r\n"
		"Preset0 Bullets in clip;int(0,100);rounds_in_clip_p0?;rounds_in_clip_p0=\r\n"
	//	"Preset0 Fire mode;enum(single,semi-auto,queue);fire_mode_p0?;fire_mode_p0=\r\n"
		"Preset0 Damage;enum(1%,2%,4%,5%,7%,10%,15%,17%,20%,25%,30%,35%,40%,50%,75%,100%);damage_p0?;damage_p0=\r\n"
		"Preset0 Friendly fire;enum(no,yes);friendly_fire_p0?;friendly_fire_p0=\r\n"
		"Preset0 Shock duration (in 1/50 sec.);int(0,255);shock_time_p0?;shock_time_p0=\r\n"
		"Preset0 Reload duration;int(0,800);reload_time_p0?;reload_time_p0=\r\n"
	//	"Preset0 IR duty cycle;int(0,50);duty_cycle_p0?;duty_cycle_p0=\r\n"
		"Preset0 Zomby;enum(no,yes);zomby_p0?;zomby_p0=\r\n"
		"Preset1 Health;int(0,999);health_p1?;health_p1=\r\n"
		"Preset1 Clips;int(0,100);clips_p1?;clips_p1=\r\n"
		"Preset1 Bullets in clip;int(0,100);rounds_in_clip_p1?;rounds_in_clip_p1=\r\n"
	//	"Preset1 Fire mode;enum(single,semi-auto,queue);fire_mode_p1?;fire_mode_p1=\r\n"
		"Preset1 Damage;enum(1%,2%,4%,5%,7%,10%,15%,17%,20%,25%,30%,35%,40%,50%,75%,100%);damage_p1?;damage_p1=\r\n"
		"Preset1 Friendly fire;enum(no,yes);friendly_fire_p1?;friendly_fire_p1=\r\n"
		"Preset1 Shock duration (in 1/50 sec.);int(0,255);shock_time_p1?;shock_time_p1=\r\n"
		"Preset1 Reload duration;int(0,800);reload_time_p1?;reload_time_p1=\r\n"
	//	"Preset1 IR duty cycle;int(0,50);duty_cycle_p1?;duty_cycle_p1=\r\n"
		"Preset1 Zomby;enum(no,yes);zomby_p1?;zomby_p1=\r\n"
		"Preset2 Health;int(0,999);health_p2?;health_p2=\r\n"
		"Preset2 Clips;int(0,100);clips_p2?;clips_p2=\r\n"
		"Preset2 Bullets in clip;int(0,100);rounds_in_clip_p2?;rounds_in_clip_p2=\r\n"
	//	"Preset2 Fire mode;enum(single,semi-auto,queue);fire_mode_p2?;fire_mode_p2=\r\n"
		"Preset2 Damage;enum(1%,2%,4%,5%,7%,10%,15%,17%,20%,25%,30%,35%,40%,50%,75%,100%);damage_p2?;damage_p2=\r\n"
		"Preset2 Friendly fire;enum(no,yes);friendly_fire_p2?;friendly_fire_p2=\r\n"
		"Preset2 Shock duration (in 1/50 sec.);int(0,255);shock_time_p2?;shock_time_p2=\r\n"
		"Preset2 Reload duration;int(0,800);reload_time_p2?;reload_time_p2=\r\n"
	//	"Preset2 IR duty cycle;int(0,50);duty_cycle_p2?;duty_cycle_p2=\r\n"
		"Preset2 Zomby;enum(no,yes);zomby_p2?;zomby_p2=\r\n"
		"Preset3 Health;int(0,999);health_p3?;health_p3=\r\n"
		"Preset3 Clips;int(0,100);clips_p3?;clips_p3=\r\n"
		"Preset3 Bullets in clip;int(0,100);rounds_in_clip_p3?;rounds_in_clip_p3=\r\n"
	//	"Preset3 Fire mode;enum(single,semi-auto,queue);fire_mode_p3?;fire_mode_p3=\r\n"
		"Preset3 Damage;enum(1%,2%,4%,5%,7%,10%,15%,17%,20%,25%,30%,35%,40%,50%,75%,100%);damage_p3?;damage_p3=\r\n"
		"Preset3 Friendly fire;enum(no,yes);friendly_fire_p3?;friendly_fire_p3=\r\n"
		"Preset3 Shock duration (in 1/50 sec.);int(0,255);shock_time_p3?;shock_time_p3=\r\n"
		"Preset3 Reload duration;int(0,800);reload_time_p3?;reload_time_p3=\r\n"
	//	"Preset3 IR duty cycle;int(0,50);duty_cycle_p3?;duty_cycle_p3=\r\n"
		"Preset3 Zomby;enum(no,yes);zomby_p3?;zomby_p3=\r\n"
		"Preset4 Health;int(0,999);health_p4?;health_p4=\r\n"
		"Preset4 Clips;int(0,100);clips_p4?;clips_p4=\r\n"
		"Preset4 Bullets in clip;int(0,100);rounds_in_clip_p4?;rounds_in_clip_p4=\r\n"
	//	"Preset4 Fire mode;enum(single,semi-auto,queue);fire_mode_p4?;fire_mode_p4=\r\n"
		"Preset4 Damage;enum(1%,2%,4%,5%,7%,10%,15%,17%,20%,25%,30%,35%,40%,50%,75%,100%);damage_p4?;damage_p4=\r\n"
		"Preset4 Friendly fire;enum(no,yes);friendly_fire_p4?;friendly_fire_p4=\r\n"
		"Preset4 Shock duration (in 1/50 sec.);int(0,255);shock_time_p4?;shock_time_p4=\r\n"
		"Preset4 Reload duration;int(0,800);reload_time_p4?;reload_time_p4=\r\n"
	//	"Preset4 IR duty cycle;int(0,50);duty_cycle_p4?;duty_cycle_p4=\r\n"
		"Preset4 Zomby;enum(no,yes);zomby_p4?;zomby_p4=\r\n"
		;


/*
		"Р В РЎв„ўР В РЎвЂўР В Р’В»Р В РЎвЂ�Р РЋРІР‚РЋР В Р’ВµР РЋР С“Р РЋРІР‚С™Р В Р вЂ Р В РЎвЂў Р В РЎвЂ”Р В Р’В°Р РЋРІР‚С™Р РЋР вЂљР В РЎвЂўР В Р вЂ¦Р В РЎвЂўР В Р вЂ  Р В Р вЂ  Р В РЎпїЅР В Р’В°Р В РЎвЂ“Р В Р’В°Р В Р’В·Р В РЎвЂ�Р В Р вЂ¦Р В Р’Вµ;int(0,100);bullets_in_clip?;bullets_in_clip=\r\n"
		"Р В РЎС™Р В Р’В°Р В РЎвЂ“Р В Р’В°Р В Р’В·Р В РЎвЂ�Р В Р вЂ¦Р В РЎвЂўР В Р вЂ ;int(0,100);clips?;clips=\r\n"
		"Р В Р Р‹Р В РЎвЂќР В РЎвЂўР РЋР вЂљР В РЎвЂўР РЋР С“Р РЋРІР‚С™Р РЋР вЂљР В Р’ВµР В Р’В»Р РЋР Р‰Р В Р вЂ¦Р В РЎвЂўР РЋР С“Р РЋРІР‚С™Р РЋР Р‰;int(30,800);cyclic_rate?;cyclic_rate=\r\n"
		"Р В РЎСљР В Р’В°Р В Р вЂ¦Р В РЎвЂўР РЋР С“Р В РЎвЂ�Р В РЎпїЅР РЋРІР‚в„–Р В РІвЂћвЂ“ Р РЋРЎвЂњР РЋР вЂљР В РЎвЂўР В Р вЂ¦;enum(1%,2%,4%,5%,7%,10%,15%,17%,20%,25%,30%,35%,40%,50%,75%,100%);damage_index?;damage_index=\r\n"
		"Р В РїС—Р…Р В РўвЂ�Р В Р’ВµР В Р вЂ¦Р РЋРІР‚С™Р В РЎвЂ�Р РЋРІР‚С›Р В РЎвЂ�Р В РЎвЂќР В Р’В°Р РЋРІР‚С™Р В РЎвЂўР РЋР вЂљ Р В РЎвЂ�Р В РЎвЂ“Р РЋР вЂљР В РЎвЂўР В РЎвЂќР В Р’В°;int(0,127);player_id?;player_id=\r\n"
		"Р В Р’В¦Р В Р вЂ Р В Р’ВµР РЋРІР‚С™ Р В РЎвЂќР В РЎвЂўР В РЎпїЅР В Р’В°Р В Р вЂ¦Р В РўвЂ�Р РЋРІР‚в„–;enum(Р В РЎв„ўР РЋР вЂљР В Р’В°Р РЋР С“Р В Р вЂ¦Р В Р’В°Р РЋР РЏ,Р В Р Р‹Р В РЎвЂ�Р В Р вЂ¦Р РЋР РЏР РЋР РЏ,Р В РІР‚вЂњР В Р’ВµР В Р’В»Р РЋРІР‚С™Р В Р’В°Р РЋР РЏ,Р В РІР‚вЂќР В Р’ВµР В Р’В»Р В Р’ВµР В Р вЂ¦Р В Р’В°Р РЋР РЏ);team_id?;team_id=\r\n"
		"Р В РІР‚СњР РЋР вЂљР РЋРЎвЂњР В Р’В¶Р В Р’ВµР РЋР С“Р РЋРІР‚С™Р В Р вЂ Р В Р’ВµР В Р вЂ¦Р В Р вЂ¦Р РЋРІР‚в„–Р В РІвЂћвЂ“ Р В РЎвЂўР В РЎвЂ“Р В РЎвЂўР В Р вЂ¦Р РЋР Р‰;enum(Р В РЎСљР В Р’ВµР РЋРІР‚С™,Р В РІР‚СњР В Р’В°);friendly_fire?;friendly_fire=\r\n"
		"Р В РІР‚вЂќР В РўвЂ�Р В РЎвЂўР РЋР вЂљР В РЎвЂўР В Р вЂ Р РЋР Р‰Р В Р’Вµ Р В РЎвЂ”Р В РЎвЂўР РЋР С“Р В Р’В»Р В Р’Вµ Р РЋР С“Р РЋРІР‚С™Р В Р’В°Р РЋР вЂљР РЋРІР‚С™Р В Р’В°;int(0,100);life_after_start?;life_after_start=\r\n"
		"Р В РЎСљР В Р’ВµР РЋРЎвЂњР РЋР РЏР В Р’В·Р В Р вЂ Р В РЎвЂ�Р В РЎпїЅР В РЎвЂўР РЋР С“Р РЋРІР‚С™Р РЋР Р‰ Р В РЎвЂ”Р В РЎвЂўР РЋР С“Р В Р’В»Р В Р’Вµ Р РЋР вЂљР В Р’В°Р В Р вЂ¦Р В Р’ВµР В Р вЂ¦Р В РЎвЂ�Р РЋР РЏ(Р В Р вЂ  1/50 Р РЋР С“Р В Р’ВµР В РЎвЂќ.);int(0,250);safe_duration?;safe_duration=\r\n"
		"Р В РЎС™Р В РЎвЂўР РЋРІР‚В°Р В Р вЂ¦Р В РЎвЂўР РЋР С“Р РЋРІР‚С™Р РЋР Р‰ Р В РїС—Р…Р В РЎв„ў Р В РЎвЂ�Р В Р’В·Р В Р’В»Р РЋРЎвЂњР РЋРІР‚РЋР В Р’ВµР В Р вЂ¦Р В РЎвЂ�Р РЋР РЏ;enum(Р В РІР‚СњР В Р’В»Р РЋР РЏ Р В РЎвЂ�Р В РЎвЂ“Р РЋР вЂљР РЋРІР‚в„– Р В Р вЂ  Р В РЎвЂ”Р В РЎвЂўР В РЎпїЅР В Р’ВµР РЋРІР‚В°Р В Р’ВµР В Р вЂ¦Р В РЎвЂ�Р В РЎвЂ�,Р В РІР‚СњР В Р’В»Р РЋР РЏ Р В РЎвЂ�Р В РЎвЂ“Р РЋР вЂљР РЋРІР‚в„– Р В Р вЂ¦Р В Р’В° Р РЋРЎвЂњР В Р’В»Р В РЎвЂ�Р РЋРІР‚В Р В Р’Вµ);ir_power?;ir_power=\r\n"
		"Р В РІР‚в„ўР РЋР вЂљР В Р’ВµР В РЎпїЅР РЋР РЏ Р РЋРІвЂљВ¬Р В РЎвЂўР В РЎвЂќР В Р’В° (Р В Р вЂ  1/50 Р РЋР С“Р В Р’ВµР В РЎвЂќ.);int(0,255);shock_time?;shock_time=\r\n"
		"Р В РІР‚в„ўР РЋР вЂљР В Р’ВµР В РЎпїЅР РЋР РЏ Р В РЎвЂ”Р В Р’ВµР РЋР вЂљР В Р’ВµР В Р’В·Р В Р’В°Р РЋР вЂљР РЋР РЏР В РўвЂ�Р В Р’В°;int(0,800);reload_duration?;reload_duration=\r\n";
	//	"Р В РЎСљР В Р’В°Р В РЎвЂ”Р РЋР вЂљР РЋР РЏР В Р’В¶Р В Р’ВµР В Р вЂ¦Р В РЎвЂ�Р В Р’Вµ Р В Р’В·Р В Р’В°Р РЋР вЂљР РЋР РЏР В Р’В¶. Р В Р’В±Р В Р’В°Р РЋРІР‚С™Р В Р’В°Р РЋР вЂљР В Р’ВµР В РЎвЂ� Р В РЎпїЅР В РІР‚в„ў;int(3000,45000);batt_full_voltage?;batt_full_voltage=\r\n"
	//	"Р В РЎСљР В Р’В°Р В РЎвЂ”Р РЋР вЂљР РЋР РЏР В Р’В¶Р В Р’ВµР В Р вЂ¦Р В РЎвЂ�Р В Р’Вµ Р РЋР вЂљР В Р’В°Р В Р’В·Р РЋР вЂљР РЋР РЏР В Р’В¶. Р В Р’В±Р В Р’В°Р РЋРІР‚С™Р В Р’В°Р РЋР вЂљР В Р’ВµР В РЎвЂ� Р В РЎпїЅР В РІР‚в„ў;int(2700,45000);batt_low_voltage?;batt_low_
	*/


const unsigned char parameter_0[] = "player_id";  //
const unsigned char parameter_1[] = "team_id";  //
const unsigned char parameter_2[] = "shot_sound_file_name";
const unsigned char parameter_3[] = "hit_sound_file_name";
const unsigned char parameter_4[] = "friendly_fire";
const unsigned char parameter_5[] = "damage_index";//Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦ Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦ Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦
const unsigned char parameter_6[] = "bt_name";
const unsigned char parameter_7[] = "bt_bind_adress";
const unsigned char parameter_8[] = "bt_baud_rate";
const unsigned char parameter_9[] = "bt_role";
const unsigned char parameter_10[] = "bt_cmode";
const unsigned char parameter_11[] = "bt_pswd";
const unsigned char parameter_12[] = "empty_clip_sound_file_name";
const unsigned char parameter_13[] = "reload_sound_file_name";
const unsigned char parameter_14[] = "reload_duration";
const unsigned char parameter_15[] = "cyclic_rate";
const unsigned char parameter_16[] = "battary_full_voltage";
const unsigned char parameter_17[] = "battary_low_voltage";
const unsigned char parameter_18[] = "ir_power";
const unsigned char parameter_19[] = "start_game_sound_file_name";
const unsigned char parameter_20[] = "clips";//Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦ Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦ Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦ Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦ Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦
const unsigned char parameter_21[] = "bullets_in_clip";//Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦ Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦
const unsigned char parameter_22[] = "bt_name?";//Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦ Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦ Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦ Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦ Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦ Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦
const unsigned char parameter_23[] = "backlight_level";//Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦ Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦ Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦ Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦
const unsigned char parameter_24[] = "ir_pwr_offset";//Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦ Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦ Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦ Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦
const unsigned char parameter_25[] = "life_after_start";//Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦ Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦, Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦ Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦ Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦ Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦ Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦ Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦ Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦ Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦
const unsigned char parameter_26[] = "life_after_start?";//Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦ Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦ Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦ Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦
const unsigned char parameter_27[] = "ir_duty_cycle";//Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦ Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦ Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦
const unsigned char parameter_28[] = "shock_time";//Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦ Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦ Р С—РЎвЂ”Р вЂ¦ Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦ Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦ Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦
const unsigned char parameter_29[] = "prepare_to_write_block";//Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦ Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦ Р С—РЎвЂ”Р вЂ¦ Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦ Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦ Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦
const unsigned char parameter_30[] = "erase_sector";//Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦ Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦ Р С—РЎвЂ”Р вЂ¦ Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦ Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦ Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦
const unsigned char parameter_31[] = "print_page";//Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦ Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦ Р С—РЎвЂ”Р вЂ¦ Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦ Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦ Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦
const unsigned char parameter_32[] = "sound_1_adress";//Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦ Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦ Р С—РЎвЂ”Р вЂ¦ Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦ Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦ Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦
const unsigned char parameter_33[] = "shock_time?";
const unsigned char parameter_34[] = "fire_mode";
const unsigned char parameter_35[] = "fire_mode?";
const unsigned char parameter_36[] = "ir_duty_cycle?";
const unsigned char parameter_37[] = "rf_net_address?";
const unsigned char parameter_38[] = "lic_key?";
const unsigned char parameter_39[] = "hardware_key?";
const unsigned char parameter_40[] = "lic_key";
const unsigned char parameter_41[] = "sound_5_size";
const unsigned char parameter_42[] = "sound_6_adress";
const unsigned char parameter_43[] = "sound_6_size";
const unsigned char parameter_44[] = "sound_7_adress";
const unsigned char parameter_45[] = "sound_7_size";
const unsigned char parameter_46[] = "sound_8_adress";
const unsigned char parameter_47[] = "sound_8_size";
const unsigned char parameter_48[] = "sound_9_adress";
const unsigned char parameter_49[] = "sound_9_size";
const unsigned char parameter_50[] = "sound_10_adress";
const unsigned char parameter_51[] = "sound_10_size";
const unsigned char parameter_52[] = "play_sound";
const unsigned char parameter_53[] = "reset_settings";
const unsigned char parameter_54[] = "save_settings";
const unsigned char parameter_55[] = "init_display";
const unsigned char parameter_56[] = "display_on";
const unsigned char parameter_57[] = "display_off";
const unsigned char parameter_58[] = "display_print";
const unsigned char parameter_59[] = "display_clr";
const unsigned char parameter_60[] = "OK";
const unsigned char parameter_61[] = "configure_bluetooth";
const unsigned char parameter_62[] = "protocol?";
const unsigned char parameter_63[] = "clips?";
const unsigned char parameter_64[] = "cyclic_rate?";
const unsigned char parameter_65[] = "friendly_fire?";
const unsigned char parameter_66[] = "ir_power?";
const unsigned char parameter_67[] = "player_id?";
const unsigned char parameter_68[] = "team_id?";
const unsigned char parameter_69[] = "damage_index?";//Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦ Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦ Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦
const unsigned char parameter_70[] = "bullets_in_clip?";//Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦ Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦";
const unsigned char parameter_71[] = "safe_duration";
const unsigned char parameter_72[] = "safe_duration?";
const unsigned char parameter_73[] = "reload_duration?";
const unsigned char parameter_74[] = "second_weapon_enable";
const unsigned char parameter_75[] = "second_weapon_enable?";
const unsigned char parameter_76[] = "autostart_game";
const unsigned char parameter_77[] = "autostart_game?";




const unsigned char parameter_78[] = "health_p0";
const unsigned char parameter_79[] = "health_p0?";

const unsigned char parameter_80[] = "clips_p0";
const unsigned char parameter_81[] = "clips_p0?";

const unsigned char parameter_82[] = "rounds_in_clip_p0";
const unsigned char parameter_83[] = "rounds_in_clip_p0?";


const unsigned char parameter_84[] = "fire_mode_p0";
const unsigned char parameter_85[] = "fire_mode_p0?";

const unsigned char parameter_86[] = "damage_p0";
const unsigned char parameter_87[] = "damage_p0?";

const unsigned char parameter_88[] = "friendly_fire_p0";
const unsigned char parameter_89[] = "friendly_fire_p0?";

const unsigned char parameter_90[] = "shock_time_p0";
const unsigned char parameter_91[] = "shock_time_p0?";

const unsigned char parameter_92[] = "reload_time_p0";
const unsigned char parameter_93[] = "reload_time_p0?";

const unsigned char parameter_94[] = "duty_cycle_p0";
const unsigned char parameter_95[] = "duty_cycle_p0?";

const unsigned char parameter_96[] = "zomby_p0";
const unsigned char parameter_97[] = "zomby_p0?";



const unsigned char parameter_98[] = "health_p1";
const unsigned char parameter_99[] = "health_p1?";

const unsigned char parameter_100[] = "clips_p1";
const unsigned char parameter_101[] = "clips_p1?";

const unsigned char parameter_102[] = "rounds_in_clip_p1";
const unsigned char parameter_103[] = "rounds_in_clip_p1?";

const unsigned char parameter_104[] = "fire_mode_p1";
const unsigned char parameter_105[] = "fire_mode_p1?";

const unsigned char parameter_106[] = "damage_p1";
const unsigned char parameter_107[] = "damage_p1?";

const unsigned char parameter_108[] = "friendly_fire_p1";
const unsigned char parameter_109[] = "friendly_fire_p1?";

const unsigned char parameter_110[] = "shock_time_p1";
const unsigned char parameter_111[] = "shock_time_p1?";

const unsigned char parameter_112[] = "reload_time_p1";
const unsigned char parameter_113[] = "reload_time_p1?";

const unsigned char parameter_114[] = "duty_cycle_p1";
const unsigned char parameter_115[] = "duty_cycle_p1?";

const unsigned char parameter_116[] = "zomby_p1";
const unsigned char parameter_117[] = "zomby_p1?";


const unsigned char parameter_118[] = "health_p2";
const unsigned char parameter_119[] = "health_p2?";

const unsigned char parameter_120[] = "clips_p2";
const unsigned char parameter_121[] = "clips_p2?";

const unsigned char parameter_122[] = "rounds_in_clip_p2";
const unsigned char parameter_123[] = "rounds_in_clip_p2?";

const unsigned char parameter_124[] = "fire_mode_p2";
const unsigned char parameter_125[] = "fire_mode_p2?";

const unsigned char parameter_126[] = "damage_p2";
const unsigned char parameter_127[] = "damage_p2?";

const unsigned char parameter_128[] = "friendly_fire_p2";
const unsigned char parameter_129[] = "friendly_fire_p2?";

const unsigned char parameter_130[] = "shock_time_p2";
const unsigned char parameter_131[] = "shock_time_p2?";

const unsigned char parameter_132[] = "reload_time_p2";
const unsigned char parameter_133[] = "reload_time_p2?";

const unsigned char parameter_134[] = "duty_cycle_p2";
const unsigned char parameter_135[] = "duty_cycle_p2?";

const unsigned char parameter_136[] = "zomby_p2";
const unsigned char parameter_137[] = "zomby_p2?";



const unsigned char parameter_138[] = "health_p3";
const unsigned char parameter_139[] = "health_p3?";

const unsigned char parameter_140[] = "clips_p3";
const unsigned char parameter_141[] = "clips_p3?";

const unsigned char parameter_142[] = "rounds_in_clip_p3";
const unsigned char parameter_143[] = "rounds_in_clip_p3?";

const unsigned char parameter_144[] = "fire_mode_p3";
const unsigned char parameter_145[] = "fire_mode_p3?";

const unsigned char parameter_146[] = "damage_p3";
const unsigned char parameter_147[] = "damage_p3?";

const unsigned char parameter_148[] = "friendly_fire_p3";
const unsigned char parameter_149[] = "friendly_fire_p3?";

const unsigned char parameter_150[] = "shock_time_p3";
const unsigned char parameter_151[] = "shock_time_p3?";

const unsigned char parameter_152[] = "reload_time_p3";
const unsigned char parameter_153[] = "reload_time_p3?";

const unsigned char parameter_154[] = "duty_cycle_p3";
const unsigned char parameter_155[] = "duty_cycle_p3?";

const unsigned char parameter_156[] = "zomby_p3";
const unsigned char parameter_157[] = "zomby_p3?";


const unsigned char parameter_158[] = "health_p4";
const unsigned char parameter_159[] = "health_p4?";

const unsigned char parameter_160[] = "clips_p4";
const unsigned char parameter_161[] = "clips_p4?";

const unsigned char parameter_162[] = "rounds_in_clip_p4";
const unsigned char parameter_163[] = "rounds_in_clip_p4?";

const unsigned char parameter_164[] = "fire_mode_p4";
const unsigned char parameter_165[] = "fire_mode_p4?";

const unsigned char parameter_166[] = "damage_p4";
const unsigned char parameter_167[] = "damage_p4?";

const unsigned char parameter_168[] = "friendly_fire_p4";
const unsigned char parameter_169[] = "friendly_fire_p4?";

const unsigned char parameter_170[] = "shock_time_p4";
const unsigned char parameter_171[] = "shock_time_p4?";

const unsigned char parameter_172[] = "reload_time_p4";
const unsigned char parameter_173[] = "reload_time_p4?";

const unsigned char parameter_174[] = "duty_cycle_p4";
const unsigned char parameter_175[] = "duty_cycle_p4?";

const unsigned char parameter_176[] = "zomby_p4";
const unsigned char parameter_177[] = "zomby_p4?";

const unsigned char parameter_178[] = "active_preset";
const unsigned char parameter_179[] = "active_preset?";

const unsigned char parameter_180[] = "zomby";
const unsigned char parameter_181[] = "zomby?";



const unsigned char* parametersPointers[] =
{	parameter_0,	parameter_1,	parameter_2,	parameter_3,	parameter_4,	parameter_5,	parameter_6,	parameter_7,	parameter_8,	parameter_9,
	parameter_10,	parameter_11,	parameter_12,	parameter_13,	parameter_14,	parameter_15,	parameter_16,	parameter_17,	parameter_18,	parameter_19,
	parameter_20,	parameter_21,	parameter_22,	parameter_23,	parameter_24,	parameter_25,	parameter_26,	parameter_27,	parameter_28,	parameter_29,
	parameter_30,	parameter_31,	parameter_32,	parameter_33,	parameter_34,	parameter_35,	parameter_36,	parameter_37,	parameter_38,	parameter_39,
	parameter_40,	parameter_41,	parameter_42,	parameter_43,	parameter_44,	parameter_45,	parameter_46,	parameter_47,	parameter_48,	parameter_49,
	parameter_50,	parameter_51,	parameter_52,	parameter_53,	parameter_54,	parameter_55,	parameter_56,	parameter_57,	parameter_58,	parameter_59,
	parameter_60,	parameter_61,	parameter_62,	parameter_63,	parameter_64,	parameter_65,	parameter_66,	parameter_67,	parameter_68,	parameter_69,
	parameter_70,	parameter_71,	parameter_72,	parameter_73,	parameter_74,	parameter_75,	parameter_76,	parameter_77,	parameter_78,	parameter_79,
	parameter_80,	parameter_81,	parameter_82,	parameter_83,	parameter_84,	parameter_85,	parameter_86,	parameter_87,	parameter_88,	parameter_89,
	parameter_90,	parameter_91,	parameter_92,	parameter_93,	parameter_94,	parameter_95,	parameter_96,	parameter_97,	parameter_98,	parameter_99,
	parameter_100,	parameter_101,	parameter_102,	parameter_103,	parameter_104,	parameter_105,	parameter_106,	parameter_107,	parameter_108,	parameter_109,
	parameter_110,	parameter_111,	parameter_112,	parameter_113,	parameter_114,	parameter_115,	parameter_116,	parameter_117,	parameter_118,	parameter_119,
	parameter_120,	parameter_121,	parameter_122,	parameter_123,	parameter_124,	parameter_125,	parameter_126,	parameter_127,	parameter_128,	parameter_129,
	parameter_130,	parameter_131,	parameter_132,	parameter_133,	parameter_134,	parameter_135,	parameter_136,	parameter_137,	parameter_138,	parameter_139,
	parameter_140,	parameter_141,	parameter_142,	parameter_143,	parameter_144,	parameter_145,	parameter_146,	parameter_147,	parameter_148,	parameter_149,
	parameter_150,	parameter_151,	parameter_152,	parameter_153,	parameter_154,	parameter_155,	parameter_156,	parameter_157,	parameter_158,	parameter_159,
	parameter_160,	parameter_161,	parameter_162,	parameter_163,	parameter_164,	parameter_165,	parameter_166,	parameter_167,	parameter_168,	parameter_169,
	parameter_170,	parameter_171,	parameter_172,	parameter_173,	parameter_174,	parameter_175,	parameter_176,	parameter_177,	parameter_178,	parameter_179,
	parameter_180,	parameter_181
};



extern RGB_t leds[];


void Parser::parse(std::string& str){
	//static uint8_t value8;
	//static uint16_t value16;
	//static uint32_t value32;

	if (str.empty()) return;
	switch (get_parameter_index(str)){

	case 0:
	{
		uint8_t player_id;
		get_int_argument_value(str,'=',player_id,0,127);
		sys.settings.player.id = player_id;
		sys.ir->set_player_id(player_id);
		sys.save_settings();
		Bluetooth::SendStr((char*)"OK\r\n");

	}
	break;

	case 1://team_id
	{
		uint8_t team_id;
		get_int_argument_value(str,'=',team_id,0,3);
		sys.settings.player.team_color = team_id;
		sys.ir->set_team_color((tteam_color)team_id);
		sys.save_settings();
		switch(sys.settings.player.team_color)
		{
		case Red:
		{

			leds[0].r=255;
		   	leds[0].g=0;
		   	leds[0].b=0;
		   	while (!ws2812b_IsReady()); // wait
		   	ws2812b_SendRGB(leds, 1);
		   	//wavPlayer->play_sound_now(46);
		}
		break;
		case Blue:
		{
		    leds[0].r=0;
		   	leds[0].g=0;
		   	leds[0].b=255;
		   	while (!ws2812b_IsReady()); // wait
		   	ws2812b_SendRGB(leds, 1);
		   	//wavPlayer->play_sound_now(47);
		}
		break;

		case Yellow:
		{
		    leds[0].r=255;
		   	leds[0].g=255;
		   	leds[0].b=0;
		   	while (!ws2812b_IsReady()); // wait
		   	ws2812b_SendRGB(leds, 1);
		   //	wavPlayer->play_sound_now(48);
		}
		break;
		case Green:
		{
		    leds[0].r=0;
		   	leds[0].g=255;
		   	leds[0].b=0;
		   	while (!ws2812b_IsReady()); // wait
		   	ws2812b_SendRGB(leds, 1);
		   //	wavPlayer->play_sound_now(49);
		}
		break;


		default: break;


		}
		Bluetooth::SendStr((char*)"OK\r\n");

	}
	break;


	case 4:
	{
		uint8_t friendly_fire;
		get_int_argument_value(str,'=',friendly_fire,0,1);
		sys.settings.friendly_fire = (bool)friendly_fire;
		sys.save_settings();
		Bluetooth::SendStr((char*)"OK\r\n");

	}
	break;

	case 5://damage_index
		{
			uint8_t damage_index;
			get_int_argument_value(str,'=',damage_index,0,15);
			sys.settings.gun.damage = (tgun_damage)damage_index;
			sys.ir->set_gun_damage((tgun_damage)damage_index);
			sys.save_settings();
			Bluetooth::SendStr((char*)"OK\r\n");
		}
	break;

	case 6://bt_name
		{
			unsigned int i=0,string_size=0;
			std::string bt_name_str;
			get_string_argument_value(str,'=', bt_name_str);

			if(bt_name_str.empty()){
				Bluetooth::SendStr((char*)"ERROR:EMPTY STRING\r\n");
				break;
			}
			string_size = bt_name_str.size();
			if (bt_name_str.size()>15)
			{
				string_size = 15;
			}
			for (i=0;i<string_size;i++){
					sys.settings.bluetooth_name[i] =  bt_name_str.at(i);
				}
			sys.settings.bluetooth_name[string_size]=0;
			sys.save_settings();
			Bluetooth::SendStr((char*)"OK\r\n");

		}
	break;






	case 14://reload_duration
	{
		uint16_t reload_duration;
		get_int_argument_value(str,'=',reload_duration,0,800);
		sys.settings.gun.reload_time = reload_duration;
		sys.save_settings();
		Bluetooth::SendStr((char*)"OK\r\n");
	}
	break;


	case 15:{//
		uint32_t rate;
		get_int_argument_value(str,'=',rate,30,800);
		sys.settings.gun.rate = (uint16_t) rate;
		sys.save_settings();
		Bluetooth::SendStr((char*)"OK\r\n");
	}
	break;

	case 18:
		{
			uint8_t ir_power;
			get_int_argument_value(str,'=',ir_power,0,1);
			sys.settings.ir_power_mode = (tir_power_mode)ir_power;
			if(sys.settings.ir_power_mode == INDOORS) sys.ir->low_power();
							else sys.ir->hight_power();
			sys.save_settings();
			Bluetooth::SendStr((char*)"OK\r\n");

		}
		break;
	case 20://clips
	{
		uint16_t clips;
		get_int_argument_value(str,'=',clips,0,100);
		sys.settings.gun.clips_after_start = clips;
		sys.save_settings();
		Bluetooth::SendStr((char*)"OK\r\n");
	}
	break;
	case 21:	//bullets_in_clip
	{
		uint16_t bullets_in_clip;
		get_int_argument_value(str,'=',bullets_in_clip,0,100);
		sys.settings.gun.rounds_placed_in_clip = bullets_in_clip;
		sys.save_settings();
		Bluetooth::SendStr((char*)"OK\r\n");
	}
	break;
	case 22:	//bt_name?
	{
		Bluetooth::SendStr(sys.settings.bluetooth_name);
		Bluetooth::SendStr((char*)"\r\nOK\r\n");
	}
	break;

	case 25:	//life_after_start
	{
		uint16_t life_after_start;
		get_int_argument_value(str,'=',life_after_start,0,999);
		sys.settings.player.health_after_start = life_after_start;
		sys.save_settings();
		Bluetooth::SendStr((char*)"OK\r\n");
	}
	break;

	case 26:{//life_after_start?
		char num_str[4];
		itoa(sys.settings.player.health_after_start, num_str, 10);
		Bluetooth::SendStr((char*)num_str);
		Bluetooth::SendStr((char*)"\r\nOK\r\n");
	}
	break;

	case 27:{//ir_duty_cycle
		uint8_t ir_duty_cycle;
		get_int_argument_value(str,'=',ir_duty_cycle,0,50);
		sys.settings.gun.ir_duty_cycle = ir_duty_cycle;
		sys.save_settings();
		Bluetooth::SendStr((char*)"OK\r\n");
	}
	break;

	case 28:{//shock_time
		uint8_t shock_time;
		get_int_argument_value(str,'=',shock_time,0,255);
		sys.settings.player.shock_time = shock_time;
		sys.save_settings();
		Bluetooth::SendStr((char*)"OK\r\n");
	}
	break;

	case 29:{
		uint32_t page_num;
		get_int_argument_value(str,' ',page_num,0,32768);
		if (xSemaphoreTake(WavPlayer::xSpiFlashSemaphore,configTICK_RATE_HZ)== pdTRUE){
		if (get_page_from_uart_and_write_to_spi_flash(page_num)){
			 xSemaphoreGive(WavPlayer::xSpiFlashSemaphore);
			Bluetooth::SendStr((char*)"OK\r\n");

		}
		else {
			 xSemaphoreGive(WavPlayer::xSpiFlashSemaphore);
			Bluetooth::SendStr((char*)"ERROR\r\n");
		}

		}//[if (xSemaphoreTake(WavPlayer::xSpiFlashSemaphore,configTICK_RATE_HZ)== pdTRUE)]
	}
	break;
	case 30:{
		uint32_t sector_num;
		get_int_argument_value(str,' ', sector_num,0,2048);
		if (xSemaphoreTake(WavPlayer::xSpiFlashSemaphore,configTICK_RATE_HZ)== pdTRUE){
		SPI_Flash_Erase_Sector(sector_num);
		 xSemaphoreGive(WavPlayer::xSpiFlashSemaphore);
		}
		Bluetooth::SendStr((char*)"OK\r\n");
	}
	break;
	case 31:{
		uint32_t page_num;
		get_int_argument_value(str,' ',page_num,0,32768);
		if (xSemaphoreTake(WavPlayer::xSpiFlashSemaphore,configTICK_RATE_HZ)== pdTRUE){
		print_spi_flash_page(page_num);
		 xSemaphoreGive(WavPlayer::xSpiFlashSemaphore);
		}
		Bluetooth::SendStr((char*)"OK\r\n");
	}
	break;

	case 33:{//shock_time?
		char num_str[4];
		itoa(sys.settings.player.shock_time, num_str, 10);
		Bluetooth::SendStr((char*)num_str);
		Bluetooth::SendStr((char*)"\r\nOK\r\n");
	}
	break;


	case 34:{
		uint8_t fire_mode;
		get_int_argument_value(str,'=',fire_mode,0,2);
		sys.settings.gun.fire_mode = (tfire_mode)fire_mode;
		sys.save_settings();
			if (xSemaphoreTake(sys.display->xDisplaySemaphore,configTICK_RATE_HZ)== pdTRUE)
			{
				sys.display->update_fire_mode();
				xSemaphoreGive(sys.display->xDisplaySemaphore);
			}
		Bluetooth::SendStr((char*)"OK\r\n");
	}
	break;

	case 35:{
		char num_str[4];
		itoa(sys.settings.gun.fire_mode, num_str, 10);
		Bluetooth::SendStr((char*)num_str);
		Bluetooth::SendStr((char*)"\r\nOK\r\n");
	}
	break;

	case 36:{//ir_duty_cycle?
		char num_str[4];
		itoa(sys.settings.gun.ir_duty_cycle, num_str, 10);
		Bluetooth::SendStr((char*)num_str);
		Bluetooth::SendStr((char*)"\r\nOK\r\n");

	}
	break;
	case 37:{//rf_net_address?
#ifdef rf_enable
		char num_str[4];
		itoa(rf_modul.net_address.address[0], num_str, 10);
		Bluetooth::SendStr((char*)num_str);
		Bluetooth::SendStr((char*)".");
		itoa(rf_modul.net_address.address[1], num_str, 10);
		Bluetooth::SendStr((char*)num_str);
		Bluetooth::SendStr((char*)".");
		itoa(rf_modul.net_address.address[2], num_str, 10);
		Bluetooth::SendStr((char*)num_str);
#endif
		Bluetooth::SendStr((char*)"\r\nOK\r\n");
	}
	break;
	case 38:{

		Bluetooth::SendStr((char*)"\r\nOK\r\n");
	}
	break;
	case 39:{

		Bluetooth::SendStr((char*)"\r\nOK\r\n");
	}
	break;
	case 40:{
	}
	break;
	case 41:
	case 42:
	case 43:
	case 44:
	case 45:
	case 46:
	case 47:
	case 48:
	case 49:
	case 50:
	case 51:
	{
		Bluetooth::SendStr((char*)"OK\r\n");
	}
	break;
	case 52:
	{
		uint8_t snd;
		get_int_argument_value(str,' ',snd,1,62);
		xQueueSendToBack(WavPlayer::xSoundsToPlayQueue,&snd,configTICK_RATE_HZ);
		Bluetooth::SendStr((char*)"OK\r\n");

	}
	break;
	case 53:{
		System::reset_settings();
		Bluetooth::SendStr((char*)"OK\r\n");
	}
	break;
	case 54:{
		System::save_settings();
		Bluetooth::SendStr((char*)"OK\r\n");
	}
	break;
	case 55:{
		lcd_alternative_init();
		Bluetooth::SendStr((char*)"OK\r\n");
	}
	break;
	case 56:{
		sys.display->power_on();
		Bluetooth::SendStr((char*)"OK\r\n");
	}
	break;
	case 57:{
		sys.display->power_off();
		Bluetooth::SendStr((char*)"OK\r\n");
	}
	break;
	case 58:{
		sys.display->print((char*)" ARMada ");
		Bluetooth::SendStr((char*)"OK\r\n");
	}
	break;
	case 59:{
		lcd_clear();
		lcd_set_xy(0,0);
		Bluetooth::SendStr((char*)"OK\r\n");
	}
	break;
	case 60:
		{//"OK"
		if(sys.bluetooth->in_at_mode())
		{
			xSemaphoreGive(sys.bluetooth->xOkReceivedSemaphore);

		}
	}
	break;
	case 61:{
		 tevents_source_type sys_event_source_tmp;
		 sys_event_source_tmp = CONFIG_BLUETOOTH;
		 xQueueSendToBack(System::xEventQueue, &sys_event_source_tmp, configTICK_RATE_HZ);
	//	if (sys.bluetooth->configure()) sys.bluetooth->SendStr((char*)"OK\r\n");
	//	else sys.bluetooth->SendStr((char*)"ERROR\r\n");
	}
	break;
	case 62:{
		//Bluetooth::SendStr((char*)protocol);
		Bluetooth::SlowSendStr((char*)protocol);
		Bluetooth::SendStr((char*)"OK\r\n");
	}
	break;
	case 63:{//clips?
		char num_str[4];
		itoa(sys.settings.gun.clips_after_start, num_str, 10);
		Bluetooth::SendStr((char*)num_str);
		Bluetooth::SendStr((char*)"\r\nOK\r\n");
	}
	break;
	case 64:{//cyclic_rate?
		char num_str[4];
		itoa(sys.settings.gun.rate, num_str, 10);
		Bluetooth::SendStr((char*)num_str);
		Bluetooth::SendStr((char*)"\r\nOK\r\n");
	}
	break;
	case 65:{//friendly_fire?
		char num_str[4];
		itoa(sys.settings.friendly_fire, num_str, 10);
		Bluetooth::SendStr((char*)num_str);
		Bluetooth::SendStr((char*)"\r\nOK\r\n");
	}
	break;
	case 66:{//ir_power?
		char num_str[4];
		itoa(sys.settings.ir_power_mode, num_str, 10);
		Bluetooth::SendStr((char*)num_str);
		Bluetooth::SendStr((char*)"\r\nOK\r\n");
	}
	break;
	case 67:{//player_id?
		char num_str[4];
		itoa(sys.settings.player.id, num_str, 10);
		Bluetooth::SendStr((char*)num_str);
		Bluetooth::SendStr((char*)"\r\nOK\r\n");
	}
	break;
	case 68:{//team_color?
		char num_str[4];
		itoa(sys.settings.player.team_color, num_str, 10);
		Bluetooth::SendStr((char*)num_str);
		Bluetooth::SendStr((char*)"\r\nOK\r\n");
	}
	break;
	case 69:{//damage_index?
		char num_str[4];
		itoa(sys.settings.gun.damage, num_str, 10);
		Bluetooth::SendStr((char*)num_str);
		Bluetooth::SendStr((char*)"\r\nOK\r\n");
	}
	break;
	case 70:{//clips_capacity?
		char num_str[4];
		itoa(sys.settings.gun.rounds_placed_in_clip, num_str, 10);
		Bluetooth::SendStr((char*)num_str);
		Bluetooth::SendStr((char*)"\r\nOK\r\n");
	}
	break;
	case 71:	//safe_duration
	{
		uint16_t safe_duration;
		get_int_argument_value(str,'=',safe_duration,0,250);
		sys.settings.player.safe_time = safe_duration;
		sys.save_settings();
		Bluetooth::SendStr((char*)"OK\r\n");
	}
	break;
	case 72:{//safe_duration?
		char num_str[4];
		itoa(sys.settings.player.safe_time, num_str, 10);
		Bluetooth::SendStr((char*)num_str);
		Bluetooth::SendStr((char*)"\r\nOK\r\n");
	}
	break;
	case 73:{//reload_duration??
		char num_str[4];
		itoa(sys.settings.gun.reload_time, num_str, 10);
		Bluetooth::SendStr((char*)num_str);
		Bluetooth::SendStr((char*)"\r\nOK\r\n");
	}
	break;
	case 74:{//second_weapon_enable
		uint8_t second_enable=false;
		get_int_argument_value(str,'=',second_enable,0,1);
		sys.settings.second_weapon_enable = (bool)second_enable;
		sys.save_settings();
		Bluetooth::SendStr((char*)"OK\r\n");
		if(second_enable)
		{
			sys.second_weapon_pin.set();
			sys.second_weapon_pin.switchToOutput();
		}
		else sys.second_weapon_pin.reset();
	}
	break;
	case 75:{//second_weapon_enable?
		char num_str[4];
		itoa(sys.settings.second_weapon_enable, num_str, 10);
		Bluetooth::SendStr((char*)num_str);
		Bluetooth::SendStr((char*)"\r\nOK\r\n");
	}
	break;
	case 76:{//autostart_game
		uint8_t autostart_enable=false;
		get_int_argument_value(str,'=',autostart_enable,0,1);
		sys.settings.autostart_game = (bool) autostart_enable;
		sys.save_settings();
		Bluetooth::SendStr((char*)"OK\r\n");
	}
	break;
	case 77:{//autostart_game?
		char num_str[4];
		itoa(sys.settings.autostart_game, num_str, 10);
		Bluetooth::SendStr((char*)num_str);
		Bluetooth::SendStr((char*)"\r\nOK\r\n");
	}
	break;




	case 78:{//preset0_health
		uint16_t health;
		get_int_argument_value(str,'=',health,1,999);
		sys.settings.presets[0].health_after_start = health;
		sys.save_settings();
		Bluetooth::SendStr((char*)"OK\r\n");
	}
	break;
	case 79:{//preset0_health?
		char health_str[4];
		itoa(sys.settings.presets[0].health_after_start, health_str, 10);
		Bluetooth::SendStr((char*)health_str);
		Bluetooth::SendStr((char*)"\r\nOK\r\n");
	}
	break;

	case 80:{//preset0_clips
		uint16_t clips;
		get_int_argument_value(str,'=',clips,1,999);
		sys.settings.presets[0].clips_after_start = clips;
		sys.save_settings();
		Bluetooth::SendStr((char*)"OK\r\n");
	}
	break;
	case 81:{//preset0_clips?
			char clips_str[4];
			itoa(sys.settings.presets[0].clips_after_start, clips_str, 10);
			Bluetooth::SendStr((char*)clips_str);
			Bluetooth::SendStr((char*)"\r\nOK\r\n");
	}
	break;

	case 82:{//preset0_rounds_in_clip
		uint16_t rounds_in_clip;
		get_int_argument_value(str,'=',rounds_in_clip,1,999);
		sys.settings.presets[0].rounds_placed_in_clip = rounds_in_clip;
		sys.save_settings();
		Bluetooth::SendStr((char*)"OK\r\n");
	}
	break;
	case 83:{//preset0_rounds_in_clip?
			char clips_str[4];
			itoa(sys.settings.presets[0].rounds_placed_in_clip, clips_str, 10);
			Bluetooth::SendStr((char*)clips_str);
			Bluetooth::SendStr((char*)"\r\nOK\r\n");
	}
	break;

	case 84:{//preset0_fire_mode
		uint8_t fire_mode_index;
		get_int_argument_value(str,'=',fire_mode_index,0,2);
		sys.settings.presets[0].fire_mode = (tfire_mode)fire_mode_index;
		sys.save_settings();
		Bluetooth::SendStr((char*)"OK\r\n");
	}
	break;
	case 85:{//preset0_fire_mode?
		char num_str[4];
		itoa(sys.settings.presets[0].fire_mode, num_str, 10);
		Bluetooth::SendStr((char*)num_str);
		Bluetooth::SendStr((char*)"\r\nOK\r\n");
	}
	break;

	case 86://preset0_damage
		{
		uint8_t damage_index;
		get_int_argument_value(str,'=',damage_index,0,15);
		sys.settings.presets[0].damage = (tgun_damage)damage_index;
		sys.save_settings();
		Bluetooth::SendStr((char*)"OK\r\n");
	}
	break;
	case 87:{//preset0_damage?
		char num_str[4];
		itoa(sys.settings.presets[0].damage, num_str, 10);
		Bluetooth::SendStr((char*)num_str);
		Bluetooth::SendStr((char*)"\r\nOK\r\n");
	}
	break;

	case 88://preset0_friendly_fire
	{
		uint8_t friendly_fire;
		get_int_argument_value(str,'=',friendly_fire,0,1);
		sys.settings.presets[0].friendly_fire = (bool)friendly_fire;
		sys.save_settings();
		Bluetooth::SendStr((char*)"OK\r\n");

	}
	break;
	case 89:{//preset0_friendly_fire?
		char num_str[4];
		itoa(sys.settings.presets[0].friendly_fire, num_str, 10);
		Bluetooth::SendStr((char*)num_str);
		Bluetooth::SendStr((char*)"\r\nOK\r\n");
	}
	break;

	case 90:{//preset0_shock_time
		uint8_t shock_time;
		get_int_argument_value(str,'=',shock_time,0,255);
		sys.settings.presets[0].shock_time = shock_time;
		sys.save_settings();
		Bluetooth::SendStr((char*)"OK\r\n");
	}
	break;

	case 91:{//reset0_shock_time?
		char num_str[4];
		itoa(sys.settings.presets[0].shock_time, num_str, 10);
		Bluetooth::SendStr((char*)num_str);
		Bluetooth::SendStr((char*)"\r\nOK\r\n");
	}
	break;

	case 92://preset0_reload_time
	{
		uint16_t reload_duration;
		get_int_argument_value(str,'=',reload_duration,0,800);
		sys.settings.presets[0].reload_time = reload_duration;
		sys.save_settings();
		Bluetooth::SendStr((char*)"OK\r\n");
	}
	break;

	case 93:{//preset0_reload_time?
		char num_str[4];
		itoa(sys.settings.presets[0].reload_time, num_str, 10);
		Bluetooth::SendStr((char*)num_str);
		Bluetooth::SendStr((char*)"\r\nOK\r\n");
	}
	break;

	case 94:{//preset0_duty_cycle
		uint8_t ir_duty_cycle;
		get_int_argument_value(str,'=',ir_duty_cycle,0,50);
		sys.settings.presets[0].ir_duty_cycle = ir_duty_cycle;
		sys.save_settings();
		Bluetooth::SendStr((char*)"OK\r\n");
	}
	break;
	case 95:{//preset0_duty_cycle?
		char num_str[4];
		itoa(sys.settings.presets[0].ir_duty_cycle, num_str, 10);
		Bluetooth::SendStr((char*)num_str);
		Bluetooth::SendStr((char*)"\r\nOK\r\n");

	}
	break;

	case 96://preset0_zomby
	{
		uint8_t zomby;
		get_int_argument_value(str,'=',zomby,0,1);
		sys.settings.presets[0].zomby = (bool)zomby;
		sys.save_settings();
		Bluetooth::SendStr((char*)"OK\r\n");

	}
	break;
	case 97:{//ppreset0_zomby?
		char num_str[4];
		itoa(sys.settings.presets[0].zomby, num_str, 10);
		Bluetooth::SendStr((char*)num_str);
		Bluetooth::SendStr((char*)"\r\nOK\r\n");
	}
	break;



	case 98:{//preset1_health
		uint16_t health;
		get_int_argument_value(str,'=',health,1,999);
		sys.settings.presets[1].health_after_start = health;
		sys.save_settings();
		Bluetooth::SendStr((char*)"OK\r\n");
	}
	break;
	case 99:{//preset1_health?
		char health_str[4];
		itoa(sys.settings.presets[1].health_after_start, health_str, 10);
		Bluetooth::SendStr((char*)health_str);
		Bluetooth::SendStr((char*)"\r\nOK\r\n");
	}
	break;

	case 100:{//preset1_clips
		uint16_t clips;
		get_int_argument_value(str,'=',clips,1,999);
		sys.settings.presets[1].clips_after_start = clips;
		sys.save_settings();
		Bluetooth::SendStr((char*)"OK\r\n");
	}
	break;
	case 101:{//preset1_clips?
			char clips_str[4];
			itoa(sys.settings.presets[1].clips_after_start, clips_str, 10);
			Bluetooth::SendStr((char*)clips_str);
			Bluetooth::SendStr((char*)"\r\nOK\r\n");
	}
	break;

	case 102:{//preset1_rounds_in_clip
		uint16_t rounds_in_clip;
		get_int_argument_value(str,'=',rounds_in_clip,1,999);
		sys.settings.presets[1].rounds_placed_in_clip = rounds_in_clip;
		sys.save_settings();
		Bluetooth::SendStr((char*)"OK\r\n");
	}
	break;
	case 103:{//preset1_rounds_in_clip?
			char clips_str[4];
			itoa(sys.settings.presets[1].rounds_placed_in_clip, clips_str, 10);
			Bluetooth::SendStr((char*)clips_str);
			Bluetooth::SendStr((char*)"\r\nOK\r\n");
	}
	break;

	case 104:{//preset1_fire_mode
		uint8_t fire_mode_index;
		get_int_argument_value(str,'=',fire_mode_index,0,2);
		sys.settings.presets[1].fire_mode = (tfire_mode)fire_mode_index;
		sys.save_settings();
		Bluetooth::SendStr((char*)"OK\r\n");
	}
	break;
	case 105:{//preset1_fire_mode?
		char num_str[4];
		itoa(sys.settings.presets[1].fire_mode, num_str, 10);
		Bluetooth::SendStr((char*)num_str);
		Bluetooth::SendStr((char*)"\r\nOK\r\n");
	}
	break;

	case 106://preset1_damage
		{
		uint8_t damage_index;
		get_int_argument_value(str,'=',damage_index,0,15);
		sys.settings.presets[1].damage = (tgun_damage)damage_index;
		sys.save_settings();
		Bluetooth::SendStr((char*)"OK\r\n");
	}
	break;
	case 107:{//preset1_damage?
		char num_str[4];
		itoa(sys.settings.presets[1].damage, num_str, 10);
		Bluetooth::SendStr((char*)num_str);
		Bluetooth::SendStr((char*)"\r\nOK\r\n");
	}
	break;

	case 108://preset1_friendly_fire
	{
		uint8_t friendly_fire;
		get_int_argument_value(str,'=',friendly_fire,0,1);
		sys.settings.presets[1].friendly_fire = (bool)friendly_fire;
		sys.save_settings();
		Bluetooth::SendStr((char*)"OK\r\n");

	}
	break;
	case 109:{//preset1_friendly_fire?
		char num_str[4];
		itoa(sys.settings.presets[1].friendly_fire, num_str, 10);
		Bluetooth::SendStr((char*)num_str);
		Bluetooth::SendStr((char*)"\r\nOK\r\n");
	}
	break;

	case 110:{//preset1_shock_time
		uint8_t shock_time;
		get_int_argument_value(str,'=',shock_time,0,255);
		sys.settings.presets[1].shock_time = shock_time;
		sys.save_settings();
		Bluetooth::SendStr((char*)"OK\r\n");
	}
	break;

	case 111:{//reset1_shock_time?
		char num_str[4];
		itoa(sys.settings.presets[1].shock_time, num_str, 10);
		Bluetooth::SendStr((char*)num_str);
		Bluetooth::SendStr((char*)"\r\nOK\r\n");
	}
	break;

	case 112://preset1_reload_time
	{
		uint16_t reload_duration;
		get_int_argument_value(str,'=',reload_duration,0,800);
		sys.settings.presets[1].reload_time = reload_duration;
		sys.save_settings();
		Bluetooth::SendStr((char*)"OK\r\n");
	}
	break;

	case 113:{//preset1_reload_time?
		char num_str[4];
		itoa(sys.settings.presets[1].reload_time, num_str, 10);
		Bluetooth::SendStr((char*)num_str);
		Bluetooth::SendStr((char*)"\r\nOK\r\n");
	}
	break;

	case 114:{//preset1_duty_cycle
		uint8_t ir_duty_cycle;
		get_int_argument_value(str,'=',ir_duty_cycle,0,50);
		sys.settings.presets[1].ir_duty_cycle = ir_duty_cycle;
		sys.save_settings();
		Bluetooth::SendStr((char*)"OK\r\n");
	}
	break;
	case 115:{//preset1_duty_cycle?
		char num_str[4];
		itoa(sys.settings.presets[1].ir_duty_cycle, num_str, 10);
		Bluetooth::SendStr((char*)num_str);
		Bluetooth::SendStr((char*)"\r\nOK\r\n");

	}
	break;

	case 116://preset1_zomby
	{
		uint8_t zomby;
		get_int_argument_value(str,'=',zomby,0,1);
		sys.settings.presets[1].zomby = (bool)zomby;
		sys.save_settings();
		Bluetooth::SendStr((char*)"OK\r\n");

	}
	break;
	case 117:{//ppreset1_zomby?
		char num_str[4];
		itoa(sys.settings.presets[1].zomby, num_str, 10);
		Bluetooth::SendStr((char*)num_str);
		Bluetooth::SendStr((char*)"\r\nOK\r\n");
	}
	break;


	case 118:{//preset2_health
		uint16_t health;
		get_int_argument_value(str,'=',health,1,999);
		sys.settings.presets[2].health_after_start = health;
		sys.save_settings();
		Bluetooth::SendStr((char*)"OK\r\n");
	}
	break;
	case 119:{//preset2_health?
		char health_str[4];
		itoa(sys.settings.presets[2].health_after_start, health_str, 10);
		Bluetooth::SendStr((char*)health_str);
		Bluetooth::SendStr((char*)"\r\nOK\r\n");
	}
	break;
	case 120:{//preset2_clips
		uint16_t clips;
		get_int_argument_value(str,'=',clips,1,999);
		sys.settings.presets[2].clips_after_start = clips;
		sys.save_settings();
		Bluetooth::SendStr((char*)"OK\r\n");
	}
	break;
	case 121:{//preset2_clips?
			char clips_str[4];
			itoa(sys.settings.presets[2].clips_after_start, clips_str, 10);
			Bluetooth::SendStr((char*)clips_str);
			Bluetooth::SendStr((char*)"\r\nOK\r\n");
	}
	break;
	case 122:{//preset2_rounds_in_clip
		uint16_t rounds_in_clip;
		get_int_argument_value(str,'=',rounds_in_clip,1,999);
		sys.settings.presets[2].rounds_placed_in_clip = rounds_in_clip;
		sys.save_settings();
		Bluetooth::SendStr((char*)"OK\r\n");
	}
	break;
	case 123:{//preset2_rounds_in_clip?
			char clips_str[4];
			itoa(sys.settings.presets[2].rounds_placed_in_clip, clips_str, 10);
			Bluetooth::SendStr((char*)clips_str);
			Bluetooth::SendStr((char*)"\r\nOK\r\n");
	}
	break;
	case 124:{//preset2_fire_mode
		uint8_t fire_mode_index;
		get_int_argument_value(str,'=',fire_mode_index,0,2);
		sys.settings.presets[2].fire_mode = (tfire_mode)fire_mode_index;
		sys.save_settings();
		Bluetooth::SendStr((char*)"OK\r\n");
	}
	break;
	case 125:{//preset2_fire_mode?
		char num_str[4];
		itoa(sys.settings.presets[2].fire_mode, num_str, 10);
		Bluetooth::SendStr((char*)num_str);
		Bluetooth::SendStr((char*)"\r\nOK\r\n");
	}
	break;
	case 126://preset2_damage
		{
		uint8_t damage_index;
		get_int_argument_value(str,'=',damage_index,0,15);
		sys.settings.presets[2].damage = (tgun_damage)damage_index;
		sys.save_settings();
		Bluetooth::SendStr((char*)"OK\r\n");
	}
	break;
	case 127:{//preset2_damage?
		char num_str[4];
		itoa(sys.settings.presets[2].damage, num_str, 10);
		Bluetooth::SendStr((char*)num_str);
		Bluetooth::SendStr((char*)"\r\nOK\r\n");
	}
	break;
	case 128://preset2_friendly_fire
	{
		uint8_t friendly_fire;
		get_int_argument_value(str,'=',friendly_fire,0,1);
		sys.settings.presets[2].friendly_fire = (bool)friendly_fire;
		sys.save_settings();
		Bluetooth::SendStr((char*)"OK\r\n");

	}
	break;
	case 129:{//preset2_friendly_fire?
		char num_str[4];
		itoa(sys.settings.presets[2].friendly_fire, num_str, 10);
		Bluetooth::SendStr((char*)num_str);
		Bluetooth::SendStr((char*)"\r\nOK\r\n");
	}
	break;

	case 130:{//preset2_shock_time
		uint8_t shock_time;
		get_int_argument_value(str,'=',shock_time,0,255);
		sys.settings.presets[2].shock_time = shock_time;
		sys.save_settings();
		Bluetooth::SendStr((char*)"OK\r\n");
	}
	break;

	case 131:{//reset2_shock_time?
		char num_str[4];
		itoa(sys.settings.presets[2].shock_time, num_str, 10);
		Bluetooth::SendStr((char*)num_str);
		Bluetooth::SendStr((char*)"\r\nOK\r\n");
	}
	break;

	case 132://preset2_reload_time
	{
		uint16_t reload_duration;
		get_int_argument_value(str,'=',reload_duration,0,800);
		sys.settings.presets[2].reload_time = reload_duration;
		sys.save_settings();
		Bluetooth::SendStr((char*)"OK\r\n");
	}
	break;

	case 133:{//preset2_reload_time?
		char num_str[4];
		itoa(sys.settings.presets[2].reload_time, num_str, 10);
		Bluetooth::SendStr((char*)num_str);
		Bluetooth::SendStr((char*)"\r\nOK\r\n");
	}
	break;

	case 134:{//preset2_duty_cycle
		uint8_t ir_duty_cycle;
		get_int_argument_value(str,'=',ir_duty_cycle,0,50);
		sys.settings.presets[2].ir_duty_cycle = ir_duty_cycle;
		sys.save_settings();
		Bluetooth::SendStr((char*)"OK\r\n");
	}
	break;
	case 135:{//preset2_duty_cycle?
		char num_str[4];
		itoa(sys.settings.presets[2].ir_duty_cycle, num_str, 10);
		Bluetooth::SendStr((char*)num_str);
		Bluetooth::SendStr((char*)"\r\nOK\r\n");

	}
	break;

	case 136://preset2_zomby
	{
		uint8_t zomby;
		get_int_argument_value(str,'=',zomby,0,1);
		sys.settings.presets[2].zomby = (bool)zomby;
		sys.save_settings();
		Bluetooth::SendStr((char*)"OK\r\n");

	}
	break;
	case 137:{//ppreset2_zomby?
		char num_str[4];
		itoa(sys.settings.presets[2].zomby, num_str, 10);
		Bluetooth::SendStr((char*)num_str);
		Bluetooth::SendStr((char*)"\r\nOK\r\n");
	}
	break;



	case 138:{//preset3_health
		uint16_t health;
		get_int_argument_value(str,'=',health,1,999);
		sys.settings.presets[3].health_after_start = health;
		sys.save_settings();
		Bluetooth::SendStr((char*)"OK\r\n");
	}
	break;
	case 139:{//preset3_health?
		char health_str[4];
		itoa(sys.settings.presets[3].health_after_start, health_str, 10);
		Bluetooth::SendStr((char*)health_str);
		Bluetooth::SendStr((char*)"\r\nOK\r\n");
	}
	break;
	case 140:{//preset3_clips
		uint16_t clips;
		get_int_argument_value(str,'=',clips,1,999);
		sys.settings.presets[3].clips_after_start = clips;
		sys.save_settings();
		Bluetooth::SendStr((char*)"OK\r\n");
	}
	break;
	case 141:{//preset3_clips?
			char clips_str[4];
			itoa(sys.settings.presets[3].clips_after_start, clips_str, 10);
			Bluetooth::SendStr((char*)clips_str);
			Bluetooth::SendStr((char*)"\r\nOK\r\n");
	}
	break;
	case 142:{//preset3_rounds_in_clip
		uint16_t rounds_in_clip;
		get_int_argument_value(str,'=',rounds_in_clip,1,999);
		sys.settings.presets[3].rounds_placed_in_clip = rounds_in_clip;
		sys.save_settings();
		Bluetooth::SendStr((char*)"OK\r\n");
	}
	break;
	case 143:{//preset3_rounds_in_clip?
			char clips_str[4];
			itoa(sys.settings.presets[3].rounds_placed_in_clip, clips_str, 10);
			Bluetooth::SendStr((char*)clips_str);
			Bluetooth::SendStr((char*)"\r\nOK\r\n");
	}
	break;
	case 144:{//preset3_fire_mode
		uint8_t fire_mode_index;
		get_int_argument_value(str,'=',fire_mode_index,0,2);
		sys.settings.presets[3].fire_mode = (tfire_mode)fire_mode_index;
		sys.save_settings();
		Bluetooth::SendStr((char*)"OK\r\n");
	}
	break;
	case 145:{//preset3_fire_mode?
		char num_str[4];
		itoa(sys.settings.presets[3].fire_mode, num_str, 10);
		Bluetooth::SendStr((char*)num_str);
		Bluetooth::SendStr((char*)"\r\nOK\r\n");
	}
	break;
	case 146://preset3_damage
		{
		uint8_t damage_index;
		get_int_argument_value(str,'=',damage_index,0,15);
		sys.settings.presets[3].damage = (tgun_damage)damage_index;
		sys.save_settings();
		Bluetooth::SendStr((char*)"OK\r\n");
	}
	break;
	case 147:{//preset3_damage?
		char num_str[4];
		itoa(sys.settings.presets[3].damage, num_str, 10);
		Bluetooth::SendStr((char*)num_str);
		Bluetooth::SendStr((char*)"\r\nOK\r\n");
	}
	break;
	case 148://preset3_friendly_fire
	{
		uint8_t friendly_fire;
		get_int_argument_value(str,'=',friendly_fire,0,1);
		sys.settings.presets[3].friendly_fire = (bool)friendly_fire;
		sys.save_settings();
		Bluetooth::SendStr((char*)"OK\r\n");

	}
	break;
	case 149:{//preset3_friendly_fire?
		char num_str[4];
		itoa(sys.settings.presets[3].friendly_fire, num_str, 10);
		Bluetooth::SendStr((char*)num_str);
		Bluetooth::SendStr((char*)"\r\nOK\r\n");
	}
	break;

	case 150:{//preset3_shock_time
		uint8_t shock_time;
		get_int_argument_value(str,'=',shock_time,0,255);
		sys.settings.presets[3].shock_time = shock_time;
		sys.save_settings();
		Bluetooth::SendStr((char*)"OK\r\n");
	}
	break;

	case 151:{//reset3_shock_time?
		char num_str[4];
		itoa(sys.settings.presets[3].shock_time, num_str, 10);
		Bluetooth::SendStr((char*)num_str);
		Bluetooth::SendStr((char*)"\r\nOK\r\n");
	}
	break;

	case 152://preset3_reload_time
	{
		uint16_t reload_duration;
		get_int_argument_value(str,'=',reload_duration,0,800);
		sys.settings.presets[3].reload_time = reload_duration;
		sys.save_settings();
		Bluetooth::SendStr((char*)"OK\r\n");
	}
	break;

	case 153:{//preset3_reload_time?
		char num_str[4];
		itoa(sys.settings.presets[3].reload_time, num_str, 10);
		Bluetooth::SendStr((char*)num_str);
		Bluetooth::SendStr((char*)"\r\nOK\r\n");
	}
	break;

	case 154:{//preset3_duty_cycle
		uint8_t ir_duty_cycle;
		get_int_argument_value(str,'=',ir_duty_cycle,0,50);
		sys.settings.presets[3].ir_duty_cycle = ir_duty_cycle;
		sys.save_settings();
		Bluetooth::SendStr((char*)"OK\r\n");
	}
	break;
	case 155:{//preset3_duty_cycle?
		char num_str[4];
		itoa(sys.settings.presets[3].ir_duty_cycle, num_str, 10);
		Bluetooth::SendStr((char*)num_str);
		Bluetooth::SendStr((char*)"\r\nOK\r\n");

	}
	break;

	case 156://preset3_zomby
	{
		uint8_t zomby;
		get_int_argument_value(str,'=',zomby,0,1);
		sys.settings.presets[3].zomby = (bool)zomby;
		sys.save_settings();
		Bluetooth::SendStr((char*)"OK\r\n");

	}
	break;
	case 157:{//preset3_zomby?
		char num_str[4];
		itoa(sys.settings.presets[3].zomby, num_str, 10);
		Bluetooth::SendStr((char*)num_str);
		Bluetooth::SendStr((char*)"\r\nOK\r\n");
	}
	break;



	case 158:{//preset4_health
		uint16_t health;
		get_int_argument_value(str,'=',health,1,999);
		sys.settings.presets[4].health_after_start = health;
		sys.save_settings();
		Bluetooth::SendStr((char*)"OK\r\n");
	}
	break;
	case 159:{//preset4_health?
		char health_str[4];
		itoa(sys.settings.presets[4].health_after_start, health_str, 10);
		Bluetooth::SendStr((char*)health_str);
		Bluetooth::SendStr((char*)"\r\nOK\r\n");
	}
	break;
	case 160:{//preset4_clips
		uint16_t clips;
		get_int_argument_value(str,'=',clips,1,999);
		sys.settings.presets[4].clips_after_start = clips;
		sys.save_settings();
		Bluetooth::SendStr((char*)"OK\r\n");
	}
	break;
	case 161:{//preset4_clips?
			char clips_str[4];
			itoa(sys.settings.presets[4].clips_after_start, clips_str, 10);
			Bluetooth::SendStr((char*)clips_str);
			Bluetooth::SendStr((char*)"\r\nOK\r\n");
	}
	break;
	case 162:{//preset4_rounds_in_clip
		uint16_t rounds_in_clip;
		get_int_argument_value(str,'=',rounds_in_clip,1,999);
		sys.settings.presets[4].rounds_placed_in_clip = rounds_in_clip;
		sys.save_settings();
		Bluetooth::SendStr((char*)"OK\r\n");
	}
	break;
	case 163:{//preset4_rounds_in_clip?
			char clips_str[4];
			itoa(sys.settings.presets[4].rounds_placed_in_clip, clips_str, 10);
			Bluetooth::SendStr((char*)clips_str);
			Bluetooth::SendStr((char*)"\r\nOK\r\n");
	}
	break;
	case 164:{//preset4_fire_mode
		uint8_t fire_mode_index;
		get_int_argument_value(str,'=',fire_mode_index,0,2);
		sys.settings.presets[4].fire_mode = (tfire_mode)fire_mode_index;
		sys.save_settings();
		Bluetooth::SendStr((char*)"OK\r\n");
	}
	break;
	case 165:{//preset4_fire_mode?
		char num_str[4];
		itoa(sys.settings.presets[4].fire_mode, num_str, 10);
		Bluetooth::SendStr((char*)num_str);
		Bluetooth::SendStr((char*)"\r\nOK\r\n");
	}
	break;
	case 166://preset4_damage
		{
		uint8_t damage_index;
		get_int_argument_value(str,'=',damage_index,0,15);
		sys.settings.presets[4].damage = (tgun_damage)damage_index;
		sys.save_settings();
		Bluetooth::SendStr((char*)"OK\r\n");
	}
	break;
	case 167:{//preset4_damage?
		char num_str[4];
		itoa(sys.settings.presets[4].damage, num_str, 10);
		Bluetooth::SendStr((char*)num_str);
		Bluetooth::SendStr((char*)"\r\nOK\r\n");
	}
	break;
	case 168://preset4_friendly_fire
	{
		uint8_t friendly_fire;
		get_int_argument_value(str,'=',friendly_fire,0,1);
		sys.settings.presets[4].friendly_fire = (bool)friendly_fire;
		sys.save_settings();
		Bluetooth::SendStr((char*)"OK\r\n");

	}
	break;
	case 169:{//preset4_friendly_fire?
		char num_str[4];
		itoa(sys.settings.presets[4].friendly_fire, num_str, 10);
		Bluetooth::SendStr((char*)num_str);
		Bluetooth::SendStr((char*)"\r\nOK\r\n");
	}
	break;

	case 170:{//preset4_shock_time
		uint8_t shock_time;
		get_int_argument_value(str,'=',shock_time,0,255);
		sys.settings.presets[4].shock_time = shock_time;
		sys.save_settings();
		Bluetooth::SendStr((char*)"OK\r\n");
	}
	break;

	case 171:{//reset4_shock_time?
		char num_str[4];
		itoa(sys.settings.presets[4].shock_time, num_str, 10);
		Bluetooth::SendStr((char*)num_str);
		Bluetooth::SendStr((char*)"\r\nOK\r\n");
	}
	break;

	case 172://preset4_reload_time
	{
		uint16_t reload_duration;
		get_int_argument_value(str,'=',reload_duration,0,800);
		sys.settings.presets[4].reload_time = reload_duration;
		sys.save_settings();
		Bluetooth::SendStr((char*)"OK\r\n");
	}
	break;

	case 173:{//preset4_reload_time?
		char num_str[4];
		itoa(sys.settings.presets[4].reload_time, num_str, 10);
		Bluetooth::SendStr((char*)num_str);
		Bluetooth::SendStr((char*)"\r\nOK\r\n");
	}
	break;

	case 174:{//preset4_duty_cycle
		uint8_t ir_duty_cycle;
		get_int_argument_value(str,'=',ir_duty_cycle,0,50);
		sys.settings.presets[4].ir_duty_cycle = ir_duty_cycle;
		sys.save_settings();
		Bluetooth::SendStr((char*)"OK\r\n");
	}
	break;
	case 175:{//preset4_duty_cycle?
		char num_str[4];
		itoa(sys.settings.presets[4].ir_duty_cycle, num_str, 10);
		Bluetooth::SendStr((char*)num_str);
		Bluetooth::SendStr((char*)"\r\nOK\r\n");

	}
	break;

	case 176://preset4_zomby
	{
		uint8_t zomby;
		get_int_argument_value(str,'=',zomby,0,1);
		sys.settings.presets[4].zomby = (bool)zomby;
		sys.save_settings();
		Bluetooth::SendStr((char*)"OK\r\n");

	}
	break;
	case 177:{//preset4_zomby?
		char num_str[4];
		itoa(sys.settings.presets[4].zomby, num_str, 10);
		Bluetooth::SendStr((char*)num_str);
		Bluetooth::SendStr((char*)"\r\nOK\r\n");
	}
	break;
	case 178://active_preset
			{
			uint8_t preset_index;
			get_int_argument_value(str,'=',preset_index,0,4);
			sys.settings.active_preset = preset_index;
			sys.save_settings();
			Bluetooth::SendStr((char*)"OK\r\n");
		}
	break;
	case 179:{//active_preset?
			char num_str[4];
			itoa(sys.settings.active_preset, num_str, 10);
			Bluetooth::SendStr((char*)num_str);
			Bluetooth::SendStr((char*)"\r\nOK\r\n");
		}
	break;
	case 180://preset4_zomby
	{
		uint8_t zomby;
		get_int_argument_value(str,'=',zomby,0,1);
		sys.settings.player.zomby = (bool)zomby;
		sys.save_settings();
		Bluetooth::SendStr((char*)"OK\r\n");

	}
	break;
	case 181:{//preset4_zomby?
		char num_str[4];
		itoa(sys.settings.player.zomby, num_str, 10);
		Bluetooth::SendStr((char*)num_str);
		Bluetooth::SendStr((char*)"\r\nOK\r\n");
	}
	break;
	case 0xFF:{
		if (!sys.bluetooth->in_at_mode()) Bluetooth::SendStr((char*)"ERROR:UNKNOWN COMMAND\r\n");
	}
	break;

	}//[switch (get_parameter_index(str))]



	/*
	Bluetooth::SendStr(const_cast<char*>(str.c_str()));
	Bluetooth::PutChar('\n');
	trim(str);
	Bluetooth::SendStr(const_cast<char*>(str.c_str()));
*/
};


uint8_t Parser::get_parameter_index(std::string& str){//Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦, Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦ Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦ Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦
		//uint8_t param_len;
		//uint8_t record_len;
		std::string s_tmp;
		//Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦ Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦ Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦ Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦ Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦
		trim(str,"\n");
		for (unsigned int index=0; index <(sizeof(parametersPointers)/sizeof(char*)); index++)
			{
				s_tmp.clear();
				s_tmp = reinterpret_cast <const char*>(parametersPointers[index]);
				std::string::size_type found = str.find(s_tmp + "\r");
				if (found == std::string::npos) found = str.find(s_tmp + " \r");
				if (found!=std::string::npos){
				if (found>0) continue;//return 0xFF;


			//	Bluetooth::SendStr(const_cast<char*>(s_tmp.c_str()));
			//	Bluetooth::SendStr("\r\n");
				//s_tmp = std::(index);
				char num_str[3];
				itoa(index, num_str, 10);
				//Bluetooth::SendStr(const_cast<char*>(s_tmp.c_str()));
			//	Bluetooth::SendStr(num_str);
			//	Bluetooth::SendStr("\r\n");
				return index;
				}

		//		param_len = strlen((const char*)parametersPointers[index]);
			//	record_len = strlen((const char*)record);
		//		if (memmem(record,record_len,parametersPointers[index],param_len)!='\0'/*NULL*/)
		//		{
			//		return index;
				//}
			}

		// Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦ Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦ Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦ Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦ Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦, Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦ Р С—РЎвЂ”Р вЂ¦ Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦
		trim(str);

							//s= (const char *)rx_buffer;


	for (unsigned int index=0; index <(sizeof(parametersPointers)/sizeof(char*)); index++)
	{
		s_tmp.clear();
		s_tmp = reinterpret_cast <const char*>(parametersPointers[index]);
		std::string::size_type found = str.find(s_tmp + "=");
		if (found == std::string::npos) found = str.find(s_tmp + " ");
		if (found!=std::string::npos){
		if (found>0) continue;//return 0xFF;


	//	Bluetooth::SendStr(const_cast<char*>(s_tmp.c_str()));
	//	Bluetooth::SendStr("\r\n");
		//s_tmp = std::(index);
		char num_str[3];
		itoa(index, num_str, 10);
		//Bluetooth::SendStr(const_cast<char*>(s_tmp.c_str()));
	//	Bluetooth::SendStr(num_str);
	//	Bluetooth::SendStr("\r\n");
		return index;
		}

//		param_len = strlen((const char*)parametersPointers[index]);
	//	record_len = strlen((const char*)record);
//		if (memmem(record,record_len,parametersPointers[index],param_len)!='\0'/*NULL*/)
//		{
	//		return index;
		//}
	}
	return 0xff;

}


void Parser::get_int_argument_value(std::string& str, const char separator, uint8_t& var, uint8_t min_val, uint8_t max_val){

	std::string::size_type found = str.find(separator);
		if (found!=std::string::npos){
			 std::string num_str = str.substr(found+1);
		//	 Bluetooth::SendStr(const_cast<char*>(num_str.c_str()));
		//	 Bluetooth::SendStr("\r\n");
			 var = atoi(num_str.c_str());
			 if (var < min_val) var = min_val;
			 if (var > max_val) var = max_val;
		}
			else //Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦ Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦ Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦
			{	 var = min_val;
				return;//Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦ Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦
			}
}


void Parser::get_int_argument_value(std::string& str, const char separator, uint16_t& var, uint16_t min_val, uint16_t max_val){
	std::string::size_type found = str.find(separator);
		if (found!=std::string::npos){
			 std::string num_str = str.substr(found+1);
		//	 Bluetooth::SendStr(const_cast<char*>(num_str.c_str()));
		//	 Bluetooth::SendStr("\r\n");
			 var = atoi(num_str.c_str());
			 if (var < min_val) var = min_val;
			 if (var > max_val) var = max_val;
		}
			else return;//Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦ Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦



}

bool Parser::hexCharToInt(char symbol, uint8_t* result){
	  switch(symbol){
	    case '0':
	    {
	        *result = 0;
	        return true;
	    }
	    case '1':
	    {
	        *result = 1;
	        return true;
	    }
	    case '2':
	    {
	        *result = 2;
	        return true;
	    }
	    case '3':
	    {
	        *result = 3;
	        return true;
	    }
	    case '4':
	    {
	        *result = 4;
	        return true;
	    }
	    case '5':
	    {
	        *result = 5;
	        return true;
	    }
	    case '6':
	    {
	        *result = 6;
	        return true;
	    }
	    case '7':
	    {
	        *result = 7;
	        return true;
	    }
	    case '8':
	    {
	        *result = 8;
	        return true;
	    }
	    case '9':
	    {
	        *result = 9;
	        return true;
	    }
	    case 'A':
	    case 'a':
	    {
	        *result = 10;
	        return true;
	    }
	    case 'B':
	    case 'b':
	    {
	        *result = 11;
	        return true;
	    }
	    case 'C':
	    case 'c':
	    {
	        *result = 12;
	        return true;
	    }
	    case 'D':
	    case 'd':
	    {
	        *result = 13;
	        return true;
	    }
	    case 'E':
	    case 'e':
	    {
	        *result = 14;
	        return true;
	    }
	    case 'F':
	    case 'f':
	    {
	        *result = 15;
	        return true;
	    }
	    default: return false;



	    }//[switch]



}

void Parser::get_string_argument_value(std::string& str, const char separator,std::string& var){
	std::string::size_type found = str.find(separator);
	if (found!=std::string::npos){
		 var = str.substr(found+1);
		 trim(var);
	}
	else return;
}

bool Parser::get_lic_key_from_str(std::string& str, uint8_t* lic_key){
    volatile int char_counter=0, byte_index=0, long_index=0, byte_pos_in_long=0;
    uint8_t half_byte;
    volatile uint8_t res8;
    if (str.size()<32) return false;
    do{
        res8 = 0;
        if (!hexCharToInt((char)str.c_str()[char_counter],&half_byte)) return false;
        res8 = half_byte<<4;
        char_counter++;
        if (!hexCharToInt((char)str.c_str()[char_counter],&half_byte)) return false;
        res8 += half_byte;
       // lic_key[char_counter/2] = res8;
        byte_index = char_counter/2;
        long_index = byte_index/4;
        byte_pos_in_long = byte_index - long_index*4;

        lic_key[4*(long_index)+3-byte_pos_in_long] = res8;
        char_counter++;
    }while(char_counter < 32);

return true;
}

void Parser::get_int_argument_value(std::string& str, const char separator, uint32_t& var, uint32_t min_val, uint32_t max_val){
	std::string::size_type found = str.find(separator);
			if (found!=std::string::npos){
				 std::string num_str = str.substr(found+1);
	//			 Bluetooth::SendStr(const_cast<char*>(num_str.c_str()));
	//			 Bluetooth::SendStr("\r\n");
				 var = atoi(num_str.c_str());
				 if (var < min_val) var = min_val;
				 if (var > max_val) var = max_val;
			}
				else return;//Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦ Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦




}

void Parser::print_spi_flash_page(uint32_t page_num){
	uint8_t bteTR;
		uint8_t bteFF = 0xFF;
		uint32_t ReadAddr;
	//	uint8_t bteREC;
		ReadAddr = page_num*0x100;
		SPI_FLASH_CS_L();
	////	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
	//	bteTR = W25X_ReadData;
	////	SPI_I2S_SendData16(SPI1,W25X_ReadData);
	////	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
	//	HAL_SPI_Transmit(&hspi1, &bteTR, 1, 100);

		SPI_Read_Write(W25X_ReadData);

		bteTR = (uint8_t)((ReadAddr) >> 16);
		SPI_Read_Write(bteTR);

	////	SPI_I2S_SendData16(SPI1,bteTR);
	////	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
	//	HAL_SPI_Transmit(&hspi1, &bteTR, 1, 100);
		bteTR = (uint8_t)((ReadAddr) >> 8);
		SPI_Read_Write(bteTR);
	////	SPI_I2S_SendData16(SPI1,bteTR);
	////	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
	//	HAL_SPI_Transmit(&hspi1, &bteTR, 1, 100);
		bteTR = (uint8_t) ReadAddr;
		SPI_Read_Write(bteTR);
	////	SPI_I2S_SendData16(SPI1,bteTR);
	////	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
		//HAL_SPI_Transmit(&hspi1, &bteTR, 1, 100);

		uint16_t i;
		uint16_t ii;
		Usart_1::FlushTxBuf();
		for (i = 0; i < 16; i++)
		{
			for (ii=0; ii<16; ii++)
			{
				uint8_t val;
				val = SPI_Read_Write(bteFF);
				char num_str[16];
				itoa(val, num_str, 16);
				Bluetooth::SendStr(num_str);
				Bluetooth::SendStr((char*)";");
			////	pBuffer[i]=SPI2_ReadWriteByte(0XFF);   //????
	//		HAL_SPI_TransmitReceive(&hspi1, &bteFF, &bteREC, 1, 100);
	//		pBuffer[i] = bteREC;
			////SPI_I2S_SendData16(SPI1,bteFF);
			////while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
			////bteREC = (uint8_t)SPI_I2S_ReceiveData16(SPI1);
			////pBuffer[i]=bteREC;

			}
			Bluetooth::SendStr((char*)"\r\n");
		}


	/*
		while (NumByteToRead){
			bteREC = SPI_Read_Write(bteFF);
			if (NumByteToRead >1){




			}
			else{
				SPI1->CR2 |=
			}



		}
	*/

		SPI_FLASH_CS_H();

}


bool Parser::get_page_from_uart_and_write_to_spi_flash(uint32_t page_num){
	uint32_t WriteAddr, bytes_in_buff;
	uint32_t received_bytes_counter = 0;
	unsigned char sym;
	WriteAddr = page_num*0x100;
	Usart_1::FlushRxBuf();
	//Usart::maximum_bytes_in_buff = 0;
	SPI_FLASH_Write_Enable();

			SPI_FLASH_CS_L();

			uint8_t bteTR;
	//		HAL_SPI_Transmit(&hspi1, &bteTR, 1, 100);
			SPI_Read_Write((uint8_t)W25X_PageProgram);

	/*
			bteTR = (uint8_t)((WriteAddr) >> 16);
			HAL_SPI_Transmit(&hspi1, &bteTR, 1, 100);
			bteTR = (uint8_t)((WriteAddr) >> 8);
			HAL_SPI_Transmit(&hspi1, &bteTR, 1, 100);
			bteTR = (uint8_t) WriteAddr;
			HAL_SPI_Transmit(&hspi1, &bteTR, 1, 100);
	*/

			bteTR = (uint8_t)((WriteAddr) >> 16);
			SPI_Read_Write(bteTR);
			bteTR = (uint8_t)((WriteAddr) >> 8);
			SPI_Read_Write(bteTR);
			bteTR = (uint8_t) WriteAddr;
			SPI_Read_Write(bteTR);

	Bluetooth::SendStr((char*)"OK\r\n");
	for(;;)
	{
	if (xSemaphoreTake(Usart_1::xUsartSemaphore, configTICK_RATE_HZ*4/*portMAX_DELAY*/ )== pdTRUE)//Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦ 2 Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦ Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦Р С—РЎвЂ”Р вЂ¦ Si4432
	{
		 do {
			 taskENTER_CRITICAL();
			 bytes_in_buff = Usart_1::GetRxCount();
			 if (bytes_in_buff ){
				 sym = Usart_1::GetChar();
				 taskEXIT_CRITICAL();

						 SPI_Read_Write((uint8_t)sym);
						 received_bytes_counter++;
						 if (received_bytes_counter == 256)
						 {
								SPI_FLASH_CS_H();
								SPI_Flash_Wait_Busy();
								/*
								taskENTER_CRITICAL();
								Usart::FlushRxBuf();
								Usart::FlushTxBuf();
								taskEXIT_CRITICAL();
								*/
								return true;
						 }
						 taskENTER_CRITICAL();
						 bytes_in_buff = Usart_1::GetRxCount();
						 taskEXIT_CRITICAL();
			 }
			 else {

				 taskEXIT_CRITICAL();

			 }


		 }while(bytes_in_buff);
	}
	else{

		SPI_FLASH_CS_H();
		SPI_Flash_Wait_Busy();
	/*
		taskENTER_CRITICAL();
		Usart::FlushRxBuf();
		Usart::FlushTxBuf();
		taskEXIT_CRITICAL();
		*/
		return false;
	}

	}//[for(;;)]
}





bool Parser::erase_lic_key(void){
	FLASH_Status ret_status;
	taskENTER_CRITICAL();
	 FLASH_Unlock();
	 ret_status = FLASH_ErasePage((uint32_t)0x807F800);
	FLASH_Lock();
	taskEXIT_CRITICAL();
	if (ret_status == FLASH_COMPLETE)return true;
	else return false;
}
