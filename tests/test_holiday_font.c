#include <stdio.h>
#include <string.h>

#include "Adafruit_GFX.h"
#include "JapaneseCalendar.h"
#include "fonts.h"

int main(void) {
    Adafruit_GFX gfx;
    const int cell_width = (400 - 10) / 7;
    const int available = cell_width - 2;
    int longest_normal = 0, longest_compact = 0, holiday_count = 0;
    int normal_height, compact_height;
    const char* longest_name = "";
    uint16_t year;
    GFX_begin(&gfx, 400, 300, 16);
    GFX_setFont(&gfx, u8g2_font_jp_ui_medium11);
    normal_height = GFX_getFontHeight(&gfx);
    printf("Normal small font metrics: ascent=%d descent=%d height=%d\n",
           GFX_getFontAscent(&gfx), GFX_getFontDescent(&gfx), normal_height);
    GFX_setFont(&gfx, u8g2_font_jp_holiday_compact);
    compact_height = GFX_getFontHeight(&gfx);
    if (compact_height < 10 || compact_height > 17) {
        fprintf(stderr, "Unexpected compact font height: %d\n", compact_height);
        return 1;
    }
    for (year = 2000; year <= 2050; ++year) {
        uint8_t month;
        for (month = 1; month <= 12; ++month) {
            uint8_t day;
            for (day = 1; day <= jp_days_in_month(year, month); ++day) {
                jp_holiday_t holiday = jp_get_holiday(year, month, day);
                int normal, compact;
                if (holiday.id == JP_HOLIDAY_NONE) continue;
                ++holiday_count;
                GFX_setFont(&gfx, u8g2_font_jp_ui_medium11);
                normal = GFX_getUTF8Width(&gfx, holiday.name);
                GFX_setFont(&gfx, u8g2_font_jp_holiday_compact);
                compact = GFX_getUTF8Width(&gfx, holiday.name);
                if (normal > longest_normal) longest_normal = normal;
                if (holiday.id == JP_HOLIDAY_ENTHRONEMENT_CEREMONY) {
                    int first = GFX_getUTF8Width(&gfx, "即位礼正殿の儀");
                    int second = GFX_getUTF8Width(&gfx, "の行われる日");
                    compact = first > second ? first : second;
                }
                if (compact > longest_compact) {
                    longest_compact = compact;
                    longest_name = holiday.name;
                }
                if (normal > available && compact > available) {
                    fprintf(stderr, "OVERFLOW %04u-%02u-%02u %s normal=%d compact=%d available=%d\n",
                            year, month, day, holiday.name, normal, compact, available);
                    return 1;
                }
            }
        }
    }
    printf("Holiday font tests: PASS (%d dates, normal=%d px high/max %d px wide, compact=%d px high/max %d px wide [%s], cell=%d px, available=%d px)\n",
           holiday_count, normal_height, longest_normal, compact_height, longest_compact,
           longest_name, cell_width, available);
    return 0;
}
