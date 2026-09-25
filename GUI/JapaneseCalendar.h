#ifndef __JAPANESE_CALENDAR_H
#define __JAPANESE_CALENDAR_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    JP_HOLIDAY_NONE = 0,
    JP_HOLIDAY_NEW_YEAR,
    JP_HOLIDAY_COMING_OF_AGE,
    JP_HOLIDAY_FOUNDATION,
    JP_HOLIDAY_EMPEROR_BIRTHDAY,
    JP_HOLIDAY_VERNAL_EQUINOX,
    JP_HOLIDAY_SHOWA,
    JP_HOLIDAY_CONSTITUTION,
    JP_HOLIDAY_GREENERY,
    JP_HOLIDAY_CHILDREN,
    JP_HOLIDAY_MARINE,
    JP_HOLIDAY_MOUNTAIN,
    JP_HOLIDAY_RESPECT_FOR_AGED,
    JP_HOLIDAY_AUTUMNAL_EQUINOX,
    JP_HOLIDAY_SPORTS,
    JP_HOLIDAY_CULTURE,
    JP_HOLIDAY_LABOR_THANKSGIVING,
    JP_HOLIDAY_ENTHRONEMENT,
    JP_HOLIDAY_ENTHRONEMENT_CEREMONY,
    JP_HOLIDAY_CITIZENS,
    JP_HOLIDAY_SUBSTITUTE,
} jp_holiday_id_t;

typedef struct {
    jp_holiday_id_t id;
    const char* name;
} jp_holiday_t;

bool jp_is_leap_year(uint16_t year);
uint8_t jp_days_in_month(uint16_t year, uint8_t month);
uint8_t jp_day_of_week(uint16_t year, uint8_t month, uint8_t day); /* 0=Sun */
uint8_t jp_iso_week_number(uint16_t year, uint8_t month, uint8_t day);
uint8_t jp_calendar_rows(uint16_t year, uint8_t month, uint8_t week_start);
uint8_t jp_reiwa_year(uint16_t year, uint8_t month, uint8_t day); /* 0 before Reiwa */
jp_holiday_t jp_get_holiday(uint16_t year, uint8_t month, uint8_t day);
const char* jp_eto_stem(uint16_t year);
const char* jp_eto_branch(uint16_t year);
const char* jp_eto_animal(uint16_t year);

#endif
