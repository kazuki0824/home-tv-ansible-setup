FROM chinachu/mirakurun
WORKDIR /tmp

ADD ./recisdb /usr/bin/recisdb
ADD ./tschscan/ /tschscan/
#ADD ./recisdb_1.0.0_amd64.deb ./recisdb.deb
#ADD https://github.com/kazuki0824/recisdb-rs/releases/download/1.0.0/recisdb_1.0.0_amd64.deb ./recisdb.deb
RUN apt-get update -q && \
#   apt-get install -yq ./recisdb.deb && \
    apt-get install -yq --no-install-recommends \
        curl \
        cmake \
        git \
        ca-certificates \
        clang \
        && \
    apt-get upgrade -yq && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

#setup decoder
RUN git clone https://github.com/kazuki0824/libarib-bxx.git /tmp/libaribb25 && \
    mkdir -p /tmp/b25_build && \
    cd /tmp/b25_build && \
    cmake ../libaribb25/ -DCMAKE_BUILD_TYPE=Release && \
    make && make install && \
    cmake ../libaribb25/ -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/tmp/export-b25 && \
    make && make install && \
    rm -r /tmp/libaribb25

WORKDIR /app
