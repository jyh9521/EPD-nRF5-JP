# Japanese UI bitmap fonts

The 400×300 calendar uses three checked-in, 1-bit subsets generated directly
at their final pixel sizes from **Noto Sans JP** variable TTF:

| UI use | BDF | Weight / raster size | Characters | CJK advance |
|---|---|---:|---:|---:|
| Header metadata, weekdays, ordinary small UI text | `epd_jp_ui_medium11.bdf` | Medium, 11 px | 262 | 11 px |
| Holiday names and 休 badges | `epd_jp_holiday_medium10.bdf` | Medium, 10 px | 57 | 8 px (7 px for a few long-name characters) |
| Rokuyō labels | `epd_jp_rokuyo_regular12.bdf` | Regular, 12 px | 13 | 12 px |

The bitmap masks are sampled from the TTF **at 12, 11, or 10 pixels** with the
baseline anchor. There is no large intermediate bitmap, image resize, or
runtime TTF parsing. BDF generation thresholds are defined in
`tools/build_compact_font.py`; the compiled glyphs remain `const` in flash.
The 400×300 day numerals and month number retain their existing bold Helvetica
bitmap font. The status device name retains its existing narrow pixel lettering.
The larger display's `u8g2_font_wqy12_t_lunar` remains unchanged.

The subset generator derives its character lists from `tools/font.txt`,
`GUI/JapaneseCalendar.c`, and the six Rokuyō names; the metadata subset also
contains the diagnostic complex glyphs. The holiday font includes the legacy
`体育の日` spelling for years before 2020. Full Noto Sans JP is not embedded.
The `FONT_ASCENT` metadata and source TTF pixel size are not a substitute for
checking actual rendered glyph bounds in the final firmware-bitmap preview.

The upstream font is available from
[Google Fonts' Noto Sans JP directory](https://github.com/google/fonts/tree/main/ofl/notosansjp).
The included derivative BDFs are covered by `OFL-NotoSansJP.txt`. The full
system TTF is intentionally not committed.

To reproduce the BDFs on a PC with Pillow installed:

```cmd
python tools\build_compact_font.py C:\Windows\Fonts\NotoSansJP-VF.ttf
cd tools
python build_fonts.py
```

Normal firmware builds use the checked-in BDFs and do not require the TTF or
Pillow. `build_fonts.py` regenerates `GUI/fonts.c` and `GUI/fonts.h` from these
subsets and the unchanged larger-display and numeric bitmap sources.

The panel is black/white/red only. No grayscale antialiasing is sent to the
device; inspect the native 400×300 or nearest-neighbor preview for pixel-level
readability.
