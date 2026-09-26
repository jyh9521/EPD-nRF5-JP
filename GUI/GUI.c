#include "GUI.h"

#include <stdio.h>
#include <string.h>

#include "JapaneseCalendar.h"
#include "JapaneseRokuyo.h"
#include "Lunar.h"
#include "fonts.h"

#define JP_SATURDAY_RED 0
enum {
    HEADER_HEIGHT = 53,
    MONTH_BLOCK_WIDTH = 59,
    META_COL1_X = 76,
    META_COL2_X = 151,
    META_ROW1_Y = 19,
    META_ROW2_Y = 40,
    STATUS_LEFT_X = 320,
    STATUS_RIGHT_X = 390,
    YEAR_Y = 18,
    MONTH_Y = 43,
};

static const char* const JP_WAFU_MONTHS[12] = {
    "睦月", "如月", "弥生", "卯月", "皐月", "水無月",
    "文月", "葉月", "長月", "神無月", "霜月", "師走",
};
#define GFX_printf_styled(gfx, fg, bg, font, ...) \
    GFX_setTextColor(gfx, fg, bg);                \
    GFX_setFont(gfx, font);                       \
    GFX_printf(gfx, __VA_ARGS__);

// height to use larger layout
#define large_layout(data) ((data)->height >= 400)

static void DrawTimeSyncTip(Adafruit_GFX* gfx, gui_data_t* data) {
    const char* title = "SYNC TIME!";
    const char* url = "https://tsl0922.github.io/EPD-nRF5";

    GFX_setFont(gfx, u8g2_font_wqy9_t_lunar);

    int16_t fh = GFX_getFontHeight(gfx);
    int16_t box_w = GFX_getUTF8Width(gfx, url) + 20;
    int16_t box_h = fh * 2 + 20;
    int16_t box_x = (data->width - box_w) / 2;
    int16_t box_y = data->height / 2 - box_h / 2;

    GFX_fillRect(gfx, box_x, box_y, box_w, box_h, GFX_WHITE);
    GFX_drawRoundRect(gfx, box_x, box_y, box_w, box_h, 5, GFX_BLACK);
    GFX_setTextColor(gfx, GFX_RED, GFX_WHITE);
    GFX_setCursor(gfx, box_x + (box_w - GFX_getUTF8Width(gfx, title)) / 2, box_y + 5 + fh);
    GFX_printf(gfx, title);
    GFX_setTextColor(gfx, GFX_BLACK, GFX_WHITE);
    GFX_setCursor(gfx, box_x + 10, box_y + box_h - GFX_getFontAscent(gfx));
    GFX_printf(gfx, url);
}

static uint8_t batt_cal(uint16_t voltage) {
    uint16_t adc_sample = (voltage * 2047) / 3600;
    if (adc_sample > 1705)
        return 100;
    else if (adc_sample <= 1705 && adc_sample > 1584)
        return 28 + (uint8_t)(((((adc_sample - 1584) << 16) / (1705 - 1584)) * 72) >> 16);
    else if (adc_sample <= 1584 && adc_sample > 1360)
        return 4 + (uint8_t)(((((adc_sample - 1360) << 16) / (1584 - 1360)) * 24) >> 16);
    else if (adc_sample <= 1360 && adc_sample > 1136)
        return (uint8_t)(((((adc_sample - 1136) << 16) / (1360 - 1136)) * 4) >> 16);
    else
        return 0;
}

static void DrawBattery(Adafruit_GFX* gfx, int16_t x, int16_t y, uint8_t iw, uint16_t voltage) {
    x -= iw;
    uint8_t level = batt_cal(voltage);
    GFX_setFont(gfx, u8g2_font_wqy9_t_lunar);
    GFX_setCursor(gfx, x - GFX_getUTF8Width(gfx, "3.2V") - 2, y + 9);
    GFX_printf(gfx, "%d.%dV", voltage / 1000, (voltage % 1000) / 100);
    GFX_fillRect(gfx, x, y, iw, 10, GFX_WHITE);
    GFX_drawRect(gfx, x, y, iw, 10, GFX_BLACK);
    GFX_fillRect(gfx, x + iw, y + 4, 2, 2, GFX_BLACK);
    GFX_fillRect(gfx, x + 2, y + 2, 16 * level / 100, 6, GFX_BLACK);
}

