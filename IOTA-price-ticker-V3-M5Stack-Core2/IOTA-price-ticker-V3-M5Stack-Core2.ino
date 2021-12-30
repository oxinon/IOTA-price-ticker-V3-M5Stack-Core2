#include <M5Core2.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiClientSecure.h>
#include "WebServer.h"
#include <Preferences.h>
#include "Wire.h"
#include "AXP192.h"


#include "info.h"
#include "alert.h"
#include "iota.h"
#include "iota1.h"
#include "iota2.h"
#include "iota3.h"
#include "rocket.h"
#include "moon.h"
#include "schnitzel.h"
#include "manheim.h"
#include "munic.h"
#include "tirol.h"
#include "wifilogo.h"
#include "wsetup.h"

#define LTFT_COLOR1    0xFFFF //TopBar
#define LTFT_TXT2      0x0000 //TopBarTXT
#define LTFT_COLOR2    0xFFFF //TopBarLine
#define LTFT_COLOR3    0xFFFF //Background
#define LTFT_TXT       0x0000 //Text

#define DTFT_COLOR1    0x0000 //TopBar
#define DTFT_TXT2      0xC618 //TopBarTXT
#define DTFT_COLOR2    0x0000 //TopBarLine
#define DTFT_COLOR3    0x0000 //Background
#define DTFT_TXT       0xC618 //TextPrice change

#define TFT_TXT       0x7BEF //TextPrice change

#define LGRAY   0xC618
#define GRAY    0x8410
#define DGRAY   0x7BEF
#define DGREEN  0x0606
#define DBLUE   0x1414
#define DRED    0xc0c0
#define DYELLOW  0xe6e6
 

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
String curren_cy;
String them_e;

float batVoltage = M5.Axp.GetBatVoltage();

int count = 0;

int count2 = 0;

String dollar = "\xA3";

// DNSServer dnsServer;
WebServer webServer(80);

// wifi and config store
Preferences preferences;


void setup() {
  m5.begin();
  Wire.begin();
  M5.Lcd.setSwapBytes(true);
  M5.Axp.EnableCoulombcounter();
  preferences.begin("wifi-config");

    M5.Lcd.fillScreen(TFT_BLACK);
    M5.Lcd.setSwapBytes(true);
    M5.Lcd.pushImage(40, 50, iotaWidth, iotaHeight, iota);
    //M5.Lcd.pushImage(0, 0, manheimWidth, manheimHeight, manheim);
    //M5.Lcd.pushImage(0, 0, tirolWidth, tirolHeight, tirol);

    delay(6000);
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
  
  if(M5.BtnA.isPressed())  
  M5.Axp.SetLcdVoltage(2600);
  



  if(M5.BtnB.isPressed())  M5.Axp.SetLcdVoltage(2800);
  if(M5.BtnC.isPressed())  M5.Axp.SetLcdVoltage(3200);

  if (count2 >=2600) {
    printTickerDataIOTA("MIOTA");

    //For the new API, you can use the currency ID or abbreviated name, such as
    //Bitcoin, you can view the letter after Circulating Supply at https://coinmarketcap.com/, it is BTC
  }
  
  count2++;
  
  delay(150);

}


