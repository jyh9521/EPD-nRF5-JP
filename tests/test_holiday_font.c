/* Test the real GUI aliases and equal-height row helper. */
#include <assert.h>
#include "../GUI/GUI.c"

int main(void) {
    Adafruit_GFX gfx;
    GFX_begin(&gfx, 400, 300, 16);
    GFX_setFont(&gfx, u8g2_font_wqy9_t_jp);
    assert(strcmp(CompactHolidayName(jp_get_holiday(2026, 11, 23)), "勤労感謝") == 0);
    assert(strcmp(CompactHolidayName(jp_get_holiday(2019, 10, 22)), "即位礼") == 0);
    assert(strcmp(CompactHolidayName(jp_get_holiday(2019, 10, 14)), "体育の日") == 0);
    assert(strcmp(CompactHolidayName(jp_get_holiday(2026, 10, 12)), "スポーツ") == 0);
    assert(strcmp(jp_get_holiday(2019, 10, 22).name, "即位礼正殿の儀の行われる日") == 0);
    int layouts = 0, holidays = 0;
    for (uint16_t year = 1980; year <= 2099; year++) {
        for (uint8_t month = 1; month <= 12; month++) {
            for (uint8_t week = 0; week < 7; week++) {
                uint8_t rows = jp_calendar_rows(year, month, week);
                int16_t height = CompactCalendarRowHeight(year, month, week, 204);
                assert(height >= 34 && height * rows <= 204);
                assert(204 - height * rows < rows);
                layouts++;
            }
            for (uint8_t day = 1; day <= jp_days_in_month(year, month); day++) {
                jp_holiday_t holiday = jp_get_holiday(year, month, day);
                if (holiday.id == JP_HOLIDAY_NONE) continue;
                const char* label = CompactHolidayName(holiday);
                assert(*label && !strchr(label, '\n'));
                assert(GFX_getUTF8Width(&gfx, label) <= 51);
                char footer[64];
                int length = snprintf(footer, sizeof(footer), "%02u/%02u  %s", month, day, holiday.name);
                assert(length > 0 && length < sizeof(footer));
                assert(GFX_getUTF8Width(&gfx, footer) <= 384);
                holidays++;
            }
        }
    }
    GFX_end(&gfx);
    printf("Single-line font tests: PASS (%d equal-row layouts, %d short labels and complete footers)\n",
           layouts, holidays);
    return 0;
}
