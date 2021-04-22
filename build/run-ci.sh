#!/bin/bash
set -e

MY_DIR=$(dirname "$(readlink -f "$0")")
pushd "$MY_DIR" > /dev/null

bash compile-proto.sh || exit 1
bash build-tests.sh || exit 1
bash run-tests.sh || exit 1

popd > /dev/null