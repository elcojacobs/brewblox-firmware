FROM brewblox/rpi-node:11

WORKDIR /app

RUN apt-get update \
    && npm install particle-cli \
    && apt-get install -y dfu-util python-pip \
    && pip install pyserial

ENTRYPOINT [ "node_modules/.bin/particle" ]