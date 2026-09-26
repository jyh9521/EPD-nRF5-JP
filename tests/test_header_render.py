"""Reference-image geometry regression for the 400x300 Japanese calendar."""

from pathlib import Path
import sys
from PIL import Image

ROOT = Path(__file__).resolve().parents[1]
reference = Image.open(ROOT / "tests/fixtures/reference-2026-09-21.png").convert("RGB")
candidate_path = Path(sys.argv[1]) if len(sys.argv) > 1 else ROOT / "artifacts/reference-redesign/calendar-sep21.png"
candidate = Image.open(candidate_path).convert("RGB")
assert reference.size == candidate.size == (400, 300)


def box(image, region, color):
    x0, y0, x1, y1 = region
    points = [(x, y) for y in range(y0, y1) for x in range(x0, x1)
              if image.getpixel((x, y)) == color]
    assert points, (region, color)
    return min(x for x, _ in points), min(y for _, y in points), max(x for x, _ in points), max(y for _, y in points)


white, black, red = (255, 255, 255), (0, 0, 0), (255, 0, 0)
reference_red = (205, 0, 0)

# Fixed geometry from the 1200x900 supplied image, downscaled exactly 3:1.
for image in (reference, candidate):
    assert image.getpixel((0, 52)) in (red, reference_red)
    assert image.getpixel((59, 52)) == black
    assert image.getpixel((66, 12)) == white
    assert image.getpixel((310, 12)) == white
    assert image.getpixel((320, 22)) == white
    assert image.getpixel((5, 74)) == black
    assert image.getpixel((5, 295)) == black

for region, color, tolerance in [
    ((0, 0, 59, 24), white, 3),       # Year
    ((0, 24, 59, 53), white, 2),      # Month
    ((135, 78, 153, 100), black, 2), # First date
    ((23, 123, 41, 144), red, 2),    # First Sunday in row 2
]:
    source_color = reference_red if color == red else color
    a, b = box(reference, region, source_color), box(candidate, region, color)
    assert all(abs(u - v) <= tolerance for u, v in zip(a, b)), (region, a, b)

# The selected holiday and two adjoining holidays must occupy the same cells.
for image, color in ((reference, reference_red), (candidate, red)):
    today = box(image, (60, 200, 115, 247), color)
    assert today[0] == 65 and today[1] == 204 and today[2] >= 110 and today[3] == 244
    assert all(any(image.getpixel((x, y)) == color for y in range(205, 244))
               for x in (120, 176))

print("Reference render tests: PASS (400x300, header divisions, weekday rules, date rows, today and holiday cells)")
