#! /usr/bin/env bash
set -e

IMAGE=brewblox/firmware-simulator

# build AMD

docker-compose exec -T compiler \
    bash -c "
        set -e
        rm -rf ../docker/simulator/amd/target
        bash compile-proto.sh
        make $MAKE_ARGS APP=brewblox PLATFORM=gcc
        cp -r target ../docker/simulator/amd/target
    "

docker build \
    --tag $IMAGE:amd-local \
    simulator/amd

# build ARM

docker run \
    --rm \
    --privileged \
    docker/binfmt:a7996909642ee92942dcd6cff44b9b95f08dad64

rm -rf ../docker/simulator/arm/target

# remove unsupported arg
sed -i 's/-m64//g' ../platform/spark/device-os/build/gcc-tools.mk 

docker run \
    -v "$(pwd)"/../:/firmware/ \
    brewblox/firmware-compiler:arm-latest \
    make APP=brewblox PLATFORM=gcc

# reset modified file
pushd ../platform/spark/device-os > /dev/null
git checkout -- build/gcc-tools.mk 
popd > /dev/null

# copy build files to context
cp -r ../build/target simulator/arm/target

docker build \
    --tag $IMAGE:arm-local \
    --platform linux/arm/v7 \
    simulator/arm
