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

    set(OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIR}/bin")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${OUTPUT_DIR}/lib")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${OUTPUT_DIR}/lib")

    if(MSVC)
        SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIR}/bin")
        SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIR}/lib")
        SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIR}/lib")
        #Protobuf says everything is rubbish on windows` https://github.com/google/protobuf/blob/master/cmake/README.md
        SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /wd4251 /wd4267")
    endif()
    
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/proto")
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/zmq")
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/comms")
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/util")
endif()
