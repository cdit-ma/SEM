set(PROJ_NAME logan_protohandler_systemevent)
project (${PROJ_NAME})

add_library(${PROJ_NAME} STATIC "")
target_compile_features(${PROJ_NAME} PRIVATE cxx_std_11)

target_sources(${PROJ_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/protohandler.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/protohandler.h
        )
target_link_libraries(${PROJ_NAME} PUBLIC zmq_protoreceiver)
target_link_libraries(${PROJ_NAME} PUBLIC re_common_proto_systemevent)
target_link_libraries(${PROJ_NAME} PUBLIC logan_server_lib)
