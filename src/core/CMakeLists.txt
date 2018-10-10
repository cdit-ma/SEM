set(PROJ_NAME "re_core")
project(${PROJ_NAME})

find_package(Boost 1.30.0 COMPONENTS thread REQUIRED)

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/activatable.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/behaviourcontainer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/component.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/basemessage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ports/port.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ports/periodicport.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/loggerprinter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/loggerproxy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/attribute.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/worker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/translate.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/threadmanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/logger.cpp
    )

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/activatable.h
    ${CMAKE_CURRENT_SOURCE_DIR}/behaviourcontainer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/component.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ports/port.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ports/periodicport.h
    ${CMAKE_CURRENT_SOURCE_DIR}/basemessage.h
    ${CMAKE_CURRENT_SOURCE_DIR}/loggerprinter.h
    ${CMAKE_CURRENT_SOURCE_DIR}/loggerproxy.h
    ${CMAKE_CURRENT_SOURCE_DIR}/logger.h
    ${CMAKE_CURRENT_SOURCE_DIR}/attribute.h
    ${CMAKE_CURRENT_SOURCE_DIR}/worker.h
    ${CMAKE_CURRENT_SOURCE_DIR}/translate.h
    ${CMAKE_CURRENT_SOURCE_DIR}/threadmanager.h
    )

add_library(${PROJ_NAME} STATIC ${SOURCE} ${HEADERS})

# Compile Definitions
target_compile_definitions(${PROJ_NAME} PUBLIC BOOST_ALL_DYN_LINK)

# Include Directories
target_include_directories(${PROJ_NAME} PUBLIC ${Boost_INCLUDE_DIRS})
target_include_directories(${PROJ_NAME} PUBLIC ${RE_SOURCE_PATH})

# Link Libraries
target_link_libraries(${PROJ_NAME} PUBLIC zmq_protowriter)
target_link_libraries(${PROJ_NAME} PUBLIC re_common_proto_modelevent)
target_link_libraries(${PROJ_NAME} PUBLIC re_common_proto_control)
target_link_libraries(${PROJ_NAME} PUBLIC ${Boost_THREAD_LIBRARY})

if(UNIX)
    target_link_libraries(${PROJ_NAME} PRIVATE pthread)
endif(UNIX)