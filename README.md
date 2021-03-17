<p><img src="https://github.com/oxinon/IOTA-price-ticker-V2-TTGO-T-Display/blob/master/picture/IOTA-Price-Ticker.png" alt="Cover" width="400"></p>

<p><img src="https://github.com/oxinon/IOTA-price-ticker-V3-M5Stack-Core2/blob/main/picture/iota-price-ticker-m5stack.png" alt="Cover" width="600"></p>

<br>

* * *

<b>Index of this project</b>

+ [Add M5Stack to Arduino IDE](#ESP32)
+ [Libraries for Arduino IDE](#libraries)
+ [How to use](#info)

* * *

<br>
<a name="ESP32"></a><h2>Add M5Stack in Arduino IDE</h2>

Before we can start compiling, the Arduino IDE must have the M5Stack Core2 board, based on an ESP32 in the board selection available.
The instruction on https://docs.m5stack.com/#/en/arduino/arduino_core2_development
In board selector you can select "M5Stack Core2" for M5Stack Core2.
<br>
<br>
For MAC users install the "CP210x USB to UART Bridge VCP Drivers" from Silicon Labs, the port to select is /dev/cu.SLAB_USBtoUART in Arduino IDE under tools-->Port
<br>
https://www.silabs.com/community/interface/knowledge-base.entry.html/2017/01/10/legacy_os_softwarea-bgvU
<br>

<a name="libraries"></a><h2>Libraries for Arduino IDE</h2>
Now we add libraries for Arduino IDE 1.8.7:<br>
The new api requires a developer key, so you must apply for a key to use https://pro.coinmarketcap.com/account
<br>

+ [CoinMarketCapApi.h](https://github.com/lewisxhe/CoinMarketCapApi2) 
+ [ArduinoJSON](https://github.com/bblanchon/ArduinoJson) version 6.x is testet 
   
The genaral instruction for the "M5Stack Core2" can be found here https://docs.m5stack.com/#/en/core/core2<br>
If you are have problem with the "wifi.h" delete the arduino/libraries/wiFi directory, you can use the esp32 WiFi.h
<br>
<br>
<a name="info"></a><h2>How to use</h2>

<p><img src="https://github.com/oxinon/IOTA-price-ticker-V3-M5Stack-Core2/blob/main/picture/Wen-settings.png" alt="Cover" width="600"></p>

* * *

If you like my work, you can give me a tip for a beer :)<br><br>
<b>IOTA Donate address:</b> 
TGSIBCRENEIPDSHKTAOSW9GXWCAFKPYSFYARVIEZDGQNNODEDP9VEVWLWEDGTAMFSBINPHBWFQQLTGK9CGJD9PGYFD <br>

<p><img src="https://github.com/oxinon/IOTA-price-ticker-V2-TTGO-T-Display/blob/master/picture/qrcode2.png" alt="Cover" width="200"></p>
