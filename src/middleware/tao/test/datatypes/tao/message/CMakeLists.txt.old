# Get the RE_PATH
set(RE_PATH "$ENV{RE_PATH}")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${RE_PATH}/cmake/modules")

set(SHARED_LIB_NAME "tao_aggregate_lib")
set(PROJ_NAME "tao_aggregate")
project(${PROJ_NAME})

set(ACE_ROOT "/home/cdit-ma/ACE_wrappers/")
set(TAO_ROOT "${ACE_ROOT}/TAO/")

include_directories("${ACE_ROOT}")
include_directories("${TAO_ROOT}")

FIND_LIBRARY(TAO_LIBS NAMES TAO PATHS ${ACE_ROOT}/lib)
FIND_LIBRARY(ACE_LIBS NAMES ACE PATHS ${ACE_ROOT}/lib)
FIND_LIBRARY(TAO_PS_LIBS NAMES TAO_PortableServer PATHS ${ACE_ROOT}/lib)
FIND_LIBRARY(TAO_ATC_LIBS NAMES TAO_AnyTypeCode PATHS ${ACE_ROOT}/lib)
FIND_LIBRARY(TAO_IORTable_LIBS NAMES TAO_IORTable PATHS ${ACE_ROOT}/lib)

# Find library re_core
find_library(RE_CORE_LIBRARIES re_core "${RE_PATH}/lib")

set(SOURCE
	${CMAKE_CURRENT_SOURCE_DIR}/messageC.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/messageS.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/convert.cpp
	)

set(HEADERS
	${CMAKE_CURRENT_SOURCE_DIR}/convert.h
	)

# Include the RE_PATH directory
include_directories("${RE_PATH}/src")

# Build the shared library that will be loaded at compile time.
add_library(${SHARED_LIB_NAME} SHARED ${SOURCE} ${HEADERS} ${RTI_SRCS} ${RTI_HDRS})

# Link the shared library.
target_link_libraries(${SHARED_LIB_NAME} ${RE_CORE_LIBRARIES})

# Link the shared library against the base type library
target_link_libraries(${SHARED_LIB_NAME} base_aggregate)

# Build the actual Module library that will be dynamically loaded.
#@add_library(${PROJ_NAME} MODULE ${CMAKE_CURRENT_SOURCE_DIR}/libportexports.cpp)
add_executable(${PROJ_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/libportexports.cpp)

target_link_libraries(${PROJ_NAME} pthread)

# Link the shared library.
target_link_libraries(${PROJ_NAME} ${RE_CORE_LIBRARIES})
target_link_libraries(${PROJ_NAME} ${TAO_LIBS})
target_link_libraries(${PROJ_NAME} ${ACE_LIBS})
target_link_libraries(${PROJ_NAME} ${TAO_ATC_LIBS})
target_link_libraries(${PROJ_NAME} ${TAO_PS_LIBS})
target_link_libraries(${PROJ_NAME} ${TAO_IORTable_LIBS})

# Link the shared library against the base type library
target_link_libraries(${PROJ_NAME} base_aggregate)

# Link the Module against the Shared library
target_link_libraries(${PROJ_NAME} ${SHARED_LIB_NAME})

