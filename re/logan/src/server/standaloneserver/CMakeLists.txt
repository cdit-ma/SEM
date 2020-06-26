set(PROJ_NAME logan_server)
project (${PROJ_NAME})

find_package(Boost 1.30.0 COMPONENTS program_options REQUIRED)

add_executable(${PROJ_NAME} "")
target_compile_features(${PROJ_NAME} PRIVATE cxx_std_11)

target_sources(${PROJ_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
        )
target_include_directories(${PROJ_NAME} PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(${PROJ_NAME}
        PRIVATE
        sem_version
        logan_server_lib
        ${Boost_PROGRAM_OPTIONS_LIBRARY}
        )
target_link_libraries(${PROJ_NAME} PRIVATE )
