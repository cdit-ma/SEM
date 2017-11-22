#Set our project names
set(PROJ_NAME test_cpuworker)
project(${PROJ_NAME})

#Get the RE_PATH
set(RE_PATH "$ENV{RE_PATH}")


SET(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/cpuworkertest.cpp
    ${GOOGLE_TEST_DIR}/src/gtest_main.cc
)


#Add RE_PATH to include directories
include_directories("${RE_PATH}/src")
include_directories(${GOOGLE_TEST_DIR}/include)

add_executable(${PROJ_NAME} ${SOURCE})
if (MSVC)
    #Windows requires protobuf in DLLs
    add_definitions(-DPROTOBUF_USE_DLLS)
    #Visual studio needs to be told to build in Multithreaded Dynamically Linked mode
    #set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
elseif(UNIX)
    target_link_libraries(${PROJ_NAME} pthread)
endif(UNIX)
target_link_libraries(${PROJ_NAME} cpu_worker)
target_link_libraries(${PROJ_NAME} gtest)