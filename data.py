import socket
import csv
from datetime import datetime

# UDP server settings
UDP_IP = "0.0.0.0"  # Replace with your IP if necessary
UDP_PORT = 12345  # Replace with your port if necessary

# Set up UDP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((UDP_IP, UDP_PORT))

# Define the CSV file path
csv_file_path = "sensor_data.csv"


# Create or load the CSV file
def create_or_load_csv():
    expected_header = ["Timestamp", "X1", "Y1", "Z1", "f1", "X2", "Y2", "Z2", "f2", "Pressure", "Temperature",
                       "Humidity", "Altitude"]

    try:
        with open(csv_file_path, mode='r', newline='') as file:
            reader = csv.reader(file)
            header = next(reader)
            # Check if header matches expected format
            if header != expected_header:
                print(f"Warning: CSV file header does not match expected format.")
                print(f"Expected header: {expected_header}")
                print(f"Current header: {header}")
                # Update the header to match the expected format
                with open(csv_file_path, mode='w', newline='') as file:
                    writer = csv.writer(file)
                    writer.writerow(expected_header)
                print("CSV file header has been updated.")
    except FileNotFoundError:
        # Create the CSV file with the correct header
        with open(csv_file_path, mode='w', newline='') as file:
            writer = csv.writer(file)
            writer.writerow(expected_header)
        print("CSV file created with the correct header.")


create_or_load_csv()

print("Listening for UDP packets...")

while True:
    # Receive UDP data
    data, _ = sock.recvfrom(1024)
    data_str = data.decode("utf-8")

    # Split the data and ensure the correct number of fields
    data_list = data_str.split(',')
    if len(data_list) != 12:
        print("Received data format is incorrect.")
        continue

    # Prepare data to be written to the CSV file
    timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S.%f")[:-3]
    new_entry = [timestamp] + data_list

    # Append the new data to the CSV file
    with open(csv_file_path, mode='a', newline='') as file:
        writer = csv.writer(file)
        writer.writerow(new_entry)

    print(f"Data received and stored: {new_entry}")
