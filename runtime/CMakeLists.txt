cmake_minimum_required (VERSION 2.6)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(PROJ_NAME test_runtime)
project (${PROJ_NAME})

set(CDITMA_RUNTIME_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/common")
include_directories(${PROJ_NAME} "${CDITMA_RUNTIME_ROOT}/includes")

set(SOURCE
	${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/message.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/basemessage.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/interfaces.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/senderimpl.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/receiverimpl.cpp
	)


add_subdirectory("${PROJECT_SOURCE_DIR}/proto")
add_subdirectory("${PROJECT_SOURCE_DIR}/opensplice")
add_subdirectory("${PROJECT_SOURCE_DIR}/qpid")
add_subdirectory("${PROJECT_SOURCE_DIR}/rti")
add_subdirectory("${PROJECT_SOURCE_DIR}/zmq")

add_executable(${PROJ_NAME} ${SOURCE} ${HEADERS})

if (MSVC)
	#Visual studio needs to be told to build in Multithreaded Static mode
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
endif()

target_link_libraries(${PROJ_NAME} ospl_library)
target_link_libraries(${PROJ_NAME} proto_library)
target_link_libraries(${PROJ_NAME} qpid_library)
target_link_libraries(${PROJ_NAME} rti_library)
target_link_libraries(${PROJ_NAME} zmq_library)
