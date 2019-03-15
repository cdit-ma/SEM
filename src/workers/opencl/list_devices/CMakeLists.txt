set(PROJ_NAME list_opencl_devices)
set(OPENCL_ROOT $ENV{OPENCL_ROOT})
set(OPENCL_INCLUDE_PATH $ENV{OPENCL_INCLUDE_PATH})
set(OPENCL_LIB_PATH $ENV{OPENCL_LIB_PATH})

project(${PROJ_NAME})

find_library(OPENCL_LIB OpenCL ${OPENCL_LIB_PATH})

if(NOT OPENCL_LIB)
    message(STATUS "Not Found OpenCL - Not building list_opencl_devices")
    return()
endif()

if(NOT OPENCL_INCLUDE_PATH)
    SET(OPENCL_INCLUDE_PATH "${OPENCL_ROOT}/include")
endif()

add_executable(${PROJ_NAME} "")

target_sources(${PROJ_NAME} PRIVATE
    # Sources
    ${CMAKE_CURRENT_SOURCE_DIR}/listdevices.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/jsonwriter.cpp
    # Headers
    ${CMAKE_CURRENT_SOURCE_DIR}/jsonwriter.h
)

# Include Directories
target_include_directories(${PROJ_NAME} PRIVATE ${OPENCL_INCLUDE_PATH})

# Link Libraries
target_link_libraries(${PROJ_NAME} PRIVATE ${OPENCL_LIB})