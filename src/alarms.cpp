#include <Arduino.h>

#include "alarms.h"
#include "sensors.h"
#include "devices.h"
#include "modes.h"

AlarmType currentAlarm = ALARM_NONE;
AlarmType previousAlarm = ALARM_NONE;

bool manualBuzzer = false;
bool alarmSilenced = false;

unsigned long lastBuzzerChange = 0;
bool buzzerOutput = false;


// ========================================
// BUZZER TIMING
// ========================================

// PIR alarm
const unsigned long PIR_ON_TIME = 200;
const unsigned long PIR_OFF_TIME = 300;
// PIR alarm minimum duration
const unsigned long PIR_MIN_ALARM_TIME = 5000;
unsigned long pirAlarmStart = 0;
// Flame alarm
const unsigned long FLAME_ON_TIME = 500;
const unsigned long FLAME_OFF_TIME = 100;

// Auto-light confirmation beep
const unsigned long AUTO_BEEP_TIME = 150;

// Sensor warning
const unsigned long SENSOR_BEEP_ON = 150;
const unsigned long SENSOR_BEEP_OFF = 150;
const unsigned long SENSOR_WARNING_INTERVAL = 10000;


// ========================================
// AUTO LIGHT BEEP STATE
// ========================================

bool previousAutoLightTrigger = false;
bool autoBeepActive = false;
unsigned long autoBeepStart = 0;


// ========================================
// SENSOR WARNING STATE
// ========================================

unsigned long lastSensorWarning = 0;
int sensorBeepCount = 0;
bool sensorWarningActive = false;


// ========================================
// INITIALIZE
// ========================================

void initAlarms()
{pirAlarmStart = 0;
    currentAlarm = ALARM_NONE;
    previousAlarm = ALARM_NONE;

    manualBuzzer = false;
    alarmSilenced = false;

    buzzerOutput = false;
    lastBuzzerChange = millis();

    previousAutoLightTrigger = false;
    autoBeepActive = false;
    autoBeepStart = 0;

    lastSensorWarning = millis();
    sensorBeepCount = 0;
    sensorWarningActive = false;

    setBuzzer(false);

    Serial.println("Alarm system initialized");
}


// ========================================
// UPDATE ALARMS
// ========================================

