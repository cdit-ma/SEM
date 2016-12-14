cmake_minimum_required (VERSION 2.6)

#Use C++11
set(CMAKE_CXX_STANDARD 11)
#Set our module path

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(PROJ_NAME ospl_library)
project (${PROJ_NAME})

find_package(OpenSplice REQUIRED)
include_directories(${PROJ_NAME} ${OpenSplice_INCLUDE_DIRS})

set(RUNTIME_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/../includes/)
include_directories(${PROJ_NAME} ${RUNTIME_INCLUDE}/)

set(SOURCE
    #Port Implementations
    ${CMAKE_CURRENT_SOURCE_DIR}/ospltxmessage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/osplrxmessage.cpp

    #Message Conversion
    ${CMAKE_CURRENT_SOURCE_DIR}/messageconvert.cpp

    #include the message type
    ${CMAKE_CURRENT_SOURCE_DIR}/../message.cpp
)

set(HEADERS
    #Port Implementations
    ${CMAKE_CURRENT_SOURCE_DIR}/ospltxmessage.h
    ${CMAKE_CURRENT_SOURCE_DIR}/osplrxmessage.h

    #Message Conversion
    ${CMAKE_CURRENT_SOURCE_DIR}/messageconvert.h

    #include the message type
    ${CMAKE_CURRENT_SOURCE_DIR}/../message.h
)

set(DDS_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/message_DCPS.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/messageconvert.cpp
    
    ${CMAKE_CURRENT_SOURCE_DIR}/message.h
    ${CMAKE_CURRENT_SOURCE_DIR}/messageDcps.h
    ${CMAKE_CURRENT_SOURCE_DIR}/messageDcps_impl.h
    ${CMAKE_CURRENT_SOURCE_DIR}/messageSplDcps.h
)

set(DDS_SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/message.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/messageDcps.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/messageDcps_impl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/messageSplDcps.cpp
)


add_library(${PROJ_NAME} STATIC ${SOURCE} ${HEADERS} ${DDS_SOURCE} ${DDS_HEADERS})
target_compile_features(${PROJ_NAME} PRIVATE cxx_range_for)

#target_compile_definitions(${PROJ_NAME} PRIVATE -D_GLIBCXX_USE_CXX11_ABI=0)

target_link_libraries(${PROJ_NAME} ${OpenSplice_LIBRARIES})
