#! /usr/bin/env bash
set -e

docker-compose exec -T compiler \
    bash -c '
        set -e
        bash compile-proto.sh
        make $MAKE_ARGS APP=brewblox PLATFORM=p1
        cp target/brewblox-p1/brewblox.bin ../docker/flasher/brewblox-p1.bin
        make $MAKE_ARGS APP=brewblox PLATFORM=photon
        cp target/brewblox-photon/brewblox.bin ../docker/flasher/brewblox-photon.bin
    '

docker run --rm --privileged multiarch/qemu-user-static:register --reset

docker build --no-cache -f flasher/amd/Dockerfile -t brewblox/firmware-flasher:local flasher
docker build --no-cache -f flasher/arm/Dockerfile -t brewblox/firmware-flasher:rpi-local flasher
