"""Pixel smoke checks for the redesigned 400x300 Japanese calendar."""

import sys
from PIL import Image


def count(image, box, color):
    return list(image.crop(box).get_flattened_data()).count(color)


def main(september_path, august_path, holiday_today_path):
    september = Image.open(september_path).convert("RGB")
    august = Image.open(august_path).convert("RGB")
    holiday_today = Image.open(holiday_today_path).convert("RGB")
    assert september.size == august.size == holiday_today.size == (400, 300)
    red, black, white = (255, 0, 0), (0, 0, 0), (255, 255, 255)
    # Six-week August and five-week September stay on the panel.
    assert count(august, (10, 239, 120, 273), black) > 30
    assert count(september, (4, 277, 396, 278), black) >= 390
    # Rokuyo labels remain black even below red Sunday dates.
    assert count(september, (19, 130, 45, 141), black) > 10
    assert count(september, (19, 130, 45, 141), red) == 0
    # Holiday names and side rails are red; today is a red card with white date.
    assert count(holiday_today, (61, 190, 115, 228), red) > 1100
    assert count(holiday_today, (66, 194, 109, 225), white) > 30
    assert count(holiday_today, (116, 191, 117, 227), red) > 30
    assert count(holiday_today, (172, 191, 173, 227), red) > 30
    for box in ((119, 210, 170, 221), (175, 210, 226, 221)):
        assert count(holiday_today, box, red) > 30
    print("Rokuyo render tests: PASS (5-row, 6-row, black Rokuyo, holiday rails, today card)")


if __name__ == "__main__":
    main(*sys.argv[1:4])
