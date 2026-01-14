# Accident Detection and Emergency Alert System Using Arduino

<img src="Project Demo/Rear View.jpeg" width="600">
<img src="Project Demo/Side View.jpeg" width="600">



## Overview

This project implements an **Accident Detection and Emergency Alert System** using Arduino.  
It detects sudden impacts or abnormal vibrations using an accelerometer, retrieves real-time GPS coordinates, and automatically sends an SMS alert containing a Google Maps location link via a GSM module.

The system is suitable for vehicle safety, emergency response, and accident monitoring applications.

---

## System Components

- **Arduino Board** (Uno / Nano compatible)
- **Accelerometer Sensor** (Analog output on X, Y, Z axes)
- **GSM Module** (SIM800L or equivalent)
- **GPS Module** (NEO-6M)
- **Buzzer**
- **Push Button** (Alert cancel/reset)
- **Connecting Wires & Power Supply**

---

## Working Principle

1. The accelerometer continuously monitors vibration and acceleration values.
2. A sudden change in acceleration beyond a defined sensitivity threshold is treated as an accident.
3. Once an impact is detected:
   - The buzzer is activated.
   - GPS coordinates are fetched from the GPS module.
   - After a predefined delay, an SMS alert is sent to the emergency number.
4. The SMS includes a Google Maps link showing the accident location.
5. The user can cancel the alert using the push button.
6. The system can also respond to SMS commands (e.g., `GET GPS`) to send the current location.

---

## Pin Configuration

### GSM Module (SoftwareSerial)
- RX → Arduino Pin 6  
- TX → Arduino Pin 7  

### GPS Module (SoftwareSerial)
- RX → Arduino Pin 9  
- TX → Arduino Pin 8  

### Other Connections
- Buzzer → Pin 12  
- Button → Pin 11 (INPUT_PULLUP)  
- Accelerometer:
  - X-axis → A0  
  - Y-axis → A1  
  - Z-axis → A2  

---

## Libraries Used

```cpp
#include <AltSoftSerial.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <math.h>
#include <Wire.h>
