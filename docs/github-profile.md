# GitHub repository profile

GitHub's **About** fields are repository settings rather than files tracked by Git.
Use these values for `jyh9521/EPD-nRF5-JP`:

- **Description:** `Japanese e-paper calendar firmware and Web Bluetooth controller for nRF5 devices. Holidays, Reiwa era, Rokuyō, and week-start settings.`
- **Website:** leave blank until `https://jyh9521.github.io/EPD-nRF5-JP/` works without redirecting to an unresolvable custom domain.
- **Topics:** `nrf52811`, `nrf5`, `e-paper`, `japanese-calendar`, `web-bluetooth`, `ble`, `dfu`

The description intentionally identifies this repository as the Japanese project,
not an extension of the Chinese web controller. The website must not point to
`tsl0922.github.io` or `epdiy.cn`.

The Pages workflow in `.github/workflows/static.yml` publishes `html/` from
`main`. GitHub Actions publishing was enabled and its deployment succeeded on
2026-09-26, but GitHub redirected the project URL to `blog.blfy.top`, which
did not resolve. This appears to be the account's custom-domain configuration,
not a defect in this repository's `html/` assets. Do not advertise the URL as
working until the account-level domain/DNS setting is corrected and retested.
