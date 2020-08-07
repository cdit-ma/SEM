
# pqxx
message(STATUS "Fetching pqxx")
FetchContent_Declare(pqxx)
if (NOT pqxx_POPULATE)
    FetchContent_Populate(pqxx
            GIT_REPOSITORY https://github.com/jtv/libpqxx
            GIT_TAG 7.1.2
            GIT_SHALLOW TRUE
            )
    add_subdirectory(${pqxx_SOURCE_DIR} ${pqxx_BINARY_DIR})
endif ()