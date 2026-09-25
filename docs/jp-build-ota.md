# EPD-nRF5-JP: nRF52811 build and DFU notes

## Build

The firmware target is `nRF52811_xxAA`, Nordic nRF5 SDK 17.1.0, and S112 7.3.0.
The original Keil project uses Arm Compiler 5. A host-side build was also verified
using Arm GNU Embedded 10.3-2021.10:

```powershell
python tools/build_nrf52811.py --toolchain C:\path\to\gcc-arm-none-eabi-10.3-2021.10
python tools/package_nrf52811_ota.py build/nrf52811/EPD-nRF52811-JP.hex
```

The GNU linker file limits the application to `0x19000` through `0x25FFF`.
`tools/package_nrf52811_ota.py` checks the complete Intel HEX range before
creating packages. The Keil project's larger legacy region must not be treated
as the DFU-safe application limit.

The first build on 2026-09-25 had these GNU `-Os -flto` results:

| State | FLASH (`text+data`) | RAM (`data+bss`) | BIN | HEX end, exclusive |
|---|---:|---:|---:|---:|
| Original HEAD | 59,628 B | 1,920 B | 59,628 B | `0x278EC` |
| Japanese Edition | 52,700 B | 1,776 B | 52,700 B | `0x25DDC` |
| Difference | −6,928 B | −144 B | −6,928 B | — |

The original HEAD measurement used the original Keil-sized linker span
(`0x19000` + `0x17000`) because it exceeded the strict DFU-safe boundary by
6,380 B by HEX extent. The Japanese Edition has 548 B of remaining address space below
`0x26000`; any later feature addition must repeat the range check.

## Artifacts

- `EPD-nRF52811-JP.hex`: application-only Intel HEX at `0x19000`.
- `EPD-nRF52811-JP.bin`: application-only raw bytes.
- `EPD-nRF52811-JP-ota.zip`: signed application DFU archive, version `0x1b`,
  hardware version `52`, S112 requirement `0x126`.
- `EPD-nRF52811-JP-manifest.json`: size and SHA-256 checksums.
- `EPD-nRF52811-JP-full.hex`: merged development image containing the
  SoftDevice, bootloader, application, settings, and UICR values from this
  repository. It is not the application OTA file.

## OTA and recovery

The firmware uses the Nordic buttonless DFU service in `main.c`. A matching
DFU client requests entry to the bootloader and transfers the `-ota.zip` file.
The project Web Controller currently handles display control and image transfer;
it does not implement a DFU client. The DFU transport and bootloader were not
changed for the Japanese calendar work.

Before any real-device transfer, confirm the installed device's nRF52811 part,
S112 ID, application start, bootloader public key, flash pages, and currently
installed version. The repository key matching the repository bootloader does
not establish what is installed in a particular device. Preserve a known-good
signed v1.10 package or an SWD backup first. If an OTA fails but the bootloader
still advertises, send the verified known-good package through Nordic DFU.
If only SWD recovery remains, restore the device-specific SoftDevice,
bootloader, settings and application backup with a debugger, then verify BLE,
display refresh and the persisted configuration. No physical upgrade or
recovery was performed while producing these artifacts.
