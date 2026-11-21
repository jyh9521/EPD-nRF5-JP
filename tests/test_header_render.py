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
    assert image.getpixel((5, 74 if image is reference else 67)) == black
    assert image.getpixel((5, 295 if image is reference else 277)) == black

for region, color, tolerance in [
    ((0, 0, 59, 24), white, 3),       # Year
    ((0, 24, 59, 53), white, 2),      # Month
]:
    source_color = reference_red if color == red else color
    a, b = box(reference, region, source_color), box(candidate, region, color)
    assert all(abs(u - v) <= tolerance for u, v in zip(a, b)), (region, a, b)

# Native-font grid starts at y=69; all five rows are 40px high.
# The independent footer divider is at y=277.
today = box(candidate, (60, 189, 116, 229), red)
assert today == (61, 190, 114, 227), today
assert candidate.getpixel((116, 210)) == red
assert candidate.getpixel((172, 210)) == red
assert box(candidate, (135, 74, 153, 91), black)[1::2] == (75, 87)
print("Reference render tests: PASS (header retained; equal rows and separate footer)")
