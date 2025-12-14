#include <Wire.h>
#include "DHT.h"
#include "TM1637Display.h"
#include "rgb_lcd.h"
#include "Seeed_BMP280.h"

// Pin Definitions
const int DHTPIN = 2;
const int DHTTYPE = DHT11; 
const int CLK = 5;
const int DIO = 6;
const int LEDPIN = 13;

// Segment definitions for letters C, h, H
const uint8_t LETTER_C[] = { SEG_A | SEG_E | SEG_F | SEG_D };
const uint8_t LETTER_h[] = { SEG_F | SEG_E | SEG_G | SEG_C };
const uint8_t LETTER_H[] = { SEG_F | SEG_E | SEG_G | SEG_B | SEG_C };

DHT dht(DHTPIN, DHTTYPE);
TM1637Display display(CLK, DIO);
rgb_lcd lcd;
BMP280 bmp280;

// Global color variables
int colorR = 255;
int colorG = 255;
int colorB = 255;

void setup() {
  Serial.begin(9600);
  
  dht.begin();
  display.setBrightness(0x0f);
  lcd.begin(16, 2);
  
  // Important: Initialize Barometer
  if (!bmp280.init()) {
    Serial.println("BMP280 Init Error!");
  }

  pinMode(LEDPIN, OUTPUT);
}

void loop() {
  // Read Sensors
  float hum = dht.readHumidity();      
  float temp = dht.readTemperature();
  float pressure = bmp280.getPressure(); // Pa
  float altitude = bmp280.calcAltitude(pressure);
  int hour = 15; // Static time simulation

  // --- 4-Digit Display Sequence ---
  
  // 1. Temperature
  display.clear();
  display.showNumberDec((int)temp, false, 2, 0);
  display.setSegments(LETTER_C, 1, 3);
  delay(2000);

  // 2. Humidity
  display.clear();
  display.showNumberDec((int)hum, false, 2, 0);
  display.setSegments(LETTER_H, 1, 3);
  delay(2000);

  // 3. Time
  display.clear();
  display.showNumberDec(hour, false, 2, 0);
  display.setSegments(LETTER_h, 1, 3);
  delay(2000);

  // --- Weather Logic & Colors ---
  String weatherState;

  [cite_start]// Logic based on Temp, Humidity, and Pressure [cite: 94]
  if (temp > 30 || hum > 70) {
    weatherState = "Hot & Humid";
    colorR = 255; colorG = 0; colorB = 0; // Red
  } 
  else if (temp < 10 || pressure < 99000) {
    weatherState = "Cold/Rain";
    colorR = 0; colorG = 0; colorB = 255; // Blue
  } 
  else {
    weatherState = "Normal";
    colorR = 0; colorG = 255; colorB = 0; // Green
  }

  // --- LCD Update ---
  lcd.clear();
  lcd.setRGB(colorR, colorG, colorB);
  lcd.setCursor(0, 0);
  lcd.print("Weather:");
  lcd.setCursor(9, 0);
  lcd.print(weatherState);

  // Page 1: Temp & Hum
  lcd.setCursor(0, 1);
  lcd.print("T:"); lcd.print((int)temp); lcd.print("C ");
  lcd.print("H:"); lcd.print((int)hum); lcd.print("%");
  delay(2000);

  [cite_start]// Page 2: Pressure & Alt [cite: 95]
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Pres: "); lcd.print((int)(pressure/100)); lcd.print(" hPa");
  lcd.setCursor(0, 1);
  lcd.print("Alt: "); lcd.print((int)altitude); lcd.print(" m");
  delay(2000);

  // --- LED Blinking Logic ---
  int blinkDelay = 0;
  
  [cite_start]// Set blink speed based on severity [cite: 96]
  if (weatherState == "Hot & Humid") {
    blinkDelay = 200; // Fast
  } else if (weatherState == "Cold/Rain") {
    blinkDelay = 500; // Medium
  }

  if (blinkDelay > 0) {
    digitalWrite(LEDPIN, HIGH);
    delay(blinkDelay);
    digitalWrite(LEDPIN, LOW);
    delay(blinkDelay);
  } else {
    digitalWrite(LEDPIN, LOW);
    delay(1000);
  }
}
