#ifndef __JAPANESE_ROKUYO_H
#define __JAPANESE_ROKUYO_H

#include <stdint.h>
#include "Lunar.h"

typedef enum {
    JP_ROKUYO_TAIAN = 0,
    JP_ROKUYO_SHAKKO,
    JP_ROKUYO_SENSHO,
    JP_ROKUYO_TOMOBIKI,
    JP_ROKUYO_SENBU,
    JP_ROKUYO_BUTSUMETSU,
    JP_ROKUYO_NONE
} jp_rokuyo_t;

jp_rokuyo_t jp_rokuyo_from_lunar(const struct Lunar_Date* lunar);
jp_rokuyo_t jp_get_rokuyo(uint16_t year, uint8_t month, uint8_t day);
const char* jp_rokuyo_name(jp_rokuyo_t rokuyo);

#endif
