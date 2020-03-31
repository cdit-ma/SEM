add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/tableoperations")
# Removing aggregation server tests until they are no longer dependant on a fixed IP test db magically existing
# TODO: Re-add these tests!
#add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/aggregationserver")
#add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/aggregationbroker")