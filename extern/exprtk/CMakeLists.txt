# exprtk
# Exports exprtk target
message(STATUS "Fetching exprtk")
FetchContent_Declare(exprtk
        GIT_REPOSITORY https://github.com/ArashPartow/exprtk
        GIT_TAG d312ba91419c9cb12c8279fd3a19096d39dfcb5e
        )
if (NOT exprtk_POPULATE)
    FetchContent_Populate(exprtk)

    add_library(exprtk INTERFACE)
    target_sources(exprtk INTERFACE
            ${exprtk_SOURCE_DIR}/exprtk.hpp
            )
    target_include_directories(exprtk INTERFACE ${exprtk_SOURCE_DIR})
endif ()
