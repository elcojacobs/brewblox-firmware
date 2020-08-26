#! /usr/bin/env bash
set -e 
pushd () {
    command pushd "$@" > /dev/null
}
popd () {
    command popd > /dev/null
}

MY_DIR=$(dirname "$(readlink -f "$0")")
shopt -s globstar

python -m compiledb make $MAKE_ARGS $@ || exit 1

rm -f "$MY_DIR/compile_commands.json"
pushd "$MY_DIR"
cat ./**/compile_commands.json > compile_commands.json
sed -i -e ':a;N;$!ba;s/\]\n\n\[/,/g' compile_commands.json
chmod 777 compile_commands.json
find . -name "*.gcda" -type f -delete # remove old profile results
popd
rm -f "compile_commands.json"
