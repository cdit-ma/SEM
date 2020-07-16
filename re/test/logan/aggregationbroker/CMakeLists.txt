project(test_logan_aggregationbroker)

set(AGG_SOURCE_DIR ${LOGAN_SOURCE_PATH}/aggregationserver)

add_executable(${PROJECT_NAME} "")
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

target_sources(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/aggregationrepliertest.cpp

        ${AGG_SOURCE_DIR}/aggregationbroker/aggregationreplier.cpp
        ${AGG_SOURCE_DIR}/aggregationbroker/aggregationreplier.h

        ${CMAKE_BINARY_DIR}/logan/src/aggregationserver/sqlstrings.h

        ${AGG_SOURCE_DIR}/databaseclient.h

        ${DOCKER_CLIENT_INCLUDE_DIRS}/docker.h
        )

# Include directories to be tested
# TODO: consider moving test folder structure to be more local to code under test;
#  thereby making code less brittle should folder hierarchy restructuring occur
target_include_directories(${PROJECT_NAME} PRIVATE ${AGG_SOURCE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${AGG_SOURCE_DIR}/aggregationbroker)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR}/logan/src/aggregationserver/)
target_include_directories(${PROJECT_NAME} PRIVATE ${RE_COMMON_SOURCE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${RE_COMMON_BINARY_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${GOOGLE_MOCK_DIR}/include)

target_link_libraries(${PROJECT_NAME} PRIVATE logan_database_client_lib)

target_link_libraries(${PROJECT_NAME} PRIVATE zmq_protoreceiver)
target_link_libraries(${PROJECT_NAME} PRIVATE zmq_protoreplier)
target_link_libraries(${PROJECT_NAME} PRIVATE zmq_protowriter)
target_link_libraries(${PROJECT_NAME} PRIVATE re_common_proto_aggregation_message)

#target_link_libraries(${PROJECT_NAME} PRIVATE docker-cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE gtest)
target_link_libraries(${PROJECT_NAME} PRIVATE gtest_main)
target_link_libraries(${PROJECT_NAME} PRIVATE gmock)
