//EVERYTHING INTEGRATED

#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <Arduino.h>
#include <ArduinoBLE.h>
#include <SPI.h>
#include <Fonts/FreeMono9pt7b.h> 
#include "cmsis_gcc.h"
#include "animations.h"

#define TFT_CS    10
#define TFT_DC    9
#define TFT_RST   8
#define FRAME_WIDTH (64)
#define FRAME_HEIGHT (64)
//SCREEN 1 (OPENING)
#define CENTER_X ((240 - FRAME_WIDTH) / 2)   // 88
#define CENTER_Y (90)  // 128
#define TEXT_Y_POSITION (175)
//SCREEN 2 (LOADING CHECKS)
#define X_POSITION (0)
#define TEXT_X_POSITION (X_POSITION + FRAME_WIDTH + 10)
#define X_HOME (25) 
#define X_BLINDSPOT (40) 
#define X_DISTANCE (75)
#define Y_POSITION_1 (25)
#define Y_POSITION_2 (125)
#define Y_POSITION_3 (225) 
//SCREEN 3 (WARNING SCREEN)
#define Y_POSITION_4 (100)
#define Y_POSITION_5 (250)
#define Y_POSITION_6 (290)

Adafruit_ILI9341 display = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);
const char* HM10address = "18:93:d7:49:2b:72";

void setup() {
  //OPENING SCREEN
  display.begin();
  display.setRotation(2);
  display.fillScreen(ILI9341_WHITE);
  display.setTextColor(ILI9341_BLACK);
  display.setTextSize(1);
  display.setFont(&FreeMono9pt7b);
  Serial.begin(115200);
  while (!Serial);
  if (!BLE.begin()) {
    display.drawBitmap(CENTER_X, CENTER_Y, openingImage, FRAME_WIDTH, FRAME_HEIGHT, ILI9341_BLACK);
    display.setCursor(X_HOME, TEXT_Y_POSITION); 
    display.print("Starting Bluetooth");
    Serial.println("Starting Bluetooth® Low Energy module failed!");
    delay(1000);
    NVIC_SystemReset();
    while (1);
  }
  BLE.scan(); 
  Serial.println("BLE started successfully");
  Serial1.begin(115200);  
  delay(100); 
}

