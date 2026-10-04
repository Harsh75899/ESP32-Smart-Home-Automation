#include <Arduino.h>

#include "modes.h"
#include "sensors.h"
#include "devices.h"
#include "animations.h"
#include "alarms.h"

// ========================================
// MODE STATE
// ========================================

SystemMode currentMode = MODE_AUTO;

// ========================================
// AUTO LIGHT TIMER
// ========================================

unsigned long lightTimerStart = 0;
bool autoLightActive = false;

const unsigned long LIGHT_ON_TIME = 10000;

// ========================================
// PIR LED ANIMATION TIMER
// ========================================

unsigned long pirAnimationStart = 0;
bool pirAnimationActive = false;

const unsigned long PIR_ANIMATION_TIME = 5000;

// ========================================
// AUTO FAN
// ========================================

const float FAN_ON_TEMPERATURE = 30.0;

// ========================================
// INITIALIZE
// ========================================

void initModes()
{
    currentMode = MODE_AUTO;

    lightTimerStart = 0;
    autoLightActive = false;

    pirAnimationStart = 0;
    pirAnimationActive = false;

    setLED1(false);
    setLED2(false);
    setLED3(false);
    setFan(false);

    setAnimation(ANIMATION_NONE);

    Serial.println("Mode system initialized");
    Serial.println("Current mode: AUTO");
}

// ========================================
// UPDATE MODE
// ========================================

void updateMode()
{
    // ====================================
    // AUTO MODE
    // ====================================

    if (currentMode == MODE_AUTO)
    {
        // -------------------------------
        // FIRE ALARM
        // -------------------------------

        if (isFlameDetected())
        {
            setAnimation(ANIMATION_FLAME_ALARM);
        }
        else
        {
            // Fire has cleared.
            // Resume normal AUTO operation.

            if (autoLightActive)
            {
                setAnimation(ANIMATION_AUTO_LIGHT);
            }
            else
            {
                setAnimation(ANIMATION_NONE);
            }

            // -------------------------------
            // AUTO LIGHT
            // -------------------------------

            if (isDark() && isMotionDetected())
            {
                if (!autoLightActive)
                {
                    autoLightActive = true;
                    lightTimerStart = millis();

                    setAnimation(ANIMATION_AUTO_LIGHT);
                }
            }

            // -------------------------------
            // 10 SECOND LIGHT TIMER
            // -------------------------------

            if (autoLightActive)
            {
                if (millis() - lightTimerStart >= LIGHT_ON_TIME)
                {
                    if (isMotionDetected() && isDark())
                    {
                        lightTimerStart = millis();
                    }
                    else
                    {
                        autoLightActive = false;

                        setAnimation(ANIMATION_NONE);
                    }
                }
            }
        }

        // -------------------------------
        // AUTO FAN
        // -------------------------------

        if (getTemperature() >= FAN_ON_TEMPERATURE)
        {
            setFan(true);
        }
        else
        {
            setFan(false);
        }
    }

    // ====================================
    // MANUAL MODE
    // ====================================

    else if (currentMode == MODE_MANUAL)
    {
        // Fire alarm has priority.
        if (isFlameDetected())
        {
            setAnimation(ANIMATION_FLAME_ALARM);
        }
        else
        {
            // Fire cleared → stop fire animation.
            setAnimation(ANIMATION_NONE);
        }

        // LEDs and fan are otherwise controlled
        // directly through the phone/API.
    }

    // ====================================
    // SECURITY MODE
    // ====================================

    else if (currentMode == MODE_SECURITY)
    {
        // Fan is always OFF.
        setFan(false);

        // --------------------------------
        // FIRE HAS HIGHEST PRIORITY
        // --------------------------------

        if (isFlameDetected())
        {
            setAnimation(ANIMATION_FLAME_ALARM);

            // Cancel PIR animation timer.
            pirAnimationActive = false;
        }

        // --------------------------------
        // PIR INTRUSION
        // --------------------------------

        else
        {
            // Start PIR animation timer
            // only once when motion is detected.
            if (isMotionDetected() && !pirAnimationActive)
            {
                pirAnimationActive = true;
                pirAnimationStart = millis();

                setAnimation(ANIMATION_PIR_ALARM);
            }

            // Keep PIR animation running
            // for the full 5 seconds.
            if (pirAnimationActive)
            {
                if (millis() - pirAnimationStart >= PIR_ANIMATION_TIME)
                {
                    pirAnimationActive = false;

                    setAnimation(ANIMATION_NONE);
                }
                else
                {
                    setAnimation(ANIMATION_PIR_ALARM);
                }
            }
            else
            {
                setAnimation(ANIMATION_NONE);
            }
        }
    }
}

// ========================================
// SET MODE
// ========================================

void setMode(SystemMode mode)
{
    // Turn everything OFF first
    setLED1(false);
    setLED2(false);
    setLED3(false);
    setFan(false);

    // Stop animations
    setAnimation(ANIMATION_NONE);

    // Reset AUTO timer
    autoLightActive = false;
    lightTimerStart = 0;

    // Reset PIR animation timer
    pirAnimationActive = false;
    pirAnimationStart = 0;

    // Reset alarm system
    resetAlarms();

    // Finally change mode
    currentMode = mode;

    Serial.print("Mode changed to: ");
    Serial.println(getModeName());
}

// ========================================
// GET MODE
// ========================================

SystemMode getMode()
{
    return currentMode;
}

// ========================================
// MODE NAME
// ========================================

const char* getModeName()
{
    switch (currentMode)
    {
        case MODE_AUTO:
            return "AUTO";

        case MODE_MANUAL:
            return "MANUAL";

        case MODE_SECURITY:
            return "SECURITY";

        default:
            return "UNKNOWN";
    }
}