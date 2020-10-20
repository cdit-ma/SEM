find_package(Boost 1.30.0 REQUIRED)

add_library(sem_types STATIC)
target_compile_features(sem_types PUBLIC cxx_std_17)
target_compile_options(sem_types PRIVATE -fPIC)
target_sources(sem_types
        INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/serializable.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ipv4.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/socketaddress.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/timeout.hpp
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/uuid.h
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/uuid.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ipv4.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/socketaddress.cpp
        )


target_include_directories(sem_types
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        )

target_link_libraries(sem_types
        PUBLIC
        # Include boost headers to get uuid.hpp and uuid_generators.hpp
        Boost::boost
        )

add_subdirectory(proto)

if (BUILD_TEST)
    add_subdirectory(test)
endif ()
