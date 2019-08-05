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
target_include_directories(${PROJ_NAME} PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(${PROJ_NAME} PRIVATE ${LOGAN_SOURCE_DIR})
target_include_directories(${PROJ_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
# Include parent binary directory to get cmakevars.h for version number
target_include_directories(${PROJ_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/../..)
target_link_libraries(${PROJ_NAME} PUBLIC logan_server_lib)
target_link_libraries(${PROJ_NAME} PUBLIC zmq_environmentrequester)
target_link_libraries(${PROJ_NAME} PUBLIC ${Boost_PROGRAM_OPTIONS_LIBRARY})