static uint8_t GetWeekOfYear(uint8_t year, uint8_t mon, uint8_t mday, uint8_t wday) {
    (void)wday;
    return jp_iso_week_number((uint16_t)year + YEAR0, (uint8_t)(mon + 1U), mday);
}

/* Narrow 3x7 status lettering keeps the full BLE device name in its fixed box. */
static void DrawTinyStatusText(Adafruit_GFX* gfx, int16_t x, int16_t y, const char* text) {
    static const uint16_t glyphs[] = {
        0x2BED, 0x6BAE, 0x3923, 0x6B6E, 0x79A7, 0x79A4, 0x396B, 0x5BED, 0x7497, 0x126A,
        0x5BAD, 0x4927, 0x5FED, 0x5FFD, 0x2B6A, 0x6BA4, 0x2B59, 0x6BAD, 0x388E, 0x7492,
        0x5B6F, 0x5B6A, 0x5BFD, 0x5AAD, 0x5A92, 0x72A7, 0x7B6F, 0x2C97, 0x63E7, 0x62CE,
        0x5BC9, 0x79CE, 0x39EF, 0x7292, 0x7BEF, 0x7BCE, 0x0007, 0x0002, 0x01C0,
    };
    static const uint8_t source_row[] = {0, 1, 1, 2, 3, 3, 4};
    for (; *text && x + 3 <= STATUS_RIGHT_X; text++, x += 4) {
        char c = *text;
        uint8_t index = c >= 'A' && c <= 'Z' ? (uint8_t)(c - 'A')
                      : c >= '0' && c <= '9' ? (uint8_t)(c - '0' + 26)
                      : c == '_' ? 36 : c == '.' ? 37 : c == '-' ? 38 : 39;
        uint16_t bits = index < 39 ? glyphs[index] : 0;
        for (uint8_t row = 0; row < 7; row++)
            for (uint8_t col = 0; col < 3; col++)
                if (bits & (1U << (14 - source_row[row] * 3 - col)))
                    GFX_drawPixel(gfx, x + col, y + row, GFX_WHITE);
    }
}

