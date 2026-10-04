#pragma once

enum AlarmType
{
    ALARM_NONE,
    ALARM_MANUAL,
    ALARM_PIR,
    ALARM_FLAME
};

void initAlarms();
void updateAlarms();

void setManualBuzzer(bool state);
void silenceAlarm();
void resetAlarms();

AlarmType getAlarmType();
bool isAlarmActive();
bool isAlarmSilenced();