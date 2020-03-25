FROM brewblox/brewblox-devcon-spark:develop
WORKDIR /app

# set by buildx
ARG TARGETPLATFORM

# status page
EXPOSE 80
# controlbox protocol
EXPOSE 8332
# debug port
EXPOSE 8333
# mdns
EXPOSE 5353/udp
# websocket display emulation
EXPOSE 7376

COPY source/brewblox-amd/brewblox-gcc/brewblox .

CMD ./brewblox --device_id=123456789012345678901234
