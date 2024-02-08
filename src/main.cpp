#include <Arduino.h>
#include <Adafruit_NeoPixel.h> //
#include <Wire.h>
#include <GxEPD.h>
#include <GxDEPG0290BS/GxDEPG0290BS.h>
#include GxEPD_BitmapExamples
#include <Adafruit_MAX31856.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h>
#include <Fonts/FreeMonoBold24pt7b.h>
#include <Fonts/FreeSansBold24pt7b.h>
#include <GxIO/GxIO_SPI/GxIO_SPI.h>
#include <GxIO/GxIO.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>
#include <HTTPUpdate.h>
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include "Adafruit_MAX1704X.h"
#include <ESPAsyncWebServer.h>
#include <WiFiClientSecure.h>
#include <Preferences.h>
#include "bitmap.h"

RTC_DATA_ATTR bool isVariableTrue = true; // Initial value

unsigned long previousMillis = 0; // will store last time LED was updated
unsigned long previousMillis_2 = 0;
const long interval = 60000;
const long mini_interval = 1000;

/// DEFINES///
// NEOPIXEL
#define PIN 12
#define NUMPIXELS 4

/// Battery
#define DIV 33
float vout = 0.0;
float vin = 0.0;
float R1 = 10000.0; // resistance of R1 (100K) -see text!
float R2 = 3300.0; // resistance of R2 (10K) - see text!
int value = 0;

/// MAX41856
#define CSPin 25
#define DIPin 26
#define DOPin 27
#define CLKPin 14
#define DRDY_PIN 32

///Buzzer
#define buzzer 19

//Swithes
#define SwA 2
#define SwB 15
#define SwC 8
#define SwD 7

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_BRG + NEO_KHZ800);
float highTemp = 25.0; // Default high temperature
float lowTemp = 20.0;  // Default low temperature

int Ftemp = 001;
int wifitimer = 0;
int mqtttimer = 0;
float BatteryV = 100;

float previousTemperature = -100.0;
float stovetemp;
float temperature = 0;

Preferences p;
Preferences q;

float SThighTemp;
float STlowTemp;

unsigned long fwchecktime;
unsigned long fwchecktimeprev;


String FirmwareVer = {
  "0.7"
};


 #define URL_fw_Version "https://raw.githubusercontent.com/NFAZ10/Woodstove_Working/main/src/fw.txt"
 #define URL_fw_Bin "https://raw.githubusercontent.com/NFAZ10/Woodstove_Working/main/.pio/build/esp32dev/firmware.bin"


// DigiCert root certificate has expiry date of 10 Nov 2031
const char *rootCACertificate =
    "-----BEGIN CERTIFICATE-----\n"
    "MIIDrzCCApegAwIBAgIQCDvgVpBCRrGhdWrJWZHHSjANBgkqhkiG9w0BAQUFADBh\n"
    "MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n"
    "d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD\n"
    "QTAeFw0wNjExMTAwMDAwMDBaFw0zMTExMTAwMDAwMDBaMGExCzAJBgNVBAYTAlVT\n"
    "MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\n"
    "b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IENBMIIBIjANBgkqhkiG\n"
    "9w0BAQEFAAOCAQ8AMIIBCgKCAQEA4jvhEXLeqKTTo1eqUKKPC3eQyaKl7hLOllsB\n"
    "CSDMAZOnTjC3U/dDxGkAV53ijSLdhwZAAIEJzs4bg7/fzTtxRuLWZscFs3YnFo97\n"
    "nh6Vfe63SKMI2tavegw5BmV/Sl0fvBf4q77uKNd0f3p4mVmFaG5cIzJLv07A6Fpt\n"
    "43C/dxC//AH2hdmoRBBYMql1GNXRor5H4idq9Joz+EkIYIvUX7Q6hL+hqkpMfT7P\n"
    "T19sdl6gSzeRntwi5m3OFBqOasv+zbMUZBfHWymeMr/y7vrTC0LUq7dBMtoM1O/4\n"
    "gdW7jVg/tRvoSSiicNoxBN33shbyTApOB6jtSj1etX+jkMOvJwIDAQABo2MwYTAO\n"
    "BgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUA95QNVbR\n"
    "TLtm8KPiGxvDl7I90VUwHwYDVR0jBBgwFoAUA95QNVbRTLtm8KPiGxvDl7I90VUw\n"
    "DQYJKoZIhvcNAQEFBQADggEBAMucN6pIExIK+t1EnE9SsPTfrgT1eXkIoyQY/Esr\n"
    "hMAtudXH/vTBH1jLuG2cenTnmCmrEbXjcKChzUyImZOMkXDiqw8cvpOp/2PV5Adg\n"
    "06O/nVsJ8dWO41P0jmP6P6fbtGbfYmbW0W5BjfIttep3Sp+dWOIrWcBAI+0tKIJF\n"
    "PnlUkiaY4IBIqDfv8NZ5YBberOgOzW6sRBc4L0na4UU+Krk2U886UAb3LujEV0ls\n"
    "YSEY1QSteDwsOoBrp+uvFRTp2InBuThs4pFsiv9kuXclVzDAGySj4dzp30d8tbQk\n"
    "CAUw7C29C79Fv1C5qfPrmAESrciIxpg0X40KPMbp1ZWVbd4=\n"
    "-----END CERTIFICATE-----\n";

