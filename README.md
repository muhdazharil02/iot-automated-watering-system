# Smart Automated Watering System using ESP32

An IoT-based automated watering system developed using ESP32, environmental sensors, and Blynk cloud integration for real-time plant monitoring and intelligent watering automation.

This project achieved **12th place out of 70+ teams** during a technology innovation competition.

---

# Overview

This system monitors:

- Soil moisture
- Temperature
- Humidity
- Light intensity

The ESP32 automatically activates a water pump based on predefined environmental conditions. The system also provides real-time monitoring through the Blynk mobile application and an OLED display.

---

# Features

- Automatic plant watering
- Real-time sensor monitoring
- WiFi & Blynk cloud integration
- OLED live status display
- Temperature-based watering logic
- Soil moisture automation
- Relay-controlled water pump
- NTP real-time clock synchronization
- Mobile application monitoring

---

# Technologies Used

- ESP32
- Arduino IDE
- Blynk IoT Platform
- OLED SSD1306
- BME680 Environmental Sensor
- C++
- Embedded Systems
- IoT


# System Architecture

```text
Sensors → ESP32 → Decision Logic → Relay → Water Pump
                     ↓
               Blynk Cloud
                     ↓
              Mobile Monitoring
```

---

# Hardware Components

| Component | Purpose |
|---|---|
| ESP32 | Main microcontroller |
| Soil Moisture Sensor | Detect soil moisture level |
| BME680 | Temperature & humidity monitoring |
| OLED Display | Live system display |
| Relay Module | Control water pump |
| Water Pump | Plant watering |
| LDR Sensor | Detect light intensity |

---

# Automation Logic

The system activates watering when:

## Rule 1
- Temperature exceeds 40°C
- Time between 11:00 AM and 5:59 PM

## Rule 2
- Soil moisture drops below 10%

When triggered:
1. OLED displays watering status
2. Relay activates water pump for 3 seconds
3. System automatically restarts

---


# Results

- Stable real-time monitoring
- Successful automated watering
- Remote monitoring through Blynk
- Low-latency relay response
- Reliable environmental sensing

Competition Achievement:
- Ranked 12th out of 70+ participating teams

---

# Future Improvements

- Solar-powered operation
- Mobile push notifications
- AI-based watering prediction
- Cloud data logging
- Multi-zone irrigation support


---

# Author

Azharil Syamri  
Bachelor of Computer Science  
Universiti Teknologi MARA (UiTM)

Interested in:
- Embedded Systems
- IoT
- Edge AI
- Computer Vision
- Raspberry Pi Development
