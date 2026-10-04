#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <LittleFS.h>

#include "api.h"
#include "config.h"
#include "sensors.h"
#include "devices.h"
#include "modes.h"
#include "alarms.h"

WebServer server(API_PORT);

// ========================================
// CORS
// ========================================

void sendCORS()
{
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
}

// ========================================
// STATUS
// ========================================

void handleStatus()
{
    sendCORS();

    String json = "{";

    json += "\"device\":\"ESP32 Smart Home\",";
    json += "\"status\":\"online\",";
    
    // System information
    json += "\"uptime\":";
    json += String(millis());
    
    json += ",";
    
    json += "\"free_heap\":";
    json += String(ESP.getFreeHeap());
    
    json += ",";
    
    json += "\"firmware\":\"1.0.0\"";
    
    json += ",";
    
    json += "\"wifi_ssid\":\"";
    json += WIFI_AP_SSID;
    json += "\"";
    
    json += ",";
    
    json += "\"ip\":\"";
    json += WiFi.softAPIP().toString();
    json += "\"";
    

    // Mode
    json += ",";
    json += "\"mode\":\"";
    json += getModeName();
    json += "\",";

    // Sensors
    json += "\"temperature\":";

    if (isnan(getTemperature()))
        json += "null";
    else
        json += String(getTemperature(), 1);

    json += ",";

    json += "\"humidity\":";

    if (isnan(getHumidity()))
        json += "null";
    else
        json += String(getHumidity(), 1);

    json += ",";

    json += "\"motion\":";
    json += isMotionDetected() ? "true" : "false";

    json += ",";

    json += "\"dark\":";
    json += isDark() ? "true" : "false";

    json += ",";

    json += "\"flame\":";
    json += isFlameDetected() ? "true" : "false";

    json += ",";
    
    // Sensor status
    json += "\"pir_status\":\"";
    json += isMotionDetected() ? "ACTIVE" : "READY";
    json += "\",";
    
    json += "\"ldr_status\":\"";
    json += isDark() ? "DARK" : "BRIGHT";
    json += "\",";
    
    json += "\"flame_status\":\"";
    json += isFlameDetected() ? "FIRE" : "SAFE";
    json += "\",";
    
    json += "\"dht_status\":\"";
    json += isnan(getTemperature()) ? "FAULT" : "OK";
    json += "\",";

    // Devices
    json += "\"led1\":";
    json += getLED1() ? "true" : "false";

    json += ",";

    json += "\"led2\":";
    json += getLED2() ? "true" : "false";

    json += ",";

    json += "\"led3\":";
    json += getLED3() ? "true" : "false";

    json += ",";

    json += "\"fan\":";
    json += getFan() ? "true" : "false";

    json += ",";

    json += "\"buzzer\":";
    json += getBuzzer() ? "true" : "false";

    json += ",";

    // Alarm
    json += "\"alarm_active\":";
    json += isAlarmActive() ? "true" : "false";

    json += ",";

    json += "\"alarm_silenced\":";
    json += isAlarmSilenced() ? "true" : "false";

    json += ",";

    json += "\"alarm_type\":\"";

    AlarmType alarm = getAlarmType();

    if (alarm == ALARM_NONE)
        json += "NONE";
    else if (alarm == ALARM_MANUAL)
        json += "MANUAL";
    else if (alarm == ALARM_PIR)
        json += "PIR";
    else if (alarm == ALARM_FLAME)
        json += "FLAME";

    json += "\"";

    json += "}";

    server.send(
        200,
        "application/json",
        json
    );
}

// ========================================
// SET MODE
// ========================================

void handleSetMode()
{   Serial.println("MODE BODY: " + server.arg("plain"));
    
    sendCORS();

    String body = server.arg("plain");

    if (body.indexOf("MANUAL") >= 0)
    {
        setMode(MODE_MANUAL);
    }
    else if (body.indexOf("SECURITY") >= 0)
    {
        setMode(MODE_SECURITY);
    }
    else if (body.indexOf("AUTO") >= 0)
    {
        setMode(MODE_AUTO);
    }
    else
    {
        server.send(
            400,
            "application/json",
            "{\"success\":false,\"error\":\"Invalid mode\"}"
        );
        return;
    }

    String json = "{";
    json += "\"success\":true,";
    json += "\"mode\":\"";
    json += getModeName();
    json += "\"";
    json += "}";

    server.send(
        200,
        "application/json",
        json
    );
}

// ========================================
// LIGHT CONTROL
// ========================================

void handleLights()
{
    sendCORS();

    if (getMode() != MODE_MANUAL)
    {
        server.send(
            403,
            "application/json",
            "{\"success\":false,\"error\":\"Lights can be controlled manually only in MANUAL mode\"}"
        );
        return;
    }

    String body = server.arg("plain");

    if (body.indexOf("\"led1\":true") >= 0)
        setLED1(true);
    else if (body.indexOf("\"led1\":false") >= 0)
        setLED1(false);

    if (body.indexOf("\"led2\":true") >= 0)
        setLED2(true);
    else if (body.indexOf("\"led2\":false") >= 0)
        setLED2(false);

    if (body.indexOf("\"led3\":true") >= 0)
        setLED3(true);
    else if (body.indexOf("\"led3\":false") >= 0)
        setLED3(false);

    String json = "{";
    json += "\"success\":true,";
    json += "\"led1\":";
    json += getLED1() ? "true" : "false";
    json += ",";
    json += "\"led2\":";
    json += getLED2() ? "true" : "false";
    json += ",";
    json += "\"led3\":";
    json += getLED3() ? "true" : "false";
    json += "}";

    server.send(
        200,
        "application/json",
        json
    );
}

