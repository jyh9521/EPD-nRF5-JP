#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "GUI.h"

#define WIDTH 400
#define HEIGHT 300
#define ROW_BYTES (WIDTH / 8)

static uint8_t black_plane[ROW_BYTES * HEIGHT];
static uint8_t red_plane[ROW_BYTES * HEIGHT];

static void collect_page(void* user_data, uint8_t* black, uint8_t* red, uint16_t x, uint16_t y, uint16_t w,
                         uint16_t h) {
    uint16_t row;
    uint16_t bytes = (uint16_t)(w / 8U);
    (void)user_data;
    for (row = 0; row < h && y + row < HEIGHT; row++) {
        memcpy(&black_plane[(y + row) * ROW_BYTES + x / 8U], &black[row * bytes], bytes);
        memcpy(&red_plane[(y + row) * ROW_BYTES + x / 8U], &red[row * bytes], bytes);
    }
}

static int write_ppm(const char* path) {
    FILE* file = fopen(path, "wb");
    uint16_t y;
    if (file == NULL) return 1;
    fprintf(file, "P6\n%d %d\n255\n", WIDTH, HEIGHT);
    for (y = 0; y < HEIGHT; y++) {
        uint16_t x;
        for (x = 0; x < WIDTH; x++) {
            uint32_t offset = (uint32_t)y * ROW_BYTES + x / 8U;
            uint8_t mask = (uint8_t)(0x80U >> (x % 8U));
            uint8_t rgb[3] = {255, 255, 255};
            if ((red_plane[offset] & mask) == 0U) {
                rgb[1] = 0;
                rgb[2] = 0;
            } else if ((black_plane[offset] & mask) == 0U) {
                rgb[0] = 0;
                rgb[1] = 0;
                rgb[2] = 0;
            }
            fwrite(rgb, 1, sizeof(rgb), file);
        }
    }
    return fclose(file) == 0 ? 0 : 1;
}

int main(int argc, char** argv) {
    gui_data_t data = {
        MODE_CALENDAR, 2, WIDTH, HEIGHT, 1790294400UL, 0, 25, 2920, "NRF_EPD_6177",
    };
    const char* output = argc > 2 ? argv[2] : "calendar.ppm";
    if (argc > 1) data.timestamp = (uint32_t)strtoul(argv[1], NULL, 0);
    if (argc > 3) data.week_start = (uint8_t)strtoul(argv[3], NULL, 0);
    memset(black_plane, 0xFF, sizeof(black_plane));
    memset(red_plane, 0xFF, sizeof(red_plane));
    DrawGUI(&data, collect_page, NULL);
    if (write_ppm(output) != 0) return 1;
    printf("Rendered %s for timestamp %lu, week_start %u\n", output,
           (unsigned long)data.timestamp, data.week_start);
    return 0;
}
