# ESP32 Smart Home Automation System

An IoT-based smart home automation system built using the ESP32 microcontroller.

The system integrates multiple sensors, appliances, alarms, an LCD interface, and a web-based control interface into a single modular home automation platform.

---

## 📌 Project Overview

This project is designed to demonstrate a practical smart home automation system using an ESP32.

The system can monitor environmental and security-related sensors and control connected devices according to the selected operating mode.

A web-based interface hosted by the ESP32 provides a convenient way to monitor sensor readings and control the system from a smartphone or computer connected to the same network.

---

## ✨ Features

- ESP32-based home automation
- Web-based control interface
- Real-time sensor monitoring
- PIR motion detection
- LDR-based light detection
- Flame/fire detection
- DHT11 temperature and humidity monitoring
- LCD status display
- Buzzer-based alarm system
- Manual and automatic operation modes
- Modular C++ software architecture
- Sensor and device status monitoring
- Smartphone-friendly web interface

---

## 🧰 Hardware Used

- ESP32 development board
- PIR motion sensor
- LDR sensor module
- Flame sensor
- DHT11 temperature and humidity sensor
- I2C LCD display
- LEDs
- Buzzer
- Transistor/MOSFET-based device control
- Breadboard
- Jumper wires
- Power supply/battery system

---

## 💻 Software & Technologies

- **Platform:** ESP32
- **Framework:** Arduino
- **Build System:** PlatformIO
- **Programming Language:** C++
- **Frontend:** HTML, CSS, JavaScript
- **Development Environment:** Visual Studio Code
- **Version Control:** Git & GitHub

---

## 📂 Project Structure

```text
ESP32-Smart-Home-Automation/
│
├── data/
│   ├── app.js
│   ├── index.html
│   └── style.css
│
├── include/
│   ├── alarms.h
│   ├── animations.h
│   ├── api.h
│   ├── config.h
│   ├── devices.h
│   ├── lcd.h
│   ├── modes.h
│   └── sensors.h
│
├── lib/
│
├── src/
│   ├── alarms.cpp
│   ├── animations.cpp
│   ├── api.cpp
│   ├── devices.cpp
│   ├── lcd.cpp
│   ├── main.cpp
│   ├── modes.cpp
│   └── sensors.cpp
│
├── test/
│
├── .gitignore
└── platformio.ini
```

---

## 🏗️ Software Architecture

The project is divided into separate modules to make the code easier to maintain and expand.

### Sensors

Responsible for reading:

- PIR
- LDR
- Flame sensor
- DHT11

### Devices

Responsible for controlling connected output devices such as LEDs, buzzer, and other appliances.

### Alarms

Handles alarm conditions and buzzer behaviour.

### Modes

Controls the different operating modes of the automation system.

### LCD

Handles information displayed on the LCD.

### API

Provides communication between the ESP32 and the web interface.

### Animations

Handles LCD/interface animations and visual feedback.

---

## 🌐 Web Interface

The ESP32 hosts a web-based interface that can be accessed from a connected smartphone or computer.

The interface provides:

- Sensor status
- Device control
- Operating mode selection
- System status
- Real-time updates

The frontend files are located in:

```text
data/
├── index.html
├── style.css
└── app.js
```

---

## ⚙️ Operating Modes

The system supports different operating modes for controlling the connected devices.

### Manual Mode

The user directly controls the connected devices through the web interface.

### Automatic Mode

The ESP32 uses sensor inputs to automatically control the system according to the programmed logic.

---

## 🔥 Safety & Alarm System

The system includes sensor-based alarm functionality.

The flame sensor can be used to detect a possible fire condition, while the buzzer provides an audible warning.

The system also provides sensor/status feedback through the LCD and web interface.

---

## 📺 LCD Interface

The LCD provides local feedback without requiring a smartphone or computer.

It can display system information, sensor information, modes, alarms, and other status messages.

---

## 🚀 Getting Started

### 1. Clone the repository

```bash
git clone https://github.com/Harsh75899/ESP32-Smart-Home-Automation.git
```

### 2. Open the project

Open the project folder in Visual Studio Code with PlatformIO installed.

### 3. Connect the ESP32

Connect the ESP32 development board to your computer using USB.

### 4. Configure the project

Check the configuration and pin definitions in:

```text
include/config.h
```

### 5. Build the project

Use PlatformIO to build the project.

### 6. Upload to ESP32

Upload the firmware using PlatformIO.

### 7. Upload web files

If the project configuration requires filesystem data to be uploaded, upload the contents of the `data` directory using the appropriate PlatformIO filesystem upload procedure.

---

## 🎥 Project Demonstration

A demonstration video showing the working of the completed system will be added here.

🎥 [Watch the ESP32 Smart Home Automation Demo on YouTube](https://youtu.be/tpzSRQXaNdg)
---

## 📸 Project Images

Project photographs and screenshots of the web interface will be added here.

---

## 🔮 Future Improvements

Possible future improvements include:

- Mobile application
- Cloud connectivity
- Remote monitoring over the Internet
- Additional sensors
- More appliance control
- Energy monitoring
- Data logging
- Notification system
- Improved user interface
- Security and authentication

---

## 👨‍💻 Author

**Harsh Ozare**

VJTI — Electronics Engineering

---

## 📜 License

This project currently does not specify a license.
