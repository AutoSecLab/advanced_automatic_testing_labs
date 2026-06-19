FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive
ENV WORKDIR=/data

RUN apt-get update && apt-get install -y \
    neovim vim nano zip curl build-essential python3-dev cmake git \
    clang libtool-bin llvm llvm-dev \
    tree gdb wget tar tmux python3

RUN mkdir -p ${WORKDIR}/libmsg

# Place source code in the container.
# Students are expected to compile this library themselves (see assignment).
COPY msgparser.h  ${WORKDIR}/libmsg/
COPY msgparser.c  ${WORKDIR}/libmsg/
COPY driver.cc    ${WORKDIR}/libmsg/
COPY crash_seed   ${WORKDIR}/libmsg/
# COPY get_coverage_report.sh ${WORKDIR}/libmsg/
# RUN chmod +x ${WORKDIR}/libmsg/get_coverage_report.sh

# Generate the crash seed that students will analyse in Task 2.
# COPY generate_crash.py ${WORKDIR}/libmsg/
# RUN python3 ${WORKDIR}/libmsg/generate_crash.py ${WORKDIR}/libmsg/crash_seed

# Pre-build the fuzzing binary so students can start a campaign immediately.
# They are asked to reproduce the debug and coverage builds themselves.
# RUN cd ${WORKDIR}/libmsg && \
#     clang++ -g -O1 -fsanitize=address,fuzzer \
#         driver.cc msgparser.c -I. -o msg_fuzz

COPY HINTS.md ${WORKDIR}/libmsg/

# Create an empty initial corpus directory.
RUN mkdir -p ${WORKDIR}/libmsg/corpus

# Set default command
CMD ["tail", "-f", "/dev/null"]
