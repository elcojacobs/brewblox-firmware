#! /usr/bin/env bash
set -e

pushd "$(dirname "$(readlink -f "$0")")" > /dev/null

IMAGE=brewblox/firmware-simulator
SRC=firmware-simulator/source

mkdir -p ../build/target
sudo chown -R "$USER" ../build/target

mkdir -p $SRC
rm -rf ${SRC:?}/*

bash start-compiler.sh

# Build and copy AMD simulator
docker-compose exec -T compiler \
    bash -c "
        set -e
        bash compile-proto.sh
        bash build-sim-amd.sh
    "
cp ../build/target/brewblox-gcc/brewblox $SRC/brewblox-amd

# Build and copy ARM simulator
# Proto was already built by AMD simulator
bash ../build/build-sim-arm.sh

# Build AMD image
docker buildx build \
    --load \
    --tag $IMAGE:amd-local \
    --platform linux/amd64 \
    firmware-simulator

popd > /dev/null
