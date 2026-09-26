# EPD-nRF5-JP：日历日本版

这是基于 [tsl0922/EPD-nRF5](https://github.com/tsl0922/EPD-nRF5) 的**独立日本版 fork**。固件与仓库内的 [`html/`](html/) Web Bluetooth 控制页面一起维护，不依赖 epdiy.cn 实现新功能。

[日本語 README](README.md) · [设备资料](docs/devices.md) · [构建与 OTA 说明](docs/jp-build-ota.md)

## 功能

- 日本月历、和历、六曜、日本法定节假日与调休规则；保留原项目的时钟、图片传输和低功耗功能。
- 设备端保存“周日开始／周一开始”，Web 页面连接时读取当前值并可修改。
- 支持 nRF52811 命令行构建与应用固件 DFU ZIP 打包。

## 当前使用的墨水屏设备

本项目使用的设备为 **nRF52811** 主控、**4.2 英寸 400×300 像素黑／白／红三色墨水屏**，屏幕驱动芯片为 **UC8176**，固件中的屏幕型号 ID 为 **`03`**。`0x1d` 及之后版本的 nRF52811 初始配置按这套硬件设置；如果设备已经保存了屏幕型号配置，则优先使用设备内的值。

## 打开项目自己的 Web 控制页面

仓库内的 [`html/`](html/) 是项目自己的 Web Bluetooth 控制页面。请在自己的电脑上启动本地服务器（Windows CMD）：

```cmd
cd /d C:\path\to\EPD-nRF5-JP
python -m http.server 8000 --directory html
```

在 Chrome 或 Edge 打开 <http://localhost:8000/>，再通过蓝牙连接设备。对于 `0x1d` 及之后的 JP 固件，页面会读取设备配置；设备处于日历或时钟模式、且时间偏差超过 60 秒时，会自动校时并刷新。需要时也可以点击“日历模式”，手动同步时间并重绘。Web 控制页面**不负责上传 OTA 固件**。

## 构建固件

本项目的 nRF52811 目标使用 Nordic nRF5 SDK 17.1.0、S112 7.3.0 和 Arm GNU Embedded 10.3-2021.10。Windows CMD：

```cmd
cd /d C:\path\to\EPD-nRF5-JP
python tools\build_nrf52811.py --toolchain C:\path\to\gcc-arm-none-eabi-10.3-2021.10
python tools\package_nrf52811_ota.py build\nrf52811\EPD-nRF52811-JP.hex
```

`build/nrf52811/EPD-nRF52811-JP-ota.zip` 是**应用固件 OTA 包**；`-full.hex` 是包含 SoftDevice 和 Bootloader 的开发用镜像，不能作为手机 OTA ZIP。

## 使用 Android 手机刷机

1. 按上面的命令构建固件，把 `build/nrf52811/EPD-nRF52811-JP-ota.zip` 复制到 Android 手机，保持 ZIP 原样，不要解压。
2. 安装 Nordic Semiconductor 的 [nRF Device Firmware Update（Android）](https://play.google.com/store/apps/details?id=no.nordicsemi.android.dfu)，打开手机蓝牙。
3. 先断开网页等程序与墨水屏的蓝牙连接；在应用中选择目标设备，再选取复制到手机的 `-ota.zip`，开始 DFU。
4. 等应用显示更新完成、设备重新启动后，用本地 Web 控制页面连接，确认固件版本及屏幕型号 `03`。

手机 OTA 应选择**应用固件 `-ota.zip`**，不要选择 `-full.hex`。设备 Bootloader 的签名公钥及 SoftDevice 必须与 DFU 包匹配。

## 协议与测试

固件保留原有 BLE 命令和 GATT UUID。`0x21 0/1` 保存周起始，`0x22` 读取设备配置；Web 页面以设备配置为准，不以浏览器 localStorage 为准。更多信息见[日文 README](README.md)。

可用 `python tests/test_pages_assets.py` 检查 Web UI 静态资源；日历规则测试需要 C 编译器和 `make test`。

许可证为 [GPL-3.0](LICENSE)。感谢原项目及其引用的开源项目。
