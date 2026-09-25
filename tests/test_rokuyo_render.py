"""Pixel-level smoke checks for the 400x300 September/August calendar renders."""

import sys
from PIL import Image


def count(image, box, color):
    return list(image.crop(box).get_flattened_data()).count(color)


def main(september_path, august_path):
    september = Image.open(september_path).convert("RGB")
    august = Image.open(august_path).convert("RGB")
    assert september.size == august.size == (400, 300)
    red, black = (255, 0, 0), (0, 0, 0)
    # Sunday 6: red date above, black 先勝 below.
    assert count(september, (12, 129, 60, 143), black) > 100
    assert count(september, (12, 129, 60, 143), red) == 0
    # Thursday 10: 大安 alone is red, independent of the black date.
    assert count(september, (229, 129, 275, 143), red) > 80
    assert count(september, (229, 129, 275, 143), black) == 0
    # Holiday labels take the lower slot on 21 and 23.
    assert count(september, (67, 213, 114, 231), red) > 100
    assert count(september, (175, 213, 221, 231), red) > 100
    # Today retains a red circle, white date, and readable black 六曜.
    assert count(september, (282, 200, 330, 239), red) > 1000
    assert count(september, (282, 200, 330, 239), black) > 100
    # August has six rows; the final row remains within the 300-pixel panel.
    assert count(august, (10, 258, 120, 289), black) > 100
    print("Rokuyo render tests: PASS (5-row, 6-row, colors, holidays, today)")


if __name__ == "__main__":
    main(sys.argv[1], sys.argv[2])
