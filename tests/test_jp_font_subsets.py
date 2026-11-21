"""Check that checked-in BDFs cover the Japanese Edition's actual text."""

from pathlib import Path
import re
import sys

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))
from build_compact_font import holiday_codes, lunar_codes, rokuyo_codes  # noqa: E402


def bdf_codes(path):
    text = path.read_text(encoding='ascii')
    result = {int(value) for value in re.findall(r'^ENCODING (\d+)$', text, re.MULTILINE)}
    count = int(re.search(r'^CHARS (\d+)$', text, re.MULTILINE).group(1))
    assert len(result) == count, (path, len(result), count)
    return result


def bdf_heights(path):
    result = {}
    for block in path.read_text(encoding='ascii').split('STARTCHAR ')[1:]:
        code = int(re.search(r'^ENCODING (\d+)$', block, re.MULTILINE).group(1))
        height = int(re.search(r'^BBX \d+ (\d+) ', block, re.MULTILINE).group(1))
        result[code] = height
    return result


fonts = ROOT / 'tools' / 'fonts'
ui = bdf_codes(fonts / 'epd_jp_wqy12.bdf')
holiday = ui
rokuyo = ui
assert ui == set(lunar_codes())
assert set(holiday_codes()) <= holiday
assert set(rokuyo_codes()) <= rokuyo

actual = set()
for path in (ROOT / 'GUI' / 'GUI.c', ROOT / 'GUI' / 'JapaneseCalendar.c',
             ROOT / 'GUI' / 'JapaneseRokuyo.c'):
    source = path.read_text(encoding='utf-8')
    for string in re.findall(r'"([^"\\]*)"', source):
        actual.update(ord(char) for char in string if ord(char) > 127)
assert actual <= ui | holiday | rokuyo, ''.join(map(chr, sorted(actual - ui - holiday - rokuyo)))
assert ord('_') in ui
# Clock mode uses these dynamic Lunar.c arrays with the same small UI font.
# Keep an independent fixture: the generator's source selection can be wrong.
clock_text = '星期一二三四五六日闰正月七八九十冬腊初廿'
missing_clock = {ord(c) for c in clock_text} - ui
assert not missing_clock, f'missing clock glyphs: {sorted(missing_clock)}'
heights = bdf_heights(fonts / 'epd_jp_wqy12.bdf')
assert all(heights[ord(char)] >= 11 for char in '勝負仏滅')

source = (ROOT / 'tools' / 'build_compact_font.py').read_text(encoding='utf-8')
assert '.resize(' not in source


print(f'Japanese font subset tests: PASS (native={len(ui)}, covered_strings={len(actual)} codepoints)')
