set(AGGREGATION_BROKER "aggregation_broker")
set(PROJ_NAME ${AGGREGATION_BROKER})

find_package(Boost 1.30.0 COMPONENTS program_options REQUIRED)


# Construct an aggregation_server binary
add_executable(aggregation_broker)
target_sources(aggregation_broker PRIVATE
	# Sources
	${CMAKE_CURRENT_SOURCE_DIR}/../utils.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/aggregationbroker.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/aggregationreplier.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/main.cpp

	# Headers
	${CMAKE_CURRENT_SOURCE_DIR}/../databaseclient.h
	${CMAKE_CURRENT_SOURCE_DIR}/../utils.h
	${CMAKE_CURRENT_SOURCE_DIR}/aggregationbroker.h
	${CMAKE_CURRENT_SOURCE_DIR}/aggregationreplier.h
)

#target_compile_definitions(aggregation_broker PRIVATE -DBOOST_ALL_DYN_LINK)
target_compile_features(aggregation_broker PRIVATE cxx_std_17)
if (CMAKE_CXX_COMPILER_ID MATCHES "[cC][lL][aA][nN][gG]") #Case insensitive match
	target_compile_options(${PROJECT_NAME} PRIVATE -Wdocumentation )
endif()

if(UNIX)
	# Unix needs to include pthread
	target_link_libraries(aggregation_broker pthread)
    target_link_libraries(aggregation_broker dl)
endif()


target_include_directories(aggregation_broker PRIVATE ${LOGAN_SRC_PATH})

#target_include_directories(aggregation_broker PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(aggregation_broker PRIVATE ${RE_COMMON_BINARY_DIR})
target_include_directories(aggregation_broker PRIVATE ${RE_COMMON_SOURCE_DIR})

target_link_libraries(aggregation_broker 
		PRIVATE
		zmq_protoreplier
		re_common_proto_aggregation_message
		Boost::program_options
		Boost::dynamic_linking
		protobuf::libprotobuf
		logan_database_client_lib
		)
