cmake_minimum_required (VERSION 2.6)

#Use C++11
set(CMAKE_CXX_STANDARD 11)
#Set our module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/")
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/")

set(PROJ_NAME test_runtime)

project (${PROJ_NAME})


set(SOURCE
	${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/message.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/basemessage.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/interfaces.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/senderimpl.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/recieverimpl.cpp
	)

add_subdirectory("${PROJECT_SOURCE_DIR}/test")
#add_subdirectory("${PROJECT_SOURCE_DIR}/zmq")


add_executable(${PROJ_NAME} ${SOURCE} ${HEADERS})

if (MSVC)
	#Visual studio needs to be told to build in Multithreaded Static mode
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
else()
	#Unix needs to include pthread
	target_link_libraries(${PROJ_NAME} pthread dl)
endif()
