#! /usr/bin/env bash
set -e

# Use repository root
pushd "$(dirname "$(readlink -f "$0")")/.."

# clean target dir to not have amd64 leftovers. Sudo needed because docker has created some files as privileged
sudo rm -rf build/target

docker run \
    --rm \
    --privileged \
    docker/binfmt:a7996909642ee92942dcd6cff44b9b95f08dad64

# remove unsupported arg
sed -i 's/-m64//g' platform/spark/device-os/build/gcc-tools.mk

# Make sure compiler is up-to-date
docker pull \
    --platform=linux/arm/v7 \
    brewblox/firmware-compiler:latest

# build
docker run \
    -it --rm \
    --platform=linux/arm/v7 \
    -v "$(pwd)/":/firmware/ \
    brewblox/firmware-compiler:latest \
    make APP=brewblox PLATFORM=gcc

# reset modified file
cd platform/spark/device-os
git checkout -- build/gcc-tools.mk 

popd
