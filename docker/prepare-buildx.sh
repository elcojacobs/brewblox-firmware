#! /usr/bin/env bash
set -e

# Early exit if current builder can handle ARM builds
if [[ $(docker buildx inspect | grep 'linux/arm/v7') != '' ]]; then
    exit 
fi

docker run --rm --privileged multiarch/qemu-user-static --reset -p yes

docker buildx rm bricklayer || true
docker buildx create --use --name bricklayer
docker buildx inspect --bootstrap
