#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include "lcd.h"
#include "sensors.h"
#include "modes.h"
#include "devices.h"
#include "config.h"

// ========================================
// LCD
// ========================================

LiquidCrystal_I2C lcd(0x27, 16, 2);


// ========================================
// LCD UPDATE TIMING
// ========================================

// Animation update speed
const unsigned long LCD_UPDATE_INTERVAL = 800;

// How long each normal screen stays visible
const unsigned long NORMAL_SCREEN_INTERVAL = 3000;

unsigned long lastLCDUpdate = 0;
unsigned long lastNormalScreenChange = 0;
unsigned long securityDisplayStart = 0;
bool securityDisplayActive = false;

const unsigned long SECURITY_DISPLAY_TIME = 5000;


// ========================================
// NORMAL SCREEN STATE
// ========================================

// 0 = Home
// 1 = Environment
// 2 = Devices
// 3 = System

int lcdScreen = 0;

const int TOTAL_NORMAL_SCREENS = 4;


// ========================================
// ANIMATION STATE
// ========================================

int lcdAnimationStep = 0;


// ========================================
// STARTUP STATE
// ========================================

bool startupActive = true;

unsigned long startupStartTime = 0;

int startupStep = 0;

const unsigned long STARTUP_STEP_TIME = 300;


// ========================================
// CUSTOM CHARACTERS
// ========================================


// ----------------------------------------
// Slot 0 - Flame frame 1
// ----------------------------------------

byte flame1[8] =
{
    B00100,
    B01110,
    B01110,
    B00100,
    B01100,
    B11100,
    B01110,
    B00100
};


// ----------------------------------------
// Slot 1 - Flame frame 2
// ----------------------------------------

byte flame2[8] =
{
    B00100,
    B01110,
    B00100,
    B01110,
    B11100,
    B01110,
    B00100,
    B00000
};


// ----------------------------------------
// Slot 2 - Warning
// ----------------------------------------

byte warningChar[8] =
{
    B00100,
    B01110,
    B01110,
    B11111,
    B11111,
    B00100,
    B00000,
    B00100
};


// ----------------------------------------
// Slot 3 - Motion / Person
// ----------------------------------------

byte motionChar[8] =
{
    B00100,
    B01110,
    B00100,
    B01110,
    B10101,
    B00100,
    B01010,
    B10001
};


// ----------------------------------------
// Slot 4 - Filled block
// ----------------------------------------

byte blockChar[8] =
{
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111
};


// ----------------------------------------
// Slot 5 - Empty block
// ----------------------------------------

byte emptyChar[8] =
{
    B10001,
    B00000,
    B00000,
    B00000,
    B00000,
    B00000,
    B00000,
    B10001
};


// ----------------------------------------
// Slot 6 - HOUSE
//
// Taken from your 5x8 house image
// ----------------------------------------

byte houseChar[8] =
{
    B00000,
    B00100,
    B01110,
    B11111,
    B01110,
    B01010,
    B01010,
    B11111
};


// ----------------------------------------
// Slot 7 - WI-FI
//
// Taken from your 5x8 Wi-Fi image
// ----------------------------------------

byte wifiChar[8] =
{
    B00000,
    B11111,
    B00000,
    B01110,
    B00000,
    B00100,
    B00000,
    B00000
};


// ========================================
// LCD INITIALIZATION
// ========================================

void initLCD()
{
    Wire.begin(LCD_SDA, LCD_SCL);

    lcd.init();

    lcd.backlight();

    lcd.clear();


    // ------------------------------------
    // Load all 8 custom characters
    // ------------------------------------

    lcd.createChar(0, flame1);
    lcd.createChar(1, flame2);
    lcd.createChar(2, warningChar);
    lcd.createChar(3, motionChar);
    lcd.createChar(4, blockChar);
    lcd.createChar(5, emptyChar);
    lcd.createChar(6, houseChar);
    lcd.createChar(7, wifiChar);


    // ------------------------------------
    // Reset LCD state
    // ------------------------------------

    startupActive = true;

    startupStartTime = millis();

    startupStep = 0;

    lcdScreen = 0;

    lcdAnimationStep = 0;

    lastLCDUpdate = millis();

    lastNormalScreenChange = millis();


    // ------------------------------------
    // Initial startup display
    // ------------------------------------

    lcd.clear();

    lcd.setCursor(0, 0);

    lcd.write(byte(6));
    lcd.print(" SMART HOME");

    lcd.setCursor(0, 1);

    lcd.print("   STARTING...");
}


