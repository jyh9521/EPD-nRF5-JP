"""Native glyphs must not overlap the next character's advance cell."""
from pathlib import Path
import re
import sys

ROOT = Path(__file__).resolve().parents[1]


def check(path):
    for glyph in path.read_text(encoding='ascii').split('STARTCHAR ')[1:]:
        code = int(re.search(r'^ENCODING (\d+)', glyph, re.M)[1])
        if code <= 127:
            continue
        advance = int(re.search(r'^DWIDTH (\d+)', glyph, re.M)[1])
        w, h, x, y = map(int, re.search(r'^BBX (\d+) (\d+) (-?\d+) (-?\d+)', glyph, re.M).groups())
        if x < 0 or x + w > advance:
            print(f'Holiday spacing tests: FAIL (U+{code:04X}: ink_right={x+w-1}, '
                  f'advance={advance}; glyph overlaps the next advance cell)')
            return 1
    print('Holiday spacing tests: PASS (all CJK glyphs fit their native advance cells)')
    return 0


if __name__ == '__main__':
    raise SystemExit(check(Path(sys.argv[1]) if len(sys.argv) > 1 else
                          ROOT / 'tools/fonts/epd_jp_wqy12.bdf'))
