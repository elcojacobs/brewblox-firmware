#! /usr/bin/env bash
set -ex

# Use repository root
pushd "$(dirname "$0")/.." > /dev/null

# clean target dir to not have amd64 leftovers. Sudo needed because docker has created some files as privileged
sudo rm -rf build/target

# Enable emulation
docker run --rm --privileged multiarch/qemu-user-static --reset -p yes

# -m64 is not supported for ARM
sed -i 's/-m64//g' platform/spark/device-os/build/gcc-tools.mk

# Make sure compiler is up-to-date
docker pull \
    --platform=linux/arm/v7 \
    brewblox/simulator-compiler:latest

# build
docker run \
    -it --rm \
    --platform=linux/arm/v7 \
    -v "$(pwd)/":/firmware/ \
    brewblox/simulator-compiler:latest \
    make APP=brewblox PLATFORM=gcc

# reset modified file
cd platform/spark/device-os
git checkout -- build/gcc-tools.mk 
