set(PROJ_NAME "re_core")
set(LIB_NAME "re_core_singletons")
project(${PROJ_NAME})

find_package(Boost 1.30.0 COMPONENTS thread REQUIRED)

# re_core_singletons
add_library(${LIB_NAME} SHARED "")
target_compile_features(${LIB_NAME} PRIVATE cxx_std_11)
set_target_properties(${LIB_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_sources(${LIB_NAME} PRIVATE
    # Sources
    ${CMAKE_CURRENT_SOURCE_DIR}/id_generator.cpp
    # Headers
    ${CMAKE_CURRENT_SOURCE_DIR}/id_generator.h
)

# re_core
add_library(${PROJ_NAME} STATIC "")
target_compile_features(${PROJ_NAME} PRIVATE cxx_std_11)
target_compile_definitions(${PROJ_NAME} PUBLIC BOOST_ALL_DYN_LINK)
set_target_properties(${PROJ_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_sources(${PROJ_NAME} PRIVATE
    # Sources
    ${CMAKE_CURRENT_SOURCE_DIR}/activatable.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/behaviourcontainer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/component.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/basemessage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ports/port.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ports/periodicport.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/loggerproxy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/attribute.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/worker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/translate.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/threadmanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/logger.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/statemachine.cpp

    # Headers
    ${CMAKE_CURRENT_SOURCE_DIR}/activatable.h
    ${CMAKE_CURRENT_SOURCE_DIR}/behaviourcontainer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/component.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ports/port.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ports/periodicport.h
    ${CMAKE_CURRENT_SOURCE_DIR}/basemessage.h
    ${CMAKE_CURRENT_SOURCE_DIR}/loggerproxy.h
    ${CMAKE_CURRENT_SOURCE_DIR}/logger.h
    ${CMAKE_CURRENT_SOURCE_DIR}/attribute.h
    ${CMAKE_CURRENT_SOURCE_DIR}/worker.h
    ${CMAKE_CURRENT_SOURCE_DIR}/translate.h
    ${CMAKE_CURRENT_SOURCE_DIR}/threadmanager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/statemachine.h
)

# Include Directories
target_include_directories(${PROJ_NAME} PUBLIC ${RE_SOURCE_PATH})

# Link Libraries
target_link_libraries(${PROJ_NAME} PUBLIC zmq_protowriter)
target_link_libraries(${PROJ_NAME} PUBLIC re_common_proto_modelevent)
target_link_libraries(${PROJ_NAME} PUBLIC re_common_proto_control)
# Have to use old CMake boost syntax TODO: Build all of Boost
# Boost::thread links against boost::atomic boost::chrono and boost::data_time which aren't built
target_link_libraries(${PROJ_NAME} PRIVATE ${Boost_THREAD_LIBRARY})
target_link_libraries(${PROJ_NAME} PUBLIC re_core_singletons)

if(UNIX)
    target_link_libraries(${PROJ_NAME} PRIVATE pthread)
endif(UNIX)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/loggers")
