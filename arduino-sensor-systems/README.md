# Arduino Sensor Systems: Intelligent Car & Weather Station

This repository contains two embedded systems projects developed as part of a **Sensors and Networks** course. These projects demonstrate the integration of multiple sensors, actuators, and logical processing using the **Arduino Uno** and the **Seeed Studio Grove** ecosystem.

## 🚗 Project 1: Intelligent Car Dashboard

A comprehensive simulation of a smart car's control system. This project manages engine states, automates safety features, and monitors environmental conditions using non-blocking logic.

### Key Features
* [cite_start]**Engine Control:** Master button to start/stop the system with visual status feedback (RGB LCD)[cite: 31, 192].
* **Safety Monitoring:**
    * [cite_start]**Seatbelt Alarm:** Triggers a specific buzzer alert if the seatbelt isn't fastened within 10 seconds of starting the engine[cite: 39, 199].
    * [cite_start]**Obstacle Detection:** Ultrasonic sensor measures distance (0-50cm) and adjusts buzzer frequency dynamically (closer = faster beeping)[cite: 76, 202].
* [cite_start]**Lighting Automation:** Headlights activate automatically in low-light conditions, with a manual override via potentiometer[cite: 66, 195].
* [cite_start]**Security Mode:** When the engine is OFF, a PIR motion sensor detects intruders and triggers a security LED sequence[cite: 34, 203].
* [cite_start]**Engine Health:** Monitors temperature via DHT11; alerts the driver if overheating occurs (>30°C)[cite: 85, 206].

### ⚡ Technical Highlight: Non-Blocking Code
Unlike basic Arduino sketches, this project avoids `delay()` for sensor management. [cite_start]It utilizes `millis()` timers to handle the buzzer, LCD updates, and sensor readings simultaneously, ensuring the "Stop" button remains responsive at all times[cite: 43, 62].

### 🔌 Pinout Configuration
| Component | Pin | Type |
| :--- | :--- | :--- |
| **Start Button** | D2 | Input |
| **Buzzer** | D3 | Output (PWM) |
| **Seatbelt Button** | D4 | Input |
| **Headlight LED** | D5 | Output |
| **Motion Sensor (PIR)** | D6 | Input |
| **Security LED** | D7 | Output |
| **DHT11 Sensor** | D8 | Input |
| **Ultrasonic Trig** | D9 | Output |
| **Ultrasonic Echo** | D10 | Input |
| **Potentiometer** | A0 | Analog Input |
| **Light Sensor** | A1 | Analog Input |
| **RGB LCD** | I2C | Display |

---

## ☁️ Project 2: Advanced Weather Station

A multi-output weather monitoring station that aggregates sensor data to predict and display meteorological conditions.

### Key Features
* **Multi-Display Output:**
    * [cite_start]**4-Digit Display:** Cycles through Time (simulated), Temperature, and Humidity sequentially[cite: 130, 215].
    * [cite_start]**RGB LCD:** Displays the calculated "Weather State" with color-coded backgrounds (e.g., Red for Hot/Humid, Blue for Cold/Rain)[cite: 137, 217].
* [cite_start]**Weather Prediction Logic:** Analyzes data from a **Barometer (BMP280)** and **DHT11** to categorize weather into states: *Hot & Humid*, *Cold/Rain*, or *Normal*[cite: 133, 217].
* [cite_start]**Visual Alerts:** A status LED blinks at different frequencies depending on the severity of the weather condition[cite: 141, 219].

### 🔌 Pinout Configuration
| Component | Pin | Type |
| :--- | :--- | :--- |
| **DHT11 Sensor** | D2 | Input |
| **4-Digit Display (CLK)**| D5 | Output |
| **4-Digit Display (DIO)**| D6 | Output |
| **Status LED** | D13 | Output |
| **BMP280 Barometer** | I2C | Input |
| **RGB LCD** | I2C | Display |

---

## 🛠️ Hardware Used
* **Microcontroller:** Arduino Uno
* **Shield:** Grove Base Shield V2
* **Sensors:** Grove DHT11, Grove BMP280, Grove Ultrasonic Ranger, Grove PIR Motion Sensor, Grove Light Sensor.
* **Actuators:** Grove RGB LCD, Grove 4-Digit Display (TM1637), LEDs, Buzzer.

## 📚 Libraries Required
To run these projects, the following libraries must be installed in the Arduino IDE:
1.  `DHT sensor library`
2.  `Grove - LCD RGB Backlight`
3.  `TM1637Display`
4.  `Seeed_BMP280`

## 👤 Author
Project created for the **Sensors and Networks** curriculum.