<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="2.0" 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:gml="http://graphml.graphdrawing.org/xmlns"
    xmlns:exsl="http://exslt.org/common"
    xmlns:xalan="http://xml.apache.org/xslt"	
    xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" 
    xmlns:cdit="http://whatever"
    xmlns:o="http://whatever"
    exclude-result-prefixes="gml exsl xalan">

    <!-- Load in Functions -->
    <xsl:import href="functions.xsl"/>
    
    <xsl:output method="text" omit-xml-declaration="yes" indent="yes" standalone="no" />
	
    <!-- Get all of the Aggregates -->
    <xsl:variable name="aggregates" as="element()*" select="cdit:get_entities_of_kind(., 'Aggregate')" />

    <xsl:template match="/">
        <xsl:for-each select="$aggregates">
            <!-- Get all the children Members, VectorInstance, AggregateInstance -->
            <xsl:variable name="members" as="element()*" select="cdit:get_entities_of_kind(., 'Member')" />
            <xsl:variable name="vectors" as="element()*" select="cdit:get_entities_of_kind(., 'VectorInstance')" />
            <xsl:variable name="aggregates" as="element()*" select="cdit:get_entities_of_kind(., 'AggregateInstance')" />
            
            <!-- Get the label of the Aggregate -->
            <xsl:variable name="aggregate_label" select="cdit:get_key_value(., 'label')" />
            <xsl:variable name="aggregate_label_lc" select="lower-case($aggregate_label)" />

            <!-- Construct file paths -->
            <xsl:variable name="base_h" select="concat('datatypes/', $aggregate_label_lc, '/', $aggregate_label_lc, '.h')" />
            <xsl:variable name="base_cpp" select="concat('datatypes/', $aggregate_label_lc, '/', $aggregate_label_lc, '.cpp')" />
            <xsl:variable name="base_cmake" select="concat('datatypes/', $aggregate_label_lc, '/CMakeLists.txt')" />
            
            <!-- Write File: datatypes/{AGGREGATE_LABEL}/{AGGREGATE_LABEL}.h -->
            <xsl:result-document href="{o:xsl_wrap_file($base_h)}">
                <xsl:value-of select="o:get_base_data_type_h(., $members, $vectors, $aggregates)" />
            </xsl:result-document>

            <!-- Write File: datatypes/{AGGREGATE_LABEL}/{AGGREGATE_LABEL}.cpp -->
            <xsl:result-document href="{o:xsl_wrap_file($base_cpp)}">
                <xsl:value-of select="o:get_base_data_type_cpp(., $members, $vectors, $aggregates)" />
            </xsl:result-document>

            <!-- Write File: datatypes/{AGGREGATE_LABEL}/CMakeLists.txt -->
            <xsl:result-document href="{o:xsl_wrap_file($base_cmake)}">
                <xsl:value-of select="o:get_base_data_type_cmake(., $members, $vectors, $aggregates)" />
            </xsl:result-document>
        </xsl:for-each>
    </xsl:template>
</xsl:stylesheet>
