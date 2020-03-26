#!/usr/bin/env bash

set -ex

# todo: linker finds wrong/incompatible files
# could be any of:
# - need to use right linker
# - need to set dynamic library path to Rpi-staging
# - boost installed in wrong way?
# - linking to host boost instead of target boost?

make \
    BOOST_ROOT=/usr/include \
    CFLAGS="--sysroot=/home/develop/RPi-sysroot" \
    LDFLAGS="--sysroot=/home/develop/RPi-sysroot" \
    CC="armv8-rpi3-linux-gnueabihf-gcc" \
    LD="armv8-rpi3-linux-gnueabihf-ld" \
    CXX="armv8-rpi3-linux-gnueabihf-g++" \
    -C /home/elco/repos/firmware/build APP=brewblox PLATFORM=gcc clean all

