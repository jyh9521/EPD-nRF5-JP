# EPD-nRF5-JP：日历日本版

这是基于 [tsl0922/EPD-nRF5](https://github.com/tsl0922/EPD-nRF5) 的**独立日本版 fork**。固件与仓库内的 [`html/`](html/) Web Bluetooth 控制页面一起维护，不依赖 epdiy.cn 实现新功能。

[日本語 README](README.md) · [设备资料](docs/devices.md) · [构建与 OTA 说明](docs/jp-build-ota.md)

## 功能

- 日本月历、和历、六曜、日本法定节假日与调休规则；保留原项目的时钟、图片传输和低功耗功能。
- 设备端保存“周日开始／周一开始”，Web 页面连接时读取当前值并可修改。
- 支持 nRF52811 命令行构建与应用固件 DFU ZIP 打包。

**实机验证范围：**2026 年 9 月，在一台 nRF52811、4.2 英寸 400×300、UC8176 三色屏设备上，确认 DFU 后运行 `0x1c (JP)`、BLE 可重连，并成功显示日文月历。其他设备型号及“周起始设置重启后保持”尚未通过该实机验证。

## 打开项目自己的 Web 控制页面

仓库的 `html/` 是正式控制页面。GitHub Pages 工作流已配置为发布 `main` 分支的 `html/`；还需在 GitHub 仓库设置中启用 Pages。

- 预定地址：<https://jyh9521.github.io/EPD-nRF5-JP/>
- 本地打开（CMD）：

  ```cmd
  cd /d C:\path\to\EPD-nRF5-JP
  python -m http.server 8000 --directory html
  ```

在 Chrome 或 Edge 打开 <http://localhost:8000/>。连接设备后选择“日历模式”会同步时间并请求刷新。墨水屏会保留旧画面，因此判断 OTA 是否生效应先读网页日志中的固件版本。Web 控制页面**没有 OTA 上传功能**。

## 构建固件

本项目的 nRF52811 目标使用 Nordic nRF5 SDK 17.1.0、S112 7.3.0 和 Arm GNU Embedded 10.3-2021.10。Windows CMD：

```cmd
cd /d C:\path\to\EPD-nRF5-JP
python tools\build_nrf52811.py --toolchain C:\path\to\gcc-arm-none-eabi-10.3-2021.10
python tools\package_nrf52811_ota.py build\nrf52811\EPD-nRF52811-JP.hex
```

`build/nrf52811/EPD-nRF52811-JP-ota.zip` 是**应用固件 OTA 包**；`-full.hex` 是包含 SoftDevice 和 Bootloader 的开发用镜像，不能作为手机 OTA ZIP。刷机前应确认设备 Bootloader 的签名公钥与固件包匹配，并保留恢复途径；只看到 `v1.10-nrf52811` 版本名不足以证明兼容。

### 刷机后屏幕不刷新

先确认网页日志显示 `0x1c (JP)`，再比较刷机前后的设备配置。实机上曾出现屏幕型号从 `03`（UC8176 三色）变成 `02`（SSD1619 三色），导致蓝牙正常但屏幕不刷新。在**同一型号的 UC8176 三色屏**上，开发模式发送 `0103`，再发送 `22`，确认配置中型号恢复为 `03`，随后点击“日历模式”同步时间并刷新。不同屏幕不能照抄 `0103`；应使用其原来的型号和引脚配置。

`91` 是软件复位命令；蓝牙可能在写入确认之前断开，使网页显示“写入失败”。重新连接并读取版本、配置判断是否已复位。`99` 会清除配置，不要把它当作重启命令。

## 协议与测试

固件保留原有 BLE 命令和 GATT UUID。`0x21 0/1` 保存周起始，`0x22` 读取设备配置；Web 页面以设备配置为准，不以浏览器 localStorage 为准。更多信息见[日文 README](README.md)。

可用 `python tests/test_pages_assets.py` 检查 Pages 静态资源；日历规则测试需要 C 编译器和 `make test`。

许可证为 [GPL-3.0](LICENSE)。感谢原项目及其引用的开源项目。
