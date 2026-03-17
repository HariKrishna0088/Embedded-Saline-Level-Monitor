<p align="center">
  <img src="https://img.shields.io/badge/Platform-Arduino-teal?style=for-the-badge&logo=arduino&logoColor=white" alt="Arduino"/>
  <img src="https://img.shields.io/badge/Sensor-HX711-blue?style=for-the-badge" alt="HX711"/>
  <img src="https://img.shields.io/badge/Application-Medical%20IoT-red?style=for-the-badge" alt="Medical"/>
  <img src="https://img.shields.io/badge/Category-Embedded-green?style=for-the-badge" alt="Embedded"/>
</p>

# ðŸ¥ Saline Level Monitoring System

> An Arduino-based IV saline drip monitoring system with load cell sensing, real-time LCD display, multi-level audio/visual alarms, and drip rate calculation â€” designed for hospital environments.

---

## ðŸ” Overview

This project addresses a critical healthcare need: **automated monitoring of IV saline drip levels**. The system uses a load cell sensor to continuously measure the saline bag weight, calculates the remaining percentage and drip rate, and alerts nursing staff when the saline level drops below configurable thresholds.

### Key Highlights
- âš–ï¸ **Precision Weighing** â€” HX711 24-bit ADC with load cell
- ðŸ“Š **4 Alert Levels** â€” Normal â†’ Low â†’ Critical â†’ Empty
- ðŸ”” **Multi-Level Alarm** â€” Buzzer frequency increases with urgency
- ðŸ’§ **Drip Rate Calculation** â€” mL/min flow rate estimation
- ðŸ“º **LCD Dashboard** â€” Live level, weight, and status display
- ðŸš¨ **3 LED Indicators** â€” Green/Yellow/Red visual status
- ðŸ”˜ **Alarm Acknowledge** â€” Nurse can silence alarm with button

---

## ðŸ—ï¸ System Architecture

```
â”Œâ”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”     â”Œâ”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”
â”‚   Load Cell      â”‚     â”‚        Arduino Uno            â”‚
â”‚   + HX711 ADC    â”œâ”€â”€â”€â”€â–ºâ”‚                               â”‚
â”‚   (Saline Bag)   â”‚     â”‚  â”Œâ”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”  â”‚
â””â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”˜     â”‚  â”‚  State Machine:        â”‚  â”‚â”€â”€â–º ðŸ“º LCD Display
                         â”‚  â”‚  NORMAL â†’ LOW â†’        â”‚  â”‚    (16x2 I2C)
                         â”‚  â”‚  CRITICAL â†’ EMPTY      â”‚  â”‚
                         â”‚  â””â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”˜  â”‚â”€â”€â–º ðŸ”´ðŸŸ¡ðŸŸ¢ LEDs
                         â”‚                               â”‚
  â”Œâ”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”       â”‚  â”Œâ”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”  â”‚â”€â”€â–º ðŸ”” Buzzer
  â”‚ ACK Button   â”œâ”€â”€â”€â”€â”€â”€â–ºâ”‚  â”‚  Alarm Manager         â”‚  â”‚
  â””â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”˜       â”‚  â””â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”˜  â”‚â”€â”€â–º ðŸ“Ÿ Serial Log
                         â””â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”˜
```

---

## ðŸ”Œ Circuit Connections

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

## âš ï¸ Alert Levels

| Level | Threshold | LED | Buzzer | LCD Display |
|:-----:|:---------:|:---:|:------:|:------------|
| **Normal** | > 25% | ðŸŸ¢ | Silent | `OK 3.2mL/min` |
| **Low** | â‰¤ 25% | ðŸŸ¡ | Intermittent beep | `âš  LOW LEVEL!` |
| **Critical** | â‰¤ 10% | ðŸ”´ | Rapid beeping | `âš  CRITICAL!!` |
| **Empty** | â‰¤ 5% | ðŸ”´ | Continuous alarm | `âš  EMPTY!!NURSE` |

---

## ðŸ“ File Structure

```
Embedded-Saline-Level-Monitor/
â”œâ”€â”€ src/
â”‚   â””â”€â”€ main.cpp              # Main application
â”œâ”€â”€ docs/
â”‚   â””â”€â”€ circuit_diagram.md
â”œâ”€â”€ platformio.ini            # PlatformIO configuration
â”œâ”€â”€ .gitignore
â”œâ”€â”€ LICENSE
â””â”€â”€ README.md
```

---

## ðŸš€ Getting Started

```bash
git clone https://github.com/Harikrishna_08/Embedded-Saline-Level-Monitor.git
cd Embedded-Saline-Level-Monitor

# Build & Upload (PlatformIO)
pio run --target upload

# Open Serial Monitor
pio device monitor
```

---

## ðŸ‘¨â€ðŸ’» Author

**Daggolu Hari Krishna** â€” B.Tech ECE | JNTUA College of Engineering, Kalikiri

[![LinkedIn](https://img.shields.io/badge/LinkedIn-Connect-blue?style=flat-square&logo=linkedin)](https://www.linkedin.com/in/contacthari88/)
[![GitHub](https://img.shields.io/badge/GitHub-Harikrishna__08-black?style=flat-square&logo=github)](https://github.com/Harikrishna_08)

---

<p align="center">â­ Star this repo if you found it useful! â­</p>