static void DrawDateHeader(Adafruit_GFX* gfx, int16_t x, int16_t y, tm_t* tm, gui_data_t* data) {
    uint16_t year = (uint16_t)(tm->tm_year + YEAR0);
    uint8_t reiwa = jp_reiwa_year(year, (uint8_t)(tm->tm_mon + 1U), tm->tm_mday);
    int16_t width, left;
    (void)x;
    (void)y;

    GFX_fillRect(gfx, 0, 0, data->width, HEADER_HEIGHT, GFX_BLACK);
    GFX_fillRect(gfx, 0, 0, MONTH_BLOCK_WIDTH, HEADER_HEIGHT, GFX_RED);
    GFX_drawFastVLine(gfx, MONTH_BLOCK_WIDTH + 7, 7, HEADER_HEIGHT - 14, GFX_WHITE);
    GFX_drawFastVLine(gfx, STATUS_LEFT_X - 10, 7, HEADER_HEIGHT - 14, GFX_WHITE);

    GFX_setTextColor(gfx, GFX_WHITE, GFX_RED);
    GFX_setFont(gfx, u8g2_font_wqy9_t_lunar);
    width = GFX_getUTF8Width(gfx, "0000");
    GFX_setCursor(gfx, (MONTH_BLOCK_WIDTH - width) / 2, YEAR_Y);
    GFX_printf(gfx, "%u", year);
    GFX_setFont(gfx, u8g2_font_helvB18_tn);
    width = GFX_getUTF8Width(gfx, "00");
    GFX_setCursor(gfx, (MONTH_BLOCK_WIDTH - width) / 2, MONTH_Y);
    GFX_printf(gfx, "%02u", (unsigned)(tm->tm_mon + 1));

    GFX_setFont(gfx, u8g2_font_wqy9_t_lunar);
    GFX_setTextColor(gfx, GFX_WHITE, GFX_BLACK);
    GFX_setCursor(gfx, META_COL1_X, META_ROW1_Y);
    if (reiwa == 1U)
        GFX_printf(gfx, "令和元年");
    else if (reiwa != 0U)
        GFX_printf(gfx, "令和%d年", reiwa);
    GFX_setCursor(gfx, META_COL2_X, META_ROW1_Y);
    GFX_printf(gfx, "%s%s年 [%s]", jp_eto_stem(year), jp_eto_branch(year), jp_eto_animal(year));
    GFX_setCursor(gfx, META_COL1_X, META_ROW2_Y);
    GFX_printf(gfx, "%s", JP_WAFU_MONTHS[tm->tm_mon]);
    GFX_setCursor(gfx, META_COL2_X, META_ROW2_Y);
    GFX_printf(gfx, "第%d週", GetWeekOfYear(tm->tm_year, tm->tm_mon, tm->tm_mday, tm->tm_wday));

    /* Keep variable device names within the fixed status block. */
    width = (int16_t)strlen(data->ssid);
    if (width > 17) width = 17;
    DrawTinyStatusText(gfx, STATUS_LEFT_X + (STATUS_RIGHT_X - STATUS_LEFT_X - width * 4) / 2, 8, data->ssid);
    GFX_drawFastHLine(gfx, STATUS_LEFT_X, 22, STATUS_RIGHT_X - STATUS_LEFT_X, GFX_WHITE);
    GFX_setCursor(gfx, STATUS_LEFT_X, META_ROW2_Y);
    GFX_printf(gfx, "%u%%", batt_cal(data->voltage));
    left = STATUS_LEFT_X + 29;
    GFX_drawRect(gfx, left, 31, 18, 10, GFX_WHITE);
    GFX_fillRect(gfx, left + 18, 34, 2, 4, GFX_WHITE);
    GFX_fillRect(gfx, left + 2, 33, 14 * batt_cal(data->voltage) / 100, 6, GFX_WHITE);
    char voltage_text[5] = {(char)('0' + data->voltage / 1000), '.',
                            (char)('0' + (data->voltage % 1000) / 100), 'V', '\0'};
    DrawTinyStatusText(gfx, STATUS_LEFT_X + 54, 33, voltage_text);
}

static void DrawWeekHeader(Adafruit_GFX* gfx, int16_t x, int16_t y, gui_data_t* data) {
    static const char WEEKDAYS[7][4] = {"日", "月", "火", "水", "木", "金", "土"};
    bool large = large_layout(data);
    GFX_setFont(gfx, large ? u8g2_font_wqy12_t_lunar : u8g2_font_wqy9_t_lunar);
    uint8_t w = (data->width - 2 * x) / 7;
    uint8_t h = large ? 32 : 22;
    uint8_t r = (data->width - 2 * x) % 7;
    uint8_t fh = (h - GFX_getFontHeight(gfx)) / 2 + GFX_getFontAscent(gfx) + 1;
    int16_t cw = GFX_getUTF8Width(gfx, WEEKDAYS[0]);
    for (int i = 0; i < 7; i++) {
        uint8_t day = (data->week_start + i) % 7;
        uint16_t bg = large ? ((day == 0 || (JP_SATURDAY_RED && day == 6)) ? GFX_RED : GFX_BLACK) : GFX_WHITE;
        uint16_t fg = large ? GFX_WHITE : ((day == 0) ? GFX_RED : GFX_BLACK);
        if (large) GFX_fillRect(gfx, x + i * w, y, i == 6 ? (w + r) : w, h, bg);
        GFX_setTextColor(gfx, fg, bg);
        GFX_setCursor(gfx, x + (w - cw) / 2 + i * w, y + fh);
        GFX_printf(gfx, "%s", WEEKDAYS[day]);
    }
    if (!large) GFX_drawFastHLine(gfx, x + 1, y + h - 1, data->width - 2 * x - 2, GFX_BLACK);
}

