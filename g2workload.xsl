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
    <xsl:param name="id" as="xs:string" select="''" />

    <xsl:variable name="debug_mode" as="xs:boolean" select="true()" />

    <xsl:template match="/*">
        <xsl:variable name="model" select="graphml:get_model(.)" />
        <xsl:variable name="node" select="graphml:get_node_by_id($model, $id)" />
        <xsl:variable name="kind" select="graphml:get_kind($node)" />
        <xsl:variable name="parent_node" select="graphml:get_parent_node($node)" />
        

        <xsl:choose>
            <xsl:when test="not($node)">
                <xsl:value-of select="cpp:comment(('Cannot find node with id:', o:wrap_quote($id)), 0)" />
            </xsl:when>
            <xsl:when test="$kind = 'Component' or $kind = 'ComponentInstance'">
                <xsl:variable name="impl" select="graphml:get_impl($node)" />
                <xsl:value-of select="cdit:get_component_impl_cpp($impl)" />
            </xsl:when>
            <xsl:when test="$kind = 'ComponentImpl'">
                <xsl:value-of select="cdit:get_component_impl_cpp($node)" />
            </xsl:when>
            <xsl:when test="$kind = 'Class'">
                <xsl:value-of select="cdit:get_class_cpp($node)" />
            </xsl:when>
            <xsl:when test="$kind = 'PeriodicPort' or
                            $kind = 'SubscriberPortImpl' or
                            $kind = 'Function' or
                            $kind = 'ReplierPortImpl'">
                <xsl:value-of select="cdit:define_workload_function($node, '')" />
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="cdit:generate_workflow_code($node, $parent_node, 0)" />
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>
</xsl:stylesheet>
