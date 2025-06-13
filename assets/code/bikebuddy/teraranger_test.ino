//LEARNING TO READ THE SENSOR DATA CORRECTLY

#include <Arduino.h>

void setup() {
  Serial.begin(115200);  
  Serial1.begin(115200); 
  Serial.println("UART communication initialized.");
}

void loop() {
  uint8_t command[] = {0x00, 0x11, 0x02, 0x4C};
  Serial1.write(command, sizeof(command));
  delay(100);

  while (Serial1.available() >= 4) {
    uint8_t header = Serial1.read();

    // Only proceed if header is 0x54
    if (header != 0x54) {
      // Optional debug: Serial.println("Syncing... Discarding invalid byte.");
      continue;
    }

    // Read the next 3 bytes
    uint8_t highByte = Serial1.read();
    uint8_t lowByte = Serial1.read();
    uint8_t checksum = Serial1.read();

    // Verify checksum
    uint8_t calculatedChecksum = header ^ highByte ^ lowByte;
    if (checksum != calculatedChecksum) {
      Serial.println("Checksum mismatch! Discarding packet.");
      uint8_t discard = Serial1.read(); // discard the remaining bytes
      discard = Serial1.read();
      discard = Serial1.read();
    }

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

    /*
    if (distance == 0x0001) {
      Serial.println("Error: Unable to measure distance");
    } else if (distance == 0x0000) {
      Serial.println("Target too close");
    } else if (distance == 0xFFFF) {
      Serial.println("Target too far");
    } else {
      Serial.print("Distance: ");
      Serial.print(distance);
      Serial.println(" mm");
    }
    */
    delay(200);
  }
}
