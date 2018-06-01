set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#Find rti library
find_package(RTI_DDS)

set(PROJ_NAME rti_dds_helper)
project (${PROJ_NAME})
set(RTI_DDS_HELPER_LIBRARIES ${PROJ_NAME})

if(NOT RTI_DDS_FOUND)
message(STATUS "Not Found RTI - Not building RTI Middleware library")
return()
endif()



set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/ddshelper.cpp
	)

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/ddshelper.h
    )

add_library(${PROJ_NAME} SHARED ${SOURCE} ${HEADERS})

# Need this to make sure headers are using the correct function calls.
target_compile_definitions(${PROJ_NAME} PRIVATE ${RTI_DDS_DEFINITIONS})


target_include_directories(${PROJ_NAME} PRIVATE ${RE_SRC_PATH})
target_include_directories(${PROJ_NAME} PRIVATE ${RTI_DDS_INCLUDE_DIRS})


target_link_libraries(${PROJ_NAME} ${RTI_DDS_LIBRARIES})


SET(RTI_HELPER_LIBRARIES ${RTI_DDS_LIBRARIES})

message("RTI: ${RTI_HELPER_LIBRARIES}")
