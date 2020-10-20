add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/core")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/nodemanager")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/middleware")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/environmentmanager")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/network")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/experimentdefinition")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/graphmlparser")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/logging")

# Have to have this before workers append to sem_exports.
# MAKE SURE APPEND IS NOT ON THE FIRST INSTANCE OF EXPORT
# Order of export when using APPEND matters, watch out
export(TARGETS
        zmq_utils
        zmq_helper
        prototranslator
        zmq_protowriter
        re_common_proto_modelevent
        re_common_proto_control
        re_core
        re_loggers
        re_core_singletons
        zmq_ports
        NAMESPACE sem::
        FILE ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/sem_exports.cmake
        )
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/workers")
