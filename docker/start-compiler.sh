#! /usr/bin/env bash
set -e
export MOUNTDIR=$(dirname $(pwd))
docker-compose up -d compiler
