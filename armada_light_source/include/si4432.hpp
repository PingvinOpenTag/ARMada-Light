#ifndef _SI4432_HPP
#define _SI4432_HPP


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

#include "spi.hpp"
#include "pin.h"





constexpr static uint8_t Si4432_Reg_Device_Type_Code				= 		0x00;
constexpr static uint8_t Si4432_Reg_Verson_Code 					=		0x01;
constexpr static uint8_t Si4432_Reg_Device_Status 					=		0x02;
constexpr static uint8_t Si4432_Reg_Interrupt_Status_1 				=		0x03;
constexpr static uint8_t Si4432_Reg_Interrupt_Status_2 				=		0x04;
constexpr static uint8_t Si4432_Reg_Interrupt_Enable_1 				=		0x05;
constexpr static uint8_t Si4432_Reg_Interrupt_Enable_2 				=		0x06;
constexpr static uint8_t Si4432_Reg_Operating_Mode_1				=		0x07;
constexpr static uint8_t Si4432_Reg_Operating_Mode_2 				=		0x08;
constexpr static uint8_t Si4432_Reg_Crystal_Load_Capacitance	 	=		0x09;
constexpr static uint8_t Si4432_Reg_MCU_Output_Clock 				=		0x0A;
constexpr static uint8_t Si4432_Reg_GPIO_Config_0 					=		0x0B;
constexpr static uint8_t Si4432_Reg_GPIO_Config_1 					=		0x0C;
constexpr static uint8_t Si4432_Reg_GPIO_Config_2 					=		0x0D;
constexpr static uint8_t Si4432_Reg_IO_Port_Config 					=		0x0E;
constexpr static uint8_t Si4432_Reg_ADC_Config 						=		0x0F;
constexpr static uint8_t Si4432_Reg_ADC_Amplifier_Offset 			=		0x10;
constexpr static uint8_t Si4432_Reg_ADC_Value 						=		0x11;
constexpr static uint8_t Si4432_Reg_Temperature_Sensor_Calibration  =		0x12;
constexpr static uint8_t Si4432_Reg_Temperature_Value_Offset 		=		0x13;
constexpr static uint8_t Si4432_Reg_Wakeup_Timer_Period_1 			=		0x14;
constexpr static uint8_t Si4432_Reg_Wakeup_Timer_Period_2 			=		0x15;
constexpr static uint8_t Si4432_Reg_Wakeup_Timer_Period_3 			=		0x16;
constexpr static uint8_t Si4432_Reg_Wakeup_Timer_Value_1 			=		0x17;
constexpr static uint8_t Si4432_Reg_Wakeup_Timer_Value_2 			=		0x18;
constexpr static uint8_t Si4432_Reg_Low_Duty_Cycle_Mode_Duration 	=		0x19;
constexpr static uint8_t Si4432_Reg_Low_Batter_Detector_Threshold 	=		0x1A;
constexpr static uint8_t Si4432_Reg_Battery_Voltage_Level 			=		0x1B;
constexpr static uint8_t Si4432_Reg_IF_FIlter_Bandwidth 			=		0x1C;
constexpr static uint8_t Si4432_Reg_AFC_Loop_Gearshift_Override 	=		0x1D;
constexpr static uint8_t Si4432_Reg_AFC_Timing_Ctrl 				=		0x1E;
constexpr static uint8_t Si4432_Reg_Clock_Recovery_Gearshift_Override =		0x1F;
constexpr static uint8_t Si4432_Reg_Clock_Recovery_Oversamping_Rate =		0x20;
constexpr static uint8_t Si4432_Reg_Clock_Recovery_Offset_2 		=		0x21;
constexpr static uint8_t Si4432_Reg_Clock_Recovery_Offset_1 		=		0x22;
constexpr static uint8_t Si4432_Reg_Clock_Recovery_Offset_0 		=		0x23;
constexpr static uint8_t Si4432_Reg_Clock_Recovery_Timing_Loop_Gain_1 =		0x24;
constexpr static uint8_t Si4432_Reg_Clock_Recovery_Timing_Loop_Gain_0 =		0x25;
constexpr static uint8_t Si4432_Reg_Received_Signal_Strength_Indicator =	0x26;
constexpr static uint8_t Si4432_Reg_RSSI_Threshold_For_Clear_Channel_Indicator  =	0x27;
constexpr static uint8_t Si4432_Reg_Antenna_Diversity_1 						=	0x28;
constexpr static uint8_t Si4432_Reg_Antenna_Diversity_2 						=	0x29;
constexpr static uint8_t Si4432_Reg_AFC_Limiter 								=	0x2A;
constexpr static uint8_t Si4432_Reg_ADC_Correction 							=	0x2B;
constexpr static uint8_t Si4432_Reg_OOK_Counter_Value_1 					=	0x2C;
constexpr static uint8_t Si4432_Reg_OOK_Counter_Value_2 					=	0x2D;
constexpr static uint8_t Si4432_Reg_Slicer_Peak_Holder 						=	0x2E;
constexpr static uint8_t Si4432_Reg_Data_Access_Ctrl 						=	0x30;
constexpr static uint8_t Si4432_Reg_EZMAC_Status 							=	0x31;
constexpr static uint8_t Si4432_Reg_Header_Ctrl_1 							=	0x32;
constexpr static uint8_t Si4432_Reg_Header_Ctrl_2 							=	0x33;
constexpr static uint8_t Si4432_Reg_Preamble_Lenth 							=	0x34;
constexpr static uint8_t Si4432_Reg_Preamble_Detection_Ctrl 				=	0x35;
constexpr static uint8_t Si4432_Reg_Synchronization_Word_3 					=	0x36;
constexpr static uint8_t Si4432_Reg_Synchronization_Word_2					=	0x37;
constexpr static uint8_t Si4432_Reg_Synchronization_Word_1 					=	0x38;
constexpr static uint8_t Si4432_Reg_Synchronization_Word_0 					=	0x39;
constexpr static uint8_t Si4432_Reg_Transmit_Header_3 						=	0x3A;
constexpr static uint8_t Si4432_Reg_Transmit_Header_2 						=	0x3B;
constexpr static uint8_t Si4432_Reg_Transmit_Header_1 						=	0x3C;
constexpr static uint8_t Si4432_Reg_Transmit_Header_0 						=	0x3D;
constexpr static uint8_t Si4432_Reg_Packet_Length 							=	0x3E;
constexpr static uint8_t Si4432_Reg_Check_Header_3 							=	0x3F;
constexpr static uint8_t Si4432_Reg_Check_Header_2 							=	0x40;
constexpr static uint8_t Si4432_Reg_Check_Header_1 							=	0x41;
constexpr static uint8_t Si4432_Reg_Check_Header_0 							=	0x42;
constexpr static uint8_t Si4432_Reg_Header_Enable_3 						=	0x43;
constexpr static uint8_t Si4432_Reg_Header_Enable_2 						=	0x44;
constexpr static uint8_t Si4432_Reg_Header_Enable_1 						=	0x45;
constexpr static uint8_t Si4432_Reg_Header_Enable_0 						=	0x46;
constexpr static uint8_t Si4432_Reg_Received_Header_3 						=	0x47;
constexpr static uint8_t Si4432_Reg_Received_Header_2 						=	0x48;
constexpr static uint8_t Si4432_Reg_Received_Header_1 						=	0x49;
constexpr static uint8_t Si4432_Reg_Received_Header_0 						=	0x4A;
constexpr static uint8_t Si4432_Reg_Receive_Packet_Length 					=	0x4B;
constexpr static uint8_t Si4432_Reg_ADC8_Ctrl 								=	0x4F;
constexpr static uint8_t Si4432_Reg_Channel_Filter_Coefficient_Offset 		=	0x60;
constexpr static uint8_t Si4432_Reg_Crystal_And_Power_On_Reset_Ctrl 		=	0x62;
constexpr static uint8_t Si4432_Reg_AGC_Override_1 							=	0x69;
constexpr static uint8_t Si4432_Reg_TX_Power 								=	0x6D;
constexpr static uint8_t Si4432_Reg_Tx_Data_Rate_1 							=	0x6E;
constexpr static uint8_t Si4432_Reg_Tx_Data_Rate_0 							=	0x6F;
constexpr static uint8_t Si4432_Reg_Modulation_Mode_Ctrl_1 					=	0x70;
constexpr static uint8_t Si4432_Reg_Modulation_Mode_Ctrl_2 					=	0x71;
constexpr static uint8_t Si4432_Reg_Frequency_Deviation 					=	0x72;
constexpr static uint8_t Si4432_Reg_Frequency_Offset_1 						=	0x73;
constexpr static uint8_t Si4432_Reg_Frequency_Offset_2 						=	0x74;
constexpr static uint8_t Si4432_Reg_Frequency_Band_Select 					=	0x75;
constexpr static uint8_t Si4432_Reg_Nominal_Carrier_Frequency_H 			=	0x76;
constexpr static uint8_t Si4432_Reg_Nominal_Carrier_Frequency_L 			=	0x77;
constexpr static uint8_t Si4432_Reg_Frequency_Hopping_Channel_Select 		=	0x79;
constexpr static uint8_t Si4432_Reg_Frequency_Hopping_Step_Size 			=	0x7A;
constexpr static uint8_t Si4432_Reg_Tx_FIFO_Ctrl_1 							=	0x7C;
constexpr static uint8_t Si4432_Reg_Tx_FIFO_Ctrl_2 							=	0x7D;
constexpr static uint8_t Si4432_Reg_Rx_FIFO_Ctrl 							=	0x7E;
constexpr static uint8_t Si4432_Reg_FIFO_Access 							=	0x7F;



