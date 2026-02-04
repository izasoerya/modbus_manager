#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

#include "constants/modbus_object.h"
#include "services/modbus_service.h"

// ---------------- SENSOR OBJECTS ----------------
ModbusService windSpeedObject(&windConditionSensor);
ModbusService illuminanceObject(&illuminanceSensor);
ModbusService pyranoMeterObject(&pyranoMeterSensor);
ModbusService airQualityObject(&airQualitySensor);
ModbusService powerMeterObject(&powerMeterSensor);

// ---------------- NETWORK ----------------
const char *ssid = "WeatherStationAP";
const char *password = "WS1234567890";
const char *endpoint = "https://api.agrisync.site/api/v1/weather-station/reading";

const char *stationId = "2567dbd8-f385-4df2-977b-14bc671dad7e";

unsigned long lastRun = 30000;
unsigned long cycleInterval = 15000;

// ---------------- SENSOR VARIABLES ----------------
float windSpeed_ms;
float windDir_deg;
float rainfall_mm;
float light_lx;
float solar_Wm2;
float humidity_val;
float temperature_val;
float voltage_V;
float current_A;
float battery;
float pressure = 0;

// ---------------------------------------------------
void connectWiFi()
{

    Serial.println("[WiFi] Checking connection...");

    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println("[WiFi] Already connected.");
        return;
    }

    Serial.println("[WiFi] Connecting to AP...");
    WiFi.begin(ssid, password);

    unsigned long startAttempt = millis();

    while (WiFi.status() != WL_CONNECTED && millis() - startAttempt < 8000)
    {
        delay(500);
        Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println("\n[WiFi] Connection restored.");
        Serial.print("[WiFi] IP: ");
        Serial.println(WiFi.localIP());
    }
    else
    {
        Serial.println("\n[WiFi] Reconnect failed.");
    }
}

// ---------------------------------------------------
void readSensors()
{
    Serial.println("\n[READ] Starting sensor read sequence");

    Serial.println("[READ] Wind speed (m/s)...");
    windSpeed_ms = windSpeedObject.readSingle(0);
    // windSpeed_ms = 0;
    delay(1000);

    Serial.println("[READ] Wind direction (deg)...");
    windDir_deg = windSpeedObject.readSingle(3) / 10.0;
    // windDir_deg = 0;
    delay(1000);

    Serial.println("[READ] Rainfall (mm)...");
    rainfall_mm = illuminanceObject.readSingle(0);
    // rainfall_mm = 0;
    delay(1000);

    Serial.println("[READ] Light (lx)...");
    light_lx = illuminanceObject.readSingle(1);
    // light_lx = 0;
    delay(1000);

    Serial.println("[READ] Solar radiation (W/m2)...");
    solar_Wm2 = pyranoMeterObject.readSingle(0);
    // solar_Wm2 = 0;
    delay(1000);

    Serial.println("[READ] Humidity (%RH)...");
    humidity_val = airQualityObject.readSingle(0) / 10.0;
    // humidity_val = 0;
    delay(1000);

    Serial.println("[READ] Temperature (°C)...");
    temperature_val = airQualityObject.readSingle(1) / 10.0;
    // temperature_val = 0;
    delay(1000);

    Serial.println("[READ] System Voltage (V)...");
    voltage_V = powerMeterObject.readSingle(3) / 10.0;
    // voltage_V = 0;
    delay(1000);

    Serial.println("[READ] System Current (A)...");
    current_A = powerMeterObject.readSingle(4) / 10000.0;
    // current_A = 0;
    delay(1000);

    Serial.println("[READ] Battery indicator...");
    battery = powerMeterObject.readSingle(0) / 1000.0;
    // battery = 0;
    delay(1000);

    Serial.println("[READ] Read sequence complete.\n");
}

// ---------------------------------------------------
void sendJson()
{

    Serial.println("[SEND] Preparing to send JSON...");

    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("[SEND] Abort: WiFi offline.");
        return;
    }

    WiFiClientSecure client;
    client.setInsecure();

    HTTPClient http;
    Serial.println("[SEND] Starting HTTPS session...");
    http.begin(client, endpoint);

    http.addHeader("Content-Type", "application/json");

    String json = "{";
    json += "\"weatherStationId\":\"" + String(stationId) + "\",";
    json += "\"solarRadiation\":" + String(solar_Wm2) + ",";
    json += "\"temperature\":" + String(temperature_val) + ",";
    json += "\"humidity\":" + String(humidity_val) + ",";
    json += "\"rainFall\":" + String(rainfall_mm) + ",";
    json += "\"light\":" + String(light_lx) + ",";
    json += "\"pressure\":" + String(pressure) + ",";
    json += "\"windSpeed\":" + String(windSpeed_ms) + ",";
    json += "\"windDirection\":" + String(windDir_deg) + ",";
    json += "\"battery\":" + String(battery) + ",";
    json += "\"voltage\":" + String(voltage_V) + ",";
    json += "\"current\":" + String(current_A);
    json += "}";

    Serial.println("[SEND] JSON Payload:");
    Serial.println(json);

    Serial.println("[SEND] Sending POST request...");
    int code = http.POST(json);

    Serial.print("[SEND] HTTP Response Code: ");
    Serial.println(code);

    if (code > 0)
    {
        Serial.println("[SEND] Server Response:");
        Serial.println(http.getString());
    }

    http.end();
    Serial.println("[SEND] Transmission complete.\n");
}

// ---------------------------------------------------
void setup()
{
    Serial.begin(115200);

    delay(5000);
    Serial.println("[SYS] Booting Weather Station...");
    Serial2.begin(9600, SERIAL_8N1, 16, 15);

    windSpeedObject.begin();
    illuminanceObject.begin();
    pyranoMeterObject.begin();
    airQualityObject.begin();
    powerMeterObject.begin();

    Serial.println("[SYS] Modbus sensors initialized.");

    connectWiFi();
}

// ---------------------------------------------------
void loop()
{
    if (millis() - lastRun >= cycleInterval)
    {
        Serial.println("\n=======================================");
        Serial.println("[CYCLE] Starting new data cycle");
        Serial.println("=======================================\n");

        readSensors();
        sendJson();

        Serial.println("[CYCLE] Cycle complete\n");
        lastRun = millis();

        connectWiFi(); // auto reconnect check
    }
}