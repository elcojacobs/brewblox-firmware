#! /usr/bin/env bash
set -e

pushd "$(dirname "$(readlink -f "$0")")" > /dev/null

bash ./enable-experimental.sh
bash ./prepare-buildx.sh
bash ./start-compiler.sh

IMAGE=brewblox/firmware-simulator
SRC=firmware-simulator/source

mkdir -p ../build/target
sudo chown -R "$USER" ../build/target
rm -rf build/target/*

mkdir -p $SRC
rm -rf ${SRC:?}/*

# Build and copy AMD simulator
docker-compose exec -T compiler \
    bash -c "
        set -e
        bash compile-proto.sh
        bash build-sim-amd.sh
    "
cp ../build/target/brewblox-gcc/brewblox $SRC/brewblox-amd

# Build AMD image
docker buildx build \
    --load \
    --tag $IMAGE:amd-local \
    --platform linux/amd64 \
    firmware-simulator

popd > /dev/null
