#include <M5Core2.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiClientSecure.h>
#include "WebServer.h"
#include <Preferences.h>
#include "Wire.h"
//#include "esp_adc_cal.h"

#include "info.h"
#include "alert.h"
#include "iota.h"
#include "iota2.h"
#include "rocket.h"
#include "moon.h"

#define TFT_COLOR1    0x21CB  //green_blue
#define GRAY    0x8410

#include "CoinMarketCapApi.h"


WiFiClientSecure client;


// CoinMarketCap's limit is "no more than 10 per minute"
// Make sure to factor in if you are requesting more than one coin.
unsigned long api_mtbs = 60000; //mean time between api requests
unsigned long api_due_time = 0;


const IPAddress apIP(192, 168, 4, 1);
const char* apSSID = "Price-Ticker_SETUP";
boolean settingMode;
String ssidList;
String wifi_ssid;
String wifi_password;
String cmc_api_key;

float batVoltage = M5.Axp.GetBatVoltage();

int count = 0;

int count2 = 0;


// DNSServer dnsServer;
WebServer webServer(80);

// wifi config store
Preferences preferences;


void setup() {
  m5.begin();
  Wire.begin();
  M5.Lcd.setSwapBytes(true);
  preferences.begin("wifi-config");

    M5.Lcd.fillScreen(TFT_BLACK);
    M5.Lcd.setSwapBytes(true);
    M5.Lcd.pushImage(40, 50, iotaWidth, iotaHeight, iota);
    delay(2000);
    M5.Lcd.fillScreen(TFT_BLACK); 

   delay(10);
  if (restoreConfig()) {
    if (checkConnection()) {
      M5.Lcd.pushImage(280, 2, infoWidth, infoHeight, info);
      delay(2000);
      M5.Lcd.fillScreen(TFT_BLACK); 
      settingMode = false;
      startWebServer();
      return;
    }
  }
  settingMode = true;
  setupMode();
}



void loop() {

  M5.update();
  if (settingMode) {
  }
  webServer.handleClient();
  


  if(M5.BtnA.isPressed())  M5.Axp.SetLcdVoltage(2600);
  if(M5.BtnB.isPressed())  M5.Axp.SetLcdVoltage(2800);
  if(M5.BtnC.isPressed())  M5.Axp.SetLcdVoltage(3200);

  if (count2 >=2600) {
    printTickerDataIOTA("MIOTA");
  }
  
  Serial.print("Counter: ");
  Serial.println(count2);

  count2++;
  
  delay(150);

}


boolean restoreConfig() {
  wifi_ssid = preferences.getString("WIFI_SSID");
  wifi_password = preferences.getString("WIFI_PASSWD");
  cmc_api_key = preferences.getString("CMC_API_KEY");
  M5.Lcd.pushImage(280, 2, infoWidth, infoHeight, info);
  Serial.print("WIFI-SSID: ");
  M5.Lcd.drawString("WIFI-SSID: ", 0, 0, 2);
  Serial.println(wifi_ssid);
  M5.Lcd.drawString(wifi_ssid, 80, 0, 2);
  Serial.print("WIFI-PASSWD: ");
  M5.Lcd.drawString("WIFI-PASSWD: ", 0, 20, 2);
  Serial.println(wifi_password);
  M5.Lcd.drawString(wifi_password, 100, 20, 2);
  Serial.print("CMC_API_KEY: ");
  M5.Lcd.drawString("CMC_API_KEY: ", 0, 40, 2);
  Serial.println(cmc_api_key);
  M5.Lcd.drawString(cmc_api_key, 100, 40, 2);

  delay(2000);
  
  WiFi.begin(wifi_ssid.c_str(), wifi_password.c_str());

  if(wifi_ssid.length() > 0) {
    return true;
} else {
    return false;
  }
}

boolean checkConnection() {
  int count = 0;
  M5.Lcd.fillScreen(TFT_BLACK);
  M5.Lcd.pushImage(280, 2, infoWidth, infoHeight, info);
  Serial.print("Waiting for Wi-Fi connection");
  M5.Lcd.drawString("Waiting for Wi-Fi connection", 0, 0, 2);
  M5.Lcd.println();
  M5.Lcd.println();
  M5.Lcd.println();
  M5.Lcd.println();


  
  while ( count < 30 ) {
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println();
      M5.Lcd.println();
      Serial.println("Connected!");
      M5.Lcd.setTextColor(GREEN);
      M5.Lcd.drawString("Connected!", 0, 20, 2);
      return (true);
    }
    delay(500);
    Serial.print(".");
    M5.Lcd.print(".");
    count++;
  }
  Serial.println("Timed out.");
  M5.Lcd.setTextColor(RED);
  M5.Lcd.drawString("Timed out.", 0, 20, 2);

  M5.Lcd.drawString("Reset Wifi config", 0, 30, 2);
  M5.Lcd.setTextColor(GREEN);
  M5.Lcd.drawString("Starting AP Setup mode after restart", 0, 40, 2);
      // reset the wifi config
      preferences.remove("WIFI_SSID");
      preferences.remove("WIFI_PASSWD");
     // preferences.remove("CMC_API_KEY");
      delay(1000);
      ESP.restart();

  return false;
}

