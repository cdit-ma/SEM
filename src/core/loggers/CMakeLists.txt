set(PROJ_NAME "re_loggers")
project(${PROJ_NAME})

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/logan_logger.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/print_logger.cpp
    )

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/logan_logger.h
    ${CMAKE_CURRENT_SOURCE_DIR}/print_logger.h
    )

add_library(${PROJ_NAME} SHARED ${SOURCE} ${HEADERS})

target_include_directories(${PROJ_NAME} PUBLIC ${RE_SOURCE_PATH})


# Link Libraries
target_link_libraries(${PROJ_NAME} PUBLIC zmq_protowriter)
target_link_libraries(${PROJ_NAME} PUBLIC re_common_proto_modelevent)
target_link_libraries(${PROJ_NAME} PUBLIC re_core)
target_link_libraries(${PROJ_NAME} PUBLIC re_core_singletons)

if(UNIX)
    target_link_libraries(${PROJ_NAME} PRIVATE pthread)
endif(UNIX)