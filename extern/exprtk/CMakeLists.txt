# exprtk
# Exports exprtk target
message(STATUS "Fetching exprtk")
FetchContent_Declare(exprtk
        GIT_REPOSITORY https://github.com/ArashPartow/exprtk
        GIT_TAG 2fe9235cdb6f3019ee29b3fe6f7651474495c5cf
        )
if (NOT exprtk_POPULATE)
    FetchContent_Populate(exprtk)

    add_library(exprtk INTERFACE)
    target_sources(exprtk INTERFACE
            ${exprtk_SOURCE_DIR}/exprtk.hpp
            )
    target_include_directories(exprtk INTERFACE ${exprtk_SOURCE_DIR})
endif ()
