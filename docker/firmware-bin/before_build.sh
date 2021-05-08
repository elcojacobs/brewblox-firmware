#! /usr/bin/env bash
set -e
pushd "$(dirname "$0")/../.." > /dev/null # Run from repo root

SRC=docker/firmware-bin/source

# This script does not build various binary/executable files.
# They are assumed to be present in docker/firmware-bin/source already.

git submodule sync
git submodule update --init --depth 1 brewblox/blox/proto
git submodule update --init --depth 1 platform/spark/device-os

FIRMWARE_VERSION=$(git rev-parse --short=8 HEAD)
FIRMWARE_DATE=$(git show -s --format=%ci)

PROTO_VERSION=$(git --git-dir ./brewblox/blox/proto/.git rev-parse --short=8 HEAD)
PROTO_DATE=$(git --git-dir ./brewblox/blox/proto/.git show -s --format=%ci)

PARTICLE_TAG=$(git --git-dir "./platform/spark/device-os/.git" fetch --tags --no-recurse-submodules && git --git-dir "./platform/spark/device-os/.git" describe --tags)
PARTICLE_RELEASES=https://github.com/particle-iot/device-os/releases/download/${PARTICLE_TAG}
PARTICLE_VERSION=${PARTICLE_TAG:1} # remove the 'v' prefix

mkdir -p "${SRC}"

curl -fL -o ${SRC}/bootloader-p1.bin "${PARTICLE_RELEASES}/p1-bootloader@${PARTICLE_VERSION}+lto.bin"
curl -fL -o ${SRC}/system-part1-p1.bin "${PARTICLE_RELEASES}/p1-system-part1@${PARTICLE_VERSION}.bin"
curl -fL -o ${SRC}/system-part2-p1.bin "${PARTICLE_RELEASES}/p1-system-part2@${PARTICLE_VERSION}.bin"

curl -fL -o ${SRC}/bootloader-photon.bin "${PARTICLE_RELEASES}/photon-bootloader@${PARTICLE_VERSION}+lto.bin"
curl -fL -o ${SRC}/system-part1-photon.bin "${PARTICLE_RELEASES}/photon-system-part1@${PARTICLE_VERSION}.bin"
curl -fL -o ${SRC}/system-part2-photon.bin "${PARTICLE_RELEASES}/photon-system-part2@${PARTICLE_VERSION}.bin"

{
    echo "[FIRMWARE]"
    echo "firmware_version=$FIRMWARE_VERSION"
    echo "firmware_date=$FIRMWARE_DATE"
    echo "proto_version=$PROTO_VERSION"
    echo "proto_date=$PROTO_DATE"
    echo "system_version=${PARTICLE_VERSION}"
} | tee "${SRC}/firmware.ini"
