set(PROJ_NAME tao_helper)
project (${PROJ_NAME})

find_package(TAO)

if(NOT TAO_FOUND)
	message(STATUS "Cannot find TAO cannot build project ${PROJ_NAME}")
	return()
endif()

add_library(${PROJ_NAME} SHARED "")
target_compile_features(${PROJ_NAME} PRIVATE cxx_std_11)

target_sources(${PROJ_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/taohelper.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/taohelper.h
)
target_include_directories(${PROJ_NAME} PUBLIC ${TAO_INCLUDE_DIRS})
target_link_libraries(${PROJ_NAME} PUBLIC ${TAO_LIBRARIES})
target_link_libraries(${PROJ_NAME} PUBLIC re_core)