void startWebServer() {
  if (settingMode) {
    M5.Lcd.fillScreen(TFT_BLACK);
    M5.Lcd.setTextColor(WHITE);
    Serial.print("Starting Web Server at ");
    //M5.Lcd.print("Starting Web Server at ");
    M5.Lcd.drawString("Starting Web Server at 192.168.4.1", 0, 0, 2);
    Serial.println(WiFi.softAPIP());
    //M5.Lcd.drawString(WiFi.softAPIP(), 100, 0, 2);
    //M5.Lcd.print(WiFi.softAPIP());
    webServer.on("/settings", []() {
      String s = "<h1>Wi-Fi and CMC API Settings</h1><p>Please enter your password by selecting the SSID<br> and your Coinmarketcap API Key.</p>";
      s += "<form method=\"get\" action=\"setap\"><label>SSID: </label><select name=\"ssid\">";
      s += ssidList;
      s += "</select><br>Password: <input name=\"pass\" length=64 type=\"password\">";
      s += "<br>CMC API Key: <input name=\"apikey\" length=64 type=\"text\"><input type=\"submit\"></form>";
      webServer.send(200, "text/html", makePage("Wi-Fi Settings", s));
    });
    webServer.on("/setap", []() {
      M5.Lcd.fillScreen(TFT_BLACK);
      String ssid = urlDecode(webServer.arg("ssid"));
      Serial.print("SSID: ");
      M5.Lcd.print("SSID: ");
      Serial.println(ssid);
      M5.Lcd.println(ssid);
      String pass = urlDecode(webServer.arg("pass"));
      Serial.print("Password: ");
      M5.Lcd.print("Password: ");
      Serial.println(pass);
      M5.Lcd.println(pass);
      String apikey = urlDecode(webServer.arg("apikey"));
      Serial.print("CMC API KEY: ");
      M5.Lcd.print("CMC API KEY: ");
      Serial.println(apikey);
      M5.Lcd.println(apikey);
      Serial.println("Writing API to EEPROM...");
      M5.Lcd.println("Writing API to EEPROM...");

      // Store wifi config
      Serial.println("Writing Password to nvr...");
      M5.Lcd.println("Writing Password to nvr...");
      preferences.putString("WIFI_SSID", ssid);
      preferences.putString("WIFI_PASSWD", pass);
      preferences.putString("CMC_API_KEY", apikey);

      Serial.println("Write nvr done!");
      M5.Lcd.println("Write nvr done!");
      String s = "<h1>Setup complete.</h1><p>device will be connected to \"";
      s += ssid;
      s += "\" after the restart.";
      webServer.send(200, "text/html", makePage("Wi-Fi Settings", s));
      delay(3000);
      ESP.restart();
    });
    webServer.onNotFound([]() {
      String s = "<h1>AP mode</h1><p><a href=\"/settings\">Wi-Fi Settings</a></p>";
      webServer.send(200, "text/html", makePage("AP mode", s));
    });
  }
  else {
    M5.Lcd.fillScreen(TFT_BLACK);
    Serial.print("Starting Web Server at ");
    //M5.Lcd.print("Starting Web Server at ");
    //M5.Lcd.drawString("Loading Ticker data...", 0, 0, 2);
   
    Serial.println(WiFi.localIP());
    //M5.Lcd.println(WiFi.localIP());
    //M5.Lcd.drawString(WiFi.localIP(), 100, 0, 2);


        
    webServer.on("/", []() {
      String s = "<h1>STA mode</h1><p><a href=\"/reset\">Reset Wi-Fi Settings</a></p>";
      webServer.send(200, "text/html", makePage("STA mode", s));
    });
    webServer.on("/reset", []() {
      // reset the wifi config
      preferences.remove("WIFI_SSID");
      preferences.remove("WIFI_PASSWD");
      preferences.remove("CMC_API_KEY");
      String s = "<h1>Wi-Fi settings was reset.</h1><p>Please reset device.</p>";
      webServer.send(200, "text/html", makePage("Reset Wi-Fi Settings", s));
      delay(3000);
      ESP.restart();
    });
    
// Header 
    M5.Lcd.fillRect(0, 0, 320, 27, TFT_COLOR1);
    printTickerDataIOTA("MIOTA");
  }
  webServer.begin();
}

