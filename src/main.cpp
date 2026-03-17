/*=============================================================================
 * Saline Level Monitoring System
 * Author: Daggolu Hari Krishna
 * Platform: Arduino Uno / Nano
 *
 * Monitors IV saline drip level using HX711 load cell sensor.
 * Alerts nurses via buzzer + LCD when saline level is critically low.
 *
 * Components:
 *  - HX711 Load Cell Amplifier + Load Cell (0-5kg)
 *  - 16x2 I2C LCD Display
 *  - Buzzer for alert
 *  - 3 LEDs (Green/Yellow/Red) for visual level indication
 *  - Push button to acknowledge/silence alarm
 *=============================================================================*/

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <HX711.h>

// ==================== Pin Definitions ====================
#define HX711_DOUT     3    // HX711 data pin
#define HX711_SCK      2    // HX711 clock pin
#define BUZZER_PIN     8    // Piezo buzzer
#define LED_GREEN      5    // Level OK
#define LED_YELLOW     6    // Level Low warning
#define LED_RED        7    // Level Critical
#define ACK_BUTTON     4    // Alarm acknowledge button

// ==================== Configuration ====================
#define FULL_WEIGHT        500.0   // Full saline bag weight (grams)
#define LOW_THRESHOLD      25.0    // Low level warning (%)
#define CRITICAL_THRESHOLD 10.0    // Critical level alarm (%)
#define EMPTY_THRESHOLD    5.0     // Empty - urgent alarm (%)
#define CALIBRATION_FACTOR -7050.0 // HX711 calibration (adjust per load cell)
#define READ_INTERVAL      1000    // Sensor read interval (ms)
#define DRIP_RATE_WINDOW   30000   // Drip rate calculation window (ms)

// ==================== Objects ====================
LiquidCrystal_I2C lcd(0x27, 16, 2);
HX711 scale;

// ==================== Global Variables ====================
float currentWeight    = 0.0;
float previousWeight   = 0.0;
float levelPercentage  = 0.0;
float dripRate         = 0.0;    // mL per minute (approx)
unsigned long lastRead = 0;
unsigned long lastDripCalc = 0;
float weightAtLastDrip = 0.0;
bool alarmActive       = false;
bool alarmAcknowledged = false;

// Custom LCD characters
byte dropletChar[8]  = {0x04, 0x04, 0x0A, 0x0A, 0x11, 0x11, 0x0E, 0x00};
byte warningChar[8]  = {0x04, 0x0E, 0x0E, 0x1F, 0x1F, 0x04, 0x00, 0x04};
byte heartChar[8]    = {0x00, 0x0A, 0x1F, 0x1F, 0x0E, 0x04, 0x00, 0x00};

// ==================== State Machine ====================
enum SalineState {
    STATE_NORMAL,
    STATE_LOW,
    STATE_CRITICAL,
    STATE_EMPTY
};
SalineState currentState = STATE_NORMAL;

// ==================== Setup ====================
void setup() {
    Serial.begin(9600);

    Serial.println(F("========================================"));
    Serial.println(F("  Saline Level Monitoring System"));
    Serial.println(F("  Author: Daggolu Hari Krishna"));
    Serial.println(F("========================================"));

    // Pin modes
    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(LED_GREEN, OUTPUT);
    pinMode(LED_YELLOW, OUTPUT);
    pinMode(LED_RED, OUTPUT);
    pinMode(ACK_BUTTON, INPUT_PULLUP);

    // LCD setup
    lcd.init();
    lcd.backlight();
    lcd.createChar(0, dropletChar);
    lcd.createChar(1, warningChar);
    lcd.createChar(2, heartChar);

    // Splash screen
    lcd.setCursor(0, 0);
    lcd.print("Saline Monitor");
    lcd.setCursor(0, 1);
    lcd.write(2);
    lcd.print(" Hari Krishna");
    delay(2000);

    // HX711 setup
    scale.begin(HX711_DOUT, HX711_SCK);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Calibrating...");

    scale.set_scale(CALIBRATION_FACTOR);
    scale.tare(); // Reset to zero
    delay(1000);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("System Ready!");
    delay(1000);

    weightAtLastDrip = 0;
    lastDripCalc = millis();

    Serial.println(F("[OK] System initialized"));
}

// ==================== Main Loop ====================
void loop() {
    unsigned long now = millis();

    // Read sensor at interval
    if (now - lastRead >= READ_INTERVAL) {
        readWeight();
        calculateLevel();
        updateState();
        updateDisplay();
        updateLEDs();
        handleAlarm();
        logData();
        lastRead = now;
    }

    // Calculate drip rate every 30 seconds
    if (now - lastDripCalc >= DRIP_RATE_WINDOW) {
        calculateDripRate();
        lastDripCalc = now;
    }

    // Check acknowledge button
    if (digitalRead(ACK_BUTTON) == LOW) {
        delay(50); // Debounce
        if (digitalRead(ACK_BUTTON) == LOW) {
            alarmAcknowledged = true;
            noTone(BUZZER_PIN);
            Serial.println(F("[INFO] Alarm acknowledged"));
        }
    }
}

