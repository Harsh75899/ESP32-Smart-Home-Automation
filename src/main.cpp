#include <Arduino.h>
#include "api.h"
#include "sensors.h"
#include "devices.h"
#include "modes.h"
#include "alarms.h"
#include "animations.h"
#include "lcd.h"


// ========================================
// SETUP
// ========================================

void setup()
{ 
    Serial.begin(115200);

    delay(1000);

    Serial.println();
    Serial.println("================================");
    Serial.println("ESP32 SMART HOME");
    Serial.println("SYSTEM STARTING");
    Serial.println("================================");


    // ------------------------------------
    // Initialize sensors
    // ------------------------------------

    initSensors();


    // ------------------------------------
    // Initialize hardware devices
    // ------------------------------------

    initDevices();

    initLCD();


    // ------------------------------------
    // Initialize operating modes
    // ------------------------------------

    initModes();


    // ------------------------------------
    // Initialize alarm system
    // ------------------------------------

    initAlarms();


    // ------------------------------------
    // Initialize LED animations
    // ------------------------------------

    initAnimations();


    // ------------------------------------
    // Start Wi-Fi + REST API
    // ------------------------------------

    initAPI();


    Serial.println();
    Serial.println("================================");
    Serial.println("SYSTEM READY");
    Serial.println("================================");
}


// ========================================
// MAIN LOOP
// ========================================

void loop()
{
    // ------------------------------------
    // 1. Read sensors
    // ------------------------------------

    updateSensors();


    // ------------------------------------
    // 2. Process operating mode
    // ------------------------------------

    updateMode();


    // ------------------------------------
    // 3. Process alarms
    // ------------------------------------

    updateAlarms();


    // ------------------------------------
    // 4. Run LED animations
    // ------------------------------------

    updateAnimations();


    // ------------------------------------
    // 5. Update LCD
    // ------------------------------------

    updateLCD();


    // ------------------------------------
    // 6. Handle REST API requests
    // ------------------------------------

    handleAPI();


    // Small delay prevents excessive
    // CPU/API polling.

    delay(10);
}