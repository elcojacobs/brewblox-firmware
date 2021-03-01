#! /usr/bin/env bash
set -ex
pushd "$(dirname "$0")" > /dev/null

# This script builds a local version of the ARM64 firmware simulator
# This serves as a placeholder until multiplatform CI builds are available

bash ./start-compiler.sh
docker-compose exec -T compiler bash compile-proto.sh
bash ../build/build-sim-arm64.sh
