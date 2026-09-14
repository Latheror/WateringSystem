#include "mqtt_handler.h"

#include <ArduinoMqttClient.h>
#include <NetworkClient.h>
#include <Preferences.h>
#include <time.h>

#include "settings.h"

namespace {
NetworkClient networkClient;
MqttClient mqttClient(networkClient);
Preferences commandPreferences;
MqttCommandHandler commandHandler = nullptr;

constexpr uint8_t COMMAND_LEDGER_CAPACITY = 32;
constexpr size_t COMMAND_ID_STORAGE_LENGTH = MQTT_COMMAND_ID_MAX_LENGTH + 1;
constexpr char COMMAND_NAMESPACE[] = "mqttcmd";

bool readJsonBool(const String &payload, const char *key, bool &value) {
    String pattern = String('"') + key + "\":";
    int position = payload.indexOf(pattern);
    if (position < 0) {
        return false;
    }

    position += pattern.length();
    while (position < payload.length() && payload[position] == ' ') {
        ++position;
    }

    if (payload.startsWith("true", position)) {
        value = true;
        return true;
    }
    if (payload.startsWith("false", position)) {
        value = false;
        return true;
    }
    return false;
}

bool readJsonCommandId(const String &payload, char *commandId,
                       size_t commandIdSize) {
    String pattern = "\"commandId\":\"";
    int position = payload.indexOf(pattern);
    if (position < 0) {
        return false;
    }

    position += pattern.length();
    int end = payload.indexOf('"', position);
    if (end <= position || static_cast<size_t>(end - position) >= commandIdSize) {
        return false;
    }

    size_t length = static_cast<size_t>(end - position);
    memcpy(commandId, payload.c_str() + position, length);
    commandId[length] = '\0';
    return true;
}

bool isKnownCommandKey(const String &payload) {
    const char *knownKeys[] = {"autoMode", "manualWatering", "commandId"};
    int position = 0;
    while ((position = payload.indexOf('"', position)) >= 0) {
        int end = payload.indexOf('"', position + 1);
        if (end < 0) {
            return false;
        }

        int next = end + 1;
        while (next < payload.length() && payload[next] == ' ') {
            ++next;
        }
        if (next >= payload.length() || payload[next] != ':') {
            position = end + 1;
            continue;
        }

        String key = payload.substring(position + 1, end);
        bool known = false;
        for (const char *knownKey : knownKeys) {
            if (key == knownKey) {
                known = true;
                break;
            }
        }
        if (!known) {
            return false;
        }
        position = end + 1;
    }
    return true;
}

bool commandIdWasProcessed(const char *commandId) {
    uint8_t count = commandPreferences.getUChar("count", 0);
    for (uint8_t index = 0; index < count && index < COMMAND_LEDGER_CAPACITY; ++index) {
        char key[12];
        snprintf(key, sizeof(key), "id%u", index);
        String stored = commandPreferences.getString(key, "");
        if (stored == commandId) {
            return true;
        }
    }
    return false;
}

bool persistCommandId(const char *commandId) {
    uint8_t count = commandPreferences.getUChar("count", 0);
    uint8_t next = commandPreferences.getUChar("next", 0);
    uint8_t slot = count < COMMAND_LEDGER_CAPACITY ? count : next;

    char key[12];
    snprintf(key, sizeof(key), "id%u", slot);
    if (commandPreferences.putString(key, commandId) != strlen(commandId)) {
        return false;
    }

    String verified = commandPreferences.getString(key, "");
    if (verified != commandId) {
        return false;
    }

    if (count < COMMAND_LEDGER_CAPACITY) {
        commandPreferences.putUChar("count", count + 1);
    } else {
        commandPreferences.putUChar("next", (next + 1) % COMMAND_LEDGER_CAPACITY);
    }
    return true;
}

void onMqttMessage(int messageSize) {
    String payload;
    payload.reserve(static_cast<size_t>(messageSize) + 1);
    while (mqttClient.available()) {
        payload += static_cast<char>(mqttClient.read());
    }

    bool requestedAutoMode = false;
    bool manualWatering = false;
    if (!isKnownCommandKey(payload) ||
        !readJsonBool(payload, "autoMode", requestedAutoMode) ||
        !readJsonBool(payload, "manualWatering", manualWatering)) {
        return;
    }

    if (!manualWatering) {
        if (commandHandler != nullptr) {
            commandHandler(requestedAutoMode, false);
        }
        return;
    }

    char commandId[COMMAND_ID_STORAGE_LENGTH] = {};
    if (requestedAutoMode ||
        !readJsonCommandId(payload, commandId, sizeof(commandId)) ||
        commandIdWasProcessed(commandId) ||
        !persistCommandId(commandId)) {
        return;
    }

    if (commandHandler != nullptr) {
        commandHandler(false, true);
    }
}
}

