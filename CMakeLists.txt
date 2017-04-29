cmake_minimum_required (VERSION 2.6)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "/Users/dan/Qt5.7.0/5.7/clang_64")

set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR})

add_subdirectory("app")
add_subdirectory("test")
add_subdirectory("src")


