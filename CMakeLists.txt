cmake_minimum_required (VERSION 2.6)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(PROJ_NAME test_runtime)
project (${PROJ_NAME})

set(CDITMA_RUNTIME_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/common")
include_directories(${PROJ_NAME} "${CDITMA_RUNTIME_ROOT}/includes")

set(SOURCE
	${CMAKE_CURRENT_SOURCE_DIR}/message.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/vectormessage.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/basemessage.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/interfaces.cpp
	)


add_subdirectory("${PROJECT_SOURCE_DIR}/proto/message")
add_subdirectory("${PROJECT_SOURCE_DIR}/proto/vectormessage")
#add_subdirectory("${PROJECT_SOURCE_DIR}/opensplice")
#add_subdirectory("${PROJECT_SOURCE_DIR}/qpid")
#add_subdirectory("${PROJECT_SOURCE_DIR}/rti")
add_subdirectory("${PROJECT_SOURCE_DIR}/zmq")

#add_subdirectory("${PROJECT_SOURCE_DIR}/common")


set(SOURCE_1
	${CMAKE_CURRENT_SOURCE_DIR}/deployment_1.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/senderimpl.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/receiverimpl.cpp
	${SOURCE}
	)

set(SOURCE_2
	${CMAKE_CURRENT_SOURCE_DIR}/deployment_2.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/receiverimpl.cpp
	${SOURCE}
	)

if(false)
set(SOURCE_3
	${CMAKE_CURRENT_SOURCE_DIR}/deployment_3.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/senderimpl.cpp
	${SOURCE}
	)

set(SOURCE_4
	${CMAKE_CURRENT_SOURCE_DIR}/deployment_4.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/receiverimpl.cpp
	${SOURCE}
	)

set(SOURCE_5
	${CMAKE_CURRENT_SOURCE_DIR}/deployment_5.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/senderimpl.cpp
	${SOURCE}
	)

set(SOURCE_6
	${CMAKE_CURRENT_SOURCE_DIR}/deployment_6.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/receiverimpl.cpp
	${SOURCE}
	)


set(SOURCE_7
	${CMAKE_CURRENT_SOURCE_DIR}/deployment_7.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/senderimpl.cpp
	${SOURCE}
	)

set(SOURCE_8
	${CMAKE_CURRENT_SOURCE_DIR}/deployment_8.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/receiverimpl.cpp
	${SOURCE}
	)

set(SOURCE_9
	${CMAKE_CURRENT_SOURCE_DIR}/deployment_9.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/senderimpl.cpp
	${SOURCE}
	)

set(SOURCE_10
	${CMAKE_CURRENT_SOURCE_DIR}/deployment_10.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/receiverimpl.cpp
	${SOURCE}
	)

endif(false)

add_library(deployment_1 MODULE ${SOURCE_1} ${HEADERS})
add_library(deployment_2 MODULE ${SOURCE_2} ${HEADERS})

if(false)
add_library(deployment_3 MODULE ${SOURCE_3} ${HEADERS})
add_library(deployment_4 MODULE ${SOURCE_4} ${HEADERS})

add_library(deployment_5 MODULE ${SOURCE_5} ${HEADERS})
add_library(deployment_6 MODULE ${SOURCE_6} ${HEADERS})

add_library(deployment_7 MODULE ${SOURCE_7} ${HEADERS})
add_library(deployment_8 MODULE ${SOURCE_8} ${HEADERS})

add_library(deployment_9 MODULE ${SOURCE_9} ${HEADERS})
add_library(deployment_10 MODULE ${SOURCE_10} ${HEADERS})
endif(false)

if (MSVC)
	#Visual studio needs to be told to build in Multithreaded Static mode
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
endif()

find_library(RE_PROTO_CONTROL_LIBRARIES re_proto_control "${CDITMA_RUNTIME_ROOT}/lib")


target_link_libraries(deployment_1 ${RE_PROTO_CONTROL_LIBRARIES})
target_link_libraries(deployment_1 proto_vectormessage_library)
target_link_libraries(deployment_1 proto_message_library)
target_link_libraries(deployment_1 zmq_library)


target_link_libraries(deployment_2 ${RE_PROTO_CONTROL_LIBRARIES})
#target_link_libraries(deployment_2 proto_vectormessage_library)
target_link_libraries(deployment_2 proto_message_library)
target_link_libraries(deployment_2 zmq_library)

if(false)
target_link_libraries(deployment_3 proto_vectormessage_library)
target_link_libraries(deployment_3 proto_message_library)
target_link_libraries(deployment_3 qpid_library)

target_link_libraries(deployment_4 proto_vectormessage_library)
target_link_libraries(deployment_4 proto_message_library)
target_link_libraries(deployment_4 qpid_library)


target_link_libraries(deployment_5 rti_library)
target_link_libraries(deployment_6 rti_library)

target_link_libraries(deployment_7 ospl_library)
target_link_libraries(deployment_8 ospl_library)

target_link_libraries(deployment_9 ospl_library)
target_link_libraries(deployment_10 ospl_library)

target_link_libraries(deployment_9 rti_library)
target_link_libraries(deployment_10 rti_library)

target_link_libraries(deployment_9 proto_message_library)
target_link_libraries(deployment_10 proto_message_library)

target_link_libraries(deployment_9 qpid_library)
target_link_libraries(deployment_10 qpid_library)

target_link_libraries(deployment_9 zmq_library)
target_link_libraries(deployment_10 zmq_library)

#target_link_libraries(${PROJ_NAME} ospl_library)
#target_link_libraries(${PROJ_NAME} qpid_library)
#target_link_libraries(${PROJ_NAME} rti_library)
endif(false)
