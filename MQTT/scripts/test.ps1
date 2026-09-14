param(
    [string]$BrokerHost = "127.0.0.1",
    [int]$Port = 1883,
    [string]$Topic = "watering/state",
    [string]$MessageFile = "$PSScriptRoot\..\examples\state-message.json"
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"
$MqttDirectory = Split-Path -Parent $PSScriptRoot
$EnvFile = Join-Path $MqttDirectory ".env"

if (-not (Test-Path -LiteralPath $EnvFile)) {
    throw "Missing MQTT environment file: $EnvFile"
}
if (-not (Test-Path -LiteralPath $MessageFile)) {
    throw "Missing MQTT test message file: $MessageFile"
}

$Message = Get-Content -LiteralPath $MessageFile -Raw
try {
    $null = $Message | ConvertFrom-Json
} catch {
    throw "MQTT test message is not valid JSON: $MessageFile"
}

$values = @{}
Get-Content -LiteralPath $EnvFile | ForEach-Object {
    if ($_ -match '^\s*([A-Za-z_][A-Za-z0-9_]*)=(.*)\s*$' -and -not $_.TrimStart().StartsWith('#')) {
        $values[$matches[1]] = $matches[2].Trim().Trim('"')
    }
}

foreach ($name in @('MQTT_USERNAME', 'MQTT_PASSWORD')) {
    if (-not $values.ContainsKey($name) -or [string]::IsNullOrWhiteSpace($values[$name]) -or $values[$name] -eq 'CHANGE_ME') {
        throw "$name must be set in the local MQTT environment file."
    }
}

$mqttx = Get-Command mqttx -ErrorAction SilentlyContinue
if ($null -eq $mqttx) {
    throw "MQTTX CLI is required. Install it with: npm install --global mqttx-cli"
}

& $mqttx.Source pub --hostname $BrokerHost --port $Port `
    --username $values['MQTT_USERNAME'] --password $values['MQTT_PASSWORD'] `
    --topic $Topic --file-read $MessageFile --qos 1 --retain
if ($LASTEXITCODE -ne 0) {
    throw "MQTT publish failed with exit code $LASTEXITCODE."
}

Write-Output "Authenticated MQTT JSON publish passed for topic $Topic."
