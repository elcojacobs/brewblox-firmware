#! /usr/bin/env bash
set -e

SCRIPT_DIR=$(dirname "$(readlink -f "$0")")
pushd "${SCRIPT_DIR}/.." > /dev/null # Run from repo root

git submodule sync
git submodule update --init --depth 1 app/brewblox/proto
git submodule update --init --depth 1 platform/spark/device-os

FIRMWARE_VERSION=$(git rev-parse --short HEAD)
FIRMWARE_DATE=$(git show -s --format=%ci)

PROTO_VERSION=$(git --git-dir ./app/brewblox/proto/.git rev-parse --short HEAD)
PROTO_DATE=$(git --git-dir ./app/brewblox/proto/.git show -s --format=%ci)

PARTICLE_TAG=$(git --git-dir "./platform/spark/device-os/.git" fetch --tags --no-recurse-submodules && git --git-dir "./platform/spark/device-os/.git" describe --tags)
PARTICLE_RELEASES=https://github.com/particle-iot/device-os/releases/download/${PARTICLE_TAG}
PARTICLE_VERSION=${PARTICLE_TAG:1} # remove the 'v' prefix


OUT_DIR=docker/firmware-bin/source
mkdir -p "${OUT_DIR}"


curl -fL -o ${OUT_DIR}/bootloader-p1.bin "${PARTICLE_RELEASES}/p1-bootloader@${PARTICLE_VERSION}+lto.bin"
curl -fL -o ${OUT_DIR}/system-part1-p1.bin "${PARTICLE_RELEASES}/p1-system-part1@${PARTICLE_VERSION}.bin"
curl -fL -o ${OUT_DIR}/system-part2-p1.bin "${PARTICLE_RELEASES}/p1-system-part2@${PARTICLE_VERSION}.bin"

curl -fL -o ${OUT_DIR}/bootloader-photon.bin "${PARTICLE_RELEASES}/photon-bootloader@${PARTICLE_VERSION}+lto.bin"
curl -fL -o ${OUT_DIR}/system-part1-photon.bin "${PARTICLE_RELEASES}/photon-system-part1@${PARTICLE_VERSION}.bin"
curl -fL -o ${OUT_DIR}/system-part2-photon.bin "${PARTICLE_RELEASES}/photon-system-part2@${PARTICLE_VERSION}.bin"

{
    echo "[FIRMWARE]"
    echo "firmware_version=$FIRMWARE_VERSION"
    echo "firmware_date=$FIRMWARE_DATE"
    echo "proto_version=$PROTO_VERSION"
    echo "proto_date=$PROTO_DATE"
    echo "system_version=${PARTICLE_VERSION}"
} | tee "${OUT_DIR}/firmware.ini"

popd > /dev/null
