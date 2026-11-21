"""Trace real firmware text: single cell labels, equal rows, full footer names."""
import argparse
import calendar
from datetime import datetime, timezone
from pathlib import Path
import re
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]
SOURCES = ['tests/render_calendar.c', 'GUI/Adafruit_GFX.c', 'GUI/u8g2_font.c',
           'GUI/fonts.c', 'GUI/GUI.c', 'GUI/JapaneseCalendar.c',
           'GUI/JapaneseRokuyo.c', 'GUI/Lunar.c']


def check(root, cc='gcc'):
    cases = [(2026, 11, 23, 1, '勤労感謝', '勤労感謝の日'),
             (2019, 10, 22, 0, '即位礼', '即位礼正殿の儀の行われる日'),
             (2019, 5, 1, 1, '天皇即位', '天皇の即位の日'),
             (2027, 2, 11, 1, '建国記念', '建国記念の日'),
             (2026, 11, 21, 1, '仏滅', None)]
    failures = []
    with tempfile.TemporaryDirectory(prefix='epd-single-line-') as temporary:
        temp = Path(temporary)
        source = (root / 'GUI/Adafruit_GFX.c').read_text(encoding='utf-8')
        old = '    len = GFX_write(gfx, buf, len);'
        assert source.count(old) == 1
        source = source.replace(old,
            '    fprintf(stderr, "TRACE\\t%d\\t%d\\t%d\\t%s\\n", gfx->tx, gfx->ty, '
            'GFX_getUTF8Width(gfx, buf), buf);\n' + old)
        probe = temp / 'GFX_trace.c'
        probe.write_text(source, encoding='utf-8')
        sources = [str(probe) if s == 'GUI/Adafruit_GFX.c' else s for s in SOURCES]
        exe = temp / 'trace.exe'
        subprocess.run([cc, '-IGUI', '-D__HEAP_SIZE=32768', *sources, '-o', str(exe)],
                       cwd=root, check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        for year, month, selected, week_start, short, full in cases:
            stamp = int(datetime(year, month, selected, tzinfo=timezone.utc).timestamp())
            run = subprocess.run([str(exe), str(stamp), str(temp / 'frame.ppm'), str(week_start)],
                                 check=True, capture_output=True, encoding='utf-8')
            trace = [(int(x), int(y), int(w), s) for x, y, w, s in
                     re.findall(r'^TRACE\t(-?\d+)\t(-?\d+)\t(\d+)\t(.*)$', run.stderr, re.M)]
            first, days = calendar.monthrange(year, month)
            offset = ((first + 1) % 7 - week_start) % 7
            rows = (offset + days + 6) // 7
            height = 204 // rows
            try:
                for day in range(1, days + 1):
                    row, col = divmod(offset + day - 1, 7)
                    left, top = 4 + col * 56, 69 + row * height
                    date_y = top + 15 + (height - 31) // 2
                    dates = [t for t in trace if t[3] == str(day) and 69 <= t[1] < 277]
                    assert len(dates) == 1 and dates[0][1] == date_y, f'day {day}: unequal row/date baseline'
                    labels = [t for t in trace if left <= t[0] < left + 56 and
                              top <= t[1] < top + height and not t[3].isdigit() and t[3] != '休']
                    assert len(labels) == 1, f'day {day}: {len(labels)} label lines'
                    x, y, width, label = labels[0]
                    assert y == date_y + 13 and x + width <= left + 55 and width <= 51, f'day {day}: label bounds'
                    if day == selected:
                        assert label == short, f'day {day}: expected {short}, got {label}'
                footer = [t for t in trace if t[1] >= 278]
                if full:
                    assert len(footer) == 1 and footer[0][3] == f'{month:02d}/{selected:02d}  {full}', 'full footer missing/truncated'
                    assert footer[0][1] == 294 and footer[0][0] + footer[0][2] <= 392, 'footer bounds'
                else:
                    assert not footer, 'non-holiday footer must be empty'
            except AssertionError as error:
                failures.append(f'{year}-{month:02d}-{selected:02d}: {error}')
    print(f'Single-line layout tests: {"FAIL" if failures else "PASS"} ({len(cases)} cases; failures={len(failures)})')
    for failure in failures:
        print(failure)
    return int(bool(failures))


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--root', type=Path, default=ROOT)
    parser.add_argument('--cc', default='gcc')
    args = parser.parse_args()
    raise SystemExit(check(args.root.resolve(), args.cc))
