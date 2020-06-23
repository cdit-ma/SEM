set(LOGAN_CLIENT "logan_client")
set(LOGAN_CLIENT_LIB ${LOGAN_CLIENT}_lib)
set(PROJ_NAME ${LOGAN_CLIENT})
project(${PROJ_NAME} VERSION ${PROJECT_VERSION})

find_package(Boost 1.30.0 COMPONENTS program_options REQUIRED)
find_package(SIGAR)

if (NOT SIGAR_FOUND)
    message(WARNING "Could not find sigar libraries, NOT building logan client or logan client libraries.")
endif ()

# Library
add_library(${LOGAN_CLIENT_LIB} STATIC "")
target_compile_features(${LOGAN_CLIENT_LIB} PRIVATE cxx_std_11)

target_sources(${LOGAN_CLIENT_LIB}
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/systeminfo.h
        ${CMAKE_CURRENT_SOURCE_DIR}/systeminfo.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/logcontroller.h
        ${CMAKE_CURRENT_SOURCE_DIR}/logcontroller.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/dummysysteminfo.h
        ${CMAKE_CURRENT_SOURCE_DIR}/dummysysteminfo.cpp
        )
target_include_directories(${LOGAN_CLIENT_LIB} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${LOGAN_CLIENT_LIB}
        PUBLIC
        re_common_proto_systemevent
        re_common_proto_control
        zmq_protowriter
        )
if (SIGAR_FOUND)
    target_sources(${LOGAN_CLIENT_LIB}
            PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/sigarsysteminfo.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/sigarsysteminfo.h
            )
    target_link_libraries(${LOGAN_CLIENT_LIB} PRIVATE ${SIGAR_LIBRARIES})
    target_include_directories(${LOGAN_CLIENT_LIB} PUBLIC ${SIGAR_INCLUDE_DIRS})
    target_compile_definitions(${LOGAN_CLIENT_LIB} PRIVATE CDIT_MA_SIGAR_FOUND)
endif (SIGAR_FOUND)

if (UNIX)
    target_link_libraries(${LOGAN_CLIENT_LIB} PUBLIC pthread)
endif (UNIX)


# Executable
add_executable(${LOGAN_CLIENT} "")
target_compile_features(${LOGAN_CLIENT} PRIVATE cxx_std_11)

target_sources(${LOGAN_CLIENT} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
target_include_directories(${LOGAN_CLIENT} PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(${LOGAN_CLIENT}
        PRIVATE
        ${LOGAN_CLIENT_LIB}
        ${Boost_PROGRAM_OPTIONS_LIBRARY}
        sem_version
        )
