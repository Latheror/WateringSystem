param(
    [string]$EnvFile = "$PSScriptRoot\..\.env"
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

if (-not (Test-Path $EnvFile)) {
    throw "Missing MQTT environment file: $EnvFile. Copy .env.example to .env and fill it locally."
}

$values = @{}
Get-Content $EnvFile | ForEach-Object {
    if ($_ -match '^\s*([A-Za-z_][A-Za-z0-9_]*)=(.*)\s*$' -and -not $_.TrimStart().StartsWith('#')) {
        $values[$matches[1]] = $matches[2].Trim().Trim('"')
    }
}

foreach ($name in @('MQTT_USERNAME', 'MQTT_PASSWORD')) {
    if (-not $values.ContainsKey($name) -or [string]::IsNullOrWhiteSpace($values[$name]) -or $values[$name] -eq 'CHANGE_ME') {
        throw "$name must be set in the local MQTT environment file."
    }
}

$runtimeDirectory = Join-Path $PSScriptRoot '..\runtime'
New-Item -ItemType Directory -Force -Path $runtimeDirectory | Out-Null
$bootstrapPath = Join-Path $runtimeDirectory 'auth-built-in-db-bootstrap.csv'

"user_id,password,is_superuser" | Set-Content -Path $bootstrapPath -Encoding ascii
"$($values['MQTT_USERNAME']),$($values['MQTT_PASSWORD']),false" | Add-Content -Path $bootstrapPath -Encoding ascii
Write-Output "Prepared EMQX authentication bootstrap data."