// ========================================
// PRINT A CLEAN 16-CHARACTER LINE
// ========================================

void lcdPrintLine(int row, String text)
{
    if (text.length() > 16)
    {
        text = text.substring(0, 16);
    }

    lcd.setCursor(0, row);

    lcd.print(text);

    // Clear unused characters
    for (int i = text.length(); i < 16; i++)
    {
        lcd.print(" ");
    }
}


// ========================================
// SIMPLE LCD DISPLAY
// ========================================

void lcdShow(const String& line1, const String& line2)
{
    lcd.clear();

    lcdPrintLine(0, line1);

    lcdPrintLine(1, line2);
}


// ========================================
// STARTUP ANIMATION
// ========================================

void updateStartupLCD()
{
    unsigned long now = millis();

    if (now - startupStartTime < STARTUP_STEP_TIME)
    {
        return;
    }

    startupStartTime = now;


    // ------------------------------------
    // STEP 0
    // ------------------------------------

    if (startupStep == 0)
    {
        lcd.clear();

        lcd.setCursor(0, 0);

        lcd.write(byte(6));
        lcd.print(" SMART HOME");

        lcdPrintLine(1, "SYSTEM START");

        startupStep++;

        return;
    }


    // ------------------------------------
    // STEP 1
    // ------------------------------------

    if (startupStep == 1)
    {
        lcd.clear();

        lcd.setCursor(0, 0);

        lcd.write(byte(6));
        lcd.print(" SMART HOME");

        lcd.setCursor(0, 1);

        lcd.print("BOOT ");

        for (int i = 0; i < 3; i++)
        {
            lcd.write(byte(4));
        }

        startupStep++;

        return;
    }


    // ------------------------------------
    // STEP 2
    // ------------------------------------

    if (startupStep == 2)
    {
        lcd.clear();

        lcd.setCursor(0, 0);

        lcd.write(byte(6));
        lcd.print(" SMART HOME");

        lcd.setCursor(0, 1);

        lcd.print("BOOT ");

        for (int i = 0; i < 6; i++)
        {
            lcd.write(byte(4));
        }

        startupStep++;

        return;
    }


    // ------------------------------------
    // STEP 3
    // ------------------------------------

    if (startupStep == 3)
    {
        lcd.clear();

        lcd.setCursor(0, 0);

        lcd.write(byte(6));
        lcd.print(" SMART HOME");

        lcd.setCursor(0, 1);

        lcd.print("BOOT ");

        for (int i = 0; i < 9; i++)
        {
            lcd.write(byte(4));
        }

        startupStep++;

        return;
    }


    // ------------------------------------
    // STEP 4
    // ------------------------------------

    if (startupStep == 4)
    {
        lcd.clear();

        lcd.setCursor(0, 0);

        lcd.write(byte(6));
        lcd.print(" SMART HOME");

        lcd.setCursor(0, 1);

        lcd.print("BOOT ");

        for (int i = 0; i < 12; i++)
        {
            lcd.write(byte(4));
        }

        startupStep++;

        return;
    }


    // ------------------------------------
    // STEP 5
    // ------------------------------------

    if (startupStep == 5)
    {
        lcd.clear();

        lcd.setCursor(0, 0);

        lcd.write(byte(7));
        lcd.print(" WIFI AP READY");

        lcdPrintLine(1, "SYSTEM CHECK...");

        startupStep++;

        return;
    }


    // ------------------------------------
    // STEP 6
    // ------------------------------------

    if (startupStep >= 6)
    {
        lcd.clear();

        lcd.setCursor(0, 0);

        lcd.write(byte(6));
        lcd.print(" SYSTEM READY");

        lcd.setCursor(0, 1);

        lcd.write(byte(7));
        lcd.print(" SMART HOME OK");

        startupActive = false;

        lcdScreen = 0;

        lcdAnimationStep = 0;

        lastNormalScreenChange = millis();

        return;
    }
}


