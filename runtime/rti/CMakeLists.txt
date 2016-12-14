cmake_minimum_required (VERSION 2.6)

#Use C++11
set(CMAKE_CXX_STANDARD 11)
#Set our module path

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

set(PROJ_NAME rti_library)
project (${PROJ_NAME})

find_package(DDS REQUIRED)
include_directories(${PROJ_NAME} ${DDS_INCLUDE_DIRS})


set(RUNTIME_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/../includes/)

include_directories(${PROJ_NAME} ${RUNTIME_INCLUDE}/)

set(SOURCE
    #Port Implementations
    ${CMAKE_CURRENT_SOURCE_DIR}/rtitxmessage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/rtirxmessage.cpp

    #Message Conversion
    ${CMAKE_CURRENT_SOURCE_DIR}/messageconvert.cpp

    #include the message type
    ${CMAKE_CURRENT_SOURCE_DIR}/../message.cpp
)

set(HEADERS
    #Port Implementations
    ${CMAKE_CURRENT_SOURCE_DIR}/rtitxmessage.h
    ${CMAKE_CURRENT_SOURCE_DIR}/rtirxmessage.h

    #Message Conversion
    ${CMAKE_CURRENT_SOURCE_DIR}/messageconvert.h

    #include the message type
    ${CMAKE_CURRENT_SOURCE_DIR}/../message.h
)

set(DDS_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/message.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/messageImpl.h
    ${CMAKE_CURRENT_SOURCE_DIR}/messageImplPlugin.h
)

set(DDS_SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/message.cxx
    ${CMAKE_CURRENT_SOURCE_DIR}/messageImpl.cxx
    ${CMAKE_CURRENT_SOURCE_DIR}/messageImplPlugin.cxx
)


add_library(${PROJ_NAME} STATIC ${SOURCE} ${HEADERS} ${DDS_SOURCE} ${DDS_HEADERS})
target_compile_features(${PROJ_NAME} PRIVATE cxx_range_for)

target_compile_definitions(${PROJ_NAME} PRIVATE -DRTI_UNIX -DRTI_DARWIN -DRTI_64BIT)# -D_GLIBCXX_USE_CXX11_ABI=0)

target_link_libraries(${PROJ_NAME} ${DDS_LIBRARIES})
