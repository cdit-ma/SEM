set(AGGREGATION_SERVER "aggregation_server")
set(PROJ_NAME ${AGGREGATION_SERVER})

project(${PROJ_NAME} VERSION ${PROJECT_VERSION})

#Find packages
find_package(Protobuf REQUIRED)
find_package(ZMQ REQUIRED)
find_package(Boost 1.30.0 COMPONENTS program_options REQUIRED)
find_package(libpqxx)

if(NOT PQXX_FOUND)
	message(WARNING "Could not find pqxx, not building aggregation broker and server")
	return()
endif()

# Build the DatabaseClient library
set(LOGAN_DB_CLIENT_LIB "logan_database_client_lib")
add_library(${LOGAN_DB_CLIENT_LIB} STATIC "")
target_compile_features(${LOGAN_DB_CLIENT_LIB} PRIVATE cxx_std_17)
target_sources(${LOGAN_DB_CLIENT_LIB} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/databaseclient.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/utils.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/databaseclient.h
        ${CMAKE_CURRENT_SOURCE_DIR}/utils.h)

target_include_directories(${LOGAN_DB_CLIENT_LIB} PRIVATE ${PROTOBUF_INCLUDE_DIRS})
target_include_directories(${LOGAN_DB_CLIENT_LIB} PRIVATE ${LOGAN_SRC_PATH})
target_include_directories(${LOGAN_DB_CLIENT_LIB} PRIVATE ${PQXX_INCLUDE_DIRECTORIES})
target_include_directories(${LOGAN_DB_CLIENT_LIB} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(${LOGAN_DB_CLIENT_LIB} PRIVATE ${RE_COMMON_BINARY_DIR})
target_include_directories(${LOGAN_DB_CLIENT_LIB} PRIVATE ${RE_COMMON_SOURCE_DIR})

target_link_libraries(${LOGAN_DB_CLIENT_LIB} ${PQXX_LIBRARIES})
target_link_libraries(${LOGAN_DB_CLIENT_LIB} ${Boost_PROGRAM_OPTIONS_LIBRARY})
target_link_libraries(${LOGAN_DB_CLIENT_LIB} ${PROTOBUF_LIBRARIES})


add_subdirectory("aggregationbroker")


# Inject SQL database initialisation file into AggServer source
file(READ ${CMAKE_CURRENT_SOURCE_DIR}/aggserverschema.sql SQL_INIT_DB_SCHEMA)
configure_file(sqlstrings.h.in sqlstrings.h @ONLY)

set(SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/aggregationserver.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/aggregationprotohandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/nodemanagerprotohandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/modeleventprotohandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/systemeventprotohandler.cpp
        #    ${CMAKE_CURRENT_SOURCE_DIR}/databaseclient.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/experimenttracker.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/utils.cpp
)
set(HEADERS
	${CMAKE_CURRENT_SOURCE_DIR}/aggregationserver.h
    ${CMAKE_CURRENT_SOURCE_DIR}/aggregationprotohandler.h
    ${CMAKE_CURRENT_SOURCE_DIR}/nodemanagerprotohandler.h
    ${CMAKE_CURRENT_SOURCE_DIR}/modeleventprotohandler.h
    ${CMAKE_CURRENT_SOURCE_DIR}/systemeventprotohandler.h
	${CMAKE_CURRENT_SOURCE_DIR}/databaseclient.h
    ${CMAKE_CURRENT_SOURCE_DIR}/experimenttracker.h
	${CMAKE_CURRENT_SOURCE_DIR}/experimentruninfo.h
    ${CMAKE_CURRENT_SOURCE_DIR}/utils.h
    ${CMAKE_CURRENT_SOURCE_DIR}/sqlstrings.h.in
	${CMAKE_CURRENT_SOURCE_DIR}/unboundedtimerange.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/prototimerange.hpp

    ${CMAKE_CURRENT_BINARY_DIR}/sqlstrings.h
		)


# Construct an aggregation_server binary
add_executable(${PROJ_NAME} ${SOURCES} ${HEADERS})
target_compile_features(${PROJ_NAME} PRIVATE cxx_std_17)

if (MSVC)
    # Windows requires protobuf in DLLs
	add_definitions(-DPROTOBUF_USE_DLLS)
	# Visual studio needs to be told to build in Multithreaded Dynamically Linked mode
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
else()
	# Unix needs to include pthread
	target_link_libraries(${PROJ_NAME} pthread)
    target_link_libraries(${PROJ_NAME} dl)
endif()

target_include_directories(${PROJ_NAME} PRIVATE ${PROTOBUF_INCLUDE_DIRS})
target_include_directories(${PROJ_NAME} PRIVATE ${ZMQ_INCLUDE_DIRS})
target_include_directories(${PROJ_NAME} PRIVATE ${LOGAN_SRC_PATH})
target_include_directories(${PROJ_NAME} PRIVATE ${PQXX_INCLUDE_DIRECTORIES})

target_include_directories(${PROJ_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(${PROJ_NAME} PRIVATE ${RE_COMMON_BINARY_DIR})
target_include_directories(${PROJ_NAME} PRIVATE ${RE_COMMON_SOURCE_DIR})


target_link_libraries(${PROJ_NAME} zmq_protoreceiver)
target_link_libraries(${PROJ_NAME} zmq_protorequester)
target_link_libraries(${PROJ_NAME} zmq_protowriter)
target_link_libraries(${PROJ_NAME} re_common_proto_modelevent)
target_link_libraries(${PROJ_NAME} re_common_proto_control)
target_link_libraries(${PROJ_NAME} re_common_proto_systemevent)
target_link_libraries(${PROJ_NAME} ${PQXX_LIBRARIES})
target_link_libraries(${PROJ_NAME} ${Boost_PROGRAM_OPTIONS_LIBRARY})
target_link_libraries(${PROJ_NAME} ${PROTOBUF_LIBRARIES})

target_link_libraries(${PROJ_NAME} ${LOGAN_DB_CLIENT_LIB})

#if(NOT DISABLE_MODEL_LOGGING)
#    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/modelprotohandler")
#    target_link_libraries(${LOGAN_SERVER_LIB} modelprotohandler)
#else()
#    target_compile_definitions(${LOGAN_SERVER_LIB} PRIVATE DISABLE_MODEL_HANDLER)
#endif()

#if(NOT DISABLE_HARDWARE_LOGGING)
#    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/hardwareprotohandler")
#    target_link_libraries(${LOGAN_SERVER_LIB} hardwareprotohandler)
#else()
#    target_compile_definitions(${LOGAN_SERVER_LIB} PRIVATE DISABLE_HARDWARE_HANDLER)
#endif()

#add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/managedserver")
#add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/standaloneserver")
