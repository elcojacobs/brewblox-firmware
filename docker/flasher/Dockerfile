FROM brewblox/firmware-particle

WORKDIR /app

COPY ./brewblox/brewblox.bin ./
COPY ./serial-switcher.py ./

RUN echo 'python serial-switcher.py' > ./prepare \
    && echo 'node_modules/.bin/particle flash --usb --yes brewblox.bin' > ./flash

ENTRYPOINT [ "bash" ]

# The container must be restarted after preparing the device
# Example calls:

# docker run --rm --privileged brewblox/firmware-flasher:local prepare
# docker run --rm --privileged brewblox/firmware-flasher:local flash
