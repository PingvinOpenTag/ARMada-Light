#include "statistics.h"
#include "w25x.h"

void erase_statistics(void){

	SPI_Flash_Erase_Sector(STATISTICS_SECTOR);
}


void write_stat_item(StatisticsItem& stat_item, uint16_t index){
	SPI_Flash_Write_Page((uint8_t*) &stat_item, STATISTICS_ADDRESS + sizeof(StatisticsItem)*index,(uint16_t) sizeof(StatisticsItem));
}


void read_stat_item(StatisticsItem& stat_item, uint16_t index){
	SPI_Flash_Read_With_DMA((uint8_t*) &stat_item, STATISTICS_ADDRESS + sizeof(StatisticsItem)*index,(uint16_t) sizeof(StatisticsItem));
}

uint16_t get_first_free_cell(void){

	uint16_t result = 0xffff;
	StatisticsItem stat_itm_tmp;
	for (uint16_t i=0; i < SPI_FLASH_SECTOR_SIZE/ sizeof(StatisticsItem); i++){
		read_stat_item(stat_itm_tmp,i);
		if(stat_itm_tmp.marker == 0xff) return i;
	}
	return result;
}

uint16_t get_first_free_cell_recursive(uint16_t begin, uint16_t end){
	StatisticsItem stat_itm_tmp;
	uint16_t delta;
	read_stat_item(stat_itm_tmp, begin);
	if (stat_itm_tmp.marker == 0xff) return begin;
	read_stat_item(stat_itm_tmp, end);
	if(stat_itm_tmp.marker != 0xff) return 0xffff;
	if(!(end>begin)) return 0xffff;
	delta = (end - begin)/2;
	if (delta==0){
		for (uint16_t i=begin; i <=end; i++){
				read_stat_item(stat_itm_tmp,i);
				if(stat_itm_tmp.marker == 0xff) return i;
			}
			return 0xffff;
	}
	read_stat_item(stat_itm_tmp, begin+delta);
	if (stat_itm_tmp.marker == 0xff) return get_first_free_cell_recursive(begin, begin + delta);
	else return get_first_free_cell_recursive(begin + delta, end);
}
