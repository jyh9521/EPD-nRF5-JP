"""Generate checked-in, 1-bit Japanese UI glyph subsets directly at target pixels.

Usage: python tools/build_compact_font.py PATH_TO_NotoSansJP-VF.ttf
Requires Pillow on the build PC only. Normal firmware builds use the checked-in
BDF subsets and do not load or parse TTF/OTF on the nRF52811.
"""

from pathlib import Path
import re
import sys


ROOT = Path(__file__).resolve().parents[1]
FONT_DIR = ROOT / 'tools' / 'fonts'
ASCII_CODES = set(range(32, 127))
ROKUYO_TEXT = '大安赤口先勝友引先負仏滅'
HOLIDAY_NARROW = 'の即位礼正殿儀'
COMPLEX_TEST_GLYPHS = '曜國国勤謝憲龍馬'


def _strings(source: str) -> str:
    return ''.join(re.findall(r'"([^"\\]*)"', source))


def lunar_codes():
    source = (ROOT / 'tools' / 'font.txt').read_text(encoding='utf-8')
    return sorted({ord(c) for c in source + COMPLEX_TEST_GLYPHS if ord(c) > 127} | ASCII_CODES)


def holiday_codes():
    source = (ROOT / 'GUI' / 'JapaneseCalendar.c').read_text(encoding='utf-8')
    names = source.split('HOLIDAY_NAMES[] = {')[1].split('};', 1)[0]
    text = _strings(names) + '体育の日休'
    return sorted({ord(c) for c in text if ord(c) > 127} | {ord('A'), ord('g')})


def rokuyo_codes():
    return sorted({ord(c) for c in ROKUYO_TEXT + 'Ag'})


def generate_bdf(font_path: Path, target: Path, *, size: int, weight: str,
                 codes: list[int], threshold: int, cjk_advance: int | None = None,
                 narrow: str = ''):
    from PIL import ImageFont

    font = ImageFont.truetype(str(font_path), size)
    font.set_variation_by_name(weight)
    lines = [
        'STARTFONT 2.1',
        f'FONT -epd-noto-jp-{weight.lower()}-r-normal--{size}-{size * 10}-75-75-p-100-iso10646-1',
        f'SIZE {size} 75 75', f'FONTBOUNDINGBOX {size + 2} {size + 4} -1 -3',
        'STARTPROPERTIES 2', f'FONT_ASCENT {size + 2}', 'FONT_DESCENT 3',
        'ENDPROPERTIES', f'CHARS {len(codes)}',
    ]
    for code in sorted(codes):
        char = chr(code)
        mask, (offset_x, offset_y) = font.getmask2(char, mode='L', anchor='ls')
        width, height = mask.size
        hits = [(x, y) for y in range(height) for x in range(width)
                if mask.getpixel((x, y)) >= threshold]
        advance = max(1, round(font.getlength(char)))
        if cjk_advance is not None and code > 127:
            advance = cjk_advance - (char in narrow)
        if hits:
            x0, x1 = min(x for x, _ in hits), max(x for x, _ in hits) + 1
            y0, y1 = min(y for _, y in hits), max(y for _, y in hits) + 1
            glyph_width, glyph_height = x1 - x0, y1 - y0
            xoff, yoff = offset_x + x0, -(offset_y + y1)
            byte_count = (glyph_width + 7) // 8
            rows = []
            for y in range(y0, y1):
                bits = sum(1 << (byte_count * 8 - 1 - (x - x0))
                           for x in range(x0, x1) if mask.getpixel((x, y)) >= threshold)
                rows.append(f'{bits:0{byte_count * 2}X}')
        else:
            glyph_width = glyph_height = 1
            xoff = yoff = 0
            rows = ['00']
        lines += [
            f'STARTCHAR epd-{code:04X}', f'ENCODING {code}',
            f'SWIDTH {advance * 80} 0', f'DWIDTH {advance} 0',
            f'BBX {glyph_width} {glyph_height} {xoff} {yoff}',
            'BITMAP', *rows, 'ENDCHAR',
        ]
    lines += ['ENDFONT', '']
    target.write_text('\n'.join(lines), encoding='ascii')
    return len(codes)


def generate_all(font_path: Path):
    specs = [
        ('epd_jp_ui_medium11.bdf', dict(size=11, weight='Medium',
                                      codes=lunar_codes(), threshold=110)),
        ('epd_jp_holiday_medium10.bdf', dict(size=10, weight='Medium',
                                           codes=holiday_codes(), threshold=105,
                                           cjk_advance=8, narrow=HOLIDAY_NARROW)),
        ('epd_jp_rokuyo_regular13.bdf', dict(size=13, weight='Regular',
                                           codes=rokuyo_codes(), threshold=95)),
    ]
    for name, options in specs:
        count = generate_bdf(font_path, FONT_DIR / name, **options)
        print(f'{name}: {count} glyphs')


if __name__ == '__main__':
    if len(sys.argv) != 2:
        raise SystemExit('usage: python tools/build_compact_font.py PATH_TO_NotoSansJP-VF.ttf')
    generate_all(Path(sys.argv[1]))
