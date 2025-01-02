#include <SoftwareSerial.h>

#define FLAME_SENSOR A0
#define SMOKE_SENSOR A1
#define WATER_PUMP 10
#define BUZZER 4

SoftwareSerial espSerial(2, 3);

bool pumpManuallyControlled = false;
bool pumpState = false;
unsigned long fireDetectedTime = 0;

void setup() {
  Serial.begin(9600);
  espSerial.begin(9600);
  
  pinMode(FLAME_SENSOR, INPUT);
  pinMode(SMOKE_SENSOR, INPUT);
  pinMode(WATER_PUMP, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  
  digitalWrite(WATER_PUMP, LOW);
  digitalWrite(BUZZER, LOW);
  
  Serial.println("Arduino Uno is ready!");
}

void beepBuzzer(int times, int duration) {
  for (int i = 0; i < times; i++) {
    digitalWrite(BUZZER, HIGH);
    delay(duration);
    digitalWrite(BUZZER, LOW);
    delay(duration);
  }
}

void loop() {
  int flameValue = digitalRead(FLAME_SENSOR);
  int smokeValue = analogRead(SMOKE_SENSOR);

  String dataString = String(flameValue) + "," + 
                      String(smokeValue) + "," + 
                      (pumpState ? "1" : "0");
  espSerial.println(dataString);

  if (espSerial.available()) {
    String command = espSerial.readStringUntil('\n');
    command.trim();

    Serial.println("Received command: " + command);

    if (command.indexOf("PUMP_ON") >= 0) {
      pumpManuallyControlled = true;
      pumpState = true;
      digitalWrite(WATER_PUMP, HIGH);
      Serial.println("Pump turned ON via command");
    } else if (command.indexOf("PUMP_OFF") >= 0) {
      pumpManuallyControlled = true;
      pumpState = false;
      digitalWrite(WATER_PUMP, LOW);
      Serial.println("Pump turned OFF via command");
    }
  }

  if (!pumpManuallyControlled) {
    if (flameValue == 0) {
      if (!pumpState) {
        pumpState = true;
        digitalWrite(WATER_PUMP, HIGH);
        fireDetectedTime = millis();
        Serial.println("Fire detected! Pump activated.");
        
        beepBuzzer(5, 100);
      }
    } else {
      if (millis() - fireDetectedTime > 5000) {
        if (pumpState) {
          pumpState = false;
          digitalWrite(WATER_PUMP, LOW);
          Serial.println("No fire detected for 5 seconds. Pump turned off.");
        }
      }
    }

    if (smokeValue > 300) {
      beepBuzzer(5, 100); // Beep 5 times in a second for smoke detection
      Serial.println("High smoke levels detected!");
    }
  }

  if (!digitalRead(WATER_PUMP) && !digitalRead(BUZZER)) {
    pumpManuallyControlled = false;
  }

  delay(100);
}
