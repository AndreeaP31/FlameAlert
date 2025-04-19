# FlameAlert

## Table of Contents
- [Overview](#overview)
- [Features](#features)
- [Hardware Components](#hardware-components)
- [Technologies Used](#technologies-used)
- [System Configuration](#system-configuration)
- [Code Explanation](#code-explanation)
- [Installation](#installation)

## Overview
**FlameAlert** is an IoT-based system built with an ESP32 that detects the presence of flames and sends alerts via WhatsApp using Google Sheets and IFTTT. It also activates a buzzer for local sound notification.

## Features
- Detects flames using a dedicated sensor
- Sends real-time alerts via WhatsApp using IFTTT
- Logs detection events in Google Sheets
- Activates a buzzer as a local alarm

## Hardware Components
- **ESP32** — Microcontroller with Wi-Fi capabilities
- **Flame Sensor** — Detects infrared light emitted by flames
- **Buzzer** — Emits sound when flame is detected

## Technologies Used
- Arduino IDE
- ESP32 Wi-Fi
- Google Sheets API via ESP-Google-Sheet-Client library
- IFTTT (If This Then That)

## System Configuration
1. **Google Cloud Service Setup:**
   - Created a Google Service Account
   - Enabled Google Sheets API
2. **IFTTT Applet:**
   - Trigger: New row added in Google Sheet
   - Action: Send WhatsApp alert

## Code Explanation
- Connects to Wi-Fi using ESP32
- Reads data from the flame sensor on pin 34
- When a flame is detected:
  - Activates the buzzer (pin 15)
  - Logs the current time and a message to Google Sheets
  - Triggers IFTTT to send a WhatsApp alert
- Uses `getFormattedTime()` to adjust time zone and format the timestamp

## Installation
1. Open the code in Arduino IDE
2. Install required libraries:
   - ESP-Google-Sheet-Client
   - WiFi
3. Fill in credentials:
   - `PROJECT_ID`, `CLIENT_EMAIL`, `PRIVATE_KEY`, and `spreadsheetId[]`
4. Upload code to ESP32 board
5. Monitor output via Serial Monitor


