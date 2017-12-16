<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="2.0" 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:xs="http://www.w3.org/2001/XMLSchema"
    xmlns:gml="http://graphml.graphdrawing.org/xmlns"
    xmlns:exsl="http://exslt.org/common"
    xmlns:xalan="http://xml.apache.org/xslt"	
    xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" 
    xmlns:cdit="http://github.com/cdit-ma/re_gen/cdit"
    xmlns:o="http://github.com/cdit-ma/re_gen/o"
    xmlns:graphml="http://github.com/cdit-ma/re_gen/graphml"
    xmlns:cpp="http://github.com/cdit-ma/re_gen/cpp"
    xmlns:cmake="http://github.com/cdit-ma/re_gen/cmake"
    exclude-result-prefixes="gml exsl xalan">
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
	 
    <xsl:template match="/*">
        <xsl:variable name="model" select="graphml:get_model(.)" />
        
        <!-- Parse the components parameter to produce a list of labels -->
        <xsl:variable name="parsed_components" select="cdit:parse_components($components)" />
        
        <xsl:variable name="output_path" select="'components'" />

        

        <!-- Construct a list of ComponentImpl Objects to code-gen -->
        <xsl:variable name="component_impls_to_generate" as="element()*">
            <xsl:for-each select="graphml:get_descendant_nodes_of_kind($model, 'ComponentImpl')">
                <xsl:variable name="component_label" select="graphml:get_label(.)" />
                <xsl:if test="count($parsed_components) = 0 or index-of($parsed_components, lower-case($component_label))">
                    <xsl:sequence select="." />
                </xsl:if>
            </xsl:for-each>
        </xsl:variable>

        <xsl:for-each select="$component_impls_to_generate">
            <xsl:variable name="component_impl" select="." />
            <xsl:variable name="component_def" select="graphml:get_definition($component_impl)" />
            
            <xsl:variable name="component_def_label" select="graphml:get_label($component_def)" />
            <xsl:variable name="component_impl_label" select="graphml:get_label($component_impl)" />
            <xsl:variable name="component_int_prefix" select="lower-case($component_def_label)" />
            <xsl:variable name="component_impl_prefix" select="lower-case($component_impl_label)" />
            
            <xsl:variable name="component_path" select="o:join_paths(($output_path, $component_int_prefix))" />

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
                    <xsl:value-of select="cdit:get_component_cmake($component_impl)" />
                </xsl:result-document>
            </xsl:if>

            <xsl:variable name="impl_h" select="concat($component_impl_prefix, 'impl.h')" />
            <xsl:variable name="impl_cpp" select="concat($component_impl_prefix, 'impl.cpp')" />

            <!-- Always Generate the Impl -->
            <xsl:result-document href="{o:write_file(($component_path, $impl_h))}">
                <xsl:value-of select="cdit:get_component_impl_h($component_impl)" />
            </xsl:result-document>

            <xsl:result-document href="{o:write_file(($component_path, $impl_cpp))}">
                <xsl:value-of select="cdit:get_component_impl_cpp($component_impl)" />
            </xsl:result-document>
        </xsl:for-each>

        <!-- Generate the component CMakeFile if we aren't in preview mode -->
        <xsl:if test="not($preview)">
            <xsl:result-document href="{o:write_file(($output_path, cmake:cmake_file()))}">
                <xsl:value-of select="cdit:get_components_cmake($component_impls_to_generate)" />
            </xsl:result-document>
        </xsl:if>

        <!-- Generate the top level cmake file -->
        <xsl:if test="not($preview)">
            <xsl:result-document href="{o:write_file((cmake:cmake_file()))}">
                <xsl:value-of select="cdit:get_top_cmake()" />
            </xsl:result-document>
        </xsl:if>
    </xsl:template>
</xsl:stylesheet>
