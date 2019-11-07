#!/bin/bash
MY_DIR=$(dirname $(readlink -f $0))
BUILD_DIR="$MY_DIR/build"
OUTPUT_DIR="$BUILD_DIR/coverage"

mkdir -p "$OUTPUT_DIR"

echo "resetting lcov counters"
lcov --zerocounters --directory "$BUILD_DIR"

echo "running lcov initial"
lcov --capture --initial --directory "$BUILD_DIR" --output-file "$OUTPUT_DIR/base.info"

echo "running tests"
$BUILD_DIR/cbox_test_runner

echo "running lcov"
lcov --capture --directory "$BUILD_DIR" --output-file "$OUTPUT_DIR/test.info"

echo "combining tracefiles"
lcov --add-tracefile "$OUTPUT_DIR/base.info" --add-tracefile "$OUTPUT_DIR/test.info" --output-file "$OUTPUT_DIR/total.info"

echo "filtering tracefiles"
lcov --remove "$OUTPUT_DIR/total.info" '/boost/*' '/usr/*' "*/platform/spark/device-os/**" -o "$OUTPUT_DIR/filtered.info"

echo "generating html"
mkdir -p "$OUTPUT_DIR/html"
genhtml --prefix /firmware/controlbox/ "$OUTPUT_DIR/filtered.info" --ignore-errors source --output-directory="$OUTPUT_DIR/html"
