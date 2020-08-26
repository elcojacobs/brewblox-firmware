#!/bin/bash
MY_DIR=$(dirname "$(readlink -f "$0")")
ROOT_DIR=$(dirname "$(readlink -f "$MY_DIR")")

bash "$MY_DIR/run-tests.sh"
TEST_RESULT=$?

mkdir -p "$ROOT_DIR/build/coverage/html"
if [ "$1" = "html" ]
then
    FORMAT="html-details"
    FORMAT_ARGS='--html-title coverage'
    OUTPUT="$ROOT_DIR/build/coverage/index.html"
else
    FORMAT="xml"
    FORMAT_ARGS=
    OUTPUT="$ROOT_DIR/build/coverage/coverage.xml"
fi


gcovr --root "$ROOT_DIR" \
  "$ROOT_DIR/controlbox/build/" \
  "$ROOT_DIR/lib/test/build/" \
  "$ROOT_DIR/app/brewblox/test/build/" \
  -e '.*/boost/.*' \
  -e '^/usr/.*' \
  -e "$ROOT_DIR/platform/spark/device-os/.*" \
  -e "$ROOT_DIR/app/brewblox/proto/.*" \
  -e "$ROOT_DIR/lib/cnl/.*" \
  -e "$ROOT_DIR/controlbox/test/.*" \
  -e "$ROOT_DIR/lib/test/" \
  -e "$ROOT_DIR/app/brewblox/test/" \
  --$FORMAT \
  --output "$OUTPUT" \
  $FORMAT_ARGS \
  --delete \
  --print-summary

exit $TEST_RESULT