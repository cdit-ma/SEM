<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="2.0" 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:xs="http://www.w3.org/2001/XMLSchema"
    xmlns:gml="http://graphml.graphdrawing.org/xmlns"
    xmlns:exsl="http://exslt.org/common"
    xmlns:xalan="http://xml.apache.org/xslt"	
    xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" 
    xmlns:cdit="http://github.com/cdit-ma/cdit"
    xmlns:o="http://github.com/cdit-ma/o"
    xmlns:graphml="http://github.com/cdit-ma/graphml"
    xmlns:cpp="http://github.com/cdit-ma/cpp"
    exclude-result-prefixes="gml exsl xalan">

    <!-- Load in Functions -->
    <xsl:import href="general_functions.xsl"/>
    <xsl:import href="cpp_functions.xsl"/>
    <xsl:import href="cmake_functions.xsl"/>
    <xsl:import href="graphml_functions.xsl"/>
    <xsl:import href="cdit_functions.xsl"/> 
    <xsl:import href="proto_functions.xsl"/>
    <xsl:import href="idl_functions.xsl"/>
    <xsl:import href="cdit_cmake_functions.xsl"/>
    
    <xsl:import href="datatype_convert.xsl"/>

    <!-- Middleware Input Parameter-->
    <xsl:param name="middlewares" as="xs:string" select="'Base'" />
    

    <xsl:output method="text" omit-xml-declaration="yes" indent="yes" standalone="no" />
	 
    <xsl:template match="/">
        <xsl:variable name="parsed_middlewares" select="cdit:parse_middlewares($middlewares)" as="xs:string*" />

        <xsl:for-each select="$parsed_middlewares">
            <xsl:message>MW:<xsl:value-of select="." /></xsl:message>
        </xsl:for-each>
        
        <xsl:for-each select="graphml:get_descendant_nodes_of_kind(., 'Aggregate')">
            <xsl:variable name="label" select="lower-case(graphml:get_label(.))" />

            <xsl:if test="0 = 1">
                <xsl:variable name="convert_h" select="concat($label, '_convert.h')" />
                <xsl:variable name="convert_cpp" select="concat($label, '_convert.cpp')" />
                <xsl:variable name="convert_cmake" select="concat($label, '_CMakeLists.txt')" />
                <xsl:variable name="convert_proto" select="concat($label, '.proto')" />
                <xsl:variable name="convert_idl" select="concat($label, '.idl')" />
            
                <xsl:result-document href="{o:write_file($convert_h)}">
                    <xsl:value-of select="cdit:get_convert_h(., 'proto')" />
                </xsl:result-document>
                
                <xsl:result-document href="{o:write_file($convert_cpp)}">
                    <xsl:value-of select="cdit:get_convert_cpp(., 'proto')" />
                </xsl:result-document>

                <xsl:result-document href="{o:write_file($convert_cmake)}">
                    <xsl:value-of select="cdit:get_convert_cmake(., 'proto')" />
                </xsl:result-document>

                <xsl:result-document href="{o:write_file($convert_proto)}">
                    <xsl:value-of select="cdit:get_proto_file(.)" />
                </xsl:result-document>

                <xsl:result-document href="{o:write_file($convert_idl)}">
                    <xsl:value-of select="cdit:get_idl_file(.)" />
                </xsl:result-document>
            </xsl:if>

            <xsl:variable name="base_h" select="concat('base_', $label, '.h')" />
            <xsl:variable name="base_cpp" select="concat('base_', $label, '.cpp')" />
            <xsl:variable name="base_cmake" select="concat('base_', $label, '_CMakeLists.txt')" />

             <xsl:result-document href="{o:write_file($base_h)}">
                <xsl:value-of select="cdit:get_aggregate_base_h(.)" />
            </xsl:result-document>
                

        </xsl:for-each>


        

    </xsl:template>
</xsl:stylesheet>
