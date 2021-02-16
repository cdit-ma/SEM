project("std_out_listener"
        VERSION ${PROJECT_VERSION})

find_package(Boost 1.30.0 COMPONENTS program_options REQUIRED)

add_executable(${PROJECT_NAME} "")
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
target_compile_definitions(${PROJECT_NAME} PUBLIC BOOST_ALL_DYN_LINK)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/..)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

target_link_libraries(${PROJECT_NAME} PUBLIC sem_types)
target_link_libraries(${PROJECT_NAME} PUBLIC re_network)
target_link_libraries(${PROJECT_NAME} PUBLIC epm_stdout_protocol)
target_link_libraries(${PROJECT_NAME} PUBLIC ${Boost_PROGRAM_OPTIONS_LIBRARY})

target_sources(${PROJECT_NAME} PUBLIC stdoutlistener.cpp)
