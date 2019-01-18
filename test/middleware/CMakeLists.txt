add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/proto")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/zmq")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/ospl")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/rti")
#Both qpid/tao require either a name server/qpid broker
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/qpid")
#add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/tao")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/base/primitivetranslations")