// ========================================
// FAN CONTROL
// ========================================

void handleFan()
{
    sendCORS();

    if (getMode() != MODE_MANUAL)
    {
        server.send(
            403,
            "application/json",
            "{\"success\":false,\"error\":\"Fan can be controlled manually only in MANUAL mode\"}"
        );
        return;
    }

    String body = server.arg("plain");

    if (body.indexOf("\"fan\":true") >= 0)
    {
        setFan(true);
    }
    else if (body.indexOf("\"fan\":false") >= 0)
    {
        setFan(false);
    }
    else
    {
        server.send(
            400,
            "application/json",
            "{\"success\":false,\"error\":\"Invalid fan command\"}"
        );
        return;
    }

    String json = "{";
    json += "\"success\":true,";
    json += "\"fan\":";
    json += getFan() ? "true" : "false";
    json += "}";

    server.send(
        200,
        "application/json",
        json
    );
}

// ========================================
// MANUAL BUZZER
// ========================================

void handleManualBuzzer()
{
    sendCORS();

    if (getMode() != MODE_MANUAL)
    {
        server.send(
            403,
            "application/json",
            "{\"success\":false,\"error\":\"Manual buzzer control is available only in MANUAL mode\"}"
        );
        return;
    }

    String body = server.arg("plain");

    if (body.indexOf("\"buzzer\":true") >= 0)
    {
        setManualBuzzer(true);
    }
    else if (body.indexOf("\"buzzer\":false") >= 0)
    {
        setManualBuzzer(false);
    }
    else
    {
        server.send(
            400,
            "application/json",
            "{\"success\":false,\"error\":\"Invalid buzzer command\"}"
        );
        return;
    }

    String json = "{";
    json += "\"success\":true,";
    json += "\"buzzer\":";
    json += getBuzzer() ? "true" : "false";
    json += "}";

    server.send(
        200,
        "application/json",
        json
    );
}

// ========================================
// SILENCE ALARM
// ========================================

void handleAlarmSilence()
{
    sendCORS();

    silenceAlarm();

    server.send(
        200,
        "application/json",
        "{\"success\":true,\"alarm_silenced\":true}"
    );
}

// ========================================
// OPTIONS / CORS
// ========================================

void handleOptions()
{
    sendCORS();

    server.send(
        204,
        "text/plain",
        ""
    );
}

// ========================================
// INITIALIZE API
// ========================================

void initAPI()
{   
    // --------------------------------
    // Initialize LittleFS
    // --------------------------------

    if (!LittleFS.begin(true))
    {
        Serial.println("ERROR: LittleFS mount failed");
        return;
    }

    Serial.println("LittleFS mounted successfully");
    WiFi.mode(WIFI_AP);
    bool result = WiFi.softAP(
        WIFI_AP_SSID,
        WIFI_AP_PASSWORD
    );
    

    if (!result)
    {
        Serial.println("ERROR: Failed to start Wi-Fi AP");
        return;
    }

    Serial.println();
    Serial.println("================================");
    Serial.println("ESP32 SMART HOME API");
    Serial.println("================================");

    Serial.print("Wi-Fi SSID: ");
    Serial.println(WIFI_AP_SSID);

    Serial.print("IP Address: ");
    Serial.println(WiFi.softAPIP());

    // -------------------------------
    // GET
    // -------------------------------

    server.on(
        "/api/status",
        HTTP_GET,
        handleStatus
    );

    // -------------------------------
    // POST
    // -------------------------------

    server.on(
        "/api/mode",
        HTTP_POST,
        handleSetMode
    );

    server.on(
        "/api/lights",
        HTTP_POST,
        handleLights
    );

    server.on(
        "/api/fan",
        HTTP_POST,
        handleFan
    );

    server.on(
        "/api/buzzer",
        HTTP_POST,
        handleManualBuzzer
    );

    server.on(
        "/api/alarm/silence",
        HTTP_POST,
        handleAlarmSilence
    );

    // -------------------------------
    // OPTIONS
    // -------------------------------

    server.on(
        "/api/status",
        HTTP_OPTIONS,
        handleOptions
    );

    server.on(
        "/api/mode",
        HTTP_OPTIONS,
        handleOptions
    );

    server.on(
        "/api/lights",
        HTTP_OPTIONS,
        handleOptions
    );

    server.on(
        "/api/fan",
        HTTP_OPTIONS,
        handleOptions
    );

    server.on(
        "/api/buzzer",
        HTTP_OPTIONS,
        handleOptions
    );

    server.on(
        "/api/alarm/silence",
        HTTP_OPTIONS,
        handleOptions
    );
    
   server.serveStatic("/style.css", LittleFS, "/style.css");
   server.serveStatic("/app.js", LittleFS, "/app.js");

   server.on("/", HTTP_GET, []()
   {
      File file = LittleFS.open("/index.html", "r");

      if (!file)
      {
        server.send(404, "text/plain", "index.html not found");
        return;
      }

     server.streamFile(file, "text/html");
     file.close();
    });
    server.begin();

    Serial.println("API server started");
    Serial.println();
    Serial.println("GET  /api/status");
    Serial.println("POST /api/mode");
    Serial.println("POST /api/lights");
    Serial.println("POST /api/fan");
    Serial.println("POST /api/buzzer");
    Serial.println("POST /api/alarm/silence");
    Serial.println("================================");
}

// ========================================
// HANDLE API
// ========================================

void handleAPI()
{
    server.handleClient();
}