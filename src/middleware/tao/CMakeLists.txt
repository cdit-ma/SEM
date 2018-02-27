

set(PROJ_NAME tao_helper)
project (${PROJ_NAME})

set(ACE_ROOT "/home/cdit-ma/ACE_wrappers/")
set(TAO_ROOT "${ACE_ROOT}/TAO/")

FIND_LIBRARY(TAO_LIBS NAMES TAO PATHS ${ACE_ROOT}/lib)
FIND_LIBRARY(ACE_LIBS NAMES ACE PATHS ${ACE_ROOT}/lib)
FIND_LIBRARY(TAO_PS_LIBS NAMES TAO_PortableServer PATHS ${ACE_ROOT}/lib)
FIND_LIBRARY(TAO_ATC_LIBS NAMES TAO_AnyTypeCode PATHS ${ACE_ROOT}/lib)
FIND_LIBRARY(TAO_IORTable_LIBS NAMES TAO_IORTable PATHS ${ACE_ROOT}/lib)

set(SOURCE
	${CMAKE_CURRENT_SOURCE_DIR}/helper.cpp
)

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/helper.h
)

add_library(${PROJ_NAME} SHARED ${SOURCE} ${HEADERS})

target_include_directories(${PROJ_NAME} PRIVATE ${ACE_ROOT})
target_include_directories(${PROJ_NAME} PRIVATE ${TAO_ROOT})

target_link_libraries(${PROJ_NAME} ${RTIDDS_LIBRARIES})
target_link_libraries(${PROJ_NAME} ${TAO_LIBS})
target_link_libraries(${PROJ_NAME} ${ACE_LIBS})
target_link_libraries(${PROJ_NAME} ${TAO_ATC_LIBS})
target_link_libraries(${PROJ_NAME} ${TAO_PS_LIBS})
target_link_libraries(${PROJ_NAME} ${TAO_IORTable_LIBS})

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/basic")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/test")
