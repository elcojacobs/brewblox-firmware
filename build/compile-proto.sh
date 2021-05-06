#!/bin/bash
MY_DIR=$(dirname "$(readlink -f "$0")")

handle_error() {
  echo "Encountered error when executing $(basename $0)!" >&2
  echo "Error on line $(caller)" >&2
  exit 1
}
trap handle_error ERR

# rebuild generator, particle doesn't keep the compiled version up to date
pushd "$MY_DIR/../platform/spark/device-os/third_party/nanopb/nanopb/generator/proto" || exit 1
if [ -f nanopb_pb2.py ]; then
  rm nanopb_pb2.py 
fi
make
popd > /dev/null

pushd "$MY_DIR/../brewblox/blox/proto" || exit 1
echo -e "Compiling proto files using nanopb for brewblox firmware"
. generate_proto_cpp.sh

echo -e "Compiling proto files using google protobuf for unit tests"
. generate_proto_test_cpp.sh
popd > /dev/null

echo "Done"

pushd "$MY_DIR/../platform/spark/device-os/third_party/nanopb/nanopb/generator/proto" || exit 1
git checkout nanopb_pb2.py # revert submodule changes
exit
