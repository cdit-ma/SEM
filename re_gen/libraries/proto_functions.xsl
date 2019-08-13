<!-- Functions for cmake syntax output -->
<xsl:stylesheet version="2.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:xs="http://www.w3.org/2001/XMLSchema"
    xmlns:o="http://github.com/cdit-ma/re_gen/o"
    xmlns:proto="http://github.com/cdit-ma/re_gen/proto"
    xmlns:cpp="http://github.com/cdit-ma/re_gen/cpp"
    >

    <!--
        produces
        syntax = ${proto_version}";
    -->
    <xsl:function name="proto:syntax" as="xs:string">
        <xsl:param name="proto_version" as="xs:string" />
        
        <xsl:value-of select="concat('syntax = ', o:wrap_dblquote($proto_version), ';', o:nl(2))" />
    </xsl:function>

    <!--
        produces
        import "${proto_file}";
    -->
    <xsl:function name="proto:import" as="xs:string">
        <xsl:param name="proto_file" as="xs:string" />
        
        <xsl:value-of select="concat('import public ', o:wrap_dblquote($proto_file), ';', o:nl(1))" />
    </xsl:function>

    <!--
        produces
        package "${package_name}";
    -->
    <xsl:function name="proto:package" as="xs:string?">
        <xsl:param name="package_name" as="xs:string" />
        <xsl:if test="$package_name">
            <xsl:value-of select="concat('package ', $package_name, ';', o:nl(1))" />
        </xsl:if>
    </xsl:function>

    <!--
        produces
        ${type} ${label} = ${index};
    -->
    <xsl:function name="proto:member" as="xs:string">
        <xsl:param name="type" as="xs:string" />
        <xsl:param name="label" as="xs:string" />
        <xsl:param name="index" as="xs:integer" />
        
        <xsl:variable name="line" select="o:join_list(($type, $label, '=', string($index)), ' ')" />
        <xsl:value-of select="concat(o:t(1), $line, ';', o:nl(1))" />
    </xsl:function>

    <!--
        produces
        repeated ${type} ${label} = ${index};
    -->
    <xsl:function name="proto:repeated_member" as="xs:string">
        <xsl:param name="type" as="xs:string" />
        <xsl:param name="label" as="xs:string" />
        <xsl:param name="index" as="xs:integer" />
        
        <xsl:value-of select="proto:member(o:join_list(('repeated', $type), ' '), $label, $index)" />
    </xsl:function>


    <!--
        produces
        package "${package_name}";
    -->
    <xsl:function name="proto:message" as="xs:string">
        <xsl:param name="message_name" as="xs:string" />
        
        <xsl:value-of select="concat('message ', $message_name, cpp:scope_start(0))" />
    </xsl:function>

    <!-- Converts from the CPP primitive type to the appropriate proto types -->
    <xsl:function name="proto:get_type" as="xs:string">
        <xsl:param name="cpp_type" as="xs:string"  />

        <xsl:choose>
            <xsl:when test="$cpp_type = 'std::string'">
                <xsl:value-of select="'string'" />
            </xsl:when>
            <xsl:when test="$cpp_type = 'int' or $cpp_type = 'char'">
                <xsl:value-of select="'int64'" />
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

    <!--
        produces an enum definition
        enum ${label} {
    -->
    <xsl:function name="proto:enum" as="xs:string">
        <xsl:param name="label" as="xs:string" />
        <xsl:value-of select="concat('enum ', $label, cpp:scope_start(0))" />
    </xsl:function>

    <!--
        produces an enum value definition
    -->
    <xsl:function name="proto:enum_value" as="xs:string">
        <xsl:param name="label" as="xs:string" />
        <xsl:param name="index" as="xs:integer" />
        <xsl:value-of select="proto:member('', $label, $index)" />
    </xsl:function>

    
</xsl:stylesheet>