#ifndef _WAVPLAYER_H
#define _WAVPLAYER_H

#include <stdint.h>
#include <stdlib.h>
#include "stm32f10x.h"



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


 constexpr static uint32_t SOUND_BUFFER_SIZE = 1024;

 enum dma_events_source_type {
 	HT,
 	TC
 };

 typedef enum dma_events_source_type tdma_events_source_type;

class WavPlayer{

public:
	 WavPlayer();
	 static void dma_ht_iqr_callback(void);
	 static void dma_tc_iqr_callback(void);
	 static xQueueHandle xDmaEventQueue;
	 static xQueueHandle xSoundsToPlayQueue;
	 static uint8_t DAC_Buff[2][SOUND_BUFFER_SIZE/2];
	 static void playback(uint8_t sound_num);
	 static void add_sound_to_play(uint8_t sound_num);
	 static void play_sound_now(uint8_t sound_num);
	 void initTask(void);
	 static  xSemaphoreHandle xSpiFlashSemaphore;
	 static void init_dac(void);
	 static xSemaphoreHandle xWavPlayerSemaphore;
	 static bool is_stop(void);
	 static bool stop(void);

private:
	 static bool m_stop;


	 static void init_dma(void);
	 static void init_dma_gpio(void);
	 static void init_timer(uint32_t sampleRate);
	 static bool get_sound_size(uint8_t sound_num, uint32_t& size);
	 static bool get_sound_sample_rate(uint8_t sound_num, uint32_t& sample_rate);
	 static bool get_sound_bits_per_sample(uint8_t sound_num, uint16_t& bitsPerSample);
	 static bool get_sound_raw_data_size(uint8_t sound_num, uint32_t& raw_data_size);
	 static void vWavPlayerTask (void *pvParameters);

protected:



 };



#endif
