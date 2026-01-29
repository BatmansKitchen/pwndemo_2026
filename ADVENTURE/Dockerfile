FROM ubuntu AS app
RUN apt-get update && apt-get install -y \
    g++ make gdb curl git \
    && rm -rf /var/lib/apt/lists/*

FROM pwn.red/jail
COPY --from=app / /srv

ENV JAIL_TIME="300"
ENV JAIL_TMP_SIZE="5M"

RUN mkdir /srv/app
COPY *.c *.h Makefile /srv/app/
RUN cd /srv/app && make
