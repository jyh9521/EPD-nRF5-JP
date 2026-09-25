#include "JapaneseRokuyo.h"

#include "JapaneseCalendar.h"

/* (old-calendar month + day) % 6: 0 大安, 1 赤口, 2 先勝,
 * 3 友引, 4 先負, 5 仏滅. A leap month retains its preceding
 * month's number, but its day count restarts at one. */
jp_rokuyo_t jp_rokuyo_from_lunar(const struct Lunar_Date* lunar) {
    if (lunar == 0 || lunar->Month < 1U || lunar->Month > 12U ||
        lunar->Date < 1U || lunar->Date > 30U)
        return JP_ROKUYO_NONE;
    return (jp_rokuyo_t)((lunar->Month + lunar->Date) % 6U);
}

jp_rokuyo_t jp_get_rokuyo(uint16_t year, uint8_t month, uint8_t day) {
    struct Lunar_Date lunar;
    /* The existing lookup tables are populated for solar years 2000-2050. */
    if (year < 2000U || year > 2050U || day > jp_days_in_month(year, month))
        return JP_ROKUYO_NONE;
    LUNAR_SolarToLunar(&lunar, year, month, day);
    return jp_rokuyo_from_lunar(&lunar);
}

const char* jp_rokuyo_name(jp_rokuyo_t rokuyo) {
    static const char names[6][7] = {"大安", "赤口", "先勝", "友引", "先負", "仏滅"};
    return (unsigned)rokuyo < 6U ? names[rokuyo] : "";
}
