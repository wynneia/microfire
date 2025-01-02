# Smoke Data Logger

Python script for logging smoke sensor data from Arduino to CSV file.

## Requirements

- Python 3.x
- pyserial library
- Arduino connected to PORT

## Setup

```bash
pip install pyserial
```

## Features

- Real-time data logging from serial port
- CSV file storage with timestamp
- Automatic file creation/appending

## CSV Format

| Column | Description |
|--------|-------------|
| Smoke_Level | Timestamp |
| Timestamp | YYYY-MM-DD HH:MM:SS |

## Usage

1. Connect Arduino to PORT
2. Run script:
```bash
python daq.py
```
3. Stop logging with Ctrl+C