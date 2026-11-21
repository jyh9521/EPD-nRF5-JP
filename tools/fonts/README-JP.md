# Japanese UI bitmap fonts

The 400x300 calendar uses the original project's pixel-designed 12px
WenQuanYi font, not small outline-font rasterizations or narrowed holiday
characters. It sends black, white and red pixels; grayscale antialiasing
is not part of the display data.

| Use | Checked-in BDF | Glyphs |
|---|---|---:|
| Compact header, weekdays, Rokuyo, holidays, clock metadata | `epd_jp_wqy12.bdf` | 242 |
| Legacy large-display overflow fallback only | `epd_jp_holiday_k8x12.bdf` | 57 |
| Large clock text (unchanged) | `wenquanyi_12ptb.bdf` subset | 83 |

The compact subset preserves every source bitmap and advance exactly.
Typical Japanese glyphs have a 12x11 ink box and a 13px advance; some kana
occupy the full 13px cell. Characters are not squeezed into smaller advances.
The month/day Helvetica bitmaps remain unchanged.

## Source and reproduction

The [original project at 7e319619](https://github.com/tsl0922/EPD-nRF5/tree/7e3196193997bd6a4644610c41d54a76eff88760)
uses `wenquanyi_9ptb.bdf` for its small text. Despite its filename, its BDF
SIZE is 12. This fork's existing copy adds U+30FC (the Japanese long-vowel
mark); the other source glyphs match upstream. The source file SHA-256 is
`a8d409e43dd135a9d3ce0b37f07ef34c9833136a38abf2fe2db79de1a26af2de`.

All native-font inputs are checked in. Generation subsets BDF records
without resampling; it does not need Pillow, FreeType or a system TTF.
`build_fonts.py` uses the repository's bdfconv tool to produce C arrays.

```cmd
python tools\build_compact_font.py
cd tools
python build_fonts.py
cd ..
python tests\test_native_font.py
python tests\test_jp_font_subsets.py
python tests\test_font_spacing.py
make test
python tests\test_calendar_cells.py --cc gcc
python tests\test_today_card.py
python tests\test_large_holiday_render.py
```

The 242-character set includes GUI/calendar strings and four dynamic Lunar.c
arrays used by clock mode. An independent fixture checks clock characters.
No complete BDF is stored in the firmware.

k8x12 is retained only for the pre-existing larger-display overflow path,
not the 400x300 layout. It is Num Kadoma's 2021-05-05 release:
[upstream](https://littlelimit.net/k8x12.htm),
[BDF archive](https://littlelimit.net/arc/k8x12/k8x12_bdf_2021-05-05.zip).
Source SHA-256: `97361b327b410c2cd6e8fe87ed3873387229e76af13829931d6987f267585277`.
Attribution and license: `LICENSE-k8x12.txt`.

## Layout (single-line, version 0x20)

Compact cells display a short holiday name on one native-font line. Full
names remain unchanged in JapaneseCalendar.c and are shown in the separate
footer for the selected holiday. The footer stays blank on non-holidays.
Aliases are display-only: e.g. 勤労感謝の日 -> 勤労感謝 and
即位礼正殿の儀の行われる日 -> 即位礼. No glyphs are scaled or squeezed.

At 400x300, the grid starts at y=69 and has a 204px budget. Every week in
the same month uses exactly floor(204 / week_count) pixels: 34px for six
weeks, 40px for five, 51px for four. Remainder pixels stay below the grid.
The footer divider is y=277 and the text baseline is y=294.

Tests cover 10,080 equal-row month/start layouts (1980-2099, all seven starts),
1,930 single-line aliases and complete footer widths, actual text traces for
4/5/6-row months, 350 selection transitions, 362 today-card ink checks, and
four larger-display long-holiday cases. Images are actual firmware 1-bit
plane renders. Version 0x20 packages this visually approved layout;
physical display readability and DFU acceptance remain to be checked on hardware.
