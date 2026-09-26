"""Regenerate the checked-in 1-bit Japanese UI subset from Noto Sans JP.

The generated BDF is committed so ordinary firmware builds need no TTF or Pillow.
Usage: python build_compact_font.py PATH_TO_NotoSansJP-VF.ttf
"""

from pathlib import Path
import re
import sys

TARGET = Path('fonts/epd_jp_ui_medium.bdf')
HOLIDAYS = Path('../GUI/JapaneseCalendar.c')
NARROW = {ord(char) for char in 'の即位礼正殿儀'}
ADVANCE = 8
HEIGHT = 10
THRESHOLD = 140


def holiday_codes():
    source = HOLIDAYS.read_text(encoding='utf-8')
    names = source.split('HOLIDAY_NAMES[] = {')[1].split('};', 1)[0]
    text = ''.join(re.findall(r'"([^"]*)"', names)) + '体育の日休先勝友引先負仏滅大安赤口'
    return sorted({ord(char) for char in text if ord(char) > 127} | {ord('A'), ord('g')})


def build_compact_bdf(font_path: Path, target: Path = TARGET):
    from PIL import Image, ImageFont

    font = ImageFont.truetype(str(font_path), 56)
    font.set_variation_by_name('Medium')
    codes = holiday_codes()
    lines = [
        'STARTFONT 2.1',
        'FONT -epd-jp-ui-medium-r-normal--10-100-75-75-p-80-iso10646-1',
        'SIZE 10 75 75', 'FONTBOUNDINGBOX 8 10 0 -1',
        'STARTPROPERTIES 2', 'FONT_ASCENT 9', 'FONT_DESCENT 2',
        'ENDPROPERTIES', f'CHARS {len(codes)}',
    ]
    for code in codes:
        char = chr(code)
        advance = 7 if code in NARROW else ADVANCE
        height = 8 if char in ('A', 'g') else HEIGHT
        y_offset = 0 if char == 'A' else -2 if char == 'g' else -1
        mask = font.getmask(char, mode='L')
        bitmap = Image.frombytes('L', mask.size, bytes(mask))
        bounds = bitmap.getbbox()
        if not bounds:
            raise ValueError(f'Missing glyph U+{code:04X}')
        bitmap = bitmap.crop(bounds).resize((advance, height), Image.Resampling.LANCZOS)
        rows = []
        for y in range(height):
            bits = sum(1 << (7 - x) for x in range(advance)
                       if bitmap.getpixel((x, y)) >= THRESHOLD)
            rows.append(f'{bits:02X}')
        lines += [
            f'STARTCHAR epd-{code:04X}', f'ENCODING {code}',
            f'SWIDTH {advance * 80} 0', f'DWIDTH {advance} 0',
            f'BBX {advance} {height} 0 {y_offset}', 'BITMAP', *rows, 'ENDCHAR',
        ]
    lines += ['ENDFONT', '']
    target.write_text('\n'.join(lines), encoding='ascii')
    return codes


if __name__ == '__main__':
    if len(sys.argv) != 2:
        raise SystemExit('usage: python build_compact_font.py PATH_TO_NotoSansJP-VF.ttf')
    print(f'Generated {TARGET} with {len(build_compact_bdf(Path(sys.argv[1])))} glyphs')