GxIO_Class io(SPI, /*CS=5*/ SS, /*DC=*/17, /*RST=*/16); // arbitrary selection of 17, 16
GxEPD_Class display(io, /*RST=*/16, /*BUSY=*/4);        // arbitrary selection of (16), 4

void connect_wifi();
void firmwareUpdate();
int FirmwareVersionCheck();
void repeatedCall();

#define DRDY_PIN 32
Adafruit_MAX31856 maxthermo = Adafruit_MAX31856(CSPin, DIPin, DOPin, CLKPin);

float readTemperature()
{

  int Ftemp = ((maxthermo.readThermocoupleTemperature() * 1.8) + 32);
  return Ftemp;
}

void setLED(int r, int b, int g)
{
  pixels.setPixelColor(0, pixels.Color(r, g, b));
  pixels.setPixelColor(1, pixels.Color(r, g, b));
  pixels.setPixelColor(2, pixels.Color(r, g, b));
  pixels.setPixelColor(3, pixels.Color(r, g, b));
  pixels.show();
}

void checkTemp(int high, int low, int temp)
{

  if (high >= temp)
  {
    Serial.println("TEMP OVER LIMIT!!!");
    setLED(255, 0, 0);
  }
  if (low <= temp)
  {
    Serial.println("ADD WOOD OR OPEN AIR");
    setLED(0, 0, 255);
  }
}

void showPartialUpdate(int Ftemp)
{

  uint16_t box_x = 0;
  uint16_t box_y = 0;
  uint16_t box_w = 296;
  uint16_t box_h = 110;
  uint16_t cursor_y = box_y + box_h - 6;
  float value = 13.95;
  display.setFont(&FreeSansBold24pt7b);
  display.setTextColor(GxEPD_WHITE);
  display.setRotation(45);
  display.setTextSize(3);
  // draw background
  display.fillRect(box_x, box_y, box_w, box_h, GxEPD_BLACK);
  display.setCursor(20, 100); // leftright,updown
  display.print(Ftemp);
  display.updateWindow(box_x, box_y, box_w, box_h, true);
  delay(2000);
}

void showPartialUpdateVOL(int BV)
{

  uint16_t box_x = 0;
  uint16_t box_y = 108;
  uint16_t box_w = 100;
  uint16_t box_h = 20;
  uint16_t cursor_y = box_y + box_h - 6;
  float value = 13.95;
  display.setFont(&FreeMonoBold12pt7b);
  display.setTextColor(GxEPD_BLACK);
  display.setRotation(45);
  display.setTextSize(1);
  // draw background
  display.fillRect(box_x, box_y, box_w, box_h, GxEPD_WHITE);
  display.setCursor(20, 125); // leftright,updown
  display.print(BV);
  display.updateWindow(box_x, box_y, box_w, box_h, true);
  delay(2000);
}

AsyncWebServer server(80);






