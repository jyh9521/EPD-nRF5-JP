# EPD-nRF5-JP (Japanese Edition)

This is the Japanese Edition fork of [tsl0922/EPD-nRF5](https://github.com/tsl0922/EPD-nRF5). It provides a Japanese monthly calendar, Japanese national holidays, Reiwa era display, and the original clock/image functions. The first target is an nRF52811 with a 4.2-inch 400×300 UC8176 black/white/red e-paper display. The original BLE display, image transfer, daily refresh, and low-power architecture remain in place.

Supported MCUs: `nrf51822` / `nrf51802` / `nrf52811` / `nrf52810`. For e-paper displays, it supports common `UC81xx` / `SSD16xx` series drivers (black & white / 3-color / 4-color). It also supports custom pin mapping from the e-paper display to the MCU, sleep/wake functionality (NFC / wireless charger), and Bluetooth OTA firmware updates.

![](docs/images/3.jpg)

## Web Interface

This repository's `html/` directory is the official Web Bluetooth controller for this fork. The existing GitHub Actions Pages workflow publishes that directory when changes reach `main` and GitHub Pages is configured for GitHub Actions.

- URL: https://jyh9521.github.io/EPD-nRF5-JP/
- Demo Video: https://www.bilibili.com/video/BV1KWAVe1EKs
- Discussion Group: [1033086563](https://qm.qq.com/q/SckzhfDxuu) (Click the link to join the chat group)

![](docs/images/0.jpg)

The web interface supports multiple image dithering algorithms and allows you to doodle on images and add text. In addition to displaying images as a digital photo frame, it can switch to a Japanese calendar mode. The calendar calculates fixed-date, Happy Monday, substitute, citizens', equinox, and 2019-2021 exceptional holidays locally without changing the BLE protocol.

The controller reads the 13-byte configuration notified by the device. Byte 12 is the persisted `week_start` field: `0` means Sunday first, `1` means Monday first. The controller writes `[0x21, value]` and the device persists the value in FDS and notifies the updated configuration. `[0x22]` requests a fresh configuration notification. Existing command values, characteristic UUIDs, and configuration field offsets are preserved. Browser local storage is not used for this setting.

## Japanese calendar

- Calendar dates use the proleptic Gregorian leap-year rule.
- Japanese holiday rules cover the current statutory rules and the 2019-2021 one-off changes.
- Equinox dates use an integer approximation for 1980-2099. Dates after the published astronomical table range are forecasts and may need a data update if the law or official dates change.
- The calendar view does not allocate additional heap memory. The original lunar conversion remains available only to the clock view.
- Run the host-side rule tests with `make test` using a C compiler, or compile `tests/test_japanese_calendar.c` together with `GUI/JapaneseCalendar.c`.

## nRF52811 firmware build

The Keil target is `nRF52811_xxAA` with Nordic SDK 17.1.0 and S112 7.3.0. For a reproducible command-line build using Arm GNU Embedded 10.3-2021.10:

```powershell
python tools/build_nrf52811.py --toolchain C:\path\to\gcc-arm-none-eabi-10.3-2021.10
python tools/package_nrf52811_ota.py build/nrf52811/EPD-nRF52811-JP.hex
```

Outputs are under `build/nrf52811/`: application `.elf`, `.hex`, `.bin`, `-ota.zip`, and a SHA-256 manifest. The application is linked to `0x19000–0x25FFF` and the packaging script rejects addresses outside that range. The `-ota.zip` is the application-only DFU package; the `-full.hex` also includes SoftDevice, bootloader, and settings and is **not** the OTA upload file.

Firmware build and static package verification are distinct from a device OTA test. Before an OTA, verify the actual device's bootloader public key, SoftDevice ID, application layout, and a recovery image of the installed firmware. No device was flashed during this work.

The original upstream authors retain attribution. This fork follows the repository's GPL-3.0 license; see `LICENSE`.

## Supported Devices

[View Documentation](docs/devices.md).

## Development

[View Documentation](docs/develop.md).

## Acknowledgments

This project uses or references code from the following projects:

- [ZinggJM/GxEPD2](https://github.com/ZinggJM/GxEPD2)
- [waveshareteam/e-Paper](https://github.com/waveshareteam/e-Paper)
- [atc1441/ATC_TLSR_Paper](https://github.com/atc1441/ATC_TLSR_Paper)
