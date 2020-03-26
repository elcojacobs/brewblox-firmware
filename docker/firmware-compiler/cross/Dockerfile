FROM tttapa/rpi3-armv8-develop-cross

USER root

# install various build deps
RUN apt-get update -q \
  && DEBIAN_FRONTEND=noninteractive apt-get install -y apt-utils \
  && DEBIAN_FRONTEND=noninteractive dpkg-reconfigure apt-utils \
  && DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
  build-essential autoconf automake libtool libprotobuf-dev gcc-multilib g++-multilib \
  git bash libboost-all-dev \
  # crc32 is not used, build particle's makefile does a 'which crc32', this dummy avoids an error
  && touch /usr/bin/crc32 \
  && chmod +x /usr/bin/crc32 \
  && apt-get clean \
  && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

USER develop

COPY build-cross.sh .

# use boost from apt
ENV BOOST_ROOT=/usr/include
