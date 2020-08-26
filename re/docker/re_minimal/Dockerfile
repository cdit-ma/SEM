ARG base_image=cdit-ma/minimal_deps
FROM $base_image

#Set up code and env vars
COPY . /SEM/
ENV CMAKE_MODULE_PATH /SEM/re/cmake_modules/
ENV RE_PATH /SEM/re

#Install re
RUN     cd re && \
        mkdir -p build && \
        cd build && \
        cmake  -G Ninja ..  -DBUILD_MEDEA=OFF && \
        cmake --build .  && \
        cd /
ENV LD_LIBRARY_PATH /SEM/re/lib

RUN ldconfig
