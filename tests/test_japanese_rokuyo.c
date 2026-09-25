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
    {2026, 10, 10, 2026, 8, 30, 0, JP_ROKUYO_SENSHO},
    {2026, 10, 11, 2026, 9, 1, 0, JP_ROKUYO_SENBU},
    {2027, 2, 6, 2026, 12, 30, 0, JP_ROKUYO_TAIAN},
    {2027, 2, 7, 2027, 1, 1, 0, JP_ROKUYO_SENSHO},
};

int main(void) {
    unsigned i;
    puts("Gregorian,ExpectedLunar,ExpectedLeap,ExpectedRokuyo,ActualLunar,ActualLeap,ActualRokuyo,Result");
    for (i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i) {
        const case_t* expected = &cases[i];
        struct Lunar_Date lunar;
        jp_rokuyo_t actual;
        int pass;
        jp_solar_to_japanese_lunar(&lunar, expected->year, expected->month, expected->day);
        actual = jp_get_rokuyo(expected->year, expected->month, expected->day);
        pass = lunar.Year == expected->lunar_year && lunar.Month == expected->lunar_month &&
            lunar.Date == expected->lunar_day && lunar.IsLeap == expected->leap &&
            actual == expected->rokuyo;
        printf("%04u-%02u-%02u,%u-%02u-%02u,%u,%s,%u-%02u-%02u,%u,%s,%s\n",
               expected->year, expected->month, expected->day,
               expected->lunar_year, expected->lunar_month, expected->lunar_day,
               expected->leap, jp_rokuyo_name(expected->rokuyo),
               lunar.Year, lunar.Month, lunar.Date, lunar.IsLeap,
               jp_rokuyo_name(actual), pass ? "PASS" : "FAIL");
        if (!pass) {
            fprintf(stderr, "%04u-%02u-%02u: lunar %u/%u/%u leap=%u, rokuyo=%s\n",
                    expected->year, expected->month, expected->day, lunar.Year, lunar.Month,
                    lunar.Date, lunar.IsLeap, jp_rokuyo_name(actual));
            return 1;
        }
    }
    assert(strcmp(jp_rokuyo_name(JP_ROKUYO_TAIAN), "大安") == 0);
    assert(jp_get_rokuyo(2051, 1, 1) == JP_ROKUYO_NONE);
    assert(jp_get_rokuyo(2026, 2, 30) == JP_ROKUYO_NONE);
    {
        static const jp_rokuyo_t FIRST_DAY[12] = {
            JP_ROKUYO_SENSHO, JP_ROKUYO_TOMOBIKI, JP_ROKUYO_SENBU,
            JP_ROKUYO_BUTSUMETSU, JP_ROKUYO_TAIAN, JP_ROKUYO_SHAKKO,
            JP_ROKUYO_SENSHO, JP_ROKUYO_TOMOBIKI, JP_ROKUYO_SENBU,
            JP_ROKUYO_BUTSUMETSU, JP_ROKUYO_TAIAN, JP_ROKUYO_SHAKKO,
        };
        uint8_t month;
        for (month = 1U; month <= 12U; ++month) {
            struct Lunar_Date first = {0U, 1U, month, 2026U};
            assert(jp_rokuyo_from_lunar(&first) == FIRST_DAY[month - 1U]);
        }
    }
    puts("JapaneseRokuyo tests: PASS (14 dated cases, 12 lunar first days)");
    return 0;
}
