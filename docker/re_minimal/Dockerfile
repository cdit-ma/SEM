ARG base_image=minimal_deps
FROM $base_image

#Set up code and env vars
COPY . /re/
ENV CMAKE_MODULE_PATH /re/cmake_modules/
ENV RE_PATH /re
ENV PUGIXML_PATH /usr/local

#Install re
RUN     cd re && \
        mkdir build && \
        cd build && \
        cmake  -G Ninja ..  && \
        cmake --build .  && \
        cd /
ENV LD_LIBRARY_PATH /re/lib

RUN ldconfig
