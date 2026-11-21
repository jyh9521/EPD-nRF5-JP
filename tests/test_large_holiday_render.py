"""Keep historic long holidays inside supported large-panel columns."""
import argparse
from datetime import datetime, timezone
from pathlib import Path
import subprocess
import tempfile

from PIL import Image
from test_calendar_cells import ROOT, SOURCES


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--cc', default='gcc')
    parser.add_argument('--mutate-fallback', action='store_true',
                        help='prove this regression test rejects the broken same-font fallback')
    args = parser.parse_args()
    failures = []
    with tempfile.TemporaryDirectory(prefix='epd-large-font-') as temp:
        temp = Path(temp)
        gui = ROOT / 'GUI/GUI.c'
        if args.mutate_fallback:
            gui = temp / 'GUI_mutant.c'
            gui.write_text((ROOT / 'GUI/GUI.c').read_text(encoding='utf-8').replace(
                'u8g2_font_jp_holiday_legacy', 'u8g2_font_wqy9_t_jp'), encoding='utf-8')
        for width, height in [(600, 448), (648, 480)]:
            runner = temp / 'render.c'
            runner.write_text((ROOT / 'tests/render_calendar.c').read_text().replace(
                '#define WIDTH 400', f'#define WIDTH {width}').replace(
                '#define HEIGHT 300', f'#define HEIGHT {height}'), encoding='utf-8')
            sources = [str(runner) if s == 'tests/render_calendar.c' else
                       str(gui) if s == 'GUI/GUI.c' else s for s in SOURCES]
            exe = temp / 'large.exe'
            subprocess.run([args.cc, '-IGUI', '-D__HEAP_SIZE=8192', *sources,
                            '-o', str(exe)], cwd=ROOT, check=True)
            for month, selected, column, row in [(5, 2, 3, 0), (10, 23, 2, 3)]:
                stamp = int(datetime(2019, month, selected, tzinfo=timezone.utc).timestamp())
                ppm = temp / 'large.ppm'
                subprocess.run([str(exe), str(stamp), str(ppm), '0'], check=True,
                               stdout=subprocess.DEVNULL)
                with Image.open(ppm) as image:
                    bw, bh = (width - 20) // 7, (height - 94) // 5
                    left, top = 10 + column * bw, 84 + row * bh
                    hits = [(x, y) for y in range(top + 32, top + bh - 2)
                            for x in range(left - 10, left + bw + 10)
                            if image.getpixel((x, y)) == (255, 0, 0)]
                    assert hits, (width, height, month)
                    bounds = min(x for x, _ in hits), max(x for x, _ in hits)
                    if bounds[0] < left or bounds[1] >= left + bw:
                        failures.append((width, height, month, bounds, (left, left+bw-1)))
    print(f'Large holiday render tests: {"FAIL" if failures else "PASS"} '
          f'(4 cases; overflow={len(failures)})')
    return int(bool(failures))


if __name__ == '__main__':
    raise SystemExit(main())
