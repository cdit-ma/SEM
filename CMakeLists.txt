cmake_minimum_required (VERSION 3.0)

project(MEDEA_PROJECT
        VERSION 3.4.0
        LANGUAGES CXX)

#Set CMake Options
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set(CMAKE_MACOSX_RPATH ON)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH ON)

#Set output directory
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

# Use CCache to speed up compilations on Clang/GCC
find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif(CCACHE_FOUND)

if(NOT DEFINED ENV{Qt5_DIR})
    message(FATAL_ERROR "Environment Variable: 'Qt5_DIR' not set!")
endif()

#Boost 1.70 requires BUILD_SHARED_LIBS to be set to on (required for MSVC 2019)
if(MSVC)
    set(BUILD_SHARED_LIBS ON)
endif()

if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/re/CMakeLists.txt)
   message(FATAL_ERROR "'re' submodule does not exist! Have you run git submodule init && git submodule update?")
endif()

#Set MEDEA Options
#VERSION in form X.Y.Z(-P?)
SET(VERSION_NO "3.3.9")
SET(VERSION "${VERSION_NO}-VIS-1")
SET(URL "https://github.com/cdit-ma/MEDEA")
SET(VERSION_NAME "MEDEAv${VERSION}")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

#Add Options for building projects, default to build everything
option(BUILD_APP "Build MEDEA App" ON)
option(BUILD_CLI "Build MEDEA CLI" ON)
option(BUILD_TEST "Build MEDEA Test" OFF)
option(BUILD_DOCS "Build MEDEA Developer documentation" ON)

if(BUILD_APP OR BUILD_CLI OR BUILD_TEST)
    message(STATUS "Building MEDEA Core")
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/src/modelcontroller")
endif()

if(BUILD_CLI)
    message(STATUS "Building MEDEA CLI")
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/src/cli")
endif(BUILD_CLI)

if(BUILD_APP)
    message(STATUS "Building MEDEA App")
    
    set(CMAKE_MODULE_PATH "$ENV{CMAKE_MODULE_PATH}")
    if (NOT CMAKE_MODULE_PATH)
        set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/re/cmake_modules")
    endif()

    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/src/app")
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/re/re_common")
endif(BUILD_APP)

if(BUILD_TEST)
    enable_testing()
    message(STATUS "Building MEDEA Tests")
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/test")
endif(BUILD_TEST)

if(BUILD_DOCS)
    message(STATUS "Building MEDEA Documentation")
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/docs")
endif(BUILD_DOCS)

SET(CPACK_GENERATOR "IFW;ZIP")
# Set CPACK generic info
SET(CPACK_PACKAGE_NAME "MEDEA")
SET(CPACK_PACKAGE_VENDOR "CDIT-MA")
SET(CPACK_PACKAGE_VERSION ${VERSION_NO})
SET(CPACK_OUTPUT_FILE_PREFIX installers)
SET(CPACK_PACKAGE_EXECUTABLES "MEDEA;${VERSION_NAME}")
SET(CPACK_PACKAGE_FILE_NAME "MEDEA-v${VERSION}-${CMAKE_SYSTEM_NAME}-${CMAKE_HOST_SYSTEM_PROCESSOR}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Modelling, Experiment DEsign and Analysis (MEDEA)")
set(CPACK_PACKAGE_INSTALL_DIRECTORY ${VERSION_NAME})

#Set IFW specific options
SET(CPACK_IFW_TARGET_DIRECTORY "@HomeDir@/${VERSION_NAME}")

# Set Icon
if(APPLE)
    SET(CPACK_IFW_PACKAGE_ICON ${CMAKE_CURRENT_SOURCE_DIR}/src/app/Resources/medea.icns)
elseif(WIN32)
    SET(CPACK_IFW_PACKAGE_ICON ${CMAKE_CURRENT_SOURCE_DIR}/src/app/Resources/medea.ico)
endif()

# Set URL/Title
SET(CPACK_IFW_PRODUCT_URL ${URL})
SET(CPACK_IFW_PACKAGE_TITLE ${VERSION_NAME})
SET(CPACK_IFW_PACKAGE_BANNER ${CMAKE_CURRENT_SOURCE_DIR}/src/app/Resources/Icons/medeaBanner.png)
SET(CPACK_IFW_PACKAGE_WIZARD_STYLE "Modern")

# Has to be last
INCLUDE(CPack)

cpack_add_component (MEDEA DISPLAY_NAME "Core: MEDEA Modelling Tool" REQUIRED)
cpack_add_component (idl2graphml DISPLAY_NAME "Util: IDL to GraphML parsing shell tool")
cpack_add_component (medea_cli DISPLAY_NAME "Util: MEDEA cli shell tool")

if(WIN32)
    # Do the windows only options
    cpack_add_component (QT_LIBS DISPLAY_NAME "Core: Qt Binaries" REQUIRED)
    cpack_add_component (SCRIPTS DISPLAY_NAME "Core: Jenkins Scripts" REQUIRED)
    cpack_add_component (RE_GEN DISPLAY_NAME "Core: re_gen XSL transforms" REQUIRED)
endif()
