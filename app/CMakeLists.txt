cmake_minimum_required (VERSION 2.6)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)


SET(PROJ_NAME "MEDEA")

SET(TRANSFORM_DEST_DIR "Resources/re_gen/")
file(GLOB TRANSFORMS 
        ${CMAKE_CURRENT_SOURCE_DIR}/../re_gen/*.xsl
        ${CMAKE_CURRENT_SOURCE_DIR}/../re_gen/*.jar
        )

SET(SCRIPTS_DEST_DIR "Resources/scripts/")
file(GLOB SCRIPTS 
        ${CMAKE_CURRENT_SOURCE_DIR}/../scripts/*.*
        )

SET(LIBS_DEST_DIR "Resources/libs/")
file(GLOB LIBS 
        ${CMAKE_CURRENT_SOURCE_DIR}/../lib/Saxon/*.jar
        )


if(APPLE)
    set(MACOSX_BUNDLE_ICON_FILE medea.icns)
    set(MACOSX_BUNDLE_LONG_VERSION_STRING "${PROJ_NAME} v${VERSION}")
    set(MACOSX_BUNDLE_BUNDLE_NAME ${PROJ_NAME})
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING ${VERSION})
    set(MACOSX_BUNDLE_BUNDLE_VERSION ${VERSION})
    set(MACOSX_BUNDLE_COPYRIGHT "Copyright 2016. All Rights Reserved.")

    #Bundle the MEDEA icon (icns) for mac
    set(MAC_ICON ${CMAKE_CURRENT_SOURCE_DIR}/../src/Resources/medea.icns)
    set_source_files_properties(${MAC_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
    set_source_files_properties(${TRANSFORMS} PROPERTIES MACOSX_PACKAGE_LOCATION "MacOS/${TRANSFORM_DEST_DIR}")
    set_source_files_properties(${SCRIPTS} PROPERTIES MACOSX_PACKAGE_LOCATION "MacOS/${SCRIPTS_DEST_DIR}")
    set_source_files_properties(${LIBS} PROPERTIES MACOSX_PACKAGE_LOCATION "MacOS/${LIBS_DEST_DIR}")

    add_executable(${PROJ_NAME} MACOSX_BUNDLE ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp ${MAC_ICON} ${TRANSFORMS} ${SCRIPTS} ${LIBS})
else()
    #set_source_files_properties(${TRANSFORMS} PROPERTIES MACOSX_PACKAGE_LOCATION "MacOS/${TRANSFORM_DEST_DIR}")
    add_executable(${PROJ_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/app/main.cpp${TRANSFORMS})
endif()

target_link_libraries(${PROJ_NAME} MEDEA_LIB)