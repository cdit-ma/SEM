project(medea_cli)

set(CMAKE_AUTOMOC OFF)
set(CMAKE_AUTORCC OFF)



add_executable(medea_cli)
target_sources(medea_cli
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/medeacli.cpp)
target_link_libraries(medea_cli
        PUBLIC
        medea_modelcontroller_lib
        PRIVATE
        sem_version
        )

INSTALL(TARGETS medea_cli RUNTIME DESTINATION . COMPONENT medea_cli)