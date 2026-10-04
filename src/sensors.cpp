#include <Arduino.h>
#include <DHT.h>

#include "config.h"
#include "sensors.h"

// ========================================
// SENSOR ACTIVE LEVELS
// ========================================

#define PIR_ACTIVE HIGH
#define LDR_DARK HIGH
#define FLAME_ACTIVE LOW

#define DHTTYPE DHT11

DHT dht(DHT_PIN, DHTTYPE);


// ========================================
// SENSOR VALUES
// ========================================

bool motion = false;
bool dark = false;
bool flame = false;

float temperature = NAN;
float humidity = NAN;


// ========================================
// DHT11 TIMING
// ========================================

unsigned long lastDHTRead = 0;

// Read DHT11 approximately every 2 seconds
const unsigned long DHT_READ_INTERVAL = 2000;


// ========================================
// INITIALIZE SENSORS
// ========================================

void initSensors()
{
    pinMode(PIR_PIN, INPUT_PULLUP);
    pinMode(LDR_PIN, INPUT_PULLUP);
    pinMode(FLAME_PIN, INPUT_PULLUP);
    dht.begin();

    lastDHTRead = 0;

    Serial.println("Sensors initialized");
}


// ========================================
// UPDATE SENSORS
// ========================================

void updateSensors()
{
    // ------------------------------------
    // Fast digital sensors
    // ------------------------------------

    motion = (digitalRead(PIR_PIN) == PIR_ACTIVE);

    dark = (digitalRead(LDR_PIN) == LDR_DARK);

    flame = (digitalRead(FLAME_PIN) == FLAME_ACTIVE);


    // ------------------------------------
    // DHT11
    // ------------------------------------

    if (millis() - lastDHTRead >= DHT_READ_INTERVAL)
    {
        lastDHTRead = millis();

        float newTemperature = dht.readTemperature();
        float newHumidity = dht.readHumidity();

        // Keep the previous valid value if
        // the current reading fails.

        if (!isnan(newTemperature))
        {
            temperature = newTemperature;
        }

        if (!isnan(newHumidity))
        {
            humidity = newHumidity;
        }
    }
}


// ========================================
// GET SENSOR VALUES
// ========================================

bool isMotionDetected()
{
    return motion;
}


bool isDark()
{
    return dark;
}


bool isFlameDetected()
{
    return flame;
}


float getTemperature()
{
    return temperature;
}


float getHumidity()
{
    return humidity;
}