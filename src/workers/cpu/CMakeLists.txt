set(PROJ_NAME cpu_worker)
project(${PROJ_NAME})

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/cpu_worker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cpu_worker_impl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/intop.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/floatop.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dhry_1.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/whets.cpp
    )

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/cpu_worker_impl.h
    ${CMAKE_CURRENT_SOURCE_DIR}/cpu_worker.h
    ${CMAKE_CURRENT_SOURCE_DIR}/dhry.h
    )

add_library(${PROJ_NAME} SHARED ${SOURCE} ${HEADERS})

# Disable Optimizing
if (UNIX)
    SET(NO_OPTIMIZE_FLAG "-O0")
elseif(MSVC)
    SET(NO_OPTIMIZE_FLAG "-Od /wd4996 /wd4244")
endif(UNIX)

SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} ${NO_OPTIMIZE_FLAG}")
SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${NO_OPTIMIZE_FLAG}")

SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${NO_OPTIMIZE_FLAG}")
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${NO_OPTIMIZE_FLAG}")

# Include Directories
target_include_directories(${PROJ_NAME} PUBLIC ${RE_SOURCE_PATH})

# Link Libraries
target_link_libraries(${PROJ_NAME} PUBLIC re_core)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/profiler")