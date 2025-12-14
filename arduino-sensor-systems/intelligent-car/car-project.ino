#include <Wire.h>
#include "rgb_lcd.h"
#include <DHT.h>

rgb_lcd lcd;

// --- PIN DEFINITIONS ---
const int buttonPin = 2;       // Master Button
const int buzzerPin = 3;       // Buzzer (Attention: Pin changed to 3 for Tone compatibility)
const int seatbeltPin = 4;     // Seatbelt Button
const int ledHeadlight = 5;    // Car Headlights
const int motionSensor = 6;    // PIR Motion Sensor
const int ledSecurity = 7;     // Red LED (Security/Motion)
const int dhtPin = 8;          // Temp Sensor
const int trigPin = 9;         // Ultrasonic Trig
const int echoPin = 10;        // Ultrasonic Echo
const int potSensor = A0;      // Potentiometer
const int lightSensor = A1;    // Light Sensor

// --- OBJECTS ---
DHT dht(dhtPin, DHT11);

// --- VARIABLES ---
// Master State
bool isCarStarted = false;
int lastButtonState = 0;

// Seatbelt
bool isSeatbeltOn = false;
int lastSeatbeltState = 0;
unsigned long seatbeltTimer = 0;
bool seatbeltWarningActive = false;

// Light System
int lightThreshold = 400;      // Adjust based on your room
int potThreshold = 900;

// Ultrasonic (Distance)
unsigned long lastDistCheck = 0;
long distance = 0;
unsigned long buzzerTimer = 0;
int buzzerState = 0;

// Motion (PIR)
int motionState = 0;

// LCD Management
unsigned long lcdMessageTimer = 0;
bool isLcdTemporary = false;

