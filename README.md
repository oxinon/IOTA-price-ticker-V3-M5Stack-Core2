# IOTA Price Ticker V3 – M5Stack Core2 (ESP32)

An IOTA (MIOTA) price ticker built on the M5Stack Core2 (ESP32), with a built-in web setup flow — no need to hard-code Wi-Fi or your API key in the sketch. Uses the CoinMarketCap API V2.

![Ticker](https://github.com/oxinon/IOTA-price-ticker-V2-TTGO-T-Display/raw/master/picture/IOTA-Price-Ticker.png)
![Ticker V3](https://github.com/oxinon/IOTA-price-ticker-V3-M5Stack-Core2/raw/main/picture/pt3v2.png)

[Watch V3 in action on YouTube](https://www.youtube.com/watch?v=CBe5YHJxLos)

## Hardware

- [M5Stack Core2](https://docs.m5stack.com/#/en/core/core2) (ESP32)

## Prerequisites

- Arduino IDE 1.8.13
- A free CoinMarketCap API key: <https://pro.coinmarketcap.com/account>

## Setup

### 1. Add the M5Stack Core2 board to the Arduino IDE

Follow M5Stack's official guide: <https://docs.m5stack.com/#/en/arduino/arduino_core2_development>

In the board selector, choose **M5Stack Core2**.

**macOS users:** install the [CP210x USB to UART Bridge VCP driver](https://www.silabs.com/community/interface/knowledge-base.entry.html/2017/01/10/legacy_os_softwarea-bgvU) from Silicon Labs. The correct port is `/dev/cu.SLAB_USBtoUART` (Tools → Port).

### 2. Install the required libraries

- [CoinMarketCapApi2](https://github.com/lewisxhe/CoinMarketCapApi2)
- [ArduinoJson](https://github.com/bblanchon/ArduinoJson) — v6.x tested

General M5Stack Core2 reference: <https://docs.m5stack.com/#/en/core/core2>

**Troubleshooting:** If you run into `WiFi.h` conflicts, delete the `arduino/libraries/WiFi` directory and use the ESP32 core's built-in `WiFi.h` instead.

### 3. Flash the sketch, then configure over Wi-Fi

Select the correct board/port and upload — no API key or Wi-Fi credentials need to be set in the code beforehand.

## How to use

On first boot, with no Wi-Fi or CoinMarketCap key configured, the device opens a Wi-Fi access point named **`Price-Ticker_SETUP`**. Connect to it and open `192.168.4.1` in a browser to enter your Wi-Fi credentials and CoinMarketCap API key. This is saved on the device, so you won't need to enter it again.

![Web setup](https://github.com/oxinon/IOTA-price-ticker-V3-M5Stack-Core2/raw/main/picture/Wen-settings.png)

After the automatic restart, the ticker connects to your Wi-Fi, confirms the saved settings on screen, and starts pulling data from CoinMarketCap — refreshing every 260 seconds.

### Resetting the configuration

- **No Wi-Fi connection established:** the Wi-Fi settings reset automatically after 30 seconds (your CoinMarketCap key is kept).
- **Connected to Wi-Fi:** open the device's IP address in a browser (shown on screen, e.g. `192.168.1.227`) to reset everything, including the CoinMarketCap key. The device then restarts in AP setup mode.

![Reset settings](https://github.com/oxinon/IOTA-price-ticker-V3-M5Stack-Core2/raw/main/picture/reset-settings.png)

### Display brightness

The three lower soft-touch buttons set the screen brightness: **A** = low, **B** = medium, **C** = high.

### Flashing via esptool

A pre-built `.bin` for flashing with esptool is available (see `IOTA-price-ticker-V3-M5Stack-Core2_for-esptool/`).

## Changelog

**2021-12-30**
- Fixed USD/EUR selection issue in the web config
- Added light theme option in the web config
- Added a pre-built `.bin` for esptool flashing

**2021-06-23**
- Added battery indicator
- Added USD/EUR selection in the web config
- Added dark theme

## Support

If you find this useful, IOTA donations are welcome:

`iota1qz4uqyauz486zvrm2jjn4gurq5dzcmhezr0d6smykgaferx6madmzj2rzxr`

## License

[GPL-3.0](LICENSE)
