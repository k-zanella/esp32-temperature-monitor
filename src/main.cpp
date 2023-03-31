#include <Arduino.h>
#include <DHT.h>
#include <BlynkSimpleEsp32.h>
#include <WiFi.h>

/*
* secrets.hpp:
*   BLYNK_TEMPLATE_ID
*   BLYNK_TEMPLATE_NAME
*   BLYNK_AUTH_TOKEN
*   WIFI_SSID
*   PASSWD
*/
#include "secrets.hpp"

#define BLYNK_NO_BUILTIN   // Disable built-in analog & digital pin operations
#define BLYNK_NO_FLOAT     // Disable float operations

#define DHT_PIN 26

DHT dht(DHT_PIN, DHT11);
BlynkTimer timer;

int temperature, humidity, hic = 0;

void send_data() {
    temperature = dht.readTemperature();
    humidity = dht.readHumidity();
    hic = dht.computeHeatIndex(false);

    // check if any reads failed
    if (isnan(temperature) || isnan(humidity) || isnan(hic)) {
        Serial.println(F("Failed to read from DHT11 sensor!"));
        return;
    }

    Blynk.virtualWrite(V0, temperature);
    Blynk.virtualWrite(V1, humidity);
    Blynk.virtualWrite(V2, hic);
}

// restart esp32
BLYNK_WRITE(InternalPinDBG) {
  if (String(param.asStr()) == "reboot") {
    Serial.println("Rebooting...");

    // TODO: Perform any neccessary preparation here,
    // i.e. turn off peripherals, write state to EEPROM, etc.

    // NOTE: You may need to defer a reboot,
    // if device is in process of some critical operation.

    ESP.restart();
  }
}

void connect_wifi() {
    WiFi.begin(WIFI_SSID, PASSWD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1500);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
}

void setup() {
    Serial.begin(9600);
    connect_wifi();
    dht.begin();

    Blynk.config(BLYNK_AUTH_TOKEN);
    timer.setInterval(2000L, send_data);
}

void loop() {
    Blynk.run();
    timer.run();
}