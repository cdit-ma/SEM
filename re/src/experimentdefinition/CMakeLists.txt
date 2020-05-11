project(experiment_definition VERSION ${PROJECT_VERSION})


add_library(${PROJECT_NAME} STATIC "")
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/..)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

target_sources(${PROJECT_NAME} PRIVATE
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

target_link_libraries(${PROJECT_NAME} PUBLIC experiment_definition_protocol)
target_link_libraries(${PROJECT_NAME} PUBLIC re_types)
target_link_libraries(${PROJECT_NAME} PUBLIC re_graphml_parser_lib)

if (BUILD_TEST)
    add_subdirectory(test)
endif ()