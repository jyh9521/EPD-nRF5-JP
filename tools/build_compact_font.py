"""Subset upstream's native 12px WenQuanYi font without resampling.

Usage: python tools/build_compact_font.py
All inputs are checked in; no system TTF/Pillow/rasterizer is needed.
"""

from pathlib import Path
import re


ROOT = Path(__file__).resolve().parents[1]
FONT_DIR = ROOT / 'tools' / 'fonts'
ASCII_CODES = set(range(32, 127))
ROKUYO_TEXT = '大安赤口先勝友引先負仏滅'
COMPLEX_TEST_GLYPHS = '曜國国勤謝憲龍馬'


def _strings(source: str) -> str:
    return ''.join(re.findall(r'"([^"\\]*)"', source))


def lunar_codes():
    # The legacy font.txt also contains unused lunar/date strings. Keep every
    # current UI/calendar string (including dynamic eto names), not that old
    # superset: nRF52811 has only 52 KiB of DFU application flash.
    text = ''.join(_strings((ROOT / 'GUI' / name).read_text(encoding='utf-8'))
                   for name in ('GUI.c', 'JapaneseCalendar.c', 'JapaneseRokuyo.c'))
    lunar = (ROOT / 'GUI' / 'Lunar.c').read_text(encoding='utf-8')
    for name in ('Lunar_DayString', 'Lunar_MonthString', 'Lunar_MonthLeapString', 'Lunar_DateString'):
        text += _strings(lunar.split(name, 1)[1].split('};', 1)[0])
    return sorted({ord(c) for c in text + COMPLEX_TEST_GLYPHS if ord(c) > 127} | ASCII_CODES)


def holiday_codes():
    source = (ROOT / 'GUI' / 'JapaneseCalendar.c').read_text(encoding='utf-8')
    names = source.split('HOLIDAY_NAMES[] = {')[1].split('};', 1)[0]
    text = _strings(names) + '体育の日休'
    return sorted({ord(c) for c in text if ord(c) > 127} | {ord('A'), ord('g')})


def rokuyo_codes():
    return sorted({ord(c) for c in ROKUYO_TEXT + 'Ag'})


def subset_bdf(source: Path, target: Path, codes: list[int]):
    """Keep original pixel-designed glyphs/metrics; only subset Unicode entries."""
    text = source.read_text(encoding='ascii')
    header = text.split('CHARS ', 1)[0]
    glyphs = {}
    for block in text.split('STARTCHAR ')[1:]:
        code = int(re.search(r'^ENCODING (\d+)$', block, re.MULTILINE).group(1))
        glyphs[code] = 'STARTCHAR ' + block.split('ENDCHAR', 1)[0] + 'ENDCHAR\n'
    missing = set(codes) - glyphs.keys()
    if missing:
        raise ValueError(f'missing glyphs: {sorted(missing)}')
    target.write_text(header + f'CHARS {len(codes)}\n' +
                      ''.join(glyphs[code] for code in sorted(codes)) + 'ENDFONT\n',
                      encoding='ascii')


def generate_all():
    codes = lunar_codes()
    subset_bdf(FONT_DIR / 'wenquanyi_9ptb.bdf', FONT_DIR / 'epd_jp_wqy12.bdf', codes)
    print(f'epd_jp_wqy12.bdf: {len(codes)} native glyphs')


if __name__ == '__main__':
    generate_all()
