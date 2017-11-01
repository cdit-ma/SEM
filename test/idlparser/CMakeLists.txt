set(PROJ_NAME test_idlparser)
find_package(Qt5 COMPONENTS Test REQUIRED)

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/idlparsertester.cpp
)

add_executable(${PROJ_NAME} ${SOURCE})

target_link_libraries(${PROJ_NAME} Qt5::Test)
target_link_libraries(${PROJ_NAME} medea_modelcontroller_lib)
target_link_libraries(${PROJ_NAME} medea_idl2graphml_lib)

#Run QT deploy
if(WIN32)
    add_custom_command(TARGET ${PROJ_NAME} POST_BUILD COMMAND $ENV{Qt5_DIR}/bin/windeployqt ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif()