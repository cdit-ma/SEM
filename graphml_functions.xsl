<!-- Functions for reading/interpretting graphml xml -->
<xsl:stylesheet version="2.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:xs="http://www.w3.org/2001/XMLSchema"
    xmlns:gml="http://graphml.graphdrawing.org/xmlns"
    xmlns:graphml="http://github.com/cdit-ma"
    >

    <!-- Use keys to speed up parsing -->
    <xsl:key name="get_key_by_name" match="gml:key" use="@attr.name"/>
    <xsl:key name="get_key_by_id" match="gml:key" use="@id"/>
    <xsl:key name="get_node_by_id" match="gml:node" use="@id"/>
    
    <xsl:key name="get_data_by_key_id" match="gml:data" use="@key"/>

    <xsl:key name="get_edge_by_id" match="gml:edge" use="@id"/>
    <xsl:key name="get_edge_by_source_id" match="gml:edge" use="@source"/>
    <xsl:key name="get_edge_by_target_id" match="gml:edge" use="@target"/>


    <!--
        Gets the id attribute from the element
    -->
    <xsl:function name="graphml:get_id" as="xs:string">
        <xsl:param name="root" as="element()"/>
        <xsl:value-of select="$root/@id" />
    </xsl:function>

    <!--
        Gets the top most
    -->
    <xsl:function name="graphml:get_graphml" as="xs:element()">
        <xsl:param name="root" as="element()"/>
        <xsl:value-of select="$root/*" />
    </xsl:function>


    <xsl:function name="cdit:get_node_by_id">
        <xsl:param name="root" />
        <xsl:param name="id"  as="xs:string"/>

         <xsl:for-each select="$root">
            <xsl:sequence select="key('get_node_by_id', $id)" />
        </xsl:for-each>
    </xsl:function>
</xsl:stylesheet>