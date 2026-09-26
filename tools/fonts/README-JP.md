# Japanese glyph subsets

`tools/font.txt` lists the 9pt glyphs used by the existing calendar and UI. `tools/font-large.txt` keeps the 12pt subset limited to the existing clock UI. The existing WenQuanYi BDFs are unchanged, including the U+30FC glyph required by `スポーツ`.

The holiday and rokuyo labels use a separate, checked-in 1-bit bitmap subset of **Noto Sans JP Medium**: `tools/fonts/epd_jp_ui_medium.bdf`. It contains the 68 characters used by the Japanese calendar, at 8 px advance (7 px for selected narrow characters) and 10 px height. Its license is `tools/fonts/OFL-NotoSansJP.txt`.

Run `python build_fonts.py` from `tools` to regenerate `GUI/fonts.c` and `GUI/fonts.h` from the checked-in BDFs. Normal firmware builds do not need a system Japanese font or Pillow. To regenerate the Noto subset from the upstream variable TTF, run `python build_compact_font.py PATH_TO_NotoSansJP-VF.ttf` from `tools` (requires Pillow), then run `python build_fonts.py`.

The three-colour e-paper display uses monochrome glyph bitmaps; the new outlines improve this small-size typeface without introducing grayscale antialiasing.
