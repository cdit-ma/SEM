include(FetchContent)

# Pugixml
set(BUILD_TESTS OFF)
FetchContent_Declare(
    pugixml
    GIT_REPOSITORY https://github.com/zeux/pugixml.git
    GIT_TAG v1.10
)
if(NOT pugixml_POPULATE)
    FetchContent_Populate(pugixml)
    add_subdirectory(${pugixml_SOURCE_DIR} ${pugixml_BINARY_DIR})
endif()

# Protobuf
set(protobuf_BUILD_TESTS OFF)
FetchContent_Declare(
    protobuf
    GIT_REPOSITORY https://github.com/protocolbuffers/protobuf.git
    GIT_TAG v3.11.2
    )
if(NOT protobuf_POPULATE)
    FetchContent_Populate(protobuf)
    add_subdirectory(${protobuf_SOURCE_DIR}/cmake ${protobuf_BINARY_DIR})
    set(Protobuf_IMPORT_DIRS ${protobuf_SOURCE_DIR}/src PARENT_SCOPE)
endif()
find_package(protobuf)

# ZeroMQ
FetchContent_Declare(
    zeromq
    GIT_REPOSITORY https://github.com/zeromq/libzmq.git
    GIT_TAG v4.3.0
)
if(NOT zeromq_POPULATE)
    FetchContent_Populate(zeromq)
    add_subdirectory(${zeromq_SOURCE_DIR} ${zeromq_BINARY_DIR})
endif()

# cppzmq
set(CPPZMQ_BUILD_TESTS OFF)
FetchContent_Declare(
    cppzmq
    GIT_REPOSITORY https://github.com/zeromq/cppzmq.git
    GIT_TAG master
)
if(NOT cppzmq_POPULATE)
    FetchContent_Populate(cppzmq)
    add_subdirectory(${cppzmq_SOURCE_DIR} ${cppzmq_BINARY_DIR})
endif()
