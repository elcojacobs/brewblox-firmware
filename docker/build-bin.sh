#! /usr/bin/env bash
set -e

MY_DIR=$(dirname $(readlink -f $0))

rm -rf ./firmware-bin/source || true
mkdir ./firmware-bin/source

docker-compose exec -T compiler \
    bash -c '
        set -e
        bash compile-proto.sh
        make $MAKE_ARGS APP=brewblox PLATFORM=p1
        cp target/brewblox-p1/brewblox.bin ../docker/firmware-bin/source/brewblox-p1.bin
        make $MAKE_ARGS APP=brewblox PLATFORM=photon
        cp target/brewblox-photon/brewblox.bin ../docker/firmware-bin/source/brewblox-photon.bin
    '

FILE="./firmware-bin/source/firmware.ini"
touch $FILE

echo "[FIRMWARE]" >> $FILE
echo "firmware_version=$(git rev-parse --short HEAD)" >> $FILE
echo "firmware_date=$(git show -s --format=%ci)" >> $FILE
echo "proto_version=$(cd $MY_DIR/../app/brewblox/proto; git rev-parse --short HEAD)" >> $FILE
echo "proto_date=$(cd $MY_DIR/../app/brewblox/proto; git show -s --format=%ci)" >> $FILE

docker build --no-cache -t brewblox/firmware-bin:local firmware-bin
