#! /usr/bin/env bash
set -e

# This script is intended for (local) use with Docker CE
# At the time of writing, Docker EE as available from Azure Pipelines does not include Buildx
# In CI pipelines, download and use this script:
# https://github.com/BrewBlox/deployed-images/blob/develop/prepare_buildx.sh

# Early exit if current builder can handle ARM builds
if [[ $(docker buildx inspect | grep 'linux/arm/v7') != '' ]]; then
    exit 
fi

docker run --rm --privileged multiarch/qemu-user-static --reset -p yes

docker buildx rm bricklayer || true
docker buildx create --use --name bricklayer
docker buildx inspect --bootstrap
