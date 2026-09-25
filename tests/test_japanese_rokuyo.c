#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "JapaneseRokuyo.h"

typedef struct {
    uint16_t year;
    uint8_t month, day;
    uint16_t lunar_year;
    uint8_t lunar_month, lunar_day, leap;
    jp_rokuyo_t rokuyo;
} case_t;

/* Independent Japanese-calendar references are recorded in the verification
 * artifact; these values are not derived from the implementation under test. */
static const case_t cases[] = {
    {2023, 3, 21, 2023, 2, 30, 0, JP_ROKUYO_SENSHO},
    {2023, 3, 22, 2023, 2, 1, 1, JP_ROKUYO_TOMOBIKI},
    {2024, 2, 10, 2024, 1, 1, 0, JP_ROKUYO_SENSHO},
    {2026, 2, 16, 2025, 12, 29, 0, JP_ROKUYO_BUTSUMETSU},
    {2026, 2, 17, 2026, 1, 1, 0, JP_ROKUYO_SENSHO},
    {2026, 9, 10, 2026, 7, 29, 0, JP_ROKUYO_TAIAN},
    {2026, 9, 11, 2026, 8, 1, 0, JP_ROKUYO_TOMOBIKI},
    {2026, 9, 25, 2026, 8, 15, 0, JP_ROKUYO_BUTSUMETSU},
    {2026, 9, 30, 2026, 8, 20, 0, JP_ROKUYO_SENBU},
    {2026, 10, 1, 2026, 8, 21, 0, JP_ROKUYO_BUTSUMETSU},
};

int main(void) {
    unsigned i;
    for (i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i) {
        const case_t* expected = &cases[i];
        struct Lunar_Date lunar;
        LUNAR_SolarToLunar(&lunar, expected->year, expected->month, expected->day);
        if (lunar.Year != expected->lunar_year || lunar.Month != expected->lunar_month ||
            lunar.Date != expected->lunar_day || lunar.IsLeap != expected->leap ||
            jp_get_rokuyo(expected->year, expected->month, expected->day) != expected->rokuyo) {
            fprintf(stderr, "%04u-%02u-%02u: lunar %u/%u/%u leap=%u, rokuyo=%s\n",
                    expected->year, expected->month, expected->day, lunar.Year, lunar.Month,
                    lunar.Date, lunar.IsLeap, jp_rokuyo_name(jp_get_rokuyo(expected->year,
                    expected->month, expected->day)));
            return 1;
        }
    }
    assert(strcmp(jp_rokuyo_name(JP_ROKUYO_TAIAN), "大安") == 0);
    assert(jp_get_rokuyo(2051, 1, 1) == JP_ROKUYO_NONE);
    assert(jp_get_rokuyo(2026, 2, 30) == JP_ROKUYO_NONE);
    puts("JapaneseRokuyo tests: PASS (10 dated cases)");
    return 0;
}
