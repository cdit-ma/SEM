set(PROJ_NAME re_environment_controller)
project(${PROJ_NAME} VERSION ${PROJECT_VERSION})
set(LIB_NAME ${PROJ_NAME}_lib)

find_package(Boost 1.30.0 COMPONENTS program_options REQUIRED)

# Library
add_library(${LIB_NAME} STATIC "")
target_compile_features(${LIB_NAME} PRIVATE cxx_std_11)
set_target_properties(${LIB_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_sources(${LIB_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/environmentcontroller.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/environmentcontroller.h)
target_link_libraries(${LIB_NAME} PUBLIC re_common_proto_environmentcontrol)
target_link_libraries(${LIB_NAME} PUBLIC zmq_protorequester)
target_link_libraries(${LIB_NAME} PUBLIC re_graphml_parser_lib)

# Executable
add_executable(${PROJ_NAME} "")
target_compile_features(${PROJ_NAME} PRIVATE cxx_std_11)
target_compile_definitions(${PROJ_NAME} PRIVATE BOOST_ALL_DYN_LINK)
set_target_properties(${PROJ_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_sources(${PROJ_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
target_include_directories(${PROJ_NAME} PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(${PROJ_NAME} PUBLIC ${LIB_NAME})
target_link_libraries(${PROJ_NAME} PRIVATE ${Boost_PROGRAM_OPTIONS_LIBRARY})



