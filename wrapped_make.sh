#! /usr/bin/env bash

MY_DIR=$(dirname $(readlink -f $0))

shopt -s globstar
compiledb make "$@"
rm "$MY_DIR/compile_commands.json"
pushd "$MY_DIR" > /dev/null
cat ./**/compile_commands.json > compile_commands.json \
  && sed -i -e ':a;N;$!ba;s/\]\n\n\[/,/g' compile_commands.json
popd > /dev/null
rm "compile_commands.json"