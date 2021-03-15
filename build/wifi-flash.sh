#!/bin/bash
set -e

MY_DIR=$(dirname "$(readlink -f "$0")")

scp "$MY_DIR/target/brewblox-p1/brewblox-p1.bin" elco@diskstation:brewblox/binaries
# run on server: 'sudo docker cp binaries/brewblox-p1.bin brewblox_sparkey_1:/app/binaries/'
# flash from ui