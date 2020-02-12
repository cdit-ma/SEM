add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/tableoperations")
# Removing aggregation server tests until they are no longer dependant on a fixed IP test db magically existing
#add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/aggregationserver")