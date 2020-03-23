#! /usr/bin/env bash
set -e

PARTICLE_TAG="v1.5.0-rc.2" # temporary workaround instead of $(git --git-dir ../platform/spark/device-os/.git describe --tags)
PARTICLE_RELEASES=https://github.com/particle-iot/device-os/releases/download/${PARTICLE_TAG}
PARTICLE_VERSION=${PARTICLE_TAG:1} # remove the 'v' prefix

MY_DIR=$(dirname "$(readlink -f "$0")")
git submodule update --init "$MY_DIR/../app/brewblox/proto"

echo "proto_version=$(cd "$MY_DIR/../app/brewblox/proto"; git rev-parse --short HEAD)"

OUT_DIR="./firmware-bin/source"

rm -rf "${OUT_DIR}" || true
mkdir "${OUT_DIR}"

curl -fL -o "${OUT_DIR}"/bootloader-p1.bin "${PARTICLE_RELEASES}/p1-bootloader@${PARTICLE_VERSION}+lto.bin"
curl -fL -o "${OUT_DIR}"/system-part1-p1.bin "${PARTICLE_RELEASES}/p1-system-part1@${PARTICLE_VERSION}.bin"
curl -fL -o "${OUT_DIR}"/system-part2-p1.bin "${PARTICLE_RELEASES}/p1-system-part2@${PARTICLE_VERSION}.bin"

curl -fL -o "${OUT_DIR}"/bootloader-photon.bin "${PARTICLE_RELEASES}/photon-bootloader@${PARTICLE_VERSION}+lto.bin"
curl -fL -o "${OUT_DIR}"/system-part1-photon.bin "${PARTICLE_RELEASES}/photon-system-part1@${PARTICLE_VERSION}.bin"
curl -fL -o "${OUT_DIR}"/system-part2-photon.bin "${PARTICLE_RELEASES}/photon-system-part2@${PARTICLE_VERSION}.bin"

{
    echo "[FIRMWARE]",
    echo "firmware_version=$(git rev-parse --short HEAD)",
    echo "firmware_date=$(git show -s --format=%ci)",
    echo "proto_version=$(cd "$MY_DIR"/../app/brewblox/proto; git rev-parse --short HEAD)",
    echo "proto_date=$(cd "$MY_DIR"/../app/brewblox/proto; git show -s --format=%ci)",
    echo "system_version=${PARTICLE_VERSION}"
} > "${OUT_DIR}"/firmware.ini
