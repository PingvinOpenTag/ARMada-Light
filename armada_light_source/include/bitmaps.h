#ifndef _BITMAPS_H
#define _BITMAPS_H

#include <stdint.h>
#include <stdlib.h>

extern const uint16_t batt_long_level_0[0x500];//18x64
extern const uint16_t batt_long_level_1[0x500];//18x64
extern const uint16_t batt_long_level_2[0x500];//18x64
extern const uint16_t batt_long_level_3[0x500];//18x64
extern const uint16_t batt_long_level_4[0x500];//18x64
extern const uint16_t batt_long_level_5[0x500];//18x64

extern const uint16_t gerb_pic[0x4000];
extern const uint16_t guns[0x4000];
extern const uint16_t bluetooth1[0x200];//16x32 pixels
extern const uint16_t bullet[0x540]; //64x21
extern const uint16_t clip_2[0x8C0];//35x64 pixels

extern const uint16_t heart_1[0xA50];
extern const uint16_t heart_2[0xA50];
extern const uint16_t heart_3[0xA50];
extern const uint16_t heart_4[0xA50];

extern const uint16_t game_over_pic[0x4000];//128x128 pixels
extern const uint16_t key_pic[0x2000];//128x64 pixels
#endif
