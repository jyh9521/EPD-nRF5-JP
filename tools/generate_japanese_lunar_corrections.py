"""Convert the independently audited mismatch days to compact firmware windows.

Input is the local audit produced from rekichu.com's Japanese old-calendar
year views (2000-2050), compared against the pre-existing lunar converter.
This generator is not needed by the firmware build.
"""

from datetime import date, timedelta
from pathlib import Path
import re
import sys


def key(value):
    return (value.year - 2000) * 512 + (value - date(value.year, 1, 1)).days + 1


def main(source, target):
    rows = []
    pattern = re.compile(
        r"(\d{4}-\d{2}-\d{2}) Japanese=\((\d+), (\d+), (\d+)\) "
        r"Existing=\((\d+), (\d+), (\d+)\)"
    )
    for line in Path(source).read_text(encoding="utf-8").splitlines():
        match = pattern.fullmatch(line)
        assert match, line
        rows.append((date.fromisoformat(match[1]), tuple(map(int, match.groups()[1:4]))))
    assert len(rows) == 718

    windows = []
    start = previous = rows[0][0]
    first_lunar = previous_lunar = rows[0][1]
    for current, lunar in rows[1:]:
        consecutive = (
            current == previous + timedelta(days=1)
            and current.year == previous.year
            and lunar[0] == previous_lunar[0]
            and lunar[2] == previous_lunar[2]
            and lunar[1] == previous_lunar[1] + 1
        )
        if not consecutive:
            windows.append((start, previous, first_lunar))
            start, first_lunar = current, lunar
        previous, previous_lunar = current, lunar
    windows.append((start, previous, first_lunar))

    lines = [
        "/* Japanese old-calendar correction windows against the existing lunar table.",
        " * Source: https://rekichu.com/rekichu/kyureki/?yyyy=YYYY (2000-2050).",
        " * Each entry is {Gregorian start key, Gregorian end key, lunar month,",
        " * day at start, leap}. Key = (year-2000)*512 + day-of-year.",
        " * Windows do not cross Gregorian years or lunar month boundaries. */",
    ]
    for start, end, (month, day, leap) in windows:
        lines.append(
            f"    {{{key(start)}, {key(end)}, {month}, {day}, {leap}}}, "
            f"/* {start} .. {end} */"
        )
    Path(target).write_text("\n".join(lines) + "\n", encoding="utf-8")
    print(f"audited_days={len(rows)} correction_windows={len(windows)}")


if __name__ == "__main__":
    main(sys.argv[1], sys.argv[2])
