#include <Arduino.h>

#include "devices.h"
#include "config.h"

// ========================================
// DEVICE STATES
// ========================================

bool led1State = false;
bool led2State = false;
bool led3State = false;
bool fanState = false;
bool buzzerState = false;


// ========================================
// INITIALIZE DEVICES
// ========================================

void initDevices()
{
    pinMode(LED1_PIN, OUTPUT);
    pinMode(LED2_PIN, OUTPUT);
    pinMode(LED3_PIN, OUTPUT);

    pinMode(FAN_PIN, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);

    digitalWrite(LED1_PIN, LOW);
    digitalWrite(LED2_PIN, LOW);
    digitalWrite(LED3_PIN, LOW);

    digitalWrite(FAN_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);

    Serial.println("Devices initialized");
}


// ========================================
// LED CONTROL
// ========================================

void setLED1(bool state)
{
    led1State = state;
    digitalWrite(LED1_PIN, state ? HIGH : LOW);
}

void setLED2(bool state)
{
    led2State = state;
    digitalWrite(LED2_PIN, state ? HIGH : LOW);
}

void setLED3(bool state)
{
    led3State = state;
    digitalWrite(LED3_PIN, state ? HIGH : LOW);
}


// ========================================
// FAN
// ========================================

void setFan(bool state)
{
    fanState = state;
    digitalWrite(FAN_PIN, state ? HIGH : LOW);
}


// ========================================
// BUZZER
// ========================================

void setBuzzer(bool state)
{
    buzzerState = state;
    digitalWrite(BUZZER_PIN, state ? HIGH : LOW);
}


// ========================================
// DEVICE STATUS
// ========================================

bool getLED1()
{
    return led1State;
}

bool getLED2()
{
    return led2State;
}

bool getLED3()
{
    return led3State;
}

bool getFan()
{
    return fanState;
}

bool getBuzzer()
{
    return buzzerState;
}