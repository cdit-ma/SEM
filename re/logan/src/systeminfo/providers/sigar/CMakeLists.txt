add_library(sigar_system_info_provider STATIC "")
target_sources(sigar_system_info_provider
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/sigarsysteminfo.h
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/sigarsysteminfo.cpp
        )

target_include_directories(sigar_system_info_provider
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        PRIVATE
        ${Sigar_INCLUDE_DIRS}
        )

target_link_libraries(sigar_system_info_provider
        PUBLIC
        system_info
        PRIVATE
        ${Sigar_LIBRARIES}
        )
