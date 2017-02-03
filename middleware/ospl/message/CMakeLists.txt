#Get the RE_PATH
set(RE_PATH "$ENV{RE_PATH}")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${RE_PATH}/cmake/modules")

set(PROJ_NAME ospl_message)
project (${PROJ_NAME})

#Find ospl library
find_package(OpenSplice REQUIRED)

OSPL_GENERATE_CPP(DDS_SOURCE DDS_HEADERS message.idl)

#Find re_core library
find_library(RE_CORE_LIBRARIES re_core "${RE_PATH}/lib")
find_library(OSPL_HELPER_LIBRARIES ospl_helper "${RE_PATH}/lib")

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/libportexports.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tx.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/rx.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/convert.cpp
)

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/tx.h
    ${CMAKE_CURRENT_SOURCE_DIR}/rx.h
    ${CMAKE_CURRENT_SOURCE_DIR}/convert.h
)

include_directories(${RE_PATH}/includes)
include_directories(${OpenSplice_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(${PROJ_NAME} MODULE ${SOURCE} ${HEADERS} ${DDS_SOURCE} ${DDS_HEADERS})

target_link_libraries(${PROJ_NAME} ${RE_CORE_LIBRARIES})
target_link_libraries(${PROJ_NAME} ${OpenSplice_LIBRARIES})
target_link_libraries(${PROJ_NAME} ${OSPL_HELPER_LIBRARIES})

target_link_libraries(${PROJ_NAME} pthread)
target_link_libraries(${PROJ_NAME} datatypes_message)