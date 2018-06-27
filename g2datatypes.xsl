<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="2.0" 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:xs="http://www.w3.org/2001/XMLSchema"
    xmlns:gml="http://graphml.graphdrawing.org/xmlns"

    xmlns:o="http://github.com/cdit-ma/re_gen/o"
    xmlns:cpp="http://github.com/cdit-ma/re_gen/cpp"
    xmlns:cmake="http://github.com/cdit-ma/re_gen/cmake"
    xmlns:cdit="http://github.com/cdit-ma/re_gen/cdit"
    xmlns:graphml="http://github.com/cdit-ma/re_gen/graphml"
    exclude-result-prefixes="gml"
    >
    <xsl:output method="text" omit-xml-declaration="yes" indent="yes" standalone="no" />

    <!-- Load in Functions -->
    
    <xsl:import href="component_functions.xsl"/>

    <xsl:import href="general_functions.xsl"/>
    <xsl:import href="cpp_functions.xsl"/>
    <xsl:import href="cmake_functions.xsl"/>
    <xsl:import href="graphml_functions.xsl"/>
    <xsl:import href="cdit_functions.xsl"/> 
    <xsl:import href="proto_functions.xsl"/>
    <xsl:import href="idl_functions.xsl"/>
    <xsl:import href="cdit_cmake_functions.xsl"/>
    <xsl:import href="datatype_functions.xsl"/>


    <!-- Middleware Input Parameter-->
    <xsl:param name="generate_all" as="xs:boolean" select="false()" />
    <xsl:param name="debug_mode" as="xs:boolean" select="true()" />

    <xsl:template match="/*">
        <xsl:variable name="model" select="graphml:get_model(.)" />
        <xsl:variable name="middlewares" select="cdit:get_utilized_middlewares($model, $generate_all)" as="xs:string*" />

        <xsl:variable name="output_path" select="'ports'" />


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

                        <xsl:if test="cdit:middleware_requires_proto_file($middleware)">
                            <xsl:variable name="proto_file" select="concat($file_label, '.proto')" />
                            <xsl:result-document href="{o:write_file(($aggregate_path, $proto_file))}">
                                <xsl:value-of select="cdit:get_proto_file($aggregate)" />
                            </xsl:result-document>
                        </xsl:if>

                        <xsl:if test="cdit:middleware_requires_idl_file($middleware)">
                            <xsl:variable name="idl_file" select="concat($file_label, '.idl')" />
                            <xsl:result-document href="{o:write_file(($aggregate_path, $idl_file))}">
                                <xsl:value-of select="cdit:get_idl_datatype($aggregate, $middleware)" />
                            </xsl:result-document>
                        </xsl:if>

                        <xsl:result-document href="{o:write_file(($aggregate_path, cmake:cmake_file()))}">
                            <xsl:value-of select="cdit:get_datatype_cmake($aggregate, $middleware)" />
                        </xsl:result-document>
                    </xsl:if>

                    <!-- Generate the Base representation for the Aggregate -->
                    <xsl:if test="$middleware = 'base'">
                        <xsl:variable name="base_h" select="concat($file_label, '.h')" />
                        <xsl:variable name="base_cpp" select="concat($file_label, '.cpp')" />

                        <xsl:result-document href="{o:write_file(($aggregate_path, $base_h))}">
                            <xsl:value-of select="cdit:get_aggregate_base_h($aggregate)" />
                        </xsl:result-document>

                        <xsl:result-document href="{o:write_file(($aggregate_path, $base_cpp))}">
                            <xsl:value-of select="cdit:get_aggregate_base_cpp($aggregate)" />
                        </xsl:result-document>

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

            <!-- Generate the Pub/Sub Ports -->
            <xsl:if test="cdit:build_module_library($middleware)">
                <xsl:for-each select="$pubsub_aggregates">
                    <xsl:variable name="aggregate" select="." />
                    <xsl:variable name="qualified_type" select="cdit:get_aggregate_qualified_type($aggregate, $middleware)" />
                    <xsl:variable name="aggregate_path" select="cdit:get_pubsub_path($middleware, $aggregate)" />
                    <xsl:value-of select="o:message(('Generating Pub/Sub Port:', o:wrap_quote($qualified_type)) )" />
                
                    <!-- Generate the Module(dll) port export class-->
                    <xsl:result-document href="{o:write_file(($aggregate_path, 'libportexport.cpp'))}">
                        <xsl:value-of select="cdit:get_port_pubsub_export($aggregate, $middleware)" />
                    </xsl:result-document>

                    <xsl:result-document href="{o:write_file(($aggregate_path, cmake:cmake_file()))}">
                        <xsl:value-of select="cdit:get_pubsub_cmake($aggregate, $middleware)" />
                    </xsl:result-document>
                </xsl:for-each>

                <!-- Generate the middleware CMakeFile -->
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

        <!-- Generate the middleware CMakeFile -->
        <xsl:result-document href="{o:write_file((('datatypes'), cmake:cmake_file()))}">
            <xsl:variable name="datatype_middlewares" as="xs:string*">
                <xsl:for-each select="$middlewares">
                    <xsl:if test="cdit:build_shared_library(.)">
                        <xsl:sequence select="." />
                    </xsl:if>
                </xsl:for-each>
            </xsl:variable>
            <xsl:value-of select="cdit:get_directories_cmake($datatype_middlewares)" />
        </xsl:result-document>


        <!-- Generate the middleware CMakeFile -->
        <xsl:result-document href="{o:write_file((('ports', 'pubsub'), cmake:cmake_file()))}">
            <xsl:variable name="pubsub_middlewares" as="xs:string*">
                <xsl:for-each select="$middlewares">
                    <xsl:if test="cdit:build_module_library(.) and cdit:middleware_implements_pubsub(.)">
                        <xsl:sequence select="." />
                    </xsl:if>
                </xsl:for-each>
            </xsl:variable>
            <xsl:value-of select="cdit:get_directories_cmake($pubsub_middlewares)" />
        </xsl:result-document>

        <xsl:result-document href="{o:write_file((('ports', 'requestreply'), cmake:cmake_file()))}">
            <xsl:variable name="reqrep_middlewares" as="xs:string*">
                <xsl:for-each select="$middlewares">
                    <xsl:if test="cdit:build_module_library(.)  and cdit:middleware_implements_reqrep(.)">
                        <xsl:sequence select="." />
                    </xsl:if>
                </xsl:for-each>
            </xsl:variable>
            <xsl:value-of select="cdit:get_directories_cmake($reqrep_middlewares)" />
        </xsl:result-document>

        <xsl:value-of select="o:message(('Generating Enums'))" />

        <!-- Generate the Enum headers -->
        <xsl:for-each select="graphml:get_descendant_nodes_of_kind($model, 'Enum')">
            <xsl:variable name="enum_h" select="cdit:get_aggregate_h_path('Base', .)" />
            
            <xsl:result-document href="{o:write_file($enum_h)}">
                <xsl:value-of select="cdit:get_enum_h(.)" />
            </xsl:result-document>
        </xsl:for-each>

        <!-- Generate the datatypes CMakeFile -->
        <xsl:result-document href="{o:write_file(($output_path, cmake:cmake_file()))}">
            <xsl:value-of select="cdit:get_directories_cmake(('pubsub', 'requestreply'))" />
        </xsl:result-document>

        <!-- Generate the top level cmake file -->
        <xsl:result-document href="{o:write_file(cmake:cmake_file())}">
            <xsl:value-of select="cmake:get_top_cmakelists()" />
        </xsl:result-document>
    </xsl:template>
</xsl:stylesheet>
