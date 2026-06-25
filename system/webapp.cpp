#include "webapp.h"

// Global variables for web app
float solarVoltage = 0.0;
float batteryVoltage = 0.0;
bool pumpActive = false;

// Non-blocking variables for watering
unsigned long wateringStartTime = 0;
bool isWatering = false;

/**
 * @brief Initialize the web server routes.
 */
void initWebApp() {
    server.on("/", handleRoot);
    server.on("/water", handleManualWatering);
    server.begin();
    Serial.println("Web server started on port 80");
}

/**
 * @brief Handle root page request.
 */
void handleRoot() {
    String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'>";
    html += "<title>Watering System Dashboard</title>";
    html += "<style>";
    html += "body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; background-color: #eef2f7; margin: 0; padding: 20px; color: #333; }";
    html += ".container { max-width: 600px; margin: 0 auto; }";
    html += "h1 { text-align: center; color: #2c3e50; margin-bottom: 30px; }";
    html += ".grid { display: grid; grid-template-columns: 1fr 1fr; gap: 20px; }";
    html += ".card { background: white; padding: 20px; border-radius: 15px; box-shadow: 0 4px 6px rgba(0,0,0,0.1); text-align: center; }";
    html += ".card h2 { margin: 0 0 10px 0; font-size: 0.9em; color: #7f8c8d; text-transform: uppercase; }";
    html += ".card p { margin: 0; font-size: 1.5em; font-weight: bold; }";
    html += ".status-wet { color: #27ae60; }";
    html += ".status-dry { color: #e67e22; }";
    html += ".pump-on { color: #e74c3c; animation: blink 1s infinite; }";
    html += ".pump-off { color: #95a5a6; }";
    html += "@keyframes blink { 0% { opacity: 1; } 50% { opacity: 0.5; } 100% { opacity: 1; } }";
    html += ".btn-container { margin-top: 30px; text-align: center; }";
    html += "button { padding: 15px 30px; font-size: 1.1em; background-color: #3498db; color: white; border: none; border-radius: 50px; cursor: pointer; transition: background 0.3s; box-shadow: 0 4px 6px rgba(0,0,0,0.1); }";
    html += "button:active { transform: scale(0.98); }";
    html += "button:hover { background-color: #2980b9; }";
    html += "</style></head><body>";
    html += "<div class='container'>";
    html += "<h1>🌿 Watering System</h1>";
    html += "<div class='grid'>";
    
    // Soil Status Card
    html += "<div class='card'><h2>Soil Status</h2>";
    html += "<p class='status-" + String(digitalRead(SOIL_MOISTURE_SENSOR_DIGITAL_PIN) ? "dry" : "wet") + "'>" + String(digitalRead(SOIL_MOISTURE_SENSOR_DIGITAL_PIN) ? "DRY" : "WET") + "</p></div>";
    
    // Solar Voltage Card
    html += "<div class='card'><h2>Solar Voltage</h2>";
    html += "<p>" + String(solarVoltage, 2) + " V</p></div>";
    
    // Battery Level Card
    html += "<div class='card'><h2>Battery Level</h2>";
    html += "<p>" + String(batteryVoltage, 2) + " V</p></div>";
    
    // Pump Status Card
    html += "<div class='card'><h2>Pump Status</h2>";
    html += "<p class='" + String(pumpActive ? "pump-on" : "pump-off") + "'>" + String(pumpActive ? "ACTIVE" : "OFF") + "</p></div>";
    
    html += "</div>";
    html += "<div class='btn-container'>";
    html += "<button onclick=\"fetch('/water').then(() => location.reload())\">💧 Manual Watering (10s)</button>";
    html += "</div>";
    html += "</div></body></html>";
    server.send(200, "text/html", html);
}

/**
 * @brief Handle manual watering request.
 */
void handleManualWatering() {
    if (!isWatering) {
        digitalWrite(RELAY_PIN, RELAY_ON);
        wateringStartTime = millis();
        isWatering = true;
        Serial.println("Watering started");
    }
    server.send(200, "text/plain", "Watering in progress or complete");
}
