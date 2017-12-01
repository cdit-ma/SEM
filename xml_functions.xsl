<xsl:stylesheet version="2.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:xs="http://www.w3.org/2001/XMLSchema"
    xmlns:xml="http://github.com/cdit-ma"
    >
    <xsl:import href="general_functions.xsl"/>

    <!--
        Wraps a tagname to be used as a start tag
        <${tag_name} ${args}>
    -->
    <xsl:function name="xml:start_tag" as="xs:string">
        <xsl:param name="tag_name" as="xs:string"/>
        <xsl:param name="args" as="xs:string"/>

        <xsl:variable name="tag_value">
            <xsl:choose>
                <xsl:when test="$args != ''">
                    <xsl:value-of select="concat($tag_name, ' ', $args)" />
                </xsl:when>
                <xsl:otherwise>
                    <xsl:value-of select="$tag_name" />
                </xsl:otherwise>
            </xsl:choose>
        </xsl:variable>
        
        <xsl:value-of select="o:wrap_angle($tag_value)" />
    </xsl:function>

    <!--
        Wraps a tagname to be used as a start tag
        </${tag_name}>
    -->
    <xsl:function name="xml:end_tag" as="xs:string">
        <xsl:value-of select="o:wrap_angle(concat('/', $tag_name))" />
    </xsl:function>

    <!--
        Wraps an xml tag element
    -->
    <xsl:function name="xml:wrap_tag" as="xs:string">
        <xsl:param name="key_name"/>
        <xsl:param name="attribute"/>
        <xsl:param name="value"/>
        <xsl:param name="tab"/>

        <xsl:variable name="flat_val" select="string-join($value, '')" />

        <xsl:value-of select="concat(o:t($tab), xml:start_tag($key_name, $attribute))" />

        <xsl:if test="contains($flat_val, o:nl(1))">
            <xsl:value-of select="o:nl(1)" />
        </xsl:if>
        <xsl:value-of select="concat($flat_val, xml:end_tag($key_name), o:nl(1))" />
    </xsl:function>
</xsl:stylesheet>