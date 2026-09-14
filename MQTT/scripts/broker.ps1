param(
    [ValidateSet('start', 'stop', 'restart', 'status', 'logs', 'test')]
    [string]$Action = 'status'
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
$MqttDirectory = Split-Path -Parent $PSScriptRoot
$Compose = @('-f', (Join-Path $MqttDirectory 'docker-compose.yml'))

if ($Action -in @('start', 'restart', 'test')) {
    & (Join-Path $PSScriptRoot 'prepare.ps1')
}

switch ($Action) {
    'start'   { docker compose @Compose up -d }
    'stop'    { docker compose @Compose down }
    'restart' { docker compose @Compose restart }
    'status'  { docker compose @Compose ps }
    'logs'    { docker compose @Compose logs --tail 100 emqx }
    'test'    {
        docker compose @Compose up -d
        docker compose @Compose ps
        Write-Output 'Use the MQTT test task with the local .env credentials to publish and subscribe.'
    }
}