// ========================================
// FIRE LCD
// ========================================

void updateFireLCD()
{
    lcd.clear();


    // ------------------------------------
    // Frame 1
    // ------------------------------------

    if (lcdAnimationStep % 2 == 0)
    {
        lcd.setCursor(0, 0);

        lcd.write(byte(0));
        lcd.print(" FIRE ALERT ");
        lcd.write(byte(0));

        lcdPrintLine(1, "!! EVACUATE !!");
    }


    // ------------------------------------
    // Frame 2
    // ------------------------------------

    else
    {
        lcd.setCursor(0, 0);

        lcd.write(byte(1));
        lcd.print(" FIRE ALERT ");
        lcd.write(byte(1));

        lcdPrintLine(1, "!! LEAVE NOW !!");
    }


    lcdAnimationStep++;

    if (lcdAnimationStep >= 2)
    {
        lcdAnimationStep = 0;
    }
}


// ========================================
// SECURITY LCD
// ========================================

void updateSecurityLCD()
{
    lcd.clear();


    // ------------------------------------
    // Frame 1
    // ------------------------------------

    if (lcdAnimationStep % 2 == 0)
    {
        lcd.setCursor(0, 0);

        lcd.write(byte(2));
        lcd.print(" INTRUSION ");
        lcd.write(byte(2));

        lcd.setCursor(0, 1);

        lcd.write(byte(3));
        lcd.print(" MOTION ");
        lcd.write(byte(3));
    }


    // ------------------------------------
    // Frame 2
    // ------------------------------------

    else
    {
        lcd.setCursor(0, 0);

        lcd.write(byte(2));
        lcd.print(" SECURITY ");
        lcd.write(byte(2));

        lcdPrintLine(1, "!!! ALERT !!!");
    }


    lcdAnimationStep++;

    if (lcdAnimationStep >= 2)
    {
        lcdAnimationStep = 0;
    }
}


// ========================================
// NORMAL SCREEN 0
// HOME
// ========================================

void showHomeScreen()
{
    // ------------------------------------
    // Top line
    // ------------------------------------

    lcd.setCursor(0, 0);

    lcd.write(byte(6));

    lcd.print(" SMART HOME");


    // ------------------------------------
    // Bottom line
    // ------------------------------------

    lcd.setCursor(0, 1);

    lcd.print("MODE:");

    String mode = getModeName();

    lcd.print(mode);


    // ------------------------------------
    // Animation indicator
    // ------------------------------------

    int used = 5 + mode.length();

    if (used < 15)
    {
        lcd.setCursor(15, 1);

        if (lcdAnimationStep % 2 == 0)
        {
            lcd.write(byte(4));
        }
        else
        {
            lcd.write(byte(5));
        }
    }
}


// ========================================
// NORMAL SCREEN 1
// ENVIRONMENT
// ========================================

void showEnvironmentScreen()
{
    float temp = getTemperature();

    float hum = getHumidity();


    // ------------------------------------
    // Temperature
    // ------------------------------------

    String line1 = "";

    if (isnan(temp))
    {
        line1 = "T:FAULT";
    }
    else
    {
        line1 = "T:";
        line1 += String(temp, 1);
        line1 += (char)223;
        line1 += "C";
    }


    // ------------------------------------
    // Humidity
    // ------------------------------------

    if (isnan(hum))
    {
        line1 += " H:FAULT";
    }
    else
    {
        line1 += " H:";
        line1 += String(hum, 0);
        line1 += "%";
    }

    lcdPrintLine(0, line1);


    // ------------------------------------
    // Alternate sensor information
    // ------------------------------------

    if (lcdAnimationStep % 2 == 0)
    {
        if (isDark())
        {
            lcdPrintLine(1, "LIGHT: DARK");
        }
        else
        {
            lcdPrintLine(1, "LIGHT: BRIGHT");
        }
    }

    else
    {
        if (isMotionDetected())
        {
            lcd.setCursor(0, 1);

            lcd.write(byte(3));
            lcd.print(" PIR: MOTION");
        }
        else
        {
            lcdPrintLine(1, "PIR: CLEAR");
        }
    }
}


// ========================================
// NORMAL SCREEN 2
// DEVICES
// ========================================

