#! /usr/bin/env bash
set -e

# The compiler image is expected to remain relatively stable
# It does not contain any firmware code - just the software required to compile the firmware

SCRIPT_DIR=$(dirname "$(readlink -f "$0")")
pushd "$SCRIPT_DIR" > /dev/null

bash ./prepare-buildx.sh

DEFAULT_IMAGE=brewblox/firmware-compiler
DEFAULT_TAG=latest

IMAGE=${IMAGE:-$DEFAULT_IMAGE}
TAG=${TAG:-$DEFAULT_TAG}

docker buildx build \
    --tag "$IMAGE":amd-"$TAG" \
    --platform linux/amd64 \
    --push \
    firmware-compiler/amd

docker buildx build \
    --tag "$IMAGE":arm-"$TAG" \
    --platform linux/arm/v7,linux/arm64 \
    --push \
    firmware-compiler/arm

popd > /dev/null
