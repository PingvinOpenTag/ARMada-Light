/*
 * wavplayer.cpp
 *
 *  Created on: 29 ���. 2019 �.
 *      Author: Pingvin
 */

#include "wavplayer.h"
#include "stm32f10x_dac.h"
#include "stm32f10x_dma.h"
#include "w25x.h"

xQueueHandle WavPlayer::xDmaEventQueue;
xQueueHandle WavPlayer::xSoundsToPlayQueue;
uint8_t WavPlayer::DAC_Buff[2][SOUND_BUFFER_SIZE/2];
xSemaphoreHandle WavPlayer::xSpiFlashSemaphore;
xSemaphoreHandle WavPlayer::xWavPlayerSemaphore;
bool  WavPlayer::m_stop = true;

bool WavPlayer::is_stop(){
	return m_stop;
}

bool WavPlayer::stop(){
if (m_stop) return true;
taskENTER_CRITICAL();
xQueueReset(xSoundsToPlayQueue);
taskEXIT_CRITICAL();
m_stop = true;
if (xSemaphoreTake( xWavPlayerSemaphore, (portTickType)600/*50*/ /*portMAX_DELAY*/)== pdTRUE)//���, ���� ����� �����������
	{
		return true;//�������� �������, ����� ��������� �����������
	}
	else {
		return false;//�� 50 ����� ����� �� ����������� (������� �� �������)
	}

}



WavPlayer::WavPlayer(){

	 init_dma_gpio();
	 init_dac();
	 xDmaEventQueue = xQueueCreate( 4, sizeof(tdma_events_source_type));
	 xSoundsToPlayQueue = xQueueCreate( 62, sizeof(uint8_t));
	 vSemaphoreCreateBinary( xSpiFlashSemaphore);
	 vSemaphoreCreateBinary( xWavPlayerSemaphore);
}

void WavPlayer::initTask(void){
	 xTaskCreate( vWavPlayerTask,/* ( signed char * )*/ "WavPlayerTask", configMINIMAL_STACK_SIZE, NULL, 2,
		                            ( xTaskHandle * ) NULL);
}


void WavPlayer::add_sound_to_play(uint8_t sound_num){
	uint8_t snd;
	snd = sound_num;
	xQueueSendToBack(WavPlayer::xSoundsToPlayQueue,&snd,configTICK_RATE_HZ);
}

