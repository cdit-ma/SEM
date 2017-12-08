<!-- Functions for cmake syntax output -->
<xsl:stylesheet version="2.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:xs="http://www.w3.org/2001/XMLSchema"
    xmlns:o="http://github.com/cdit-ma/o"
    xmlns:idl="http://github.com/cdit-ma/idl"
    xmlns:cpp="http://github.com/cdit-ma/cpp"
    >

    <!--
        produces include statement for idl files
    -->
    <xsl:function name="idl:include" as="xs:string">
        <xsl:param name="idl_file" as="xs:string" />
        
        <xsl:value-of select="cpp:include_library_header($idl_file)" />
    </xsl:function>

    <!--
        produces
        module ${label} {
    -->
    <xsl:function name="idl:module" as="xs:string">
        <xsl:param name="label" as="xs:string" />
        <xsl:value-of select="concat('module ', $label, cpp:scope_start(0))" />
    </xsl:function>

    <!--
        produces an enum definition
        enum ${label} {
    -->
    <xsl:function name="idl:enum" as="xs:string">
        <xsl:param name="label" as="xs:string" />
        <xsl:value-of select="concat('enum ', $label, cpp:scope_start(0))" />
    </xsl:function>

    <!--
        produces an enum value definition
    -->
    <xsl:function name="idl:enum_value" as="xs:string">
        <xsl:param name="label" as="xs:string" />
        <xsl:param name="is_last" as="xs:boolean" />
        <xsl:value-of select="concat(o:t(1), $label, if($is_last) then '' else ',', o:nl(1))" />
    </xsl:function>

    <!--
        produces
        struct ${label} {
    -->
    <xsl:function name="idl:struct" as="xs:string">
        <xsl:param name="label" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />
        <xsl:value-of select="concat(o:t($tab), 'struct ', $label, cpp:scope_start(0))" />
    </xsl:function>

    <!--
        produces
        // @key
    -->
    <xsl:function name="idl:key" as="xs:string">
        <xsl:param name="is_key" as="xs:boolean" />
        <xsl:value-of select="if($is_key) then cpp:comment('@key', 0) else ''" />
    </xsl:function>

    <!--
        produces
        // @key
    -->
    <xsl:function name="idl:key_pragma" as="xs:string">
        <xsl:param name="type" as="xs:string" />
        <xsl:param name="key" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />

        <xsl:value-of select="concat(o:t($tab), '#pragma keylist ', $type, ' ', $key, o:nl(1))" />
    </xsl:function>

    <!--
        produces
        ${type} ${label} = ${index}; //@key
    -->
    <xsl:function name="idl:member" as="xs:string">
        <xsl:param name="type" as="xs:string" />
        <xsl:param name="label" as="xs:string" />
        <xsl:param name="is_key" as="xs:boolean" />
        <xsl:param name="tab" as="xs:integer" />
        
        <xsl:variable name="line" select="o:join_list(($type, $label), ' ')" />
        <xsl:value-of select="concat(o:t($tab), $line, ';', if($is_key) then idl:key($is_key) else o:nl(1))" />
    </xsl:function>

    <!--
        produces
        sequence<${type}> ${label}; //@key
    -->
    <xsl:function name="idl:sequence_member" as="xs:string">
        <xsl:param name="type" as="xs:string" />
        <xsl:param name="label" as="xs:string" />
        <xsl:param name="is_key" as="xs:boolean" />
        <xsl:param name="tab" as="xs:integer" />
        
        <xsl:value-of select="idl:member(concat('sequence', o:wrap_angle($type)), $label, $is_key, $tab)" />
    </xsl:function>

    <!-- Converts from the CPP primitive type to the appropriate idl types -->
    <xsl:function name="idl:get_type" as="xs:string">
        <xsl:param name="cpp_type" as="xs:string"  />

        <xsl:choose>
            <xsl:when test="$cpp_type = 'std::string'">
                <xsl:value-of select="'string'" />
            </xsl:when>
             <xsl:when test="$cpp_type = 'int'">
                <xsl:value-of select="'long'" />
            </xsl:when>
            <xsl:when test="$cpp_type = 'char'">
                <xsl:value-of select="'char'" />
            </xsl:when>
            <xsl:when test="$cpp_type = 'double' or $cpp_type = 'float' or $cpp_type = 'bool'">
                <xsl:value-of select="$cpp_type" />
            </xsl:when>
            <xsl:otherwise>
                <xsl:message>Warning: Unknown Type <xsl:value-of select="o:wrap_quote($cpp_type)" /></xsl:message>
                <xsl:value-of select="''" />
            </xsl:otherwise>
        </xsl:choose>
    </xsl:function>
</xsl:stylesheet>