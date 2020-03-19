#! /usr/bin/env bash
set -e

# The particle image is expected to remain relatively stable
# It wraps the Particle CLI in a docker image, so it can easily be used to flash the firmware

export DOCKER_CLI_EXPERIMENTAL=enabled
docker run --rm --privileged multiarch/qemu-user-static --reset -p yes

docker buildx rm bricklayer || true
docker buildx create --use --name bricklayer
docker buildx inspect --bootstrap

# Don't forget to call with --push
docker buildx build \
    --platform linux/amd64,linux/arm/v7 \
    --tag brewblox/firmware-particle:latest \
    --tag brewblox/firmware-particle:rpi-latest \
    "$@" \
    particle
