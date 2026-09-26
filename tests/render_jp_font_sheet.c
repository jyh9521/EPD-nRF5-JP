/* Host-only, firmware-bitmap font sheet for the 400x300 three-colour panel. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Adafruit_GFX.h"
#include "fonts.h"

#define WIDTH 400
#define HEIGHT 300
#define ROW_BYTES (WIDTH / 8)

static uint8_t black_plane[ROW_BYTES * HEIGHT];
static uint8_t red_plane[ROW_BYTES * HEIGHT];

static void collect_page(void* context, uint8_t* black, uint8_t* red,
                         uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
    uint16_t row;
    uint16_t stride = w / 8U;
    (void)context;
    for (row = 0; row < h && y + row < HEIGHT; ++row) {
        memcpy(&black_plane[(y + row) * ROW_BYTES + x / 8U], &black[row * stride], stride);
        memcpy(&red_plane[(y + row) * ROW_BYTES + x / 8U], &red[row * stride], stride);
    }
}

static int write_ppm(const char* path) {
    FILE* file = fopen(path, "wb");
    uint16_t y;
    if (file == NULL) return 1;
    fprintf(file, "P6\n%d %d\n255\n", WIDTH, HEIGHT);
    for (y = 0; y < HEIGHT; ++y) {
        uint16_t x;
        for (x = 0; x < WIDTH; ++x) {
            uint32_t offset = (uint32_t)y * ROW_BYTES + x / 8U;
            uint8_t mask = (uint8_t)(0x80U >> (x % 8U));
            uint8_t rgb[3] = {255, 255, 255};
            if ((red_plane[offset] & mask) == 0U) {
                rgb[1] = rgb[2] = 0;
            } else if ((black_plane[offset] & mask) == 0U) {
                rgb[0] = rgb[1] = rgb[2] = 0;
            }
            fwrite(rgb, 1, 3, file);
        }
    }
    return fclose(file) == 0 ? 0 : 1;
}

int main(int argc, char** argv) {
    Adafruit_GFX gfx;
    if (argc != 2) {
        fprintf(stderr, "usage: render_jp_font_sheet OUTPUT.ppm\n");
        return 2;
    }
    memset(black_plane, 0xFF, sizeof(black_plane));
    memset(red_plane, 0xFF, sizeof(red_plane));
    GFX_begin_3c(&gfx, WIDTH, HEIGHT, HEIGHT);
    GFX_firstPage(&gfx);
    do {
        GFX_fillScreen(&gfx, GFX_WHITE);
        GFX_fillRect(&gfx, 0, 0, WIDTH, 68, GFX_BLACK);
        GFX_setFontMode(&gfx, 1);
        GFX_setFont(&gfx, u8g2_font_jp_ui_medium11);
        GFX_setTextColor(&gfx, GFX_WHITE, GFX_BLACK);
        GFX_drawUTF8(&gfx, 12, 22, "令和8年");
        GFX_drawUTF8(&gfx, 150, 22, "丙午年 [馬]");
        GFX_drawUTF8(&gfx, 12, 51, "長月");
        GFX_drawUTF8(&gfx, 150, 51, "第39週");

        GFX_setFont(&gfx, u8g2_font_jp_rokuyo_readable);
        GFX_setTextColor(&gfx, GFX_BLACK, GFX_WHITE);
        GFX_drawUTF8(&gfx, 20, 94, "大安");
        GFX_drawUTF8(&gfx, 150, 94, "赤口");
        GFX_drawUTF8(&gfx, 20, 118, "先勝");
        GFX_drawUTF8(&gfx, 150, 118, "友引");
        GFX_drawUTF8(&gfx, 20, 142, "先負");
        GFX_drawUTF8(&gfx, 150, 142, "仏滅");

        GFX_setFont(&gfx, u8g2_font_jp_holiday_compact);
        GFX_setTextColor(&gfx, GFX_RED, GFX_WHITE);
        GFX_drawUTF8(&gfx, 20, 180, "敬老の日");
        GFX_drawUTF8(&gfx, 150, 180, "国民の休日");
        GFX_drawUTF8(&gfx, 20, 204, "秋分の日");

        GFX_setFont(&gfx, u8g2_font_jp_ui_medium11);
        GFX_setTextColor(&gfx, GFX_BLACK, GFX_WHITE);
        GFX_drawUTF8(&gfx, 20, 232, "曜 國 国 勤 謝 憲 龍 馬");

        GFX_setFont(&gfx, u8g2_font_helvB14_tn);
        GFX_drawUTF8(&gfx, 20, 275, "0123456789");
    } while (GFX_nextPage(&gfx, collect_page, NULL));
    GFX_end(&gfx);
    if (write_ppm(argv[1]) != 0) return 1;
    printf("Font sheet rendered: %s\n", argv[1]);
    return 0;
}
