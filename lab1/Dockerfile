FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

ENV WORKDIR=/data   

RUN apt-get update && apt-get install -y neovim vim nano zip curl build-essential python3-dev cmake git flex \
    bison libglib2.0-dev libpixman-1-dev python3-setuptools cargo libgtk-3-dev clang libtool-bin llvm \
    tree gdb wget tar tmux texinfo

RUN apt-get install -y gcc-$(gcc --version|head -n1|sed 's/\..*//'|sed 's/.* //')-plugin-dev libstdc++-$(gcc --version|head -n1|sed 's/\..*//'|sed 's/.* //')-dev

RUN mkdir -p ${WORKDIR}

RUN cd ${WORKDIR} && git clone --no-checkout https://github.com/AFLplusplus/AFLplusplus && \
    cd AFLplusplus && git checkout v4.31c && NO_NYX=1 make source-only -j2

RUN cd ${WORKDIR} && git clone --no-checkout https://gitlab.gnome.org/GNOME/libxml2.git && \
    cd ${WORKDIR}/libxml2 && git checkout v2.14.1

COPY ./error.patch /data/libxml2

RUN cd ${WORKDIR}/libxml2 && git apply error.patch && rm error.patch

RUN cd ${WORKDIR}/libxml2 && ./autogen.sh && ./configure --enable-shared=no && \
    make CC=${WORKDIR}/AFLplusplus/afl-clang-fast CXX=${WORKDIR}/AFLplusplus/afl-clang-fast++ LD=${WORKDIR}/AFLplusplus/afl-clang-fast -j2

RUN mv ${WORKDIR}/libxml2/xmllint ${WORKDIR}/xmllint && rm -Rf ${WORKDIR}/libxml2 && mkdir ${WORKDIR}/libxml2 \
    && mv ${WORKDIR}/xmllint ${WORKDIR}/libxml2/xmllint

RUN cd ${WORKDIR} && wget https://ftp.gnu.org/gnu/binutils/binutils-2.44.tar.gz && \
    tar -xvzf binutils-2.44.tar.gz
RUN rm ${WORKDIR}/binutils-2.44.tar.gz

# COPY submit.sh /submit.sh
# RUN chmod +x /submit.sh

# Set default command
CMD ["tail", "-f", "/dev/null"]
