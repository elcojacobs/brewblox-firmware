FROM node:10-alpine

WORKDIR /app

RUN apk add --no-cache make gcc g++ python linux-headers udev \
  && npm install serialport --build-from-source=serialport \
  && npm install particle-cli \
  && apk add --no-cache py2-pip \
  && apk add --no-cache --repository http://dl-3.alpinelinux.org/alpine/edge/testing/ dfu-util \
  && pip install pyserial \
  && apk del make gcc g++ linux-headers udev py2-pip

ENTRYPOINT [ "node_modules/.bin/particle" ]
