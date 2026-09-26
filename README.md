# EPD-nRF5-JP — Japanese Edition

**日本向け電子ペーパーカレンダーの独立 fork。** [tsl0922/EPD-nRF5](https://github.com/tsl0922/EPD-nRF5) を基に、日本の暦表示と、このリポジトリで管理する Web Bluetooth コントローラーを追加しています。中国語版への機能追加ではなく、日本向けのファームウェアと Web UI を一緒に開発するプロジェクトです。

[简体中文说明](README.zh-CN.md) · [対応デバイス](docs/devices.md) · [開発資料](docs/develop.md) · [ビルドと OTA の詳細](docs/jp-build-ota.md)

## 主な機能

- 月間カレンダー、和暦、六曜、日本の祝日・振替休日・国民の休日。
- 週の開始曜日を **日曜日（初期値）／月曜日** から選択。設定はブラウザーではなくデバイス側に保存します。
- 既存の時計・画像表示、画像転送、低消費電力動作を継承。
- `html/` の Web Bluetooth コントローラーで接続、時刻同期、表示モード切り替え、画像転送、週初めの読出し・変更が可能。
- nRF52811 向けのコマンドラインビルドと、アプリケーション専用 DFU ZIP の生成。

日本の祝日計算には 2019–2021 年の特例も含みます。春分・秋分は 1980–2099 年向けの近似式を使うため、将来の正式な日付が公表された場合は確認・更新が必要です。

### 動作確認の範囲

2026 年 9 月、nRF52811 搭載の 4.2 インチ・400×300・UC8176 3 色機で、`0x1c (JP)` の BLE 読出し、DFU 後の再接続、日本語カレンダーの表示を確認しました。すべての互換基板・OTA 構成での動作確認を意味しません。週初め設定の**再起動後の保持**は、この実機ではまだ確認中です。

## Web コントローラー

正式な Web UI はこのリポジトリの [`html/`](html/) です。外部サイトへの依存はありません。GitHub Pages 用ワークフローは `main` の `html/` を配信し、デプロイは成功しています。ただし現時点ではアカウント側のカスタムドメイン `blog.blfy.top` に転送され、そのドメインを解決できないため、公開 URL は利用できません。利用可能になるまでローカル版を使ってください。

- 予定している公開先：<https://jyh9521.github.io/EPD-nRF5-JP/>（現在は上記ドメインへ転送）
- ローカルで開く場合（Windows CMD）：

  ```cmd
  cd /d C:\path\to\EPD-nRF5-JP
  python -m http.server 8000 --directory html
  ```

  Chrome または Edge で <http://localhost:8000/> を開いてください。BLE 接続後、カレンダーモードを選ぶと現在時刻を送信して画面を更新します。ファームウェアを書き換えても電子ペーパーは前の画像を保持するため、表示だけで書換え成否を判断せず、Web UI のファームウェアバージョンを確認してください。

Web UI は **DFU クライアントではありません**。OTA 更新には nRF5 SDK Secure DFU に対応したアプリを使用します。

## nRF52811 ファームウェアのビルド

対象は Nordic nRF5 SDK 17.1.0 / S112 7.3.0 / `nRF52811_xxAA` です。Arm GNU Embedded 10.3-2021.10 を用いる Windows CMD の例：

```cmd
cd /d C:\path\to\EPD-nRF5-JP
python tools\build_nrf52811.py --toolchain C:\path\to\gcc-arm-none-eabi-10.3-2021.10
python tools\package_nrf52811_ota.py build\nrf52811\EPD-nRF52811-JP.hex
```

生成物は `build/nrf52811/` に保存されます。`EPD-nRF52811-JP-ota.zip` は**アプリケーション専用**の BLE DFU パッケージです。`EPD-nRF52811-JP-full.hex` は SoftDevice・Bootloader・設定を含む開発用イメージであり、スマートフォンからの OTA に使うファイルではありません。パッケージ作成時にはアプリケーションのアドレス範囲 `0x19000–0x25FFF` を検査します。

DFU の実行前に、実機の Bootloader 公開鍵・SoftDevice・復旧手段を確認してください。`v1.10-nrf52811` の表示だけでは署名鍵の一致は分かりません。初回実機試験では DFU は完了しましたが、既存の画面モデル設定が `03`（UC8176 3 色）から `02`（SSD1619 3 色）に変わり、画面が更新されませんでした。同じ **UC8176 3 色機** では、開発モードで `0103` を送信し、続けて `22` で設定を読み直すと、モデル `03` を確認して日本語画面を表示できました。**他の機種に `0103` をそのまま送信しないでください。** ピン配置や画面モデルは実機に合わせて確認してください。

リセット命令 `91` は BLE 切断が先に発生して Web UI が「書き込み失敗」を表示する場合があります。再接続後にバージョンと設定を確認してください。また、リセット後の時計が古い日時に戻った場合は Web UI から時刻を再同期してください。

## BLE 設定と互換性

| コマンド | 内容 |
| --- | --- |
| `0x20` | 時刻と表示モードを設定 |
| `0x21 0x00/0x01` | 週の開始曜日を日曜／月曜に変更 |
| `0x22` | 現在の設定を通知で読み出し |
| `0x01 <model>` | 画面モデルを初期化・保存 |

既存のコマンド値と GATT UUID は維持しています。週初めはデバイスの 13 バイト設定の最後の 1 バイトで、FDS に保存します。`html/` は接続時にデバイスから設定を読み、ブラウザーの localStorage を正本にはしません。

ホスト側のカレンダーテストは C コンパイラーと `make test` で実行できます。Pages の静的ファイル検査は `python tests/test_pages_assets.py` で実行できます。

## ライセンスと謝辞

本 fork は [GPL-3.0](LICENSE) に従います。原作者の功績を保持し、[ZinggJM/GxEPD2](https://github.com/ZinggJM/GxEPD2)、[Waveshare e-Paper](https://github.com/waveshareteam/e-Paper)、[ATC_TLSR_Paper](https://github.com/atc1441/ATC_TLSR_Paper) を参照しています。
