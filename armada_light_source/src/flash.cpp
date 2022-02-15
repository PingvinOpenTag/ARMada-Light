/*
 * flash.cpp
 *
 *      Author: chubara_av
 */

#include <stdint.h>
#include <stdlib.h>
#include "stm32f10x.h"

#include "flash.h"


uint8_t flash_ready(void) {
return !(FLASH->SR & FLASH_SR_BSY);
}


void flash_unlock(void) {
FLASH->KEYR = FLASH_KEY1;
FLASH->KEYR = FLASH_KEY2;
}

void flash_lock(void) {
FLASH->CR |= FLASH_CR_LOCK;
}



void flash_erase_all_pages(void) {
FLASH->CR |= FLASH_CR_MER;
FLASH->CR |= FLASH_CR_STRT;
while(!flash_ready()){};
FLASH->CR &= FLASH_CR_MER;
}



void flash_erase_page(uint32_t address) {
FLASH->CR|= FLASH_CR_PER;
FLASH->AR = address;
FLASH->CR|= FLASH_CR_STRT;
while(!flash_ready()){};
FLASH->CR&= ~FLASH_CR_PER;
}



void flash_write(uint32_t address,uint32_t data) {
FLASH->CR |= FLASH_CR_PG;
while(!flash_ready());{}
*(__IO uint16_t*)address = (uint16_t)data;
while(!flash_ready()){};
address+=2;
data>>=16;
*(__IO uint16_t*)address = (uint16_t)data;
FLASH->CR &= ~(FLASH_CR_PG);
}



void flash_Write_alternative(const void * Src, void * Address, uint32_t Length)
{
    flash_unlock();

    FLASH->CR |= FLASH_CR_PG;

    while(FLASH->SR & FLASH_SR_BSY) {}



    {
        uint32_t * S = (uint32_t *)Src;
        uint32_t * D = (uint32_t *)Address;

        uint32_t   L = (Length / sizeof(uint32_t)) + 1;
        uint32_t   i;

        for(i = 0; i < L; i++)
        {
            D[i] = S[i];


            while(FLASH->SR & FLASH_SR_BSY) {}
        }
    }


    FLASH->CR &= ~FLASH_CR_PG;

    flash_lock();
}



void Internal_Flash_Erase(unsigned int pageAddress) {
	while (FLASH->SR & FLASH_SR_BSY);
	/*
	if (FLASH->SR & FLASH_SR_EOP) {
		FLASH->SR = FLASH_SR_EOP;
	}
*/
	FLASH->CR |= FLASH_CR_PER;
	FLASH->AR = pageAddress;
	FLASH->CR |= FLASH_CR_STRT;
	while (!(FLASH->SR & FLASH_SR_EOP));
	FLASH->SR = FLASH_SR_EOP;
	FLASH->CR &= ~FLASH_CR_PER;
}

void Internal_Flash_Write(unsigned char* data, unsigned int address, unsigned int count) {
	unsigned int i;

	while (FLASH->SR & FLASH_SR_BSY);
	if (FLASH->SR & FLASH_SR_EOP) {
		FLASH->SR = FLASH_SR_EOP;
	}

	FLASH->CR |= FLASH_CR_PG;

	for (i = 0; i < count; i += 2) {
		*(volatile unsigned short*)(address + i) = (((unsigned short)data[i + 1]) << 8) + data[i];
		while (!(FLASH->SR & FLASH_SR_EOP));
		FLASH->SR = FLASH_SR_EOP;
	}

	FLASH->CR &= ~(FLASH_CR_PG);
}


