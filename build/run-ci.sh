#!/bin/bash
set -e

MY_DIR=$(dirname "$(readlink -f "$0")")
pushd "$MY_DIR" > /dev/null

bash compile-proto.sh
bash build-tests.sh
bash run-tests.sh

popd > /dev/null