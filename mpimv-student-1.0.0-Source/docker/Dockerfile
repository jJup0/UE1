FROM ubuntu:18.04

MAINTAINER Sascha Hunold <sascha.hunold@tuwien.ac.at>

RUN apt-get update \
 && apt-get install -y \
    curl \
    git \
    make \
    cmake \
    vim \
    wget \
    gcc \
    gdb \
    valgrind \
    libopenmpi-dev \
    openmpi-bin \
 && apt-get clean \
 && rm -rf /var/lib/apt/lists/*
