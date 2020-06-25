add_library(dummy_system_info_provider STATIC "")
target_sources(dummy_system_info_provider
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/dummysysteminfo.h
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/dummysysteminfo.cpp
        )

target_include_directories(dummy_system_info_provider
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        )

target_link_libraries(dummy_system_info_provider PUBLIC system_info)
