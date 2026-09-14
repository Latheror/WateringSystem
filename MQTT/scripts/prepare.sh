#!/usr/bin/env sh
set -eu

SCRIPT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
ENV_FILE=${1:-"$SCRIPT_DIR/../.env"}

if [ ! -f "$ENV_FILE" ]; then
    echo "Missing MQTT environment file: $ENV_FILE. Copy .env.example to .env and fill it locally." >&2
    exit 1
fi

set -a
. "$ENV_FILE"
set +a

: "${MQTT_USERNAME:?MQTT_USERNAME must be set in the local MQTT environment file}"
: "${MQTT_PASSWORD:?MQTT_PASSWORD must be set in the local MQTT environment file}"
[ "$MQTT_USERNAME" != "CHANGE_ME" ] || { echo "MQTT_USERNAME must be replaced" >&2; exit 1; }
[ "$MQTT_PASSWORD" != "CHANGE_ME" ] || { echo "MQTT_PASSWORD must be replaced" >&2; exit 1; }

RUNTIME_DIR="$SCRIPT_DIR/../runtime"
mkdir -p "$RUNTIME_DIR"
printf '%s\n' 'user_id,password,is_superuser' "$MQTT_USERNAME,$MQTT_PASSWORD,false" > "$RUNTIME_DIR/auth-built-in-db-bootstrap.csv"
printf '%s\n' 'Prepared EMQX authentication bootstrap data.'
