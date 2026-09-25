#!/usr/bin/env python3
"""Reproducible GNU Arm build for the Keil nRF52811_xxAA target."""

import argparse
import os
from pathlib import Path
import shlex
import subprocess
import xml.etree.ElementTree as ET


def run(command, cwd):
    print("+", shlex.join(str(item) for item in command))
    subprocess.run([str(item) for item in command], cwd=cwd, check=True)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--toolchain", default=os.environ.get("ARM_GCC_ROOT"))
    parser.add_argument("--source-root", type=Path, default=Path(__file__).resolve().parents[1])
    parser.add_argument("--output", type=Path)
    args = parser.parse_args()

    root = args.source_root.resolve()
    output = (args.output or root / "build" / "nrf52811").resolve()
    if not args.toolchain:
        parser.error("pass --toolchain or set ARM_GCC_ROOT")
    toolchain = Path(args.toolchain).resolve()
    gcc = toolchain / "bin" / "arm-none-eabi-gcc.exe"
    objcopy = toolchain / "bin" / "arm-none-eabi-objcopy.exe"
    size = toolchain / "bin" / "arm-none-eabi-size.exe"
    for program in (gcc, objcopy, size):
        if not program.is_file():
            parser.error(f"missing tool: {program}")

    project = root / "Keil" / "EPD-nRF52.uvprojx"
    target = ET.parse(project).getroot().find(".//Target")
    if target is None or target.findtext("TargetName") != "nRF52811_xxAA":
        raise SystemExit("nRF52811_xxAA target not found")

    project_dir = project.parent
    sources = []
    for node in target.findall(".//FilePath"):
        path = (project_dir / node.text.replace("\\", os.sep)).resolve()
        if path.suffix.lower() not in (".c", ".s"):
            continue
        if path.name == "app_error_handler_keil.c":
            path = path.with_name("app_error_handler_gcc.c")
        elif path.name == "SEGGER_RTT_Syscalls_KEIL.c":
            path = path.with_name("SEGGER_RTT_Syscalls_GCC.c")
        elif path.name == "arm_startup_nrf52811.s":
            path = path.with_name("gcc_startup_nrf52811.S")
        sources.append(path)

    include_paths = {root, root / "EPD", root / "GUI", root / "SDK" / "17.1.0_ddde560"}
    for node in target.findall(".//IncludePath"):
        for entry in (node.text or "").split(";"):
            if entry:
                include_paths.add((project_dir / entry.replace("\\", os.sep)).resolve())

    defines_node = target.find(".//Cads/VariousControls/Define")
    if defines_node is None:
        raise SystemExit("target defines not found")
    defines = (defines_node.text or "").split()

    output.mkdir(parents=True, exist_ok=True)
    object_dir = output / "obj"
    object_dir.mkdir(exist_ok=True)
    common = [
        "-mcpu=cortex-m4", "-mthumb", "-mabi=aapcs", "-mfloat-abi=soft",
        "-Os", "-flto", "-Wall", "-Werror=return-type", "-ffunction-sections",
        "-fdata-sections", "-fno-strict-aliasing", "-fshort-enums",
    ]
    include_args = [arg for path in sorted(include_paths) for arg in ("-I", str(path))]
    define_args = [f"-D{item}" for item in defines]
    objects = []
    for index, source in enumerate(sources):
        if not source.is_file():
            raise SystemExit(f"missing source: {source}")
        obj = object_dir / f"{index:02d}_{source.stem}.o"
        language = ["-x", "assembler-with-cpp"] if source.suffix == ".S" else ["-std=gnu11"]
        run([gcc, *common, *language, *define_args, *include_args, "-c", source, "-o", obj], root)
        objects.append(obj)

    linker = root / "Keil" / "gcc_nrf52811_s112_ota.ld"
    elf = output / "EPD-nRF52811-JP.elf"
    map_file = output / "EPD-nRF52811-JP.map"
    sdk_mdk = root / "SDK" / "17.1.0_ddde560" / "modules" / "nrfx" / "mdk"
    run([
        gcc, *common, *objects, "-T", linker, "-L", sdk_mdk,
        "--specs=nano.specs", "--specs=nosys.specs", "-Wl,--gc-sections",
        f"-Wl,-Map={map_file}", "-o", elf,
    ], root)

    hex_file = output / "EPD-nRF52811-JP.hex"
    bin_file = output / "EPD-nRF52811-JP.bin"
    run([objcopy, "-O", "ihex", elf, hex_file], root)
    run([objcopy, "-O", "binary", elf, bin_file], root)
    run([size, "-A", elf], root)
    print(f"ELF={elf}")
    print(f"HEX={hex_file}")
    print(f"BIN={bin_file}")


if __name__ == "__main__":
    main()
