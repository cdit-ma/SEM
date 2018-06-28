find_package(REGEN)

if(NOT REGEN_FOUND)
    message(STATUS "Not Found REGEN - Not building REGEN Tests")
    return()
endif()

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/re_gen)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/re_gen)

set(RE_CORE_LIBRARIES re_core)

#Define Helper Libraries
set(OSPL_DDS_HELPER_LIBRARIES ospl_dds_helper)
set(RTI_DDS_HELPER_LIBRARIES rti_dds_helper)
set(TAO_HELPER_LIBRARIES tao_helper)
set(QPID_HELPER_LIBRARIES qpid_helper)
set(ZMQ_HELPER_LIBRARIES zmq_helper)

#Define Workers
set(CPU_WORKER_LIBRARIES cpu_worker)
set(OPENCL_WORKER_LIBRARIES opencl_worker)
set(UTILITY_WORKER_LIBRARIES utility_worker)
set(MEMORY_WORKER_LIBRARIES memory_worker)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/helloworld")