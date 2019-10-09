set(PROJ_NAME test_logan_aggregationserver)
project(${PROJ_NAME})

set(AGG_SOURCE_DIR ${LOGAN_SOURCE_PATH}/aggregationserver)

add_executable(${PROJ_NAME} "")
target_compile_features(${PROJ_NAME} PRIVATE cxx_std_17)

target_sources(${PROJ_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/databaseclienttest.cpp
        #        ${CMAKE_CURRENT_SOURCE_DIR}/aggregationservertest.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/experimenttrackertest.cpp

        ${AGG_SOURCE_DIR}/experimenttracker.cpp
        #       ${AGG_SOURCE_DIR}/aggregationserver.cpp
        ${AGG_SOURCE_DIR}/aggregationprotohandler.cpp
        ${AGG_SOURCE_DIR}/modeleventprotohandler.cpp
        ${AGG_SOURCE_DIR}/systemeventprotohandler.cpp
        ${AGG_SOURCE_DIR}/nodemanagerprotohandler.cpp

        ${CMAKE_BINARY_DIR}/logan/src/aggregationserver/sqlstrings.h

        ${AGG_SOURCE_DIR}/databaseclient.h
        )

# Include directories to be tested
# TODO: consider moving test folder structure to be more local to code under test;
# thereby making code less brittle should folder hierarchy restructuring
target_include_directories(${PROJ_NAME} PRIVATE ${AGG_SOURCE_DIR})
target_include_directories(${PROJ_NAME} PRIVATE ${CMAKE_BINARY_DIR}/logan/src/aggregationserver/)
target_include_directories(${PROJ_NAME} PRIVATE ${RE_COMMON_SOURCE_DIR})
target_include_directories(${PROJ_NAME} PRIVATE ${RE_COMMON_BINARY_DIR})
target_include_directories(${PROJ_NAME} PRIVATE ${GOOGLE_TEST_DIR}/googlemock/include)

target_link_libraries(${PROJ_NAME} PRIVATE logan_database_client_lib)

target_link_libraries(${PROJ_NAME} PRIVATE zmq_protoreceiver)
target_link_libraries(${PROJ_NAME} PRIVATE zmq_protorequester)
target_link_libraries(${PROJ_NAME} PRIVATE zmq_protowriter)
target_link_libraries(${PROJ_NAME} PRIVATE re_common_proto_modelevent)
target_link_libraries(${PROJ_NAME} PRIVATE re_common_proto_control)
target_link_libraries(${PROJ_NAME} PRIVATE re_common_proto_systemevent)

target_link_libraries(${PROJ_NAME} PRIVATE gtest)
target_link_libraries(${PROJ_NAME} PRIVATE gtest_main)
target_link_libraries(${PROJ_NAME} PRIVATE gmock)
