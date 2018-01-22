if (MSVC)
    #Protobuf says everything is rubbish on windows https://github.com/google/protobuf/blob/master/cmake/README.md
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /wd4251 /wd4267")
endif()

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/proto")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/zmq")