#ifndef _STATISTICS_H
#define _STATISTICS_H

#include <stdint.h>
#include <stdlib.h>
#include "w25x.h"

enum sending_status {
	DELIVERED = 0x77,
	WAITING_FOR_DELIVERY = 0xff
};

using SendingStatus = sending_status;

#pragma pack(push, 1)
struct StatisticsItem {
	uint8_t marker;
	uint8_t player_id;
	uint8_t team_color:3;
	uint8_t damage:4;
	bool is_fatal:1;
	bool is_delivery;

};
#pragma pack(pop)



using StatItem = StatisticsItem;



constexpr static   uint32_t STATISTICS_SECTOR = 2047;//последний сектор
constexpr static   uint32_t STATISTICS_ADDRESS =STATISTICS_SECTOR*SPI_FLASH_SECTOR_SIZE;//последний сектор


void erase_statistics(void);
void write_stat_item(StatisticsItem& stat_item, uint16_t index);
void read_stat_item(StatisticsItem& stat_item, uint16_t index);
uint16_t get_first_free_cell(void);
uint16_t get_first_free_cell_recursive(uint16_t begin, uint16_t end);


#endif
