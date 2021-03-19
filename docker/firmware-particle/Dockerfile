FROM node:14-buster-slim

WORKDIR /app

RUN set -ex \
  && apt-get update \
  && apt-get install -y make gcc g++ udev libudev-dev python python-pip usbutils curl dfu-util \
  && python -m pip install esptool \
  && npm install serialport --build-from-source --unsafe-perm \
  && npm install particle-cli --unsafe-perm \
  && npm install particle-usb \
  && apt-get remove -y make gcc g++ \
  && rm -rf /var/lib/apt/lists/*

COPY particle /usr/local/bin/particle

ENTRYPOINT [ "/bin/bash" ]
