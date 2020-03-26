#! /usr/bin/env bash
set -e

# The particle image is expected to remain relatively stable
# It wraps the Particle CLI in a docker image, so it can easily be used to flash the firmware

SCRIPT_DIR=$(dirname "$(readlink -f "$0")")
pushd "$SCRIPT_DIR" > /dev/null

bash ./enable-experimental.sh
bash ./prepare-buildx.sh

# Don't forget to call with --push
docker buildx build \
    --platform linux/amd64,linux/arm/v7 \
    --tag brewblox/firmware-particle:latest \
    --tag brewblox/firmware-particle:rpi-latest \
    "$@" \
    firmware-particle

popd > /dev/null
