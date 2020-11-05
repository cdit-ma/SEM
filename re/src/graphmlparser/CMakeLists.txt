add_library(re_graphml_parser_lib STATIC)

target_sources(re_graphml_parser_lib
        PUBLIC
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/assembly.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/assembly.h
        ${CMAKE_CURRENT_SOURCE_DIR}/graphmlparser.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/graphmlparser.h
        ${CMAKE_CURRENT_SOURCE_DIR}/modelparser.h
        ${CMAKE_CURRENT_SOURCE_DIR}/modelparser.cpp
        )

target_compile_features(re_graphml_parser_lib PRIVATE cxx_std_11)

target_include_directories(re_graphml_parser_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(re_graphml_parser_lib
        PRIVATE
        PUBLIC
        pugixml
        re_common_proto_control
        )


add_executable(re_graphml_parser)
target_compile_features(re_graphml_parser PRIVATE cxx_std_11)
target_link_libraries(re_graphml_parser PRIVATE re_graphml_parser_lib)
target_sources(re_graphml_parser
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
        )
