# sqlite
# Exports sqlite3 target
message(STATUS "Fetching sqlite amalgamation")
FetchContent_Declare(sqlite
        URL https://www.sqlite.org/2020/sqlite-amalgamation-3330000.zip
        URL_HASH SHA3_256=fb7dfd39009fb40519097b0b1a6af5e8acb17006c848f6d6b7707a4a0c3885c3
        )
if (NOT sqlite_POPULATE)
    FetchContent_Populate(sqlite)

    add_library(sqlite3 STATIC)
    target_sources(sqlite3 PUBLIC
            ${sqlite_SOURCE_DIR}/sqlite3.c
            ${sqlite_SOURCE_DIR}/sqlite3.h
            )
    target_include_directories(sqlite3 PUBLIC ${sqlite_SOURCE_DIR})
endif ()
