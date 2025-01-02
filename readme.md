# IoT Fire Detection System

Complete fire detection and monitoring system with hardware sensors, microcontrollers, API backend, and data logging capabilities.

## System Architecture

```
Arduino Uno (Sensors) <-> ESP8266 (WiFi) <-> Node.js API <-> Python Logger <-> Mobile Application
```

## Components

### Hardware
- Arduino Uno
- ESP8266 NodeMCU
- Flame Sensor (A0)
- Smoke Sensor MQ-2 (A1)
- Water Pump (D10)
- Buzzer (D4)

### Software
- Node.js REST API
- MongoDB Database
- Python Data Logger
- Arduino/ESP8266 Firmware

## Setup Instructions

### 1. API Server
```bash
cd api
npm install
# Configure .env with:
# MONGO_URI=your_mongodb_url
# ACCESS_TOKEN_SECRET=your_secret
npm start
```

### 2. Arduino Setup
1. Connect sensors according to pin configuration
2. Upload Arduino code via IDE
3. Connect to ESP8266 via pins D2/D3

### 3. ESP8266 Setup
1. Update WiFi credentials and API endpoints
2. Upload NodeMCU code via IDE
3. Connect to Arduino via pins D1/D2

### 4. Data Logger
```bash
pip install pyserial
python daq.py
```

## Features

- Real-time fire and smoke detection
- Automated water pump control
- Web API for remote monitoring
- JWT authentication
- CSV data logging
- Alert system for fire/smoke

## API Endpoints

- POST `/api/register` - User registration
- POST `/api/login` - Authentication
- POST `/api/sensor-data` - Submit readings
- GET `/api/sensor-data/latest` - Latest data
- GET `/api/pump/status` - Pump state
- POST `/api/pump/control` - Control pump
- GET `/api/alerts` - System alerts

## Data Formats

### Sensor Data
```json
{
  "flameValue": number,
  "smokeValue": number
}
```

### CSV Log
```
Smoke_Level,Timestamp
```

## Alert Thresholds
- Fire Alert: flame value < 1
- Smoke Alert: value > 300

## Security
- JWT-based API authentication
- Encrypted passwords
- Protected endpoints