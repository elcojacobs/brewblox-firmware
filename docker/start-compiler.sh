#! /usr/bin/env bash
set -e
export MOUNTDIR=$(dirname $(pwd))
TOTALRAM=$(grep MemTotal /proc/meminfo | awk '{print $2}')
if [ ${TOTALRAM} -gt 10000000 ] ; then \
    echo "detected over 10GB ram, using parallel build"
    export MAKE_ARGS=-j
fi

docker-compose up -d compiler
