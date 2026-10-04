#pragma once

#include <Arduino.h>

void initLCD();

void lcdShow(const String& line1, const String& line2);

void updateLCD();