static void DrawMonthDays(Adafruit_GFX* gfx, int16_t x, int16_t y, tm_t* tm, gui_data_t* data) {
    uint16_t year = (uint16_t)(tm->tm_year + YEAR0);
    uint8_t month = (uint8_t)(tm->tm_mon + 1U);
    uint8_t firstDayWeek = jp_day_of_week(year, month, 1U);
    int8_t adjustedFirstDay = (firstDayWeek - data->week_start + 7) % 7;
    uint8_t monthMaxDays = jp_days_in_month(year, month);
    uint8_t monthDayRows = jp_calendar_rows(year, month, data->week_start);

    bool large = large_layout(data);
    int16_t bw = large ? (data->width - x - 10) / 7 : (data->width - 2 * x) / 7;
    int16_t bh = (data->height - y - (large ? 10 : 9)) / monthDayRows;

    if (large) {
        for (uint8_t i = 1; i < monthDayRows; i++)
            GFX_drawDottedLine(gfx, x, y + i * bh, x + 7 * bw - 1, y + i * bh, GFX_BLACK, 1, 5);
        for (uint8_t i = 1; i < 7; i++)
            GFX_drawDottedLine(gfx, x + i * bw, y, x + i * bw, y + monthDayRows * bh - 1, GFX_BLACK, 1, 5);
    }

    for (uint8_t i = 0; i < monthMaxDays; i++) {
        uint8_t day = i + 1;
        jp_holiday_t holiday = jp_get_holiday(year, month, day);

        int16_t actualWeek = (firstDayWeek + i) % 7;
        int16_t displayWeek = (adjustedFirstDay + i) % 7;
        bool red_day = (actualWeek == 0) || (JP_SATURDAY_RED && actualWeek == 6) ||
                       holiday.id != JP_HOLIDAY_NONE;

        int16_t cr = large ? 15 : 11;
        if (monthDayRows > 5) cr -= 1;  // reduce circle height for 6 week rows
        int16_t cell_left = x + displayWeek * bw;
        int16_t cell_center_x = cell_left + bw / 2;
        int16_t row_top = y + (i + adjustedFirstDay) / 7 * bh;
        int16_t bx = cell_center_x - cr;
        int16_t by = row_top + (bh - 2 * cr) / 2 + 3;

        if (day == tm->tm_mday) {
            if (large) {
                GFX_fillCircle(gfx, bx + cr, by + cr - 3,
                               holiday.id == JP_HOLIDAY_NONE ? 2 * cr : cr + 4, GFX_RED);
            } else {
                const int16_t card_x = cell_left + 5;
                const int16_t card_y = row_top - 1;
                const int16_t card_w = bw - 9;
                const int16_t card_h = bh - 2;
                GFX_fillRect(gfx, card_x + 2, card_y, card_w - 4, card_h, GFX_RED);
                GFX_fillRect(gfx, card_x + 1, card_y + 1, card_w - 2, card_h - 2, GFX_RED);
                GFX_fillRect(gfx, card_x, card_y + 2, card_w, card_h - 4, GFX_RED);
            }
            GFX_setTextColor(gfx, GFX_WHITE, GFX_RED);
        } else {
            GFX_setTextColor(gfx, red_day ? GFX_RED : GFX_BLACK, GFX_WHITE);
            if (!large && holiday.id != JP_HOLIDAY_NONE)
                GFX_fillRect(gfx, cell_left + 4, row_top, 4, bh - 4, GFX_RED);
        }

        char buf[10] = {0};
        snprintf(buf, sizeof(buf), "%d", day);
        GFX_setFont(gfx, large ? u8g2_font_helvB18_tn : u8g2_font_helvB14_tn);
        int16_t date_width = GFX_getUTF8Width(gfx, buf);
        int16_t date_ascent = GFX_getFontAscent(gfx);
        int16_t date_x = cell_center_x - date_width / 2;
        int16_t date_y = large ? by - (cr - GFX_getFontHeight(gfx)) - 1 : row_top + 23;
        GFX_setCursor(gfx, date_x, date_y);
        GFX_printf(gfx, "%s", buf);

        GFX_setFont(gfx, large ? u8g2_font_wqy12_t_lunar : u8g2_font_jp_holiday_compact);
        int16_t label_y = large ? date_y + GFX_getFontHeight(gfx) + 5 : row_top + 38;
        GFX_setFontMode(gfx, 1);  // transparent
        if (holiday.id != JP_HOLIDAY_NONE) {
            int16_t available = bw - 2;
            GFX_setFont(gfx, large ? u8g2_font_wqy9_t_lunar : u8g2_font_jp_holiday_compact);
            int16_t label_width = GFX_getUTF8Width(gfx, holiday.name);
            if (label_width > available || holiday.id == JP_HOLIDAY_ENTHRONEMENT_CEREMONY) {
                GFX_setFont(gfx, u8g2_font_jp_holiday_compact);
                label_width = GFX_getUTF8Width(gfx, holiday.name);
            }
            GFX_setTextColor(gfx, day == tm->tm_mday && !large ? GFX_WHITE : GFX_RED,
                             day == tm->tm_mday && !large ? GFX_RED : GFX_WHITE);
            if (holiday.id == JP_HOLIDAY_ENTHRONEMENT_CEREMONY) {
                /* The 2019 legal name is 13 glyphs: keep it complete on two lines. */
                const char* first = "即位礼正殿の儀";
                const char* second = "の行われる日";
                int16_t first_width = GFX_getUTF8Width(gfx, first);
                int16_t second_width = GFX_getUTF8Width(gfx, second);
                if (day == tm->tm_mday) {
                    GFX_fillRect(gfx, cell_center_x - available / 2 - 1,
                                 label_y - GFX_getFontAscent(gfx) - 1, available + 2,
                                 2 * GFX_getFontHeight(gfx) + 2, GFX_WHITE);
                    GFX_setTextColor(gfx, GFX_RED, GFX_WHITE);
                }
                GFX_setCursor(gfx, cell_center_x - first_width / 2, label_y);
                GFX_printf(gfx, "%s", first);
                GFX_setCursor(gfx, cell_center_x - second_width / 2, label_y + GFX_getFontHeight(gfx));
                GFX_printf(gfx, "%s", second);
            } else {
                if (day == tm->tm_mday && large)
                    GFX_fillRect(gfx, cell_center_x - label_width / 2 - 1,
                                 label_y - GFX_getFontAscent(gfx) - 1, label_width + 2,
                                 GFX_getFontHeight(gfx) + 2, GFX_WHITE);
                GFX_setCursor(gfx, cell_center_x - label_width / 2, label_y);
                GFX_printf(gfx, "%s", holiday.name);
            }
            GFX_setFont(gfx, u8g2_font_jp_holiday_compact);
            int16_t badge_width = GFX_getUTF8Width(gfx, "休");
            int16_t badge_top = large ? date_y - date_ascent - 3 : row_top + 3;
            if (large && day == tm->tm_mday && badge_top > by - cr - 5)
                badge_top = by - cr - 5;
            if (large && day == tm->tm_mday)
                GFX_fillRect(gfx, date_x + date_width, badge_top - 1,
                             badge_width + 2, GFX_getFontHeight(gfx) + 2, GFX_WHITE);
            if (!large) {
                if (day == tm->tm_mday)
                    GFX_drawRect(gfx, date_x + date_width + 1, badge_top,
                                 badge_width + 3, GFX_getFontHeight(gfx) + 2, GFX_WHITE);
                else
                    GFX_fillRect(gfx, date_x + date_width + 1, badge_top,
                                 badge_width + 3, GFX_getFontHeight(gfx) + 2, GFX_RED);
                GFX_setTextColor(gfx, GFX_WHITE, GFX_RED);
            }
            GFX_setCursor(gfx, date_x + date_width + 2, badge_top + GFX_getFontAscent(gfx) + (large ? 0 : 1));
            GFX_printf(gfx, "休");
        } else {
            jp_rokuyo_t rokuyo = jp_get_rokuyo(year, month, day);
            if (rokuyo != JP_ROKUYO_NONE) {
                const char* name = jp_rokuyo_name(rokuyo);
                if (!large) GFX_setFont(gfx, u8g2_font_jp_holiday_compact);
                GFX_setTextColor(gfx, GFX_BLACK, GFX_WHITE);
                GFX_setCursor(gfx, cell_center_x - GFX_getUTF8Width(gfx, name) / 2, label_y);
                GFX_printf(gfx, "%s", name);
            }
        }
    }
    if (!large) GFX_drawFastHLine(gfx, x + 1, data->height - 5, data->width - 2 * x - 2, GFX_BLACK);
}

