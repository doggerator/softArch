# ---- Release ----

FROM ubuntu:22.04
ENV TZ=Europe/Moscow
RUN apt-get clean && apt-get update -y 
RUN DEBIAN_FRONTEND=noninteractive TZ=Etc/UTC apt-get -y install \
    tzdata git sudo build-essential autoconf libtool libssl-dev \
    zlib1g-dev librdkafka-dev pkg-config cmake gcc git g++ \
    mysql-client libmysqlclient-dev libboost-all-dev \
    libssl-dev && apt-get clean

RUN git clone -b poco-1.12.4-release https://github.com/pocoproject/poco.git && \
    cd poco && \
    mkdir cmake-build && \
    cd cmake-build && \
    cmake .. && \
    make -j$(nproc) && \
    sudo make install

RUN git clone https://github.com/tdv/redis-cpp.git && \
    mkdir redis-cpp/cmake-build && \
    cd redis-cpp/cmake-build && \
    cmake .. && \
    make -j$(nproc) && \
    sudo make install
RUN git clone https://github.com/edenhill/librdkafka.git && cd librdkafka && ./configure && make && sudo make install && cd ..
RUN git clone https://github.com/mfontanini/cppkafka && cd cppkafka && mkdir build && cd build && cmake .. && make && sudo make install && cd .. && cd ..

# ---- Develop ----

#FROM uservice-build:hl

# ---- ---- ---- ----

WORKDIR /app
RUN sudo ldconfig

COPY CMakeLists.txt .
COPY common common
COPY user-service user-service
COPY delivery-service delivery-service

RUN mkdir build && cd build && cmake .. && make
ENTRYPOINT [ "/app/highload/build/user-service" ]
