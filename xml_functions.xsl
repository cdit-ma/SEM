<xsl:stylesheet version="2.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:xs="http://www.w3.org/2001/XMLSchema"
    xmlns:o="http://github.com/cdit-ma/re_gen/o"
    xmlns:xmlo="http://github.com/cdit-ma/re_gen/xmlo"
    >
    <!--
        Wraps a tagname to be used as a start tag
        <${tag_name} ${args}>
    -->
    <xsl:function name="xmlo:start_tag" as="xs:string">
        <xsl:param name="tag_name" as="xs:string"/>
        <xsl:param name="args" as="xs:string*"/>
        <xsl:param name="empty" as="xs:boolean"/>
        <xsl:param name="tab" as="xs:integer" />

        <xsl:variable name="tag_value" select="o:join_list(($tag_name, $args, if($empty) then '/' else ''), ' ')" />
        <xsl:value-of select="concat(o:t($tab), o:wrap_angle($tag_value))" />
    </xsl:function>

    <!--
        Wraps a tagname to be used as a start tag
        </${tag_name}>
    -->
    <xsl:function name="xmlo:end_tag" as="xs:string">
        <xsl:param name="tag_name" as="xs:string"/>
        <xsl:param name="tab" as="xs:integer" />
        <xsl:value-of select="concat(o:t($tab), o:wrap_angle(concat('/', $tag_name)))" />
    </xsl:function>

    <!--
        Wraps an xml tag element
    -->
    <xsl:function name="xmlo:wrap_tag" as="xs:string*">
        <xsl:param name="key_name" as="xs:string"/>
        <xsl:param name="attributes" as="xs:string"/>
        <xsl:param name="value" as="xs:string*"/>
        <xsl:param name="tab" as="xs:integer" />

        <xsl:variable name="flat_val" select="string-join($value, '')" />

        <xsl:choose>
            <xsl:when test="contains($flat_val, o:nl(1))">
                <!-- Multiline -->
                <xsl:value-of select="concat(xmlo:start_tag($key_name, $attributes, false(), $tab), o:nl(1), $flat_val, xmlo:end_tag($key_name, $tab), o:nl(1))" />
            </xsl:when>
            <xsl:when test="$flat_val = ''">
                <!-- Multiline -->
                <xsl:value-of select="concat(xmlo:start_tag($key_name, $attributes, true(), $tab), o:nl(1))" />
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="concat(xmlo:start_tag($key_name, $attributes, false(), $tab), $flat_val, xmlo:end_tag($key_name, 0), o:nl(1))" />
            </xsl:otherwise>
        </xsl:choose>
    </xsl:function>
</xsl:stylesheet>