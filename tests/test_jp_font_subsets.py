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
ui = bdf_codes(fonts / 'epd_jp_ui_medium11.bdf')
holiday = bdf_codes(fonts / 'epd_jp_holiday_medium10.bdf')
rokuyo = bdf_codes(fonts / 'epd_jp_rokuyo_regular13.bdf')
assert ui == set(lunar_codes())
assert holiday == set(holiday_codes())
assert rokuyo == set(rokuyo_codes())

actual = set()
for path in (ROOT / 'GUI' / 'GUI.c', ROOT / 'GUI' / 'JapaneseCalendar.c',
             ROOT / 'GUI' / 'JapaneseRokuyo.c'):
    source = path.read_text(encoding='utf-8')
    for string in re.findall(r'"([^"\\]*)"', source):
        actual.update(ord(char) for char in string if ord(char) > 127)
assert actual <= ui | holiday | rokuyo, ''.join(map(chr, sorted(actual - ui - holiday - rokuyo)))
assert ord('_') in ui
heights = bdf_heights(fonts / 'epd_jp_rokuyo_regular13.bdf')
assert all(heights[ord(char)] >= 11 for char in '勝負仏滅')

source = (ROOT / 'tools' / 'build_compact_font.py').read_text(encoding='utf-8')
assert '.resize(' not in source
assert "anchor='ls'" in source
assert 'size=13' in source and 'size=11' in source and 'size=10' in source

print(f'Japanese font subset tests: PASS (UI={len(ui)}, holiday={len(holiday)}, '
      f'rokuyo={len(rokuyo)}, covered_strings={len(actual)} codepoints)')
