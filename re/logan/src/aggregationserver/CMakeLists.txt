#Find packages
find_package(Boost 1.30.0 COMPONENTS program_options REQUIRED)

# Build the DatabaseClient library
set(LOGAN_DB_CLIENT_LIB "logan_database_client_lib")
add_library(${LOGAN_DB_CLIENT_LIB} STATIC "")
target_compile_features(${LOGAN_DB_CLIENT_LIB} PRIVATE cxx_std_17)
target_sources(${LOGAN_DB_CLIENT_LIB} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/databaseclient.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/utils.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/databaseclient.h
        ${CMAKE_CURRENT_SOURCE_DIR}/utils.h
		${CMAKE_CURRENT_SOURCE_DIR}/unboundedtimerange.hpp
		)

target_include_directories(${LOGAN_DB_CLIENT_LIB} PRIVATE ${PROTOBUF_INCLUDE_DIRS})
target_include_directories(${LOGAN_DB_CLIENT_LIB} PRIVATE ${LOGAN_SRC_PATH})
target_include_directories(${LOGAN_DB_CLIENT_LIB} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(${LOGAN_DB_CLIENT_LIB} PRIVATE ${RE_COMMON_BINARY_DIR})
target_include_directories(${LOGAN_DB_CLIENT_LIB} PRIVATE ${RE_COMMON_SOURCE_DIR})

if (TARGET pqxx)
        target_link_libraries(${LOGAN_DB_CLIENT_LIB} PUBLIC pqxx)
elseif (TARGET pqxx_static)
        # This fallback triggers on Ubuntu and centos
	message(WARNING "pqxx target not found, falling back to pqxx_static")
        target_link_libraries(${LOGAN_DB_CLIENT_LIB} PUBLIC pqxx_static)
else ()
        message(FATAL_ERROR "No viable libpqxx target found (couldn't find pqxx or pqxx_static")
endif ()
target_link_libraries(${LOGAN_DB_CLIENT_LIB} PUBLIC protobuf::libprotobuf)


add_subdirectory("aggregationbroker")


# Inject SQL database initialisation file into AggServer source
file(READ ${CMAKE_CURRENT_SOURCE_DIR}/aggserverschema.sql SQL_INIT_DB_SCHEMA)
configure_file(sqlstrings.h.in sqlstrings.h @ONLY)

# Construct an aggregation_server binary
add_executable(aggregation_server)
target_compile_features(aggregation_server PRIVATE cxx_std_17)

if (UNIX)
	# Unix needs to include pthread
	target_link_libraries(aggregation_server pthread)
    target_link_libraries(aggregation_server dl)
endif()

target_sources(aggregation_server
    PRIVATE
    # Sources
	${CMAKE_CURRENT_SOURCE_DIR}/aggregationserver.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/aggregationprotohandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/nodemanagerprotohandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/modeleventprotohandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/systemeventprotohandler.cpp
        #    ${CMAKE_CURRENT_SOURCE_DIR}/databaseclient.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/experimenttracker.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/utils.cpp
    # Headers
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
target_include_directories(aggregation_server
        PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}
        ${LOGAN_SRC_PATH}
        ${RE_COMMON_BINARY_DIR}
        ${RE_COMMON_SOURCE_DIR}
        )


target_link_libraries(aggregation_server
        PUBLIC
        Boost::program_options
        PRIVATE
        zmq_protoreceiver
        zmq_protorequester
        zmq_protowriter
        re_common_proto_modelevent
        re_common_proto_control
        re_common_proto_systemevent
        ${LOGAN_DB_CLIENT_LIB}
        )