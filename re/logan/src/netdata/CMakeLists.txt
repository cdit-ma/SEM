
find_package(Boost REQUIRED COMPONENTS system regex)

add_library(sem_netdata_adapter STATIC)

target_sources(sem_netdata_adapter
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/tcp_receiver.h
        ${CMAKE_CURRENT_SOURCE_DIR}/chart_listener.hpp
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/json_info.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/tcp_receiver.cpp
        )

target_include_directories(sem_netdata_adapter
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(sem_netdata_adapter
        PUBLIC
        Boost::boost
        Boost::system
        Boost::regex
        Boost::dynamic_linking
        )