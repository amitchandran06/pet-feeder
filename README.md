# Smart Pet Feeder - BLE IoT System

A robust IoT solution for an Automatic Pet Feeder that communicates via **Bluetooth Low Energy (BLE)** using the **ESP32** and a custom **Android App**. This project utilizes the **NimBLE** library for high-performance, low-latency communication.

![Project Status](https://img.shields.io/badge/Status-Beta-yellow)
![Platform](https://img.shields.io/badge/Platform-ESP32%20%7C%20Android-green)
![Protocol](https://img.shields.io/badge/Protocol-BLE%20(Nordic%20UART)-blue)

---

## Table of Contents
1. [System Architecture](#-system-architecture)
2. [Hardware & Firmware (ESP32)](#-hardware--firmware-esp32)
3. [Android App (Client)](#-android-app-client)
4. [Communication Protocol](#-communication-protocol)
5. [Setup & Installation](#-setup--installation)
6. [Troubleshooting](#-troubleshooting)

---

## System Architecture

The system follows a standard **GATT Server/Client** architecture:

* **Server (Peripheral):** The **ESP32** runs the Pet Feeder logic. It advertises its presence and holds the state of the feeder (e.g., LED status, Motor status).
* **Client (Central):** The **Android Phone** connects to the ESP32, writes commands (e.g., "Feed Now"), and subscribes to notifications for updates.

---

## Hardware & Firmware (ESP32)

### Prerequisites
* **Microcontroller:** ESP32 (WROOM, DevKit V1, etc.)
* **Framework:** Arduino IDE or PlatformIO (VS Code)
* **Library:** `NimBLE-Arduino` by h2zero (v1.4 or v2.x compatible)

### Features Implemented
* ✅ **NimBLE Stack:** Uses 50% less RAM/Flash than standard Bluedroid.
* ✅ **Nordic UART Service:** Compatible with standard terminal apps.
* ✅ **Event-Driven Architecture:** Uses Callbacks for `onWrite`, `onConnect`, and `onSubscribe`.
* ✅ **Input Sanitization:** Automatically trims `\n` and `\r` from incoming commands.
* ✅ **Welcome Handshake:** Waits for the client to Subscribe before sending the welcome message (prevents data loss).

### Pin Configuration
| Component | ESP32 Pin | Logic |
| :--- | :--- | :--- |
| **Status LED** | GPIO 2 | High = ON |
| **Feeder Motor** | GPIO 5 (Example) | High = Active |

---

## Android App (Client)

The Android application is designed to act as a remote control for the feeder.

### Tech Stack
* **IDE:** Android Studio (Kotlin/Java)
* **Min SDK:** 24 (Android 7.0)
* **Permissions Required:**
    * `BLUETOOTH`, `BLUETOOTH_ADMIN` (Legacy)
    * `BLUETOOTH_SCAN`, `BLUETOOTH_CONNECT` (Android 12+)
    * `ACCESS_FINE_LOCATION` (Required for scanning)

### Core Features
1. **Scanning:** Filters specifically for the Service UUID `6E40...` to find the feeder instantly.
2. **Connection Management:** Handles GATT connections and auto-reconnect logic.
3. **Command Center:** Simple UI buttons to send "ON" / "OFF" text commands.
4. **Live Terminal:** Displays incoming notifications from the feeder (e.g., "Fed at 12:00").

---

## Communication Protocol

This project uses the industry-standard **Nordic UART Service (NUS)** UUIDs. This allows you to test the device using generic apps like *nRF Connect* or *Serial Bluetooth Terminal*.

### UUID Configuration
| Component | UUID | Description |
| :--- | :--- | :--- |
| **Service** | `6E400001-B5A3-F393-E0A9-E50E24DCCA9E` | The main container. |
| **RX Char** | `6E400002-B5A3-F393-E0A9-E50E24DCCA9E` | **Phone -> ESP32** (Write). |
| **TX Char** | `6E400003-B5A3-F393-E0A9-E50E24DCCA9E` | **ESP32 -> Phone** (Notify). |

### Command List
| Command | Action on ESP32 | Response (TX) |
| :--- | :--- | :--- |
| `"ON"` | Turns on the LED / Motor | `>> Turning ON` |
| `"OFF"` | Turns off the LED / Motor | `>> Turning OFF` |
| *Any* | Unknown command | `Command not recognized` |

---

## Setup & Installation

### 1. Flash the ESP32
1. Open the project in VS Code (PlatformIO) or Arduino IDE.
2. Install the `NimBLE-Arduino` library.
3. Select your board and upload.
4. Open Serial Monitor at **115200 baud**.

### 2. Run the Android App
1. Open the project in Android Studio.
2. Connect your phone via USB (USB Debugging ON).
3. Build and Run.
4. **Important:** Grant "Location" and "Nearby Devices" permissions when prompted.

---

## Troubleshooting

**Issue: The App scans but finds nothing.**
* **Fix:** Ensure GPS/Location is turned **ON** on your phone. Android blocks BLE scanning without it.

**Issue: "Command not recognized" even when I send "ON".**
* **Fix:** Your terminal app is sending hidden characters like `\n` or `\r`. The firmware has a trimmer, but check your app settings to send "No Line Ending" if possible.

**Issue: The Welcome Message never appears.**
* **Fix:** Ensure your App subscribes to the TX Characteristic (`setCharacteristicNotification`) immediately after connecting. The ESP32 waits for this subscription before talking.

---

### Authors
**Amit** - *Lead Developer for firmware / ESP32 C++*

**Alberto** - *Lead Developer for Android / Kotlin & Java*
*Version 0.1 Beta*
