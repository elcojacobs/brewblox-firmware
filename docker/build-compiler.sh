#! /usr/bin/env bash
set -e

# The compiler image is expected to remain relatively stable
# It does not contain any firmware code - just the software required to compile the firmware

bash ./enable-experimental.sh
bash ./prepare-buildx.sh

DEFAULT_IMAGE=brewblox/firmware-compiler
DEFAULT_TAG=latest

IMAGE=${IMAGE:-$DEFAULT_IMAGE}
TAG=${TAG:-$DEFAULT_TAG}

docker buildx build \
    --tag "$IMAGE":amd-"$TAG" \
    --platform linux/amd64 \
    --load \
    firmware-compiler/amd

docker buildx build \
    --tag "$IMAGE":arm-"$TAG" \
    --platform linux/arm/v7 \
    --load \
    firmware-compiler/arm

docker push "$IMAGE":amd-"$TAG"
docker push "$IMAGE":arm-"$TAG"

docker manifest create "$IMAGE":"$TAG" \
    --amend "$IMAGE":amd-"$TAG" \
    --amend "$IMAGE":arm-"$TAG"

docker manifest push "$IMAGE":"$TAG"
