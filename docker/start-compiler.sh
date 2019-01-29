#! /usr/bin/env bash
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

docker-compose up -d compiler
