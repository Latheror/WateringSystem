#include "webapp.h"

// Global variables (ONLY for display, not control)
float solarVoltage = 0.0;
float batteryVoltage = 0.0;

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

    String html = "";

    // =========================
    // HTML + STYLE HEADER
    // =========================
    html += "<!DOCTYPE html><html><head>";
    html += "<meta charset='UTF-8'>";
    html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
    html += "<meta http-equiv='refresh' content='2'>";
    html += "<title>Watering System</title>";

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
    html += ".pump-on { color: #27ae60; animation: blink 1s infinite; }";
    html += ".pump-off { color: #95a5a6; }";

    html += "@keyframes blink { 0% { opacity: 1; } 50% { opacity: 0.5; } 100% { opacity: 1; } }";

    html += ".btn-container { margin-top: 30px; text-align: center; }";

    html += "button { padding: 15px 30px; font-size: 1.1em; background-color: #3498db; color: white; border: none; border-radius: 50px; cursor: pointer; transition: background 0.3s; box-shadow: 0 4px 6px rgba(0,0,0,0.1); }";
    html += "button:active { transform: scale(0.98); }";
    html += "button:hover { background-color: #2980b9; }";

    html += "</style>";

    html += "</head><body>";

    // =========================
    // CONTENT
    // =========================
    html += "<div class='container'>";
    html += "<h1>🌿 Watering System</h1>";

    html += "<div class='grid'>";

    // Soil Status
    bool soilDry = digitalRead(SOIL_MOISTURE_SENSOR_DIGITAL_PIN);

    html += "<div class='card'><h2>Soil Status</h2>";
    html += "<p class='status-";
    html += (soilDry ? "dry" : "wet");
    html += "'>";
    html += (soilDry ? "DRY" : "WET");
    html += "</p></div>";

    // Solar Voltage
    html += "<div class='card'><h2>Solar Voltage</h2>";
    html += "<p>" + String(solarVoltage, 2) + " V</p></div>";

    // Battery
    html += "<div class='card'><h2>Battery Level</h2>";
    html += "<p>" + String(batteryVoltage, 2) + " V</p></div>";

    // Pump Status
    html += "<div class='card'><h2>Pump Status</h2>";
    html += "<p class='";
    html += (pumpActive ? "pump-on" : "pump-off");
    html += "'>";
    html += (pumpActive ? "ACTIVE" : "OFF");
    html += "</p></div>";

    html += "</div>";

    // =========================
    // BUTTON
    // =========================
    html += "<div class='btn-container'>";
    html += "<button onclick=\"fetch('/water').then(() => location.reload())\">";
    html += "💧 Manual Watering (10s)";
    html += "</button>";
    html += "</div>";

    html += "</div>";

    html += "</body></html>";

    server.send(200, "text/html", html);
}

/**
 * @brief Handle manual watering request.
 * ONLY sends command to system.ino via shared flag.
 */
void handleManualWatering() {

    onManualWateringRequest();
}