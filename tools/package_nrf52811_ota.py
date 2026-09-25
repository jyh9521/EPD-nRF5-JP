#!/usr/bin/env python3
"""Validate and package an nRF52811 application for the repository bootloader."""

import argparse
import hashlib
import json
from pathlib import Path
import re
import subprocess


APP_START = 0x19000
APP_END = 0x26000  # exclusive; 0x26000+ is reserved by DFU/FDS/bootloader data

def application_version(root):
    header = (root / "EPD" / "EPD_service.h").read_text(encoding="utf-8")
    match = re.search(r"^#define\s+APP_VERSION\s+(0x[0-9a-fA-F]+|[0-9]+)\b", header, re.MULTILINE)
    if match is None:
        raise ValueError("APP_VERSION macro not found")
    return match.group(1)


def run(command, cwd):
    print("+", " ".join(str(item) for item in command))
    subprocess.run([str(item) for item in command], cwd=cwd, check=True)


def intel_hex_extent(path):
    upper = 0
    minimum = None
    maximum = None
    byte_count = 0
    with path.open("r", encoding="ascii") as stream:
        for line_number, line in enumerate(stream, 1):
            line = line.strip()
            if not line:
                continue
            if not line.startswith(":"):
                raise ValueError(f"{path}:{line_number}: invalid Intel HEX record")
            record = bytes.fromhex(line[1:])
            if (sum(record) & 0xFF) != 0:
                raise ValueError(f"{path}:{line_number}: checksum mismatch")
            length = record[0]
            address = (record[1] << 8) | record[2]
            record_type = record[3]
            data = record[4 : 4 + length]
            if record_type == 0:
                absolute = upper + address
                minimum = absolute if minimum is None else min(minimum, absolute)
                maximum = absolute + length if maximum is None else max(maximum, absolute + length)
                byte_count += length
            elif record_type == 4:
                upper = int.from_bytes(data, "big") << 16
            elif record_type == 2:
                upper = int.from_bytes(data, "big") << 4
    if minimum is None or maximum is None:
        raise ValueError(f"{path}: no data records")
    return minimum, maximum, byte_count


def sha256(path):
    digest = hashlib.sha256()
    with path.open("rb") as stream:
        for chunk in iter(lambda: stream.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("hex", type=Path)
    parser.add_argument("--output", type=Path)
    args = parser.parse_args()

    root = Path(__file__).resolve().parents[1]
    app_version = application_version(root)
    tools = root / "tools"
    source_hex = args.hex.resolve()
    output = (args.output or source_hex.parent).resolve()
    output.mkdir(parents=True, exist_ok=True)

    start, end, byte_count = intel_hex_extent(source_hex)
    if start != APP_START or end > APP_END:
        raise SystemExit(
            f"unsafe application range 0x{start:05X}-0x{end - 1:05X}; "
            f"required 0x{APP_START:05X}-0x{APP_END - 1:05X}"
        )

    stem = "EPD-nRF52811-JP"
    ota_zip = output / f"{stem}-ota.zip"
    settings = output / f"{stem}-settings.hex"
    full_hex = output / f"{stem}-full.hex"
    private_key = tools / "priv.pem"
    bootloader = tools / "bootloader" / "bl_nrf52811_xxaa_s112.hex"
    softdevice = root / "SDK" / "17.1.0_ddde560" / "components" / "softdevice" / "s112" / "hex" / "s112_nrf52_7.3.0_softdevice.hex"
    nrfutil = tools / "bin" / "nrfutil.exe"
    mergehex = tools / "bin" / "mergehex.exe"

    run([nrfutil, "pkg", "generate", "--application", source_hex, "--key-file", private_key,
         "--hw-version", "52", "--sd-req", "0x126", "--sd-id", "0x126",
         "--application-version", app_version, ota_zip], root)
    run([nrfutil, "settings", "generate", "--family", "NRF52810", "--application", source_hex,
         "--softdevice", softdevice, "--application-version", app_version,
         "--bootloader-version", "1", "--bl-settings-version", "1", "--key-file", private_key,
         "--no-backup", settings], root)
    run([mergehex, "-m", softdevice, bootloader, source_hex, settings, "-o", full_hex], root)
    run([nrfutil, "pkg", "display", ota_zip], root)

    manifest = {
        "application_version": app_version,
        "softdevice_id": "0x126",
        "application_start": f"0x{start:05X}",
        "application_end_exclusive": f"0x{end:05X}",
        "application_data_bytes": byte_count,
        "files": {},
    }
    for path in (source_hex, ota_zip, settings, full_hex):
        manifest["files"][path.name] = {"bytes": path.stat().st_size, "sha256": sha256(path)}
    manifest_path = output / f"{stem}-manifest.json"
    manifest_path.write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")
    print(manifest_path.read_text(encoding="utf-8"))


if __name__ == "__main__":
    main()
