#!/bin/bash
set -e

MY_DIR=$(dirname "$(readlink -f "$0")")
ROOT_DIR=$(dirname "$(readlink -f "$MY_DIR")")

sudo chown -R $USER "$ROOT_DIR"
make -C "../app/brewblox/test" clean
make -C "../lib/test" clean
make -C "../cbox/test" clean
make clean
