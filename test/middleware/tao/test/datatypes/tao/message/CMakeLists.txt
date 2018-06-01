set(PROJ_NAME "tao_message")
project(${PROJ_NAME})


# Find package TAO
find_package(TAO)

if(NOT TAO_FOUND)
	message(STATUS "Cannot find TAO cannot build project ${PROJ_NAME}")
	return()
endif()

set(SHARED_LIBRARY_NAME "tao_message_lib")

# Copy the <idl> file into the binary directory so it can be used by the middleware compilers
#configure_file(message.idl ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)

# Run the tao Compiler over the <idl> files
TAO_GENERATE_CPP(TAO_SOURCES TAO_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/message.idl)

set(SOURCE
	${CMAKE_CURRENT_SOURCE_DIR}/translator.cpp
)

add_library(${SHARED_LIBRARY_NAME} SHARED ${SOURCE} ${HEADERS} ${TAO_SOURCES} ${TAO_HEADERS})
#add_library(${PROJ_NAME} MODULE ${CMAKE_CURRENT_SOURCE_DIR}/libportexport.cpp)
add_executable(${PROJ_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/libportexport.cpp)

target_include_directories(${SHARED_LIBRARY_NAME} PRIVATE ${RE_SRC_PATH})
target_include_directories(${SHARED_LIBRARY_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

target_include_directories(${SHARED_LIBRARY_NAME} PRIVATE ${RE_SRC_PATH})
target_include_directories(${SHARED_LIBRARY_NAME} PRIVATE ${TAO_INCLUDE_DIRS})
target_include_directories(${SHARED_LIBRARY_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(${SHARED_LIBRARY_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../..")

target_include_directories(${PROJ_NAME} PRIVATE ${RE_SRC_PATH})
target_include_directories(${PROJ_NAME} PRIVATE ${TAO_INCLUDE_DIRS})
target_include_directories(${PROJ_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../..")

if(UNIX)
    target_link_libraries(${PROJ_NAME} pthread)
    target_link_libraries(${PROJ_NAME} dl)
endif()


target_link_libraries(${SHARED_LIBRARY_NAME} re_core)
target_link_libraries(${SHARED_LIBRARY_NAME} ${TAO_LIBRARIES})
target_link_libraries(${SHARED_LIBRARY_NAME} tao_helper)
target_link_libraries(${SHARED_LIBRARY_NAME} "tao_base_message_lib")

target_link_libraries(${PROJ_NAME} ${TAO_LIBRARIES})

target_link_libraries(${PROJ_NAME} "tao_message_lib")

