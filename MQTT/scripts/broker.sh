#!/usr/bin/env sh
set -eu

SCRIPT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
MQTT_DIR=$(CDPATH= cd -- "$SCRIPT_DIR/.." && pwd)
ACTION=${1:-status}
COMPOSE_FILE="$MQTT_DIR/docker-compose.yml"

case "$ACTION" in
    start|restart|test) "$SCRIPT_DIR/prepare.sh" ;;
esac

case "$ACTION" in
    start) docker compose -f "$COMPOSE_FILE" up -d ;;
    stop) docker compose -f "$COMPOSE_FILE" down ;;
    restart) docker compose -f "$COMPOSE_FILE" restart ;;
    status) docker compose -f "$COMPOSE_FILE" ps ;;
    logs) docker compose -f "$COMPOSE_FILE" logs --tail 100 emqx ;;
    test)
        docker compose -f "$COMPOSE_FILE" up -d
        docker compose -f "$COMPOSE_FILE" ps
        printf '%s\n' 'Use the MQTT test task with the local .env credentials to publish and subscribe.'
        ;;
    *) printf 'Usage: %s {start|stop|restart|status|logs|test}\n' "$0" >&2; exit 2 ;;
esac
