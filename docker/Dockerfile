# syntax=docker/dockerfile:1.7
#
# Sonic Mania Plus -> Sega Dreamcast build environment
# ----------------------------------------------------
# Provides a reproducible KallistiOS (KOS) SH4 cross toolchain plus the
# sh4zam math library, matching what dependencies/RSDKv5 (branch
# sf94/dreamcast-kallistios-pvr) expects:
#
#   - toolchain lives at /opt/toolchains/dc/kos          (KallistiOS.cfg hardcode)
#   - kos-cc / kos-c++ / kos-strip wrappers on PATH
#   - sh4zam installed into the KOS tree (Scene.cpp/Math.cpp #include <sh4zam/...>)
#   - RETRO_REVISION=2 only (v5U/rev3 is FATAL_ERROR on KOS: "will never fit")
#
# The image only contains the TOOLCHAIN. The game source is bind-mounted at
# build time (see docker/build.sh) so edits on the host branch are picked up
# without rebuilding the image.
#
# Build args let you pin versions for reproducibility.
ARG BASE=debian:bookworm-slim

# =============================================================================
# Stage 1: base system + build prerequisites for dc-chain
# =============================================================================
FROM ${BASE} AS base

ENV DEBIAN_FRONTEND=noninteractive \
    DC=/opt/toolchains/dc \
    KOS_BASE=/opt/toolchains/dc/kos \
    KOS_PORTS=/opt/toolchains/dc/kos-ports

RUN apt-get update && apt-get install -y --no-install-recommends \
        build-essential ca-certificates curl wget git \
        pkg-config cmake ninja-build \
        gawk bison flex sed texinfo \
        libgmp-dev libmpfr-dev libmpc-dev libelf-dev \
        libjpeg-dev libpng-dev \
        python3 python3-pip \
        bzip2 gzip tar xz-utils patch rsync \
        genisoimage \
    && rm -rf /var/lib/apt/lists/*

# =============================================================================
# Stage 2: KallistiOS dc-chain (SH4 + ARM cross compilers)
# =============================================================================
FROM base AS toolchain

# Pin KallistiOS. Override with --build-arg KOS_REF=<sha|branch>.
ARG KOS_REPO=https://github.com/KallistiOS/KallistiOS
ARG KOS_REF=master
# GCC version for the SH4 target. sh4zam wants a modern SH-GCC (15/16.x).
ARG DC_GCC_VER=15.1.0
ARG DC_BINUTILS_VER=2.44
ARG DC_NEWLIB_VER=4.5.0.20241231
ARG MAKE_JOBS=8

RUN git clone ${KOS_REPO} ${KOS_BASE} \
    && cd ${KOS_BASE} && git checkout ${KOS_REF}

# --- dc-chain: download + build the SH4/ARM GCC cross toolchain -------------
WORKDIR ${KOS_BASE}/utils/dc-chain
RUN cp Makefile.default.cfg Makefile.cfg \
    && sed -i "s/^sh_gcc_ver=.*/sh_gcc_ver=${DC_GCC_VER}/"           Makefile.cfg 2>/dev/null || true \
    && sed -i "s/^sh_binutils_ver=.*/sh_binutils_ver=${DC_BINUTILS_VER}/" Makefile.cfg 2>/dev/null || true \
    && sed -i "s/^makejobs=.*/makejobs=-j${MAKE_JOBS}/"              Makefile.cfg 2>/dev/null || true
RUN ./download.sh && ./unpack.sh
RUN make -j1 2>&1 | tail -80    # dc-chain drives its own -j internally
RUN ./cleanup.sh || true

# =============================================================================
# Stage 3: build KOS itself + sh4zam against the toolchain
# =============================================================================
FROM toolchain AS kos

ENV PATH=${DC}/sh-elf/bin:${DC}/arm-eabi/bin:${PATH}

# KOS environ config (points KOS_BASE / KOS_PORTS, sets CFLAGS)
RUN cp ${KOS_BASE}/doc/environ.sh.sample ${KOS_BASE}/environ.sh \
    && sed -i "s|^export KOS_BASE=.*|export KOS_BASE=\"${KOS_BASE}\"|" ${KOS_BASE}/environ.sh \
    && sed -i "s|^export KOS_PORTS=.*|export KOS_PORTS=\"${KOS_PORTS}\"|" ${KOS_BASE}/environ.sh

# Build the KOS kernel/libc
RUN bash -c 'source ${KOS_BASE}/environ.sh && cd ${KOS_BASE} && make -j${MAKE_JOBS:-8}' 2>&1 | tail -40

# kos-ports (needed for libogg/theora etc used by the engine video path)
ARG KOSPORTS_REPO=https://github.com/KallistiOS/kos-ports
RUN git clone ${KOSPORTS_REPO} ${KOS_PORTS}

# sh4zam math library (Falco Girgis) -> installs headers+lib into KOS tree
ARG SH4ZAM_REPO=https://github.com/gyrovorbis/sh4zam
ARG SH4ZAM_REF=master
RUN git clone ${SH4ZAM_REPO} /tmp/sh4zam && cd /tmp/sh4zam && git checkout ${SH4ZAM_REF}
# The KOS toolchain file already routes headers -> $KOS_ADDONS/include and
# libs -> $KOS_ADDONS/lib/$KOS_ARCH, and sets kos-cc/kos-c++. Do NOT override
# CMAKE_INSTALL_PREFIX or it lands outside the tree the engine searches.
RUN bash -c 'source ${KOS_BASE}/environ.sh \
      && cd /tmp/sh4zam \
      && cmake -B build -G Ninja \
           -DCMAKE_TOOLCHAIN_FILE=${KOS_BASE}/utils/cmake/kallistios.toolchain.cmake \
           -DPLATFORM_DREAMCAST=ON -DCMAKE_BUILD_TYPE=Release \
      && cmake --build build \
      && cmake --install build' 2>&1 | tail -40

# Convenience entrypoint: always have the KOS environment sourced
RUN printf '#!/usr/bin/env bash\nset -e\nsource %s/environ.sh\nexec "$@"\n' "${KOS_BASE}" > /usr/local/bin/with-kos \
    && chmod +x /usr/local/bin/with-kos

WORKDIR /work
ENTRYPOINT ["/usr/local/bin/with-kos"]
CMD ["bash"]
