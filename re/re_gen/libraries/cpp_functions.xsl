<!--
    A set of XSLT2.0 Functions for outputting C++17 code.
-->
<xsl:stylesheet version="2.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:xs="http://www.w3.org/2001/XMLSchema"
    xmlns:o="http://github.com/cdit-ma/re_gen/o"
    xmlns:cpp="http://github.com/cdit-ma/re_gen/cpp"
    >

    <!--
        Places the text (handles newlines) in a cpp comment, can be tabbed
        // ${text}
    -->
    <xsl:function name="cpp:comment">
        <xsl:param name="text" as="xs:string*" />
        <xsl:param name="tab" as="xs:integer" />

        <xsl:variable name="joined_text" select="o:join_list($text, ' ')" />

        <xsl:for-each select="tokenize($joined_text, '\n\r?')[.]">
            <xsl:value-of select="concat(o:t($tab), '// ', normalize-space(.), o:nl(1))" />
        </xsl:for-each>
    </xsl:function>

    <!--
        Places the text in an inline comment block, useful for returning errors into code
        /* ${text} */
    -->
    <xsl:function name="cpp:comment_inline">
        <xsl:param name="text" as="xs:string*" />

        <xsl:variable name="joined_text" select="o:join_list($text, ' ')" />
        <xsl:value-of select="o:join_list(('/*', $joined_text, '*/'), ' ')" />
    </xsl:function>

    <!--
        Places the text in an inline commend block, on with a new-line
        /* Warning: ${text} */
    -->
    <xsl:function name="cpp:warning">
        <xsl:param name="text" as="xs:string*" />
        <xsl:param name="tab" as="xs:integer" />
        
        <xsl:value-of select="o:warning($text)" />
        <xsl:value-of select="concat(o:t($tab), cpp:comment_inline(('Warning:', $text)))" />
    </xsl:function>

    <!--
        Adds a '->'' symbol
    -->
    <xsl:function name="cpp:arrow">
        <xsl:value-of select="concat('-', o:gt())" />
    </xsl:function>

    <!--
        Adds a '.' symbol
    -->
    <xsl:function name="cpp:dot">
        <xsl:value-of select="'.'" />
    </xsl:function>

    <!--
        Returns  'void'
    -->
    <xsl:function name="cpp:void">
        <xsl:value-of select="'void'" />
    </xsl:function>

    <!--
        Wraps a type in template brackets, handles nested type semantics
        <${type}>
    -->
    <xsl:function name="cpp:wrap_template">
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
    <xsl:function name="cpp:include_library_header">
        <xsl:param name="library_path" as="xs:string" />

        <xsl:if test="$library_path != ''">
            <xsl:value-of select="concat('#include ', o:wrap_angle($library_path), o:nl(1))" />
        </xsl:if>
    </xsl:function>

    <!--
        Used for including local headers
        #include "${library_path}"
    -->
    <xsl:function name="cpp:include_local_header">
        <xsl:param name="library_path" as="xs:string" />
        <xsl:if test="$library_path != ''">
            <xsl:value-of select="concat('#include ', o:wrap_dblquote($library_path), o:nl(1))" />
        </xsl:if>
    </xsl:function>

    <!--
        Produces a define guard (Will uppercase)
        #ifndef ${CLASS_NAME}_H
        #define ${CLASS_NAME}_H
    -->
    <xsl:function name="cpp:define_guard_start">
        <xsl:param name="class_name" as="xs:string" />

        <xsl:variable name="class_name_uc" select="upper-case(concat($class_name, '_h'))"  />
        <xsl:value-of select="concat('#ifndef ', $class_name_uc, o:nl(1))" />
        <xsl:value-of select="concat('#define ', $class_name_uc, o:nl(1))" />
    </xsl:function>

    <!--
        Ends a define guard (Will uppercase)
        #endif // ${CLASS_NAME}_H
    -->
    <xsl:function name="cpp:define_guard_end">
        <xsl:param name="class_name" as="xs:string" />

        <xsl:variable name="class_name_uc" select="upper-case(concat($class_name, '_h'))"  />
        <xsl:value-of select="concat('#endif ', cpp:comment($class_name_uc, 0))" />
    </xsl:function>

    <!--
        Produces a define 
        #define ${define_name} ${define_val}
    -->
    <xsl:function name="cpp:define">
        <xsl:param name="define_name" as="xs:string" />
        <xsl:param name="define_val" as="xs:string" />

        <xsl:value-of select="o:join_list(('#define', $define_name, $define_val), ' ')" />
        <xsl:value-of select="o:nl(1)" />
    </xsl:function>

     <!--
        Produces a undef
        #undef ${define_name}
    -->
    <xsl:function name="cpp:undef">
        <xsl:param name="define_name"/>

        <xsl:value-of select="o:join_list(('#undef', $define_name), ' ')" />
        <xsl:value-of select="o:nl(1)" />
    </xsl:function>

    <!--
        Produces a function declaration
        ${return_type} ${function_name}(${parameters})${suffix}
    -->
    <xsl:function name="cpp:declare_function">
        <xsl:param name="return_type" as="xs:string" />
        <xsl:param name="function_name" as="xs:string" />
        <xsl:param name="parameters" as="xs:string?" />
        <xsl:param name="suffix" as="xs:string?" />
        <xsl:param name="tab" as="xs:integer" />

        <xsl:value-of select="concat(o:t($tab), o:join_list(($return_type, $function_name), ' '), '(', $parameters, ')', $suffix, o:nl(1))" />
    </xsl:function>

     <!--
        Produces a templated function declaration
        template <${template_type_name}> ${return_type} ${function_name}(${parameters})${suffix}
    -->
    <xsl:function name="cpp:declare_templated_function">
        <xsl:param name="template_type_name" as="xs:string" />
        <xsl:param name="return_type" as="xs:string" />
        <xsl:param name="function_name" as="xs:string" />
        <xsl:param name="parameters" as="xs:string?" />
        <xsl:param name="suffix" as="xs:string?" />
        <xsl:param name="tab" as="xs:integer" />

        <xsl:variable name="template_return_type" select="concat('template ', cpp:wrap_template($template_type_name), ' ', $return_type)" />
        <xsl:value-of select="cpp:declare_function($template_return_type, $function_name, $parameters, $suffix, $tab)" />
    </xsl:function>

    <!--
        Declare a template specialization function declaration
        template <>
        ${return_type} ${function_name}<${template_type_name}>(${parameters})${suffix}
    -->
    <xsl:function name="cpp:declare_templated_function_specialisation">
        <xsl:param name="template_type_name" as="xs:string" />
        <xsl:param name="return_type" as="xs:string" />
        <xsl:param name="function_name" as="xs:string" />
        <xsl:param name="parameters" as="xs:string?" />
        <xsl:param name="suffix" as="xs:string?" />
        <xsl:param name="tab" as="xs:integer" />

        <xsl:value-of select="cpp:declare_templated_class_function_specialisation($template_type_name, $return_type, '', $function_name, $parameters, $suffix, $tab)" />
    </xsl:function>

    <!--
        Produces a class template specialisation function declaration
        template <>
        ${return_type} ${class_name}<${template_type_name}>::${function_name}(${parameters})${suffix}
    -->
    <xsl:function name="cpp:declare_templated_class_function_specialisation">
        <xsl:param name="template_type_name" as="xs:string" />
        <xsl:param name="return_type" as="xs:string" />
        <xsl:param name="class_name" as="xs:string?" />
        <xsl:param name="function_name" as="xs:string" />
        <xsl:param name="parameters" as="xs:string?" />
        <xsl:param name="suffix" as="xs:string?" />
        <xsl:param name="tab" as="xs:integer" />

        <xsl:variable name="template_return_type" select="concat('template ', cpp:wrap_template(''), o:nl(1), o:t($tab), $return_type)" />

        <xsl:variable name="template_function_name">
            <xsl:choose>
                <xsl:when test="$class_name != ''">
                    <xsl:value-of select="concat($class_name, cpp:wrap_template($template_type_name), '::', $function_name)" />
                </xsl:when>
                <xsl:otherwise>
                    <xsl:value-of select="concat($function_name, cpp:wrap_template($template_type_name))" />
                </xsl:otherwise>
            </xsl:choose>
        </xsl:variable>

        <xsl:value-of select="cpp:declare_function($template_return_type, $template_function_name, $parameters, $suffix, $tab)" />
    </xsl:function>

    <!--
        Calls a templated static function
        ${function_name}<${template_type_name}>(${parameters})${suffix}
    -->
    <xsl:function name="cpp:invoke_templated_static_function">
        <xsl:param name="template_type_name" as="xs:string" />
        <xsl:param name="function_name" as="xs:string" />
        <xsl:param name="parameters" as="xs:string?" />
        <xsl:param name="suffix" as="xs:string?" />
        <xsl:param name="tab" as="xs:integer" />

        <xsl:variable name="template_function_name" select="concat($function_name, cpp:wrap_template($template_type_name))" />
        <xsl:value-of select="cpp:invoke_templated_class_static_function($template_type_name, '', $function_name, $parameters, $suffix, $tab)" />
    </xsl:function>

    <!--
        Calls a templated function in a class
        ${class_name}<${template_type_name}>::${function_name}(${parameters})${suffix}
    -->
    <xsl:function name="cpp:invoke_templated_class_static_function">
        <xsl:param name="template_type_name" as="xs:string" />
        <xsl:param name="class_name" as="xs:string?" />
        <xsl:param name="function_name" as="xs:string" />
        <xsl:param name="parameters" as="xs:string?" />
        <xsl:param name="suffix" as="xs:string?" />
        <xsl:param name="tab" as="xs:integer" />

        <xsl:variable name="template_function_name">
            <xsl:choose>
                <xsl:when test="$class_name != ''">
                    <xsl:value-of select="concat($class_name, cpp:wrap_template($template_type_name), '::', $function_name)" />
                </xsl:when>
                <xsl:otherwise>
                    <xsl:value-of select="concat($function_name, cpp:wrap_template($template_type_name))" />
                </xsl:otherwise>
            </xsl:choose>
        </xsl:variable>

        <xsl:value-of select="cpp:invoke_static_function('', $template_function_name, $parameters, $suffix, $tab)" />
    </xsl:function>

    <!--
        Produces a function definition
        ${return_type} ${namespace}::${function_name}(${parameters})${suffix}
    -->
    <xsl:function name="cpp:define_function">
        <xsl:param name="return_type" as="xs:string" />
        <xsl:param name="namespace" as="xs:string" />
        <xsl:param name="function_name" as="xs:string" />
        <xsl:param name="parameters" as="xs:string?" />
        <xsl:param name="suffix" as="xs:string?" />

        <!-- Attach the namespace to the function_name -->
        <xsl:variable name="namespaced_function" select="cpp:combine_namespaces(($namespace, $function_name))" />
        <xsl:variable name="function_call" select="concat($namespaced_function, '(', $parameters, ')', $suffix)" />
        
        <xsl:value-of select="o:join_list(($return_type, $function_call), ' ')" />
    </xsl:function>

    <!--
        Produces a forward declare template function declaration
        template <>
        ${return_type} ${namespace}::${function_name}<${template_type_name}>(${parameters})${suffix}
    -->
    <xsl:function name="cpp:define_templated_function_specialisation">
        <xsl:param name="template_type_name" as="xs:string" />
        <xsl:param name="return_type" as="xs:string" />
        <xsl:param name="namespace" as="xs:string" />
        <xsl:param name="function_name" as="xs:string" />
        <xsl:param name="parameters" as="xs:string" />
        <xsl:param name="suffix" as="xs:string" />

        <xsl:variable name="template_return_type" select="concat('template ', cpp:wrap_template(''), o:nl(1), $return_type)" />
        <xsl:variable name="template_function_name" select="concat($function_name, cpp:wrap_template($template_type_name))" />

        <xsl:value-of select="cpp:define_function($template_return_type, $namespace, $template_function_name, $parameters, $suffix)" />
    </xsl:function>
    
    <!--
        Produces a variable declaration
        ${variable_type} ${variable_name}${suffix}
    -->
    <xsl:function name="cpp:declare_variable">
        <xsl:param name="variable_type" as="xs:string" />
        <xsl:param name="variable_name" as="xs:string?" />
        <xsl:param name="suffix" as="xs:string?" />
        <xsl:param name="tab" as="xs:integer" />
        
        <xsl:value-of select="concat(o:t($tab), o:join_list(($variable_type, $variable_name), ' '), $suffix)" />
    </xsl:function>

    <!--
        Produces a variable definition
        ${variable_type} ${variable_name} = ${value}${suffix}
    -->
    <xsl:function name="cpp:define_variable">
        <xsl:param name="variable_type" as="xs:string" />
        <xsl:param name="variable_name" as="xs:string" />
        <xsl:param name="value" as="xs:string" />
        <xsl:param name="suffix" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />

        <xsl:value-of select="cpp:declare_variable($variable_type, o:join_list(($variable_name, $value), ' = '), $suffix, $tab) " />
    </xsl:function>

    <!--
        Wraps a type as a std::shared_ptr
        std::shared_ptr<${type}>
    -->
    <xsl:function name="cpp:shared_ptr">
        <xsl:param name="type" as="xs:string" />
        <xsl:value-of select="concat('std::shared_ptr', cpp:wrap_template($type))" />
    </xsl:function>

    <!--
        Wraps a type as a std::weak_ptr
        std::weak_ptr<${type}>
    -->
    <xsl:function name="cpp:weak_ptr">
        <xsl:param name="type" as="xs:string" />
        <xsl:value-of select="concat('std::weak_ptr', cpp:wrap_template($type))" />
    </xsl:function>

    <!--
        Calls std::make_shared to construct a shared pointer
        std::make_shared<${type}>(${args})
    -->
    <xsl:function name="cpp:make_shared_ptr">
        <xsl:param name="type" as="xs:string" />
        <xsl:param name="args" as="xs:string" />
        <xsl:value-of select="concat('std::make_shared', cpp:wrap_template($type), o:wrap_bracket($args))" />
    </xsl:function>

    <!--
        Used to open a namespace
        namespace ${namespace}{
    -->
    <xsl:function name="cpp:namespace_start">
        <xsl:param name="namespace" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />

        <xsl:value-of select="concat(o:t($tab), 'namespace ', $namespace, cpp:scope_start(0))" />
    </xsl:function>

    <!--
        Used to close a namespace
        }; // ${namespace}
    -->
    <xsl:function name="cpp:namespace_end">
        <xsl:param name="namespace" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />

        <xsl:value-of select="concat(o:t($tab), '}; ', cpp:comment($namespace, 0))" />
    </xsl:function>

    <!--
        Used to forward declare a class, handles multiple namespaces
        namespace ${namespace1}{
            namespace ${namespace2}{
                class ${class_name};
            }; // ${namespace2}
        }; // ${namespace1}
    -->
    <xsl:function name="cpp:forward_declare_class" as="xs:string*">
        <xsl:param name="namespaces" as="xs:string*" />
        <xsl:param name="class_name" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />

        <xsl:variable name="pruned_namespaces" as="xs:string*" select="o:trim_list($namespaces)" />

        <xsl:value-of select="cpp:define_namespaces($pruned_namespaces)" />
        <xsl:value-of select="concat(o:t($tab + count($pruned_namespaces)), 'class ', $class_name, ';', o:nl(1))" />
        <xsl:value-of select="cpp:close_namespaces($pruned_namespaces)" />
    </xsl:function>
    
    <!--
        Used to convert a type into a reference
        &${var}
    -->
    <xsl:function name="cpp:ref_var">
        <xsl:param name="var" as="xs:string"/>

        <xsl:value-of select="concat(o:and(), $var)" />
    </xsl:function>

    <!--
        Used to define a type as a reference
        ${type}& ${value}
    -->
    <xsl:function name="cpp:ref_var_def">
        <xsl:param name="type" as="xs:string"/>
        <xsl:param name="value" as="xs:string"/>

        <xsl:value-of select="cpp:var_def(concat($type, o:and()), $value)" />
    </xsl:function>

    <!--
        Used to define a type as a reference
        ${type} ${value}
    -->
    <xsl:function name="cpp:var_def">
        <xsl:param name="type" as="xs:string"/>
        <xsl:param name="value" as="xs:string"/>

        <xsl:value-of select="o:join_list(($type, $value), ' ')" />
    </xsl:function>

    <!--
        Used to define a type as a const reference
        const ${type}& ${value}
    -->
    <xsl:function name="cpp:const_ref_var_def">
        <xsl:param name="type" as="xs:string"/>
        <xsl:param name="value" as="xs:string"/>

        <xsl:value-of select="concat('const ', cpp:ref_var_def($type, $value))" />
    </xsl:function>
    
        <!--
        Used to define a type as a const
        ie const ${type} ${value}
    -->
    <xsl:function name="cpp:const_var_def">
        <xsl:param name="type" as="xs:string"/>
        <xsl:param name="value" as="xs:string"/>

        <xsl:value-of select="concat('const ', cpp:var_def($type, $value))" />
    </xsl:function>

    <!--
        Used to define a type as a pointer
        ${type}* ${value}
    -->
    <xsl:function name="cpp:pointer_var_def">
        <xsl:param name="type" as="xs:string"/>
        <xsl:param name="value" as="xs:string"/>
        <xsl:value-of select="o:join_list((concat($type, '*'), $value), ' ')" />
    </xsl:function>

    <!--
        Used to define a type as a pointer
        std::unique_ptr<${type}>
    -->
    <xsl:function name="cpp:unique_ptr">
        <xsl:param name="type" as="xs:string"/>
        <xsl:value-of select="concat('std::unique_ptr', o:wrap_angle($type))" />
    </xsl:function>

    <!--
        Used to define a type as a pointer
        std::unique_ptr<${type}> ${label}
    -->
    <xsl:function name="cpp:unique_ptr_def">
        <xsl:param name="type" as="xs:string"/>
        <xsl:param name="label" as="xs:string"/>
        <xsl:value-of select="o:join_list((cpp:unique_ptr($type), $label), ' ')" />
    </xsl:function>

    <!--
        Constructs a new object of the provided type (Combines the list into namespace syntax)
        std::unique_ptr<${namespace_1}::${namespace_2}::${type}>(${value})
    -->
    <xsl:function name="cpp:unique_ptr_object">
        <xsl:param name="name_list" as="xs:string*" />
        <xsl:param name="variable" as="xs:string" />

        <xsl:variable name="type" select="cpp:combine_namespaces($name_list)" />
        <xsl:value-of select="concat(cpp:unique_ptr($type), o:wrap_bracket($variable))" />
    </xsl:function>

    <!--
        Used to dereference a pointed object
        *${var}
    -->
    <xsl:function name="cpp:dereference_var">
        <xsl:param name="var" as="xs:string"/>
        <xsl:value-of select="concat('*', $var)" />
    </xsl:function>

    <!--
        Returns a const auto reference
        const auto&
    -->
    <xsl:function name="cpp:const_ref_auto">
        <xsl:value-of select="cpp:const_ref_var_def(cpp:auto(), '')" />
    </xsl:function>

    <!--
        Returns auto
        auto
    -->
    <xsl:function name="cpp:auto">
        <xsl:value-of select="'auto'" />
    </xsl:function>

    <!--
        Used to start a scoped bracketted section
        {
    -->
    <xsl:function name="cpp:scope_start">
        <xsl:param name="tab" as="xs:integer" />

        <xsl:value-of select="concat(o:t($tab), '{', o:nl(1))" />
    </xsl:function>

    <!--
        Used to close a scoped bracketted section
        };
    -->
    <xsl:function name="cpp:scope_end">
        <xsl:param name="tab" as="xs:integer" />

        <xsl:value-of select="concat(o:t($tab), '}', cpp:nl())" />
    </xsl:function>

    <!--
        Used to end a command
        ;\n
    -->
    <xsl:function name="cpp:nl">
        <xsl:value-of select="concat(';', o:nl(1))" />
    </xsl:function>

    <!--
        Used to invoke a function on an object
        ${obj}${operator}${function_name}(${parameters})
    -->
    <xsl:function name="cpp:invoke_function">
        <xsl:param name="obj" as="xs:string?"/>
        <xsl:param name="operator" as="xs:string"/>
        <xsl:param name="function_name" as="xs:string?"/>
        <xsl:param name="parameters" as="xs:string*"/>
        <xsl:param name="tab" as="xs:integer"/>

        <xsl:if test="$function_name != ''">
            <xsl:value-of select="concat(o:t($tab), o:join_list(($obj, $function_name), $operator), o:wrap_bracket($parameters))" />
        </xsl:if>
    </xsl:function>

    <!--
        Used to invoke a static function
        ${namespace_1}::${namespace_2}::${function_name}(${parameters})${suffix}
    -->
    <xsl:function name="cpp:invoke_static_function">
        <xsl:param name="namespaces" as="xs:string*"/>
        <xsl:param name="function_name" as="xs:string"/>
        <xsl:param name="parameters" as="xs:string"/>
        <xsl:param name="suffix" as="xs:string"/>
        <xsl:param name="tab" as="xs:integer"/>

        <xsl:value-of select="concat(o:t($tab), cpp:combine_namespaces(($namespaces, $function_name)), o:wrap_bracket($parameters), $suffix)" />
    </xsl:function>

    <!--
        Used to delete an allocated object
        ie delete ${var};
    -->
    <xsl:function name="cpp:delete">
        <xsl:param name="var" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />

        <xsl:value-of select="concat(o:t($tab), 'delete ', $var, cpp:nl())" />
    </xsl:function>

    <!--
        Used to return out of a function
        ie return ${var};
    -->
    <xsl:function name="cpp:return">
        <xsl:param name="var" as="xs:string?" />
        <xsl:param name="tab" as="xs:integer" />

        <xsl:value-of select="concat(o:t($tab), o:join_list(('return', $var), ' '), cpp:nl())" />
    </xsl:function>

    <!--
        Used to combine a list of namespace elements into CPP namespace syntax
        ${namespace_1}::${namespace_2}
    -->
    <xsl:function name="cpp:combine_namespaces">
        <xsl:param name="name_list" as="xs:string*" />
        <xsl:value-of select="o:join_list($name_list, '::')" />
    </xsl:function>

    <!--
        Constructs a new object of the provided type (Combines the list into namespace syntax)
        new ${namespace_1}::${namespace_2}::${function}(${parameters})
    -->
    <xsl:function name="cpp:new_object">
        <xsl:param name="namespace_list" as="xs:string*" />
        <xsl:param name="parameters" as="xs:string" />

        <xsl:value-of select="concat('new ', cpp:invoke_function('', '', cpp:combine_namespaces($namespace_list), $parameters, 0))" />
    </xsl:function>

    <!--
        Declares a for loop.
        for(${first}; ${second}; ${third})${suffix}
    -->
    <xsl:function name="cpp:for">
        <xsl:param name="first" as="xs:string" />
        <xsl:param name="second" as="xs:string" />
        <xsl:param name="third" as="xs:string" />
        <xsl:param name="suffix" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />

        <xsl:variable name="arguments" select="string-join(($first, $second, $third), '; ')" />

        <xsl:value-of select="concat(o:t($tab), 'for', o:wrap_bracket($arguments), $suffix)" />
    </xsl:function>

    <!--
        Declares a for each loop
        for(${first} : ${second})${suffix}
    -->
    <xsl:function name="cpp:for_each">
        <xsl:param name="first" as="xs:string" />
        <xsl:param name="second" as="xs:string" />
        <xsl:param name="suffix" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />

        <xsl:variable name="arguments" select="string-join(($first, $second), ' : ')" />

        <xsl:value-of select="concat(o:t($tab), 'for', o:wrap_bracket($arguments), $suffix)" />
    </xsl:function>

    <!--
        Compare ${val1} against ${val2} using an operator
        ${val1} ${operator} ${val2}
    -->
    <xsl:function name="cpp:compare" as="xs:string">
        <xsl:param name="val1" as="xs:string" />
        <xsl:param name="operator" as="xs:string" />
        <xsl:param name="val2" as="xs:string" />

        <xsl:value-of select="o:join_list(($val1, $operator, $val2), ' ')" />
    </xsl:function>

    <!--
        Compares ${val1} is less than ${val2}
        ${val1} < ${val2}
    -->
    <xsl:function name="cpp:compare_lt" as="xs:string">
        <xsl:param name="val1" as="xs:string" />
        <xsl:param name="val2" as="xs:string" />

        <xsl:value-of select="cpp:compare($val1, o:lt(), $val2)" />
    </xsl:function>

    <!--
        Compares ${val1} is greater than ${val2}
        ie ${val1} > ${val2}
    -->
    <xsl:function name="cpp:compare_gt" as="xs:string">
        <xsl:param name="val1" as="xs:string" />
        <xsl:param name="val2" as="xs:string" />

        <xsl:value-of select="cpp:compare($val1, o:gt(), $val2)" />
    </xsl:function>

    <!--
        produces a '++''
    -->
    <xsl:function name="cpp:plusplus" as="xs:string">
        <xsl:value-of select="'++'" />
    </xsl:function>

    <!--
        Increments a val by 1
        ${val}++
    -->
    <xsl:function name="cpp:increment" as="xs:string">
        <xsl:param name="val" as="xs:string" />
        <xsl:value-of select="concat($val, cpp:plusplus())" />
    </xsl:function>

    <!--
        Gets the value of an array at a position
        ${array}[${index}]
    -->
    <xsl:function name="cpp:array_get" as="xs:string">
        <xsl:param name="array" as="xs:string" />
        <xsl:param name="index" as="xs:string" />

        <xsl:value-of select="concat($array, o:wrap_square($index))" />
    </xsl:function>

    <!--
        static casts a value as a type
        static_cast<${type}>(${value})
    -->
    <xsl:function name="cpp:static_cast" as="xs:string">
        <xsl:param name="type" as="xs:string" />
        <xsl:param name="value" as="xs:string" />

        <xsl:value-of select="cpp:invoke_templated_static_function($type, 'static_cast', $value, '', 0)" />
    </xsl:function>

    <!--
        cast a value as a type
        (${type})(${value})
    -->
    <xsl:function name="cpp:cast" as="xs:string">
        <xsl:param name="type" as="xs:string" />
        <xsl:param name="value" as="xs:string" />

        <xsl:value-of select="concat(o:wrap_bracket($type), o:wrap_bracket($value))" />
    </xsl:function>

    <!--
        Declares a class, handles optional inheritted classes
        class ${class_name} : ${sub_classes}{
    -->
    <xsl:function name="cpp:declare_class" as="xs:string">
        <xsl:param name="class_name" as="xs:string" />
        <xsl:param name="sub_classes" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />

        <xsl:variable name="arguments" select="o:join_list(($class_name, $sub_classes), ' : ')" />
        <xsl:value-of select="concat(o:t($tab), 'class ', $arguments, cpp:scope_start(0))" />
    </xsl:function>

    <!--
        Produces a public access specifier
        public:
    -->
    <xsl:function name="cpp:public" as="xs:string">
        <xsl:param name="tab" as="xs:integer" />
        <xsl:value-of select="concat(o:t($tab), 'public:', o:nl(1))" />
    </xsl:function>
    
    <!--
        Produces a private access specifier
        private:
    -->
    <xsl:function name="cpp:private" as="xs:string">
        <xsl:param name="tab" as="xs:integer" />
        <xsl:value-of select="concat(o:t($tab), 'private:', o:nl(1))" />
    </xsl:function>

    <!--
        Produces a protected access specifier
        protected:
    -->
    <xsl:function name="cpp:protected" as="xs:string">
        <xsl:param name="tab" as="xs:integer" />
        <xsl:value-of select="concat(o:t($tab), 'protected:', o:nl(1))" />
    </xsl:function>

    <!--
        Joins a list of arguments seperating them with commas
        ${args1}, ${args2}, ${args3}
    -->
    <xsl:function name="cpp:join_args" as="xs:string">
        <xsl:param name="args" as="xs:string*" />
        <xsl:value-of select="o:join_list($args, ', ')" />
    </xsl:function>

    <!--
        Used to acces a templated type
        ${class}<${template_type}>
    -->
    <xsl:function name="cpp:templated_type" as="xs:string">
        <xsl:param name="class" as="xs:string" />
        <xsl:param name="template_type" as="xs:string" />
        <xsl:value-of select="concat($class, cpp:wrap_template($template_type))" />
    </xsl:function>

    <!--
        Produces an enum declaration (strongly typed)
        enum class ${label}{
    -->
    <xsl:function name="cpp:enum" as="xs:string">
        <xsl:param name="label" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />
        <xsl:value-of select="concat(o:t($tab), 'enum class ', $label, cpp:scope_start(0))" />
    </xsl:function>

    <!--
        Produces the end of an enum declaration scope
        };
    -->
    <xsl:function name="cpp:enum_end" as="xs:string">
        <xsl:param name="tab" as="xs:integer" />
        <xsl:value-of select="cpp:scope_end($tab)" />
    </xsl:function>

    <!--
        Produces a enum member declaration (strongly typed)
        ${label} = 0,
    -->
    <xsl:function name="cpp:enum_value" as="xs:string">
        <xsl:param name="label" as="xs:string" />
        <xsl:param name="value" as="xs:integer" />
        <xsl:param name="is_last" as="xs:boolean" />
        <xsl:param name="tab" as="xs:integer" />
        <xsl:value-of select="concat(o:t($tab), o:join_list(($label, string($value)), ' = '), if($is_last) then '' else ',', o:nl(1))" />
    </xsl:function>

    <!--
        Produces a switch statement declaration
        switch(${label}){
    -->
    <xsl:function name="cpp:switch" as="xs:string">
        <xsl:param name="label" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />
        <xsl:value-of select="concat(o:t($tab), 'switch', o:wrap_bracket($label), cpp:scope_start(0))" />
    </xsl:function>

    <!--
        Produces an end of a switch statement declaration scope
        };
    -->
    <xsl:function name="cpp:switch_end" as="xs:string">
        <xsl:param name="tab" as="xs:integer" />
        <xsl:value-of select="cpp:scope_end($tab)" />
    </xsl:function>

    <!--
        Produces a switch statement case declaration
        case ${label}:{
    -->
    <xsl:function name="cpp:switch_case" as="xs:string">
        <xsl:param name="label" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />
        <xsl:value-of select="concat(o:t($tab), 'case ', $label, ':', cpp:scope_start(0))" />
    </xsl:function>

     <!--
        Produces an end to a switch statement case declaration scope
        };
    -->
    <xsl:function name="cpp:switch_case_end" as="xs:string">
        <xsl:param name="tab" as="xs:integer" />
        <xsl:value-of select="cpp:scope_end($tab)" />
    </xsl:function>

    <!--
        Produces a break statement
        break;
    -->
    <xsl:function name="cpp:break" as="xs:string">
        <xsl:param name="tab" as="xs:integer" />
        <xsl:value-of select="concat(o:t($tab), 'break', cpp:nl())" />
    </xsl:function>

    <!--
        Produces a switch statement default case declaration
        default:{
    -->
    <xsl:function name="cpp:switch_default" as="xs:string">
        <xsl:param name="tab" as="xs:integer" />
        <xsl:value-of select="concat(o:t($tab), 'default:', cpp:scope_start(0))" />
    </xsl:function>

    <!--
        Produces a an if statement
        if(${condition})${suffix}
    -->
    <xsl:function name="cpp:if" as="xs:string">
        <xsl:param name="condition" as="xs:string" />
        <xsl:param name="suffix" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />
        <xsl:value-of select="concat(o:t($tab), 'if', o:wrap_bracket($condition), $suffix)" />
    </xsl:function>

    <!--
        Produces a try statement
        try{
    -->
    <xsl:function name="cpp:try" as="xs:string">
        <xsl:param name="tab" as="xs:integer" />
        <xsl:value-of select="concat(o:t($tab), 'try', cpp:scope_start(0))" />
    </xsl:function>

    <!--
        Produces a catch statement
        ${prefix}catch(${exception}){
    -->
    <xsl:function name="cpp:catch" as="xs:string">
        <xsl:param name="prefix" as="xs:string" />
        <xsl:param name="exception" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />
        <xsl:value-of select="concat(o:t($tab), $prefix, 'catch', o:wrap_bracket($exception), cpp:scope_start(0))" />
    </xsl:function>

    <!--
        Produces a throw statement
        throw ${exception_type}(${exception_value});
    -->
    <xsl:function name="cpp:throw" as="xs:string">
        <xsl:param name="exception_type" as="xs:string" />
        <xsl:param name="exception_value" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />
        <xsl:value-of select="concat(o:t($tab), 'throw ', $exception_type, o:wrap_bracket($exception_value), cpp:nl())" />
    </xsl:function>

    <!--
        Produces a catch statement
        ${prefix}catch(const ${exception_type}& ${exception_name}){
    -->
    <xsl:function name="cpp:catch_exception" as="xs:string">
        <xsl:param name="prefix" as="xs:string" />
        <xsl:param name="exception_type" as="xs:string" />
        <xsl:param name="exception_name" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />
        <xsl:value-of select="cpp:catch($prefix, cpp:const_ref_var_def($exception_type, $exception_name), $tab)" />
    </xsl:function>

    <!-->
        Produces a negated if statement
        ie. if(!(${val})(){
    -->
    <xsl:function name="cpp:if_not" as="xs:string">
        <xsl:param name="condition" as="xs:string" />
        <xsl:param name="suffix" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />
        <xsl:value-of select="cpp:if(concat('!', o:wrap_bracket($condition)), $suffix, $tab)" />
    </xsl:function>

    <!--
        Opens the scope for a namespace list
        namespace ${namespace_1}{
            namespace ${namespace_2}{
                namespace ${namespace_3}{
    -->
    <xsl:function name="cpp:define_namespaces">
        <xsl:param name="namespaces" as="xs:string*" />

        <xsl:for-each select="$namespaces">
            <xsl:value-of select="cpp:namespace_start(., position() - 1)" />
        </xsl:for-each>
    </xsl:function>

    <!--
        Closes the scope for a namespace list
                }; // ${namespace_3}
            }; // ${namespace_2}
        }; // ${namespace_1}
    -->
    <xsl:function name="cpp:close_namespaces">
        <xsl:param name="namespaces" as="xs:string*" />

        <xsl:for-each select="$namespaces">
            <xsl:sort select="position()" data-type="number" order="descending"/>
            <xsl:value-of select="cpp:namespace_end(., count($namespaces) - position())" />
        </xsl:for-each>
    </xsl:function>

    <!--
        Produces an and statement for an if
    -->
    <xsl:function name="cpp:and" as="xs:string">
        <xsl:value-of select="concat(o:and(), o:and())" />
    </xsl:function>

    <!--
        Produces a or statement for an if
    -->
    <xsl:function name="cpp:or" as="xs:string">
        <xsl:value-of select="'||'" />
    </xsl:function>

    <!--
        Produces a std::cerr statement
    -->
    <xsl:function name="cpp:cerr" as="xs:string">
        <xsl:param name="message" as="xs:string*"/>
        <xsl:param name="tab" as="xs:integer" />

        <xsl:variable name="pipe" select="concat(' ', o:lt(), o:lt(), ' ')" />
        
        <xsl:value-of select="concat(o:t($tab), 'std::cerr', $pipe, o:join_list($message, $pipe), $pipe, 'std::endl', cpp:nl())" />
    </xsl:function>

    <!--
        Produces a std::pair statement
        std::pair<${message_1}, ${message_2}>
    -->
    <xsl:function name="cpp:define_pair" as="xs:string">
        <xsl:param name="type_1" as="xs:string"/>
        <xsl:param name="type_2" as="xs:string"/>

        <xsl:value-of select="concat('std::pair', o:wrap_angle(cpp:join_args(($type_1, $type_2))))" />
    </xsl:function>

    <!--
        Tests to see if a CPP type is a primitive
    -->
    <xsl:function name="cpp:is_primitive_type" as="xs:boolean">
        <xsl:param name="cpp_type" as="xs:string" />

        <xsl:variable name="primitive_types" select="'std::string', 'int', 'char', 'bool', 'double', 'float'" as="xs:string*" />
        <xsl:value-of select="$cpp_type = $primitive_types" />
    </xsl:function>

    <!--
        Gets the place holder arguments for std::bind
        std::placeholders::_1, std::placeholders::_2, std::placeholders::_3
    -->
    <xsl:function name="cpp:get_placeholders" as="xs:string">
        <xsl:param name="count" as="xs:integer" />

        <xsl:variable name="args_list" as="xs:string*">
            <xsl:for-each select="1 to $count">
                <xsl:value-of select="concat('std::placeholders::_', string(.))" />
            </xsl:for-each>
        </xsl:variable>
        <xsl:value-of select="cpp:join_args($args_list)" />
    </xsl:function>
</xsl:stylesheet>