void updateAlarms()
{
    SystemMode mode = getMode();

    // ====================================
    // DETERMINE CURRENT ALARM
    // ====================================

    if (mode == MODE_MANUAL)
    {
        // Flame always has highest priority
        if (isFlameDetected())
        {
            currentAlarm = ALARM_FLAME;
        }
        else if (manualBuzzer)
        {
            currentAlarm = ALARM_MANUAL;
        }
        else
        {
            currentAlarm = ALARM_NONE;
        }
    }

    else if (mode == MODE_SECURITY)
  {
    // Flame has highest priority
    if (isFlameDetected())
    {
        currentAlarm = ALARM_FLAME;
    }

    // PIR detected
    else if (isMotionDetected())
    {
        // Start a new PIR alarm timer
        if (currentAlarm != ALARM_PIR)
        {
            pirAlarmStart = millis();
        }

        currentAlarm = ALARM_PIR;
    }

    // PIR went LOW, but minimum 5 seconds
    // has not completed yet
    else if (currentAlarm == ALARM_PIR &&
             millis() - pirAlarmStart < PIR_MIN_ALARM_TIME)
    {
        currentAlarm = ALARM_PIR;
    }

    // PIR alarm can finally clear
    else
    {
        currentAlarm = ALARM_NONE;
    }
  }

    else if (mode == MODE_AUTO)
    {
        // Flame has priority in AUTO
        if (isFlameDetected())
        {
            currentAlarm = ALARM_FLAME;
        }
        else
        {
            currentAlarm = ALARM_NONE;
        }
    }


    // ====================================
    // NEW ALARM DETECTED
    // ====================================

   if (currentAlarm != previousAlarm)
  {
    alarmSilenced = false;

    previousAlarm = currentAlarm;

    lastBuzzerChange = millis();

    // Start a new flame alarm immediately
    if (currentAlarm == ALARM_FLAME)
    {
        buzzerOutput = true;
        setBuzzer(true);
    }
    else
    {
        buzzerOutput = false;
        setBuzzer(false);
    }
   }
    // ====================================
    // FIRE / PIR / MANUAL ALARMS
    // These have priority over everything
    // ====================================

    if (currentAlarm == ALARM_MANUAL)
    {
        setBuzzer(true);
        return;
    }

// ====================================
// FLAME ALARM
// ====================================

    if (currentAlarm == ALARM_FLAME)
    {
    // Alarm has been silenced by the user
    if (alarmSilenced)
    {
        setBuzzer(false);
        buzzerOutput = false;
        return;
    }

    unsigned long interval;

    if (buzzerOutput)
        interval = FLAME_ON_TIME;
    else
        interval = FLAME_OFF_TIME;

    if (millis() - lastBuzzerChange >= interval)
    {
        buzzerOutput = !buzzerOutput;

        lastBuzzerChange = millis();

        setBuzzer(buzzerOutput);
    }

    return;
    }

    if (currentAlarm == ALARM_PIR)
    {
        if (alarmSilenced)
        {
            setBuzzer(false);
            return;
        }

        unsigned long interval;

        if (buzzerOutput)
            interval = PIR_ON_TIME;
        else
            interval = PIR_OFF_TIME;

        if (millis() - lastBuzzerChange >= interval)
        {
            buzzerOutput = !buzzerOutput;
            lastBuzzerChange = millis();

            setBuzzer(buzzerOutput);
        }

        return;
    }


    // ====================================
    // NO AUTOMATIC ALARM
    // ====================================

    setBuzzer(false);
    buzzerOutput = false;


    // ====================================
    // AUTO LIGHT CONFIRMATION BEEP
    // ====================================

    if (mode == MODE_AUTO)
    {
        bool autoLightTrigger =
            isDark() && isMotionDetected();

        // Detect only the moment the condition
        // changes from false → true.
        if (autoLightTrigger && !previousAutoLightTrigger)
        {
            autoBeepActive = true;
            autoBeepStart = millis();

            setBuzzer(true);
        }

        previousAutoLightTrigger = autoLightTrigger;

        // Keep the confirmation beep short.
        if (autoBeepActive)
        {
            if (millis() - autoBeepStart >= AUTO_BEEP_TIME)
            {
                autoBeepActive = false;
                setBuzzer(false);
            }
        }
    }
    else
    {
        previousAutoLightTrigger = false;
        autoBeepActive = false;
    }


    // ====================================
    // DHT SENSOR FAILURE WARNING
    // ====================================

    bool sensorError =
        isnan(getTemperature()) ||
        isnan(getHumidity());

    if (sensorError && mode != MODE_SECURITY)
    {
        // Start warning every 10 seconds
        if (!sensorWarningActive &&
            millis() - lastSensorWarning >= SENSOR_WARNING_INTERVAL)
        {
            sensorWarningActive = true;
            sensorBeepCount = 0;

            lastBuzzerChange = millis();

            setBuzzer(true);
            buzzerOutput = true;
        }

        if (sensorWarningActive)
        {
            unsigned long elapsed =
                millis() - lastBuzzerChange;

            if (buzzerOutput &&
                elapsed >= SENSOR_BEEP_ON)
            {
                setBuzzer(false);
                buzzerOutput = false;

                lastBuzzerChange = millis();

                sensorBeepCount++;
            }
            else if (!buzzerOutput &&
                     elapsed >= SENSOR_BEEP_OFF)
            {
                if (sensorBeepCount >= 3)
                {
                    sensorWarningActive = false;
                    lastSensorWarning = millis();
                }
                else
                {
                    setBuzzer(true);
                    buzzerOutput = true;

                    lastBuzzerChange = millis();
                }
            }
        }
    }
    else
    {
        sensorWarningActive = false;
        sensorBeepCount = 0;
    }
}


// ========================================
// MANUAL BUZZER CONTROL
// ========================================

void setManualBuzzer(bool state)
{
    manualBuzzer = state;

    if (state)
    {
        alarmSilenced = false;
        buzzerOutput = true;

        setBuzzer(true);
    }
    else
    {
        buzzerOutput = false;

        setBuzzer(false);
    }
}


// ========================================
// SILENCE CURRENT ALARM
// ========================================

void silenceAlarm()
{
    if (currentAlarm == ALARM_PIR ||
        currentAlarm == ALARM_FLAME)
    {
        alarmSilenced = true;

        buzzerOutput = false;

        setBuzzer(false);
    }
}


// ========================================
// RESET ALARM SYSTEM
// ========================================

void resetAlarms()
{pirAlarmStart = 0;
    currentAlarm = ALARM_NONE;
    previousAlarm = ALARM_NONE;

    manualBuzzer = false;
    alarmSilenced = false;

    buzzerOutput = false;
    lastBuzzerChange = millis();

    previousAutoLightTrigger = false;
    autoBeepActive = false;

    sensorWarningActive = false;
    sensorBeepCount = 0;
    lastSensorWarning = millis();

    setBuzzer(false);
}


// ========================================
// GET ALARM TYPE
// ========================================

AlarmType getAlarmType()
{
    return currentAlarm;
}


// ========================================
// CHECK ALARM
// ========================================

bool isAlarmActive()
{
    return currentAlarm != ALARM_NONE;
}


// ========================================
// CHECK SILENCE
// ========================================

bool isAlarmSilenced()
{
    return alarmSilenced;
}