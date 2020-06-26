set(PROJ_NAME logan_managedserver)
project (${PROJ_NAME})

find_package(Boost 1.30.0 COMPONENTS program_options REQUIRED)

add_executable(${PROJ_NAME} "")
target_compile_features(${PROJ_NAME} PRIVATE cxx_std_11)
target_compile_definitions(${PROJ_NAME} PUBLIC BOOST_ALL_DYN_LINK)

target_sources(${PROJ_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/managedserver.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/managedserver.h
        )
target_link_libraries(${PROJ_NAME}
        PRIVATE
        sem_version
        logan_server_lib
        zmq_environmentrequester
        ${Boost_PROGRAM_OPTIONS_LIBRARY}
        )
