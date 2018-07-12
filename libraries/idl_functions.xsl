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
    <xsl:function name="idl:module_start" as="xs:string">
        <xsl:param name="namespace" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />

        <xsl:value-of select="concat(o:t($tab), 'module ', $namespace, cpp:scope_start(0))" />
    </xsl:function>

    <!--
        Used to close a module
        ie };
    -->
    <xsl:function name="idl:module_end">
        <xsl:param name="namespace" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />

        <xsl:value-of select="concat(o:t($tab), '}; ', cpp:comment($namespace, 0))" />
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
        Produces a union member definition
        ie. case ${case_index}: ${type} ${label};
    -->
    <xsl:function name="idl:union_member" as="xs:string">
        <xsl:param name="case_index" as="xs:string" />
        <xsl:param name="type" as="xs:string" />
        <xsl:param name="label" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />
        
        <xsl:value-of select="concat(o:t($tab), 'case ', $case_index, ': ', $type, ' ', $label, cpp:nl())" />
    </xsl:function>

    <!--
        Produces a union definition
        ie. union ${label} switch(long) {
    -->
    <xsl:function name="idl:union" as="xs:string">
        <xsl:param name="label" as="xs:string" />
        <xsl:param name="union_type" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />
        <xsl:value-of select="concat(o:t($tab), 'union ', $label, ' switch', o:wrap_bracket($union_type), cpp:scope_start(0))" />
    </xsl:function>

    

    <!--
        Produces a interface definition
        ie. interface ${label} {
    -->
    <xsl:function name="idl:interface" as="xs:string">
        <xsl:param name="label" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />
        <xsl:value-of select="concat(o:t($tab), 'interface ', $label, cpp:scope_start(0))" />
    </xsl:function>

    <!--
        Produces a interface definition
        ie. interface ${label} {
    -->
    <xsl:function name="idl:function" as="xs:string">
        <xsl:param name="function_name" as="xs:string" />
        <xsl:param name="return_type" as="xs:string" />
        <xsl:param name="input_parameters" as="xs:string" />
        <xsl:param name="one_way" as="xs:boolean" />
        <xsl:param name="tab" as="xs:integer" />
        <xsl:value-of select="concat(o:t($tab), if($one_way) then 'oneway' else '', $return_type, ' ', $function_name, o:wrap_bracket($input_parameters), cpp:nl())" />
    </xsl:function>

    <!--
        Produces key pragma for RTI
        ie. // @key
    -->
    <xsl:function name="idl:key" as="xs:string">
        <xsl:param name="is_key" as="xs:boolean" />
        <xsl:value-of select="if($is_key) then concat(' //@key', o:nl(1)) else ''" />
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

    <xsl:function name="idl:get_enum">
        <xsl:param name="enum_namespaces" as="xs:string*" />
        <xsl:param name="enum_label" as="xs:string" />
        <xsl:param name="enum_members" as="xs:string*" />
        <xsl:param name="tab" />

        <xsl:variable name="enum_namespace" select="o:trim_list($enum_namespaces)" />

        <xsl:variable name="enum_guard_name" select="upper-case(o:join_list(($enum_namespace, $enum_label, 'DATATYPE', 'IDL'), '_'))" />

        <!-- Define Guard -->
        <xsl:value-of select="cpp:define_guard_start($enum_guard_name)" />

        

        <xsl:variable name="enum_tab" select="$tab + count($enum_namespace)" />

        
        <!-- Define Namespaces -->
        <xsl:for-each select="$enum_namespace">
            <xsl:value-of select="idl:module_start(., position() - 1)" />
        </xsl:for-each>

        <xsl:value-of select="idl:enum($enum_label, $enum_tab)" />
            <xsl:for-each select="$enum_members">
                <xsl:variable name="enum_member_label" select="upper-case(.)" />
                <xsl:value-of select="idl:enum_value($enum_member_label, position() = last(), $enum_tab + 1)" />
            </xsl:for-each>
        <xsl:value-of select="cpp:scope_end($tab)" />

        <!-- Define Namespaces -->
        <xsl:for-each select="$enum_namespace">
            <xsl:value-of select="idl:module_end(., position() - 1)" />
        </xsl:for-each>

        <!-- Define Guard -->
        <xsl:value-of select="o:nl(1)" />
        <xsl:value-of select="cpp:define_guard_end($enum_guard_name)" />
    </xsl:function>
</xsl:stylesheet>