void setup() {
  pinMode(buttonPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(seatbeltPin, INPUT);
  pinMode(ledHeadlight, OUTPUT);
  pinMode(motionSensor, INPUT);
  pinMode(ledSecurity, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  lcd.begin(16, 2);
  dht.begin();
  Serial.begin(9600);
  
  // Initial State
  displayEngineOff();
}

void loop() {
  unsigned long currentMillis = millis();

  // 1. MASTER BUTTON (Engine Start/Stop)
  int btnState = digitalRead(buttonPin);
  if (btnState == HIGH && lastButtonState == LOW) {
    isCarStarted = !isCarStarted;
    if (isCarStarted) {
      displayEngineOn();
      seatbeltTimer = currentMillis; // Reset seatbelt timer
    } else {
      displayEngineOff();
    }
    delay(50); // Small debounce
  }
  lastButtonState = btnState;

  // 2. LOGIC WHEN CAR IS ON
  if (isCarStarted) {
    checkSeatbelt(currentMillis);
    checkLights(currentMillis);
    checkDistance(currentMillis);
    checkTemperature(currentMillis);
    
    // Security LED is OFF when driving
    digitalWrite(ledSecurity, LOW);
  } 
  // 3. LOGIC WHEN CAR IS OFF
  else {
    checkMotion(currentMillis);
    digitalWrite(ledHeadlight, LOW);
    noTone(buzzerPin);
  }

  // Clear temporary LCD messages after 5 seconds
  if (isLcdTemporary && (currentMillis - lcdMessageTimer > 5000)) {
    lcd.clear();
    // Restore default background based on state
    if(isCarStarted) lcd.setRGB(0, 255, 0); 
    else lcd.setRGB(255, 0, 0);
    isLcdTemporary = false;
  }
}

// --- FUNCTIONS ---

void displayEngineOn() {
  lcd.setRGB(0, 255, 0); // Green
  lcd.setCursor(0, 0);
  lcd.print("CAR STARTED     ");
  lcdMessageTimer = millis();
  isLcdTemporary = true;
}

void displayEngineOff() {
  lcd.setRGB(255, 0, 0); // Red
  lcd.setCursor(0, 0);
  lcd.print("CAR STOPPED     ");
  lcdMessageTimer = millis();
  isLcdTemporary = true;
}

void checkSeatbelt(unsigned long currentMillis) {
  int sbState = digitalRead(seatbeltPin);
  
  // Toggle Seatbelt
  if (sbState == HIGH && lastSeatbeltState == LOW) {
    isSeatbeltOn = !isSeatbeltOn;
    lcd.clear();
    if (isSeatbeltOn) {
      lcd.print("BELT OK");
      seatbeltWarningActive = false;
      noTone(buzzerPin);
    } else {
      lcd.print("BELT OFF");
      seatbeltTimer = currentMillis; // Reset timer
    }
    lcdMessageTimer = currentMillis;
    isLcdTemporary = true;
    delay(50);
  }
  lastSeatbeltState = sbState;

  // Warning Logic (10 seconds)
  if (!isSeatbeltOn && (currentMillis - seatbeltTimer > 10000)) {
    // Non-blocking buzzer beep (500ms ON, 500ms OFF)
    if (currentMillis - buzzerTimer > 500) {
      buzzerTimer = currentMillis;
      if (buzzerState == 0) {
        tone(buzzerPin, 500);
        buzzerState = 1;
      } else {
        noTone(buzzerPin);
        buzzerState = 0;
      }
    }
  }
}

void checkLights(unsigned long currentMillis) {
  int lightVal = analogRead(lightSensor);
  int potVal = analogRead(potSensor);

  // Logic: Potentiometer > Threshold OR Light < Threshold (Dark)
  // Assuming Light Sensor: Low Value = Dark, High Value = Bright
  
  bool shouldLightBeOn = (lightVal < lightThreshold); 
  bool forcedOff = (potVal < 100); // Example: Pot turned all the way down forces off

  if (potVal >= potThreshold) {
    // Force ON
    digitalWrite(ledHeadlight, HIGH);
  } else if (shouldLightBeOn && !forcedOff) {
    // Auto ON
    digitalWrite(ledHeadlight, HIGH);
  } else {
    digitalWrite(ledHeadlight, LOW);
  }

  // Warning: It's dark, but light is forced OFF
  if (shouldLightBeOn && forcedOff) {
     lcd.setCursor(0, 1);
     lcd.print("LOW LIGHT!");
     // Small low freq beep
     if (currentMillis % 1000 < 100) tone(buzzerPin, 200);
     else noTone(buzzerPin);
  }
}

void checkDistance(unsigned long currentMillis) {
  // Check distance every 100ms (not every loop to save CPU)
  if (currentMillis - lastDistCheck > 100) {
    lastDistCheck = currentMillis;
    
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    
    long duration = pulseIn(echoPin, HIGH, 30000); // Timeout 30ms
    distance = duration * 0.034 / 2;
    
    if (distance > 0 && distance < 50) {
      lcd.setCursor(9, 1); // Bottom right
      lcd.print("D:"); lcd.print(distance); lcd.print("cm ");
      
      // Buzzer Logic based on distance
      int delayBeep = distance * 10; // Closer = Smaller delay
      if (delayBeep < 50) delayBeep = 50;
      
      // Simple tone logic
      tone(buzzerPin, 1000, 50); 
      // Note: A true non-blocking variable delay is complex, 
      // simple tone with duration works ok here.
    }
  }
}

void checkMotion(unsigned long currentMillis) {
  int motion = digitalRead(motionSensor);
  
  if (motion == HIGH) {
    // Presence detected -> LED OFF
    digitalWrite(ledSecurity, LOW);
  } else {
    // No one -> Blink LED
    if ((currentMillis / 500) % 2 == 0) {
      digitalWrite(ledSecurity, HIGH);
    } else {
      digitalWrite(ledSecurity, LOW);
    }
  }
}

void checkTemperature(unsigned long currentMillis) {
  // Check only every 2 seconds
  if (currentMillis % 2000 == 0) {
    float t = dht.readTemperature();
    if (t > 30) { // Threshold
       lcd.setRGB(255, 0, 0); // RED Alert
       lcd.setCursor(0, 1);
       lcd.print("TEMP HIGH!");
       tone(buzzerPin, 2000); // Continuous
    } else if (isCarStarted) {
       // Restore Green if temp is OK
       lcd.setRGB(0, 255, 0);
    }
  }
}
