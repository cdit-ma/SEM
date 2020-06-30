project(MEDEA_PROJECT
        VERSION ${PROJECT_VERSION}
        LANGUAGES CXX)

#Set CMake Options
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set(CMAKE_MACOSX_RPATH ON)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH ON)

#Set output directory
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

#Boost 1.70 requires BUILD_SHARED_LIBS to be set to on (required for MSVC 2019)
if(MSVC)
    set(BUILD_SHARED_LIBS ON)
endif()

#Set MEDEA Options
#VERSION in form X.Y.Z(-P?)
SET(URL "https://github.com/cdit-ma/MEDEA")
SET(VERSION_NAME "MEDEAv${PROJECT_VERSION}")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

#Add Options for building projects, default to build everything
option(BUILD_APP "Build MEDEA App" ON)
option(BUILD_CLI "Build MEDEA CLI" ON)

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
SET(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
SET(CPACK_OUTPUT_FILE_PREFIX installers)
SET(CPACK_PACKAGE_EXECUTABLES "MEDEA;${VERSION_NAME}")
SET(CPACK_PACKAGE_FILE_NAME "MEDEA-v${PROJECT_VERSION}-${CMAKE_SYSTEM_NAME}-${CMAKE_HOST_SYSTEM_PROCESSOR}")
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
SET(CPACK_IFW_PRODUCT_URL ${HOMEPAGE_URL})
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
