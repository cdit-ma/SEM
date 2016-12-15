cmake_minimum_required (VERSION 2.6)

set(CMAKE_CXX_STANDARD 11) # C++11...
set(CMAKE_CXX_STANDARD_REQUIRED ON) #...is required...
set(CMAKE_CXX_EXTENSIONS OFF) #...without compiler extensions like gnu++11

set(PROJ_NAME test_runtime)
set(RUNTIME_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/includes/)
include_directories(${PROJ_NAME} ${RUNTIME_INCLUDE}/)

project (${PROJ_NAME})

set(SOURCE
	${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/message.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/basemessage.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/interfaces.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/senderimpl.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/receiverimpl.cpp
	)


add_subdirectory("${PROJECT_SOURCE_DIR}/rti")
add_subdirectory("${PROJECT_SOURCE_DIR}/opensplice")
add_subdirectory("${PROJECT_SOURCE_DIR}/qpid")
add_subdirectory("${PROJECT_SOURCE_DIR}/zmq")

add_executable(${PROJ_NAME} ${SOURCE} ${HEADERS})

if (MSVC)
	#Visual studio needs to be told to build in Multithreaded Static mode
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
else()
	#Unix needs to include pthread
	target_link_libraries(${PROJ_NAME} pthread)
endif()



target_compile_features(${PROJ_NAME} PRIVATE cxx_range_for)

target_link_libraries(${PROJ_NAME} rti_library)
target_link_libraries(${PROJ_NAME} ospl_library)
target_link_libraries(${PROJ_NAME} qpid_library)
target_link_libraries(${PROJ_NAME} zmq_library)

