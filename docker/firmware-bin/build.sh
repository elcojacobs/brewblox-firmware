#! /usr/bin/env bash
set -e
pushd "$(dirname "$0")" > /dev/null

# Prerequisite steps:
# - Build all desired binary/executable firmware files
# - Run before_build.sh to add dependency files

TAG=${TAG:-local}

# don't forget to call with --push
docker buildx build \
    --tag brewblox/firmware-bin:"$TAG" \
    "$@" \
    .
