<p align="center">
  <img src="https://img.shields.io/badge/Platform-Arduino-teal?style=for-the-badge&logo=arduino&logoColor=white" alt="Arduino"/>
  <img src="https://img.shields.io/badge/Sensor-HX711-blue?style=for-the-badge" alt="HX711"/>
  <img src="https://img.shields.io/badge/Application-Medical%20IoT-red?style=for-the-badge" alt="Medical"/>
  <img src="https://img.shields.io/badge/Category-Embedded-green?style=for-the-badge" alt="Embedded"/>
</p>

# 🏥 Saline Level Monitoring System

> An Arduino-based IV saline drip monitoring system with load cell sensing, real-time LCD display, multi-level audio/visual alarms, and drip rate calculation — designed for hospital environments.

---

## 🔍 Overview

This project addresses a critical healthcare need: **automated monitoring of IV saline drip levels**. The system uses a load cell sensor to continuously measure the saline bag weight, calculates the remaining percentage and drip rate, and alerts nursing staff when the saline level drops below configurable thresholds.

### Key Highlights
- ⚖️ **Precision Weighing** — HX711 24-bit ADC with load cell
- 📊 **4 Alert Levels** — Normal → Low → Critical → Empty
- 🔔 **Multi-Level Alarm** — Buzzer frequency increases with urgency
- 💧 **Drip Rate Calculation** — mL/min flow rate estimation
- 📺 **LCD Dashboard** — Live level, weight, and status display
- 🚨 **3 LED Indicators** — Green/Yellow/Red visual status
- 🔘 **Alarm Acknowledge** — Nurse can silence alarm with button

---

## 🏗️ System Architecture

```
┌──────────────────┐     ┌──────────────────────────────┐
│   Load Cell      │     │        Arduino Uno            │
│   + HX711 ADC    ├────►│                               │
│   (Saline Bag)   │     │  ┌────────────────────────┐  │
└──────────────────┘     │  │  State Machine:        │  │──► 📺 LCD Display
                         │  │  NORMAL → LOW →        │  │    (16x2 I2C)
                         │  │  CRITICAL → EMPTY      │  │
                         │  └────────────────────────┘  │──► 🔴🟡🟢 LEDs
                         │                               │
  ┌──────────────┐       │  ┌────────────────────────┐  │──► 🔔 Buzzer
  │ ACK Button   ├──────►│  │  Alarm Manager         │  │
  └──────────────┘       │  └────────────────────────┘  │──► 📟 Serial Log
                         └──────────────────────────────┘
```

---

## 🔌 Circuit Connections

| Component | Arduino Pin | Description |
|:---------:|:-----------:|:------------|
| HX711 DOUT | D3 | Load cell data |
| HX711 SCK | D2 | Load cell clock |
| Buzzer | D8 | Alert buzzer |
| Green LED | D5 | Level OK |
| Yellow LED | D6 | Low warning |
| Red LED | D7 | Critical alert |
| ACK Button | D4 | Silence alarm |
| LCD SDA | A4 | I2C data |
| LCD SCL | A5 | I2C clock |

---

## ⚠️ Alert Levels

| Level | Threshold | LED | Buzzer | LCD Display |
|:-----:|:---------:|:---:|:------:|:------------|
| **Normal** | > 25% | 🟢 | Silent | `OK 3.2mL/min` |
| **Low** | ≤ 25% | 🟡 | Intermittent beep | `⚠ LOW LEVEL!` |
| **Critical** | ≤ 10% | 🔴 | Rapid beeping | `⚠ CRITICAL!!` |
| **Empty** | ≤ 5% | 🔴 | Continuous alarm | `⚠ EMPTY!!NURSE` |

---

## 📁 File Structure

```
Embedded-Saline-Level-Monitor/
├── src/
│   └── main.cpp              # Main application
├── docs/
│   └── circuit_diagram.md
├── platformio.ini            # PlatformIO configuration
├── .gitignore
├── LICENSE
└── README.md
```

---

## 🚀 Getting Started

```bash
git clone https://github.com/Harikrishna_08/Embedded-Saline-Level-Monitor.git
cd Embedded-Saline-Level-Monitor

# Build & Upload (PlatformIO)
pio run --target upload

# Open Serial Monitor
pio device monitor
```

---

## 👨‍💻 Author

**Daggolu Hari Krishna** — B.Tech ECE | JNTUA College of Engineering, Kalikiri

[![LinkedIn](https://img.shields.io/badge/LinkedIn-Connect-blue?style=flat-square&logo=linkedin)](https://linkedin.com/in/harikrishnadaggolu)
[![GitHub](https://img.shields.io/badge/GitHub-Harikrishna__08-black?style=flat-square&logo=github)](https://github.com/Harikrishna_08)

---

<p align="center">⭐ Star this repo if you found it useful! ⭐</p>
