#define ARDUINOJSON_DECODE_UNICODE 1
#include <ArduinoJson.h>

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>

const char* ssid = "[REDACTED]";
const char* pass = "[REDACTED]";
const uint8_t fingerprint[20] = {0x33, 0x1F, 0xC6, 0xD5, 0x2C, 0x05, 0xC8, 0x23, 0x6D, 0xEF, 0xBB, 0xF4, 0x81, 0x63, 0x2D, 0x16, 0x8A, 0x15, 0xEF, 0x6D};

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 0
Adafruit_SSD1306 OLED(OLED_RESET);

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

  delay(3000);
}

void loop() {
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
          int confirmed = doc["totals"]["confirmed"];
          int dead = doc["totals"]["dead"];
          int recovered = doc["totals"]["recovered"];
          
          Serial.print("Confirmed infected in Norway: ");
          Serial.println(confirmed);

          OLED.begin();
          OLED.clearDisplay();

          JsonArray cases = doc["cases"];
          String scrollingString;
          String scrollSpace = "      ";

          // scrollingString += scrollSpace;
          
          for (byte i = 0; i < sizeof(cases) - 1; i++) {
            int confirmedCases = cases[i]["confirmed"];
            const char* county = cases[i]["name"];
            
            Serial.print(county);
            Serial.print(": ");
            Serial.println(confirmedCases);

            scrollingString += county;
            scrollingString += ": ";
            scrollingString += confirmedCases;
            scrollingString += scrollSpace;
          }

          OLED.setTextSize(1);
          OLED.setCursor(0, 0);
          OLED.print(scrollingString);
          OLED.setTextColor(WHITE);
          // OLED.startscrollleft(0x00, 0x06);

          OLED.setTextWrap(false);
          OLED.setTextSize(1);
          OLED.setCursor(0, 8);
          OLED.println("Inf. / Rec. / Dead:");
          
          OLED.setTextSize(2);
          OLED.setCursor(0, 16);
          OLED.print(confirmed);
          OLED.print("/");
          OLED.print(recovered);
          OLED.print("/");
          OLED.print(dead);
          
          OLED.display();
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
  delay(300000);
}