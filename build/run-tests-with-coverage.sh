#!/bin/bash
MY_DIR=$(dirname $(readlink -f $0))
ROOT_DIR=$(dirname "$(readlink -f "$MY_DIR")")

CBOX_DIR="$ROOT_DIR/controlbox"
OUTPUT_DIR="$ROOT_DIR/build/coverage"

mkdir -p "$OUTPUT_DIR"

function run_test() {
    EXECUTABLE=$(readlink -f "$1")
    BUILD_DIR=$(dirname "$(readlink -f "$EXECUTABLE")")
    echo "running test executable:  $1"
    
    # Reset lcov counters
    lcov --zerocounters --directory "$BUILD_DIR" --quiet

    # Run lcov initial
    lcov --capture --initial --directory "$BUILD_DIR" --output-file "$OUTPUT_DIR/base.info" --quiet

    "$EXECUTABLE"

    lcov --capture --directory "$BUILD_DIR" --output-file "$OUTPUT_DIR/test.info" --quiet

    # combine tracefiles
    lcov --add-tracefile "$OUTPUT_DIR/base.info" --add-tracefile "$OUTPUT_DIR/test.info" --output-file "$2" --quiet
    rm "$OUTPUT_DIR/base.info" "$OUTPUT_DIR/test.info"
}

run_test "$ROOT_DIR/controlbox/build/cbox_test_runner" "$OUTPUT_DIR/controlbox.info"
run_test "$ROOT_DIR/lib/test/build/lib_test_runner" "$OUTPUT_DIR/lib.info"
run_test "$ROOT_DIR/app/brewblox/test/build/brewblox_test_runner" "$OUTPUT_DIR/brewblox.info"

lcov --quiet \
 --add-tracefile "$OUTPUT_DIR/lib.info" \
 --add-tracefile "$OUTPUT_DIR/controlbox.info" \
 --add-tracefile "$OUTPUT_DIR/brewblox.info" \
 --output-file "$OUTPUT_DIR/combined_traces.info" 

# filter tracefiles
lcov --remove "$OUTPUT_DIR/combined_traces.info" \
 '*/boost/*' \
 '/usr/*' \
 "$ROOT_DIR/platform/spark/device-os/*" \
 "$ROOT_DIR/app/brewblox/proto/*" \
 "$ROOT_DIR/lib/cnl/**" \
 -o "$OUTPUT_DIR/filtered_traces.info" --quiet

echo "Generating coverage report"
mkdir -p "$OUTPUT_DIR/html"
genhtml --prefix "$ROOT_DIR" "$OUTPUT_DIR/filtered_traces.info" \
 --ignore-errors source \
 --output-directory="$OUTPUT_DIR/html" \
 --quiet \
 --show-details \
 --title "Brewblox firmware unit tests"


