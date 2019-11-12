FROM brewblox/rpi-node:12-alpine

WORKDIR /app

RUN apk add --no-cache make gcc g++ python linux-headers udev py2-pip \
  && apk add --no-cache --repository http://dl-3.alpinelinux.org/alpine/edge/testing/ dfu-util \
  && npm install serialport --build-from-source --unsafe-perm \
  && npm install particle-cli --unsafe-perm \
  && pip install --upgrade pip \
  && pip install pyserial \
  && apk del make gcc g++ linux-headers py2-pip

ENTRYPOINT [ "node_modules/.bin/particle" ]
