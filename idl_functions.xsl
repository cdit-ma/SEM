<!--
    A set of XSLT2.0 Functions for outputting Interface Definition Language files.
-->
<xsl:stylesheet version="2.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:xs="http://www.w3.org/2001/XMLSchema"
    xmlns:o="http://github.com/cdit-ma/re_gen/o"
    xmlns:idl="http://github.com/cdit-ma/re_gen/idl"
    xmlns:cpp="http://github.com/cdit-ma/re_gen/cpp"
    >

    <!--
        Produces an include statement
        (Same as CPP)
    -->
    <xsl:function name="idl:include" as="xs:string">
        <xsl:param name="idl_file" as="xs:string" />
        
        <xsl:value-of select="cpp:include_library_header($idl_file)" />
    </xsl:function>

    <!--
        Produces a module element
        ie. module ${label} {
    -->
    <xsl:function name="idl:module" as="xs:string">
        <xsl:param name="label" as="xs:string" />
        <xsl:value-of select="concat('module ', $label, cpp:scope_start(0))" />
    </xsl:function>

    <!--
        Produces an enum definition
        ie. enum ${label} {
    -->
    <xsl:function name="idl:enum" as="xs:string">
        <xsl:param name="label" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />
        <xsl:value-of select="concat(o:t($tab), 'enum ', $label, cpp:scope_start(0))" />
    </xsl:function>

    <!--
        Produces an enum value definition
        ie. ${label},
    -->
    <xsl:function name="idl:enum_value" as="xs:string">
        <xsl:param name="label" as="xs:string" />
        <xsl:param name="is_last" as="xs:boolean" />
        <xsl:param name="tab" as="xs:integer" />
        <xsl:value-of select="concat(o:t($tab), $label, if($is_last) then '' else ',', o:nl(1))" />
    </xsl:function>

    <!--
        Produces a struct definition
        ie. struct ${label} {
    -->
    <xsl:function name="idl:struct" as="xs:string">
        <xsl:param name="label" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />
        <xsl:value-of select="concat(o:t($tab), 'struct ', $label, cpp:scope_start(0))" />
    </xsl:function>

    <!--
        Produces key pragma for RTI
        ie. // @key
    -->
    <xsl:function name="idl:key" as="xs:string">
        <xsl:param name="is_key" as="xs:boolean" />
        <xsl:value-of select="if($is_key) then cpp:comment('@key', 0) else ''" />
    </xsl:function>

    <!--
        Produces key pragma for OSPL
        ie. #pragma keylist ${type} ${key}
    -->
    <xsl:function name="idl:key_pragma" as="xs:string">
        <xsl:param name="type" as="xs:string" />
        <xsl:param name="key" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />

        <xsl:value-of select="concat(o:t($tab), '#pragma keylist ', $type, ' ', $key, o:nl(1))" />
    </xsl:function>

    <!--
        Produces a member definition
        ie. ${type} ${label} = ${index}; //@key
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
        Produces a sequence member definition
        ie. sequence<${type}> ${label}; //@key
    -->
    <xsl:function name="idl:sequence_member" as="xs:string">
        <xsl:param name="type" as="xs:string" />
        <xsl:param name="label" as="xs:string" />
        <xsl:param name="is_key" as="xs:boolean" />
        <xsl:param name="tab" as="xs:integer" />
        
        <xsl:value-of select="idl:member(concat('sequence', o:wrap_angle($type)), $label, $is_key, $tab)" />
    </xsl:function>

    <!--
        Converts from the CPP primitive type to the appropriate idl types
    -->
    <xsl:function name="idl:get_type" as="xs:string">
        <xsl:param name="cpp_type" as="xs:string" />

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
            <xsl:when test="$cpp_type = 'bool'">
                <xsl:value-of select="'boolean'" />
            </xsl:when>
            <xsl:when test="$cpp_type = 'double' or $cpp_type = 'float'">
                <xsl:value-of select="$cpp_type" />
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="o:warning(('Unhandled CPP to IDL conversion', o:wrap_quote($cpp_type)))" />
            </xsl:otherwise>
        </xsl:choose>
    </xsl:function>
</xsl:stylesheet>