constexpr static uint8_t Si4432_IT1_FIFO_Over_And_Under_Flow 		=		0x80;
constexpr static uint8_t Si4432_IT1_Tx_FIFO_Almost_Full 			=		0x40;
constexpr static uint8_t Si4432_IT1_Tx_FIFO_Almost_Empty 			=		0x20;
constexpr static uint8_t Si4432_IT1_Rx_FIFO_Almost_Full 			=		0x10;
constexpr static uint8_t Si4432_IT1_External_Interrupt 				=		0x08;
constexpr static uint8_t Si4432_IT1_Packet_Sent 					=		0x04;
constexpr static uint8_t Si4432_IT1_Valid_Packed_Received 			=		0x02;
constexpr static uint8_t Si4432_IT1_CRC_Error 						=		0x01;
constexpr static uint8_t Si4432_IT1_Disable 						=		0x00;
constexpr static uint8_t Si4432_IT2_Sync_Word_Detected 				=		0x80;
constexpr static uint8_t Si4432_IT2_Valid_Preamble_Detected 		=		0x40;
constexpr static uint8_t Si4432_IT2_Invalid_Preamble_Detected 		=		0x20;
constexpr static uint8_t Si4432_IT2_RSSI 							=		0x10;
constexpr static uint8_t Si4432_IT2_Wakeup_Timer 					=		0x08;
constexpr static uint8_t Si4432_IT2_Low_Battery_Detected 			=		0x04;
constexpr static uint8_t Si4432_IT2_Chip_Ready 						=		0x02;
constexpr static uint8_t Si4432_IT2_POR 							=		0x01;
constexpr static uint8_t Si4432_IT2_Disable 						=		0x00;
constexpr static uint8_t Si4432_Mode1_Soft_Reset					=		0x80;
constexpr static uint8_t Si4432_Mode1_Low_Battery_Detect 			=		0x40;
constexpr static uint8_t Si4432_Mode1_Wakeup_Timer 					=		0x20;
constexpr static uint8_t Si4432_Mode1_Crystal_32K 					=		0x00;
constexpr static uint8_t Si4432_Mode1_Crystal_RC 					=		0x10;
constexpr static uint8_t Si4432_Mode1_Tx_On 						=		0x08;
constexpr static uint8_t Si4432_Mode1_Rx_On 						=		0x04;
constexpr static uint8_t Si4432_Mode1_Tune 							=		0x02;
constexpr static uint8_t Si4432_Mode1_Ready 						=		0x01;
constexpr static uint8_t Si4432_Mode2_Rx_Multi_Packet 				=		0x10;
constexpr static uint8_t Si4432_Mode2_Auto_Transmission 			=		0x08;
constexpr static uint8_t Si4432_Mode2_Low_Duty_Cycle 				=		0x04;
constexpr static uint8_t Si4432_Mode2_Rx_FIFO_Reset 				=		0x02;
constexpr static uint8_t Si4432_Mode2_Tx_FIFO_Reset 				=		0x01;
constexpr static uint8_t Si4432_MCU_0_Cycles 						=		0x00;
constexpr static uint8_t Si4432_MCU_128_Cycles 						=		0x10;
constexpr static uint8_t Si4432_MCU_256_Cycles 						=		0x20;
constexpr static uint8_t Si4432_MCU_512_Cycles 						=		0x30;
constexpr static uint8_t Si4432_MCU_Enable_Low_Frequency_Clock 		=		0x80;
constexpr static uint8_t Si4432_MCU_Clock_30Mhz 					=		0x00;
constexpr static uint8_t Si4432_MCU_Clock_15Mhz 					=		0x01;
constexpr static uint8_t Si4432_MCU_Clock_10Mhz 					=		0x02;
constexpr static uint8_t Si4432_MCU_Clock_4Mhz 						=		0x03;
constexpr static uint8_t Si4432_MCU_Clock_3Mhz 						=		0x04;
constexpr static uint8_t Si4432_MCU_Clock_2Mhz 						=		0x05;
constexpr static uint8_t Si4432_MCU_Clock_1Mhz 						=		0x06;
constexpr static uint8_t Si4432_MCU_Clock_32khz 					=		0x07;
constexpr static uint8_t Si4432_MCU_Clear 							=		0x00;
constexpr static uint8_t  Si4432_GPIO_Pullup 						=		0x20;
constexpr static uint8_t Si4432_GPIO_Power_On_Reset 				=		0x00;
constexpr static uint8_t  Si4432_GPIO_Wakeup_Timer 					=		0x01;
constexpr static uint8_t Si4432_GPIO_Low_Battery_Detect 			=		0x02;
constexpr static uint8_t  Si4432_GPIO_Digital_Input		 			=		0x03;
constexpr static uint8_t  Si4432_GPIO_EXIT_Falling 					=		0x04;
constexpr static uint8_t  Si4432_GPIO_EXIR_Rising 					=		0x05;
constexpr static uint8_t  Si4432_GPIO_EXIT_Chang 					=		0x06;
constexpr static uint8_t  Si4432_GPIO_Analog_Input 					=		0x07;
constexpr static uint8_t  Si4432_GPIO_R_Analog_Test_N_In 			=		0x08;
constexpr static uint8_t  Si4432_GPIO_R_Analog_Test_P_In 			=		0x09;
constexpr static uint8_t  Si4432_GPIO_Digital_Output 				=		0x0A;
constexpr static uint8_t  Si4432_GPIO_R_Digital_Test 				=		0x0B;
constexpr static uint8_t  Si4432_GPIO_R_Analog_Test_N_Out 			=		0x0C;
constexpr static uint8_t  Si4432_GPIO_R_Analog_Test_P_Out 			=		0x0D;
constexpr static uint8_t  Si4432_GPIO_Reference_Voltage 			=		0x0E;
constexpr static uint8_t  Si4432_GPIO_Tx_Rx_Clock_Output 			=		0x0F;
constexpr static uint8_t  Si4432_GPIO_Tx_Data_Input 				=		0x10;
constexpr static uint8_t  Si4432_GPIO_External_Retransmission 		=		0x11;
constexpr static uint8_t  Si4432_GPIO_Tx_State 						=		0x12;
constexpr static uint8_t  Si4432_GPIO_Tx_FIFO_Almost_Full 			=		0x13;
constexpr static uint8_t  Si4432_GPIO_Rx_Data_Out 					=		0x14;
constexpr static uint8_t  Si4432_GPIO_Rx_State 						=		0x15;
constexpr static uint8_t  Si4432_GPIO_Rx_FIFO_Almost_Full 			=		0x16;
constexpr static uint8_t  Si4432_GPIO_Antenna1 						=		0x17;
constexpr static uint8_t  Si4432_GPIO_Antenna2 						=		0x18;
constexpr static uint8_t  Si4432_GPIO_Valid_Preamble_Detected 		=		0x19;
constexpr static uint8_t  Si4432_GPIO_Invalid_Preamble_Detected 	=		0x1A;
constexpr static uint8_t  Si4432_GPIO_Sync_Word_Detected 			=		0x1B;
constexpr static uint8_t  Si4432_GPIO_Clear_Channel_Assessment 		=		0x1C;
constexpr static uint8_t  Si4432_GPIO_VDD 							=		0x1D;
constexpr static uint8_t  Si4432_GPIO_GND 							=		0x1E;
constexpr static uint8_t  Si4432_GPIO_Clear 						=		0x00;

