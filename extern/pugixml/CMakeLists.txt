
# Pugixml
message(STATUS "Fetching PugiXML")
set(BUILD_TESTS OFF)
cmake_policy(SET CMP0077 NEW)
FetchContent_Declare(
        pugixml
        GIT_REPOSITORY https://github.com/zeux/pugixml.git
        GIT_TAG v1.10
)
if (NOT pugixml_POPULATE)
    FetchContent_Populate(pugixml)
    add_subdirectory(${pugixml_SOURCE_DIR} ${pugixml_BINARY_DIR})
endif ()