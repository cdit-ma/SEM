cmake_minimum_required (VERSION 2.6)

#Use C++11
set(CMAKE_CXX_STANDARD 11)

set(PROJ_NAME tao_test)
set(SEND_NAME tao_client)
set(RECV_NAME tao_server)

project (${PROJ_NAME})

set(ACE_ROOT "/home/cdit-ma/ACE_wrappers/")
set(TAO_ROOT "${ACE_ROOT}/TAO/")

include_directories("${ACE_ROOT}")
include_directories("${TAO_ROOT}")

FIND_LIBRARY(TAO_LIBS NAMES TAO PATHS ${ACE_ROOT}/lib)
FIND_LIBRARY(ACE_LIBS NAMES ACE PATHS ${ACE_ROOT}/lib)
FIND_LIBRARY(TAO_PS_LIBS NAMES TAO_PortableServer PATHS ${ACE_ROOT}/lib)
FIND_LIBRARY(TAO_ATC_LIBS NAMES TAO_AnyTypeCode PATHS ${ACE_ROOT}/lib)
FIND_LIBRARY(TAO_IORTable_LIBS NAMES TAO_IORTable PATHS ${ACE_ROOT}/lib)



#Link our Logger against all the Cpps
set(SEND_SOURCE
	${CMAKE_CURRENT_SOURCE_DIR}/messageC.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/client.cpp
)

set(RECV_SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/server.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/messageC.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/messageS.cpp
)


add_executable(${SEND_NAME} ${SEND_SOURCE} ${HEADERS})
add_executable(${RECV_NAME} ${RECV_SOURCE} ${HEADERS})


target_include_directories(${SEND_NAME} PRIVATE ${RE_SRC_PATH})
target_include_directories(${RECV_NAME} PRIVATE ${RE_SRC_PATH})

target_link_libraries(${SEND_NAME} pthread dl)
target_link_libraries(${SEND_NAME} ${TAO_LIBS})
target_link_libraries(${SEND_NAME} ${ACE_LIBS})
target_link_libraries(${SEND_NAME} ${TAO_ATC_LIBS})
target_link_libraries(${SEND_NAME} ${TAO_PS_LIBS})


target_link_libraries(${RECV_NAME} pthread dl)
target_link_libraries(${RECV_NAME} ${TAO_LIBS})
target_link_libraries(${RECV_NAME} ${ACE_LIBS})
target_link_libraries(${RECV_NAME} ${TAO_ATC_LIBS})
target_link_libraries(${RECV_NAME} ${TAO_PS_LIBS})
target_link_libraries(${RECV_NAME} ${TAO_IORTable_LIBS})

target_link_libraries(${SEND_NAME} tao_helper)
target_link_libraries(${RECV_NAME} tao_helper)


