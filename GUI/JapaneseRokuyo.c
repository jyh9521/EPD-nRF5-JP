#include "JapaneseRokuyo.h"

#include "JapaneseCalendar.h"

typedef struct {
    uint16_t start;
    uint16_t end;
    uint8_t month;
    uint8_t day;
    uint8_t leap;
} jp_lunar_window_t;

static const jp_lunar_window_t JP_LUNAR_WINDOWS[] = {
#include "JapaneseLunarCorrections.inc"
};

/* (old-calendar month + day) % 6: 0 大安, 1 赤口, 2 先勝,
 * 3 友引, 4 先負, 5 仏滅. A leap month retains its preceding
 * month's number, but its day count restarts at one. */
jp_rokuyo_t jp_rokuyo_from_lunar(const struct Lunar_Date* lunar) {
    if (lunar == 0 || lunar->Month < 1U || lunar->Month > 12U ||
        lunar->Date < 1U || lunar->Date > 30U)
        return JP_ROKUYO_NONE;
    return (jp_rokuyo_t)((lunar->Month + lunar->Date) % 6U);
}

void jp_solar_to_japanese_lunar(struct Lunar_Date* lunar, uint16_t year, uint8_t month, uint8_t day) {
    uint16_t key;
    uint16_t day_of_year = day;
    uint8_t m;
    unsigned i;
    if (year < 2000U || year > 2050U || day == 0U || day > jp_days_in_month(year, month)) {
        lunar->Year = 0U;
        lunar->Month = 0U;
        lunar->Date = 0U;
        lunar->IsLeap = 0U;
        return;
    }
    LUNAR_SolarToLunar(lunar, year, month, day);
    for (m = 1U; m < month; ++m) day_of_year += jp_days_in_month(year, m);
    key = (uint16_t)((year - 2000U) * 512U + day_of_year);
    for (i = 0; i < sizeof(JP_LUNAR_WINDOWS) / sizeof(JP_LUNAR_WINDOWS[0]); ++i) {
        const jp_lunar_window_t* window = &JP_LUNAR_WINDOWS[i];
        if (key < window->start) break;
        if (key <= window->end) {
            lunar->Month = window->month;
            lunar->Date = (uint8_t)(window->day + key - window->start);
            lunar->IsLeap = window->leap;
            lunar->Year = (uint16_t)(year - ((month <= 3U && window->month >= 10U) ? 1U : 0U));
            break;
        }
    }
}

jp_rokuyo_t jp_get_rokuyo(uint16_t year, uint8_t month, uint8_t day) {
    struct Lunar_Date lunar;
    jp_solar_to_japanese_lunar(&lunar, year, month, day);
    return jp_rokuyo_from_lunar(&lunar);
}

const char* jp_rokuyo_name(jp_rokuyo_t rokuyo) {
    static const char names[6][7] = {"大安", "赤口", "先勝", "友引", "先負", "仏滅"};
    return (unsigned)rokuyo < 6U ? names[rokuyo] : "";
}
