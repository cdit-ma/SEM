<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="2.0" 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:gml="http://graphml.graphdrawing.org/xmlns"
    xmlns:exsl="http://exslt.org/common"
    xmlns:xalan="http://xml.apache.org/xslt"	
    xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" 
    xmlns:xs="http://www.w3.org/2001/XMLSchema"
    xmlns:cdit="http://github.com/cdit-ma"
    xmlns:o="http://github.com/cdit-ma"
    exclude-result-prefixes="gml exsl xalan">
		
    <!-- Load in Functions -->
    <xsl:import href="qos_functions.xsl"/>

    <xsl:output method="text" omit-xml-declaration="yes" indent="yes" standalone="no" />

    <!-- Get all of the Aggregates -->
    <xsl:variable name="qos_profiles" as="element()*" select="cdit:get_entities_of_kind(., 'DDS_QOSProfile')" />
    <xsl:template match="/">
        <!-- Construct file paths -->
        <xsl:variable name="qos_path" select="'qos/'" />

        <xsl:for-each select="$qos_profiles">
            <xsl:variable name="qos_profile" select="." />

            <!-- Get the label of the Aggregate -->
            <xsl:variable name="label" select="cdit:get_key_value(., 'label')" />
            <xsl:variable name="label_lc" select="lower-case($label)" />

            <!-- Define output file -->
            <xsl:variable name="qos_profile_xml" select="concat($qos_path, $label_lc, '.xml')" />

            <xsl:result-document href="{o:xsl_wrap_file($qos_profile_xml)}">
                <xsl:value-of select="cdit:get_qos_profile($qos_profile)" />
            </xsl:result-document>
        </xsl:for-each>
    </xsl:template>
</xsl:stylesheet>