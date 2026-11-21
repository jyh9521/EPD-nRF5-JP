"""Read-only renderer probe: expose hidden white-on-white ink as black pixels.

Run from EPD-nRF5-JP checkout with MinGW gcc on PATH. Only temporary copies
of Adafruit_GFX.c are instrumented; the reviewed checkout is never modified.
"""
import argparse
import calendar
import subprocess
import sys
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tests'))
from test_calendar_cells import SOURCES, cell, render


def replace_required(source, old, new):
    assert old in source, f"instrumentation no longer matches: {old}"
    return source.replace(old, new)


def check(cc):
    with tempfile.TemporaryDirectory(prefix='epd-review-') as temporary:
        directory = Path(temporary)
        source = (ROOT / 'GUI/Adafruit_GFX.c').read_text(encoding='utf-8')
        source = replace_required(source,
            'void GFX_setTextColor(Adafruit_GFX* gfx, uint16_t fg, uint16_t bg) {',
            'void GFX_setTextColor(Adafruit_GFX* gfx, uint16_t fg, uint16_t bg) {\n'
            '    if (fg == GFX_WHITE && bg == GFX_RED) fg = GFX_BLACK;')
        source = replace_required(source,
            'void GFX_drawRect(Adafruit_GFX* gfx, int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {',
            'void GFX_drawRect(Adafruit_GFX* gfx, int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {\n'
            '    if (color == GFX_WHITE) color = GFX_BLACK;')
        probe = directory / 'GFX_probe.c'
        probe.write_text(source, encoding='utf-8')
        sources = [str(probe) if item == 'GUI/Adafruit_GFX.c' else item for item in SOURCES]
        executable = directory / 'probe.exe'
        subprocess.run([cc, '-IGUI', '-D__HEAP_SIZE=32768', *sources,
                        '-o', str(executable)], cwd=ROOT, check=True)
        failures = []
        count = 0
        for year, month in [(2026, 11), (2026, 9), (2027, 2), (2026, 8),
                            (2019, 10), (2019, 5)]:
            for week in (1, 0):
                for day in range(1, calendar.monthrange(year, month)[1] + 1):
                    image = render(executable, directory / 'probe.ppm',
                                   year, month, day, week)
                    x0, y0, x1, y1 = cell(year, month, day, week)
                    # The topmost red run excludes two rounded-corner columns.
                    top = [x for x in range(x0, x1)
                           if image.getpixel((x, y0 + 1)) == (255, 0, 0)]
                    assert top, ('no top red', year, month, day, week)
                    left, right = min(top) - 2, max(top) + 2
                    for y in range(y0, y1):
                        for x in range(x0, x1):
                            if image.getpixel((x, y)) != (0, 0, 0):
                                continue
                            inset = (2 if y in (y0 + 1, y1 - 2) else
                                     1 if y in (y0 + 2, y1 - 3) else 0)
                            if not (y0 + 1 <= y <= y1 - 2 and
                                    left + inset <= x <= right - inset):
                                failures.append((year, month, day, week,
                                                 x - x0, y - y0,
                                                 left - x0, right - x0))
                                break
                    count += 1
        # Includes single-line aliases and full-size holiday badges.
        print('TODAY CARD INK CHECK', count, 'FAILURES', len(failures), failures[:30])
        return int(bool(failures))


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--cc', default='gcc')
    raise SystemExit(check(parser.parse_args().cc))
