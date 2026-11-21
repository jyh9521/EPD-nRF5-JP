"""Match actual 1-bit glyph pixels to the upstream native 12px font."""
from pathlib import Path
import re
import sys

ROOT = Path(__file__).resolve().parents[1]


def read_bdf(path):
    result = {}
    for block in path.read_text(encoding='utf-8').split('STARTCHAR ')[1:]:
        code = int(re.search(r'^ENCODING (\d+)', block, re.M)[1])
        advance = int(re.search(r'^DWIDTH (\d+)', block, re.M)[1])
        w, h, x, y = map(int, re.search(r'^BBX (\d+) (\d+) (-?\d+) (-?\d+)', block, re.M).groups())
        rows = block.split('BITMAP\n')[1].split('ENDCHAR')[0].split()
        bits = (w + 7) // 8 * 8
        pixels = {(x + col, -h-y + row) for row, value in enumerate(rows)
                  for col in range(w) if int(value, 16) & (1 << (bits-col-1))}
        result[code] = advance, pixels
    return result


def main():
    source = read_bdf(ROOT / 'tools/fonts/wenquanyi_9ptb.bdf')
    target_path = Path(sys.argv[1]) if len(sys.argv) > 1 else ROOT / 'tools/fonts/epd_jp_wqy12.bdf'
    candidate = read_bdf(target_path)
    for char in '令和霜月第週大安赤口先勝友引先負仏滅勤労感謝の日文化休':
        if candidate.get(ord(char)) != source[ord(char)]:
            print(f'Native font tests: FAIL (U+{ord(char):04X} differs from upstream 12px pixels/advance)')
            return 1
    assert all(candidate[code] == source[code] for code in candidate)
    print(f'Native font tests: PASS ({len(candidate)} glyphs exactly match checked-in native pixels/advance)')
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
