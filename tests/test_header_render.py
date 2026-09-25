"""Pixel-level regression for the 400x300 Japanese calendar preview."""

from pathlib import Path
from PIL import Image


ROOT = Path(__file__).resolve().parents[1]
before = Image.open(ROOT / "artifacts/japanese-calendar-v2/calendar-2026-09-25.png").convert("RGB")
after = Image.open(ROOT / "artifacts/header-web-i18n/calendar-2026-09-25.png").convert("RGB")
assert before.size == after.size == (400, 300)

# The weekday bar is compressed into the new header's 52..63 pixel band;
# everything from the first day row down must remain byte-for-byte identical.
assert before.crop((0, 64, 400, 300)).tobytes() == after.crop((0, 64, 400, 300)).tobytes()

red = (255, 0, 0)
black = (0, 0, 0)
white = (255, 255, 255)
for y in range(52):
    assert after.getpixel((0, y)) == red
    assert after.getpixel((70, y)) == black
assert after.getpixel((65, 10)) == white
assert after.getpixel((288, 10)) == white
assert after.getpixel((296, 27)) == white

# Ensure the year and month both render within the red block, with a visible
# gap, and the device-name/battery rows stay inside the status block.
year_pixels = [(x, y) for y in range(2, 25) for x in range(58)
               if after.getpixel((x, y)) == white]
month_pixels = [(x, y) for y in range(25, 51) for x in range(58)
                if after.getpixel((x, y)) == white]
assert year_pixels and month_pixels
assert max(y for _, y in year_pixels) < min(y for _, y in month_pixels)
assert any(after.getpixel((x, y)) == white for x in range(296, 390) for y in range(3, 25))
assert any(after.getpixel((x, y)) == white for x in range(296, 390) for y in range(30, 50))

print("Header render tests: PASS (400x300, fixed dividers, year/month separation, status, calendar body unchanged)")
