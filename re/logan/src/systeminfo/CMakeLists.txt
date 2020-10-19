add_subdirectory(providers)

add_library(system_info INTERFACE)
target_compile_features(system_info INTERFACE cxx_std_11)

target_sources(system_info
        INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/systeminfo.h
        )

target_include_directories(system_info
        INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR})


add_library(system_info_broker STATIC)
target_sources(system_info_broker
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/systeminfobroker.h
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/systeminfobroker.cpp
        )

target_link_libraries(system_info_broker
        PUBLIC
        re_common_proto_systemevent
        )

if (TARGET sigar)
    target_link_libraries(system_info_broker
            PRIVATE
            sigar_system_info_provider
            )
else ()
    message(WARNING "Sigar libraries not found, building dummy_system_info_provider")
    target_link_libraries(system_info_broker
            PRIVATE
            dummy_system_info_provider
            )
endif ()

target_include_directories(system_info_broker PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
