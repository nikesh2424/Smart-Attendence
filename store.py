import serial
import csv
import os
from datetime import datetime

ser = serial.Serial('COM4', 9600, timeout=1) # enter your own port and address
csv_filename = "attendance_records.csv"

if not os.path.exists(csv_filename):
    with open(csv_filename, mode="w", newline="") as file:
        writer = csv.writer(file)
        writer.writerow(["Date", "Name", "Attendance Count"])

print("Listening for RFID scans...")

attendance_data = {}

with open(csv_filename, mode="r", newline="") as file:
    reader = csv.reader(file)
    next(reader, None)
    for row in reader:
        if len(row) == 3:
            date, name, count = row
            if date not in attendance_data:
                attendance_data[date] = {}
            attendance_data[date][name] = int(count)

while True:
    try:
        data = ser.readline().decode('utf-8').strip()

        if data:
            print(f"Received: {data}")

            if data != "READY":
                name = data
                today_date = datetime.now().strftime("%Y-%m-%d")

                if today_date not in attendance_data:
                    attendance_data[today_date] = {}

                new_count = attendance_data[today_date].get(name, 0) + 1

                try:
                    with open(csv_filename, mode="w", newline="") as file:
                        writer = csv.writer(file)
                        writer.writerow(["Date", "Name", "Attendance Count"])

                        attendance_data[today_date][name] = new_count

                        for date, records in attendance_data.items():
                            for person, count in records.items():
                                writer.writerow([date, person, count])

                    print(f"✅ Logged: {name} - Count: {new_count}")

                except Exception as file_error:
                    print(f"❌ File Write Error: {file_error}")

    except Exception as e:
        print(f"❌ Serial Read Error: {e}")
