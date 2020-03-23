#! /usr/bin/env bash
set -e

# Enables experimental features for both the Docker daemon and the Docker CLI

# Configuration files
CFG_CLI="$HOME/.docker/config.json"
CFG_DAEMON="/etc/docker/daemon.json"

# Ensure "jq" is installed.
command -v jq > /dev/null || sudo apt-get install -y jq

if [ ! -f "$CFG_CLI" ]; then
    echo "Creating $CFG_CLI"
    echo '{}' > "$CFG_CLI"
fi;

CURRENT=$(jq -r '.experimental' "$CFG_CLI")
if [[ "$CURRENT" != "enabled" ]]; then
    echo "Updating $CFG_CLI"
    OUTPUT=$(jq '. + {"experimental":"enabled"}' "$CFG_CLI")
    echo "$OUTPUT" > "$CFG_CLI"
fi;

if [ ! -f "$CFG_DAEMON" ]; then
    echo "Creating $CFG_DAEMON"
    echo '{}' | sudo tee "$CFG_DAEMON"
fi;

CURRENT=$(jq -r '.experimental' "$CFG_DAEMON")
if [[ "$CURRENT" != "true" ]]; then
    echo "Updating $CFG_DAEMON"
    OUTPUT=$(jq '. + {"experimental":true}' "$CFG_DAEMON")
    echo "$OUTPUT" | sudo tee "$CFG_DAEMON"
    sudo service docker restart
fi;
