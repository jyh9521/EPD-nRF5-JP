#include <stdio.h>
#include "JapaneseRokuyo.h"

int main(void) {
    unsigned year, month, day;
    while (scanf("%u-%u-%u", &year, &month, &day) == 3) {
        struct Lunar_Date lunar;
        jp_solar_to_japanese_lunar(&lunar, (uint16_t)year, (uint8_t)month, (uint8_t)day);
        printf("%04u-%02u-%02u,%u,%u,%u,%u\n", year, month, day,
               lunar.Year, lunar.Month, lunar.Date, lunar.IsLeap);
    }
    return 0;
}
