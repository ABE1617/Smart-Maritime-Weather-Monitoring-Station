# 🌊 Project W4 (Word Wide Weather Watcher): Smart Maritime Weather Monitoring Station

OceanGuardian is a lightweight, Arduino-based weather station designed for ships. It collects and logs essential environmental data like temperature, humidity, brightness, and GPS coordinates — helping monitor ocean conditions and detect early signs of cyclones and natural disasters.

---

## 📦 Key Features

- Real-time sensor data (Temperature, Humidity, Light, GPS)
- Data logging on SD card with timestamps
- Multiple operating modes (Standard, Config, Maintenance, Eco)
- Serial interface for configuration
- Visual system diagnostics using LED indicators

---

## ⚙️ Hardware Overview

- **Arduino Uno**
- **DHT11** – Temperature & humidity sensor
- **BH1750** – Light intensity sensor
- **GPS Module** – For real-time geolocation
- **RTC Module** – Keeps accurate date & time
- **SD Card Adapter** – For local data storage
- **2 Push Buttons** – For switching modes
- **RGB LED** – Status/error indicator

---

## 🔧 Modes of Operation

| Mode            | Purpose                                          |
|-----------------|--------------------------------------------------|
| **Standard**     | Regular data collection and SD logging          |
| **Configuration**| Adjust settings via serial console              |
| **Maintenance**  | SD card access without writing                  |
| **Eco**          | Power-saving mode with slower sensor polling    |

---

## 🚀 Getting Started

### Requirements

- Arduino IDE
- Required libraries: `DHT`, `RTClib`, `SD`, `Wire`, `SoftwareSerial`, `BH1750`

### Upload & Run

1. Clone this repository
2. Open the `Main.ino` file in Arduino IDE
3. Connect your Arduino board and upload the code

---

## 🖥️ Configuration via Serial

Use the serial monitor to change parameters:

- `LOG_INTERVAL-10` → Set data interval (in minutes)
- `FILE_MAX_SIZE-4096` → Max log file size (in bytes)
- `RESET` → Restore default settings
- `CLOCK`, `DATE`, `DAY` → Set RTC

---

## 🔍 LED Indicators

| LED Status                     | Meaning                        |
|--------------------------------|--------------------------------|
| Green (solid)                 | Standard mode                  |
| Yellow (solid)                | Configuration mode             |
| Blue (solid)                  | Eco mode                       |
| Orange (solid)                | Maintenance mode               |
| Red + Other (blinking)        | Hardware or sensor errors      |

---

## 📂 Documentation

- Project reports & diagrams available in the `/docs` folder:
  - `Main.ino` file
  - User manual


