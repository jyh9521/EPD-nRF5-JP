"""Measure the firmware-rendered September 2026 holiday row and draw debug overlays.

Input is the PPM emitted by tests/render_calendar.c, which calls the firmware
DrawGUI and uses the same compiled font arrays and two E-paper planes.
"""

from pathlib import Path
import sys

from PIL import Image, ImageDraw


def bbox_for(image, left, right, top, bottom, color):
    pixels = image.load()
    hits = [(x, y) for y in range(top, bottom + 1)
            for x in range(left, right + 1) if pixels[x, y] == color]
    assert hits, (left, right, top, bottom, color)
    return (min(x for x, _ in hits), min(y for _, y in hits),
            max(x for x, _ in hits), max(y for _, y in hits))


def main(source, destination):
    image = Image.open(source).convert('RGB')
    assert image.size == (400, 300)
    red = (255, 0, 0)
    white = (255, 255, 255)
    specs = [(21, 60), (22, 116), (23, 172)]
    bounds = {}
    for day, left in specs:
        center = left + 28
        # The badge is to the right of the date; the narrow x-window excludes it.
        date_color = white if day == 21 else red
        date_box = bbox_for(image, center - 15, center + 7, 215, 230, date_color)
        label_color = white if day == 21 else red
        label_box = bbox_for(image, left + 4, left + 55, 233, 245, label_color)
        bar_box = None
        if day != 21:
            bar_box = bbox_for(image, left, left + 3, 205, 245, red)
            assert bar_box[2] - bar_box[0] + 1 == 2, (day, bar_box)
            gap = label_box[0] - bar_box[2] - 1
            assert gap >= 2, (day, gap, bar_box, label_box)
            date_visual_center = (date_box[0] + date_box[2]) / 2
            label_visual_center = (label_box[0] + label_box[2]) / 2
            assert abs(date_visual_center - label_visual_center) <= 0.5, (
                day, date_visual_center, label_visual_center)
            print(f'{day}: bar={bar_box} label={label_box} gap={gap}px')
        else:
            # Today + holiday has a red card but no separate narrow bar.
            assert image.getpixel((left + 1, 220)) == white
            print(f'{day}: today card; separate_bar=none')
        bounds[day] = (left, center, date_box, label_box, bar_box)
        print(f'{day}: cellCenterX={center} date_bbox={date_box} '
              f'label_bbox={label_box} layout_anchor={center}')

    # Sep 26 is a normal (non-today, non-holiday) white cell in this preview.
    assert image.getpixel((350, 242)) == white
    assert bbox_for(image, 350, 387, 234, 245, (0, 0, 0))
    print('26: ordinary white cell with black Rokuyo; PASS')

    crop_x, crop_y, scale = 60, 202, 8
    debug = image.crop((crop_x, crop_y, 228, 249)).resize((168 * scale, 47 * scale),
                                                       Image.Resampling.NEAREST)
    draw = ImageDraw.Draw(debug)
    def line_x(x, color):
        px = (x - crop_x) * scale
        draw.line((px, 0, px, debug.height - 1), fill=color, width=1)
    def box(rect, color):
        x0, y0, x1, y1 = rect
        draw.rectangle(((x0 - crop_x) * scale, (y0 - crop_y) * scale,
                        (x1 - crop_x + 1) * scale - 1,
                        (y1 - crop_y + 1) * scale - 1), outline=color, width=2)
    for day, (left, center, date_box, label_box, bar_box) in bounds.items():
        line_x(left, (0, 200, 0))          # left boundary
        line_x(left + 56, (0, 200, 0))     # right boundary
        line_x(center, (0, 90, 255))       # shared layout anchor
        box(date_box, (0, 230, 0))         # date glyph bounding box
        box(label_box, (0, 220, 220))      # holiday glyph bounding box
        if bar_box:
            box(bar_box, (255, 150, 0))    # independent bar
    debug.save(destination)
    print(f'debug={destination}')


if __name__ == '__main__':
    if len(sys.argv) != 3:
        raise SystemExit('usage: calendar_display_quality.py INPUT.ppm DEBUG.png')
    main(Path(sys.argv[1]), Path(sys.argv[2]))
