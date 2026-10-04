#pragma once

// ================================
// ESP32 SMART HOME CONFIGURATION
// ================================

// -------- Wi-Fi Access Point --------

#define WIFI_AP_SSID     "ESP32-SmartHome"
#define WIFI_AP_PASSWORD "12345678"

// -------- API --------

#define API_PORT 80

// -------- Sensors --------

#define PIR_PIN     27
#define LDR_PIN     26
#define DHT_PIN     4
#define FLAME_PIN   25

// -------- Outputs --------

#define LED1_PIN    16
#define LED2_PIN    17
#define LED3_PIN    18

#define FAN_PIN     19
#define BUZZER_PIN  23

// -------- I2C LCD --------

#define LCD_SDA     21
#define LCD_SCL     22