static void DrawCalendar(Adafruit_GFX* gfx, tm_t* tm, gui_data_t* data) {
    bool large = large_layout(data);
    DrawDateHeader(gfx, 10, large ? 38 : 28, tm, data);
    DrawWeekHeader(gfx, large ? 10 : 4, large ? 52 : HEADER_HEIGHT, data);
    DrawMonthDays(gfx, large ? 10 : 4, large ? 84 : 76, tm, data);
}

// clang-format off
/* Routine to Draw Large 7-Segment formated number
   Contributed by William Zaggle.

   int n - The number to be displayed
   int xLoc = The x location of the upper left corner of the number
   int yLoc = The y location of the upper left corner of the number
   int cS = The size of the number. 
   fC is the foreground color of the number
   bC is the background color of the number (prevents having to clear previous space)
   nD is the number of digit spaces to occupy (must include space for minus sign for numbers < 0).

   width: nD*(11*cS+2)-2*cS
   height: 20*cS+4

   https://forum.arduino.cc/t/fast-7-segment-number-display-for-tft/296619/4
*/
static void Draw7Number(Adafruit_GFX *gfx, int16_t n, uint16_t xLoc, uint16_t yLoc, int16_t cS, uint16_t fC, uint16_t bC, int16_t nD) {
    uint16_t num=abs(n),i,t,w,col,h,a,b,j=1,d=0,S2=5*cS,S3=2*cS,S4=7*cS,x1=cS+1,x2=S3+S2+1,y1=yLoc+x1,y3=yLoc+S3+S4+1;
    uint16_t seg[7][3]={{x1,yLoc,1},{x2,y1,0},{x2,y3+x1,0},{x1,(2*y3)-yLoc,1},{0,y3+x1,0},{0,y1,0},{x1,y3,1}};
    uint8_t nums[12]={0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x00,0x40},c=(c=abs(cS))>10?10:(c<1)?1:c,cnt=(cnt=abs(nD))>10?10:(cnt<1)?1:cnt;
    for (xLoc+=cnt*(d=S2+(3*S3)+2);cnt>0;cnt--){
      for (i=(num>9)?num%10:((!cnt)&&(n<0))?11:((nD<0)&&(!num))?10:num,xLoc-=d,num/=10,j=0;j<7;++j){
        col=(nums[i]&(1<<j))?fC:bC;
        if (seg[j][2])for(w=S2,t=seg[j][1]+S3,h=seg[j][1]+cS,a=xLoc+seg[j][0]+cS,b=seg[j][1];b<h;b++,a--,w+=2)GFX_drawFastHLine(gfx,a,b,w,col);
        else for(w=S4,t=xLoc+seg[j][0]+S3,h=xLoc+seg[j][0]+cS,b=xLoc+seg[j][0],a=seg[j][1]+cS;b<h;b++,a--,w+=2)GFX_drawFastVLine(gfx,b,a,w,col);
        for (;b<t;b++,a++,w-=2)seg[j][2]?GFX_drawFastHLine(gfx,a,b,w,col):GFX_drawFastVLine(gfx,b,a,w,col);
        }
    }
}
// clang-format on

