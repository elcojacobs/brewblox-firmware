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
        cp -r /firmware/build/target/brewblox-p1 /firmware/docker/flasher/brewblox-p1
        cp -r /firmware/build/target/brewblox-p1 /firmware/docker/rpi-flasher/brewblox-p1
        make clean APP=brewblox PLATFORM=photon
        make APP=brewblox PLATFORM=photon
        cp -r /firmware/build/target/brewblox-photon /firmware/docker/flasher/brewblox-photon
        cp -r /firmware/build/target/brewblox-photon /firmware/docker/rpi-flasher/brewblox-photon
    '

docker run --rm --privileged multiarch/qemu-user-static:register --reset

docker build -t brewblox/firmware-flasher:local flasher
docker build -t brewblox/firmware-flasher:rpi-local rpi-flasher