void setup()
{

  Serial.begin(115200);
  Serial.println("WoodStove...V"+FirmwareVer);
  display.init(); // enable diagnostic output on Serial

  pinMode(buzzer, OUTPUT);
  pinMode(DIV, INPUT); //It is necessary to declare the input pin
  


  pixels.begin();
  /*

   p.begin("test-1", true);
   q.begin("test-2", true);

   SThighTemp = p.getUInt("hightemp", 0);
   STlowTemp = q.getUInt("lowTemp", 0);

 */

  tone(buzzer,1000, 1000);
  tone(buzzer,2000, 1000);
  tone(buzzer,3000, 1000);

  display.setRotation(1);
  display.setFont(&FreeMonoBold12pt7b);
  display.setTextColor(GxEPD_BLACK);
  display.fillScreen(GxEPD_WHITE);
  display.setCursor(20, 64);
  display.print("WIFI SETUP");
  display.update();
  delay(2000);
  display.fillScreen(GxEPD_WHITE);
  display.update();

  display.drawBitmap(epd_bitmap_Boot_screen, 0, 0, 296, 128, GxEPD_WHITE);
  display.update();
  delay(5000);
  display.fillScreen(GxEPD_WHITE);
  display.update();

  ///////////////WIFI MANAGER/////////////////////

  WiFiManager wm;
  // wiFiManager.autoConnect("CustomAP");

  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
  Serial.println("WIFI ENABLED");
  // showPartialUpdateWIFION();

  // wm.resetSettings();  //FORTESTING

  wm.setConfigPortalBlocking(true);
  wm.setConfigPortalTimeout(120);
  // automatically connect using saved credentials if they exist
  // If connection fails it starts an access point with the specified name
  if (wm.autoConnect("WoodStoveSetup"))
  {
    Serial.println("connected...yeey :)");
  }
  else
  {
    Serial.println("Configportal running");
    // showPartialUpdateWIFIFAIL();
  }

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  ////////////DISPLAY IP///////////////////
  display.setRotation(1);
  display.setFont(&FreeMonoBold12pt7b);
  display.setTextColor(GxEPD_BLACK);
  display.fillScreen(GxEPD_WHITE);
  display.setCursor(20, 64);
  display.print("IP ADDRESS is");
  display.setCursor(40, 80);
  display.print(WiFi.localIP());
  display.update();

  delay(10000);
  ////////////////TEMP SENSOR//////////////

  boolean METRIC = false;

  Serial.println("MAX31856 thermocouple test");

  pinMode(DRDY_PIN, INPUT);
  pinMode(SwA,INPUT_PULLUP);
  pinMode(SwB,INPUT_PULLUP);
  pinMode(SwC,INPUT_PULLUP);
  pinMode(SwD,INPUT_PULLUP);

  if (!maxthermo.begin())
  {
    Serial.println("Could not initialize thermocouple.");
    while (1)
      delay(10);
  }

  maxthermo.setThermocoupleType(MAX31856_TCTYPE_K);
  maxthermo.setConversionMode(MAX31856_CONTINUOUS);

  ////////////
  Serial.print("Wi-Fi Status: ");
  Serial.println(WiFi.status());

  Serial.println("setup done");

  display.fillScreen(GxEPD_WHITE);
  display.update();

  delay(100);
  // Serve web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    String html = "<html><body>";
    html += "<h1>Temperature Monitoring</h1>";
    html += "<p>High Temperature: <input type='number' step='0.1' value='" + String(highTemp) + "' id='highTemp'></p>";
    html += "<p>Low Temperature: <input type='number' step='0.1' value='" + String(lowTemp) + "' id='lowTemp'></p>";
    html += "<p>Current Temperature: " + String(readTemperature()) + " &#8451;</p>";
    html += "<p>Battery Percentage: " + String(vin) + "%</p>";
   // html += "<img src='/image.jpg' width='300' height='200'>"; // Replace 'image.jpg' with your image file
    html += "<p>Variable Status: " + String(isVariableTrue ? "True" : "False") + "</p>";
    html += "<button onclick='updateSettings()'>Update Settings</button>";
    html += "<button onclick='toggleVariable()'>Reboot</button>";
    html += "<button onclick='resetWiFiManager()'>Reset WiFi</button>";
    html += "<script>function updateSettings() {"
            "var highTemp = document.getElementById('highTemp').value;"
            "var lowTemp = document.getElementById('lowTemp').value;"
            "fetch('/update?highTemp=' + highTemp + '&lowTemp=' + lowTemp);"
            "}"
            "function toggleVariable() {"
            "fetch('/toggle');"
            "}"
            "function resetWiFiManager() {"
            "fetch('/resetWiFiManager');"
            "}</script>";
    html += "</body></html>";
    request->send(200, "text/html", html); });

  // Endpoint to update settings
  server.on("/update", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    if(request->hasParam("highTemp") && request->hasParam("lowTemp")){
      highTemp = request->getParam("highTemp")->value().toFloat();
      lowTemp = request->getParam("lowTemp")->value().toFloat();
    }
    request->send(200, "text/plain", "Settings Updated"); });

  // Endpoint to toggle boolean variable and restart
  server.on("/toggle", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    isVariableTrue = !isVariableTrue;
    ESP.restart(); // Reboot the ESP32
    request->send(200, "text/plain", "Variable Toggled and ESP32 Restarted"); });

  // Endpoint to reset WiFiManager
  server.on("/resetWiFiManager", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    WiFiManager wifiManager;
    wifiManager.resetSettings(); // Erase WiFiManager stored credentials
    ESP.restart(); // Reboot the ESP32
    request->send(200, "text/plain", "WiFiManager Reset and ESP32 Restarted"); });

  // Serve image file
  // server.serveStatic("/image.jpg", U_SPIFFS, "/image.jpg"); // Replace 'image.jpg' with your image file

  server.begin();
}

