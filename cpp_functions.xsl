<!-- Functions for cpp syntax output -->
<xsl:stylesheet version="2.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:xs="http://www.w3.org/2001/XMLSchema"
    xmlns:o="http://github.com/cdit-ma"
    xmlns:cpp="http://github.com/cdit-ma"
    >
    <!--
        Places the text (handles newlines) in a cpp comment, can be tabbed
    -->
    <xsl:function name="cpp:comment" as="xs:string">
        <xsl:param name="text" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />

        <xsl:for-each select="tokenize($text, '\n\r?')[.]">
            <xsl:value-of select="concat(o:t($tab), '// ', ., o:nl(1))" />
        </xsl:for-each>
    </xsl:function>

    <!--
        Adds a -> symbol
    -->
    <xsl:function name="cpp:arrow" as="xs:string">
        <xsl:value-of select="concat('-', o:gt())" />
    </xsl:function>

    <!--
        Wraps a type in template brackets, handles nested type semantics
    -->
    <xsl:function name="cpp:wrap_template" as="xs:string">
        <xsl:param name="type" as="xs:string" />

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
    <xsl:function name="cpp:include_library_header" as="xs:string">
        <xsl:param name="library_path" as="xs:string" />

        <xsl:value-of select="concat('#include ', o:wrap_angle($library_path), o:nl(1))" />
    </xsl:function>

    <!--
        Used for including local headers
        #include "${library_path}"
    -->
    <xsl:function name="cpp:include_local_header" as="xs:string">
        <xsl:param name="library_path" as="xs:string" />

        <xsl:value-of select="concat('#include ', o:wrap_dblquote($library_path), o:nl(1))" />
    </xsl:function>

    <!--
        Used for header files
        #ifndef ${CLASS_NAME}_H
        #define ${CLASS_NAME}_H
    -->
    <xsl:function name="cpp:define_guard_start" as="xs:string">
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
    <xsl:function name="cpp:define_guard_end" as="xs:string">
        <xsl:param name="class_name" as="xs:string"  />

        <xsl:variable name="class_name_uc" select="upper-case(concat($class_name, '_h'))"  />
        <xsl:value-of select="concat('#endif ', cpp:comment($class_name_uc, 0))" />
    </xsl:function>

    <!--
        Produces a function definition
        ie. void namespace::function(int value)
    -->
    <xsl:function name="cpp:function_definition" as="xs:string">
        <xsl:param name="return_type" as="xs:string" />
        <xsl:param name="namespace" as="xs:string" />
        <xsl:param name="function_name" as="xs:string" />
        <xsl:param name="parameters" as="xs:string" />
        
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
    <xsl:function name="cpp:variable_definition" as="xs:string">
        <xsl:param name="variable_type" as="xs:string" />
        <xsl:param name="variable_name" as="xs:string" />
        <xsl:value-of select="concat($variable_type, ' ', $variable_name)" />
    </xsl:function>

    <!--
        Wraps a type as a std::shared_ptr
        ie. std::shared_ptr<std::string>
    -->
    <xsl:function name="cpp:shared_ptr" as="xs:string">
        <xsl:param name="type" as="xs:string" />
        <xsl:value-of select="concat('std::shared_ptr', cpp:wrap_template($type))" />
    </xsl:function>

    <!--
        Wraps a type as a std::weak_ptr
        ie. std::weak_ptr<std::string>
    -->
    <xsl:function name="cpp:weak_ptr" as="xs:string">
        <xsl:param name="type" as="xs:string" />
        <xsl:value-of select="concat('std::weak_ptr', cpp:wrap_template($type))" />
    </xsl:function>

    <!--
        Calls std::make_shared to construct a shared pointer
        ie. std::make_shared<${type_name}>(${args})
    -->
    <xsl:function name="cpp:make_shared_ptr" as="xs:string">
        <xsl:param name="type" as="xs:string" />
        <xsl:param name="args" as="xs:string" />
        <xsl:value-of select="concat('std::make_shared', cpp:wrap_template($type), o:wrap_bracket($args))" />
    </xsl:function>


    <!--
        Used to open a namespace
        ie namespace ${namespace} {
    -->
    <xsl:function name="cpp:namespace" as="xs:string">
        <xsl:param name="namespace" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />

        <xsl:value-of select="concat(o:t($tab), 'namespace ', $namespace, '{', o:nl(1))" />
    </xsl:function>


    <!--
        Used to forward declare a class
        ie namespace ${namespace}{
            class ${class_name};
        };
    -->
    <xsl:function name="cpp:forward_declare_class" as="xs:string">
        <xsl:param name="namespace" as="xs:string" />
        <xsl:param name="class_name" as="xs:string" />
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