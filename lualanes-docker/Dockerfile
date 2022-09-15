FROM debian:bullseye

RUN apt-get update && apt-get install -y lua5.1 luarocks libxml2-dev git && apt-get clean
RUN luarocks install lanes

COPY test.lua /opt/
ENTRYPOINT ["/opt/test.lua"]
