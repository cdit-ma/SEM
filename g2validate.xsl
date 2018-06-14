<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="2.0" 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:xs="http://www.w3.org/2001/XMLSchema"
    xmlns:gml="http://graphml.graphdrawing.org/xmlns"
    xmlns:cdit="http://github.com/cdit-ma/re_gen/cdit"
    xmlns:graphml="http://github.com/cdit-ma/re_gen/graphml"
    xmlns:xmlo="http://github.com/cdit-ma/re_gen/xmlo"
    xmlns:o="http://github.com/cdit-ma/re_gen/o"
    exclude-result-prefixes="gml">
		
    <!-- Load in Functions -->
    <xsl:import href="general_functions.xsl"/>
    <xsl:import href="xml_functions.xsl"/>
    <xsl:import href="graphml_functions.xsl"/>
    <xsl:import href="cdit_functions.xsl"/>
    <xsl:import href="cpp_functions.xsl"/>
    <xsl:import href="cmake_functions.xsl"/>
    <xsl:import href="validate_functions.xsl"/>

    <xsl:output method="text" omit-xml-declaration="yes" indent="yes" standalone="no" />
    <xsl:param name="debug_mode" as="xs:boolean" select="false()" />
    <xsl:param name="write_file" as="xs:boolean" select="true()" />

    <xsl:template match="/*">
        <xsl:variable name="model" select="graphml:get_model(.)" />
        <xsl:variable name="results">
            <xsl:value-of select="cdit:aggregate_tests($model)" />
            <xsl:value-of select="cdit:component_tests($model)" />
            <xsl:value-of select="cdit:deployment_tests($model)" />
        </xsl:variable>

        <xsl:variable name="xml_result" select="xmlo:wrap_tag('validation_report', '', $results, 0)" />

        <xsl:choose>
            <xsl:when test="$write_file">
                <xsl:result-document href="{o:write_file('validation_report.xml')}">
                    <xsl:value-of select="$xml_result" />
                </xsl:result-document>
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="$xml_result" />
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>
</xsl:stylesheet>