void loop()
{

   value = analogRead(DIV);
   vout = (value * 3.3) / 1024.0; // see text
   vin = vout / (R2/(R1+R2)); 
   if (vin<0.09) {
   vin=0.0;//statement to quash undesired reading !
   }

  fwchecktime = millis();

   if ((fwchecktimeprev-fwchecktime)>=6000){

  if (FirmwareVersionCheck())
  {
    firmwareUpdate();
  }
}
  fwchecktimeprev = fwchecktime;

  delay(1000);
  // wm.process();

  // Send the updated pixel colors to the hardware.

  previousTemperature = temperature;
  delay(1000);

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  int count = 0;
  /*
    while (digitalRead(DRDY_PIN)) {
      if (count++ > 200) {
        count = 0;
        Serial.print(".");
      }
    }
  */

  previousTemperature = temperature;
  temperature = maxthermo.readThermocoupleTemperature();

  Ftemp = ((temperature * 1.8) + 32);

  Serial.print("TEMP:");
  Serial.println(Ftemp);
  // showPartialUpdate(Ftemp);
  Serial.print("High Temp: ");
  Serial.println(highTemp);
  Serial.print("Low Temp:  ");
  Serial.println(lowTemp);
  if (previousTemperature != temperature)
  {

    showPartialUpdate(Ftemp);

    // Publish temperature to MQTT

    Serial.print(maxthermo.readThermocoupleTemperature());
    Serial.print("C  ");
    Serial.print(Ftemp);
    Serial.println("F");
  }
  else
  {
    Serial.println("NO CHANGE IN TEMP");
  }

  checkTemp(SThighTemp, STlowTemp, Ftemp);

  if (highTemp != SThighTemp)
  {

    p.putUInt("highTemp", highTemp);
    q.putUInt("lowTemp", lowTemp);

    p.end();
    q.end();
  }
}

void firmwareUpdate(void)
{
  HTTPClient https;

  WiFiClientSecure client;
  client.setCACert(rootCACertificate);
  t_httpUpdate_return ret = httpUpdate.update(client, URL_fw_Bin);

  switch (ret)
  {
  case HTTP_UPDATE_FAILED:
    Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
    break;

  case HTTP_UPDATE_NO_UPDATES:
    Serial.println("HTTP_UPDATE_NO_UPDATES");
    break;

  case HTTP_UPDATE_OK:
    Serial.println("HTTP_UPDATE_OK");
    break;
  }
}
int FirmwareVersionCheck(void)
{
  String payload;
  int httpCode;
  String fwurl = "";
  fwurl += URL_fw_Version;
  fwurl += "?";
  fwurl += String(rand());
  Serial.println(fwurl);
  WiFiClientSecure *client = new WiFiClientSecure;

  if (client)
  {
    client->setCACert(rootCACertificate);

    // Make sure it is destroyed before WiFiClientSecure *client is / Add a scoping block for HTTPClient https to m

    HTTPClient https;

    if (https.begin(*client, fwurl))
    { // HTTPS
      Serial.print("[HTTPS] GET...\n");
      // start connection and send HTTP header
      delay(100);
      httpCode = https.GET();
      delay(100);
      if (httpCode == HTTP_CODE_OK) // if version received
      {
        payload = https.getString(); // save received version
      }
      else
      {
        Serial.print("error in downloading version file:");
        Serial.println(httpCode);
      }
      https.end();
    }
    delete client;
  }

  if (httpCode == HTTP_CODE_OK) // if version received
  {
    payload.trim();
    if (payload.equals(FirmwareVer))
    {
      Serial.printf("\nDevice already on latest firmware version:%s\n", FirmwareVer);
      return 0;
    }
    else
    {
      Serial.println(payload);
      Serial.println("New firmware detected");
      firmwareUpdate();

      return 1;
    }
  }
  return 0;
}