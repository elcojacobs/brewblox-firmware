#! /usr/bin/env bash
set -e

TARGET="$1"

if [[ "$TARGET" == "linux/amd64" ]]; then
    cp source/brewblox-amd brewblox
else
    cp source/brewblox-arm brewblox
fi

chmod +x brewblox
touch device_key.der server_key.der eeprom.bin
