project(re_types VERSION ${PROJECT_VERSION})

find_package(Boost 1.30.0 REQUIRED)

add_library(${PROJECT_NAME} STATIC "")
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
target_compile_options(${PROJECT_NAME} PRIVATE -fPIC)
target_sources(${PROJECT_NAME} PRIVATE
        serializable.hpp
        uuid.cpp
        timeout.h
        uuid.h
        ipv4.hpp
        socketaddress.hpp)

add_subdirectory(proto)

if (BUILD_TEST)
    add_subdirectory(test)
endif ()
