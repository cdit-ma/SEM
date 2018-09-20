
if(NOT RE_COMMON_FOUND)
    if(MSVC)
        #Protobuf says everything is rubbish on windows https://github.com/google/protobuf/blob/master/cmake/README.md
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /wd4251 /wd4267")
    endif()
    message(STATUS "Building re_common")
    
    set(RE_COMMON_FOUND true PARENT_SCOPE)
    set(RE_COMMON_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
    set(RE_COMMON_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}")

    set(RE_COMMON_SOURCE_DIR "${RE_COMMON_SOURCE_DIR}" PARENT_SCOPE)
    set(RE_COMMON_BINARY_DIR "${RE_COMMON_BINARY_DIR}" PARENT_SCOPE)
    
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/proto")
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/zmq")
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/comms")
endif()
