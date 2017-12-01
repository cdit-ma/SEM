<!-- Functions for cpp syntax output -->
<xsl:stylesheet version="2.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:xs="http://www.w3.org/2001/XMLSchema"
    xmlns:o="http://github.com/cdit-ma/o"
    xmlns:cpp="http://github.com/cdit-ma/cpp"
    >
    <!--
        Places the text (handles newlines) in a cpp comment, can be tabbed
    -->
    <xsl:function name="cpp:comment">
        <xsl:param name="text" />
        <xsl:param name="tab" as="xs:integer" />

        <xsl:for-each select="tokenize($text, '\n\r?')[.]">
            <xsl:value-of select="concat(o:t($tab), '// ', ., o:nl(1))" />
        </xsl:for-each>
    </xsl:function>

    <!--
        Adds a -> symbol
    -->
    <xsl:function name="cpp:arrow">
        <xsl:value-of select="concat('-', o:gt())" />
    </xsl:function>

    <!--
        Wraps a type in template brackets, handles nested type semantics
    -->
    <xsl:function name="cpp:wrap_template">
        <xsl:param name="type" />

        <!-- Handle template types which end with >> -->
        <xsl:variable name="safe_type">
            <xsl:choose>
                <xsl:when test="ends-with($type, o:gt())">
                    <xsl:value-of select="concat($type, ' ')" />
                </xsl:when>
                <xsl:otherwise>
                    <xsl:value-of select="$type" />
                </xsl:otherwise>
            </xsl:choose>
        </xsl:variable>
        <xsl:value-of select="o:wrap_angle($safe_type)" />
    </xsl:function>

    

    <!--
        Used for including system library headers
        #include <${library_path}>
    -->
    <xsl:function name="cpp:include_library_header">
        <xsl:param name="library_path" />

        <xsl:value-of select="concat('#include ', o:wrap_angle($library_path), o:nl(1))" />
    </xsl:function>

    <!--
        Used for including local headers
        #include "${library_path}"
    -->
    <xsl:function name="cpp:include_local_header">
        <xsl:param name="library_path" />

        <xsl:value-of select="concat('#include ', o:wrap_dblquote($library_path), o:nl(1))" />
    </xsl:function>

    <!--
        Used for header files
        #ifndef ${CLASS_NAME}_H
        #define ${CLASS_NAME}_H
    -->
    <xsl:function name="cpp:define_guard_start">
        <xsl:param name="class_name" as="xs:string"  />

        <xsl:variable name="class_name_uc" select="upper-case(concat($class_name, '_h'))"  />
        <xsl:value-of select="concat('#ifndef ', $class_name_uc, o:nl(1))" />
        <xsl:value-of select="concat('#define ', $class_name_uc, o:nl(1))" />
        <xsl:value-of select="o:nl(1)" />
    </xsl:function>

    <!--
        Used for header files
        #endif ${CLASS_NAME}_H
    -->
    <xsl:function name="cpp:define_guard_end">
        <xsl:param name="class_name" as="xs:string"  />

        <xsl:variable name="class_name_uc" select="upper-case(concat($class_name, '_h'))"  />
        <xsl:value-of select="concat('#endif ', cpp:comment($class_name_uc, 0))" />
    </xsl:function>

    <!--
        Produces a function definition
        ie. void namespace::function(int value)
    -->
    <xsl:function name="cpp:function_definition">
        <xsl:param name="return_type" />
        <xsl:param name="namespace" />
        <xsl:param name="function_name" />
        <xsl:param name="parameters" />
        
        <!-- Attach the namespace to the function_name -->
        <xsl:variable name="namespaced_function">
            <xsl:choose>
                <xsl:when test="$namespace = ''">
                    <xsl:value-of select="$function_name" />
                </xsl:when>
                <xsl:otherwise>
                    <xsl:value-of select="concat($namespace, '::', $function_name)" />
                </xsl:otherwise>
            </xsl:choose>
        </xsl:variable>

        <xsl:value-of select="concat($return_type, ' ', $namespaced_function, '(', $parameters, ')')" />
    </xsl:function>

    <!--
        Produces a variable definition
        ie. std::string message
    -->
    <xsl:function name="cpp:variable_definition">
        <xsl:param name="variable_type" />
        <xsl:param name="variable_name" />
        <xsl:value-of select="concat($variable_type, ' ', $variable_name)" />
    </xsl:function>

    <!--
        Wraps a type as a std::shared_ptr
        ie. std::shared_ptr<std::string>
    -->
    <xsl:function name="cpp:shared_ptr">
        <xsl:param name="type" />
        <xsl:value-of select="concat('std::shared_ptr', cpp:wrap_template($type))" />
    </xsl:function>

    <!--
        Wraps a type as a std::weak_ptr
        ie. std::weak_ptr<std::string>
    -->
    <xsl:function name="cpp:weak_ptr">
        <xsl:param name="type" />
        <xsl:value-of select="concat('std::weak_ptr', cpp:wrap_template($type))" />
    </xsl:function>

    <!--
        Calls std::make_shared to construct a shared pointer
        ie. std::make_shared<${type_name}>(${args})
    -->
    <xsl:function name="cpp:make_shared_ptr">
        <xsl:param name="type" />
        <xsl:param name="args" />
        <xsl:value-of select="concat('std::make_shared', cpp:wrap_template($type), o:wrap_bracket($args))" />
    </xsl:function>


    <!--
        Used to open a namespace
        ie namespace ${namespace} {
    -->
    <xsl:function name="cpp:namespace">
        <xsl:param name="namespace" />
        <xsl:param name="tab" as="xs:integer" />

        <xsl:value-of select="concat(o:t($tab), 'namespace ', $namespace, '{', o:nl(1))" />
    </xsl:function>


    <!--
        Used to forward declare a class
        ie namespace ${namespace}{
            class ${class_name};
        };
    -->
    <xsl:function name="cpp:forward_declare_class" as="xs:string*">
        <xsl:param name="namespace" />
        <xsl:param name="class_name" />
        <xsl:param name="tab" as="xs:integer" />

        <xsl:if test="$namespace != ''">
            <xsl:value-of select="cpp:namespace($namespace, $tab)" />
            <xsl:value-of select="o:t($tab + 1)" />
        </xsl:if>

        <xsl:value-of select="concat('class ', $class_name, ';', o:nl(1))" />

        <xsl:if test="$namespace != ''">
            <xsl:value-of select="concat(o:t($tab), '};', o:nl(1))" />
        </xsl:if>
    </xsl:function>
</xsl:stylesheet>