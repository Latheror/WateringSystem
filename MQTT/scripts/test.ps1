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
$Message = $Message.Trim()
$ExpectedJson = ($Message | ConvertFrom-Json | ConvertTo-Json -Compress)

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
    param($HostName, $BrokerPort, $Username, $Password, $SubscriptionTopic)
    & mqttx sub -h $HostName -p $BrokerPort -u $Username -P $Password -t $SubscriptionTopic
} -ArgumentList $BrokerHost, $Port, $values['MQTT_USERNAME'], $values['MQTT_PASSWORD'], $Topic

try {
    & mqttx pub -h $BrokerHost -p $Port -u $values['MQTT_USERNAME'] -P $values['MQTT_PASSWORD'] -t $Topic -m $Message
    if ($LASTEXITCODE -ne 0) {
        throw "MQTT publish failed with exit code $LASTEXITCODE."
    }

    $received = $null
    $deadline = [DateTime]::UtcNow.AddSeconds(15)
    while ([DateTime]::UtcNow -lt $deadline -and $null -eq $received) {
        $output = @(Receive-Job -Job $subscriber -Keep -ErrorAction SilentlyContinue)
            foreach ($line in $output) {
                try {
                    $receivedJson = ([string]$line | ConvertFrom-Json | ConvertTo-Json -Compress)
                    if ($receivedJson -eq $ExpectedJson) {
                        $received = $receivedJson
                        break
                    }
                } catch {
                    # Ignore MQTTX connection/status output until the payload arrives.
                }
            }
            if ($null -eq $received) {
                Start-Sleep -Milliseconds 250
        }
    }

    if ($null -eq $received) {
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
