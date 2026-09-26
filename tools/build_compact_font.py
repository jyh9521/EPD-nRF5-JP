"""Make a narrow, full-height BDF subset for official holiday labels.

Glyphs come from the existing WenQuanYi 9pt source. Only horizontal bitmap
columns are compacted; the original 11-pixel Kanji height is retained.
"""

from pathlib import Path
import re


SOURCE = Path("fonts/wenquanyi_9ptb.bdf")
TARGET = Path("_jp_holiday_compact.bdf")
HOLIDAYS = Path("../GUI/JapaneseCalendar.c")
ADVANCE = 8
NARROW = {ord(char) for char in "の即位礼正殿儀"}


def holiday_codes():
    source = HOLIDAYS.read_text(encoding="utf-8")
    names = source.split("HOLIDAY_NAMES[] = {")[1].split("};", 1)[0]
    text = "".join(re.findall(r'"([^"]*)"', names)) + "体育の日休先勝友引先負仏滅大安赤口"
    # bdfconv derives ascent_A/descent_g from these reference glyphs; without
    # them GFX_getFontHeight() becomes zero and two-line labels overlap.
    return sorted({ord(char) for char in text if ord(char) > 127} | {ord("A"), ord("g")})


def build_compact_bdf():
    wanted = set(holiday_codes())
    lines = SOURCE.read_text(encoding="ascii", errors="ignore").splitlines()
    glyphs = []
    index = 0
    while index < len(lines):
        if not lines[index].startswith("STARTCHAR "):
            index += 1
            continue
        end = lines.index("ENDCHAR", index) + 1
        block = lines[index:end]
        encoding = next((int(row.split()[1]) for row in block if row.startswith("ENCODING ")), -1)
        if encoding in wanted:
            advance = 7 if encoding in NARROW else ADVANCE
            bbx = next(row for row in block if row.startswith("BBX "))
            width, height, x_offset, y_offset = map(int, bbx.split()[1:])
            bitmap_index = block.index("BITMAP")
            bitmap = block[bitmap_index + 1 : bitmap_index + 1 + height]
            columns = []
            for row in bitmap:
                bits = int(row, 16) >> (len(row) * 4 - width)
                columns.append([bool(bits & (1 << (width - 1 - col))) for col in range(width)])
            output = []
            for row in columns:
                packed = 0
                for dest in range(advance):
                    left = dest * 13 // advance
                    right = (dest + 1) * 13 // advance
                    on = any(row[col - x_offset] for col in range(left, right)
                             if x_offset <= col < x_offset + width)
                    if on:
                        packed |= 1 << (7 - dest)
                output.append(f"{packed:02X}")
            glyphs += [
                f"STARTCHAR jp-{encoding:04x}",
                f"ENCODING {encoding}",
                f"SWIDTH {advance * 80} 0",
                f"DWIDTH {advance} 0",
                f"BBX {advance} {height} 0 {y_offset}",
                "BITMAP",
                *output,
                "ENDCHAR",
            ]
            wanted.remove(encoding)
        index = end
    if wanted:
        raise ValueError(f"missing WenQuanYi glyphs: {sorted(wanted)}")
    header = [
        "STARTFONT 2.1",
        "FONT -jp-holiday-compact-medium-r-normal--12-120-75-75-p-70-iso10646-1",
        "SIZE 12 75 75",
        "FONTBOUNDINGBOX 8 17 0 -2",
        "STARTPROPERTIES 2",
        "FONT_ASCENT 12",
        "FONT_DESCENT 3",
        "ENDPROPERTIES",
        f"CHARS {len(holiday_codes())}",
    ]
    TARGET.write_text("\n".join(header + glyphs + ["ENDFONT", ""]), encoding="ascii")
    return holiday_codes()


if __name__ == "__main__":
    print(f"compact glyphs={len(build_compact_bdf())}")