boolean restoreConfig() {
  wifi_ssid = preferences.getString("WIFI_SSID");
  wifi_password = preferences.getString("WIFI_PASSWD");
  cmc_api_key = preferences.getString("CMC_API_KEY");
  curren_cy = preferences.getString("CURRENCY");  
  them_e = preferences.getString("THEME"); 
  M5.Lcd.pushImage(280, 2, infoWidth, infoHeight, info);

  M5.Lcd.pushImage(40, 100, wifilogoWidth, wifilogoHeight, wifilogo);

  Serial.print("WIFI-SSID: ");
  M5.Lcd.drawString("WIFI-SSID: ", 0, 0, 2);
  Serial.println(wifi_ssid);
  M5.Lcd.drawString(wifi_ssid, 100, 0, 2);
  Serial.print("WIFI-PASSWD: ");
  M5.Lcd.drawString("WIFI-PASSWD: ", 0, 20, 2);
  Serial.println(wifi_password);
  M5.Lcd.drawString(wifi_password, 100, 20, 2);
  Serial.print("CMC_API_KEY: ");
  M5.Lcd.drawString("CMC_API_KEY: ", 0, 40, 2);
  Serial.println(cmc_api_key);
  M5.Lcd.drawString(cmc_api_key, 100, 40, 2);
  Serial.print("CURRENCY: ");
  M5.Lcd.drawString("CURRENCY: ", 0, 60, 2);
  Serial.println(curren_cy);
  M5.Lcd.drawString(curren_cy, 100, 60, 2);

  Serial.print("THEME: ");
  M5.Lcd.drawString("THEME: ", 0, 80, 2);
  Serial.println(them_e);
  M5.Lcd.drawString(them_e, 100, 80, 2);

          
  Serial.print("Bat Voltage: ");
  Serial.println(M5.Axp.GetBatVoltage()); 
  Serial.print("Bat Current: ");
  Serial.println(M5.Axp.GetBatCurrent());
  Serial.print("USB Voltage: ");
  Serial.println(M5.Axp.GetVBusVoltage());
  Serial.print("USB Current: ");
  Serial.println(M5.Axp.GetVBusCurrent());
  Serial.print("Vin Voltage: ");
  Serial.println(M5.Axp.GetVinVoltage());
  Serial.print("Vin Current: ");
  Serial.println(M5.Axp.GetVinCurrent());
  Serial.print("Bat Power: ");
  Serial.println(M5.Axp.GetBatPower());  

  delay(2000);
  
  WiFi.begin(wifi_ssid.c_str(), wifi_password.c_str());

  if(wifi_ssid.length() > 0) {
    return true;
  } 
    else {
    return false;
  }
}

boolean checkConnection() {
  int count = 0;
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.pushImage(280, 2, infoWidth, infoHeight, info);

  M5.Lcd.pushImage(40, 100, wifilogoWidth, wifilogoHeight, wifilogo);

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
      preferences.remove("CMC_API_KEY");
      preferences.remove("CURRENCY");
      preferences.remove("THEME");
      delay(1000);
      ESP.restart();

  return false;
}

void startWebServer() {
  if (settingMode) {
    M5.Lcd.fillScreen(TFT_BLACK);

    M5.Lcd.pushImage(40, 100, wifilogoWidth, wifilogoHeight, wifilogo);

    M5.Lcd.pushImage(270, 190, wsetupWidth, wsetupHeight, wsetup);

    M5.Lcd.setTextColor(WHITE);
    Serial.print("Starting Web Server at ");
    M5.Lcd.drawString("Starting Web Server at 192.168.4.1", 0, 0, 2);
    Serial.println(WiFi.softAPIP());
    webServer.on("/settings", []() {
      String s = "<h1>Wi-Fi CMC-API and Currency Settings</h1><p>Please enter your password by selecting the SSID<br> and your Coinmarketcap API Key.<br>Currency USD or EUR</p>";
      s += "<form method=\"get\" action=\"setap\"><label>SSID: </label><select name=\"ssid\">";
      s += ssidList;
      s += "</select><br><br>Wifi Password: <input name=\"pass\" length=64 type=\"password\">";
      s += "<br><br>CMC API Key: <input name=\"apikey\" length=64 type=\"text\">";
      s += "<br><br>Currency: <input name=\"currency\" length=3 type=\"text\">";
      s += "<br><br>Theme: <input name=\"theme\" length=3 type=\"text\">";
      s += "<br><br><input type=\"submit\"></form>";      
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
      String currency = urlDecode(webServer.arg("currency"));
      Serial.print("Currency: ");
      M5.Lcd.print("Currency: ");
      Serial.println(currency);
      M5.Lcd.println(currency);
      String theme = urlDecode(webServer.arg("theme"));
      Serial.print("Theme: ");
      M5.Lcd.print("Theme: ");
      Serial.println(theme);
      M5.Lcd.println(theme);
      Serial.println("Writing API to EEPROM...");
      M5.Lcd.println("Writing API to EEPROM...");


      // Store wifi config
      Serial.println("Writing Password to nvr...");
      M5.Lcd.println("Writing Password to nvr...");
      preferences.putString("WIFI_SSID", ssid);
      preferences.putString("WIFI_PASSWD", pass);
      preferences.putString("CMC_API_KEY", apikey);
      preferences.putString("CURRENCY", currency);
      preferences.putString("THEME", theme);      

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
      webServer.send(200, "text/html", makePage("AP setup mode", s));
    });
  }
  else {
    
    if (them_e == "LIGHT"){
    M5.Lcd.fillScreen(LTFT_COLOR3);
     }
    else {M5.Lcd.fillScreen(DTFT_COLOR3);
     }
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
      preferences.remove("CURRENCY");
      preferences.remove("THEME");
      String s = "<h1>Wi-Fi settings was reset.</h1><p>Please reset device.</p>";
      webServer.send(200, "text/html", makePage("Reset Wi-Fi Settings", s));
      delay(3000);
      ESP.restart();
    });
    
