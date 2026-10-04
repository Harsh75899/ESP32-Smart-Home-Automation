#pragma once

enum SystemMode
{
    MODE_AUTO,
    MODE_MANUAL,
    MODE_SECURITY
};

void initModes();
void updateMode();

void setMode(SystemMode mode);
SystemMode getMode();

const char* getModeName();