#! /usr/bin/env bash
set -e

PARTICLE_TAG=$(git --git-dir ../platform/spark/device-os/.git describe --tags)
PARTICLE_RELEASES=https://github.com/particle-iot/device-os/releases/download/${PARTICLE_TAG}
PARTICLE_VERSION=${PARTICLE_TAG:1} # remove the 'v' prefix

SCRIPT_DIR=$(dirname "$(readlink -f "$0")")
pushd "$SCRIPT_DIR/.." > /dev/null # Run from repo root

git submodule update --init app/brewblox/proto
echo "proto_version=$(cd app/brewblox/proto; git rev-parse --short HEAD)"

OUT_DIR=docker/firmware-bin/source
mkdir -p ${OUT_DIR}

curl -fL -o ${OUT_DIR}/bootloader-p1.bin "${PARTICLE_RELEASES}/p1-bootloader@${PARTICLE_VERSION}+lto.bin"
curl -fL -o ${OUT_DIR}/system-part1-p1.bin "${PARTICLE_RELEASES}/p1-system-part1@${PARTICLE_VERSION}.bin"
curl -fL -o ${OUT_DIR}/system-part2-p1.bin "${PARTICLE_RELEASES}/p1-system-part2@${PARTICLE_VERSION}.bin"

curl -fL -o ${OUT_DIR}/bootloader-photon.bin "${PARTICLE_RELEASES}/photon-bootloader@${PARTICLE_VERSION}+lto.bin"
curl -fL -o ${OUT_DIR}/system-part1-photon.bin "${PARTICLE_RELEASES}/photon-system-part1@${PARTICLE_VERSION}.bin"
curl -fL -o ${OUT_DIR}/system-part2-photon.bin "${PARTICLE_RELEASES}/photon-system-part2@${PARTICLE_VERSION}.bin"

{
    echo "[FIRMWARE]"
    echo "firmware_version=$(git rev-parse --short HEAD)"
    echo "firmware_date=$(git show -s --format=%ci)"
    echo "proto_version=$(cd app/brewblox/proto; git rev-parse --short HEAD)"
    echo "proto_date=$(cd app/brewblox/proto; git show -s --format=%ci)"
    echo "system_version=${PARTICLE_VERSION}"
} | tee ${OUT_DIR}/firmware.ini

popd > /dev/null