// Header 
  if (them_e == "LIGHT"){
    M5.Lcd.fillRect(0, 0, 320, 27, LTFT_COLOR1);
    M5.Lcd.drawLine(0, 27, 320, 27, LTFT_COLOR2);
    } else { M5.Lcd.fillRect(0, 0, 320, 27, DTFT_COLOR1);
             M5.Lcd.drawLine(0, 27, 320, 27, DTFT_COLOR2);
             }

    
  }
  printTickerDataIOTA("MIOTA");  
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
  M5.Lcd.drawString("Starting Access Point at: Price-Ticker_SETUP", 0, 20, 2);
  Serial.print(apSSID);
  Serial.println("\"");
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


    CoinMarketCapApi api(client, cmc_api_key);
    CMCTickerResponse response = api.GetTickerInfo(ticker, curren_cy);
    
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

        Serial.print("Bat Voltage: ");
        Serial.println(M5.Axp.GetBatVoltage()); 
        Serial.print("Bat Current: ");
        Serial.println(M5.Axp.GetBatCurrent());
        Serial.print("USB Voltage: ");
        Serial.println(M5.Axp.GetVBusVoltage());
        Serial.print("USB Current: ");
        Serial.println(M5.Axp.GetVBusCurrent());
        Serial.print("Vin Voltage: ");
        Serial.println(M5.Axp.GetVinVoltage());
        Serial.print("Vin Current: ");
        Serial.println(M5.Axp.GetVinCurrent());
        Serial.print("Bat Power: ");
        Serial.println(M5.Axp.GetBatPower()); 


        


// Wifi signal bars
        Serial.print("WiFi Signal strength: ");
        Serial.print(WiFi.RSSI());

        if (them_e == "LIGHT"){
        M5.Lcd.fillRect(280, 0, 40, 26, LTFT_COLOR1); //wifi RSSI
        } else M5.Lcd.fillRect(280, 0, 40, 26, DTFT_COLOR1);

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
          if (them_e == "LIGHT"){
            M5.Lcd.fillRect(281 + (b * 6), 23 - (b * 4), 5, b * 4, LTFT_TXT2);
            } else M5.Lcd.fillRect(281 + (b * 6), 23 - (b * 4), 5, b * 4, DTFT_TXT2);
        }
        
        if (them_e == "LIGHT"){
        M5.Lcd.pushImage(10, 40, iota3Width, iota3Height, iota3);

        } else {M5.Lcd.pushImage(10, 40, iota2Width, iota2Height, iota2);
        }        


// Battery stat

