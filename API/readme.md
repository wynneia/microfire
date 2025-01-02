# Fire Detection System API

A REST API for a fire detection system that monitors flame and smoke sensors, controls water pumps, and manages user authentication.

## Features

- Real-time sensor data monitoring (flame and smoke)
- Automated alert system for fire and smoke detection
- Water pump control system
- JWT-based authentication

## Prerequisites

- Node.js
- MongoDB
- npm/yarn

## Environment Variables

Create a `.env` file with:

```
MONGO_URI=your_mongodb_connection_string
ACCESS_TOKEN_SECRET=your_jwt_secret
```

## Installation

```bash
npm install
npm start
```

## API Endpoints

### Authentication

#### Register User
- **POST** `/api/register`
- **Body**: `{ "username": string, "password": string }`
- **Response**: 201 Created

#### Login
- **POST** `/api/login`
- **Body**: `{ "username": string, "password": string }`
- **Response**: JWT access token

### Sensor Data

#### Submit Sensor Data
- **POST** `/api/sensor-data`
- **Auth**: Required
- **Body**: `{ "flameValue": number, "smokeValue": number }`
- **Response**: Created sensor data entry

#### Get Latest Sensor Data
- **GET** `/api/sensor-data/latest`
- **Auth**: Required
- **Response**: Latest sensor readings

### Pump Control

#### Get Pump Status
- **GET** `/api/pump/status`
- **Auth**: Required
- **Response**: Current pump status

#### Control Pump
- **POST** `/api/pump/control`
- **Auth**: Required
- **Body**: `{ "status": boolean }`
- **Response**: Updated pump status

### Alerts

#### Get Active Alerts
- **GET** `/api/alerts`
- **Auth**: Required
- **Response**: Array of active alerts

## Alert Thresholds

- Fire Alert: Triggered when flame value < 1
- Smoke Alert: Triggered when smoke value > 300

## Authentication

All protected endpoints require a valid JWT token in the Authorization header:
```
Authorization: Bearer <token>
```

## Error Handling

The API returns appropriate HTTP status codes:
- 200: Success
- 201: Created
- 400: Bad Request
- 401: Unauthorized
- 403: Forbidden
- 404: Not Found
- 500: Server Error