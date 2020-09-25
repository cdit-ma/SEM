ARG base_image=cdit-ma/full_deps
FROM $base_image

#Set up code and env vars
COPY . /SEM/
COPY re/docker/re_full/configure.sh configure.sh
RUN chmod +x configure.sh
ENV CMAKE_MODULE_PATH /SEM/re/cmake_modules/
ENV RE_PATH /SEM/re

#Change to using bash while we run our configure scripts and build.
RUN ln -sf /bin/bash /bin/sh
#Install re
RUN     cd re && \
        . /opt/HDE/x86_64.linux/release.com && \
        . /opt/RTI/rti_connext_dds-5.3.0/resource/scripts/rtisetenv_x64Linux3gcc5.4.0.bash && \
        rm -rf build && \
        mkdir -p build && \
        cd build && \
        cmake -G Ninja .. -DBUILD_MEDEA=OFF&& \
        cmake --build .
#Change back to using dash
RUN ln -sf /bin/dash /bin/sh

ENV PATH="/SEM/build/lib:${PATH}"

ENV LD_LIBRARY_PATH /SEM/build/lib
RUN ldconfig