// reding battery voltage
    float battery_voltage = M5.Axp.GetBatVoltage();
    String voltage = String(battery_voltage);
    Serial.println(voltage);
    Serial.println("");

// battery symbol
   if (them_e == "LIGHT"){
    M5.Lcd.fillRect(4, 7, 28, 2, LTFT_TXT2);
    M5.Lcd.fillRect(4, 19, 28, 2, LTFT_TXT2);
    M5.Lcd.fillRect(4, 7, 2, 12, LTFT_TXT2);
    M5.Lcd.fillRect(32, 7, 2, 14, LTFT_TXT2);
    M5.Lcd.fillRect(34, 11, 3, 6, LTFT_TXT2);
    }
     else { 
       M5.Lcd.fillRect(4, 7, 28, 2, DTFT_TXT2);
       M5.Lcd.fillRect(4, 19, 28, 2, DTFT_TXT2);
       M5.Lcd.fillRect(4, 7, 2, 12, DTFT_TXT2);
       M5.Lcd.fillRect(32, 7, 2, 14, DTFT_TXT2);
       M5.Lcd.fillRect(34, 11, 3, 6, DTFT_TXT2);
      }

// battery level symbol
   if(battery_voltage <= 2.99){
      M5.Lcd.fillRect(4, 7, 28, 2, DRED);
      M5.Lcd.fillRect(4, 19, 28, 2, DRED);
      M5.Lcd.fillRect(4, 7, 2, 14, DRED);
      M5.Lcd.fillRect(32, 7, 2, 14, DRED);
      M5.Lcd.fillRect(34, 11, 3, 6, DRED);
      //M5.Lcd.drawString(voltage, 45, 8, 1);
    }
       
   if(battery_voltage >= 3.0){
    if (them_e == "LIGHT"){
      M5.Lcd.fillRect(6, 9, 26, 10, LTFT_COLOR1);
      }
      else {
      M5.Lcd.fillRect(6, 9, 26, 10, DTFT_COLOR1);
      }
      M5.Lcd.fillRect(6, 9, 4, 10, DRED);
      //M5.Lcd.drawString(voltage, 45, 8, 1);
    }
   if(battery_voltage >= 3.2){
    if (them_e == "LIGHT"){
      M5.Lcd.fillRect(6, 9, 26, 10, LTFT_COLOR1);
      }
      else {
      M5.Lcd.fillRect(6, 9, 26, 10, DTFT_COLOR1);
      }
      M5.Lcd.fillRect(6, 9, 6, 10, DRED);
      //M5.Lcd.drawString(voltage, 45, 8, 1);
    }
   if(battery_voltage >= 3.4){
    if (them_e == "LIGHT"){
      M5.Lcd.fillRect(6, 9, 26, 10, LTFT_COLOR1);
      }
      else {
      M5.Lcd.fillRect(6, 9, 26, 10, DTFT_COLOR1);
      }
      M5.Lcd.fillRect(6, 9, 11, 10, DYELLOW);
      //M5.Lcd.drawString(voltage, 45, 8, 1);
    }
   if(battery_voltage >= 3.6){
    if (them_e == "LIGHT"){
      M5.Lcd.fillRect(6, 9, 26, 10, LTFT_COLOR1);
      }
      else {
      M5.Lcd.fillRect(6, 9, 26, 10, DTFT_COLOR1);
      }
      M5.Lcd.fillRect(6, 9, 16, 10, DYELLOW);
      //M5.Lcd.drawString(voltage, 45, 8, 1);
    }
   if(battery_voltage >= 3.8){
    if (them_e == "LIGHT"){
      M5.Lcd.fillRect(6, 9, 26, 10, LTFT_COLOR1);
      }
      else {
      M5.Lcd.fillRect(6, 9, 26, 10, DTFT_COLOR1);
      }
      M5.Lcd.fillRect(6, 9, 21, 10, DGREEN);
      //M5.Lcd.drawString(voltage, 45, 8, 1);
    }
   if(battery_voltage >= 4.0){
    if (them_e == "LIGHT"){
      M5.Lcd.fillRect(6, 9, 26, 10, LTFT_COLOR1);
      }
      else {
      M5.Lcd.fillRect(6, 9, 26, 10, DTFT_COLOR1);
      }
      M5.Lcd.fillRect(6, 9, 26, 10, DGREEN);
      //M5.Lcd.drawString(voltage, 45, 8, 1);
    }
   if(battery_voltage >= 4.60){
    if (them_e == "LIGHT"){
      M5.Lcd.fillRect(6, 9, 26, 10, LTFT_COLOR1);
      }
      else {
      M5.Lcd.fillRect(6, 9, 26, 10, DTFT_COLOR1);
      }
      M5.Lcd.fillRect(6, 9, 26, 10, DGREEN);
      //M5.Lcd.drawString("CHG", 10, 7, 1);
    }
   if(battery_voltage >= 4.85){
    if (them_e == "LIGHT"){
      M5.Lcd.fillRect(6, 9, 26, 10, LTFT_COLOR1);
      }
      else {
      M5.Lcd.fillRect(6, 9, 26, 10, DTFT_COLOR1);
      }
      M5.Lcd.fillRect(6, 9, 26, 10, CYAN);
      //M5.Lcd.drawString("USB", 10, 7, 1);
    }





