#! /usr/bin/env bash
set -ex

pushd "$(dirname "$(readlink -f "$0")")" > /dev/null

bash ./start-compiler.sh

docker-compose exec -T compiler bash compile-proto.sh

bash ../build/build-sim-arm.sh

popd > /dev/null

# The resulting executable must be manually copied from build/target/brewblox-gcc/brewblox
# To pull firmware binaries in devcon:
# - (devcon) bash pull-firmware.sh develop
# - (firmware) git checkout develop && git pull
# - (firmware) bash docker/build-bin-arm.sh
# - (firmware) cp build/target/brewblox-gcc/brewblox ../brewblox-devcon-spark/binaries/brewblox-arm
# - then commit and push devcon repository

# The devcon service checks running firmware vs the date/commit in the firmware.ini file.
# It is important to run build-bin-arm.sh vs the latest develop commit,
# and on the same day as that commit was built by azure-pipelines.
# You may need to manually restart the CI build.
