#!/bin/bash

# generates cpp with google's protobuf implementation. Not used in firmware, but used in unit test code
# do some renames so the names don't cause conflicts when both are used

PROTO_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
NANOPB_PATH="$(readlink -f "${PROTO_DIR}/../../../platform/spark/device-os/nanopb/nanopb")"
pushd "$PROTO_DIR" > /dev/null # .option files are read from execution directory, so have to cd into this dir 

mkdir -p "test/proto"
mkdir -p "test/cpp"
mkdir -p "test/tmp_cpp"

# copy proto files with .test appended and fix includes 
for file in *.proto 
do
  testfile="test/proto/${file%.proto}.test.proto"
  cp -f "$file" "$testfile"
  sed -i 's/brewblox.proto/brewblox.test.proto/g' "$testfile"
  sed -i 's/ActuatorDigital.proto/ActuatorDigital.test.proto/g' "$testfile"
  sed -i 's/AnalogConstraints.proto/AnalogConstraints.test.proto/g' "$testfile"
  sed -i 's/DigitalConstraints.proto/DigitalConstraints.test.proto/g' "$testfile"
  sed -i 's/BrewbloxOptions/BrewbloxTestOptions/g' "$testfile"
done

# generate code
cd test/proto
cp ${NANOPB_PATH}/generator/proto/nanopb.proto .
protoc *.proto --cpp_out=../tmp_cpp --proto_path ${PROTO_DIR}/test/proto -I${NANOPB_PATH}/generator/proto

#rename .cc files to .cpp
cd ../tmp_cpp
for file in *.cc 
do
  rsync --checksum "$file" "../cpp/${file%.cc}.cpp"
done
for file in *.h 
do
  rsync --checksum "$file" "../cpp/${file}"
done
popd > /dev/null
