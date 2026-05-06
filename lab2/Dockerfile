FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

ENV WORKDIR=/data

RUN apt-get update && apt-get install -y neovim vim nano zip curl build-essential python3-dev cmake git flex \
    bison libglib2.0-dev libpixman-1-dev python3-setuptools cargo clang libtool-bin llvm \
    tree gdb wget tar tmux texinfo nasm tcl-dev 

RUN apt-get install -y gcc-$(gcc --version|head -n1|sed 's/\..*//'|sed 's/.* //')-plugin-dev libstdc++-$(gcc --version|head -n1|sed 's/\..*//'|sed 's/.* //')-dev

RUN mkdir -p ${WORKDIR}

RUN cd ${WORKDIR} && git clone --no-checkout https://github.com/AFLplusplus/AFLplusplus && \
    cd AFLplusplus && git checkout v4.31c && NO_NYX=1 make source-only -j2

ENV CC=${WORKDIR}/AFLplusplus/afl-clang-fast 
ENV CXX=${WORKDIR}/AFLplusplus/afl-clang-fast++
ENV LD=${WORKDIR}/AFLplusplus/afl-clang-fast 

# libxml2
RUN cd ${WORKDIR} && git clone --no-checkout https://gitlab.gnome.org/GNOME/libxml2.git && \
    cd ${WORKDIR}/libxml2 && git checkout v2.14.1 && ./autogen.sh && ./configure --enable-shared=no && \
    make -j4

# sqlite3
RUN cd ${WORKDIR} && git clone --no-checkout https://github.com/sqlite/sqlite.git && cd sqlite && \
    git checkout version-3.44.4 && mkdir build && cd build && ../configure && make -j4 sqlite3

# libtiff
RUN cd ${WORKDIR} && git clone --no-checkout https://gitlab.com/libtiff/libtiff.git && cd libtiff && \
    git checkout v4.7.0 && (./autogen.sh || true) && ./configure --disable-shared && make -j4

# libvpx
RUN cd ${WORKDIR} && git clone --no-checkout https://github.com/webmproject/libvpx.git && cd libvpx && \
    git checkout v1.15.0 && mkdir build_fuzz && cd build_fuzz && \
    ../configure --enable-vp9-highbitdepth --disable-unit-tests --size-limit=12288x12288 --disable-webm-io && \
    make -j4

# zlib
RUN cd ${WORKDIR} && git clone --no-checkout https://github.com/madler/zlib.git && cd zlib && \
    git checkout v1.3.1 && mkdir build && cd build && cmake .. -DBUILD_SHARED_LIBS=off -DCMAKE_BUILD_TYPE=Release && \
    make -j4

# openssl for libfuzzer
RUN cd ${WORKDIR} && git clone --no-checkout https://github.com/openssl/openssl.git \
  && cd openssl && git checkout openssl-3.5 && \
  export PATH_TO_LIBFUZZER=$(clang -print-file-name=libclang_rt.fuzzer-x86_64.a) && \
  CC=clang \
  CFLAGS="-fsanitize=fuzzer-no-link,address,undefined -fno-sanitize=alignment" \
  CXXFLAGS="$CFLAGS" \
  ./config enable-fuzz-libfuzzer \
  --with-fuzzer-lib="$PATH_TO_LIBFUZZER" \
  -DPEDANTIC enable-asan enable-ubsan no-shared no-module\
  -DFUZZING_BUILD_MODE_UNSAFE_FOR_PRODUCTION \
  enable-ec_nistp_64_gcc_128 -fno-sanitize=alignment \
  enable-weak-ssl-ciphers enable-rc5 enable-md2 \
  enable-ssl3 enable-ssl3-method enable-nextprotoneg \
  --debug && \
  LDCMD=clang++ make -j4

# libtiff for libfuzzer
RUN cd ${WORKDIR} && git clone --no-checkout https://gitlab.com/libtiff/libtiff.git libtiff_libfuzzer && \
    cd libtiff_libfuzzer && git checkout v4.7.0 && (./autogen.sh || true) && \
    ./configure --disable-shared --prefix="${WORKDIR}/libtiff_libfuzzer/install_fuzz" \
       CXXFLAGS="-fsanitize=fuzzer-no-link,address" \
       CFLAGS="-fsanitize=fuzzer-no-link,address" && \
    make -j4 && make install && make clean && \
    ./configure --disable-shared --prefix="${WORKDIR}/libtiff_libfuzzer/install_debug" \
       CXXFLAGS="-fsanitize=address -g -fPIE" \
       CFLAGS="-fsanitize=address -g -fPIE" && \
    make -j4 && make install && make clean && \
    ./configure --disable-shared --prefix="${WORKDIR}/libtiff_libfuzzer/install_coverage" \
       CXXFLAGS="-fprofile-instr-generate -fcoverage-mapping -g" \
       CFLAGS="-fprofile-instr-generate -fcoverage-mapping -g" && \
    make -j4 && make install 

# include broken libtiff driver
RUN mkdir -p ${WORKDIR}/libtiff_driver
COPY driver.cc ${WORKDIR}/libtiff_driver

# COPY submit.sh /submit.sh
# RUN chmod +x /submit.sh

# Set default command
CMD ["tail", "-f", "/dev/null"]
