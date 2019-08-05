ARG base_image=minimal_deps
FROM $base_image

#Set up code and env vars
COPY . /re/

#Install re
RUN     cd re && \
        rm -rf build && \
        mkdir build && \
        cd build && \
        cmake  -G Ninja ..  && \
        cmake --build . --target aggregation_broker && \
        cmake --build . --target aggregation_server && \
        cd /