// Line
        M5.Lcd.drawLine(10, 133, 310, 133, DGRAY);
        
      if (them_e == "LIGHT"){
        M5.Lcd.setTextColor(LTFT_TXT2);
        }
        else {M5.Lcd.setTextColor(DTFT_TXT2);
        }
        M5.Lcd.drawString("IOTA Price Ticker", 65, 4, 4);
              
      if (them_e == "LIGHT"){
        M5.Lcd.setTextColor(DYELLOW);
        }
        else {M5.Lcd.setTextColor(YELLOW);
        }
        
        if (response.percent_change_1h < 0) {
            M5.Lcd.setTextColor(RED);          
            }
        
        if (response.percent_change_1h > 0) {
          if (them_e == "LIGHT"){
            M5.Lcd.setTextColor(DGREEN);
            } 
            else {
            M5.Lcd.setTextColor(GREEN);  
            }            
        }
        
// Price
       if (them_e == "LIGHT"){
        M5.Lcd.fillRect(115, 38, 205, 50, LTFT_COLOR3); 
         }
         else {
        M5.Lcd.fillRect(115, 38, 205, 50, DTFT_COLOR3);
        }
                
        M5.Lcd.drawString(String(response.price).c_str(), 115, 50, 6);
        if(curren_cy == "EUR"){

         if (response.price < 10) {
          M5.Lcd.drawString("EUR", 215, 67, 4);
         }

         if (response.price > 10) {
          M5.Lcd.drawString("EUR", 235, 67, 4);
         }
        }


        if(curren_cy == "USD"){

         if (response.price < 10) {
          M5.Lcd.drawString("USD", 215, 67, 4);
         }

         if (response.price > 10) {
          M5.Lcd.drawString("USD", 235, 67, 4);
         }
        }        

        if (response.price > 6) {
          M5.Lcd.pushImage(265, 38, schnitzelWidth, schnitzelHeight, schnitzel);              
        }
        
        if (response.price > 2) {
           M5.Lcd.pushImage(265, 38, rocketWidth, rocketHeight, rocket);              
        }

        if (response.price > 5) {
          M5.Lcd.pushImage(265, 38, moonWidth, moonHeight, moon);              
        }


// Rank

        M5.Lcd.setTextColor(DBLUE);
        M5.Lcd.drawString("Rank:", 115, 100, 4);
        if (them_e == "LIGHT"){
        M5.Lcd.fillRect(185, 100, 120, 20, LTFT_COLOR3); 
        }
         else {M5.Lcd.fillRect(185, 100, 120, 20, DTFT_COLOR3); 
        }        
        M5.Lcd.drawString(String(response.cmc_rank).c_str(), 190, 100, 4);




