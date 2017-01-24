cmake_minimum_required (VERSION 2.6)

set(CMAKE_MACOSX_RPATH 1)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)

SET(UBUNTU_TEST "/home/dig/re/common/lib")
SET(MAC_TEST "/Users/dan/Desktop/re/common/lib")

set(CMAKE_INSTALL_RPATH "@executable_path;@loader_path;${MAC_TEST};$ORIGIN;")


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CDITMA_RUNTIME_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/common")
include_directories(${PROJ_NAME} "${CDITMA_RUNTIME_ROOT}/includes")

#add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/common")

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/ports/message")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/ports/message/zmq")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/ports/message/proto")


add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/components/hellosender")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/components/helloreceiver")




set(SOURCE_2
	${CMAKE_CURRENT_SOURCE_DIR}/deployment_2.cpp
	)

add_library(deployment_2 MODULE ${SOURCE_2} ${HEADERS})

if (MSVC)
	#Visual studio needs to be told to build in Multithreaded Static mode
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
endif()


find_library(RE_CORE_LIBRARIES re_core PATHS "${CDITMA_RUNTIME_ROOT}/lib")
find_library(RE_PROTO_CONTROL_LIBRARIES re_proto_control PATHS "${CDITMA_RUNTIME_ROOT}/lib")

target_link_libraries(deployment_2 ${RE_CORE_LIBRARIES})
target_link_libraries(deployment_2 ${RE_PROTO_CONTROL_LIBRARIES})

target_link_libraries(deployment_2 components_hellosender)
target_link_libraries(deployment_2 components_helloreceiver)
target_link_libraries(deployment_2 ports_message_zmq)