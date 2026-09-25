# Japanese glyph subset

`tools/font.txt` is the single list of non-ASCII characters included in both firmware fonts. Run
`python build_fonts.py` from the `tools` directory to regenerate `GUI/fonts.c` and `GUI/fonts.h`.

WenQuanYi 9pt did not contain U+30FC KATAKANA-HIRAGANA PROLONGED SOUND MARK, which is required by
`スポーツ`. A 12-pixel horizontal glyph was added to `wenquanyi_9ptb.bdf` using the same 13-pixel
advance and bounding box conventions as the adjacent 9pt Katakana glyphs. The 12pt source already
contains U+30FC. This keeps the generated subset reproducible without adding a complete Japanese font.