static void DrawTime(Adafruit_GFX* gfx, tm_t* tm, int16_t x, int16_t y, uint16_t cS, uint16_t nD) {
    Draw7Number(gfx, tm->tm_hour, x, y, cS, GFX_BLACK, GFX_WHITE, nD);
    x += (nD * (11 * cS + 2) - 2 * cS) + 2 * cS;
    GFX_fillRect(gfx, x, y + 4.5 * cS + 1, 2 * cS, 2 * cS, GFX_BLACK);
    GFX_fillRect(gfx, x, y + 13.5 * cS + 3, 2 * cS, 2 * cS, GFX_BLACK);
    x += 4 * cS;
    Draw7Number(gfx, tm->tm_min, x, y, cS, GFX_BLACK, GFX_WHITE, nD);
}

static void DrawClock(Adafruit_GFX* gfx, tm_t* tm, struct Lunar_Date* Lunar, gui_data_t* data) {
    uint8_t padding = large_layout(data) ? 100 : 40;
    GFX_setCursor(gfx, padding, 36);
    GFX_printf_styled(gfx, GFX_RED, GFX_WHITE, u8g2_font_helvB18_tn, "%d", tm->tm_year + YEAR0);
    GFX_printf_styled(gfx, GFX_BLACK, GFX_WHITE, u8g2_font_wqy12_t_lunar, "年");
    GFX_printf_styled(gfx, GFX_RED, GFX_WHITE, u8g2_font_helvB18_tn, "%02d", tm->tm_mon + 1);
    GFX_printf_styled(gfx, GFX_BLACK, GFX_WHITE, u8g2_font_wqy12_t_lunar, "月");
    GFX_printf_styled(gfx, GFX_RED, GFX_WHITE, u8g2_font_helvB18_tn, "%02d", tm->tm_mday);
    GFX_printf_styled(gfx, GFX_BLACK, GFX_WHITE, u8g2_font_wqy12_t_lunar, "日 ");

    GFX_setCursor(gfx, padding, 58);
    GFX_setFont(gfx, u8g2_font_wqy9_t_lunar);
    GFX_printf(gfx, "星期%s", Lunar_DayString[tm->tm_wday]);
    GFX_setCursor(gfx, 138, 58);
    GFX_printf(gfx, "%s%s%s", Lunar_MonthLeapString[Lunar->IsLeap], Lunar_MonthString[Lunar->Month],
               Lunar_DateString[Lunar->Date]);

    DrawBattery(gfx, data->width - padding, 25, 20, data->voltage);

    char ssid[5] = {0};
    int16_t ssid_len = strlen(data->ssid);
    int16_t sw = GFX_getUTF8Width(gfx, "25℃[1234]");
    memcpy(ssid, &data->ssid[ssid_len - 4], 4);
    GFX_setCursor(gfx, data->width - padding - sw - 2, 58);
    GFX_setFont(gfx, u8g2_font_wqy9_t_lunar);
    GFX_printf(gfx, "%d℃[%s]", data->temperature, ssid);

    GFX_drawFastHLine(gfx, padding - 10, 68, data->width - 2 * (padding - 10), GFX_BLACK);

    uint16_t cS = data->height / 45;
    uint16_t nD = 2;
    uint16_t time_width = 2 * (nD * (11 * cS + 2) - 2 * cS) + 4 * cS;
    uint16_t time_height = 20 * cS + 4;
    int16_t time_x = (data->width - time_width) / 2;
    int16_t time_y = (68 + (data->height - 68)) / 2 - time_height / 2;
    DrawTime(gfx, tm, time_x, time_y, cS, nD);

    GFX_drawFastHLine(gfx, padding - 10, data->height - 68, data->width - 2 * (padding - 10), GFX_BLACK);

    GFX_setCursor(gfx, padding, data->height - 68 + 30);
    GFX_setFont(gfx, u8g2_font_wqy12_t_lunar);
    GFX_printf(gfx, "%s%s", Lunar_StemStrig[LUNAR_GetStem(Lunar)], Lunar_BranchStrig[LUNAR_GetBranch(Lunar)]);
    GFX_setTextColor(gfx, GFX_RED, GFX_WHITE);
    GFX_printf(gfx, "%s", Lunar_ZodiacString[LUNAR_GetZodiac(Lunar)]);
    GFX_setTextColor(gfx, GFX_BLACK, GFX_WHITE);
    GFX_printf(gfx, "年");

    GFX_setCursor(gfx, padding, data->height - 68 + 30 + 20);
    GFX_printf(gfx, "%d周", GetWeekOfYear(tm->tm_year, tm->tm_mon, tm->tm_mday, tm->tm_wday));

    uint8_t day = 0;
    uint8_t JQday = GetJieQiStr(tm->tm_year + YEAR0, tm->tm_mon + 1, tm->tm_mday, &day);
    if (day == 0) {
        GFX_setCursor(gfx, data->width - GFX_getUTF8Width(gfx, "小暑") - padding, data->height - 68 + 30);
        GFX_setTextColor(gfx, GFX_RED, GFX_WHITE);
        GFX_printf(gfx, "%s", JieQiStr[JQday % 24]);
    } else {
        GFX_setCursor(gfx, data->width - GFX_getUTF8Width(gfx, "离小暑") - padding, data->height - 68 + 30);
        GFX_printf(gfx, "离%");
        GFX_setTextColor(gfx, GFX_RED, GFX_WHITE);
        GFX_printf(gfx, "%s", JieQiStr[JQday % 24]);
        GFX_setTextColor(gfx, GFX_BLACK, GFX_WHITE);
        char buf[15] = {0};
        snprintf(buf, sizeof(buf), "还有%d天", day);
        GFX_setCursor(gfx, data->width - GFX_getUTF8Width(gfx, buf) - padding, data->height - 68 + 30 + 20);
        GFX_printf(gfx, buf);
    }
}

