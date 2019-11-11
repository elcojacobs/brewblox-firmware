#!/bin/bash
set -e

MY_DIR=$(dirname "$(readlink -f "$0")")
ROOT_DIR=$(dirname "$(readlink -f "$MY_DIR")")

pushd "$ROOT_DIR"/docker > /dev/null
docker-compose exec compiler make -C "../app/brewblox/test" clean
docker-compose exec compiler make -C "../lib/test" clean
docker-compose exec compiler make -C "../controlbox" clean

docker-compose exec compiler make clean PLATFORM=gcc
docker-compose exec compiler make clean PLATFORM=p1
docker-compose exec compiler make clean PLATFORM=photon
popd > /dev/null