// hours change
      if (them_e == "LIGHT"){
        M5.Lcd.setTextColor(LTFT_TXT);
        }        
         else {M5.Lcd.setTextColor(DTFT_TXT);
        }
        M5.Lcd.drawString("% Price change 1h:", 11, 145, 4);
        if (them_e == "LIGHT"){
        M5.Lcd.fillRect(240, 140, 80, 90, LTFT_COLOR3); 
        }
         else {M5.Lcd.fillRect(240, 140, 80, 90, DTFT_COLOR3); 
        }

        if (them_e == "LIGHT"){
        M5.Lcd.fillRect(240, 140, 80, 90, LTFT_COLOR3); 
        }
         else {M5.Lcd.fillRect(240, 140, 80, 90, DTFT_COLOR3); 
        }

        
      if (them_e == "LIGHT"){
        M5.Lcd.setTextColor(DYELLOW);
        }
        else {M5.Lcd.setTextColor(YELLOW);
        }
        
        if (response.percent_change_1h < 0) {
            M5.Lcd.setTextColor(RED);          
            }
        
        if (response.percent_change_1h > 0) {
          if (them_e == "LIGHT"){
            M5.Lcd.setTextColor(DGREEN);
            } 
            else {
            M5.Lcd.setTextColor(GREEN);  
            }            
        }

        M5.Lcd.drawString(String(response.percent_change_1h).c_str(), 245, 145, 4);
        delay(1);


// 24 hours change
      if (them_e == "LIGHT"){
        M5.Lcd.setTextColor(LTFT_TXT);
        }
         else {M5.Lcd.setTextColor(DTFT_TXT);
        }
        M5.Lcd.drawString("% Price change 24h:", 11, 175, 4);
      if (them_e == "LIGHT"){
        M5.Lcd.setTextColor(DYELLOW);
        }
        else {M5.Lcd.setTextColor(YELLOW);
        }
        
        if (response.percent_change_24h < 0) {
            M5.Lcd.setTextColor(RED);          
            }
        
        if (response.percent_change_24h > 0) {
          if (them_e == "LIGHT"){
            M5.Lcd.setTextColor(DGREEN);
            } 
            else {
            M5.Lcd.setTextColor(GREEN);  
            }            
        }

        M5.Lcd.drawString(String(response.percent_change_24h).c_str(), 245, 175, 4);
        delay(1);


// 7d hours change
      if (them_e == "LIGHT"){
        M5.Lcd.setTextColor(LTFT_TXT);
        }
         else {M5.Lcd.setTextColor(DTFT_TXT);
        }
        M5.Lcd.drawString("% Price change 7d:", 11, 205, 4);
      if (them_e == "LIGHT"){
        M5.Lcd.setTextColor(DYELLOW);
        }
        else {M5.Lcd.setTextColor(YELLOW);
        }
        
        if (response.percent_change_7d < 0) {
            M5.Lcd.setTextColor(RED);          
            }
        
        if (response.percent_change_7d > 0) {
          if (them_e == "LIGHT"){
            M5.Lcd.setTextColor(DGREEN);
            } 
            else {
            M5.Lcd.setTextColor(GREEN);  
            }            
        }

        M5.Lcd.drawString(String(response.percent_change_7d).c_str(), 245, 205, 4);
      
        count2 = 2;
       

    }

    else {
        Serial.print("Error getting data: ");
        Serial.println(response.error);
        if (them_e == "LIGHT"){
        M5.Lcd.fillRect(115, 38, 205, 50, LTFT_COLOR3);
        }
        else {M5.Lcd.fillRect(115, 38, 205, 50, DTFT_COLOR3);
        }        
        M5.Lcd.pushImage(283, 53, alertWidth, alertHeight, alert);
        delay(1000);
    }

    
    Serial.println("---------------------------------");
}
