set(PROJ_NAME medea_test)
find_package(Qt5 COMPONENTS Test REQUIRED)


set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/modelcontrollertests.cpp
)

SET(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/modelcontrollertests.h
)

set(RESOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/models.qrc
    )

add_executable(${PROJ_NAME} ${SOURCE} ${HEADERS} ${RESOURCES} ${MAC_ICON})

target_link_libraries(${PROJ_NAME} Qt5::Test)
target_link_libraries(${PROJ_NAME} medea_modelcontroller_lib)

INSTALL(TARGETS ${PROJ_NAME} RUNTIME DESTINATION . COMPONENT ${PROJ_NAME})
