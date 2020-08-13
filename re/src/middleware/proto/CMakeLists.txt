add_library(prototranslator INTERFACE)
target_sources(prototranslator
        INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/prototranslator.hpp)
target_include_directories(prototranslator
        INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}
        )