#Set our project names
set(PROJ_NAME test_periodicevent)
project(${PROJ_NAME})

include_directories(${GOOGLE_TEST_DIR}/include)

SET(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/periodiceventtest.cpp
)


add_executable(${PROJ_NAME} ${SOURCE})
if (MSVC)
    #Windows requires protobuf in DLLs
    add_definitions(-DPROTOBUF_USE_DLLS)
    #Visual studio needs to be told to build in Multithreaded Dynamically Linked mode
    #set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
elseif(UNIX)
    target_link_libraries(${PROJ_NAME} pthread)
endif(UNIX)
target_link_libraries(${PROJ_NAME} re_core)
target_link_libraries(${PROJ_NAME} gtest)