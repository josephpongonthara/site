//BLUETOOTH TRANSMISSION TEST + READING DATA

#include <ArduinoBLE.h>
#include <Arduino.h>
#include "cmsis_gcc.h"

const char* HM10address = "18:93:d7:49:2b:72";

void setup() {
  Serial.begin(115200);
  while (!Serial);

  if (!BLE.begin()) {
    Serial.println("Starting Bluetooth® Low Energy module failed!");
    BLE.end();
    delay(100);
    NVIC_SystemReset();
    while (1);
  }

  Serial.println("Scanning for HM-10...");
  BLE.scan(); 
  Serial1.begin(115200);  
  delay(100);
}

void loop() {
  BLEDevice peripheral = BLE.available();

  if (peripheral) {
    Serial.println("Discovered a peripheral");
    Serial.print("Address: ");
    Serial.println(peripheral.address());
    Serial.print("RSSI: ");
    Serial.println(peripheral.rssi());

    if (peripheral.address() == HM10address) {
      Serial.println("HM-10 found!");
      BLE.stopScan();
      if (peripheral.connect()) {
        delay(500);

        Serial.println("Discovering services...");
        if (!peripheral.discoverAttributes()) {
          Serial.println("Failed to discover attributes. Resetting...");
          delay(1000);
          NVIC_SystemReset();
        } else {
          Serial.println("Services discovered");
        }

        BLEService service = peripheral.service("FFE0"); // HM-10 Service UUID
        if (service) {
          Serial.println("Service FFE0 found!");
          BLECharacteristic characteristic = service.characteristic("FFE1"); // HM-10 TX Characteristic
          if (characteristic) {
            Serial.println("Characteristic FFE1 found!");
            delay(1000);

            uint8_t buffer[4];
            delay(100);
            while (peripheral.connected()) {
              if (characteristic.readValue(buffer, sizeof(buffer))) {
                uint8_t header = buffer[0];
                uint8_t highByte = buffer[1];
                uint8_t lowByte = buffer[2];
                uint8_t checksum = buffer[3];

                uint16_t distance = (highByte << 8) | lowByte; 

                Serial.print("Raw Data: ");
                Serial.print(header, HEX);
                Serial.print(" ");
                Serial.print(highByte, HEX);
                Serial.print(" ");
                Serial.print(lowByte, HEX);
                Serial.print(" ");
                Serial.print(checksum, HEX);
                Serial.println();
                Serial.print("Distance: ");
                Serial.print(distance);
                Serial.println(" mm");

                delay(200);
              }
            }
          } else {
            Serial.println("Characteristic not found.");
          }
        } else {
          Serial.println("Service not found.");
        }

        // Disconnect and restart scanning after handling the peripheral
        peripheral.disconnect();
        Serial.println("Disconnected. Restarting scan.");
        BLE.scan();
      } else {
        Serial.println("Failed to connect to peripheral.");
      }
    }
  }
}
