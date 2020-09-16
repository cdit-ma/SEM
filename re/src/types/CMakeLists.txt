find_package(Boost 1.30.0 REQUIRED)

add_library(re_types STATIC)
target_compile_features(re_types PUBLIC cxx_std_17)
target_compile_options(re_types PRIVATE -fPIC)
target_sources(re_types
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


target_include_directories(re_types
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        )

target_link_libraries(re_types
        PUBLIC
        Boost::boost
        PRIVATE
        # Include boost headers to get uuid.hpp and uuid_generators.hpp
        )

add_subdirectory(proto)

if (BUILD_TEST)
    add_subdirectory(test)
endif ()
