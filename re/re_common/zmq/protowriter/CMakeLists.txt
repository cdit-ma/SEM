set(PROJ_NAME zmq_protowriter)
project (${PROJ_NAME})

find_package(Boost 1.30.0 COMPONENTS filesystem system REQUIRED)
find_package(Protobuf REQUIRED)
find_package(cppzmq REQUIRED)

add_library(${PROJ_NAME} STATIC "")
target_compile_features(${PROJ_NAME} PRIVATE cxx_std_11)
target_compile_definitions(${PROJ_NAME} PUBLIC BOOST_ALL_DYN_LINK)
target_compile_definitions(${PROJ_NAME} PUBLIC PROTOBUF_USE_DLLS)
set_target_properties(${PROJ_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_sources(${PROJ_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/protowriter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/cachedprotowriter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/monitor.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/protowriter.h
        ${CMAKE_CURRENT_SOURCE_DIR}/cachedprotowriter.h
        ${CMAKE_CURRENT_SOURCE_DIR}/monitor.h
        )
target_include_directories(${PROJ_NAME} PUBLIC ${Boost_INCLUDE_DIRS})
target_include_directories(${PROJ_NAME} PUBLIC ${PROTOBUF_INCLUDE_DIRS})

target_link_libraries(${PROJ_NAME} PUBLIC ${PROTOBUF_LIBRARIES})
target_link_libraries(${PROJ_NAME} PUBLIC cppzmq)
target_link_libraries(${PROJ_NAME} PUBLIC ${Boost_SYSTEM_LIBRARY})
target_link_libraries(${PROJ_NAME} PUBLIC ${Boost_FILESYSTEM_LIBRARY})
