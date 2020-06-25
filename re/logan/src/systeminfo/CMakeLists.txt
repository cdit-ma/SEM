add_subdirectory(providers)

find_package(SIGAR)

add_library(system_info INTERFACE)
target_compile_features(system_info INTERFACE cxx_std_11)

target_sources(system_info
        INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/systeminfo.h
        )

target_include_directories(system_info INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})


add_library(system_info_handler STATIC "")
target_sources(system_info_handler
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/systeminfohandler.h
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/systeminfohandler.cpp
        )

target_link_libraries(system_info_handler
        PRIVATE
        re_common_proto_systemevent
        )

if (SIGAR_FOUND)
    target_link_libraries(system_info_handler
            PRIVATE
            sigar_system_info_provider
            )
else ()
    message(WARNING "SIGAR libraries not found, building dummy_system_info_provider")
    target_link_libraries(system_info_handler
            PRIVATE
            dummy_system_info_provider)
endif ()

target_include_directories(system_info_handler PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
