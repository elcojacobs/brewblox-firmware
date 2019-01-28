#! /usr/bin/env bash
set -e

docker run \
    --rm \
    --volume $(pwd)/../:/firmware \
    --name simulator-compiler \
    brewblox/firmware-compiler \
    bash -c '
        set -e
        cd /firmware/build
        rm -rf /firmware/docker/simulator/target
        bash compile-proto.sh
        make clean APP=brewblox PLATFORM=gcc
        make -j APP=brewblox PLATFORM=gcc
        cp -r /firmware/build/target /firmware/docker/simulator/target
    '

docker build -t brewblox/firmware-simulator:local simulator
