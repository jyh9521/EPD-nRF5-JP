#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "JapaneseCalendar.h"

static void expect_holiday(uint16_t year, uint8_t month, uint8_t day, jp_holiday_id_t expected) {
    jp_holiday_t holiday = jp_get_holiday(year, month, day);
    if (holiday.id != expected) {
        fprintf(stderr, "%04u-%02u-%02u: expected %d, got %d (%s)\n", year, month, day, expected, holiday.id,
                holiday.name);
        assert(holiday.id == expected);
    }
}

int main(void) {
    assert(jp_is_leap_year(2000));
    assert(jp_is_leap_year(2024));
    assert(!jp_is_leap_year(2100));
    assert(jp_days_in_month(2100, 2) == 28);
    assert(jp_day_of_week(2026, 9, 21) == 1);
    assert(jp_iso_week_number(2021, 1, 1) == 53);
    assert(jp_iso_week_number(2021, 1, 4) == 1);
    assert(jp_iso_week_number(2026, 12, 31) == 53);
    assert(jp_calendar_rows(2026, 2, 0) == 4); /* starts Sunday */
    assert(jp_calendar_rows(2025, 3, 0) == 6); /* starts Saturday */
    assert(jp_calendar_rows(2026, 8, 0) == 6);
    assert(jp_reiwa_year(2019, 4, 30) == 0);
    assert(jp_reiwa_year(2019, 5, 1) == 1);
    assert(jp_reiwa_year(2020, 1, 1) == 2);
    assert(jp_reiwa_year(2026, 9, 1) == 8);
    assert(jp_reiwa_year(2026, 9, 25) == 8);
    assert(jp_reiwa_year(2027, 1, 1) == 9);

    expect_holiday(2019, 4, 30, JP_HOLIDAY_CITIZENS);
    expect_holiday(2019, 5, 1, JP_HOLIDAY_ENTHRONEMENT);
    expect_holiday(2019, 5, 2, JP_HOLIDAY_CITIZENS);
    expect_holiday(2019, 10, 22, JP_HOLIDAY_ENTHRONEMENT_CEREMONY);
    expect_holiday(2020, 7, 23, JP_HOLIDAY_MARINE);
    expect_holiday(2020, 7, 24, JP_HOLIDAY_SPORTS);
    expect_holiday(2020, 8, 10, JP_HOLIDAY_MOUNTAIN);
    expect_holiday(2020, 7, 20, JP_HOLIDAY_NONE);
    expect_holiday(2021, 7, 22, JP_HOLIDAY_MARINE);
    expect_holiday(2021, 7, 23, JP_HOLIDAY_SPORTS);
    expect_holiday(2021, 8, 8, JP_HOLIDAY_MOUNTAIN);
    expect_holiday(2021, 8, 9, JP_HOLIDAY_SUBSTITUTE);

    expect_holiday(2026, 1, 1, JP_HOLIDAY_NEW_YEAR);
    expect_holiday(2026, 1, 12, JP_HOLIDAY_COMING_OF_AGE);
    expect_holiday(2026, 2, 11, JP_HOLIDAY_FOUNDATION);
    expect_holiday(2026, 2, 23, JP_HOLIDAY_EMPEROR_BIRTHDAY);
    expect_holiday(2026, 3, 20, JP_HOLIDAY_VERNAL_EQUINOX);
    expect_holiday(2026, 4, 29, JP_HOLIDAY_SHOWA);
    expect_holiday(2026, 5, 3, JP_HOLIDAY_CONSTITUTION);
    expect_holiday(2026, 5, 4, JP_HOLIDAY_GREENERY);
    expect_holiday(2026, 5, 5, JP_HOLIDAY_CHILDREN);
    expect_holiday(2026, 5, 6, JP_HOLIDAY_SUBSTITUTE);
    expect_holiday(2026, 7, 20, JP_HOLIDAY_MARINE);
    expect_holiday(2026, 8, 11, JP_HOLIDAY_MOUNTAIN);
    expect_holiday(2026, 9, 21, JP_HOLIDAY_RESPECT_FOR_AGED);
    expect_holiday(2026, 9, 22, JP_HOLIDAY_CITIZENS);
    expect_holiday(2026, 9, 23, JP_HOLIDAY_AUTUMNAL_EQUINOX);
    expect_holiday(2026, 10, 12, JP_HOLIDAY_SPORTS);
    expect_holiday(2026, 11, 3, JP_HOLIDAY_CULTURE);
    expect_holiday(2026, 11, 23, JP_HOLIDAY_LABOR_THANKSGIVING);
    expect_holiday(2026, 9, 24, JP_HOLIDAY_NONE);

    assert(strcmp(jp_get_holiday(2026, 10, 12).name, "スポーツの日") == 0);
    assert(strcmp(jp_get_holiday(2026, 9, 21).name, "敬老の日") == 0);
    assert(strcmp(jp_get_holiday(2026, 9, 22).name, "国民の休日") == 0);
    assert(strcmp(jp_get_holiday(2026, 9, 23).name, "秋分の日") == 0);
    assert(strcmp(jp_get_holiday(2026, 3, 20).name, "春分の日") == 0);
    assert(strcmp(jp_get_holiday(2026, 11, 23).name, "勤労感謝の日") == 0);
    assert(strcmp(jp_get_holiday(2021, 8, 9).name, "振替休日") == 0);
    assert(strcmp(jp_get_holiday(2019, 10, 22).name, "即位礼正殿の儀の行われる日") == 0);
    assert(strcmp(jp_get_holiday(2019, 10, 14).name, "体育の日") == 0);
    assert(strcmp(jp_eto_stem(2024), "甲") == 0 && strcmp(jp_eto_branch(2024), "辰") == 0);
    assert(strcmp(jp_eto_stem(2025), "乙") == 0 && strcmp(jp_eto_branch(2025), "巳") == 0);
    assert(strcmp(jp_eto_stem(2026), "丙") == 0 && strcmp(jp_eto_branch(2026), "午") == 0);
    assert(strcmp(jp_eto_animal(2026), "馬") == 0);
    assert(strcmp(jp_eto_stem(2027), "丁") == 0 && strcmp(jp_eto_branch(2027), "未") == 0);
    assert(strcmp(jp_eto_stem(2028), "戊") == 0 && strcmp(jp_eto_branch(2028), "申") == 0);
    assert(strcmp(jp_eto_stem(1966), "丙") == 0 && strcmp(jp_eto_branch(1966), "午") == 0);
    assert(strcmp(jp_eto_stem(1966), jp_eto_stem(2026)) == 0);
    assert(strcmp(jp_eto_branch(1966), jp_eto_branch(2026)) == 0);
    puts("JapaneseCalendar tests: PASS");
    return 0;
}