void WavPlayer::play_sound_now(uint8_t sound_num){
	stop();
	add_sound_to_play(sound_num);
}
void WavPlayer::playback(uint8_t sound_num){
	 uint32_t sound_size, sound_sample_rate, remaining_sound_raw_data_size;
	 uint16_t sound_bits_per_sample;
	 uint32_t raw_data_curr_pos;
	 tdma_events_source_type dma_event_tmp;

	 if (sound_num <1) return;
	 raw_data_curr_pos = SECTORS_FOR_ONE_SOUND*SPI_FLASH_SECTOR_SIZE*(sound_num-1)+48;

		DMA_DeInit(DMA2_Channel3);
		TIM_DeInit(TIM6);


	 if(!get_sound_size(sound_num, sound_size)) return;


	if (!get_sound_sample_rate(sound_num, sound_sample_rate)) return;

	if (!get_sound_bits_per_sample(sound_num,  sound_bits_per_sample)) return;

	if(!get_sound_raw_data_size(sound_num, remaining_sound_raw_data_size )) return;

	 init_timer(sound_sample_rate);
	 init_dma();
	 //xSemaphoreTake( xWavPlayerSemaphore, (portTickType)1);
	 m_stop = false;
	 if (xSemaphoreTake(xSpiFlashSemaphore,configTICK_RATE_HZ)== pdTRUE){
		 if (remaining_sound_raw_data_size>=SOUND_BUFFER_SIZE){
			 SPI_Flash_Read_With_DMA((uint8_t*) DAC_Buff,raw_data_curr_pos,SOUND_BUFFER_SIZE);
			 remaining_sound_raw_data_size -=SOUND_BUFFER_SIZE;
			 raw_data_curr_pos += SOUND_BUFFER_SIZE;
		 }
		 else{
			 SPI_Flash_Read_With_DMA((uint8_t*) DAC_Buff,raw_data_curr_pos,remaining_sound_raw_data_size);
			 raw_data_curr_pos += remaining_sound_raw_data_size;
			 remaining_sound_raw_data_size =0;

		 }
		xSemaphoreGive(xSpiFlashSemaphore);
	}//[ if (xSemaphoreTake(xSpiFlashSemaphore,configTICK_RATE_HZ)== pdTRUE)]
	 else return;
	 TIM_Cmd(TIM6, ENABLE);
	 do{
		if( xQueueReceive( xDmaEventQueue, (void*)&dma_event_tmp, configTICK_RATE_HZ)==pdTRUE){
			switch (dma_event_tmp){
			case HT:{
				 if (xSemaphoreTake(xSpiFlashSemaphore,configTICK_RATE_HZ)== pdTRUE){
					 if(remaining_sound_raw_data_size >= SOUND_BUFFER_SIZE/2){
						 SPI_Flash_Read_With_DMA((uint8_t*) &DAC_Buff[0],raw_data_curr_pos,SOUND_BUFFER_SIZE/2);
						 remaining_sound_raw_data_size -=SOUND_BUFFER_SIZE/2;
						 raw_data_curr_pos += SOUND_BUFFER_SIZE/2;
					 }
					 else if(remaining_sound_raw_data_size==0){
						 xSemaphoreGive(xSpiFlashSemaphore);
						 break;}
				else{
					 	 SPI_Flash_Read_With_DMA((uint8_t*) &DAC_Buff[1],raw_data_curr_pos,remaining_sound_raw_data_size);
						 raw_data_curr_pos += remaining_sound_raw_data_size;
					 	 remaining_sound_raw_data_size =0;

					 }

					 xSemaphoreGive(xSpiFlashSemaphore);
				 }
			}
			break;
			case TC:{
				 if (xSemaphoreTake(xSpiFlashSemaphore,configTICK_RATE_HZ)== pdTRUE){
					 if(remaining_sound_raw_data_size >= SOUND_BUFFER_SIZE/2){
						 SPI_Flash_Read_With_DMA((uint8_t*) &DAC_Buff[1],raw_data_curr_pos,SOUND_BUFFER_SIZE/2);
						 remaining_sound_raw_data_size -=SOUND_BUFFER_SIZE/2;
						 raw_data_curr_pos += SOUND_BUFFER_SIZE/2;
					 }
					 else if(remaining_sound_raw_data_size==0){
						 xSemaphoreGive(xSpiFlashSemaphore);
						 break;}
					else{
					 	 SPI_Flash_Read_With_DMA((uint8_t*) &DAC_Buff[1],raw_data_curr_pos,remaining_sound_raw_data_size);
						 raw_data_curr_pos += remaining_sound_raw_data_size;
					 	 remaining_sound_raw_data_size =0;

					 }


					 xSemaphoreGive(xSpiFlashSemaphore);
				 }
			}
			break;
			default: break;




			}//	[switch (dma_event_tmp)]




		}//[if( xQueueReceive( xDmaEventQueue, (void*)&dma_event_tmp, configTICK_RATE_HZ)==pdTRUE)]
		else break;




	 }
	 while(remaining_sound_raw_data_size&&(!m_stop));

	 //xSemaphoreGive(xWavPlayerSemaphore);
	 TIM_Cmd(TIM6, DISABLE);


	 while (DAC->DHR8R1 > 0) {

		 DAC->DHR8R1--;

			volatile uint32_t tmpvar;
			for (tmpvar=200;tmpvar!=0;tmpvar--);

	 }

	 m_stop = true;
}