void initMqtt(MqttCommandHandler handler) {
    commandHandler = handler;
    commandPreferences.begin(COMMAND_NAMESPACE, false);
    mqttClient.setId(MQTT_CLIENT_ID);
    mqttClient.setUsernamePassword(MQTT_USERNAME, MQTT_PASSWORD);
    mqttClient.setKeepAliveInterval(MQTT_KEEP_ALIVE_SECONDS * 1000UL);
    mqttClient.setConnectionTimeout(MQTT_CONNECTION_TIMEOUT_MS);
    mqttClient.setCleanSession(false);
    mqttClient.onMessage(onMqttMessage);
}

bool connectMqtt() {
    if (mqttClient.connected()) {
        return true;
    }

    for (uint8_t attempt = 0; attempt < MQTT_RETRY_LIMIT; ++attempt) {
        if (mqttClient.connect(MQTT_BROKER_HOST, MQTT_BROKER_PORT)) {
            if (mqttClient.subscribe(MQTT_COMMAND_TOPIC, MQTT_QOS) == 1) {
                return true;
            }
            mqttClient.stop();
            return false;
        }
    }
    return false;
}

void handleMqtt() {
    if (mqttClient.connected()) {
        mqttClient.poll();
    }
}

bool publishMqttState(float soilMoisture, SoilStatus soilStatus,
                      float solarVoltage, float batteryVoltage,
                      bool pumpActive, bool shouldWater, bool autoMode,
                      uint32_t remainingWaitingTimeSeconds, bool batteryLow,
                      bool wifiConnected) {
    if (!mqttClient.connected()) {
        return false;
    }

    const char *soilStatusText = "UNKNOWN";
    switch (soilStatus) {
        case SoilStatus::DRY: soilStatusText = "DRY"; break;
        case SoilStatus::WET: soilStatusText = "WET"; break;
        case SoilStatus::FLOATING: soilStatusText = "FLOATING"; break;
        default: break;
    }

    time_t now = time(nullptr);
    bool timestampValid = now > 0;
    char payload[512];
    int payloadLength = snprintf(
        payload, sizeof(payload),
        "{\"schemaVersion\":1,\"deviceId\":\"%s\",\"timestamp\":%lld,"
        "\"timestampValid\":%s,\"soilStatus\":\"%s\","
        "\"soilMoisture\":%.1f,\"solarVoltage\":%.2f,"
        "\"batteryVoltage\":%.2f,\"pumpActive\":%s,"
        "\"shouldWater\":%s,\"autoMode\":%s,"
        "\"remainingWaitingTimeSeconds\":%lu,\"batteryLow\":%s,"
        "\"wifiConnected\":%s,\"mqttConnected\":true,"
        "\"firmwareVersion\":\"%s\"}",
        MQTT_CLIENT_ID, static_cast<long long>(timestampValid ? now : 0),
        timestampValid ? "true" : "false", soilStatusText, soilMoisture,
        solarVoltage, batteryVoltage, pumpActive ? "true" : "false",
        shouldWater ? "true" : "false", autoMode ? "true" : "false",
        static_cast<unsigned long>(remainingWaitingTimeSeconds),
        batteryLow ? "true" : "false", wifiConnected ? "true" : "false",
        FIRMWARE_VERSION);

    if (payloadLength <= 0 || static_cast<size_t>(payloadLength) >= sizeof(payload)) {
        return false;
    }

    mqttClient.beginMessage(MQTT_STATE_TOPIC,
                            static_cast<unsigned long>(payloadLength),
                            MQTT_RETAIN_STATE, MQTT_QOS);
    mqttClient.print(payload);
    return mqttClient.endMessage() == 1;
}

bool isMqttConnected() {
    return mqttClient.connected();
}
