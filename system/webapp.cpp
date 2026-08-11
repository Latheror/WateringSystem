#include <Arduino.h>
#include <WebServer.h>
#include "soil_moisture_sensor.h"
#include "webapp.h"

// =========================
// External dependencies
// =========================
extern WebServer server;
extern bool pumpActive;
extern bool shouldWater;
extern bool autoMode;
extern void onManualWateringRequest();

// =========================
// Global sensor + values
// =========================
float solarVoltage = 0.0;
float batteryVoltage = 0.0;
float soilMoisture = 0.0;
SoilStatus soilStatus = SoilStatus::UNKNOWN;

// =========================
// HTML TEMPLATE (Flash)
// =========================
const char PAGE_TEMPLATE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">

<!-- FIX MOBILE SCALING -->
<meta name="viewport" content="width=device-width, initial-scale=1.0">

<!-- AUTO REFRESH -->
<meta http-equiv="refresh" content="2">

<title>Watering System</title>

<style>
body {
    font-family: 'Segoe UI', Tahoma, sans-serif;
    background: #eef2f7;
    margin: 0;
    padding: 16px;
    color: #333;
}

.container {
    max-width: 600px;
    margin: auto;
}

h1 {
    text-align: center;
    margin-bottom: 20px;
    font-size: 1.6em;
}

.grid {
    display: grid;
    grid-template-columns: 1fr 1fr;
    gap: 14px;
}

@media (max-width: 600px) {
    .grid {
        grid-template-columns: 1fr;
    }

    body {
        padding: 12px;
    }
}

.card {
    background: white;
    padding: 16px;
    border-radius: 14px;
    box-shadow: 0 4px 6px rgba(0,0,0,0.1);
    text-align: center;
}

.card h2 {
    font-size: 0.8em;
    color: #777;
    text-transform: uppercase;
    margin-bottom: 8px;
}

.value {
    font-size: 1.5em;
    font-weight: bold;
}

.soil { border-left: 6px solid #27ae60; }
.solar { border-left: 6px solid #f39c12; }
.battery { border-left: 6px solid #f1c40f; }
.pump { border-left: 6px solid #3498db; }
.should-water { border-left: 6px solid #e67e22; }

.card-btn {
    border: none;
    cursor: pointer;
    font-family: inherit;
    color: #333;
    width: 100%;
    transition: transform 0.1s ease;
}

.card-btn:active {
    transform: scale(0.97);
}

.auto { border-left: 6px solid #9b59b6; }

.water {
    border-left: 6px solid #3498db;
    display: flex;
    flex-direction: column;
    justify-content: center;
}
</style>

<script>
async function triggerWatering() {
    await fetch('/water');
}

async function toggleAutoMode() {
    await fetch('/toggleAutoMode');
}
</script>

</head>

<body>

<div class="container">

<h1>🌿 Watering System</h1>

<div class="grid">

<div class="card soil">
<h2>Soil Status</h2>
<div class="value">%SOIL%</div>
<div class="value" style="font-size:1.2em;margin-top:4px">%SOIL_MOISTURE%%</div>
</div>

<div class="card solar">
<h2>Solar Voltage</h2>
<div class="value">☀️ %SOLAR% V</div>
</div>

<div class="card battery">
<h2>Battery Voltage</h2>
<div class="value">🔋 %BATTERY% V</div>
</div>

<div class="card pump">
<h2>Pump Status</h2>
<div class="value">%PUMP%</div>
</div>

<div class="card should-water">
<h2>Should Water</h2>
<div class="value">%SHOULD_WATER%</div>
</div>

<button class="card auto card-btn" onclick="toggleAutoMode()">
<h2>Auto Mode</h2>
<div class="value">%AUTO%</div>
</button>

<button class="card water card-btn" onclick="triggerWatering()">
<h2>Manual Watering</h2>
<div class="value">💧 10s (%REMAINING%)</div>
</button>

</div>

</div>

</body>
</html>
)rawliteral";

// =========================
// Helpers
// =========================
static String renderPage()
{
    String page = FPSTR(PAGE_TEMPLATE);

    page.replace("%SOIL%",
        soilStatus == SoilStatus::FLOATING ? "⚠️ FLOATING" : (soilStatus == SoilStatus::DRY ? "🌵 DRY" : "💧 WET"));
    page.replace("%SOIL_MOISTURE%", String(soilMoisture, 1));

    page.replace("%SOLAR%", String(solarVoltage, 2));
    page.replace("%BATTERY%", String(batteryVoltage, 2));
    page.replace("%SHOULD_WATER%", shouldWater ? "✅ YES" : "❌ NO");
    page.replace("%PUMP%", pumpActive ? "🟢 ON" : "⚪ OFF");
    page.replace("%AUTO%", autoMode ? "✅ ON" : "❌ OFF");
    {
        int remainingSeconds = remainingWaitingTimeMs / 1000;
        if (remainingSeconds >= 60) {
            page.replace("%REMAINING%", String(remainingSeconds / 60) + "mn");
        } else {
            page.replace("%REMAINING%", String(remainingSeconds) + "s");
        }
    }

    return page;
}

// =========================
// Web server setup
// =========================
void initWebApp()
{
    server.on("/", handleRoot);
    server.on("/water", handleManualWatering);
    server.on("/toggleAutoMode", handleToggleAutoMode);

    server.begin();
    Serial.println("Web server started");
}


// =========================
// Routes
// =========================
void handleRoot()
{
    server.send(200, "text/html", renderPage());
}

void handleManualWatering()
{
    onManualWateringRequest();
    server.send(200, "text/plain", "OK");
}

void handleToggleAutoMode()
{
    autoMode = !autoMode;
    server.send(200, "text/plain", "OK");
}
