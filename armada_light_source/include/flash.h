#ifndef _FLASH_H
#define _FLASH_H


#define FLASH_KEY1 ((uint32_t)0x45670123)
#define FLASH_KEY2 ((uint32_t)0xCDEF89AB)

uint8_t flash_ready(void);
void flash_unlock(void);
void flash_lock(void);
void flash_erase_all_pages(void);
void flash_erase_page(uint32_t address);
void flash_write(uint32_t address,uint32_t data);
void flash_Write_alternative(const void * Src, void * Address, uint32_t Length);
void Internal_Flash_Erase(unsigned int pageAddress);
void Internal_Flash_Write(unsigned char* data, unsigned int address, unsigned int count);

#endif
