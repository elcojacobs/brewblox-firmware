#! /usr/bin/env bash
set -e

docker run \
    --rm \
    --volume $(pwd)/../:/firmware \
    --name flasher-compiler \
    brewblox/firmware-compiler \
    bash -c '
        set -e
        cd /firmware/build
        rm -rf /firmware/docker/simulator/target
        bash compile-proto.sh
        make clean APP=brewblox PLATFORM=p1
        make APP=brewblox PLATFORM=p1
        cp -r /firmware/build/target/brewblox-p1 /firmware/docker/flasher/brewblox
    '

docker build -t brewblox/firmware-flasher:local flasher
