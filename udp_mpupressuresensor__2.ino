#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <MPU6050.h>
#include <WiFi.h>
#include <WiFiUdp.h>

// I2C Addresses
#define MPU1_ADDR 0x68
#define MPU2_ADDR 0x69
#define BME280_ADDR 0x76 // Change to 0x77 if necessary

MPU6050 mpu1(MPU1_ADDR);
MPU6050 mpu2(MPU2_ADDR);
Adafruit_BME280 bme;

const char* ssid = "Unio Labs";
const char* password = "India123..";

WiFiUDP udp;
const char* udpAddress = "192.168.0.41"; // IP address of the receiver
unsigned int udpPort = 12345;

void setup() {
  Serial.begin(115200);
  
  // Initialize I2C communication
  Wire.begin();

  // Initialize MPU6050 sensors
  mpu1.initialize();
  mpu2.initialize();

  if (!mpu1.testConnection()) {
    Serial.println("MPU6050 #1 not connected");
    while (1);
  }
  if (!mpu2.testConnection()) {
    Serial.println("MPU6050 #2 not connected");
    while (1);
  }

  // Initialize BME280 sensor
  if (!bme.begin(BME280_ADDR)) {
    Serial.println("BME280 not found");
    while (1);
  }

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to Wi-Fi");

  // Start UDP
  udp.begin(udpPort);
  Serial.printf("UDP client started, sending to %s:%d\n", udpAddress, udpPort);
}

void loop() {
  // Variables for MPU6050 readings
  int16_t accel1X, accel1Y, accel1Z;
  int16_t accel2X, accel2Y, accel2Z;
  float accel1X_g, accel1Y_g, accel1Z_g;
  float accel2X_g, accel2Y_g, accel2Z_g;
  
  // Reading data from MPU6050 #1
  mpu1.getAcceleration(&accel1X, &accel1Y, &accel1Z);
  accel1X_g = accel1X / 16384.0; // Convert to g
  accel1Y_g = accel1Y / 16384.0; // Convert to g
  accel1Z_g = accel1Z / 16384.0; // Convert to g
  
  // Reading data from MPU6050 #2
  mpu2.getAcceleration(&accel2X, &accel2Y, &accel2Z);
  accel2X_g = accel2X / 16384.0; // Convert to g
  accel2Y_g = accel2Y / 16384.0; // Convert to g
  accel2Z_g = accel2Z / 16384.0; // Convert to g

  // Calculate combined acceleration for MPU1 and MPU2
  float combinedAccel1 = sqrt(pow(accel1X_g, 2) + pow(accel1Y_g, 2) + pow(accel1Z_g, 2));
  float combinedAccel2 = sqrt(pow(accel2X_g, 2) + pow(accel2Y_g, 2) + pow(accel2Z_g, 2));

  // Calculate forces based on F = m * a (assuming mass = 1kg for simplicity)
  float force1 = combinedAccel1 * 9.81; // Combined Force for MPU1
  float force2 = combinedAccel2 * 9.81; // Combined Force for MPU2

  // Reading data from BME280
  float temperature = bme.readTemperature();
  float humidity = bme.readHumidity();
  float pressure = bme.readPressure() / 100.0F;  // Pressure in hPa

  // Calculate altitude based on pressure
  float altitude = bme.readAltitude(1013.25); // Sea level pressure in hPa

  // Create a string to hold the data
  String data=" ";
data += String(accel1X_g) + "," + String(accel1Y_g) + "," + String(accel1Z_g) + "," + String(force1) + ",";
data += String(accel2X_g) + "," + String(accel2Y_g) + "," + String(accel2Z_g) + "," + String(force2) + ",";
data += String(temperature) + "," + String(humidity) + "," + String(pressure) + "," + String(altitude);



  // Send the data via UDP
  udp.beginPacket(udpAddress, udpPort);
  udp.print(data);
  udp.endPacket();

  // Print the data to Serial Monitor
  Serial.println(data);

  // Delay before next reading
  delay(100);
}