void setupMode() {
  WiFi.mode(WIFI_MODE_STA);
  WiFi.disconnect();
  delay(100);
  int n = WiFi.scanNetworks();
  delay(100);
  Serial.println("");
  M5.Lcd.println("");
  for (int i = 0; i < n; ++i) {
    ssidList += "<option value=\"";
    ssidList += WiFi.SSID(i);
    ssidList += "\">";
    ssidList += WiFi.SSID(i);
    ssidList += "</option>";
  }
  delay(100);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(apSSID);
  WiFi.mode(WIFI_MODE_AP);
  // WiFi.softAPConfig(IPAddress local_ip, IPAddress gateway, IPAddress subnet);
  // WiFi.softAP(const char* ssid, const char* passphrase = NULL, int channel = 1, int ssid_hidden = 0);
  // dnsServer.start(53, "*", apIP);
  startWebServer();
  Serial.print("Starting Access Point at \"");
  //M5.Lcd.print("Starting Access Point at \"");
   M5.Lcd.drawString("Starting Access Point at: Price-Ticker_SETUP", 0, 20, 2);
  Serial.print(apSSID);
  //M5.Lcd.print(apSSID);
  Serial.println("\"");
  //M5.Lcd.println("\"");
}

String makePage(String title, String contents) {
  String s = "<!DOCTYPE html><html><head>";
  s += "<meta name=\"viewport\" content=\"width=device-width,user-scalable=0\">";
  s += "<title>";
  s += title;
  s += "</title></head><body>";
  s += contents;
  s += "</body></html>";
  return s;
}

String urlDecode(String input) {
  String s = input;
  s.replace("%20", " ");
  s.replace("+", " ");
  s.replace("%21", "!");
  s.replace("%22", "\"");
  s.replace("%23", "#");
  s.replace("%24", "$");
  s.replace("%25", "%");
  s.replace("%26", "&");
  s.replace("%27", "\'");
  s.replace("%28", "(");
  s.replace("%29", ")");
  s.replace("%30", "*");
  s.replace("%31", "+");
  s.replace("%2C", ",");
  s.replace("%2E", ".");
  s.replace("%2F", "/");
  s.replace("%2C", ",");
  s.replace("%3A", ":");
  s.replace("%3A", ";");
  s.replace("%3C", "<");
  s.replace("%3D", "=");
  s.replace("%3E", ">");
  s.replace("%3F", "?");
  s.replace("%40", "@");
  s.replace("%5B", "[");
  s.replace("%5C", "\\");
  s.replace("%5D", "]");
  s.replace("%5E", "^");
  s.replace("%5F", "-");
  s.replace("%60", "`");
  return s;
}