template<class SPIx>

class Si4432: public  PinIRQ {
public:
	Si4432(uint8_t nSEL_PORT, uint8_t nSEL_PIN,
			uint8_t SDN_PORT, uint8_t SDN_PIN,
			uint8_t nIQR_PORT, uint8_t nIQR_PIN):
		nSEL(nSEL_PORT,nSEL_PIN ),
		SDN(SDN_PORT, SDN_PIN),
		nIQR(nIQR_PORT, nIQR_PIN)

	{
		vSemaphoreCreateBinary(IQR_Semaphore);
		nSEL.switchToOutput();
		SDN.switchToOutput();
		SDN.set();
		nIQR.switchToInput();
		nIQR.setCallback((TPIN_IRQ_CALLBACK) this);
		nIQR.enableExti(true, EXTI_Trigger_Falling);
	}


void power_safe_mode(bool enable){
	if(enable) SDN.set();
	else SDN.reset();

}

unsigned char readWriteReg(unsigned char reg, unsigned char value)
	{
	        unsigned char status;
	        nSEL.reset();
	        // select register
	        status = spi.readWrite(reg);
	        // set value
	        spi.readWrite(value);
	        nSEL.set();
	        return(status);
	}


unsigned char readReg(unsigned char reg)
{
    unsigned char reg_val;
    nSEL.reset();
    spi.readWrite(reg);
	reg_val = spi.readWrite(0xAA);
    nSEL.set();
	return(reg_val);
}



void writeReg(unsigned char reg, unsigned char value)
{
        nSEL.reset();
        // select register
        spi.readWrite(reg| 0x80);
        // set value
        spi.readWrite(value);
        nSEL.set();
}

void readBuf(unsigned char reg,unsigned char *pBuf, unsigned char bytes)
{
        unsigned char i;
        nSEL.reset();
        // Select register to write to and read status byte
        spi.readWrite(reg);
        for(i=0;i<bytes;i++)
        	pBuf[i] = spi.readWrite(0xAA);
        nSEL.set();
}

void writeBuf(unsigned char reg, unsigned char *pBuf, unsigned char bytes)
{
        unsigned char i;
        nSEL.reset();
        // Select register to write to and read status byte
        spi.readWrite(reg| 0x80);
        for(i=0; i<bytes; i++) // then write all byte in buffer(*pBuf)
        	spi.readWrite(*pBuf++);
        nSEL.set();
}


void	pin_irq_callback(void) override{
	static portBASE_TYPE xHigherPriorityTaskWoken;
	xHigherPriorityTaskWoken = pdFALSE;
	xSemaphoreGiveFromISR( IQR_Semaphore, &xHigherPriorityTaskWoken);
	 if (xHigherPriorityTaskWoken != pdFALSE)
	              {
	                  portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
	              }

}


public:
xSemaphoreHandle IQR_Semaphore;



private:
//Spi_1 spi;
SPIx spi;
Pin nSEL;
Pin SDN;
Pin nIQR;

protected:



};




#ifdef __cplusplus
 extern "C" {
  #endif






#ifdef __cplusplus
  }
#endif



#endif
