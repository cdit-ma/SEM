cmake_minimum_required (VERSION 2.6)

set(CMAKE_CXX_STANDARD 11) # C++11...
set(CMAKE_CXX_STANDARD_REQUIRED ON) #...is required...
set(CMAKE_CXX_EXTENSIONS OFF) #...without compiler extensions like gnu++11

#Set our module path
#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/../client/cmake/modules/")

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


#add_subdirectory("${PROJECT_SOURCE_DIR}/test")
#add_subdirectory("${PROJECT_SOURCE_DIR}/zmq")
add_subdirectory("${PROJECT_SOURCE_DIR}/rti")
add_subdirectory("${PROJECT_SOURCE_DIR}/opensplice")
add_subdirectory("${PROJECT_SOURCE_DIR}/qpid")
add_subdirectory("${PROJECT_SOURCE_DIR}/zmq")



#find OSPL
#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/opensplice/cmake")
#find_package(OpenSplice REQUIRED)
#include_directories(${PROJ_NAME} ${OpenSplice_INCLUDE_DIRS})

#find ZMQ
#find_package(ZeroMQ REQUIRED)
#include_directories(${PROJ_NAME} ${ZeroMQ_INCLUDE_DIRS})

#find RTIDDS
#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/dds/cmake/modules")
#find_package(DDS REQUIRED)
#include_directories(${PROJ_NAME} ${DDS_INCLUDE_DIRS})

#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/rti/cmake/modules")

#find_package(DDS REQUIRED)
#include_directories(${PROJ_NAME} ${DDS_INCLUDE_DIRS})


#set(CMAKE_CXX_FLAGS "-Wl,--no-as-needed")

add_executable(${PROJ_NAME} ${SOURCE} ${HEADERS})

if (MSVC)
	#Visual studio needs to be told to build in Multithreaded Static mode
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
else()
	#Unix needs to include pthread
	target_link_libraries(${PROJ_NAME} pthread)
	#target_link_libraries(${PROJ_NAME} rt)

endif()



#target_compile_definitions(${PROJ_NAME} PRIVATE -DRTI_UNIX -DRTI_64BIT)
#target_compile_definitions(${PROJ_NAME} PRIVATE -D_GLIBCXX_USE_CXX11_ABI=0)
target_compile_features(${PROJ_NAME} PRIVATE cxx_range_for)


#target_link_libraries(${PROJ_NAME} ${ZeroMQ_LIBRARIES})
#target_link_libraries(${PROJ_NAME} ${DDS_LIBRARIES})
#target_link_libraries(${PROJ_NAME} ${OpenSplice_LIBRARIES})

target_link_libraries(${PROJ_NAME} rti_library)
target_link_libraries(${PROJ_NAME} ospl_library)
target_link_libraries(${PROJ_NAME} qpid_library)
target_link_libraries(${PROJ_NAME} zmq_library)


