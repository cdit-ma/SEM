<!-- Functions for reading/interpretting graphml xml -->
<xsl:stylesheet version="2.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:xs="http://www.w3.org/2001/XMLSchema"
    xmlns:gml="http://graphml.graphdrawing.org/xmlns"
    xmlns:cdit="http://github.com/cdit-ma/cdit"
    xmlns:o="http://github.com/cdit-ma/o"
    xmlns:graphml="http://github.com/cdit-ma/graphml"
    xmlns:cpp="http://github.com/cdit-ma/cpp"
    >

    <!--
        Gets the id attribute from the element
    -->
    <xsl:function name="o:get_convert_h">
        <xsl:param name="aggregate" />
        <xsl:param name="middleware" as="xs:string" />

        <xsl:variable name="aggregate_namespace" select="graphml:get_data_value($aggregate, 'namespace')" />
        <xsl:variable name="aggregate_namespace_lc" select="lower-case($aggregate_namespace)" />
        <xsl:variable name="aggregate_label" select="graphml:get_data_value($aggregate, 'label')" />
        <xsl:variable name="aggregate_label_lc" select="lower-case($aggregate_label)" />
        
        <xsl:variable name="define_guard_name" select="upper-case(concat(upper-case($middleware), '_', $aggregate_label, '_CONVERT'))" />

        <xsl:value-of select="cpp:define_guard_start($define_guard_name)" />
        <xsl:value-of select="cpp:include_library_header('iostream')" />

        <!-- Include the base message type -->
        <xsl:value-of select="cpp:comment('Include the base type', 0)" />

        <xsl:variable name="path_list" select="('','base', $aggregate_namespace_lc, $aggregate_label_lc, '','',concat($aggregate_label_lc,'.h'),'')" as="xs:string*"/>


        <xsl:value-of select="cpp:include_local_header(o:join_list($path_list, '/'))" />

        <!-- End Define Guard -->
        <xsl:value-of select="cpp:define_guard_end($define_guard_name)" />
    </xsl:function>
</xsl:stylesheet>