void DrawGUI(gui_data_t* data, buffer_callback callback, void* callback_data) {
    if (data->week_start > 6) data->week_start = 0;

    tm_t tm = {0};
    struct Lunar_Date Lunar;

    transformTime(data->timestamp, &tm);

    Adafruit_GFX gfx;
    int16_t ph = (__HEAP_SIZE - 512) / (data->width / 8);

    if (data->color == 2)
        GFX_begin_3c(&gfx, data->width, data->height, ph);
    else if (data->color == 3)
        GFX_begin_4c(&gfx, data->width, data->height, ph);
    else
        GFX_begin(&gfx, data->width, data->height, ph);

    GFX_firstPage(&gfx);
    do {
        GFX_fillScreen(&gfx, GFX_WHITE);

        switch (data->mode) {
            case MODE_CALENDAR:
                DrawCalendar(&gfx, &tm, data);
                break;
            case MODE_CLOCK:
                LUNAR_SolarToLunar(&Lunar, tm.tm_year + YEAR0, tm.tm_mon + 1, tm.tm_mday);
                DrawClock(&gfx, &tm, &Lunar, data);
                break;
            default:
                break;
        }
        if ((data->mode == MODE_CALENDAR || data->mode == MODE_CLOCK) &&
            (tm.tm_year + YEAR0 == 2025 && tm.tm_mon + 1 == 1)) {
            DrawTimeSyncTip(&gfx, data);
        }
    } while (GFX_nextPage(&gfx, callback, callback_data));

    GFX_end(&gfx);
}
