#! /usr/bin/env bash
set -e
pushd "$(dirname "$0")" > /dev/null

# The compiler image is expected to remain relatively stable
# It does not contain any firmware code - just the software required to compile the firmware

TAG=${TAG:-latest}

bash ../prepare-buildx.sh

# don't forget to call with --push
docker buildx build \
    --tag brewblox/simulator-compiler:"$TAG" \
    --platform linux/amd64,linux/arm/v7,linux/arm64/v8 \
    "$@" \
    .