void WavPlayer::init_dma( ) {


	DMA_InitTypeDef  DMA_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
/*
	if(bitsPerSample == 16)  {
		DMA_InitStructure.DMA_PeripheralBaseAddr =(uint32_t)&DAC->DHR12L1;
		  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&DAC_Buff;
		  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
		  DMA_InitStructure.DMA_BufferSize = SOUND_BUFFER_SIZE;
		  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
		  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
		  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
		  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
		  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
		  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
		  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	}
	*/
	//else
	//{
	DMA_InitStructure.DMA_PeripheralBaseAddr =(uint32_t)&DAC->DHR8R1;
	  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&DAC_Buff;//&Sinus;
	  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	  DMA_InitStructure.DMA_BufferSize = SOUND_BUFFER_SIZE; //1024
	  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

	//}

	DMA_Init(DMA2_Channel3, &DMA_InitStructure);

	/* ��������� ���������� �� ��������� �������� ������ */
	DMA_ITConfig(DMA2_Channel3, DMA_IT_TC , ENABLE);
	DMA_ITConfig(DMA2_Channel3, DMA_IT_HT , ENABLE);

	DMA_Cmd(DMA2_Channel3, ENABLE);
    NVIC_SetPriority(DMA2_Channel3_IRQn, 13);
	NVIC_EnableIRQ(DMA2_Channel3_IRQn);

}





 void  WavPlayer::dma_ht_iqr_callback(void){
	 	portBASE_TYPE xHigherPriorityTaskWoken;
	 	xHigherPriorityTaskWoken = pdFALSE;
	 	tdma_events_source_type dma_event_tmp;
	 	dma_event_tmp = HT;
	 	xQueueSendToBackFromISR(xDmaEventQueue, &dma_event_tmp, &xHigherPriorityTaskWoken);
		  if (xHigherPriorityTaskWoken != pdFALSE) {
		      portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
		    }
 }

 void  WavPlayer::dma_tc_iqr_callback(void){
	 	portBASE_TYPE xHigherPriorityTaskWoken;
	 	xHigherPriorityTaskWoken = pdFALSE;
	 	tdma_events_source_type dma_event_tmp;
	 	dma_event_tmp = TC;
	 	xQueueSendToBackFromISR(xDmaEventQueue, &dma_event_tmp, &xHigherPriorityTaskWoken);
		  if (xHigherPriorityTaskWoken != pdFALSE) {
		      portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
		    }
 }



 void  WavPlayer::init_dma_gpio(void){
	GPIO_InitTypeDef gpio_init;
	 RCC_APB1PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	 gpio_init.GPIO_Mode = GPIO_Mode_AIN;
	 gpio_init.GPIO_Pin = GPIO_Pin_4;
	 //gpio_init.GPIO_OType = GPIO_OType_PP;
	 //gpio_init.GPIO_PuPd = GPIO_PuPd_NOPULL;
	 gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
	 GPIO_Init(GPIOA, &gpio_init);


 }



 void WavPlayer::init_dac(void) {
 //PA4
	 //DAC_InitTypeDef dac_init;



	 DAC_InitTypeDef DAC_InitStructure;
	 RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
 /* DAC channel1 Configuration */
   DAC_InitStructure.DAC_Trigger = DAC_Trigger_None;
   DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
   DAC_InitStructure.DAC_OutputBuffer =  DAC_OutputBuffer_Enable;// DAC_OutputBuffer_Disable;
   DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude = 0;
   DAC_Init(DAC_Channel_1, &DAC_InitStructure);
//   DAC_Init(DAC_Channel_2, &DAC_InitStructure);
   DAC_Cmd(DAC_Channel_1, ENABLE);
   //DAC->DHR8R2=0;
   DAC->DHR8R1=0;
  // DAC->DHR12L1 = 0;
   //DAC->DHR12L1 = (uint16_t)0x8000;
  // DAC_Cmd(DAC_Channel_2, ENABLE);
 /*
 DAC_StructInit(&dac_init);
 dac_init.DAC_LFSRUnmask_TriangleAmplitude = 0;
 dac_init.DAC_Trigger = DAC_Trigger_T7_TRGO;
 dac_init.DAC_WaveGeneration = DAC_WaveGeneration_None;
 //dac_init.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
 dac_init.DAC_OutputBuffer = DAC_OutputBuffer_Disable;
 DAC_Init(DAC_Channel_1, &dac_init);
 DAC_Cmd(DAC_Channel_1, ENABLE);
 DAC_DMACmd(DAC_Channel_1, ENABLE);
 */

 }


 bool WavPlayer::get_sound_size(uint8_t sound_num, uint32_t& size){
	 if (xSemaphoreTake(xSpiFlashSemaphore,configTICK_RATE_HZ)== pdTRUE){
	 uint8_t buff[4];
	 uint32_t start_address;
	 size = 0xFFFFFFFF;
	 if (sound_num==0) return false;
	 start_address = SPI_FLASH_SECTOR_SIZE*SECTORS_FOR_ONE_SOUND*(sound_num-1);
	 //SPI_Flash_Read(buff,(uint32_t)0x00, 256);
	 SPI_Flash_Read((uint8_t*) buff,start_address,4);
	 size = *((uint32_t*)buff);
	 if((size!=0)&&(size!= 0XFFFFFFFF)&&(size <= (SPI_FLASH_SECTOR_SIZE*SECTORS_FOR_ONE_SOUND-4))) {
		 xSemaphoreGive(xSpiFlashSemaphore);
		 return true;
	 }
	 xSemaphoreGive(xSpiFlashSemaphore);
	 return false;

	 }//[if (xSemaphoreTake(xSpiFlashSemaphore,configTICK_RATE_HZ)== pdTRUE)]
	 return false;
 }

 bool WavPlayer::get_sound_sample_rate(uint8_t sound_num, uint32_t& sample_rate){
	 if (xSemaphoreTake(xSpiFlashSemaphore,configTICK_RATE_HZ)== pdTRUE){
	 uint8_t buff[4];
 	 uint32_t start_address;
 	 sample_rate= 0xFFFFFFFF;
	 if (sound_num==0) return false;
	 start_address = SPI_FLASH_SECTOR_SIZE*SECTORS_FOR_ONE_SOUND*(sound_num-1)+28;
	 SPI_Flash_Read((uint8_t*) buff,start_address,4);
	 sample_rate = *((uint32_t*)buff);
		 if((sample_rate!=0)&&(sample_rate!= 0XFFFFFFFF)&&(sample_rate<=11025)){
			 xSemaphoreGive(xSpiFlashSemaphore);
			 return true;}
		 xSemaphoreGive(xSpiFlashSemaphore);
		 return false;
	 }//[	 if (xSemaphoreTake(xSpiFlashSemaphore,configTICK_RATE_HZ)== pdTRUE)]
	 return false;
 }

 bool WavPlayer::get_sound_bits_per_sample(uint8_t sound_num, uint16_t& bitsPerSample){
	 if (xSemaphoreTake(xSpiFlashSemaphore,configTICK_RATE_HZ)== pdTRUE){
	 uint8_t buff[2];
 	 uint32_t start_address;
 	 bitsPerSample= 0xFFFF;
	 if (sound_num==0) return false;
	 start_address = SPI_FLASH_SECTOR_SIZE*SECTORS_FOR_ONE_SOUND*(sound_num-1)+38;
	 SPI_Flash_Read((uint8_t*) buff,start_address,2);
	 bitsPerSample = *((uint16_t*)buff);
		 if((bitsPerSample!= 0)&&(bitsPerSample!= 0XFFFF)&&(bitsPerSample<=8)){
			 xSemaphoreGive(xSpiFlashSemaphore);
			 return true;}
		 xSemaphoreGive(xSpiFlashSemaphore);
		 return false;

	 } //[	 if (xSemaphoreTake(xSpiFlashSemaphore,configTICK_RATE_HZ)== pdTRUE)]
	 return false;
 }


 bool WavPlayer::get_sound_raw_data_size(uint8_t sound_num, uint32_t& raw_data_size){
	 if (xSemaphoreTake(xSpiFlashSemaphore,configTICK_RATE_HZ)== pdTRUE){
	 uint8_t buff[4];
 	 uint32_t start_address;
 	raw_data_size = (uint32_t) 0xFFFFFFFF;
	 if (sound_num==0) return false;
	 start_address = SPI_FLASH_SECTOR_SIZE*SECTORS_FOR_ONE_SOUND*(sound_num-1)+44;
	 SPI_Flash_Read((uint8_t*) buff,start_address,4);
	 raw_data_size = *((uint32_t*)buff);
		 if((raw_data_size!= (uint32_t)0XFFFFFFFF)&&(raw_data_size!=0)){
			 xSemaphoreGive(xSpiFlashSemaphore);
			 return true;}
		 xSemaphoreGive(xSpiFlashSemaphore);
		 return false;
	 } return false;
 }


 void WavPlayer::vWavPlayerTask (void *pvParameters){
	 uint8_t snd = 1;
	 for(;;){
		 if( xQueueReceive( xSoundsToPlayQueue, (void*)&snd, portMAX_DELAY)==pdTRUE){
//			 if (xSemaphoreTake( xWavPlayerSemaphore, (portTickType)configTICK_RATE_HZ)==pdTRUE)
	//		 {
				 playback(snd);
				 xSemaphoreGive(xWavPlayerSemaphore);

		//	 }

		 }

/*
		vTaskDelay(configTICK_RATE_HZ);
		 playback(snd++);

	 if (snd >NUMBER_OF_SOUNDS) snd = 1;
	*/


	 }


 }


 //������ 6 ������ ������� ������ ���, ����� ��� ������ ��������� �������� ������
 void WavPlayer::init_timer(uint32_t sampleRate) {
 	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

 	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
 	//TIM2 - ���������� DAC (������������ ��������� ���������)
 	  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
 	  TIM_TimeBaseStructure.TIM_Period = (uint16_t)(/*72000000*/SystemCoreClock/sampleRate);//1636;//1333;
 	  TIM_TimeBaseStructure.TIM_Prescaler = 0;
 	  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
 	  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
 	  TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);

 	  //TIM2 TRGO selection
 	  TIM_SelectOutputTrigger(TIM6, TIM_TRGOSource_Update);

 	  TIM_DMACmd(TIM6, TIM_DMA_Update, ENABLE);

 ////	  TIM_Cmd(TIM2, ENABLE);


 /*
 TIM_TimeBaseInitTypeDef tim_init;
 RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);
 TIM_TimeBaseStructInit(&tim_init);
 tim_init.TIM_RepetitionCounter = 0;
 tim_init.TIM_Period = 1705 - 1;
 tim_init.TIM_Prescaler = 0;
 tim_init.TIM_ClockDivision = TIM_CKD_DIV1;
 tim_init.TIM_CounterMode = TIM_CounterMode_Up;
 TIM_TimeBaseInit(TIM7, &tim_init);
 TIM_SelectOutputTrigger(TIM7, TIM_TRGOSource_Update);
 //TIM_Cmd(TIM4, ENABLE);
 TIM_Cmd(TIM7, DISABLE);
 //TIM_DMACmd();
 */

 }




#ifdef __cplusplus
 extern "C" {
  #endif




 void DMA2_Channel3_IRQHandler(void)
 {
 	//static portBASE_TYPE xHigherPriorityTaskWoken;
 	//xHigherPriorityTaskWoken = pdFALSE;
 	   if (DMA_GetFlagStatus(DMA2_FLAG_HT3)==SET)
 	   {
 		   	DMA_ClearFlag(DMA2_FLAG_HT3);
 		    WavPlayer::dma_ht_iqr_callback();
// 		    xSemaphoreGiveFromISR(xSoundBuffHTSemaphore, &xHigherPriorityTaskWoken );


 	   }

 	   if (DMA_GetFlagStatus(DMA2_FLAG_TC3)==SET)
 	   {
 		   	DMA_ClearFlag(DMA2_FLAG_TC3);
 		    WavPlayer::dma_tc_iqr_callback();
// 		    xSemaphoreGiveFromISR(xSoundBuffTCSemaphore, &xHigherPriorityTaskWoken );
 	   }

 }




#ifdef __cplusplus
  }
#endif

