#pragma once

void initSensors();
void updateSensors();

bool isMotionDetected();
bool isDark();
bool isFlameDetected();

float getTemperature();
float getHumidity();