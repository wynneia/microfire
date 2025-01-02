#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "lynxAdapt";
const char* password = "janice69";

const char* apiUrl = "http://mikro.a2s.ltd/api/sensor-data";
const char* pumpStatusUrl = "http://mikro.a2s.ltd/api/pump/status";
const char* pumpControlUrl = "http://mikro.a2s.ltd/api/pump/control";

const String bearerToken = "your_bearer_token";

#define ARDUINO_RX D2
#define ARDUINO_TX D1

SoftwareSerial arduinoSerial(ARDUINO_RX, ARDUINO_TX);

int flameValue = 0;
int smokeValue = 0;
bool pumpStatus = false;

unsigned long lastArduinoReadTime = 0;
unsigned long lastWebPostTime = 0;
unsigned long lastPumpCheckTime = 0;

const unsigned long arduinoReadInterval = 1000; // 1 second
const unsigned long webPostInterval = 3000; // 3 seconds
const unsigned long pumpCheckInterval = 5000; // 5 seconds

void setup() {
  Serial.begin(9600);
  arduinoSerial.begin(9600);
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  Serial.println("NodeMCU is ready!");
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - lastArduinoReadTime >= arduinoReadInterval) {
    lastArduinoReadTime = currentMillis;
    if (arduinoSerial.available()) {
      String data = arduinoSerial.readStringUntil('\n');
      Serial.println("Received from Arduino: " + data);
      parseSensorData(data);
    }
  }

  if (currentMillis - lastWebPostTime >= webPostInterval) {
    lastWebPostTime = currentMillis;
    sendSensorData(flameValue, smokeValue);
  }

  if (currentMillis - lastPumpCheckTime >= pumpCheckInterval) {
    lastPumpCheckTime = currentMillis;
    fetchPumpStatus();
  }
}

void parseSensorData(String data) {
  int firstComma = data.indexOf(',');
  int secondComma = data.indexOf(',', firstComma + 1);

  if (firstComma != -1 && secondComma != -1) {
    flameValue = data.substring(0, firstComma).toInt();
    smokeValue = data.substring(firstComma + 1, secondComma).toInt();
  }
}

void sendSensorData(int flameValue, int smokeValue) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    WiFiClient client;

    http.begin(client, apiUrl);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", "Bearer " + bearerToken);

    String payload = String("{\"flameValue\":") + flameValue + 
                     ",\"smokeValue\":" + smokeValue + "}";

    int httpResponseCode = http.POST(payload);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Response: " + response);
    } else {
      Serial.println("Error in sending POST request: " + String(httpResponseCode));
    }

    http.end();
  } else {
    Serial.println("Error: No WiFi connection");
  }
}

void fetchPumpStatus() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    WiFiClient client;

    http.begin(client, pumpStatusUrl);
    http.addHeader("Authorization", "Bearer " + bearerToken);

    int httpResponseCode = http.GET();

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Pump status response: " + response);

      bool newPumpStatus = false;
      if (response.indexOf("\"pumpStatus\":true") != -1) {
        newPumpStatus = true;
      }

      if (newPumpStatus != pumpStatus) {
        pumpStatus = newPumpStatus;
        arduinoSerial.println(pumpStatus ? "PUMP_ON" : "PUMP_OFF");
        Serial.println("Sent to Arduino: " + String(pumpStatus ? "PUMP_ON" : "PUMP_OFF"));
      }
    } else {
      Serial.println("Error in fetching pump status: " + String(httpResponseCode));
    }

    http.end();
  } else {
    Serial.println("Error: No WiFi connection");
  }
}
