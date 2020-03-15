#include "config.h"

// Add support for unicode in JSON
#define ARDUINOJSON_DECODE_UNICODE 1
#include <ArduinoJson.h>

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 0
Adafruit_SSD1306 OLED(OLED_RESET);

// Wifi Settings (edit config.h)
const char* ssid = WIFI_SSID;
const char* pass = WIFI_PASS;

// SSL Certificate Fingerprint for the URL we are fetching
// https://knowledge.digicert.com/solution/SO9840.html
const uint8_t fingerprint[20] = {0x33, 0x1F, 0xC6, 0xD5, 0x2C, 0x05, 0xC8, 0x23, 0x6D, 0xEF, 0xBB, 0xF4, 0x81, 0x63, 0x2D, 0x16, 0x8A, 0x15, 0xEF, 0x6D};

// Intervals
#define INTERVAL_WIFI_WAIT 3000
#define INTERVAL_STATS_UPDATE 300000

// Timers
unsigned long timer_stats_update = 0;

// Variables
String scrolling_text = "";
int confirmed = 0;
int recovered = 0;
int dead = 0;

int x;
int minX;

void setup() {
  OLED.begin();
  OLED.clearDisplay();

  OLED.setTextWrap(false);
  OLED.setTextSize(1);
  OLED.setTextColor(WHITE);
  OLED.setCursor(0, 0);
  OLED.println("Kobler til wifi:");
  OLED.print(ssid);
  
  OLED.display();
  
  Serial.begin(115200);

  Serial.println();

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);

  delay(10);

  Serial.print("Connecting to ");
  Serial.print(ssid);
  while (WiFi.status() != WL_CONNECTED) {
     delay(500);
     Serial.print(".");
  }

  Serial.println("");
  Serial.print("Successfully connected to ");
  Serial.println(ssid);
  Serial.println(WiFi.localIP());

  OLED.clearDisplay();
  OLED.setTextWrap(false);
  OLED.setTextSize(1);
  OLED.setTextColor(WHITE);
  OLED.setCursor(0, 0);
  OLED.println("Koblet til wifi!");
  OLED.print("IP: ");
  OLED.println(WiFi.localIP());

  OLED.display();

  delay(INTERVAL_WIFI_WAIT);

  // Fetch data
  fetch_data();
  
  // Show stats
  show_stats();

  x = OLED.width();
  minX = -6 * scrolling_text.length();
}

void loop() {
  OLED.clearDisplay();
  
  if (millis() >= timer_stats_update + INTERVAL_STATS_UPDATE) {
    timer_stats_update += INTERVAL_STATS_UPDATE;
    fetch_data();
  }

  show_stats();
  
  scroll_text();

  OLED.display();
}

void fetch_data() {
  std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);

  client->setFingerprint(fingerprint);

  HTTPClient https;

  Serial.print("[HTTPS] begin...\n");
  if (https.begin(*client, "https://www.vg.no/spesial/2020/corona-viruset/data/norway-table-overview/?region=county")) {  // HTTPS

    Serial.print("[HTTPS] GET...\n");
    // start connection and send HTTP header
    int httpCode = https.GET();

    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTPS] GET... code: %d\n", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
        String payload = https.getString();
        Serial.println("JSON data fetched");

        const size_t capacity = JSON_ARRAY_SIZE(11) + JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3) + 11*JSON_OBJECT_SIZE(6) + 2048;
        DynamicJsonDocument doc(capacity);

        DeserializationError error = deserializeJson(doc, payload);

        if (error) {
          Serial.print("Error parsing JSON data: ");
          Serial.println(error.c_str());
          return;
        } else {
          confirmed = doc["totals"]["confirmed"];
          recovered = doc["totals"]["recovered"];
          dead = doc["totals"]["dead"];
          
          Serial.print("Confirmed infected in Norway: ");
          Serial.println(confirmed);

          JsonArray cases = doc["cases"];
          String scroll_space = "      ";
          
          for (byte i = 0; i < sizeof(cases) - 1; i++) {
            int confirmedCases = cases[i]["confirmed"];
            const char* county = cases[i]["name"];
            
            Serial.print(county);
            Serial.print(": ");
            Serial.println(confirmedCases);

            scrolling_text += county;
            scrolling_text += ": ";
            scrolling_text += confirmedCases;
            scrolling_text += scroll_space;
          }
        }
      }
    } else {
      Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
    }

    https.end();
  } else {
    Serial.printf("[HTTPS] Unable to connect\n");
  }

  Serial.println("Wait 5m before next fetch...");
}

void show_stats() {
  OLED.setTextSize(1);
  OLED.setCursor(0, 0);
  OLED.print("Infected:");
  
  OLED.setTextSize(2);
  OLED.setCursor(0, 8);
  OLED.print(confirmed);

  OLED.setTextSize(1);
  OLED.setCursor(94, 8);
  OLED.print("Rec: ");
  OLED.setCursor(120, 8);
  OLED.print(recovered);

  OLED.setCursor(88, 16);
  OLED.print("Dead: ");
  OLED.setCursor(120, 16);
  OLED.print(dead);
}

void scroll_text() {
  OLED.setCursor(x, 24);
  OLED.print(scrolling_text);
  
  x = x-4;
  if (x < minX) x = OLED.width();
}
