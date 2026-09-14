param(
    [string]$Host = "host.docker.internal",
    [int]$Port = 1883
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"
$MqttDirectory = Split-Path -Parent $PSScriptRoot
$EnvFile = Join-Path $MqttDirectory ".env"
$TestTopic = "watering/test"
$Message = "mqtt-test-$([DateTimeOffset]::UtcNow.ToUnixTimeSeconds())"

if (-not (Test-Path -LiteralPath $EnvFile)) {
    throw "Missing MQTT environment file: $EnvFile"
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

$subscriber = Start-Job -ScriptBlock {
    param($HostName, $BrokerPort, $Username, $Password, $Topic)
    & mqttx sub -h $HostName -p $BrokerPort -u $Username -P $Password -t $Topic -c 1 --timeout 15
} -ArgumentList $Host, $Port, $values['MQTT_USERNAME'], $values['MQTT_PASSWORD'], $TestTopic

try {
    Start-Sleep -Seconds 2
    & mqttx pub -h $Host -p $Port -u $values['MQTT_USERNAME'] -P $values['MQTT_PASSWORD'] -t $TestTopic -m $Message
    if ($LASTEXITCODE -ne 0) {
        throw "MQTT publish failed with exit code $LASTEXITCODE."
    }

    $received = Receive-Job -Job $subscriber -Wait -AutoRemoveJob
    if ($received -ne $Message) {
        throw "MQTT test received an unexpected message."
    }
    Write-Output "Authenticated MQTT publish/subscribe test passed."
}
finally {
    if ($subscriber -and $subscriber.State -eq 'Running') {
        Stop-Job -Job $subscriber -ErrorAction SilentlyContinue
        Remove-Job -Job $subscriber -Force -ErrorAction SilentlyContinue
    }
}
