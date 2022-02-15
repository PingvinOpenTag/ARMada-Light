#ifndef _PIN_H
#define _PIN_H
#include <stdint.h>
#include <stdlib.h>
#include <functional>
#include "stm32f10x_gpio.h"

typedef void(*ExtiCallbackFuncPointer)();

        constexpr static uint8_t PORTA = 0;
	constexpr static uint8_t PORTB = 1;
	constexpr static uint8_t PORTC = 2;
	constexpr static uint8_t PORTD = 3;
	constexpr static uint8_t notAPort = 0xFF;


        constexpr static uint8_t PIN0 = 0;
	constexpr static uint8_t PIN1 = 1;
	constexpr static uint8_t PIN2 = 2;
	constexpr static uint8_t PIN3 = 3;
	    constexpr static uint8_t PIN4 = 4;
	constexpr static uint8_t PIN5 = 5;
	constexpr static uint8_t PIN6 = 6;
	constexpr static uint8_t PIN7 = 7;
	constexpr static uint8_t PIN8 = 8;
	constexpr static uint8_t PIN9 = 9;
	constexpr static uint8_t PIN10 = 10;
	constexpr static uint8_t PIN11 = 11;
	constexpr static uint8_t PIN12 = 12;
	constexpr static uint8_t PIN13 = 13;
	constexpr static uint8_t PIN14 = 14;
	constexpr static uint8_t PIN15 = 15;

	constexpr static uint8_t notAPin = 0xFF;
//	class PinIRQ;

	//using TPIN_IRQ_CALLBACK = std::function<void(PinIRQ&)> ;

class PinIRQ{
public:
	PinIRQ(){};
	virtual ~PinIRQ(){};
	virtual void	pin_irq_callback(void)=0;

private:

protected:

	};

	using TPIN_IRQ_CALLBACK = PinIRQ*;

class Pin{

public:
Pin(uint8_t port, uint8_t pin);
void switchToOutput();
void switchToOutputOD();
void switchToInput();
void switchToInputPU();
void set();
void reset();

void enableExti( bool enable, EXTITrigger_TypeDef mode=EXTI_Trigger_Rising_Falling) {
IRQn_Type irq;
EXTI_InitTypeDef exti;
NVIC_InitTypeDef nvic;
uint32_t extiLine;
switch(m_pin)
	{
	default:
		//printf("Attempt to enable EXTI on invalid IOPin object");
                return;
	case 0:
              {
                irq = EXTI0_IRQn;
                extiLine = EXTI_Line0;
              }
              break;
	case 1:
              {
                irq = EXTI1_IRQn;
                extiLine = EXTI_Line1;
              }
              break;
	case 2:
              {
                irq = EXTI2_IRQn;
                extiLine = EXTI_Line2;
              }
              break;
	case 3:
              {
                irq = EXTI3_IRQn;
                extiLine = EXTI_Line3;
              }
              break;
	case 4:
              {
                irq = EXTI4_IRQn;
                extiLine = EXTI_Line4;
              }
              break;
	case 5:
              {
                irq = EXTI9_5_IRQn;
                extiLine = EXTI_Line5;
              }
              break;
        case 6:
             {
                irq = EXTI9_5_IRQn;
                extiLine = EXTI_Line6;
             }
             break;
        case 7:
             {
                irq = EXTI9_5_IRQn;
                extiLine = EXTI_Line7;
             }
             break;

        case 8:
             {
                irq = EXTI9_5_IRQn;
                extiLine = EXTI_Line8;
             }
             break;


        case 9:
             {
		irq = EXTI9_5_IRQn;
                extiLine = EXTI_Line9;
             }
             break;
	case 10:
              {
                irq = EXTI15_10_IRQn;
                extiLine = EXTI_Line10;
              }
              break;
        case 11:
              {
                irq = EXTI15_10_IRQn;
                extiLine = EXTI_Line11;
              }
              break;
        case 12:
              {
                irq = EXTI15_10_IRQn;
                extiLine = EXTI_Line12;
              }
              break;
        case 13:
              {
                irq = EXTI15_10_IRQn;
                extiLine = EXTI_Line13;
              }
              break;
        case 14:
              {
                irq = EXTI15_10_IRQn;
                extiLine = EXTI_Line14;
              }
              break;
        case 15:
              {
		irq = EXTI15_10_IRQn;
                extiLine = EXTI_Line15;
              }
              break;
        }
/* Enable AFIO clock */
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	  /* Connect EXTI7 Line to PC.07 pin */
	GPIO_EXTILineConfig(m_port, m_pin);
	/* Configure EXTI7 line */
	exti.EXTI_Line = extiLine;//EXTI_Line7;
	exti.EXTI_Mode = EXTI_Mode_Interrupt;
	exti.EXTI_Trigger = mode/*EXTI_Trigger_Rising_Falling*//*EXTI_Trigger_Rising*/;
	exti.EXTI_LineCmd = ENABLE;

	EXTI_Init(&exti);

	/* Enable and set EXTI9_5 Interrupt to the lowest priority */
	nvic.NVIC_IRQChannel = irq;//EXTI9_5_IRQn;
	nvic.NVIC_IRQChannelPreemptionPriority = 14;
	nvic.NVIC_IRQChannelSubPriority = 14;
	nvic.NVIC_IRQChannelCmd = ENABLE;

	NVIC_Init(&nvic);


}
void setCallback(TPIN_IRQ_CALLBACK/*ExtiCallbackFuncPointer*/ pointer);
static inline void extiCallback(uint8_t line_number);
bool isSet(void);
private:
uint8_t m_port;
uint8_t m_pin;
//static ExtiCallbackFuncPointer callbecksPointers[4][16];
static TPIN_IRQ_CALLBACK callbecksPointers[4][16];
protected:




};



class Led: private Pin{

public:
Led(uint8_t port, uint8_t pin): Pin(port,pin){switchToOutput();};
void on(){set();};
void off(){reset();};

};




#endif
