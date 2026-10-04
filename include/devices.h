#pragma once

#include <Arduino.h>

void initDevices();

// -------- LEDs --------

void setLED1(bool state);
void setLED2(bool state);
void setLED3(bool state);

// -------- Fan --------

void setFan(bool state);

// -------- Buzzer --------

void setBuzzer(bool state);

// -------- Status --------

bool getLED1();
bool getLED2();
bool getLED3();
bool getFan();
bool getBuzzer();