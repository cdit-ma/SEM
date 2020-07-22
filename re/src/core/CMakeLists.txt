# re_core_singletons
add_library(re_core_singletons SHARED)
target_compile_features(re_core_singletons PRIVATE cxx_std_11)
set_target_properties(re_core_singletons PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_sources(re_core_singletons PRIVATE
    # Sources
    ${CMAKE_CURRENT_SOURCE_DIR}/id_generator.cpp
    # Headers
    ${CMAKE_CURRENT_SOURCE_DIR}/id_generator.h
)

# re_core
find_package(Boost 1.67.0 COMPONENTS thread REQUIRED)
add_library(re_core STATIC)
target_compile_features(re_core PRIVATE cxx_std_11)
set_target_properties(re_core PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_sources(re_core PRIVATE
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
target_include_directories(re_core
        PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}
        )


set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
# Link Libraries
target_link_libraries(re_core 
        PUBLIC
        zmq_protowriter
        re_common_proto_modelevent
        re_core_singletons
        Boost::boost
        PRIVATE
        re_common_proto_control
        Boost::thread
        )
# Have to use old CMake boost syntax TODO: Build all of Boost
# Boost::thread links against boost::atomic boost::chrono and boost::data_time which aren't built

if(UNIX)
    target_link_libraries(re_core PRIVATE pthread)
endif(UNIX)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/loggers")
