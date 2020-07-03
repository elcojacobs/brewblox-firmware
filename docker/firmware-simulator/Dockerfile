FROM debian:buster
WORKDIR /app

# set by buildx
ARG TARGETPLATFORM="linux/amd64"

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

COPY source/* /app/source/
COPY ./pick-binary.sh /app/pick-binary.sh

RUN bash ./pick-binary.sh "${TARGETPLATFORM}" \
    && rm -rf source \
    && rm ./pick-binary.sh

CMD ./brewblox --device_id=123456789012345678901234
