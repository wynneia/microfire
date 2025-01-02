import serial
import csv
import os
from datetime import datetime

csv_file = 'smoke_data.csv'

ser = serial.Serial('COM4', 9600, timeout=1) 

file_exists = os.path.isfile(csv_file)

try:
    with open(csv_file, 'a', newline='') as csvfile:
        csvwriter = csv.writer(csvfile)

        if not file_exists:
            csvwriter.writerow(["Smoke_Level", "Timestamp"])

        print("Starting data acquisition... Press Ctrl+C to stop.")

        while True:
            line = ser.readline().decode('utf-8').strip()
            if line:
                data = line.split(',')
                smoke_level = data[0]
                arduino_time = int(data[1])

                timestamp = datetime.now().strftime('%Y-%m-%d %H:%M:%S')

                csvwriter.writerow([smoke_level, timestamp])
                print(f"Saved: {smoke_level}, {timestamp}")

except KeyboardInterrupt:
    print("Data acquisition stopped by user.")

except Exception as e:
    print(f"An error occurred: {e}")

finally:
    ser.close()
    print("Serial connection closed.")
