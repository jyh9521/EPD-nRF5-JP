#include "JapaneseCalendar.h"

#include <stddef.h>

typedef struct {
    uint16_t year;
    uint8_t month;
    uint8_t day;
} jp_date_t;

static const char* const HOLIDAY_NAMES[] = {
    "",         "元日",     "成人",     "建国記念", "天皇誕生", "春分",     "昭和",
    "憲法記念", "みどり",   "こども",   "海の日",   "山の日",   "敬老",     "秋分",
    "スポーツ", "文化",     "勤労感謝", "即位",     "即位礼",   "国民休日", "振替休日",
};

bool jp_is_leap_year(uint16_t year) {
    return (year % 400U == 0U) || ((year % 4U == 0U) && (year % 100U != 0U));
}

uint8_t jp_days_in_month(uint16_t year, uint8_t month) {
    static const uint8_t DAYS[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month < 1U || month > 12U) return 0;
    if (month == 2U && jp_is_leap_year(year)) return 29;
    return DAYS[month - 1U];
}

static bool valid_date(uint16_t year, uint8_t month, uint8_t day) {
    uint8_t max_day = jp_days_in_month(year, month);
    return year >= 1U && max_day != 0U && day >= 1U && day <= max_day;
}

uint8_t jp_day_of_week(uint16_t year, uint8_t month, uint8_t day) {
    static const uint8_t OFFSETS[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
    uint16_t y = year;
    if (!valid_date(year, month, day)) return 0;
    if (month < 3U) y--;
    return (uint8_t)((y + y / 4U - y / 100U + y / 400U + OFFSETS[month - 1U] + day) % 7U);
}

static uint16_t day_of_year(uint16_t year, uint8_t month, uint8_t day) {
    uint16_t result = day;
    uint8_t m;
    for (m = 1U; m < month; m++) result += jp_days_in_month(year, m);
    return result;
}

static uint8_t iso_weeks_in_year(uint16_t year) {
    uint8_t jan1 = jp_day_of_week(year, 1, 1);
    return (jan1 == 4U || (jan1 == 3U && jp_is_leap_year(year))) ? 53U : 52U;
}

uint8_t jp_iso_week_number(uint16_t year, uint8_t month, uint8_t day) {
    int16_t week;
    uint8_t weekday;
    if (!valid_date(year, month, day)) return 0;
    weekday = jp_day_of_week(year, month, day);
    weekday = weekday == 0U ? 7U : weekday; /* ISO Monday=1 ... Sunday=7 */
    week = (int16_t)((int16_t)day_of_year(year, month, day) - weekday + 10) / 7;
    if (week < 1) return iso_weeks_in_year((uint16_t)(year - 1U));
    if (week > iso_weeks_in_year(year)) return 1U;
    return (uint8_t)week;
}

uint8_t jp_calendar_rows(uint16_t year, uint8_t month, uint8_t week_start) {
    uint8_t first;
    uint8_t days;
    if (week_start > 6U) return 0;
    days = jp_days_in_month(year, month);
    if (days == 0U) return 0;
    first = (uint8_t)((jp_day_of_week(year, month, 1U) + 7U - week_start) % 7U);
    return (uint8_t)((first + days + 6U) / 7U);
}

uint8_t jp_reiwa_year(uint16_t year, uint8_t month, uint8_t day) {
    if (!valid_date(year, month, day)) return 0;
    if (year < 2019U || (year == 2019U && (month < 5U || (month == 5U && day < 1U)))) return 0;
    return (uint8_t)(year - 2018U);
}

static uint8_t nth_weekday(uint16_t year, uint8_t month, uint8_t weekday, uint8_t nth) {
    uint8_t first = jp_day_of_week(year, month, 1);
    return (uint8_t)(1U + ((weekday + 7U - first) % 7U) + 7U * (nth - 1U));
}

static uint8_t vernal_equinox_day(uint16_t year) {
    uint16_t delta;
    if (year < 1980U || year > 2099U) return 0;
    delta = (uint16_t)(year - 1980U);
    return (uint8_t)((208431UL + 2422UL * delta - 10000UL * (delta / 4U)) / 10000UL);
}

static uint8_t autumnal_equinox_day(uint16_t year) {
    uint16_t delta;
    if (year < 1980U || year > 2099U) return 0;
    delta = (uint16_t)(year - 1980U);
    return (uint8_t)((232488UL + 2422UL * delta - 10000UL * (delta / 4U)) / 10000UL);
}

static jp_holiday_id_t base_holiday(uint16_t year, uint8_t month, uint8_t day) {
    if (!valid_date(year, month, day)) return JP_HOLIDAY_NONE;

    /* One-off holidays and Olympic holiday moves. */
    if (year == 2019U && month == 5U && day == 1U) return JP_HOLIDAY_ENTHRONEMENT;
    if (year == 2019U && month == 10U && day == 22U) return JP_HOLIDAY_ENTHRONEMENT_CEREMONY;
    if (year == 2020U) {
        if (month == 7U && day == 23U) return JP_HOLIDAY_MARINE;
        if (month == 7U && day == 24U) return JP_HOLIDAY_SPORTS;
        if (month == 8U && day == 10U) return JP_HOLIDAY_MOUNTAIN;
    }
    if (year == 2021U) {
        if (month == 7U && day == 22U) return JP_HOLIDAY_MARINE;
        if (month == 7U && day == 23U) return JP_HOLIDAY_SPORTS;
        if (month == 8U && day == 8U) return JP_HOLIDAY_MOUNTAIN;
    }

    if (month == 1U && day == 1U) return JP_HOLIDAY_NEW_YEAR;
    if (month == 1U && year >= 2000U && day == nth_weekday(year, month, 1U, 2U))
        return JP_HOLIDAY_COMING_OF_AGE;
    if (month == 2U && day == 11U && year >= 1967U) return JP_HOLIDAY_FOUNDATION;
    if (month == 2U && day == 23U && year >= 2020U) return JP_HOLIDAY_EMPEROR_BIRTHDAY;
    if (month == 3U && day == vernal_equinox_day(year)) return JP_HOLIDAY_VERNAL_EQUINOX;
    if (month == 4U && day == 29U && year >= 2007U) return JP_HOLIDAY_SHOWA;
    if (month == 5U && day == 3U) return JP_HOLIDAY_CONSTITUTION;
    if (month == 5U && day == 4U && year >= 2007U) return JP_HOLIDAY_GREENERY;
    if (month == 5U && day == 5U) return JP_HOLIDAY_CHILDREN;
    if (month == 7U && year >= 2003U && year != 2020U && year != 2021U &&
        day == nth_weekday(year, month, 1U, 3U))
        return JP_HOLIDAY_MARINE;
    if (month == 8U && day == 11U && year >= 2016U && year != 2020U && year != 2021U)
        return JP_HOLIDAY_MOUNTAIN;
    if (month == 9U && year >= 2003U && day == nth_weekday(year, month, 1U, 3U))
        return JP_HOLIDAY_RESPECT_FOR_AGED;
    if (month == 9U && day == autumnal_equinox_day(year)) return JP_HOLIDAY_AUTUMNAL_EQUINOX;
    if (month == 10U && year >= 2000U && year != 2020U && year != 2021U &&
        day == nth_weekday(year, month, 1U, 2U))
        return JP_HOLIDAY_SPORTS;
    if (month == 11U && day == 3U) return JP_HOLIDAY_CULTURE;
    if (month == 11U && day == 23U) return JP_HOLIDAY_LABOR_THANKSGIVING;
    return JP_HOLIDAY_NONE;
}

static void previous_day(jp_date_t* date) {
    if (date->day > 1U) {
        date->day--;
        return;
    }
    if (date->month > 1U) {
        date->month--;
    } else {
        date->year--;
        date->month = 12U;
    }
    date->day = jp_days_in_month(date->year, date->month);
}

static void next_day(jp_date_t* date) {
    if (date->day < jp_days_in_month(date->year, date->month)) {
        date->day++;
        return;
    }
    date->day = 1U;
    if (date->month < 12U) {
        date->month++;
    } else {
        date->year++;
        date->month = 1U;
    }
}

static bool is_citizens_holiday(uint16_t year, uint8_t month, uint8_t day) {
    jp_date_t before = {year, month, day};
    jp_date_t after = before;
    if (year < 1986U || base_holiday(year, month, day) != JP_HOLIDAY_NONE) return false;
    previous_day(&before);
    next_day(&after);
    return base_holiday(before.year, before.month, before.day) != JP_HOLIDAY_NONE &&
           base_holiday(after.year, after.month, after.day) != JP_HOLIDAY_NONE;
}

static bool is_holiday_before_substitute(uint16_t year, uint8_t month, uint8_t day) {
    return base_holiday(year, month, day) != JP_HOLIDAY_NONE || is_citizens_holiday(year, month, day);
}

static bool is_substitute_holiday(uint16_t year, uint8_t month, uint8_t day) {
    jp_date_t cursor = {year, month, day};
    uint8_t checked = 0;
    if (year < 1973U || is_holiday_before_substitute(year, month, day)) return false;
    previous_day(&cursor);

    if (year < 2007U) {
        return base_holiday(cursor.year, cursor.month, cursor.day) != JP_HOLIDAY_NONE &&
               jp_day_of_week(cursor.year, cursor.month, cursor.day) == 0U;
    }

    while (checked++ < 7U && is_holiday_before_substitute(cursor.year, cursor.month, cursor.day)) {
        if (base_holiday(cursor.year, cursor.month, cursor.day) != JP_HOLIDAY_NONE &&
            jp_day_of_week(cursor.year, cursor.month, cursor.day) == 0U)
            return true;
        previous_day(&cursor);
    }
    return false;
}

jp_holiday_t jp_get_holiday(uint16_t year, uint8_t month, uint8_t day) {
    jp_holiday_t result = {JP_HOLIDAY_NONE, HOLIDAY_NAMES[JP_HOLIDAY_NONE]};
    result.id = base_holiday(year, month, day);
    if (result.id == JP_HOLIDAY_NONE && is_citizens_holiday(year, month, day)) result.id = JP_HOLIDAY_CITIZENS;
    if (result.id == JP_HOLIDAY_NONE && is_substitute_holiday(year, month, day)) result.id = JP_HOLIDAY_SUBSTITUTE;
    if (result.id != JP_HOLIDAY_NONE) result.name = HOLIDAY_NAMES[result.id];
    return result;
}
