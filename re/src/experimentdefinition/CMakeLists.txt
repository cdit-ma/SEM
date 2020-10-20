add_library(experiment_definition STATIC)
target_compile_features(experiment_definition PRIVATE cxx_std_17)
target_include_directories(experiment_definition
        PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR}/..
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
        )

target_sources(experiment_definition
        PRIVATE
        experimentdefinition.h
        attributedefinition.h
        attributeinstance.h
        componentdefinition.h
        componentinstance.h
        componentassembly.h
        periodicportdefinition.h
        periodicportinstance.h
        triggerdefinition.h
        triggerinstance.h
        workerdefinition.h
        workerinstance.h
        container.h
        strategy.h
        portdelegateinstance.h
        modelentity.h
        coredata.h
        node.h
        cluster.h
        middlewareportinstance.h
        middlewareportdefinition.h
        loggingclientdefinition.h
        loggingserverdefinition.h


        experimentdefinition.cpp
        attributedefinition.cpp
        attributeinstance.cpp
        componentdefinition.cpp
        componentinstance.cpp
        componentassembly.cpp
        periodicportdefinition.cpp
        periodicportinstance.cpp
        triggerdefinition.cpp
        triggerinstance.cpp
        workerdefinition.cpp
        workerinstance.cpp
        container.cpp
        strategy.cpp
        portdelegateinstance.cpp
        modelentity.cpp
        coredata.cpp
        node.cpp
        cluster.cpp
        middlewareportinstance.cpp
        middlewareportdefinition.cpp
        loggingclientdefinition.cpp
        loggingserverdefinition.cpp
        )

target_link_libraries(experiment_definition
        PUBLIC
        experiment_definition_protocol
        sem_types
        re_graphml_parser_lib
        )

if (BUILD_TEST)
    add_subdirectory(test)
endif ()