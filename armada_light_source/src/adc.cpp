#include "adc.h"

ADC::ADC(){
	init();

}


void ADC::init(void){
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;

		GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AIN;
		/*
			GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1 ;		// that's ADC1 IN1 (PA1 on STM32)
			GPIO_Init(GPIOA, &GPIO_InitStructure);

			GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2 ;		// that's ADC1 (PÑ2 on STM32)
			GPIO_Init(GPIOC, &GPIO_InitStructure);
*/
			GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0 ;		// that's ADC1 IN2 (PÑ0 on STM32)
			GPIO_Init(GPIOC, &GPIO_InitStructure);


			//clock for ADC (max 14MHz --> 72/6=12MHz)
	//		RCC_ADCCLKConfig (RCC_PCLK2_Div6);
			RCC_ADCCLKConfig (RCC_PCLK2_Div2);
		// enable ADC system clock
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

		//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);
		//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);

		// define ADC config
			ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
			ADC_InitStructure.ADC_ScanConvMode = DISABLE;
			ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;	// we work in continuous sampling mode
			ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
			ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
			ADC_InitStructure.ADC_NbrOfChannel = 1;


		//	ADC_RegularChannelConfig(ADC2,ADC_Channel_1, 1,ADC_SampleTime_28Cycles5); // define regular conversion config
		//	ADC_RegularChannelConfig(ADC3,ADC_Channel_12, 1,ADC_SampleTime_28Cycles5); // define regular conversion config
	//PC0 - battary control
			ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_28Cycles5);


			//	ADC_RegularChannelConfig(ADC1,ADC_Channel_2, 1,ADC_SampleTime_28Cycles5); // define regular conversion config

			ADC_Init ( ADC1, &ADC_InitStructure);	//set config of ADC1
//			ADC_Init ( ADC2, &ADC_InitStructure);	//set config of ADC2
//			ADC_Init ( ADC3, &ADC_InitStructure);	//set config of ADC3

			ADC_TempSensorVrefintCmd(ENABLE);
	/*
			NVIC_EnableIRQ(ADC1_2_IRQn);
			ADC_ITConfig(ADC2, ADC_IT_EOC, ENABLE);
			ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);
	*/

		// enable ADC
			ADC_Cmd (ADC1,ENABLE);	//enable ADC1
//			ADC_Cmd (ADC2,ENABLE);	//enable ADC2
//			ADC_Cmd (ADC3,ENABLE);	//enable ADC3

		//	ADC calibration (optional, but recommended at power on)
			ADC_ResetCalibration(ADC1);	// Reset previous calibration
			while(ADC_GetResetCalibrationStatus(ADC1));
			ADC_StartCalibration(ADC1);	// Start new calibration (ADC must be off at that time)
			while(ADC_GetCalibrationStatus(ADC1));
/*
		//	ADC calibration (optional, but recommended at power on)
			ADC_ResetCalibration(ADC2);	// Reset previous calibration
			while(ADC_GetResetCalibrationStatus(ADC2));
			ADC_StartCalibration(ADC2);	// Start new calibration (ADC must be off at that time)
			while(ADC_GetCalibrationStatus(ADC2));


		//	ADC calibration (optional, but recommended at power on)
			ADC_ResetCalibration(ADC3);	// Reset previous calibration
			while(ADC_GetResetCalibrationStatus(ADC3));
			ADC_StartCalibration(ADC3);	// Start new calibration (ADC must be off at that time)
			while(ADC_GetCalibrationStatus(ADC3));

*/


		// start conversion
			ADC_Cmd (ADC1,ENABLE);	//enable ADC1
			ADC_SoftwareStartConvCmd(ADC1, ENABLE);	// start conversion (will be endless as we are in continuous mode)

/*
		// start conversion
			ADC_Cmd (ADC2,ENABLE);	//enable ADC2
			ADC_SoftwareStartConvCmd(ADC2, ENABLE);	// start conversion (will be endless as we are in continuous mode)


		// start conversion
			ADC_Cmd (ADC3,ENABLE);	//enable ADC3
			ADC_SoftwareStartConvCmd(ADC3, ENABLE);	// start conversion (will be endless as we are in continuous mode)
*/


		// debug information
			RCC_ClocksTypeDef forTestOnly;
			RCC_GetClocksFreq(&forTestOnly);	//this could be used with debug to check to real speed of ADC clock






}



int ADC::value(void){
	return ADC_GetConversionValue(ADC1);
}


int ADC::voltage(void){

return (9900*value())/4096+300;

}
