"""Exercise real firmware bitplanes: selecting today must not alter another cell.

Run with --cc gcc (MinGW on Windows). Pillow is needed only on the host.
"""
import argparse
import calendar
from datetime import datetime, timezone
from pathlib import Path
import subprocess
import tempfile

from PIL import Image, ImageChops, ImageDraw

ROOT = Path(__file__).resolve().parents[1]
SOURCES = ['tests/render_calendar.c', 'GUI/Adafruit_GFX.c', 'GUI/u8g2_font.c',
           'GUI/fonts.c', 'GUI/GUI.c', 'GUI/JapaneseCalendar.c',
           'GUI/JapaneseRokuyo.c', 'GUI/Lunar.c']


def build_renderer(cc, output, heap=8192):
    subprocess.run([cc, '-Wall', '-IGUI', f'-D__HEAP_SIZE={heap}', *SOURCES,
                    '-o', str(output)], cwd=ROOT, check=True)


def render(exe, output, year, month, day, week_start):
    timestamp = int(datetime(year, month, day, tzinfo=timezone.utc).timestamp())
    subprocess.run([str(exe), str(timestamp), str(output), str(week_start)],
                   check=True, stdout=subprocess.DEVNULL)
    with Image.open(output) as image:
        return image.convert('RGB')


# Independent equal-height fixtures. The 204px grid leaves a separate footer.
ROW_HEIGHTS = {
    (2026, 11, 1): [34] * 6, (2026, 11, 0): [40] * 5,
    (2026, 9, 1): [40] * 5, (2026, 9, 0): [40] * 5,
    (2026, 8, 1): [34] * 6, (2026, 8, 0): [34] * 6,
    (2027, 2, 1): [51] * 4, (2027, 2, 0): [40] * 5,
    (2019, 10, 1): [40] * 5, (2019, 10, 0): [40] * 5,
    (2019, 5, 1): [40] * 5, (2019, 5, 0): [40] * 5,
}

def cell(year, month, day, week_start):
    first, _ = calendar.monthrange(year, month)
    offset = ((first + 1) % 7 - week_start) % 7
    heights = ROW_HEIGHTS[year, month, week_start]
    index = offset + day - 1
    row = index // 7
    x, y = 4 + index % 7 * 56, 69 + sum(heights[:row])
    return x, y, x + 56, y + heights[row]


def check_month(exe, directory, year, month, week_start):
    reference = render(exe, directory / 'reference.ppm', year, month, 1, week_start)
    count = 0
    for day in range(2, calendar.monthrange(year, month)[1] + 1):
        current = render(exe, directory / 'current.ppm', year, month, day, week_start)
        changed = ImageChops.difference(reference, current)
        mask = Image.new('L', current.size, 255)
        draw = ImageDraw.Draw(mask)
        draw.rectangle((0, 0, 399, 68), fill=0)  # Header week number also changes.
        draw.rectangle((0, 278, 399, 299), fill=0)  # Selected holiday's footer.
        for selected in (1, day):
            x0, y0, x1, y1 = cell(year, month, selected, week_start)
            draw.rectangle((x0, y0, x1 - 1, y1 - 1), fill=0)
        outside = ImageChops.multiply(changed, mask.convert('RGB')).getbbox()
        assert outside is None, (
            f'{year}-{month:02}-{day:02} week_start={week_start}: '
            f'today changes pixels outside its cell: {outside}')
        count += 1
    return count


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--cc', default='gcc')
    parser.add_argument('--quick', action='store_true')
    args = parser.parse_args()
    try:
        with tempfile.TemporaryDirectory(prefix='epd-cell-test-') as temporary:
            directory = Path(temporary)
            exe = directory / 'render.exe'
            build_renderer(args.cc, exe)
            # First case matches the reported six-row Monday-first layout.
            cases = [(2026, 11, 1)] if args.quick else [
                (year, month, start) for year, month in
                [(2026, 11), (2026, 9), (2027, 2), (2026, 8), (2019, 10), (2019, 5)]
                for start in (1, 0)]
            count = sum(check_month(exe, directory, *case) for case in cases)
            if not args.quick:
                full = directory / 'full.exe'
                build_renderer(args.cc, full, 32768)
                for case in [(2026, 11, 21, 1), (2019, 10, 22, 0)]:
                    paged = render(exe, directory / 'paged.ppm', *case)
                    whole = render(full, directory / 'whole.ppm', *case)
                    assert paged.tobytes() == whole.tobytes(), 'paged bitplanes differ'
        print(f'Calendar cell tests: PASS ({count} selections; '
              f'{len(cases)} month/week-start layouts)')
        return 0
    except AssertionError as error:
        print(f'Calendar cell tests: FAIL ({error})')
        return 1


if __name__ == '__main__':
    raise SystemExit(main())
