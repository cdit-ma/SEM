set(PROJ_NAME test_logan_aggregationserver)
project(${PROJ_NAME})

add_executable(${PROJ_NAME} "")
target_compile_features(${PROJ_NAME} PRIVATE cxx_std_17)

target_sources(${PROJ_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/databaseclienttest.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/aggregationservertest.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/experimenttrackertest.cpp
        )

# Include directories to be tested
# TODO: consider moving test folder structure to be more local to code under test;
# thereby making code less brittle should folder hierarchy restructuring
target_include_directories(${PROJ_NAME} PRIVATE ${LOGAN_SOURCE_PATH}/aggregationserver)
target_include_directories(${PROJ_NAME} PRIVATE ${GOOGLE_TEST_DIR}/googlemock/include)

target_link_libraries(${PROJ_NAME} PRIVATE logan_database_client_lib)
target_link_libraries(${PROJ_NAME} PRIVATE gtest)
target_link_libraries(${PROJ_NAME} PRIVATE gtest_main)
target_link_libraries(${PROJ_NAME} PRIVATE gmock)
