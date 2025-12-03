#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Update.h>

const char* ssid = "POCOF4";
const char* password = "ryan1234";

// URL of the compiled .bin firmware (raw GitHub URL)
const char* firmware_url = "https://raw.githubusercontent.com/USERNAME/REPO/BRANCH/firmware.bin";

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected!");

  performOTA();
}

void loop() {
  // Nothing here
}

void performOTA() {
  HTTPClient http;
  http.begin(firmware_url);
  int httpCode = http.GET();

  if (httpCode == HTTP_CODE_OK) {
    int contentLength = http.getSize();
    bool canBegin = Update.begin(contentLength);

    if (canBegin) {
      WiFiClient* client = http.getStreamPtr();
      size_t written = Update.writeStream(*client);

      if (written == contentLength) {
        Serial.println("OTA done!");
      } else {
        Serial.println("OTA failed: written != contentLength");
      }

      if (Update.end()) {
        if (Update.isFinished()) {
          Serial.println("Update successfully finished. Rebooting...");
          ESP.restart();
        } else {
          Serial.println("Update not finished? Something went wrong!");
        }
      } else {
        Serial.println("Update failed. Error #: " + String(Update.getError()));
      }
    } else {
      Serial.println("Not enough space to begin OTA");
    }
  } else {
    Serial.println("HTTP GET failed. Code: " + String(httpCode));
  }

  http.end();
}
