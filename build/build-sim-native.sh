#! /usr/bin/env bash
set -e
pushd "$(dirname "$0")" > /dev/null

make APP=brewblox PLATFORM=gcc
