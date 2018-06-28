<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="2.0" 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:xs="http://www.w3.org/2001/XMLSchema"
    xmlns:gml="http://graphml.graphdrawing.org/xmlns"
    xmlns:o="http://github.com/cdit-ma/re_gen/o"
    xmlns:cdit="http://github.com/cdit-ma/re_gen/cdit"
    xmlns:graphml="http://github.com/cdit-ma/re_gen/graphml"
    xmlns:cmake="http://github.com/cdit-ma/re_gen/cmake"
    xmlns:cpp="http://github.com/cdit-ma/re_gen/cpp"
    exclude-result-prefixes="gml">

    <xsl:output method="text" omit-xml-declaration="yes" indent="yes" standalone="no" />

    <!-- Load in Functions -->
    <xsl:import href="libraries/general_functions.xsl"/>
    <xsl:import href="libraries/cpp_functions.xsl"/>
    <xsl:import href="libraries/cmake_functions.xsl"/>
    <xsl:import href="libraries/graphml_functions.xsl"/>
    <xsl:import href="libraries/cdit_functions.xsl"/> 
    <xsl:import href="libraries/cdit_cmake_functions.xsl"/>
    <xsl:import href="libraries/component_functions.xsl"/>
    <xsl:import href="libraries/proto_functions.xsl"/>
    <xsl:import href="libraries/idl_functions.xsl"/>
    <xsl:import href="libraries/datatype_functions.xsl"/>

    <!-- Parameters -->
    <xsl:param name="generate_all" as="xs:boolean" select="false()" />
    <xsl:param name="debug_mode" as="xs:boolean" select="true()" />
    <xsl:param name="library_prefix" as="xs:string" select="''" />

    <xsl:template match="/*">
        <xsl:variable name="model" select="graphml:get_model(.)" />
        
        <!-- Construct a list of ComponentImpl Objects to code-gen -->
        <xsl:variable name="component_impls" select="cdit:get_components_to_build($model, $generate_all)" />
        <xsl:variable name="classes" select="cdit:get_classes_to_build($model, $generate_all)" />
        <xsl:variable name="middlewares" select="cdit:get_utilized_middlewares($model, $generate_all)" as="xs:string*" />
        <xsl:variable name="enums" select="graphml:get_nodes_of_kind($model, 'Enum')"/>

        <xsl:for-each select="$middlewares">
            <xsl:variable name="middleware" select="." />
            <xsl:value-of select="o:message(('Generating Middleware:', o:wrap_quote($middleware)))" />

            <xsl:variable name="datatype_aggregates" select="cdit:get_aggregates_for_middleware($model, $middleware, $generate_all)" />
            <xsl:variable name="pubsub_aggregates" select="cdit:get_pubsub_aggregates_for_middleware($model, $middleware, $generate_all)" />
            <xsl:variable name="server_interfaces" select="cdit:get_serverinterfaces_for_middleware($model, $middleware, $generate_all)" />
            
            <!-- Generate the Shared Library translate/middleware classes -->
            <xsl:if test="cdit:build_shared_library($middleware)">
                <xsl:for-each select="$datatype_aggregates">
                    <xsl:variable name="aggregate" select="." />
                    
                    <xsl:variable name="qualified_type" select="cdit:get_aggregate_qualified_type($aggregate, $middleware)" />
                    <xsl:variable name="file_label" select="cdit:get_aggregate_file_prefix($aggregate, $middleware)" />
                    <xsl:variable name="aggregate_path" select="cdit:get_aggregate_path_for_middleware(., $middleware)" />
                
                    <xsl:value-of select="o:message(('Generating Datatype:', o:wrap_quote($qualified_type)) )" />

                    <xsl:if test="$middleware != 'base'">
                        <!-- Generate the Translator CPP -->
                        <xsl:result-document href="{o:write_file(($aggregate_path, 'translator.cpp'))}">
                            <xsl:value-of select="cdit:get_translator_cpp($aggregate, $middleware)" />
                        </xsl:result-document>

                        <!-- Generate the Proto File -->
                        <xsl:if test="cdit:middleware_requires_proto_file($middleware)">
                            <xsl:variable name="proto_file" select="concat($file_label, '.proto')" />
                            <xsl:result-document href="{o:write_file(($aggregate_path, $proto_file))}">
                                <xsl:value-of select="cdit:get_proto_file($aggregate)" />
                            </xsl:result-document>
                        </xsl:if>

                        <!-- Generate the IDL File -->
                        <xsl:if test="cdit:middleware_requires_idl_file($middleware)">
                            <xsl:variable name="idl_file" select="concat($file_label, '.idl')" />
                            <xsl:result-document href="{o:write_file(($aggregate_path, $idl_file))}">
                                <xsl:value-of select="cdit:get_idl_datatype($aggregate, $middleware)" />
                            </xsl:result-document>
                        </xsl:if>

                        <!-- Generate the CMakeFile -->
                        <xsl:result-document href="{o:write_file(($aggregate_path, cmake:cmake_file()))}">
                            <xsl:value-of select="cdit:get_datatype_cmake($aggregate, $middleware)" />
                        </xsl:result-document>
                    </xsl:if>

                    <!-- Genaret the Bas -->
                    <xsl:if test="$middleware = 'base'">
                        <!-- Generate the Base Aggregate Header -->
                        <xsl:result-document href="{o:write_file(($aggregate_path, concat($file_label, '.h')))}">
                            <xsl:value-of select="cdit:get_aggregate_base_h($aggregate)" />
                        </xsl:result-document>

                        <!-- Generate the Base Aggregate CPP -->
                        <xsl:result-document href="{o:write_file(($aggregate_path, concat($file_label, '.cpp')))}">
                            <xsl:value-of select="cdit:get_aggregate_base_cpp($aggregate)" />
                        </xsl:result-document>

                        <!-- Generate the Base Aggregate CMakeFile -->
                        <xsl:result-document href="{o:write_file(($aggregate_path, cmake:cmake_file()))}">
                            <xsl:value-of select="cdit:get_aggregate_base_cmake($aggregate)" />
                        </xsl:result-document>
                    </xsl:if>
                </xsl:for-each>
                
                <!-- Generate the middleware CMakeFile -->
                <xsl:result-document href="{o:write_file((('datatypes', $middleware), cmake:cmake_file()))}">
                    <xsl:value-of select="cdit:get_middleware_cmake($datatype_aggregates)" />
                </xsl:result-document>
            </xsl:if>

            <!-- Generate the Pub/Sub Port for a Middleware -->
            <xsl:if test="cdit:build_module_library($middleware)">
                <xsl:for-each select="$pubsub_aggregates">
                    <xsl:variable name="aggregate" select="." />
                    <xsl:variable name="qualified_type" select="cdit:get_aggregate_qualified_type($aggregate, $middleware)" />
                    <xsl:variable name="port_path" select="cdit:get_pubsub_path($middleware, $aggregate)" />
                    <xsl:value-of select="o:message(('Generating Pub/Sub Port:', o:wrap_quote($qualified_type)) )" />
                
                    <!-- Generate the Pub/Sub Port Export CPP -->
                    <xsl:result-document href="{o:write_file(($port_path, 'libportexport.cpp'))}">
                        <xsl:value-of select="cdit:get_port_pubsub_export($aggregate, $middleware)" />
                    </xsl:result-document>

                    <!-- Generate the Pub/Sub CMake File -->
                    <xsl:result-document href="{o:write_file(($port_path, cmake:cmake_file()))}">
                        <xsl:value-of select="cdit:get_pubsub_cmake($aggregate, $middleware)" />
                    </xsl:result-document>
                </xsl:for-each>

                <!-- Generate the Pub/Sub CMakeFile -->
                <xsl:result-document href="{o:write_file((('ports', 'pubsub', $middleware), cmake:cmake_file()))}">
                    <xsl:value-of select="cdit:get_middleware_cmake($pubsub_aggregates)" />
                </xsl:result-document>
            </xsl:if>

            <!-- Generate the Request/Reply Ports -->
            <xsl:if test="cdit:build_module_library($middleware)">
                <xsl:for-each select="$server_interfaces">
                    <xsl:variable name="server_interface" select="." />
                    <xsl:variable name="qualified_type" select="cdit:get_aggregate_qualified_type($server_interface, $middleware)" />
                    <xsl:variable name="reqrep_path" select="cdit:get_reqrep_path($middleware, $server_interface)" />
                    <xsl:value-of select="o:message(('Generating Req/Rep Port:', o:wrap_quote($qualified_type)) )" />

                    <!-- Generate the Module(dll) port export class-->
                    <xsl:result-document href="{o:write_file(($reqrep_path, 'libportexport.cpp'))}">
                        <xsl:value-of select="cdit:get_port_requestreply_export($server_interface, $middleware)" />
                    </xsl:result-document>

                    <xsl:result-document href="{o:write_file(($reqrep_path, cmake:cmake_file()))}">
                        <xsl:value-of select="cdit:get_requestreply_cmake($server_interface, $middleware)" />
                    </xsl:result-document>

                    <xsl:if test="$middleware = 'tao'">
                        <xsl:variable name="file_label" select="cdit:get_aggregate_file_prefix($server_interface, $middleware)" />
                        <xsl:variable name="idl_file" select="concat($file_label, '.idl')" />
                        <xsl:result-document href="{o:write_file(($reqrep_path, $idl_file))}">
                            <xsl:value-of select="cdit:get_server_interface_idl($server_interface, $middleware)" />
                        </xsl:result-document>
                    </xsl:if>
                </xsl:for-each>

                <!-- Generate the middleware CMakeFile -->
                <xsl:result-document href="{o:write_file((('ports', 'requestreply', $middleware), cmake:cmake_file()))}">
                    <xsl:value-of select="cdit:get_middleware_cmake($server_interfaces)" />
                </xsl:result-document>
            </xsl:if>
        </xsl:for-each>

        <!-- Generate the Enum headers -->
        <xsl:for-each select="$enums">
            <xsl:result-document href="{o:write_file(cdit:get_aggregate_h_path('Base', .))}">
                <xsl:value-of select="cdit:get_enum_h(.)" />
            </xsl:result-document>
        </xsl:for-each>

        <xsl:for-each select="$component_impls">
            <xsl:variable name="component_impl" select="." />
            <xsl:variable name="component_def" select="graphml:get_definition($component_impl)" />
            <xsl:value-of select="o:message(('Generating Component:', o:wrap_quote(cdit:get_qualified_type($component_def))))" />
            
            <xsl:variable name="label" select="lower-case(graphml:get_label($component_def))" />
            <xsl:variable name="component_path" select="cdit:get_component_path($component_def)" />

            <!-- Generate Interface Header -->
            <xsl:result-document href="{o:write_file(($component_path, concat($label, 'int.h')))}">
                <xsl:value-of select="cdit:get_component_int_h($component_def)" />
            </xsl:result-document>

            <!-- Generate Interface CPP -->
            <xsl:result-document href="{o:write_file(($component_path, concat($label, 'int.cpp')))}">
                <xsl:value-of select="cdit:get_component_int_cpp($component_def)" />
            </xsl:result-document>
            
            <!-- Generate Implementation Header -->
            <xsl:result-document href="{o:write_file(($component_path, concat($label, 'impl.h')))}">
                <xsl:value-of select="cdit:get_component_impl_h($component_impl)" />
            </xsl:result-document>

            <!-- Generate Implementation CPP -->
            <xsl:result-document href="{o:write_file(($component_path, concat($label, 'impl.cpp')))}">
                <xsl:value-of select="cdit:get_component_impl_cpp($component_impl)" />
            </xsl:result-document>

            <!-- Generate Component Export -->
            <xsl:result-document href="{o:write_file(($component_path, 'libcomponentexport.cpp'))}">
                <xsl:value-of select="cdit:get_component_export($component_impl)" />
            </xsl:result-document>

            <!-- Generate Component CMake File -->
            <xsl:result-document href="{o:write_file(($component_path, cmake:cmake_file()))}">
                <xsl:value-of select="cdit:get_class_cmake($component_impl)" />
            </xsl:result-document>
        </xsl:for-each>

        <xsl:for-each select="$classes">
            <xsl:variable name="class" select="." />
            <xsl:value-of select="o:message(('Generating Class:', o:wrap_quote(cdit:get_qualified_type($class))))" />

            <xsl:variable name="label" select="lower-case(graphml:get_label($class))" />
            <xsl:variable name="class_path" select="cdit:get_class_path($class)" />

            <!-- Generate Class Header -->
            <xsl:result-document href="{o:write_file(($class_path, concat($label, '.h')))}">
                <xsl:value-of select="cdit:get_class_h($class)" />
            </xsl:result-document>

            <!-- Generate Class CPP -->
            <xsl:result-document href="{o:write_file(($class_path, concat($label, '.cpp')))}">
                <xsl:value-of select="cdit:get_class_cpp($class)" />
            </xsl:result-document>

            <!-- Generate Class CMake File -->
            <xsl:result-document href="{o:write_file(($class_path, cmake:cmake_file()))}">
                <xsl:value-of select="cdit:get_class_cmake($class)" />
            </xsl:result-document>
        </xsl:for-each>

        <!-- Generate the Component CMakeFile -->
        <xsl:result-document href="{o:write_file(('components', cmake:cmake_file()))}">
            <xsl:value-of select="cdit:get_components_cmake($component_impls)" />
        </xsl:result-document>

        <!-- Generate the Classes CMakeFile -->
        <xsl:result-document href="{o:write_file(('classes', cmake:cmake_file()))}">
            <xsl:value-of select="cdit:get_components_cmake($classes)" />
        </xsl:result-document>

        <!-- Generate the datatypes CMakeFile -->
        <xsl:result-document href="{o:write_file(('datatypes', cmake:cmake_file()))}">
            <xsl:value-of select="cdit:get_directories_cmake($middlewares[cdit:build_shared_library(.)])" />
        </xsl:result-document>

        <!-- Generate the pubsub CMakeFile -->
        <xsl:result-document href="{o:write_file(('ports', 'pubsub', cmake:cmake_file()))}">
            <xsl:value-of select="cdit:get_directories_cmake($middlewares[cdit:build_module_library(.) and cdit:middleware_implements_pubsub(.)])" />
        </xsl:result-document>

        <!-- Generate the requestreply CMakeFile -->
        <xsl:result-document href="{o:write_file(('ports', 'requestreply', cmake:cmake_file()))}">
            <xsl:value-of select="cdit:get_directories_cmake($middlewares[cdit:build_module_library(.) and cdit:middleware_implements_reqrep(.)])" />
        </xsl:result-document>

        <!-- Generate the ports CMakeFile -->
        <xsl:result-document href="{o:write_file(('ports', cmake:cmake_file()))}">
            <xsl:value-of select="cdit:get_directories_cmake(('pubsub', 'requestreply'))" />
        </xsl:result-document>

        <!-- Generate the top level cmake file -->
        <xsl:result-document href="{o:write_file(cmake:cmake_file())}">
            <xsl:value-of select="cmake:get_top_cmakelists()" />
        </xsl:result-document>
    </xsl:template>
</xsl:stylesheet>
