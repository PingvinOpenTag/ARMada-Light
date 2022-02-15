///////////////////////////////////
/// CritialSection
#include "os-wrappers.hpp"

#ifdef __cplusplus
	extern "C" {
#endif

#include "FreeRTOS.h"
#include "task.h"
#include "mcu-status.h"
#include "stm32f10x.h"


#ifdef __cplusplus
	}
#endif
#include <stdio.h>
#include <stdlib.h>
#include <stm32f1xx.h>
#include <stdlib.h>
#include <cstdio>


//#include "cmsis_os.h"


	char isInsideInterrupt()
	{
		return (SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) != 0 ;
	}



CritialSection::~CritialSection()
{
	if (isLocked())
		unlock();
}

void CritialSection::lock()
{
	if (isInsideInterrupt())
		taskENTER_CRITICAL_FROM_ISR();
	else
		taskENTER_CRITICAL();
	m_isLocked = true;
}

void CritialSection::unlock()
{
	if (!m_isLocked)
		return;
	if (isInsideInterrupt())
		taskEXIT_CRITICAL_FROM_ISR(0);
	else
		taskEXIT_CRITICAL();
	m_isLocked = false;
}

bool CritialSection::isLocked()
{
	return m_isLocked;
}



///////////////////////////////////////
/// new/delete wrappers

void * operator new(std::size_t n)
{
	CritialSection cs; cs.lock();
	//Kernel::heapAllocatedTotal += n;
    return malloc(n);
}
void operator delete(void * p)
{
	CritialSection cs; cs.lock();
    free(p);
}

void *operator new[](std::size_t n)
{
	CritialSection cs; cs.lock();
	//Kernel::heapAllocatedTotal += n;
	return malloc(n);
}

void operator delete[](void *p) throw()
{
	CritialSection cs; cs.lock();
    free(p);
}
