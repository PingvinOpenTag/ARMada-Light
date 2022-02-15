/*
 * This file is part of the µOS++ distribution.
 *   (https://github.com/micro-os-plus)
 * Copyright (c) 2014 Liviu Ionescu.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

// ----------------------------------------------------------------------------



//#include "Timer.h"
//#include "BlinkLed.h"
#include <map>
#include <string>

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



// ----------------------------------------------------------------------------
//
// Standalone STM32F1 led blink sample (trace via DEBUG).
//
// In debug configurations, demonstrate how to print a greeting message
// on the trace device. In release configurations the message is
// simply discarded.
//
// Then demonstrates how to blink a led with 1 Hz, using a
// continuous loop and SysTick delays.
//
// Trace support is enabled by adding the TRACE macro definition.
// By default the trace messages are forwarded to the DEBUG output,
// but can be rerouted to any device or completely suppressed, by
// changing the definitions required in system/src/diag/trace_impl.c
// (currently OS_USE_TRACE_SEMIHOSTING_DEBUG/_STDOUT).
//
// The external clock frequency is specified as a preprocessor definition
// passed to the compiler via a command line option (see the 'C/C++ General' ->
// 'Paths and Symbols' -> the 'Symbols' tab, if you want to change it).
// The value selected during project creation was HSE_VALUE=8000000.
//
// Note: The default clock settings take the user defined HSE_VALUE and try
// to reach the maximum possible system clock. For the default 8 MHz input
// the result is guaranteed, but for other values it might not be possible,
// so please adjust the PLL settings in system/src/cmsis/system_stm32f10x.c
//

// Definitions visible only within this translation unit.
namespace
{
  // ----- Timing definitions -------------------------------------------------

  // Keep the LED on for 2/3 of a second.
//  constexpr Timer::ticks_t BLINK_ON_TICKS = Timer::FREQUENCY_HZ * 3 / 4;
 // constexpr Timer::ticks_t BLINK_OFF_TICKS = Timer::FREQUENCY_HZ
 //     - BLINK_ON_TICKS;
}

// ----- main() ---------------------------------------------------------------

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "diag/Trace.h"
#include "keyboard.h"
#include "wavplayer.h"
#include "ir.h"
#include "adc.h"

#include <pin.h>
#include "bluetooth.h"
#include "system.h"
#include "w25x.h"
#include "wavplayer.h"
#include <si4432.hpp>
#include <rf_modul.hpp>
#include <spi.hpp>
//#include "ili9163.hpp"
//#include "uart.hpp"
#include "statistics.h"

#ifdef __cplusplus
 extern "C" {
  #endif

// Подключаем FreeRTOS
#include "FreeRTOS.h"
// из всех возможностей используем пока только переключение задач
#include "task.h"
//#include "hd44780_driver.h"

 #ifdef __cplusplus
   }
 #endif
/*
extern unsigned char MSG1[];
extern const unsigned char MSG2[];
extern const unsigned char MSG3[];
extern const unsigned char MSG4[];



 extern const unsigned char CGRAM1[];
 extern const unsigned char CGRAM2[];
 extern const unsigned char CGRAM3[];
*/
#define NUM_LEDS    4
 RGB_t leds[NUM_LEDS];


/*
 const unsigned char MSG1[]="Winstar ";
 const unsigned char MSG2[]="display ";
 const unsigned char MSG3[]="OLED SPI";
 const unsigned char MSG4[]="WEH0802A";
*/

 volatile uint32_t id = 0;
 volatile uint8_t sr;


#ifdef rf_enable
   RfModul<Spi_1> rf_modul(PORTC,PIN5,PORTC,PIN15,PORTB,PIN0);
#endif
   	System sys;




   	//  	Uart uart;



 //  SSD1283<Spi_3> disp(PORTB,PIN9,PORTB,PIN8,PORTB,PIN7, PORTC, PIN10);


int

main(int argc, char* argv[])
{




		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
//		AFIO->MAPR|=AFIO_MAPR_SWJ_CFG_JTAGDISABLE; //��������� JTAG, SWD �������
		AFIO->MAPR|= AFIO_MAPR_SWJ_CFG_DISABLE;/*!< JTAG-DP Disabled and SW-DP Disabled */

		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); //��� ���� �


		init_SPI2();
		SPI_FLASH_CS_H();
		SPI_FLASH_CS_L();
		SPI_FLASH_CS_H();
		uint8_t reg1;

			reg1 = (uint8_t) SPI_Flash_ReadSR();
			reg1 &=0b11100011;
			SPI_Flash_WriteSR(reg1);





	vTaskStartScheduler();

  // Infinite loop
  while (1)
    {

    }
  // Infinite loop, never return.

}


#ifdef __cplusplus
 extern "C" {
  #endif
void DMA2_Channel2_IRQHandler(void)
{
}
#ifdef __cplusplus
  }
#endif








#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
