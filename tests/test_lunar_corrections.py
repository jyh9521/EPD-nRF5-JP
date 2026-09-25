"""Verify every independently audited Japanese/Chinese old-date difference."""

from pathlib import Path
import re
import subprocess
import sys


def main(probe, audit):
    expected = {}
    pattern = re.compile(r"(\d{4}-\d{2}-\d{2}) Japanese=\((\d+), (\d+), (\d+)\) Existing=.*")
    for line in Path(audit).read_text(encoding="utf-8").splitlines():
        match = pattern.fullmatch(line)
        assert match, line
        date, month, day, leap = match.groups()
        year = int(date[:4]) - (int(date[5:7]) <= 3 and int(month) >= 10)
        expected[date] = (year, int(month), int(day), int(leap))
    assert len(expected) == 718
    result = subprocess.run([probe], input="\n".join(expected) + "\n", text=True,
                            capture_output=True, check=True)
    actual = {}
    for line in result.stdout.splitlines():
        date, *fields = line.split(",")
        actual[date] = tuple(map(int, fields))
    assert actual == expected, [
        (date, expected[date], actual.get(date)) for date in expected
        if actual.get(date) != expected[date]
    ][:10]
    print(f"Japanese lunar correction tests: PASS ({len(expected)} independent mismatch dates)")


if __name__ == "__main__":
    main(*sys.argv[1:])
