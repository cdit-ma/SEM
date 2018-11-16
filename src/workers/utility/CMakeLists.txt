set(PROJ_NAME utility_worker)
project(${PROJ_NAME})

find_package(Boost 1.30.0 REQUIRED)

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/utility_worker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utility_worker_impl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/exprtk.hpp
    )

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/utility_worker.h
    ${CMAKE_CURRENT_SOURCE_DIR}/utility_worker_impl.h
    )

add_library(${PROJ_NAME} SHARED ${SOURCE} ${HEADERS})

# Include Directories
target_include_directories(${PROJ_NAME} PUBLIC ${Boost_INCLUDE_DIRS})

# Link Libraries
target_link_libraries(${PROJ_NAME} re_core)
target_link_libraries(${PROJ_NAME} re_loggers)

if (MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /bigobj")
endif(MSVC)