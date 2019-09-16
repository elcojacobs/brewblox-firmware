#!/bin/bash
set -e

MY_DIR=$(dirname "$(readlink -f "$0")")
ROOT_DIR=$(dirname "$(readlink -f "$MY_DIR")")

sudo chown -R $USER "$ROOT_DIR"
make -C "../app/brewblox/test" clean
make -C "../lib/test" clean
make -C "../controlbox" clean
make clean PLATFORM=gcc
make clean PLATFORM=p1
make clean PLATFORM=photon
