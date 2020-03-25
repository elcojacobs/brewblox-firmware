#! /usr/bin/env bash
set -e

pushd "$(dirname "$(readlink -f "$0")")" > /dev/null

docker-compose down
touch .env
ENV_MOUNTDIR="$(grep MOUNTDIR .env)"

if [ -z "$ENV_MOUNTDIR" ]; then
  MOUNTDIR=$(dirname "$(pwd)")
  echo "Configuring '$MOUNTDIR' as mount path in container"
  echo "MOUNTDIR=$MOUNTDIR" >> .env
else
  echo "Using mount path '$ENV_MOUNTDIR'"
fi

ENV_MAKE_ARGS="$(grep MAKE_ARGS .env)"
if [ -z "$ENV_MAKE_ARGS" ]; then
  TOTALRAM=$(grep MemTotal /proc/meminfo | awk '{print $2}')
  if [ ${TOTALRAM} -gt 10000000 ] ; then \
    echo "detected over 10GB ram, using parallel build"
    MAKE_ARGS=-j
  else
    MAKE_ARGS=
  fi
  echo "Using '$MAKE_ARGS' as MAKE_ARGS"
  echo "MAKE_ARGS=$MAKE_ARGS" >> .env
else
  echo "Using make args '$ENV_MAKE_ARGS'"
fi

ENV_UID="$(grep DOCKER_UID .env)"
ENV_GID="$(grep DOCKER_GID .env)"
if [ -z "$ENV_UID" ]; then
  DOCKER_UID=$(id -u)
  echo "Configuring DOCKER_UID=$DOCKER_UID"
  echo "DOCKER_UID=$DOCKER_UID" >> .env
else
  echo "Using $ENV_UID"
fi
if [ -z "$ENV_GID" ]; then
  DOCKER_GID=$(id -u)
  echo "Configuring DOCKER_GID=$DOCKER_GID"
  echo "DOCKER_GID=$DOCKER_GID" >> .env
else
  echo "Using $ENV_GID"
fi

docker-compose up -d compiler
popd > /dev/null
