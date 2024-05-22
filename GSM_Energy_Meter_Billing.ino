#include <SoftwareSerial.h>
#include <LiquidCrystal.h>

// Pin definitions
const int currentSensorPin = A0;
const int voltageSensorPin = A1;
const int relayPin = 8;
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
SoftwareSerial gsm(9, 10); // RX, TX

float energyConsumed = 0;
float voltage = 0;
float current = 0;
float power = 0;
float tariffRate = 0.1; // Example tariff rate per kWh

void setup() {
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);
  lcd.begin(16, 2);
  gsm.begin(9600);
  Serial.begin(9600);
  lcd.print("Energy Meter");
  delay(2000);
  lcd.clear();
}

void loop() {
  voltage = readVoltage();
  current = readCurrent();
  power = voltage * current;
  energyConsumed += (power / 3600); // Convert to kWh
  
  lcd.setCursor(0, 0);
  lcd.print("Power: ");
  lcd.print(power);
  lcd.print(" W");
  
  lcd.setCursor(0, 1);
  lcd.print("Energy: ");
  lcd.print(energyConsumed);
  lcd.print(" kWh");

  delay(1000);

  // Check if it's time to send an SMS
  if (shouldSendSMS()) {
    sendBillingInfo();
  }
}

float readVoltage() {
  int sensorValue = analogRead(voltageSensorPin);
  // Convert sensor value to voltage
  return sensorValue * (5.0 / 1023.0) * 11; // Assuming a voltage divider of 10:1
}

float readCurrent() {
  int sensorValue = analogRead(currentSensorPin);
  // Convert sensor value to current
  return (sensorValue - 512) * (5.0 / 1023.0) / 0.066; // For ACS712-30A
}

bool shouldSendSMS() {
  // Logic to determine if an SMS should be sent (e.g., at the end of the billing period)
  return false; // Placeholder
}

void sendBillingInfo() {
  float billAmount = energyConsumed * tariffRate;
  gsm.print("AT+CMGF=1\r");
  delay(100);
  gsm.print("AT+CMGS=\"+1234567890\"\r"); // Replace with actual phone number
  delay(100);
  gsm.print("Energy Consumed: ");
  gsm.print(energyConsumed);
  gsm.print(" kWh\nBill Amount: $");
  gsm.print(billAmount);
  gsm.write(26); // Ctrl+Z to send the SMS
  delay(1000);
}
