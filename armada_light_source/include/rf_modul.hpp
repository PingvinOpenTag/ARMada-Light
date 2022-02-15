#ifndef _RF_MODUL_HPP
#define _RF_MODUL_HPP
#include "bluetooth.h"
#ifdef __cplusplus
 extern "C" {
  #endif
#include "../FreeRTOS/include/FreeRTOS.h"
#include "../FreeRTOS/include/task.h"
#include "../FreeRTOS/include/queue.h"
#include "../FreeRTOS/include/semphr.h"

#include <string.h>
#include <stdint.h>

#ifdef __cplusplus
  }
#endif

#include "si4432.hpp"
#include "network-base-types.hpp"
#include "system.h"
#include <list>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <cmath>

 constexpr static uint8_t last_packages_max_size = 64;
 constexpr static uint8_t wait_packages_max_size = 32;
 constexpr static uint8_t  NMBR_OF_SAMPLE_SETTING1 = 7;
 constexpr static uint8_t  NMBR_OF_PARAMETER = 12;

 const uint8_t RMRfSettings[NMBR_OF_SAMPLE_SETTING1][NMBR_OF_PARAMETER] =
 {
 // IFBW,    COSR, 	CRO2, CRO1, CRO0, 	CTG1, CTG0, 	TDR1, TDR0, 	MMC1, 	FDEV, 	AFC
 {  0x01,    0x83, 	0xc0, 0x13, 0xa9, 	0x00, 0x05, 	0x13, 0xa9, 	0x20, 	0x3a, 	0x40}, //DR: 2.4kbps, DEV: +-36kHz, BBBW: 75.2kHz
 {  0x04,    0x41, 	0x60, 0x27, 0x52, 	0x00, 0x0a, 	0x27, 0x52, 	0x20, 	0x48, 	0x40}, //DR: 4.8kbps, DEV: +-45kHz, BBBW: 95.3kHz
 {  0x91,    0x71, 	0x40, 0x34, 0x6e, 	0x00, 0x18, 	0x4e, 0xa5, 	0x20, 	0x48, 	0x40}, //DR: 9.6kbps, DEV: +-45kHz, BBBW: 112.8kHz
 {  0x91,    0x39, 	0x20, 0x68, 0xDC, 	0x00, 0xD3, 	0x9D, 0x49, 	0x20, 	0x0F, 	0x40}, //DR: 19.2kbps, DEV: +-9.6kHz, BBBW: 28.8kHz
 {  0x91, 	0x9C, 	0x00, 0xD1, 0xB7, 	0x01, 0xA6, 	0x09, 0xD5, 	0x00, 	0x1F, 	0x40}, //DR: 38.4kbps, DEV: +-19.2kHz, BBBW: 57.6kHz
 {  0x92, 	0x68, 	0x01, 0x3A, 0x93, 	0x02, 0x78, 	0x0E, 0xBF, 	0x00, 	0x2E, 	0x40}, //DR: 57.6kbps, DEV: +-28.8kHz, BBBW: 86.4kHz
 {  0x9B, 	0x34, 	0x02, 0x75, 0x25, 	0x04, 0xEE, 	0x1D, 0x7E, 	0x00, 	0x5C, 	0x00}, //DR: 115.2kbps, DEV: +-57.6kHz, BBBW: 172.8kHz

 };

 const trx_package null_hit_data = {0,0,0};

 enum rfSetings{
 // Data    Rate;  Freq Deviation; Receiver Bandwidth
 dataRate2400BPS_DEV36KHZ=0,
 // DR = 2.4kbps;Fdev = +-36kHz;BBBW = 75.2kHz;
 dataRate4800BPS_DEV45KHZ = 1,
 // DR = 4.8kbps;Fdev = +-45kHz;BBBW = 95.3kHz;
 dataRate9600BPS_DEV45KHZ = 2,
 // DR = 9.6kbps;     Fdev = +-45kHz;    BBBW = 112.8kHz;
 dataRate10000BPS_DEV12KHZ = 3,
 // DR = 10kbps;      Fdev = +-12kHz;    BBBW = 41.7kHz;
 dataRate20000BPS_DEV12KHZ = 4,
 // DR = 20kbps;      Fdev = +-12kHz;    BBBW = 45.2kHz;
 dataRate40000BPS_DEV20KHZ = 5,
 // DR = 40kbps;      Fdev = +-20kHz;    BBBW = 83.2kHz;
 dataRate50000BPS_DEV25KHZ = 6,
 // DR = 50kbps;      Fdev = +-25kHz;    BBBW = 112.8kHz;
 dataRate100000BPS_DEV50KHZ = 7,
 // DR = 100kbps;    Fdev = +-50kHz;    BBBW = 208kHz;
 dataRate128000BPS_DEV64KHZ = 8,
 // DR = 128kbps;    Fdev = +-64kHz;    BBBW = 269.3kHz;
 };

 using tRfSettings = rfSetings;

enum rfTxPower{
	power_1dBm =0,
	power_2dBm,
	power_5dBm,
	power_8dBm,
	power_11dBm,
	power_14dBm,
	power_17dBm,
	power_20dBm

};





using tRF_Power = rfTxPower;

enum _RF_ENUM
{
                RF_OK  = 0x00,  //function  response  parameters
                RF_ERROR_TIMING  = 0x01,
                RF_ERROR_PARAMETER  = 0x02,
                RF_PACKET_RECEIVED  = 0x03,
                RF_RX_FIFO_ALMOST_FULL = 0x04,
                RF_NO_PACKET = 0x05,
                RF_CRC_ERROR  = 0x06,
};


using RF_ENUM = _RF_ENUM;

using tPackageId = uint16_t;

/*
struct DeviceAddress
{
	uint8_t address[3];
};
*/
using  tDeviceAddress = DeviceAddress;

/*
#pragma pack(push, 1)
struct PackageDetails
{
	tPackageId packageId;
	uint8_t TTL : 7;
uint8_t needAck : 1;

};
#pragma pack(pop)
*/

using  tPackageDetails = PackageDetails;


//constexpr static uint8_t payloadLength = 25;
constexpr static unsigned long int RF_RX_BUFFER_SIZE  = 16;
constexpr static uint8_t RF_LAST_RECEIVED_PACKAGES_BUFFER_SIZE  = 64;

constexpr static uint32_t defaultTimeout = 5000000;
constexpr static uint32_t defaultResendTime = 500000;
constexpr static uint32_t defaultResendTimeDelta = 500000;


/*
#pragma pack(push, 1)
struct Package
{
	tDeviceAddress sender;
	tDeviceAddress target;
	tPackageDetails details;
	uint8_t payload[payloadLength];

};
#pragma pack(pop)
*/
using tPackage = Package;

