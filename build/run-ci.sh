#!/bin/bash
set -e

bash compile-proto.sh
bash build-tests.sh
bash run-tests.sh
