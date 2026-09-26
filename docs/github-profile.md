# GitHub repository profile

GitHub's **About** fields are repository settings rather than files tracked by Git.
Use these values for `jyh9521/EPD-nRF5-JP`:

- **Description:** `Japanese e-paper calendar firmware and Web Bluetooth controller for nRF5 devices. Holidays, Reiwa era, Rokuyō, and week-start settings.`
- **Website:** `https://jyh9521.github.io/EPD-nRF5-JP/` (after GitHub Pages is enabled and the URL works)
- **Topics:** `nrf52811`, `nrf5`, `e-paper`, `japanese-calendar`, `web-bluetooth`, `ble`, `dfu`

The description intentionally identifies this repository as the Japanese project,
not an extension of the Chinese web controller. The website must not point to
`tsl0922.github.io` or `epdiy.cn`.

The Pages workflow in `.github/workflows/static.yml` publishes `html/` from
`main`. Under GitHub **Settings → Pages**, select **GitHub Actions** as the
build/deployment source, then trigger the `Pages` workflow once if needed.
