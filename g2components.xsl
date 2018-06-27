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
    <xsl:param name="generate_all" as="xs:boolean" select="false()" />
    <xsl:param name="debug_mode" as="xs:boolean" select="true()" />
    

    <xsl:template match="/*">
        <xsl:variable name="model" select="graphml:get_model(.)" />
        
        <!-- Parse the components parameter to produce a list of labels -->
        <xsl:variable name="behaviour_definitions" select="graphml:get_descendant_nodes_of_kind($model, 'BehaviourDefinitions')" />
        <xsl:variable name="classes" select="graphml:get_descendant_nodes_of_kind($behaviour_definitions, 'Class')" />
        <xsl:variable name="parsed_components" select="cdit:parse_components($components)" as="xs:string*" />
        <xsl:variable name="output_path" select="'components'" />
        
        <!-- Construct a list of ComponentImpl Objects to code-gen -->
        <xsl:variable name="component_impls" as="element()*">
            <xsl:choose>
                <xsl:when test="$generate_all = true() or $preview = true()">
                    <xsl:for-each select="graphml:get_nodes_of_kind($model, 'ComponentImpl')">
                        <xsl:variable name="label" select="lower-case(graphml:get_label(.))" />
                        <xsl:if test="$generate_all or ($label = $parsed_components)">
                            <xsl:sequence select="." />
                        </xsl:if>
                    </xsl:for-each>
                </xsl:when>
                <xsl:otherwise>
                    <xsl:for-each select="graphml:get_deployed_component_instances($model)">
                        <xsl:sequence select="graphml:get_impl(.)" />
                    </xsl:for-each>
                </xsl:otherwise>
            </xsl:choose>
        </xsl:variable>

        <xsl:variable name="component_impls_to_generate" select="o:remove_duplicates($component_impls)" />
        

        <xsl:for-each-group select="$component_impls_to_generate" group-by=".">
            <xsl:variable name="component_impl" select="." />
            <xsl:variable name="component_def" select="graphml:get_definition($component_impl)" />
            
            <xsl:variable name="label" select="lower-case(graphml:get_label($component_def))" />
            <xsl:variable name="component_path" select="cdit:get_component_path($component_def)" />
            <xsl:variable name="qualified_type" select="cdit:get_qualified_type($component_def)" />
            
            <xsl:value-of select="o:message(('Generating Component:', o:wrap_quote($qualified_type)))" />
                
            <xsl:if test="not($preview)">
                <!-- Only Generate the Interfaces and CMake files when we aren't in preview mode -->
                <xsl:variable name="int_h" select="concat($label, 'int.h')" />
                <xsl:variable name="int_cpp" select="concat($label, 'int.cpp')" />

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

            <xsl:variable name="impl_h" select="concat($label, 'impl.h')" />
            <xsl:variable name="impl_cpp" select="concat($label, 'impl.cpp')" />

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
            
            <xsl:variable name="label" select="lower-case(graphml:get_label($class))" />
            <xsl:variable name="path" select="cdit:get_class_path($class)" />
            <xsl:variable name="type" select="cdit:get_qualified_type($class)" />

            <xsl:value-of select="o:message(('Generating Class:', o:wrap_quote($type)))" />
            
            <xsl:if test="not($preview)">
                <!-- Only Generate the Interfaces and CMake files when we aren't in preview mode -->
                <xsl:variable name="class_h" select="concat($label, '.h')" />
                <xsl:variable name="class_cpp" select="concat($label, '.cpp')" />

                <xsl:result-document href="{o:write_file(($path, $class_h))}">
                    <xsl:value-of select="cdit:get_class_h($class)" />
                </xsl:result-document>

                <xsl:result-document href="{o:write_file(($path, $class_cpp))}">
                    <xsl:value-of select="cdit:get_class_cpp($class)" />
                </xsl:result-document>

                <xsl:result-document href="{o:write_file(($path, cmake:cmake_file()))}">
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
                <xsl:value-of select="cmake:get_top_cmakelists()" />
            </xsl:result-document>
        </xsl:if>
    </xsl:template>
</xsl:stylesheet>
