#ifndef _ADC_H
#define _ADC_H

#include <stdint.h>
#include <stdlib.h>
#include <string>
#include "stm32f10x.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_adc.h"
#include "pin.h"
#include "usart.h"
#include "parser.h"



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




 class ADC {
 public:
 	ADC();
 	void init(void);
 	int value(void);
 	int voltage(void);

 private:

 protected:


 };





#endif
