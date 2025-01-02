#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncHTTPClient.h>

const char* ssid = "lynxAdapt";
const char* password = "janice69";

const char* apiUrl = "http://mikro.a2s.ltd/api/sensor-data";
const char* pumpStatusUrl = "http://mikro.a2s.ltd/api/pump/status";
const String bearerToken = "your_bearer_token";

#define ARDUINO_RX D2
#define ARDUINO_TX D1

AsyncHTTPClient asyncHttpClient;
SoftwareSerial arduinoSerial(ARDUINO_RX, ARDUINO_TX);

int flameValue = 0;
int smokeValue = 0;
bool pumpStatus = false;

unsigned long lastArduinoReadTime = 0;
const unsigned long arduinoReadInterval = 1000;

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
}

void parseSensorData(String data) {
  int firstComma = data.indexOf(',');
  int secondComma = data.indexOf(',', firstComma + 1);

  if (firstComma != -1 && secondComma != -1) {
    flameValue = data.substring(0, firstComma).toInt();
    smokeValue = data.substring(firstComma + 1, secondComma).toInt();
  }

  sendSensorDataAsync(flameValue, smokeValue);
  fetchPumpStatusAsync();
}

void sendSensorDataAsync(int flameValue, int smokeValue) {
  if (WiFi.status() == WL_CONNECTED) {
    String payload = String("{\"flameValue\":") + flameValue + 
                     ",\"smokeValue\":" + smokeValue + "}";

    asyncHttpClient.begin(apiUrl);
    asyncHttpClient.addHeader("Content-Type", "application/json");
    asyncHttpClient.addHeader("Authorization", "Bearer " + bearerToken);

    asyncHttpClient.onRequestComplete([](AsyncHTTPClientResponse* response) {
      if (response->statusCode() > 0) {
        Serial.println("Sensor data POST response: " + response->responseText());
      } else {
        Serial.println("Error in POST request");
      }
    });

    asyncHttpClient.POST(payload);
  } else {
    Serial.println("Error: No WiFi connection");
  }
}

void fetchPumpStatusAsync() {
  if (WiFi.status() == WL_CONNECTED) {
    asyncHttpClient.begin(pumpStatusUrl);
    asyncHttpClient.addHeader("Authorization", "Bearer " + bearerToken);

    asyncHttpClient.onRequestComplete([](AsyncHTTPClientResponse* response) {
      if (response->statusCode() > 0) {
        String responseText = response->responseText();
        Serial.println("Pump status response: " + responseText);

        bool newPumpStatus = responseText.indexOf("\"pumpStatus\":true") != -1;

        if (newPumpStatus != pumpStatus) {
          pumpStatus = newPumpStatus;
          arduinoSerial.println(pumpStatus ? "PUMP_ON" : "PUMP_OFF");
          Serial.println("Sent to Arduino: " + String(pumpStatus ? "PUMP_ON" : "PUMP_OFF"));
        }
      } else {
        Serial.println("Error in fetching pump status");
      }
    });

    asyncHttpClient.GET();
  } else {
    Serial.println("Error: No WiFi connection");
  }
}
