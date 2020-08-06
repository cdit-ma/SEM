if (TARGET sigar)
    add_subdirectory(sigar)
else ()
    add_subdirectory(dummy)
endif ()

