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
    <xsl:param name="sparse" as="xs:boolean" select="true()" />
    <xsl:param name="debug_mode" as="xs:boolean" select="true()" />

    <xsl:template match="/*">
        <xsl:variable name="model" select="graphml:get_model(.)" />

        <xsl:variable name="parsed_middlewares" select="cdit:get_deployed_middlewares($model, $sparse)" as="xs:string*" />

        <xsl:variable name="aggregates" select="graphml:get_descendant_nodes_of_kind($model, 'Aggregate')" />
        
        <xsl:variable name="output_path" select="'datatypes'" />

        <!-- Itterate through all middlewares-->
        <xsl:for-each select="$parsed_middlewares">
            <xsl:variable name="middleware" select="lower-case(.)" />
            <xsl:variable name="middleware_path" select="o:join_paths(($output_path, $middleware))" />
            <xsl:value-of select="o:message(('Generating Middleware:', o:wrap_quote($middleware)))" />

            <xsl:variable name="required_middleware_aggregates" select="cdit:get_required_aggregates_for_middleware($model, $middleware)" />
            
            <xsl:for-each select="$required_middleware_aggregates">
                <xsl:variable name="aggregate" select="." />

                <xsl:variable name="aggregate_namespace" select="graphml:get_namespace($aggregate)" />
                <xsl:variable name="aggregate_label" select="graphml:get_label($aggregate)" />
                <xsl:variable name="file_label" select="cdit:get_aggregate_file_prefix($aggregate, $middleware)" />
                

                <xsl:variable name="aggregate_path" select="o:join_paths(($middleware_path, cdit:get_aggregates_path($aggregate)))" />
                
                <xsl:value-of select="o:message(('Generating Datatype:', o:wrap_quote(cpp:combine_namespaces(($aggregate_namespace, $aggregate_label)))))" />

                <!-- Generate the Shared Library translate/middleware classes -->
                <xsl:if test="cdit:build_shared_library($middleware)">
                    <xsl:result-document href="{o:write_file(($aggregate_path, 'translate.h'))}">
                        <xsl:value-of select="cdit:get_translate_h($aggregate, $middleware)" />
                    </xsl:result-document>
                    
                    <xsl:result-document href="{o:write_file(($aggregate_path, 'translate.cpp'))}">
                        <xsl:value-of select="cdit:get_translate_cpp($aggregate, $middleware)" />
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
                            <xsl:value-of select="cdit:get_idl_file($aggregate)" />
                        </xsl:result-document>
                    </xsl:if>
                </xsl:if>

                <!-- Generate the Module(dll) port export class-->
                <xsl:if test="cdit:build_module_library($middleware)">
                    <xsl:result-document href="{o:write_file(($aggregate_path, 'libportexport.cpp'))}">
                        <xsl:value-of select="cdit:get_port_export($aggregate, $middleware)" />
                    </xsl:result-document>
                </xsl:if>

                <!-- Generate the CMake file for this middleware's aggregate implementation -->
                <xsl:if test="cdit:build_module_library($middleware) or cdit:build_shared_library($middleware)">
                    <xsl:result-document href="{o:write_file(($aggregate_path, cmake:cmake_file()))}">
                        <xsl:value-of select="cdit:get_translate_cmake($aggregate, $middleware)" />
                    </xsl:result-document>
                </xsl:if>

                <!-- Generate the Base representation for the aggregate -->
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
            <xsl:result-document href="{o:write_file(($middleware_path, cmake:cmake_file()))}">
                <xsl:value-of select="cdit:get_middleware_cmake($required_middleware_aggregates)" />
            </xsl:result-document>
        </xsl:for-each>

        <xsl:value-of select="o:message(('Generating Enums'))" />

        <!-- Generate the Enum headers -->
        <xsl:for-each select="graphml:get_descendant_nodes_of_kind($model, 'Enum')">

            <xsl:variable name="enum" select="." />
            <xsl:variable name="enum_path" select="o:join_paths(($output_path, 'base', 'enums', cdit:get_aggregates_path($enum)))" />
            <xsl:variable name="enum_h" select="lower-case(concat(graphml:get_label($enum), '.h'))" />

            <xsl:result-document href="{o:write_file(($enum_path, $enum_h))}">
                <xsl:value-of select="cdit:get_enum_h($enum)" />
            </xsl:result-document>
        </xsl:for-each>

        <!-- Generate the datatypes CMakeFile -->
        <xsl:result-document href="{o:write_file(($output_path, cmake:cmake_file()))}">
            <xsl:value-of select="cdit:get_datatypes_cmake($parsed_middlewares)" />
        </xsl:result-document>

        <!-- Generate the top level cmake file -->
        <xsl:result-document href="{o:write_file(cmake:cmake_file())}">
            <xsl:value-of select="cdit:get_top_cmake()" />
        </xsl:result-document>
    </xsl:template>
</xsl:stylesheet>
