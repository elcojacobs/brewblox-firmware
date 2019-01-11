FROM brewblox/firmware-particle

WORKDIR /app
ENV PARTICLE_VERSION=1.0.0
ENV PARTICLE_RELEASES=https://github.com/particle-iot/firmware/releases/download/v${PARTICLE_VERSION}


RUN apt-get update \
    && apt-get install -y usbutils \
    && echo 'python serial-switcher.py' > ./prepare \
    \
    && curl -sL -o bootloader-p1.bin ${PARTICLE_RELEASES}/bootloader-${PARTICLE_VERSION}-p1.bin \
    && curl -sL -o system-part1-p1.bin ${PARTICLE_RELEASES}/system-part1-${PARTICLE_VERSION}-p1.bin \
    && curl -sL -o system-part2-p1.bin ${PARTICLE_RELEASES}/system-part2-${PARTICLE_VERSION}-p1.bin \
    \
    && curl -sL -o bootloader-photon.bin ${PARTICLE_RELEASES}/bootloader-${PARTICLE_VERSION}-photon.bin \
    && curl -sL -o system-part1-photon.bin ${PARTICLE_RELEASES}/system-part1-${PARTICLE_VERSION}-photon.bin \
    && curl -sL -o system-part2-photon.bin ${PARTICLE_RELEASES}/system-part2-${PARTICLE_VERSION}-photon.bin


COPY ./brewblox-p1 ./brewblox-p1
COPY ./brewblox-photon ./brewblox-photon
COPY ./serial-switcher.py ./
COPY ./flash ./
COPY ./bootloader ./


ENTRYPOINT [ "bash" ]

# The container must be restarted after preparing the device
# Example calls:

# docker run --rm --privileged brewblox/firmware-flasher:local prepare
# docker run --rm --privileged brewblox/firmware-flasher:local flash
# docker run --rm --privileged -it brewblox/firmware-flasher:local bootloader
