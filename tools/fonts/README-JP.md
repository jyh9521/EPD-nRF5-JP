# Japanese glyph subset

`tools/font.txt` lists the 9pt glyphs used by the calendar and other UI. `tools/font-large.txt`
keeps the 12pt subset limited to the existing clock UI. `build_compact_font.py` extracts every
official holiday name from `GUI/JapaneseCalendar.c` and generates an 8px-advance, 11px-high
holiday-label subset (7px advance for selected long-name glyphs). Run `python build_fonts.py`
from `tools` to regenerate all three fonts in `GUI/fonts.c` and `GUI/fonts.h`. Temporary BDF,
map, and C files are removed by the script.

WenQuanYi 9pt did not contain U+30FC KATAKANA-HIRAGANA PROLONGED SOUND MARK, which is required by
`スポーツ`. A 12-pixel horizontal glyph was added to `wenquanyi_9ptb.bdf` using the same 13-pixel
advance and bounding box conventions as the adjacent 9pt Katakana glyphs. The 12pt source already
contains U+30FC. This keeps the generated subset reproducible without adding a complete Japanese font.
