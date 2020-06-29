if (Sigar_FOUND)
    add_subdirectory(sigar)
else ()
    add_subdirectory(dummy)
endif ()