void loop() {
  //BLE STARTS
  display.fillScreen(ILI9341_WHITE);
  BLEDevice peripheral = BLE.available();

  //DISPLAYING LOADING SCREEN
    display.drawBitmap(X_POSITION, Y_POSITION_1, loadingImage, FRAME_WIDTH, FRAME_HEIGHT, ILI9341_BLACK);
    display.drawBitmap(X_POSITION, Y_POSITION_2, loadingImage, FRAME_WIDTH, FRAME_HEIGHT, ILI9341_BLACK);
    display.drawBitmap(X_POSITION, Y_POSITION_3, loadingImage, FRAME_WIDTH, FRAME_HEIGHT, ILI9341_BLACK);
    //text prompts for loading screen
    display.setCursor(TEXT_X_POSITION, Y_POSITION_1 + 30); display.print("Seeking HM-10");
    display.setCursor(TEXT_X_POSITION, Y_POSITION_2 + 30); display.print("Data Check");
    display.setCursor(TEXT_X_POSITION, Y_POSITION_3 + 30); display.print("Failure/Success");

  //SECOND SCREEN AND CONDITIONS
  if (peripheral){
    Serial.println("Discovered a peripheral");
    Serial.print("Address: ");
    Serial.println(peripheral.address());
    Serial.print("RSSI: ");
    Serial.println(peripheral.rssi());
    
    //PERFORMING CHECKS
    if (peripheral.address() == HM10address){
      Serial.println("HM-10 found!");
      BLE.stopScan(); peripheral.connect(); delay(500);
      //CLEARING SCREEN & TEXT AREA
      display.fillRect(X_POSITION, Y_POSITION_1, FRAME_WIDTH, FRAME_HEIGHT, ILI9341_WHITE);
      display.fillRect(TEXT_X_POSITION, Y_POSITION_1, 240, 75, ILI9341_WHITE);
      delay(250);
      //DISPLAYING FIRST CHECK
      display.drawBitmap(X_POSITION, Y_POSITION_1, successImage, FRAME_WIDTH, FRAME_HEIGHT, ILI9341_BLACK);
      display.setCursor(TEXT_X_POSITION, Y_POSITION_1 + 30); display.print("HM-10 Found");
      Serial.println("Discovering services...");

      //CHECK 2
      if(peripheral.discoverAttributes() == 0){
        //CLEARING SCREEN & TEXT AREA
        display.fillScreen(ILI9341_WHITE);
        delay(250);
        //DISPLAYING FAILURE MESSAGES
        display.drawBitmap(X_POSITION, Y_POSITION_2, failedImage, FRAME_WIDTH, FRAME_HEIGHT, ILI9341_BLACK);
        display.setCursor(TEXT_X_POSITION, Y_POSITION_2 + 30); display.print("Data Absent");
        display.drawBitmap(X_POSITION, Y_POSITION_3, failedImage, FRAME_WIDTH, FRAME_HEIGHT, ILI9341_BLACK);
        display.setCursor(TEXT_X_POSITION, Y_POSITION_3 + 30); display.print("Failed Restart");
        Serial.println("Failed to discover attributes. Resetting...");
        delay(500);
        NVIC_SystemReset();
      }
      else{
        //CLEARING SCREEN & TEXT AREA
        display.fillScreen(ILI9341_WHITE);
        delay(250);
        //DISPLAYING SUCCESS MESSAGES
        display.drawBitmap(X_POSITION, Y_POSITION_2, successImage, FRAME_WIDTH, FRAME_HEIGHT, ILI9341_BLACK);
        display.setCursor(TEXT_X_POSITION, Y_POSITION_2 + 30); display.print("Data Found");
        display.drawBitmap(X_POSITION, Y_POSITION_3, successImage, FRAME_WIDTH, FRAME_HEIGHT, ILI9341_BLACK);
        display.setCursor(TEXT_X_POSITION, Y_POSITION_3 + 30); display.print("Success!");
        delay(500);
      }
      //CLEAR FULL SCREEN
      display.fillScreen(ILI9341_WHITE);

      //NOW USING DATA
      BLEService service = peripheral.service("FFE0");
      if (service){
        BLECharacteristic characteristic = service.characteristic("FFE1");
        if (characteristic){
          uint8_t command[] = {0x00, 0x11, 0x02, 0x4C};
          characteristic.writeValue(command, sizeof(command));
          delay(100);

          //reading data
          uint8_t buffer[4];
          while(true){
            if (characteristic.readValue(buffer, sizeof(buffer))){
              uint8_t header = buffer[0];
              uint8_t highByte = buffer[1];
              uint8_t lowByte = buffer[2];
              uint8_t checksum = buffer[3];

              if (checksum == 0x54){
                continue;
              }
              else{
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
                if (distance >= 0 && distance <= 1000){
                  display.fillScreen(ILI9341_WHITE);
                  display.drawBitmap(CENTER_X, Y_POSITION_4, warningImage, FRAME_WIDTH, FRAME_HEIGHT, ILI9341_BLACK);
                  display.setCursor(X_BLINDSPOT, Y_POSITION_5); display.print("CHECK  BEHIND");
                }
                */                                                               
                if (distance>= 0 && distance <1000){
                  display.fillScreen(ILI9341_WHITE);
                  display.setTextSize(3);
                  display.drawBitmap(CENTER_X, Y_POSITION_4, warningImage, FRAME_WIDTH, FRAME_HEIGHT, ILI9341_BLACK);
                  display.setCursor(X_BLINDSPOT, Y_POSITION_5); display.print("CHECK");
                  display.setCursor(26, Y_POSITION_6); display.print("BEHIND");
                }
                if (distance>= 1000 && distance <=5000){
                  display.fillScreen(ILI9341_WHITE);
                  display.setTextSize(3);
                  delay(250);
                  display.setCursor(X_DISTANCE, 128); display.print("5m");
                }
                if (distance>= 5000 && distance <=10000){
                  display.fillScreen(ILI9341_WHITE);
                  display.setTextSize(3);
                  delay(250);
                  display.setCursor(X_DISTANCE, 128); display.print("10m");
                }
                if (distance>= 10000 && distance <=15000){
                  display.fillScreen(ILI9341_WHITE);
                  display.setTextSize(3);
                  delay(250);
                  display.setCursor(X_DISTANCE, 128); display.print("15m");
                }
                if (distance>= 15000 && distance <=20000){
                  display.fillScreen(ILI9341_WHITE);
                  display.setTextSize(3);
                  delay(250);
                  display.setCursor(X_DISTANCE, 128); display.print("20m");
                }
                if (distance>= 20000 && distance <=25000){
                  display.fillScreen(ILI9341_WHITE);
                  display.setTextSize(3);
                  delay(250);
                  display.setCursor(X_DISTANCE, 128); display.print("25m");
                }
                if (distance>= 25000 && distance <=30000){
                  display.fillScreen(ILI9341_WHITE);
                  display.setTextSize(3);
                  delay(250);
                  display.setCursor(X_DISTANCE, 128); display.print("30m");
                }
                if (distance>= 30000 && distance <=35000){
                  display.fillScreen(ILI9341_WHITE);
                  display.setTextSize(3);
                  delay(250);
                  display.setCursor(X_DISTANCE, 128); display.print("35m");
                }
                if (distance>= 35000 && distance <=40000){
                  display.fillScreen(ILI9341_WHITE);
                  display.setTextSize(3);
                  delay(250);
                  display.setCursor(X_DISTANCE, 128); display.print("40m");
                }
                if (distance>= 40000 && distance <=45000){
                  display.fillScreen(ILI9341_WHITE);
                  display.setTextSize(3);
                  delay(250);
                  display.setCursor(X_DISTANCE, 128); display.print("45m");
                }
                if (distance>= 45000 && distance <=50000){
                  display.fillScreen(ILI9341_WHITE);
                  display.setTextSize(3);
                  delay(250);
                  display.setCursor(X_DISTANCE, 128); display.print("50m");
                }
                if (distance>= 50000 && distance <=55000){
                  display.fillScreen(ILI9341_WHITE);
                  display.setTextSize(3);
                  delay(250);
                  display.setCursor(X_DISTANCE, 128); display.print("55m");
                }
                if (distance>= 55000 && distance <=60000){
                  display.fillScreen(ILI9341_WHITE);
                  display.setTextSize(3);
                  delay(250);
                  display.setCursor(X_DISTANCE, 128); display.print("60m");
                }
              }
            }
          }
        }
      }
    }
  }
}
