"""Pixel-level smoke checks for the 400x300 September/August calendar renders."""

import sys
from PIL import Image


def count(image, box, color):
    return list(image.crop(box).get_flattened_data()).count(color)


def main(september_path, august_path, holiday_today_path):
    september = Image.open(september_path).convert("RGB")
    august = Image.open(august_path).convert("RGB")
    holiday_today = Image.open(holiday_today_path).convert("RGB")
    assert september.size == august.size == holiday_today.size == (400, 300)
    red, black = (255, 0, 0), (0, 0, 0)
    # Sunday 6: red date above, black 先勝 below.
    assert count(september, (12, 129, 60, 143), black) > 100
    assert count(september, (12, 129, 60, 143), red) == 0
    # Thursday 10: 大安 is black, including ordinary weekdays.
    assert count(september, (229, 129, 275, 143), red) == 0
    assert count(september, (229, 129, 275, 143), black) > 80
    # All three consecutive holidays show formal red names, not 六曜.
    assert count(september, (67, 213, 114, 231), red) > 100
    assert count(september, (120, 213, 168, 231), red) > 100
    assert count(september, (175, 213, 221, 231), red) > 100
    for x in (94, 148, 202):
        assert count(september, (x, 199, x + 12, 213), red) > 10
    # An ordinary Sunday is red but has no 休 badge.
    assert count(september, (48, 199, 56, 213), red) == 0
    # Today retains a red circle, white date, and readable black 六曜.
    assert count(september, (282, 200, 330, 239), red) > 1000
    assert count(september, (282, 200, 330, 239), black) > 100
    # The holiday-today case retains its circle, formal label and 休.
    assert count(holiday_today, (67, 200, 114, 239), red) > 500
    assert count(holiday_today, (94, 191, 106, 213), red) > 10
    # August has six rows; the final row remains within the 300-pixel panel.
    assert count(august, (10, 258, 120, 289), black) > 100
    print("Rokuyo render tests: PASS (5-row, 6-row, black Rokuyo, formal holidays, badge, today-holiday)")


if __name__ == "__main__":
    main(sys.argv[1], sys.argv[2], sys.argv[3])