// ==================== Weight Reading ====================
void readWeight() {
    if (scale.is_ready()) {
        previousWeight = currentWeight;
        currentWeight = scale.get_units(5); // Average of 5 readings
        if (currentWeight < 0) currentWeight = 0;
    } else {
        Serial.println(F("[WARN] HX711 not ready!"));
    }
}

// ==================== Level Calculation ====================
void calculateLevel() {
    levelPercentage = (currentWeight / FULL_WEIGHT) * 100.0;
    if (levelPercentage > 100.0) levelPercentage = 100.0;
    if (levelPercentage < 0.0) levelPercentage = 0.0;
}

// ==================== Drip Rate ====================
void calculateDripRate() {
    float weightDiff = weightAtLastDrip - currentWeight;
    if (weightDiff < 0) weightDiff = 0;

    // Convert grams to mL (approx 1g = 1mL for saline)
    // Rate per minute = (diff in 30s) * 2
    dripRate = weightDiff * 2.0;
    weightAtLastDrip = currentWeight;

    Serial.print(F("[DRIP] Rate: "));
    Serial.print(dripRate, 1);
    Serial.println(F(" mL/min"));
}

// ==================== State Update ====================
void updateState() {
    SalineState newState;

    if (levelPercentage <= EMPTY_THRESHOLD)
        newState = STATE_EMPTY;
    else if (levelPercentage <= CRITICAL_THRESHOLD)
        newState = STATE_CRITICAL;
    else if (levelPercentage <= LOW_THRESHOLD)
        newState = STATE_LOW;
    else
        newState = STATE_NORMAL;

    if (newState != currentState) {
        currentState = newState;
        alarmAcknowledged = false; // Reset ack on state change
        Serial.print(F("[STATE] Changed to: "));
        switch (currentState) {
            case STATE_NORMAL:   Serial.println(F("NORMAL")); break;
            case STATE_LOW:      Serial.println(F("LOW")); break;
            case STATE_CRITICAL: Serial.println(F("CRITICAL")); break;
            case STATE_EMPTY:    Serial.println(F("EMPTY!")); break;
        }
    }
}

// ==================== LCD Display ====================
void updateDisplay() {
    lcd.clear();

    // Line 1: Level and weight
    lcd.setCursor(0, 0);
    lcd.write(0); // Droplet icon
    lcd.print(" ");
    lcd.print(levelPercentage, 1);
    lcd.print("% ");
    lcd.print(currentWeight, 0);
    lcd.print("g");

    // Line 2: Status and drip rate
    lcd.setCursor(0, 1);
    switch (currentState) {
        case STATE_NORMAL:
            lcd.print("OK ");
            lcd.print(dripRate, 1);
            lcd.print("mL/min");
            break;
        case STATE_LOW:
            lcd.write(1);
            lcd.print(" LOW LEVEL!");
            break;
        case STATE_CRITICAL:
            lcd.write(1);
            lcd.print(" CRITICAL!!");
            break;
        case STATE_EMPTY:
            lcd.write(1);
            lcd.print(" EMPTY!!NURSE");
            break;
    }
}

// ==================== LED Indicators ====================
void updateLEDs() {
    digitalWrite(LED_GREEN,  currentState == STATE_NORMAL  ? HIGH : LOW);
    digitalWrite(LED_YELLOW, currentState == STATE_LOW     ? HIGH : LOW);
    digitalWrite(LED_RED,    (currentState == STATE_CRITICAL || currentState == STATE_EMPTY) ? HIGH : LOW);
}

// ==================== Alarm Handler ====================
void handleAlarm() {
    if (alarmAcknowledged) {
        noTone(BUZZER_PIN);
        return;
    }

    switch (currentState) {
        case STATE_NORMAL:
            noTone(BUZZER_PIN);
            break;
        case STATE_LOW:
            // Intermittent beep
            tone(BUZZER_PIN, 1000, 200);
            break;
        case STATE_CRITICAL:
            // Rapid beeping
            tone(BUZZER_PIN, 2000, 100);
            break;
        case STATE_EMPTY:
            // Continuous alarm
            tone(BUZZER_PIN, 3000);
            break;
    }
}

// ==================== Serial Logging ====================
void logData() {
    Serial.print(F("[DATA] Weight="));
    Serial.print(currentWeight, 1);
    Serial.print(F("g | Level="));
    Serial.print(levelPercentage, 1);
    Serial.print(F("% | Rate="));
    Serial.print(dripRate, 1);
    Serial.print(F("mL/min | State="));
    switch (currentState) {
        case STATE_NORMAL:   Serial.println(F("NORMAL")); break;
        case STATE_LOW:      Serial.println(F("LOW")); break;
        case STATE_CRITICAL: Serial.println(F("CRITICAL")); break;
        case STATE_EMPTY:    Serial.println(F("EMPTY")); break;
    }
}
