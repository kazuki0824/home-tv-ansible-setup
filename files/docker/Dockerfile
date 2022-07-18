# syntax = docker/dockerfile:experimental
FROM rust:buster AS buildstage
WORKDIR /tmp
RUN apt-get update && \
    apt-get upgrade -y && \
    apt-get install -y --no-install-recommends \
        curl \
        cmake \
        git \
        ca-certificates \
        libpcsclite-dev \
        clang \
        && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

#setup decoder
RUN git clone https://github.com/stz2012/libarib25.git /tmp/libarib25 && \
    mkdir -p /tmp/b25_build && \
    cd /tmp/b25_build && \
    cmake ../libarib25/ -DCMAKE_BUILD_TYPE=Release && \
    make && make install && \
    cmake ../libarib25/ -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/tmp/export-b25 && \
    make && make install && \
    rm -r /tmp/libarib25
#setup device handler
RUN CARGO_LOC=$(dirname `which cargo`) ; printf "[net]\ngit-fetch-with-cli = true" > $CARGO_LOC/../config.toml
RUN mkdir -m 700 $HOME/.ssh
RUN ssh-keyscan github.com > $HOME/.ssh/known_hosts
RUN --mount=type=ssh,required=true cargo install --git ssh://git@github.com/kazuki0824/recisdb-rust.git --root /tmp/export-rec 

FROM chinachu/mirakurun:latest
ENV LD_LIBRARY_PATH $LD_LIBRARY_PATH:/usr/local/lib
COPY --from=buildstage /tmp/export-b25 /usr/local
RUN bash -c "[ -e /usr/local/lib/libarib25.so ]"
COPY --from=buildstage /tmp/export-rec/bin/* /rec/bin/
RUN bash -c "[ -e /rec/bin/recisdb-rust ]"
#EXPOSE 40772 9229