using tPackageTimings = PackageTimings;


extern System sys;

template<class SPIx>
class RfModul {
public:


RfModul(uint8_t nSEL_PORT, uint8_t nSEL_PIN,
		uint8_t SDN_PORT, uint8_t SDN_PIN,
		uint8_t nIQR_PORT, uint8_t nIQR_PIN): si4432(
				nSEL_PORT, nSEL_PIN,
				SDN_PORT, SDN_PIN,
				nIQR_PORT, nIQR_PIN) {
	m_lastReceivedIds.reserve(last_packages_max_size);
	m_waitingPackages.reserve(wait_packages_max_size);
	get_net_adrress(&net_address);
	self_pointer = this;
	vSemaphoreCreateBinary(tx_buff_Semaphore);
	xRfRxEventQueue = xQueueCreate( RF_RX_BUFFER_SIZE, sizeof(tPackage));
	//xRfRxUniquePackagesQueue=xQueueCreate( 4/*RF_RX_BUFFER_SIZE*/, sizeof(tPackage));


	xTaskCreate( rf_tx_interrogate_task, "rf_tx_task", configMINIMAL_STACK_SIZE*2, NULL, tskIDLE_PRIORITY +1,
				                        ( xTaskHandle * ) NULL);
/*
	xTaskCreate( rf_rx_interrogate_task, "rf_rx_task", configMINIMAL_STACK_SIZE*2, NULL, tskIDLE_PRIORITY+1 ,
					                        ( xTaskHandle * ) NULL);
*/
				                        //init();
//	init_with_parameters(/*dataRate9600BPS_DEV45KHZ*/dataRate2400BPS_DEV36KHZ,power_11dBm);

};


static RfModul* self_pointer;
public:
static tPackageTimings timings_to_send;


static void rf_rx_interrogate_task (void *pvParameters){
	 for(;;)
	 {

		 	 self_pointer->read_next_packages();

	 }

}


void inline read_next_packages(void){


	static volatile tPackage package_tmp;//,test_package;
	static volatile portTickType waiting_interval = configTICK_RATE_HZ*5;//portMAX_DELAY;
	tstatus_payload *status_payload_tmp;
	if (xQueueReceive( xRfRxEventQueue, (void*)&package_tmp, waiting_interval)==pdPASS)
	{//если прочитали

		status_payload_tmp = (tstatus_payload*)&(package_tmp.payload[0]);
		if (status_payload_tmp->hit_data.need_to_send==true){

			if ((status_payload_tmp->hit_data.team_color==sys.settings.player.team_color)&&(status_payload_tmp->hit_data.player_id==sys.settings.player.id)){
				if(sys.bluetooth->connectStatus()){
//						Bluetooth::SendStr((char*)"B");
				}

			}
		}





/*
		static volatile uint32_t i=0, y=0;
		y++;
		if(y==5){

			i=rx_counter;
			y=0;
			rx_counter=0;
		}
*/

	}
	else{//не дождались пакета
		send_status_package(false,null_hit_data);
	}


}


static void rf_tx_interrogate_task (void *pvParameters){
		volatile static bool result = false;
		static unsigned char reg_tmp = 0;

	while (self_pointer == 0){
		 vTaskDelay(100);
	 }
	timings_to_send.infiniteResend=0;
	timings_to_send.timeout = (defaultTimeout/1000000)*configTICK_RATE_HZ;
	timings_to_send.resendTime = (defaultResendTime*configTICK_RATE_HZ)/1000000;
	timings_to_send.resendTimeDelta = (defaultResendTimeDelta*configTICK_RATE_HZ)/1000000;

//	Pin FlashLedPin(PORTB,PIN1);
//	 FlashLedPin.switchToOutput();

	while (reg_tmp!=0x08){
	 vTaskDelay(100);
	 result = self_pointer->init_with_parameters(dataRate2400BPS_DEV36KHZ,power_11dBm);
	 vTaskDelay(100);
	 reg_tmp =  self_pointer->si4432.readReg(Si4432_Reg_Device_Type_Code);
	// result = self_pointer->TxBuf((unsigned char*)"Hello",5);
	 if (reg_tmp==0x08){
	 			vTaskDelay(100);
	 			/*
	 			FlashLedPin.set();
	 			vTaskDelay(100);
	 			FlashLedPin.reset();
	 			vTaskDelay(100);
	 			FlashLedPin.set();
	 			vTaskDelay(100);
	 			FlashLedPin.reset();
	 			*/
	 	 }
	 	 else{
	 	//	 FlashLedPin.set();
	 		 vTaskDelay(600);
	 	//	 FlashLedPin.reset();


	 	 }
	}
	 xTaskCreate( rf_rx_interrogate_task, "rf_rx_task", configMINIMAL_STACK_SIZE*2, NULL, tskIDLE_PRIORITY+1 ,
					                        ( xTaskHandle * ) NULL);
	 vTaskDelay(100);
	 for(;;){
		 self_pointer->send_next_package();
	 }
}




void inline send_next_package(void){

	static volatile portTickType waiting_interval = portMAX_DELAY;
	static volatile portTickType unlock_time = 0;
	static volatile portTickType time_tmp;
	static tPackage rx_package_tmp;
	volatile unsigned char Bytes=0;
	constexpr static portTickType BLOCKING_PERIOD_AFTER_RECEIVING = 2;
	constexpr static portTickType BLOCKING_PERIOD_AFTER_TRANSMISSION = 1;
	static AckPayload ackPayload_tmp;

	if (Rx((unsigned char *)&rx_package_tmp,(unsigned char *)&Bytes,waiting_interval))
	{
		rx_counter++;
		time_tmp =  xTaskGetTickCount();
		unlock_time = time_tmp + BLOCKING_PERIOD_AFTER_RECEIVING;
		waiting_interval = 	BLOCKING_PERIOD_AFTER_RECEIVING;

		tstatus_payload* pStatusPayload;
		pStatusPayload = (tstatus_payload* )&(rx_package_tmp.payload[0]);

		if (!package_for_me(rx_package_tmp.target)){

			if(pStatusPayload->hit_data.need_to_send==false)return;//если это не подтверждение попадания
		}
		if ((rx_package_tmp.details.needAck)//принятый пакет требует подтверждения доставки
			&&(pStatusPayload->hit_data.need_to_send==false))//но игнорируем, если это подтверждение попадания
				{

					ackPayload_tmp.size=0x02;
					ackPayload_tmp.operationCode=ackPayload_tmp.acknoledgementCode;
					ackPayload_tmp.packageId=rx_package_tmp.details.packageId;
					send(rx_package_tmp.sender, net_address, (uint8_t*)&ackPayload_tmp,sizeof(AckPayload),false, timings_to_send);

				}//[if (rx_package_tmp.details.needAck)]

		else //принятый пакет не требует подтверждения доставки
		{
			AckPayload* pAckPayload_tmp;
			pAckPayload_tmp = (AckPayload*)&rx_package_tmp.payload;
			if(pAckPayload_tmp->isAck())//если пришло подтверждение доставки

			{
//						display_backlight_on();
			PackageId pack_id_tmp;

			pack_id_tmp = pAckPayload_tmp->packageId;
			 if (xSemaphoreTake(tx_buff_Semaphore, configTICK_RATE_HZ*2)== pdTRUE)//ждем пока буфер освободится
					{
				 	 	 remove_waiting_package(pack_id_tmp);
					 	 xSemaphoreGive(tx_buff_Semaphore);

					}



			return;
			}//[if(pAckPayload_tmp->isAck())//если пришло подтверждение доставки]
		}//[else //принятый пакет не требует подтверждения доставки]


		if(!package_already_received(rx_package_tmp.details.packageId)){//если этот пакет ранее не принимался
			lastReceivedIdsAdd(rx_package_tmp.details.packageId);
			xQueueSend( xRfRxEventQueue, ( void * ) &rx_package_tmp, configTICK_RATE_HZ);
		}
		return;



	}//[if (self_pointer->Rx((unsigned char *)&rx_package_tmp,(unsigned char *)&Bytes,waiting_interval))]

	else//пакет не получен, либо период закончился, либо в буфере изменения
	{
		//>>1
		time_tmp =  xTaskGetTickCount();
		if  (unlock_time > time_tmp)//передача заблокирована
		{
		//>>2
			waiting_interval = unlock_time - time_tmp; //дождемся конца блокировки
			return;
		}
		//>>1


		if (xSemaphoreTake(tx_buff_Semaphore, configTICK_RATE_HZ*2)== pdTRUE)//ждем 2 секунды семафора
		{
			//>>2
			if(m_waitingPackages.size())//если буфер для отправки не пуст
			{
				//>>3
				if(m_waitingPackages.front().nextTransmission > time_tmp)//рано отправлять пакет
				{
					//>>4
					waiting_interval = m_waitingPackages.front().nextTransmission - time_tmp;
					xSemaphoreGive(tx_buff_Semaphore);
					return;
				}
				//>>3
				else//пора отправлять пакет
				{
					//>>4
					WaitingPackage waiting_package_tmp;
					if (m_waitingPackages.size())//если смогли забрать пакет из буфера
					{
						//>>5
						waiting_package_tmp = m_waitingPackages.front();
						m_waitingPackages.erase(m_waitingPackages.begin());//удаляем пакет из очереди на отправку
						if(!waiting_package_tmp.package.details.needAck)////если пакет не требует подтверждения
						{
							//>>6
							TxBuf((unsigned char*)&waiting_package_tmp.package, sizeof(tPackage));//отправляем пакет
							unlock_time = xTaskGetTickCount() +BLOCKING_PERIOD_AFTER_TRANSMISSION;
							waiting_interval=BLOCKING_PERIOD_AFTER_TRANSMISSION;
							xSemaphoreGive(tx_buff_Semaphore);
							return;
						}//[if(!self_pointer->m_waitingPackages.front().package.details.needAck)////если пакет не требует подтверждения]
						//>>5
						else//пакет требует подтверждения
						{
							//>>6
							//если подтверждение ещё не получено
							//и не закончился таймаут передачи
							if((waiting_package_tmp.wasCreated + waiting_package_tmp.timings.timeout) > time_tmp)
							{
								TxBuf((unsigned char*)&waiting_package_tmp.package, sizeof(tPackage));//отправляем пакет
								waiting_package_tmp.nextTransmission = time_tmp + waiting_package_tmp.timings.resendTime+random(waiting_package_tmp.timings.resendTimeDelta);
							//	self_pointer->m_waitingPackages.pop_front();//удаляем пакет из начала очереди на отправку
								m_waitingPackages.push_back(waiting_package_tmp);//добавляем в конец очереди
							}
						}//[else//пакет требует подтверждения]
						//>>5

					}//[if (self_pointer->m_waitingPackages.size())//если смогли забрать пакет из буфера]
					//>>4
					unlock_time = time_tmp + BLOCKING_PERIOD_AFTER_TRANSMISSION;
					if(m_waitingPackages.size())//если на очереди есть пакет
					{

						if(m_waitingPackages.front().nextTransmission > time_tmp)
						{
							waiting_interval = m_waitingPackages.front().nextTransmission - time_tmp;
						}
						else
						{
							//waiting_interval = 0;

							m_waitingPackages.front().nextTransmission = time_tmp + m_waitingPackages.front().timings.resendTime+ random(m_waitingPackages.front().timings.resendTimeDelta);// + random(waiting_package_tmp.timings.resendTimeDelta);
							waiting_interval =m_waitingPackages.front().nextTransmission -time_tmp;

						}
					}//[if(self_pointer->m_waitingPackages.size())]
					else//очередь на отправку пуста
					{
						waiting_interval = portMAX_DELAY;
					}
					xSemaphoreGive(tx_buff_Semaphore);
					return;

				}//[else//пора отправлять пакет]
				//>>3


			}//[if(self_pointer->m_waitingPackages.size())//если буфер для отправки не пуст]
			//>>2
			else//если буфер для отправки пуст
			{
				waiting_interval = portMAX_DELAY;
				xSemaphoreGive(tx_buff_Semaphore);
				return;
			}//[else//если буфер для отправки пуст]
			xSemaphoreGive(tx_buff_Semaphore);
		}//[if (xSemaphoreTake(self_pointer->tx_buff_Semaphore, configTICK_RATE_HZ*2)== pdTRUE)//ждем 2 секунды семафора]

	}//[else//пакет не получен, либо период закончился, либо в буфере изменения]




	/*
	if (self_pointer->Rx((unsigned char *)&rx_package_tmp,(unsigned char *)&Bytes,waiting_interval)){
		time_tmp =  xTaskGetTickCount();
		unlock_time = time_tmp + BLOCKING_PERIOD_AFTE_RECEIVING;
		waiting_interval = 	BLOCKING_PERIOD_AFTE_RECEIVING;

		if (!self_pointer->package_for_me(rx_package_tmp.target)) return;
		if (rx_package_tmp.details.needAck)//принятый пакет требует подтверждения доставки
				{

					ackPayload_tmp.size=0x02;
					ackPayload_tmp.operationCode=ackPayload_tmp.acknoledgementCode;
					ackPayload_tmp.packageId=rx_package_tmp.details.packageId;
					self_pointer->send(rx_package_tmp.sender, net_address, (uint8_t*)&ackPayload_tmp,sizeof(AckPayload),false, timings_to_send);

				}//[if (rx_package_tmp.details.needAck)]

		else //принятый пакет не требует подтверждения доставки
		{
			AckPayload* pAckPayload_tmp;
			pAckPayload_tmp = (AckPayload*)&rx_package_tmp.payload;
			if(pAckPayload_tmp->isAck())//если пришло подтверждение доставки

			{
//						display_backlight_on();
			PackageId pack_id_tmp;

			pack_id_tmp = pAckPayload_tmp->packageId;
			 if (xSemaphoreTake(tx_buff_Semaphore, configTICK_RATE_HZ*2)== pdTRUE)//ждем пока буфер освободится
					{
				 	 	 self_pointer->remove_waiting_package(pack_id_tmp);
					 	 xSemaphoreGive(tx_buff_Semaphore);

					}



			return;
			}//[if(pAckPayload_tmp->isAck())//если пришло подтверждение доставки]
		}//[else //принятый пакет не требует подтверждения доставки]


		if(!package_already_received(rx_package_tmp.details.packageId)){//если этот пакет ранее не принимался
			xQueueSend( self_pointer->xRfRxEventQueue, ( void * ) &rx_package_tmp, configTICK_RATE_HZ);
		}
		return;

	}//[if (self_pointer->Rx((unsigned char *)&rx_package_tmp,(unsigned char *)&Bytes,waiting_interval))]

	else{//пакет не получен, либо период закончился, либо в буфере изменения
		time_tmp =  xTaskGetTickCount();
		if  (unlock_time > time_tmp)//передача заблокирована
		{
			waiting_interval = unlock_time - time_tmp; //дождемся конца блокировки
			return;
		}

		if (xSemaphoreTake(self_pointer->tx_buff_Semaphore, configTICK_RATE_HZ*2)== pdTRUE)//ждем 2 секунды семафора
		{
				if(self_pointer->m_waitingPackages.size())//если буфер для отправки не пуст
				{


					if(self_pointer->m_waitingPackages.front().nextTransmission > time_tmp)//рано отправлять пакет
					{
						waiting_interval = self_pointer->m_waitingPackages.front().nextTransmission - time_tmp;
										xSemaphoreGive(self_pointer->tx_buff_Semaphore);
										return;

					}

					else//пора отправлять пакет
					{
						WaitingPackage waiting_package_tmp;

						waiting_package_tmp = self_pointer->m_waitingPackages.front();
						self_pointer->m_waitingPackages.pop_front();//удаляем пакет из очереди на отправку

						if(!waiting_package_tmp.package.details.needAck)////если пакет не требует подтверждения
						{

							self_pointer->TxBuf((unsigned char*)&waiting_package_tmp.package, sizeof(tPackage));//отправляем пакет

							unlock_time = xTaskGetTickCount() +BLOCKING_PERIOD_AFTE_TRANSMISSION;
							waiting_interval=BLOCKING_PERIOD_AFTE_TRANSMISSION;
							xSemaphoreGive(self_pointer->tx_buff_Semaphore);
							return;
						}//[if(!self_pointer->m_waitingPackages.front().package.details.needAck)////если пакет не требует подтверждения]
						else//пакет требует подтверждения
						{
							//если подтверждение ещё не получено
							//и не закончился таймаут передачи
							//пропускаем все отправленные пакеты
							if((waiting_package_tmp.wasCreated + waiting_package_tmp.timings.timeout) > time_tmp)
							{
								self_pointer->TxBuf((unsigned char*)&waiting_package_tmp.package, sizeof(tPackage));//отправляем пакет
								waiting_package_tmp.nextTransmission = time_tmp + waiting_package_tmp.timings.resendTime;
							//	self_pointer->m_waitingPackages.pop_front();//удаляем пакет из начала очереди на отправку
								self_pointer->m_waitingPackages.push_back(waiting_package_tmp);//добавляем в конец очереди


							}



						}//[else//пакет требует подтверждения]


					}	//[else//пора отправлять пакет]





				}//[if(self_pointer->m_waitingPackages.size()){//если буфер для отправки не пуст]
				unlock_time = time_tmp +BLOCKING_PERIOD_AFTE_TRANSMISSION;
				if(self_pointer->m_waitingPackages.size())
				{

					if(self_pointer->m_waitingPackages.front().nextTransmission > time_tmp)
					{
						waiting_interval = self_pointer->m_waitingPackages.front().nextTransmission - time_tmp;
					}
					else
					{
						self_pointer->m_waitingPackages.front().nextTransmission = time_tmp + self_pointer->m_waitingPackages.front().timings.resendTime;// + random(waiting_package_tmp.timings.resendTimeDelta);
						waiting_interval =self_pointer->m_waitingPackages.front().nextTransmission -time_tmp;
					}
				}//[if(self_pointer->m_waitingPackages.size())]
					else//если буфер для отправки пуст
					{
						waiting_interval = portMAX_DELAY;
						xSemaphoreGive(self_pointer->tx_buff_Semaphore);
						return;
					}//[else //если буфер для отправки пуст]
			xSemaphoreGive(self_pointer->tx_buff_Semaphore);
		}//[if (xSemaphoreTake(self_pointer->tx_buff_Semaphore, configTICK_RATE_HZ*2)== pdTRUE)//ждем 2 секунды семафора]

	}//[else{//пакет не получен, либо период закончился, либо в буфере изменения]

	//static tAckPayload ackPayload_tmp;



*/
}

bool send(tDeviceAddress target, tDeviceAddress sender,  uint8_t* data, uint16_t size,  bool waitForAck,  tPackageTimings timings){
	bool result=false;
	WaitingPackage rf_package_tmp;
	rf_package_tmp.package.target = target;
	rf_package_tmp.package.sender = sender;
	rf_package_tmp.package.details.needAck = waitForAck;
	rf_package_tmp.timings = timings;
	memcpy (rf_package_tmp.package.payload, data, size);
	 if (size<Package::payloadLength/*rf_package_tmp.package.payloadLength*/)
	        {
	            memset(&(rf_package_tmp.package.payload[size]), 0, Package::payloadLength-size);
	}
	 rf_package_tmp.package.details.packageId = generatePackageId();
	 rf_package_tmp.wasCreated = xTaskGetTickCount();
	 rf_package_tmp.nextTransmission = rf_package_tmp.wasCreated;
	 if (xSemaphoreTake(tx_buff_Semaphore, configTICK_RATE_HZ*2/*portMAX_DELAY*/ )== pdTRUE)//���� 2 ������� ���������� Si4432
		{
		 	 result=PutWaitingPackage(rf_package_tmp);
		 	 xSemaphoreGive(tx_buff_Semaphore);
		 	xSemaphoreGive( self_pointer->si4432.IQR_Semaphore);
			return result;
		}
	 else result =  false;//�� ��������� ��������

return result;
}



void send_status_package(bool need_to_send_hit_confirmation, trx_package hit_data){
	tstatus_payload status_payload_tmp;
	status_payload_tmp.hit_data.need_to_send = need_to_send_hit_confirmation;
	status_payload_tmp.hit_data.in_game = sys.in_game();
	status_payload_tmp.hit_data.gun_damage = hit_data.damage;
	status_payload_tmp.hit_data.player_id = hit_data.player_id;
	status_payload_tmp.hit_data.team_color = hit_data.team_id;

	status_payload_tmp.type = STATUS_DATA;
	status_payload_tmp.team_color = sys.settings.player.team_color;
	status_payload_tmp.gun_damage = sys.settings.gun.damage;
	status_payload_tmp.in_game = sys.in_game();
	status_payload_tmp.bluetooth_is_connected = sys.bluetooth->connectStatus();
	status_payload_tmp.player_id = sys.settings.player.id;
	status_payload_tmp.health = sys.settings.player.health;
	status_payload_tmp.clips = sys.settings.gun.clips;
	int voltage;
	voltage = sys.adc->voltage();
	status_payload_tmp.battery_voltage = (uint16_t) voltage;
	memcpy((void*)status_payload_tmp.nickname,"Pingvin\0",strlen("Pingvin\0")+1);
	bool send_result;
	constexpr static uint16_t packageLength = 32;//IRadioPhysicalDevice::defaultRFPhysicalPayloadSize;
		constexpr static uint16_t payloadLength /*23*/ = packageLength /*32*/ - sizeof(DeviceAddress)/*3*/ - sizeof(DeviceAddress)/*3*/ - sizeof(PackageDetails)/*3*/;
	send_result = self_pointer->send(server_address, this->net_address, (uint8_t*)&status_payload_tmp,payloadLength,true/*false*/, timings_to_send);
}




bool PutWaitingPackage(WaitingPackage& new_package) //�������� ����� � �����
{

	if (m_waitingPackages.size()>=wait_packages_max_size) return false;
	m_waitingPackages.insert(m_waitingPackages.begin(),new_package);
	return true;
}


void init(void){

	static unsigned char reg_tmp;
		xSemaphoreTake(si4432.IQR_Semaphore, 1/*portMAX_DELAY*/ );
		//init_spi_for_si4432();
		//init_isr_for_si4432();
		//чтение статусов
		reg_tmp =  si4432.readReg(Si4432_Reg_Interrupt_Status_1);
		reg_tmp =  si4432.readReg(Si4432_Reg_Interrupt_Status_2);
		//выполнение программного сброса
		si4432.writeReg(Si4432_Reg_Operating_Mode_1, Si4432_Mode1_Soft_Reset);
		if (xSemaphoreTake(si4432.IQR_Semaphore, configTICK_RATE_HZ*3	/*portMAX_DELAY*/ )== pdTRUE)//ждем 3 секунды прерывание Si4432
		  {//дождались
		 	 volatile int a;
		 	 a++;
		  }
		 else {// не дождались
			 volatile int b;
			 b++;
			 }

		//чтение статусов
		reg_tmp =  si4432.readReg(Si4432_Reg_Interrupt_Status_1);
		reg_tmp =  si4432.readReg(Si4432_Reg_Interrupt_Status_2);
		//установка центральной частоты 433,92 МГц
		si4432.writeReg(Si4432_Reg_Frequency_Band_Select/*0x75*/, 0x53);
		si4432.writeReg(Si4432_Reg_Nominal_Carrier_Frequency_H /*0x76*/, 0x62);
		si4432.writeReg(Si4432_Reg_Nominal_Carrier_Frequency_L /*0x77*/, 0x00);
		//установка скорости передачи (9.6 kbps)
		si4432.writeReg(Si4432_Reg_Tx_Data_Rate_1/*0x6E*/, 0x4E);
		si4432.writeReg(Si4432_Reg_Tx_Data_Rate_0 /*0x6F*/, 0xA5);
		si4432.writeReg(Si4432_Reg_Modulation_Mode_Ctrl_1  /*0x70*/, 0x2C);

		/*set the modem parameters according to the excel calculator(parameters: 9.6 kbps, deviation:
		 45 kHz, channel filter BW:
		112.1 kHz*/

		si4432.writeReg(Si4432_Reg_IF_FIlter_Bandwidth /*0x1C*/, 0x05); //write 0x05 to the IF Filter Bandwidth register
		si4432.writeReg(Si4432_Reg_Clock_Recovery_Oversamping_Rate/*0x20*/, 0xA1);//write 0xA1 to the Clock Recovery Oversampling Ratio register
		si4432.writeReg(Si4432_Reg_Clock_Recovery_Offset_2 /*0x21*/, 0x20);	//write 0x20 to the Clock Recovery Offset 2 register
		si4432.writeReg(Si4432_Reg_Clock_Recovery_Offset_1/*0x22*/, 0x4E); //write 0x4E to the Clock Recovery Offset 1 register
		si4432.writeReg(Si4432_Reg_Clock_Recovery_Offset_0 /*0x23*/, 0xA5); //write 0xA5 to the Clock Recovery Offset 0 register
		si4432.writeReg(Si4432_Reg_Clock_Recovery_Timing_Loop_Gain_1 /*0x24*/, 0x00);//write 0x00 to the Clock Recovery Timing Loop Gain 1 register
		si4432.writeReg(Si4432_Reg_Clock_Recovery_Timing_Loop_Gain_0 /*0x25*/, 0x13);//write 0x13 to the Clock Recovery Timing Loop Gain 0 register
		si4432.writeReg(Si4432_Reg_AFC_Loop_Gearshift_Override /*0x1D*/, 0x40);//write 0x40 to the AFC Loop Gearshift Override register
		si4432.writeReg(Si4432_Reg_Frequency_Deviation/*0x72*/, 0x48);//write 0x1F to the Frequency Deviation register
		//set preamble detection threshold to 20bits
		si4432.writeReg(Si4432_Reg_Preamble_Detection_Ctrl/*0x35*/, 0x28); //write 0x28 to the Preamble Detection Control  register
		//Disable header bytes; set variable packet length (the length of the payload is defined by the
		//received packet length field of the packet); set the synch word to two bytes long
		si4432.writeReg(Si4432_Reg_Header_Ctrl_2 /*0x33*/, 0x02);//write 0x02 to the Header Control2 register
		//Set the sync word pattern to 0x2DD4
		si4432.writeReg(Si4432_Reg_Synchronization_Word_3/*0x36*/, 0x2D);//write 0x2D to the Sync Word 3 register
		si4432.writeReg(Si4432_Reg_Synchronization_Word_2/*0x37*/, 0xD4);//write 0xD4 to the Sync Word 2 registe
		//enable the TX & RX packet handler and CRC-16 (IBM) check
		si4432.writeReg(Si4432_Reg_Data_Access_Ctrl/*0x30*/, 0x8D);//write 0x8D to the Data Access Control register
		//Disable the receive header filters
		si4432.writeReg(Si4432_Reg_Header_Ctrl_1 /*0x32*/, 0x00 );//write 0x00 to the Header Control1 register
		//enable FIFO mode and GFSK modulation
		si4432.writeReg(Si4432_Reg_Modulation_Mode_Ctrl_2 /*0x71*/, 0x63);//write 0x63 to the Modulation Mode Control 2 registe
/*
		SpiWriteRegister(0x0C, 0x12);//write 0x12 to the GPIO1 Configuration(set the TX state)
		SpiWriteRegister(0x0D, 0x15);//write 0x15 to the GPIO2 Configuration(set the RX state)
*/

		si4432.writeReg(Si4432_Reg_GPIO_Config_0/*0x0B*/, 0x12);// настройка GPIO0 для включения режима передачи TX
		si4432.writeReg(Si4432_Reg_GPIO_Config_1 /*0x0C*/, 0x15);//настройка GPIO1 для включения режима приема RX

		/*set the non-default Si4432 registers*/
		//set the VCO and PLL
		si4432.writeReg(0x5A, 0x7F);//write 0x7F to the VCO Current Trimming register
		si4432.writeReg(0x58, 0x80); //write 0x80 to the ChargepumpCurrentTrimmingOverride register
		si4432.writeReg(0x59, 0x40);//write 0x40 to the Divider Current Trimming register
		//set the AGC
		si4432.writeReg(0x6A, 0x0B);//write 0x0B to the AGC Override 2 register
		//set ADC reference voltage to 0.9V
		si4432.writeReg(0x68, 0x04);//write 0x04 to the Deltasigma ADC Tuning 2 register
		si4432.writeReg(0x1F, 0x03);//write 0x03 to the Clock Recovery Gearshift Override register
		//set  Crystal Oscillator Load Capacitance register
		si4432.writeReg(0x09, 0xD7);	//write 0xD7 to the Crystal Oscillator Load Capacitance register

		/*
		 * 000 +1 dBm
		 * 001 +2 dBm
		 * 010 +5 dBm
		 * 011 +8 dBm
		 * 100 +11 dBm
		 * 101 +14 dBm
		 * 110 +17 dBm
		 * 111 +20 dBm
		 */

		//Настройка мощности передатчика 10 mW
		set_tx_power(power_11dBm);
		/*
			reg_tmp =  si4432_spi_readReg(Si4432_Reg_TX_Power);
			reg_tmp &=(0b11111100);
			reg_tmp |=(0b00000000);
			si4432_spi_writeReg(Si4432_Reg_TX_Power, reg_tmp);
		 */

}



bool init_with_parameters(tRfSettings setting, tRF_Power tx_power){
	static unsigned char reg_tmp;
	si4432.power_safe_mode(true);
	vTaskDelay(configTICK_RATE_HZ/2);
	si4432.power_safe_mode(false);
	vTaskDelay(configTICK_RATE_HZ/2);
	xSemaphoreTake(si4432.IQR_Semaphore, 1/*portMAX_DELAY*/ );


			reg_tmp =  si4432.readReg(Si4432_Reg_Interrupt_Status_1);
			reg_tmp =  si4432.readReg(Si4432_Reg_Interrupt_Status_2);
			//чтение статусов
			si4432.writeReg(Si4432_Reg_Operating_Mode_1, Si4432_Mode1_Soft_Reset);
			if (xSemaphoreTake(si4432.IQR_Semaphore, 1500)== pdTRUE)//���� 3 ������� ���������� Si4432
			  {//дождались
			 	 volatile int a;
			 	 a++;
			  }
			 else {// не дождались
				 	 return false;
				 }

			//чтение статусов
			reg_tmp =  si4432.readReg(Si4432_Reg_Interrupt_Status_1);
			reg_tmp =  si4432.readReg(Si4432_Reg_Interrupt_Status_2);
			//установка центральной частоты 433,92 МГц
			si4432.writeReg(Si4432_Reg_Frequency_Band_Select/*0x75*/, 0x53);
			si4432.writeReg(Si4432_Reg_Nominal_Carrier_Frequency_H /*0x76*/, 0x62);
			si4432.writeReg(Si4432_Reg_Nominal_Carrier_Frequency_L /*0x77*/, 0x00);
			SetRfParameters(setting);

			//set preamble detection threshold to 20bits

			si4432.writeReg(Si4432_Reg_Preamble_Detection_Ctrl/*0x35*/, 0x28); //write 0x28 to the Preamble Detection Control  register
					//Disable header bytes; set variable packet length (the length of the payload is defined by the
					//received packet length field of the packet); set the synch word to two bytes long
					si4432.writeReg(Si4432_Reg_Header_Ctrl_2 /*0x33*/, 0x02);//write 0x02 to the Header Control2 register
					//Set the sync word pattern to 0x2DD4
					si4432.writeReg(Si4432_Reg_Synchronization_Word_3/*0x36*/, 0x2D);//write 0x2D to the Sync Word 3 register
					si4432.writeReg(Si4432_Reg_Synchronization_Word_2/*0x37*/, 0xD4);//write 0xD4 to the Sync Word 2 registe
					//enable the TX & RX packet handler and CRC-16 (IBM) check
					si4432.writeReg(Si4432_Reg_Data_Access_Ctrl/*0x30*/, 0x8D);//write 0x8D to the Data Access Control register
					//Disable the receive header filters
					si4432.writeReg(Si4432_Reg_Header_Ctrl_1 /*0x32*/, 0x00 );//write 0x00 to the Header Control1 register
					//enable FIFO mode and GFSK modulation
					si4432.writeReg(Si4432_Reg_Modulation_Mode_Ctrl_2 /*0x71*/, 0x63);//write 0x63 to the Modulation Mode Control 2 registe
			/*
					SpiWriteRegister(0x0C, 0x12);//write 0x12 to the GPIO1 Configuration(set the TX state)
					SpiWriteRegister(0x0D, 0x15);//write 0x15 to the GPIO2 Configuration(set the RX state)
			*/

					si4432.writeReg(Si4432_Reg_GPIO_Config_0/*0x0B*/, 0x12);// ��������� GPIO0 ��� ��������� ������ �������� TX
					si4432.writeReg(Si4432_Reg_GPIO_Config_1 /*0x0C*/, 0x15);//��������� GPIO1 ��� ��������� ������ ������ RX

					/*set the non-default Si4432 registers*/
					//set the VCO and PLL
					si4432.writeReg(0x5A, 0x7F);//write 0x7F to the VCO Current Trimming register
					si4432.writeReg(0x58, 0x80); //write 0x80 to the ChargepumpCurrentTrimmingOverride register
					si4432.writeReg(0x59, 0x40);//write 0x40 to the Divider Current Trimming register
					//set the AGC
					si4432.writeReg(0x6A, 0x0B);//write 0x0B to the AGC Override 2 register
					//set ADC reference voltage to 0.9V
					si4432.writeReg(0x68, 0x04);//write 0x04 to the Deltasigma ADC Tuning 2 register
					si4432.writeReg(0x1F, 0x03);//write 0x03 to the Clock Recovery Gearshift Override register
					//set  Crystal Oscillator Load Capacitance register
					si4432.writeReg(0x09, 0xD7);	//write 0xD7 to the Crystal Oscillator Load Capacitance register

					/*
					 * 000 +1 dBm
					 * 001 +2 dBm
					 * 010 +5 dBm
					 * 011 +8 dBm
					 * 100 +11 dBm
					 * 101 +14 dBm
					 * 110 +17 dBm
					 * 111 +20 dBm
					 */

					//Настройка мощности передатчика 10 mW
					set_tx_power(tx_power);
					return true;


}


void set_tx_power(tRF_Power power_level){
	unsigned char reg_tmp;
	reg_tmp =  si4432.readReg(Si4432_Reg_TX_Power);
	reg_tmp &=(0b11111000);
	reg_tmp |=(unsigned char)power_level;
	si4432.writeReg(Si4432_Reg_TX_Power, reg_tmp);
}


bool TxBuf(unsigned char *pBuf, unsigned char bytes){
volatile bool result = false;
//установка отклонения частоты (+-45 кГц) (необходимый параметр при GFSK модуляции)
//si4432_spi_writeReg(Si4432_Reg_Frequency_Deviation /*0x72*/, 0x48);
	/*Настройка содержания пакета*/
	 //установка длины передаваемых данных - 8 байт
	si4432.writeReg(Si4432_Reg_Packet_Length/*0x3E*/, bytes);//запись в регистр длины передаваемых данных
	//заполнение FIFO передаваемыми данными
	si4432.writeBuf(Si4432_Reg_FIFO_Access, pBuf, bytes);

	//for(i=0;i<bytes;i++)
	//	     si4432_spi_writeReg(Si4432_Reg_FIFO_Access/*0x7F*/, pBuf[i]);

	//Отключение всех прерываний кроме прерывания по отправке пакета
	 //Данное действие сделано для извещения МК о успешной отправке
	si4432.writeReg(Si4432_Reg_Interrupt_Enable_1 /*0x05*/,Si4432_IT1_Packet_Sent /* 0x04*/);
	si4432.writeReg(Si4432_Reg_Interrupt_Enable_2/*0x06*/, 0x00);
	 //Чтение регистров статусов прерываний, для очистки текущих прерываний и сброса NIRQ в лог. 1
	si4432.readReg(Si4432_Reg_Interrupt_Status_1);
	si4432.readReg(Si4432_Reg_Interrupt_Status_2);
	/*активация передатчика*/
	 //При этом радиомодуль формирует пакет и отправляет его автоматически
	si4432.writeReg(Si4432_Reg_Operating_Mode_1/*0x07*/, 0x09);
	 //Ожидание прерывания по отправке
	if (xSemaphoreTake(si4432.IQR_Semaphore, 1500/*portMAX_DELAY*/ )== pdTRUE)//ждем 3 секунды прерывание Si4432
	  {//дождались
	 	 result = true;
	  }
	 else {//не дождались
		 result = false;
		 }
	 //Чтение регистров статусов прерываний, для очистки текущих прерываний и сброса NIRQ в лог. 1
	si4432.readReg(Si4432_Reg_Interrupt_Status_1);
	si4432.readReg(Si4432_Reg_Interrupt_Status_2);
	return result;

}


bool  Rx(unsigned char *pBuf, unsigned char* pBytes, portTickType timeout){
volatile bool result = false;
unsigned char ItStatus1,ItStatus2;
//установка отклонения частоты (+-45 кГц) (необходимый параметр при GFSK модуляции)
////////	si4432_spi_writeReg(Si4432_Reg_Frequency_Deviation /*0x72*/, 0x48);
//si4432_spi_writeReg(Si4432_Reg_Frequency_Deviation/*0x72*/, 0x1F);
//Включаем два прерывания:
//1) прерывание по приему валидного пакета;
//2) прерывание по ошибке CRC.
//	Активация приемника:
si4432.writeReg(Si4432_Reg_Operating_Mode_1/*0x07*/, 0x05);//запись 0x05 в Operating Function Control 1 регистр

	si4432.writeReg(Si4432_Reg_Interrupt_Enable_1/*0x05*/, Si4432_IT1_Valid_Packed_Received | Si4432_IT1_CRC_Error);
	si4432.writeReg(Si4432_Reg_Interrupt_Enable_2/*0x06*/, /*Si4432_IT2_Valid_Preamble_Detected | */ /*Si4432_IT2_Invalid_Preamble_Detected*/ 0x00);
	 //Чтение регистров статусов прерываний, для очистки текущих прерываний и сброса NIRQ в лог. 1
	si4432.readReg(Si4432_Reg_Interrupt_Status_1);
	si4432.readReg(Si4432_Reg_Interrupt_Status_2);
	if (xSemaphoreTake(si4432.IQR_Semaphore, timeout/*portMAX_DELAY*/ )== pdTRUE)//���� 3 ������� ���������� Si4432
	  {//дождались
		//Чтение регистров статусов прерываний
		ItStatus1=si4432.readReg(Si4432_Reg_Interrupt_Status_1);
		ItStatus2=si4432.readReg(Si4432_Reg_Interrupt_Status_2);

		if((ItStatus1 & Si4432_IT1_CRC_Error) == Si4432_IT1_CRC_Error )//произошла ошибка CRC
		{
			 //отключение приёмника
			si4432.writeReg(Si4432_Reg_Operating_Mode_1/*0x07*/, 0x01);//запись 0x01 в Operating Function Control 1 регистр
			//сброс RX FIFO
			si4432.writeReg(Si4432_Reg_Operating_Mode_2 /*0x08*/, 0x02);//запись 0x02 в Operating Function Control 2 регистр
			si4432.writeReg(Si4432_Reg_Operating_Mode_2 /*0x08*/, 0x00);//запись 0x00 в Operating Function Control 2 регистр
			//включение приёмника
//			si4432_spi_writeReg(Si4432_Reg_Operating_Mode_1/*0x07*/, 0x05);//������ 0x05 � Operating Function Control 1 �������
			result = false;
		}
		if((ItStatus1 & Si4432_IT1_Valid_Packed_Received) == Si4432_IT1_Valid_Packed_Received)////принят валидный пакет
		{
			 //отключение передатчика
			si4432.writeReg(Si4432_Reg_Operating_Mode_1/*0x07*/, 0x01);//запись 0x01 в Operating Function Control 1 регистр
			//Чтение длины принятых данных
			*pBytes=si4432.readReg(Si4432_Reg_Receive_Packet_Length/*0x4B*/);//чтение регистра Received Packet Length

//		for(i=0;i<*pBytes;i++)
//			pBuf[i]=si4432_spi_readReg(Si4432_Reg_FIFO_Access /*0x7F*/);//чтение FIFO Access регистра
			si4432.readBuf(Si4432_Reg_FIFO_Access,pBuf,*pBytes);
		//сброс RX FIFO
		si4432.writeReg(Si4432_Reg_Operating_Mode_2 /*0x08*/, 0x02);//������ 0x02 � Operating Function Control 2 �������
		si4432.writeReg(Si4432_Reg_Operating_Mode_2 /*0x08*/, 0x00);//������ 0x00 � Operating Function Control 2 �������
		//включение приёмника
//		si4432_spi_writeReg(Si4432_Reg_Operating_Mode_1/*0x07*/, 0x05);//������ 0x05 � Operating Function Control 1 �������
			result = true;

		}


	  }
	 else {// не дождались
		 result = false;
		 }


return result;

}

RF_ENUM SetRfParameters(tRfSettings setting){
	si4432.writeReg(Si4432_Reg_IF_FIlter_Bandwidth,RMRfSettings[setting][0]);
	si4432.writeReg(Si4432_Reg_Clock_Recovery_Oversamping_Rate, RMRfSettings[setting][1]);
	si4432.writeReg(Si4432_Reg_Clock_Recovery_Offset_2,RMRfSettings[setting][2]);
	si4432.writeReg(Si4432_Reg_Clock_Recovery_Offset_1, RMRfSettings[setting][3]);
	si4432.writeReg(Si4432_Reg_Clock_Recovery_Offset_0, RMRfSettings[setting][4]);
	si4432.writeReg(Si4432_Reg_Clock_Recovery_Timing_Loop_Gain_1 , RMRfSettings[setting][5]);
	si4432.writeReg(Si4432_Reg_Clock_Recovery_Timing_Loop_Gain_0 , RMRfSettings[setting][6]);
	si4432.writeReg(Si4432_Reg_Tx_Data_Rate_1, RMRfSettings[setting][7]);
	si4432.writeReg(Si4432_Reg_Tx_Data_Rate_0, RMRfSettings[setting][8]);
	si4432.writeReg(Si4432_Reg_Modulation_Mode_Ctrl_1, RMRfSettings[setting][9]);
	si4432.writeReg(Si4432_Reg_Frequency_Deviation , RMRfSettings[setting][10]);
	si4432.writeReg(Si4432_Reg_AFC_Loop_Gearshift_Override 	 , RMRfSettings[setting][11]);
	return RF_OK;
}

unsigned char get_RSSI(void){

	return  si4432.readReg(Si4432_Reg_Received_Signal_Strength_Indicator );
}



uint16_t generatePackageId()
{
	uint16_t id = ( xTaskGetTickCount()) & 0xFFFF;
	if (id == 0 || id == 1 || id == 2)
		id = 3;
	return id;
	/*
static volatile uint16_t last_ID = 3;
	if (last_ID==0xFFFF) last_ID = 3;
	return last_ID++;
	*/
}


void get_uid(uint32_t *text)
{
	text[0]= UID[0];
	text[1]= UID[1];
	text[2]= UID[2];
}


void get_net_adrress(DeviceAddress* address){


	get_uid(uid);
	unsigned char* pTmp =  (unsigned char*)&uid[2];
	address->address[2] =*pTmp;
	pTmp++;
	address->address[1] = *pTmp;
	pTmp++;
	address->address[0] = *pTmp;
/*
	address->address[2] =2;
	address->address[1] =1;
	address->address[0] =1;
*/


}





bool package_already_received(PackageId packageID){

	bool result;
	std::vector<PackageId>::iterator findIter = std::find( m_lastReceivedIds.begin(),  m_lastReceivedIds.end(), packageID);
	result = (findIter!=m_lastReceivedIds.end());
	return result;


}


void lastReceivedIdsAdd(PackageId packageID){

	if (m_lastReceivedIds.size() >= last_packages_max_size){
		m_lastReceivedIds.erase(m_lastReceivedIds.begin());
	}
		m_lastReceivedIds.push_back(packageID);
}



bool package_for_me(DeviceAddress address){

	return (net_address == address);

}



void remove_waiting_package(PackageId packageID){
	for (auto it = m_waitingPackages.begin(); it != m_waitingPackages.end(); it++)
		{
			if ((*it).package.details.packageId == packageID){
				m_waitingPackages.erase(it);
				return;
			}
	}

}


uint16_t random16(void){
	return 0xFFFF & random32();
 }
 uint32_t random32(void){
	 m_lastSeed = 0xFFFFFFFF & ( (m_lastSeed+1) * ( xTaskGetTickCount() & 0xFFFF));
	 return m_lastSeed;

 }
uint32_t random(uint32_t maxValue){
	 return (uint32_t) round((( (float) random32()) / UINT32_MAX * maxValue));
}
uint32_t m_lastSeed = 0;

xQueueHandle xRfRxEventQueue;
//xQueueHandle xRfRxUniquePackagesQueue;
xSemaphoreHandle tx_buff_Semaphore;
uint32_t rx_counter = 0;



DeviceAddress net_address;
private:
Si4432<SPIx> si4432;

//std::map<PackageId, WaitingPackage> m_waitingPackages;

std::vector<WaitingPackage> m_waitingPackages;
std::vector<PackageId> m_lastReceivedIds;

unsigned long *UID = (unsigned long *)0x1FFFF7E8;

uint32_t uid[3];


DeviceAddress server_address = {1,1,2};

protected:


};


template<class SPIx>
RfModul<SPIx>* RfModul<SPIx>::self_pointer;
template<class SPIx>
tPackageTimings RfModul<SPIx>::timings_to_send;

#endif
