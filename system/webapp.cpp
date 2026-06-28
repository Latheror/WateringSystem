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

    html += ".card { background: white; padding: 20px; border-radius: 15px; box-shadow: 0 4px 6px rgba(0,0,0,0.1); text-align: center; border-left: 6px solid transparent; }";

    html += ".battery { border-left-color: #f1c40f; }";
    html += ".solar { border-left-color: #f39c12; }";
    html += ".soil { border-left-color: #27ae60; }";
    html += ".pump { border-left-color: #3498db; }";

    html += ".card h2 { margin: 0 0 10px 0; font-size: 0.85em; color: #7f8c8d; text-transform: uppercase; letter-spacing: 1px; }";

    html += ".value { font-size: 1.6em; font-weight: bold; margin-top: 8px; display: block; }";

    html += ".icon { font-size: 1.4em; margin-right: 6px; }";

    html += ".status-wet { color: #27ae60; }";
    html += ".status-dry { color: #e67e22; }";
    html += ".pump-on { color: #27ae60; animation: blink 1s infinite; }";
    html += ".pump-off { color: #95a5a6; }";

    html += "@keyframes blink { 0% { opacity: 1; } 50% { opacity: 0.5; } 100% { opacity: 1; } }";

    html += ".btn-container { margin-top: 30px; text-align: center; }";

    html += "button { padding: 15px 30px; font-size: 1.1em; background-color: #3498db; color: white; border: none; border-radius: 50px; cursor: pointer; transition: 0.3s; box-shadow: 0 4px 6px rgba(0,0,0,0.1); }";
    html += "button:hover { background-color: #2980b9; }";
    html += "button:active { transform: scale(0.98); }";

    html += "</style></head><body>";

    // =========================
    // CONTENT
    // =========================
    html += "<div class='container'>";
    html += "<h1>🌿 Watering System</h1>";

    html += "<div class='grid'>";

    // Soil Status
    bool soilDry = digitalRead(SOIL_MOISTURE_SENSOR_DIGITAL_PIN);

    html += "<div class='card soil'>";
    html += "<div><span class='icon'>🌱</span>Soil Status</div>";
    html += "<span class='value " + String(soilDry ? "status-dry" : "status-wet") + "'>";
    html += (soilDry ? "🌵 DRY" : "💧 WET");
    html += "</span></div>";

    // Solar Voltage
    html += "<div class='card solar'>";
    html += "<div><span class='icon'>☀️</span>Solar Voltage</div>";
    html += "<span class='value'>" + String(solarVoltage, 2) + " V</span>";
    html += "</div>";

    // Battery
    html += "<div class='card battery'>";
    html += "<div><span class='icon'>🔋</span>Battery Level</div>";
    html += "<span class='value'>" + String(batteryVoltage, 2) + " V</span>";
    html += "</div>";

    // Pump Status
    html += "<div class='card pump'>";
    html += "<div><span class='icon'>🚰</span>Pump Status</div>";
    html += "<span class='value " + String(pumpActive ? "pump-on" : "pump-off") + "'>";
    html += (pumpActive ? "🟢 ON" : "⚪ OFF");
    html += "</span></div>";

    html += "</div>"; // grid

    // =========================
    // BUTTON
    // =========================
    html += "<div class='btn-container'>";
    html += "<button onclick=\"fetch('/water').then(() => location.reload())\">";
    html += "💧 Manual Watering (10s)";
    html += "</button>";
    html += "</div>";

    html += "</div>"; // container

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