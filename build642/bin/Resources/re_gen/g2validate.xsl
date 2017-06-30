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
    <xsl:import href="validate_functions.xsl"/>

    <xsl:output method="text" omit-xml-declaration="yes" indent="yes" standalone="no" />

    <!-- Middleware Input Parameter-->
    <xsl:param name="middlewares"></xsl:param>
    
    <!-- Get all of the Aggregates -->
    
    <xsl:template match="/">
        <!-- THINGS TO CATCH -->
        <!--
            * Strings without double quotes in Functions in Behaviour
        -->
        
        <xsl:variable name="results">
            <xsl:value-of select="cdit:aggregate_tests(.)" />
            <xsl:value-of select="cdit:component_tests(.)" />
        </xsl:variable>

        <xsl:value-of select="o:xml_wrap('validation_report', '', $results, 0)" />
    </xsl:template>
</xsl:stylesheet>