set(PROJ_NAME "re_loggers")
project(${PROJ_NAME})

add_library(${PROJ_NAME} SHARED)

target_sources(${PROJ_NAME} PRIVATE
    # Sources
    ${CMAKE_CURRENT_SOURCE_DIR}/logan_logger.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/print_logger.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/experiment_logger.cpp
    # Headers
    ${CMAKE_CURRENT_SOURCE_DIR}/logan_logger.h
    ${CMAKE_CURRENT_SOURCE_DIR}/print_logger.h
    ${CMAKE_CURRENT_SOURCE_DIR}/experiment_logger.h
)

# Link Libraries
target_link_libraries(${PROJ_NAME} PUBLIC zmq_protowriter)
target_link_libraries(${PROJ_NAME} PUBLIC re_core)
target_link_libraries(${PROJ_NAME} PUBLIC re_core_singletons)