FROM brewblox/firmware-particle:rpi-latest

WORKDIR /app
ENV PARTICLE_VERSION=1.0.0
ENV PARTICLE_RELEASES=https://github.com/particle-iot/firmware/releases/download/v${PARTICLE_VERSION}


RUN apk add --no-cache usbutils curl bash \
    && echo 'python serial-switcher.py' > ./prepare \
    && echo 'python serial-switcher.py -b 28800' > ./prepare-bootloader \
    \
    && curl -sL -o bootloader-p1.bin ${PARTICLE_RELEASES}/bootloader-${PARTICLE_VERSION}-p1.bin \
    && curl -sL -o system-part1-p1.bin ${PARTICLE_RELEASES}/system-part1-${PARTICLE_VERSION}-p1.bin \
    && curl -sL -o system-part2-p1.bin ${PARTICLE_RELEASES}/system-part2-${PARTICLE_VERSION}-p1.bin \
    \
    && curl -sL -o bootloader-photon.bin ${PARTICLE_RELEASES}/bootloader-${PARTICLE_VERSION}-photon.bin \
    && curl -sL -o system-part1-photon.bin ${PARTICLE_RELEASES}/system-part1-${PARTICLE_VERSION}-photon.bin \
    && curl -sL -o system-part2-photon.bin ${PARTICLE_RELEASES}/system-part2-${PARTICLE_VERSION}-photon.bin \
    && apk del curl


COPY ./brewblox-p1.bin ./
COPY ./brewblox-photon.bin ./
COPY ./serial-switcher.py ./
COPY ./flash ./
COPY ./flash-bootloader ./


ENTRYPOINT [ "bash" ]

# The container must be restarted after preparing the device
# Example calls:

# docker run --rm --privileged brewblox/firmware-flasher:rpi-local prepare
# docker run --rm --privileged brewblox/firmware-flasher:rpi-local flash
# docker run --rm --privileged brewblox/firmware-flasher:rpi-local prepare-bootloader
# docker run --rm --privileged -it brewblox/firmware-flasher:rpi-local flash-bootloader
