FROM gcc:8

ENV DEBIAN_FRONTEND=noninteractive \
  DEBCONF_NONINTERACTIVE_SEEN=true

# install various build deps
RUN apt-get update -q \
  && apt-get install -qy --no-install-recommends build-essential autoconf automake libtool unzip pkg-config libprotobuf-dev protobuf-compiler git bash python python-pip rsync curl libboost-dev \
  && pip install --upgrade pip setuptools \
  # and install protobuf python package (used by nanopb), and pyserial (used by program-dfu)
  && pip install protobuf \
  # crc32 is not used, build particle's makefile does a 'which crc32', this dummy avoids an error
  && touch /usr/bin/crc32 \
  && chmod +x /usr/bin/crc32 \
  && apt-get clean \
  && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

# use boost from apt


ENV BOOST_ROOT=/usr/include/boost  

WORKDIR /firmware/build
CMD bash
