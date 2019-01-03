#! /usr/bin/env bash
set -e

# The particle image is expected to remain relatively stable
# It wraps the Particle CLI in a docker image, so it can easily be used to flash the firmware

docker run --rm --privileged multiarch/qemu-user-static:register --reset

docker build -t brewblox/firmware-particle:latest particle
docker build -t brewblox/firmware-particle:rpi-latest rpi-particle
