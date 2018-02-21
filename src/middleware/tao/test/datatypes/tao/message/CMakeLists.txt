set(PROJ_NAME "tao_message")
project(${PROJ_NAME})

set(ACE_ROOT "$ENV{ACE_ROOT}")
set(TAO_ROOT "$ENV{TAO_ROOT}")

# Find TAO
FIND_LIBRARY(TAO_LIBS NAMES TAO PATHS ${ACE_ROOT}/lib)
FIND_LIBRARY(ACE_LIBS NAMES ACE PATHS ${ACE_ROOT}/lib)
FIND_LIBRARY(TAO_PS_LIBS NAMES TAO_PortableServer PATHS ${ACE_ROOT}/lib)
FIND_LIBRARY(TAO_ATC_LIBS NAMES TAO_AnyTypeCode PATHS ${ACE_ROOT}/lib)
FIND_LIBRARY(TAO_IORTable_LIBS NAMES TAO_IORTable PATHS ${ACE_ROOT}/lib)


set(SHARED_LIBRARY_NAME "tao_message_lib")
# Copy the <idl> file into the binary directory so it can be used by the middleware compilers
configure_file(message.idl ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)

# Run the rti Compiler over the <idl> files
#RTI_GENERATE_CPP(RTI_SOURCES RTI_HEADER ${CMAKE_CURRENT_BINARY_DIR}/message.idl)

set(TAO_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/messageC.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/messageS.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/messageS.h
	${CMAKE_CURRENT_SOURCE_DIR}/messageC.h
)

set(SOURCE
	${CMAKE_CURRENT_SOURCE_DIR}/convert.cpp
)

set(HEADERS
	${CMAKE_CURRENT_SOURCE_DIR}/convert.h
)

add_library(${SHARED_LIBRARY_NAME} SHARED ${SOURCE} ${HEADERS} ${TAO_SOURCES} ${TAO_HEADERS})
#add_library(${PROJ_NAME} MODULE ${CMAKE_CURRENT_SOURCE_DIR}/libportexport.cpp)
add_executable(${PROJ_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/libportexport.cpp)

# Include the runtime environment directory
target_include_directories(${SHARED_LIBRARY_NAME} PRIVATE "${RE_PATH}/src")
# Include the middleware include directory
target_include_directories(${SHARED_LIBRARY_NAME} PRIVATE "${ACE_ROOT}")
target_include_directories(${SHARED_LIBRARY_NAME} PRIVATE "${TAO_ROOT}")

# Include the current binary directory to allow inclusion of generated files
target_include_directories(${SHARED_LIBRARY_NAME} PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")

# Include required aggregates source dirs
target_include_directories(${SHARED_LIBRARY_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../..")

# Link against runtime environment
target_link_libraries(${SHARED_LIBRARY_NAME} re_core)
# Link against the middleware libraries
target_link_libraries(${SHARED_LIBRARY_NAME} "${RTIDDS_LIBRARIES}")
# Link against the base aggregate
target_link_libraries(${SHARED_LIBRARY_NAME} "base_message_lib")

target_include_directories(${PROJ_NAME} PRIVATE "${RE_PATH}/src")
target_include_directories(${PROJ_NAME} PRIVATE "${ACE_ROOT}")
target_include_directories(${PROJ_NAME} PRIVATE "${TAO_ROOT}")

# Include the current binary directory to allow inclusion of generated files
target_include_directories(${PROJ_NAME} PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
target_include_directories(${PROJ_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../..")

target_link_libraries(${PROJ_NAME} "${TAO_LIBS}")
target_link_libraries(${PROJ_NAME} "${ACE_LIBS}")
target_link_libraries(${PROJ_NAME} "${TAO_ATC_LIBS}")
target_link_libraries(${PROJ_NAME} "${TAO_PS_LIBS}")
target_link_libraries(${PROJ_NAME} "${TAO_IORTable_LIBS}")

target_link_libraries(${PROJ_NAME} re_core)

target_link_libraries(${PROJ_NAME} ${SHARED_LIB_NAME})
target_link_libraries(${PROJ_NAME} "tao_message_lib")
target_link_libraries(${PROJ_NAME} "base_message_lib")
target_link_libraries(${PROJ_NAME} pthread)

