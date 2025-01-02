# Fire Detection & Control System

An IoT-based fire detection and control system using Arduino Uno, ESP8266, and web API integration.

## Components

- Arduino Uno
- ESP8266 NodeMCU
- Flame Sensor
- Smoke Sensor (MQ-2)
- Water Pump
- Buzzer
- SoftwareSerial for communication

## Pin Configuration

### Arduino
- Flame Sensor: A0
- Smoke Sensor: A1
- Water Pump: D10
- Buzzer: D4
- ESP8266 Communication: D2 (RX), D3 (TX)1

### ESP8266
- Arduino Communication: D2 (RX), D1 (TX)

## Features

- Real-time flame and smoke detection
- Automated water pump activation
- Manual pump control via web API
- Audible alerts via buzzer
- Data transmission to web API
- WiFi connectivity

## Operation

### Automatic Mode
1. Flame detection triggers pump activation
2. Pump deactivates after 5 seconds of no flame
3. Smoke levels >300 trigger buzzer alerts

### Manual Mode
- Remote pump control via web API
- Overrides automatic operation
- Resets to automatic when pump and buzzer are off

## Network Configuration

```cpp
API Endpoints:
- Sensor Data: http://mikro.a2s.ltd/api/sensor-data
- Pump Status: http://mikro.a2s.ltd/api/pump/status
- Pump Control: http://mikro.a2s.ltd/api/pump/control
```

## Data Format

### Arduino to ESP8266
`flameValue,smokeValue,pumpState`

### ESP8266 to Server
```json
{
  "flameValue": int,
  "smokeValue": int
}
```

## Alert Thresholds
- Fire: flame sensor reading = 0
- Smoke: sensor reading > 300

## Communication Intervals
- Arduino Reading: 1s
- Web Data Post: 3s
- Pump Status Check: 5s