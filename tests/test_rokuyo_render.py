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
    assert count(august, (10, 258, 120, 290), black) > 30
    assert count(september, (4, 295, 396, 296), black) >= 390
    # Rokuyo labels remain black even below red Sunday dates.
    assert count(september, (20, 147, 45, 159), black) > 10
    assert count(september, (20, 147, 45, 159), red) == 0
    # Holiday names and side rails are red; today is a red card with white date.
    assert count(holiday_today, (65, 204, 112, 245), red) > 1100
    assert count(holiday_today, (70, 210, 108, 240), white) > 30
    assert count(holiday_today, (117, 205, 119, 244), red) > 60
    assert count(holiday_today, (173, 205, 175, 244), red) > 60
    for box in ((120, 230, 170, 244), (176, 230, 226, 244)):
        assert count(holiday_today, box, red) > 30
    print("Rokuyo render tests: PASS (5-row, 6-row, black Rokuyo, holiday rails, today card)")


if __name__ == "__main__":
    main(*sys.argv[1:4])
