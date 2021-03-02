#! /usr/bin/env bash
set -ex

# Use repository root
pushd "$(dirname "$0")/.." > /dev/null

# clean target dir to not have amd64 leftovers. Sudo needed because docker has created some files as privileged
sudo rm -rf build/target

# Pull native compiler
docker pull \
    brewblox/simulator-compiler:latest

# Compile proto with native compiler
docker run \
    -it --rm \
    -v "$(pwd)":/firmware \
    -w /firmware/build \
    brewblox/simulator-compiler:latest \
    bash compile-proto.sh

# Enable emulation
if [[ $(arch) != 'aarch64' ]]; then
    docker run --rm --privileged multiarch/qemu-user-static --reset -p yes
fi

# -m64 is not supported for ARM
sed -i 's/-m64//g' platform/spark/device-os/build/gcc-tools.mk

# Make sure compiler is up-to-date
docker pull \
    --platform=linux/arm64/v8 \
    brewblox/simulator-compiler:latest

# build
docker run \
    -it --rm \
    --platform=linux/arm64/v8 \
    -v "$(pwd)/":/firmware/ \
    brewblox/simulator-compiler:latest \
    make APP=brewblox PLATFORM=gcc

# reset modified file
cd platform/spark/device-os
git checkout -- build/gcc-tools.mk 
