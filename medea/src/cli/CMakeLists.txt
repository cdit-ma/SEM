set(PROJ_NAME medea_cli)
project(${PROJ_NAME})

set(CMAKE_AUTOMOC OFF)
set(CMAKE_AUTORCC OFF)

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/medeacli.cpp
)

set(HEADERS
)

add_executable(${PROJ_NAME} ${SOURCE} ${HEADERS})
target_link_libraries(${PROJ_NAME} PUBLIC medea_modelcontroller_lib)

INSTALL(TARGETS ${PROJ_NAME} RUNTIME DESTINATION . COMPONENT ${PROJ_NAME})