void showDeviceScreen()
{
    bool led1 = getLED1();
    bool led2 = getLED2();
    bool led3 = getLED3();

    bool fan = getFan();
    bool buzzer = getBuzzer();


    // ------------------------------------
    // LED state
    // ------------------------------------

    String line1 = "L:";

    line1 += led1 ? "1" : "0";
    line1 += led2 ? "1" : "0";
    line1 += led3 ? "1" : "0";

    line1 += " F:";
    line1 += fan ? "ON" : "OFF";


    lcdPrintLine(0, line1);


    // ------------------------------------
    // Alternate fan/buzzer information
    // ------------------------------------

    if (lcdAnimationStep % 2 == 0)
    {
        String line2 = "BUZZER:";

        line2 += buzzer ? "ON" : "OFF";

        lcdPrintLine(1, line2);
    }

    else
    {
        if (fan)
        {
            lcdPrintLine(1, "FAN: RUNNING");
        }
        else
        {
            lcdPrintLine(1, "FAN: STOPPED");
        }
    }
}


// ========================================
// NORMAL SCREEN 3
// SYSTEM
// ========================================

void showSystemScreen()
{
    float temp = getTemperature();


    // ------------------------------------
    // System header
    // ------------------------------------

    lcd.setCursor(0, 0);

    lcd.write(byte(7));
    lcd.print(" SYSTEM");

    // Animation on right
    lcd.setCursor(15, 0);

    if (lcdAnimationStep % 2 == 0)
    {
        lcd.write(byte(4));
    }
    else
    {
        lcd.write(byte(5));
    }


    // ------------------------------------
    // DHT status
    // ------------------------------------

    if (isnan(temp))
    {
        lcdPrintLine(1, "DHT: FAULT");
    }
    else
    {
        lcdPrintLine(1, "DHT: OK  SYSTEM OK");
    }
}


// ========================================
// NORMAL LCD UPDATE
// ========================================

void updateNormalLCD()
{
    unsigned long now = millis();


    // ------------------------------------
    // Change screen
    // ------------------------------------

    if (now - lastNormalScreenChange >= NORMAL_SCREEN_INTERVAL)
    {
        lastNormalScreenChange = now;

        lcdScreen++;

        if (lcdScreen >= TOTAL_NORMAL_SCREENS)
        {
            lcdScreen = 0;
        }

        lcd.clear();

        lcdAnimationStep = 0;
    }


    // ------------------------------------
    // Display selected screen
    // ------------------------------------

    if (lcdScreen == 0)
    {
        showHomeScreen();
    }

    else if (lcdScreen == 1)
    {
        showEnvironmentScreen();
    }

    else if (lcdScreen == 2)
    {
        showDeviceScreen();
    }

    else if (lcdScreen == 3)
    {
        showSystemScreen();
    }


    // ------------------------------------
    // Advance animation
    // ------------------------------------

    lcdAnimationStep++;

    if (lcdAnimationStep >= 2)
    {
        lcdAnimationStep = 0;
    }
}


// ========================================
// MAIN LCD UPDATE
// ========================================

void updateLCD()
{
    // ====================================
    // STARTUP
    // ====================================

    if (startupActive)
    {
        updateStartupLCD();

        return;
    }


    // ====================================
    // TIMING
    // ====================================

    unsigned long now = millis();

    if (now - lastLCDUpdate < LCD_UPDATE_INTERVAL)
    {
        return;
    }

    lastLCDUpdate = now;


    // ====================================
    // FIRE = HIGHEST PRIORITY
    // ====================================

    if (isFlameDetected())
    {
        updateFireLCD();

        return;
    }


    // ====================================
    // SECURITY + MOTION
    // ====================================
    
    if (getMode() == MODE_SECURITY)
    {
        if (isMotionDetected())
        {
            securityDisplayActive = true;
            securityDisplayStart = now;
        }
    
        if (securityDisplayActive)
        {
            updateSecurityLCD();
    
            if (now - securityDisplayStart >= SECURITY_DISPLAY_TIME)
            {
                securityDisplayActive = false;
            }
    
            return;
        }
    }

    // ====================================
    // NORMAL DISPLAY
    // ====================================

    updateNormalLCD();
}