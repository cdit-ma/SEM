add_library(re_loggers SHARED)
target_compile_features(re_loggers PRIVATE cxx_std_11)
set_target_properties(re_loggers PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_sources(re_loggers
        PRIVATE
        # Headers
        ${CMAKE_CURRENT_SOURCE_DIR}/logan_logger.h
        ${CMAKE_CURRENT_SOURCE_DIR}/print_logger.h
        ${CMAKE_CURRENT_SOURCE_DIR}/experiment_logger.h
        PRIVATE
        # Sources
        ${CMAKE_CURRENT_SOURCE_DIR}/logan_logger.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/print_logger.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/experiment_logger.cpp
)

target_include_directories(re_loggers PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(re_loggers
        PUBLIC
        zmq_protowriter
        re_core
        re_core_singletons
        Boost::dynamic_linking
        Boost::thread
        )