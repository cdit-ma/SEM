<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="2.0" 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:xs="http://www.w3.org/2001/XMLSchema"
    xmlns:gml="http://graphml.graphdrawing.org/xmlns"
    xmlns:o="http://github.com/cdit-ma/re_gen/o"
    xmlns:cdit="http://github.com/cdit-ma/re_gen/cdit"
    xmlns:graphml="http://github.com/cdit-ma/re_gen/graphml"
    xmlns:cmake="http://github.com/cdit-ma/re_gen/cmake"
    exclude-result-prefixes="gml">

    <xsl:output method="text" omit-xml-declaration="yes" indent="yes" standalone="no" />
    

    <!-- Load in Functions -->
    <xsl:import href="general_functions.xsl"/>
    <xsl:import href="cpp_functions.xsl"/>
    <xsl:import href="cmake_functions.xsl"/>
    <xsl:import href="graphml_functions.xsl"/>
    <xsl:import href="cdit_functions.xsl"/> 
    <xsl:import href="cdit_cmake_functions.xsl"/>
    <xsl:import href="component_functions.xsl"/>

    <!-- Middleware Input Parameter-->
    <xsl:param name="components" as="xs:string" select="''" />
    <xsl:param name="preview" as="xs:boolean" select="false()" />
    <xsl:param name="sparse" as="xs:boolean" select="true()" />
    <xsl:param name="debug_mode" as="xs:boolean" select="true()" />
    

    <xsl:template match="/*">
        <xsl:variable name="model" select="graphml:get_model(.)" />
        
        <!-- Parse the components parameter to produce a list of labels -->
        <xsl:variable name="component_impls" select="graphml:get_descendant_nodes_of_kind($model, 'ComponentImpl')" />
        <xsl:variable name="behaviour_definitions" select="graphml:get_descendant_nodes_of_kind($model, 'BehaviourDefinitions')" />
        <xsl:variable name="classes" select="graphml:get_descendant_nodes_of_kind($behaviour_definitions, 'Class')" />
        <xsl:variable name="parsed_components" select="cdit:parse_components($components)" />
        <xsl:variable name="output_path" select="'components'" />

        <!-- Construct a list of ComponentImpl Objects to code-gen -->
        <xsl:variable name="component_impls_to_generate" as="element()*">
            <xsl:if test="$sparse">
                <xsl:variable name="component_instances" select="graphml:get_descendant_nodes_of_kind($model, 'ComponentInstance')" />
                <xsl:variable name="component_definitions" select="graphml:get_definitions($component_instances)" />

                <xsl:for-each select="$component_impls">
                    <xsl:if test="graphml:get_definition(.) = $component_definitions">
                        <xsl:sequence select="." />
                    </xsl:if>
                </xsl:for-each>
            </xsl:if>
            <xsl:if test="$sparse = false()">
                <xsl:for-each select="$component_impls">
                    <xsl:variable name="component_label" select="graphml:get_label(.)" />
                    <xsl:if test="count($parsed_components) = 0 or index-of($parsed_components, lower-case($component_label))">
                        <xsl:sequence select="." />
                    </xsl:if>
                </xsl:for-each>
            </xsl:if>
        </xsl:variable>

        <xsl:for-each-group select="$component_impls_to_generate" group-by=".">
            <xsl:variable name="component_impl" select="." />
            <xsl:variable name="component_def" select="graphml:get_definition($component_impl)" />


            <xsl:variable name="namespace" select="graphml:get_namespace($component_def)" />

            
            
            
            <!-- Get the labels of the definition and impl -->
            <xsl:variable name="component_def_label" select="graphml:get_label($component_def)" />
            <xsl:variable name="component_impl_label" select="graphml:get_label($component_impl)" />
            <xsl:variable name="component_int_prefix" select="lower-case($component_def_label)" />
            <xsl:variable name="component_impl_prefix" select="lower-case($component_impl_label)" />
            <xsl:variable name="component_path" select="o:join_paths(($output_path, cdit:get_aggregates_path($component_def)))" />

            <xsl:value-of select="o:message(('Generating Component:', o:wrap_quote($component_impl_label)))" />
                
            <xsl:if test="not($preview)">
                <!-- Only Generate the Interfaces and CMake files when we aren't in preview mode -->
                <xsl:variable name="int_h" select="concat($component_int_prefix, 'int.h')" />
                <xsl:variable name="int_cpp" select="concat($component_int_prefix, 'int.cpp')" />

                <xsl:result-document href="{o:write_file(($component_path, $int_h))}">
                    <xsl:value-of select="cdit:get_component_int_h($component_def)" />
                </xsl:result-document>

                <xsl:result-document href="{o:write_file(($component_path, $int_cpp))}">
                    <xsl:value-of select="cdit:get_component_int_cpp($component_def)" />
                </xsl:result-document>

                <xsl:result-document href="{o:write_file(($component_path, 'libcomponentexport.cpp'))}">
                    <xsl:value-of select="cdit:get_component_export($component_impl)" />
                </xsl:result-document>

                <xsl:result-document href="{o:write_file(($component_path, cmake:cmake_file()))}">
                    <xsl:value-of select="cdit:get_class_cmake($component_impl)" />
                </xsl:result-document>
            </xsl:if>

            <xsl:variable name="impl_h" select="concat($component_impl_prefix, 'impl.h')" />
            <xsl:variable name="impl_cpp" select="concat($component_impl_prefix, 'impl.cpp')" />

            <!-- The Impl is always generated -->
            <xsl:result-document href="{o:write_file(($component_path, $impl_h))}">
                <xsl:value-of select="cdit:get_component_impl_h($component_impl)" />
            </xsl:result-document>

            <xsl:result-document href="{o:write_file(($component_path, $impl_cpp))}">
                <xsl:value-of select="cdit:get_component_impl_cpp($component_impl)" />
            </xsl:result-document>
        </xsl:for-each-group>

        <xsl:for-each-group select="$classes" group-by=".">
            <xsl:variable name="class" select="." />
            
            <!-- Get the labels of the definition and impl -->
            <xsl:variable name="class_label" select="graphml:get_label($class)" />
            <xsl:variable name="class_prefix" select="lower-case($class_label)" />
            <xsl:variable name="class_path" select="o:join_paths(('classes', $class_prefix))" />

            <xsl:value-of select="o:message(('Generating Class:', o:wrap_quote($class_label)))" />
            
            <xsl:if test="not($preview)">
                <!-- Only Generate the Interfaces and CMake files when we aren't in preview mode -->
                <xsl:variable name="class_h" select="concat($class_prefix, '.h')" />
                <xsl:variable name="class_cpp" select="concat($class_prefix, '.cpp')" />

                <xsl:result-document href="{o:write_file(($class_path, $class_h))}">
                    <xsl:value-of select="cdit:get_class_h($class)" />
                </xsl:result-document>

                <xsl:result-document href="{o:write_file(($class_path, $class_cpp))}">
                    <xsl:value-of select="cdit:get_class_cpp($class)" />
                </xsl:result-document>

                <xsl:result-document href="{o:write_file(($class_path, cmake:cmake_file()))}">
                    <xsl:value-of select="cdit:get_class_cmake($class)" />
                </xsl:result-document>
            </xsl:if>
        </xsl:for-each-group>


        <xsl:if test="not($preview)">
            <!-- Generate the Component CMakeFile -->
            <xsl:result-document href="{o:write_file(($output_path, cmake:cmake_file()))}">
                <xsl:value-of select="cdit:get_components_cmake($component_impls_to_generate)" />
            </xsl:result-document>

            <!-- Generate the Component CMakeFile -->
            <xsl:result-document href="{o:write_file(('classes', cmake:cmake_file()))}">
                <xsl:value-of select="cdit:get_components_cmake($classes)" />
            </xsl:result-document>

            <!-- Generate the top level cmake file -->
            <xsl:result-document href="{o:write_file(cmake:cmake_file())}">
                <xsl:value-of select="cdit:get_top_cmake()" />
            </xsl:result-document>
        </xsl:if>
    </xsl:template>
</xsl:stylesheet>