void printTickerDataIOTA(String ticker)
{

        
    Serial.println("---------------------------------");
    Serial.println("Getting ticker data for " + ticker);
    M5.Lcd.setTextColor(WHITE);

    //For the new API, you can use the currency ID or abbreviated name, such as
    //Bitcoin, you can view the letter after Circulating Supply at https://coinmarketcap.com/, it is BTC
    CoinMarketCapApi api(client, cmc_api_key);
    CMCTickerResponse response = api.GetTickerInfo(ticker, "USD");
    
    if (response.error == "") {
        Serial.print("ID: ");
        Serial.println(response.id);
        Serial.print("Name: ");
        Serial.println(response.name);
        Serial.print("Symbol: ");
        Serial.println(response.symbol);

        Serial.print("Rank: ");
        Serial.println(response.cmc_rank);

        Serial.print("Price: ");
        Serial.println(response.price);

        Serial.print("24h Volume: ");
        Serial.println(response.volume_24h);
        Serial.print("Market Cap: ");
        Serial.println(response.market_cap);

        Serial.print("Circulating Supply: ");
        Serial.println(response.circulating_supply);
        Serial.print("Total Supply: ");
        Serial.println(response.total_supply);

        Serial.print("Percent Change 1h: ");
        Serial.println(response.percent_change_1h);
        Serial.print("Percent Change 24h: ");
        Serial.println(response.percent_change_24h);
        Serial.print("Percent Change 7d: ");
        Serial.println(response.percent_change_7d);
        Serial.print("Last Updated: ");
        Serial.println(response.last_updated);


// Wifi signal bars
        Serial.print("WiFi Signal strength: ");
        Serial.print(WiFi.RSSI());
        M5.Lcd.fillRect(280, 0, 40, 26, TFT_COLOR1); //wifi RSSI

        float RSSI = 0.0;
        int bars;
        RSSI = WiFi.RSSI();

        if (RSSI >= -55) {
            bars = 5;
            Serial.println(" 5 bars");
        } else if (RSSI < -55 & RSSI >= -65) {
            bars = 4;
            Serial.println(" 4 bars");
        } else if (RSSI < -65 & RSSI >= -70) {
            bars = 3;
            Serial.println(" 3 bars");
        } else if (RSSI < -70 & RSSI >= -78) {
            bars = 2;
            Serial.println(" 2 bars");
        } else if (RSSI < -78 & RSSI >= -82) {
            bars = 1;
            Serial.println(" 1 bars");
        } else {
            bars = 0;
            Serial.println(" 0 bars");
        }

// print signal bars
        for (int b = 0; b <= bars; b++) {
            M5.Lcd.fillRect(281 + (b * 6), 23 - (b * 4), 5, b * 4, GRAY);
        }
        
        M5.Lcd.pushImage(10, 30, iota2Width, iota2Height, iota2);


// Battery stat





// Line
        M5.Lcd.drawLine(10, 133, 310, 133, GRAY);


        M5.Lcd.setTextColor(GRAY);
        M5.Lcd.drawString("MIOTA USD Price", 60, 4, 4);

        M5.Lcd.setTextColor(WHITE);
        
        if (response.percent_change_1h < 0) {
            M5.Lcd.setTextColor(RED);          
        }
        
        if (response.percent_change_1h > 0) {
            M5.Lcd.setTextColor(GREEN);              
        }
        
// Price
        M5.Lcd.fillRect(115, 38, 205, 50, BLACK); 
        M5.Lcd.drawString(String(response.price).c_str(), 115, 50, 6);
        M5.Lcd.setTextColor(CYAN);

        if (response.price > 2) {
           M5.Lcd.pushImage(265, 38, rocketWidth, rocketHeight, rocket);              
        }

        if (response.price > 5) {
          M5.Lcd.pushImage(265, 38, moonWidth, moonHeight, moon);              
        }


// Rank
        M5.Lcd.drawString("Rank:", 115, 100, 4);
        M5.Lcd.fillRect(185, 100, 120, 20, BLACK); 
        M5.Lcd.drawString(String(response.cmc_rank).c_str(), 190, 100, 4);




// hours change
        M5.Lcd.fillRect(240, 140, 80, 90, BLACK);
        
        M5.Lcd.setTextColor(YELLOW);

        if (response.percent_change_1h < 0) {
            M5.Lcd.setTextColor(RED);
        }
        if (response.percent_change_1h > 0) {
            M5.Lcd.setTextColor(GREEN);
        }
        M5.Lcd.drawString("% Price change 1h:", 11, 145, 4);
        M5.Lcd.drawString(String(response.percent_change_1h).c_str(), 245, 145, 4);
        delay(1);


// 24 hours change
        M5.Lcd.setTextColor(YELLOW);

        if (response.percent_change_24h < 0) {
            M5.Lcd.setTextColor(RED);
        }
        if (response.percent_change_24h > 0) {
            M5.Lcd.setTextColor(GREEN);
        }
        M5.Lcd.drawString("% Price change 24h:", 11, 175, 4);
        M5.Lcd.drawString(String(response.percent_change_24h).c_str(), 245, 175, 4);
        delay(1);


// 7d hours change
        M5.Lcd.setTextColor(YELLOW);

        if (response.percent_change_7d < 0) {
            M5.Lcd.setTextColor(RED);
        }
        if (response.percent_change_7d > 0) {
            M5.Lcd.setTextColor(GREEN);
        }
        M5.Lcd.drawString("% Price change 7d:", 11, 205, 4);
        M5.Lcd.drawString(String(response.percent_change_7d).c_str(), 245, 205, 4);
      
        count2 = 2;
       

    }

    else {
        Serial.print("Error getting data: ");
        Serial.println(response.error);
        M5.Lcd.fillRect(115, 53, 205, 38, BLACK); //wifi RSSI and alert
        M5.Lcd.pushImage(283, 53, alertWidth, alertHeight, alert);
        delay(1000);
    }

    
    Serial.println("---------------------------------");
}
