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
        <xsl:param name="text" as="xs:string*" />
        <xsl:param name="tab" as="xs:integer" />

        <xsl:variable name="joined_text" select="o:join_list($text, ' ')" />

        <xsl:for-each select="tokenize($joined_text, '\n\r?')[.]">
            <xsl:value-of select="concat(o:t($tab), '// ', normalize-space(.), o:nl(1))" />
        </xsl:for-each>
    </xsl:function>


    <!--
        Places the text in an inline comment block, useful for returning errors into code
    -->
    <xsl:function name="cpp:comment_inline">
        <xsl:param name="text" as="xs:string" />
        <xsl:value-of select="o:join_list(('/*', $text, '*/'), ' ')" />
    </xsl:function>

    <!--
        Adds a -> symbol
    -->
    <xsl:function name="cpp:arrow">
        <xsl:value-of select="concat('-', o:gt())" />
    </xsl:function>

    <!--
        Adds a . symbol
    -->
    <xsl:function name="cpp:dot">
        <xsl:value-of select="'.'" />
    </xsl:function>

    <!--
        Wraps a type in template brackets, handles nested type semantics
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

        <xsl:value-of select="concat('#include ', o:wrap_angle($library_path), o:nl(1))" />
    </xsl:function>

    <!--
        Used for including local headers
        #include "${library_path}"
    -->
    <xsl:function name="cpp:include_local_header">
        <xsl:param name="library_path" as="xs:string" />

        <xsl:value-of select="concat('#include ', o:wrap_dblquote($library_path), o:nl(1))" />
    </xsl:function>

    <!--
        Used for header files
        #ifndef ${CLASS_NAME}_H
        #define ${CLASS_NAME}_H
    -->
    <xsl:function name="cpp:define_guard_start">
        <xsl:param name="class_name"/>

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
        <xsl:param name="class_name" as="xs:string" />

        <xsl:variable name="class_name_uc" select="upper-case(concat($class_name, '_h'))"  />
        <xsl:value-of select="concat('#endif ', cpp:comment($class_name_uc, 0))" />
    </xsl:function>

    <!--
        Produces a function declaration
        ie. void function(int value);
    -->
    <xsl:function name="cpp:declare_function">
        <xsl:param name="return_type" as="xs:string" />
        <xsl:param name="function_name" as="xs:string" />
        <xsl:param name="parameters" as="xs:string" />
        <xsl:param name="prefix" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />

        <xsl:value-of select="concat(o:t($tab), $return_type, ' ', $function_name, '(', $parameters, ')', $prefix, o:nl(1))" />
    </xsl:function>

     <!--
        Produces a templated function declaration
        ie. template <class T> void function(int value);
    -->
    <xsl:function name="cpp:declare_templated_function">
        <xsl:param name="template_type_name" as="xs:string" />
        <xsl:param name="return_type" as="xs:string" />
        <xsl:param name="function_name" as="xs:string" />
        <xsl:param name="parameters" as="xs:string" />
        <xsl:param name="prefix" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />

        <xsl:variable name="template_return_type" select="concat('template ', cpp:wrap_template($template_type_name), ' ', $return_type)" />

        <xsl:value-of select="cpp:declare_function($template_return_type, $function_name, $parameters, $prefix, $tab)" />
    </xsl:function>

    <!--
        Produces a forward declare template function declaration
        ie. template<> void function<ConcreteType>(int value);
    -->
    <xsl:function name="cpp:declare_templated_function_specialisation">
        <xsl:param name="concrete_template_type" as="xs:string" />
        <xsl:param name="return_type" as="xs:string" />
        <xsl:param name="function_name" as="xs:string" />
        <xsl:param name="parameters" as="xs:string" />
        <xsl:param name="prefix" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />

        <xsl:variable name="template_return_type" select="concat('template ', cpp:wrap_template(''), ' ', $return_type)" />
        <xsl:variable name="template_function_name" select="concat($function_name, cpp:wrap_template($concrete_template_type))" />

        <xsl:value-of select="cpp:declare_function($template_return_type, $template_function_name, $parameters, $prefix, $tab)" />
    </xsl:function>

    <!--
        Produces a function definition
        ie. void namespace::function(int value)
    -->
    <xsl:function name="cpp:define_function">
        <xsl:param name="return_type" as="xs:string" />
        <xsl:param name="namespace" as="xs:string" />
        <xsl:param name="function_name" as="xs:string" />
        <xsl:param name="parameters" as="xs:string" />
        <xsl:param name="prefix" as="xs:string" />

        <!-- Attach the namespace to the function_name -->
        <xsl:variable name="namespaced_function" select="cpp:combine_namespaces(($namespace, $function_name))" />

        <xsl:value-of select="concat($return_type, ' ', $namespaced_function, '(', $parameters, ')', $prefix)" />
    </xsl:function>

    <!--
        Produces a forward declare template function declaration
        ie. template<> void function<ConcreteType>(int value);
    -->
    <xsl:function name="cpp:define_templated_function_specialisation">
        <xsl:param name="concrete_template_type" as="xs:string" />
        <xsl:param name="return_type" as="xs:string" />
        <xsl:param name="namespace" as="xs:string" />
        <xsl:param name="function_name" as="xs:string" />
        <xsl:param name="parameters" as="xs:string" />
        <xsl:param name="prefix" as="xs:string" />

        <xsl:variable name="template_return_type" select="concat('template ', cpp:wrap_template(''), o:nl(1), $return_type)" />
        <xsl:variable name="template_function_name" select="concat($function_name, cpp:wrap_template($concrete_template_type))" />

        <xsl:value-of select="cpp:define_function($template_return_type, $namespace, $template_function_name, $parameters, $prefix)" />
    </xsl:function>
    
    <!--
        Produces a variable declaration
        ie. std::string message
    -->
    <xsl:function name="cpp:declare_variable">
        <xsl:param name="variable_type" as="xs:string" />
        <xsl:param name="variable_name" as="xs:string" />
        <xsl:param name="prefix" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />
        
        <xsl:value-of select="concat(o:t($tab), o:join_list(($variable_type, $variable_name), ' '), $prefix)" />
    </xsl:function>


    <!--
        Produces a variable definition
        ie. std::string message
    -->
    <xsl:function name="cpp:define_variable">
        <xsl:param name="variable_type" as="xs:string" />
        <xsl:param name="variable_name" as="xs:string" />
        <xsl:param name="value" as="xs:string" />
        <xsl:param name="prefix" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />

        <xsl:variable name="variable_decl" select="o:join_list(($variable_type, $variable_name), ' ')" />
        

        <xsl:value-of select="cpp:declare_variable($variable_type, $variable_name, o:join_list((' = ', $value, $prefix),''), $tab) " />
    </xsl:function>

    <!--
        Wraps a type as a std::shared_ptr
        ie. std::shared_ptr<std::string>
    -->
    <xsl:function name="cpp:shared_ptr">
        <xsl:param name="type" as="xs:string" />
        <xsl:value-of select="concat('std::shared_ptr', cpp:wrap_template($type))" />
    </xsl:function>

    <!--
        Wraps a type as a std::weak_ptr
        ie. std::weak_ptr<std::string>
    -->
    <xsl:function name="cpp:weak_ptr">
        <xsl:param name="type" as="xs:string" />
        <xsl:value-of select="concat('std::weak_ptr', cpp:wrap_template($type))" />
    </xsl:function>

    <!--
        Calls std::make_shared to construct a shared pointer
        ie. std::make_shared<${type_name}>(${args})
    -->
    <xsl:function name="cpp:make_shared_ptr">
        <xsl:param name="type" as="xs:string" />
        <xsl:param name="args" as="xs:string" />
        <xsl:value-of select="concat('std::make_shared', cpp:wrap_template($type), o:wrap_bracket($args))" />
    </xsl:function>


    <!--
        Used to open a namespace
        ie namespace ${namespace} {
    -->
    <xsl:function name="cpp:namespace_start">
        <xsl:param name="namespace" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />

        <xsl:value-of select="concat(o:t($tab), 'namespace ', $namespace, cpp:scope_start(0))" />
    </xsl:function>

    <!--
        Used to close a namespace
        ie }
    -->
    <xsl:function name="cpp:namespace_end">
        <xsl:param name="namespace" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />

        <xsl:value-of select="concat(o:t($tab), '}; ', cpp:comment($namespace, 0))" />
    </xsl:function>


    <!--
        Used to forward declare a class
        ie namespace ${namespace}{
            class ${class_name};
        };
    -->
    <xsl:function name="cpp:forward_declare_class" as="xs:string*">
        <xsl:param name="namespaces" as="xs:string*" />
        <xsl:param name="class_name" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />

        <xsl:variable name="pruned_namespaces" as="xs:string*" select="o:prune_list($namespaces)" />

        <xsl:for-each select="$pruned_namespaces">
            <xsl:value-of select="cpp:namespace_start(., $tab + position() -1)" />
        </xsl:for-each>

        <xsl:value-of select="concat(o:t($tab + count($pruned_namespaces)), 'class ', $class_name, ';', o:nl(1))" />

        <xsl:for-each select="$pruned_namespaces">
            <xsl:value-of select="cpp:namespace_end(., $tab + position() -1)" />
        </xsl:for-each>
    </xsl:function>
    
    <!--
        Used to convert a type into a reference
    -->
    <xsl:function name="cpp:ref_var">
        <xsl:param name="var" as="xs:string"/>

        <xsl:value-of select="concat(o:and(), $var)" />
    </xsl:function>

    <!--
        Used to convert a type into a reference
    -->
    <xsl:function name="cpp:ref_var_def">
        <xsl:param name="type" as="xs:string"/>
        <xsl:param name="value" as="xs:string"/>

        <xsl:value-of select="o:join_list((concat($type, o:and()), $value), ' ')" />
    </xsl:function>

    <!--
        Used to convert a type into a pointer
    -->
    <xsl:function name="cpp:pointer_var_def">
        <xsl:param name="type" as="xs:string"/>
        <xsl:param name="value" as="xs:string"/>
        <xsl:value-of select="o:join_list((concat($type, '*'), $value), ' ')" />
    </xsl:function>

    <!--
        Used to convert a type into a const reference
    -->
    <xsl:function name="cpp:const_ref_var_def">
        <xsl:param name="type" as="xs:string"/>
        <xsl:param name="value" as="xs:string"/>

        <xsl:value-of select="concat('const ', cpp:ref_var_def($type, $value))" />
    </xsl:function>

    <!--
        Returns a const auto&
    -->
    <xsl:function name="cpp:const_ref_auto">
        <xsl:value-of select="cpp:const_ref_var_def(cpp:auto(), '')" />
    </xsl:function>

    <!--
        Returns a auto
    -->
    <xsl:function name="cpp:auto">
        <xsl:value-of select="'auto'" />
    </xsl:function>

    <!--
        Used to start a scoped bracketted section
        ie }
    -->
    <xsl:function name="cpp:scope_start">
        <xsl:param name="tab" as="xs:integer" />

        <xsl:value-of select="concat(o:t($tab), '{', o:nl(1))" />
    </xsl:function>


    <!--
        Used to close a scoped bracketted section
        ie }
    -->
    <xsl:function name="cpp:scope_end">
        <xsl:param name="tab" as="xs:integer" />

        <xsl:value-of select="concat(o:t($tab), '}', cpp:nl())" />
    </xsl:function>

    <!--
        Used to end a command
    -->
    <xsl:function name="cpp:nl">
        <xsl:value-of select="concat(';', o:nl(1))" />
    </xsl:function>

    <!--
        Used to call a function on an object using a ref
        ie. ${obj}${operator}(${parameters})
    -->
    <xsl:function name="cpp:invoke_function">
        <xsl:param name="obj" as="xs:string"/>
        <xsl:param name="operator" as="xs:string"/>
        <xsl:param name="function_name" as="xs:string"/>
        <xsl:param name="parameters" as="xs:string"/>

        <xsl:value-of select="concat(o:join_list(($obj, $function_name), $operator), o:wrap_bracket($parameters))" />
    </xsl:function>

    <!--
        Used to delete an allocataed object
        ie }
    -->
    <xsl:function name="cpp:delete">
        <xsl:param name="var" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />

        <xsl:value-of select="concat(o:t($tab), 'delete ', $var, cpp:nl())" />
    </xsl:function>

    <!--
        Used to delete an allocataed object
        ie }
    -->
    <xsl:function name="cpp:return">
        <xsl:param name="var" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />

        <xsl:value-of select="concat(o:t($tab), 'return ', $var, cpp:nl())" />
    </xsl:function>

    <!--
        Produces a function definition
        ie. void namespace::function(int value)
    -->
    <xsl:function name="cpp:combine_namespaces">
        <xsl:param name="name_list" as="xs:string*" />
        <xsl:value-of select="o:join_list($name_list, '::')" />
    </xsl:function>

    <!--
        Produces a function definition
        ie. void namespace::function(int value)
    -->
    <xsl:function name="cpp:new_object">
        <xsl:param name="name_list" as="xs:string*" />
        <xsl:param name="parameters" as="xs:string" />

        <xsl:value-of select="concat('new ', cpp:invoke_function('', '', o:join_list($name_list, '::'), $parameters))" />
    </xsl:function>

    <xsl:function name="cpp:for">
        <xsl:param name="first" as="xs:string" />
        <xsl:param name="second" as="xs:string" />
        <xsl:param name="third" as="xs:string" />
        <xsl:param name="prefix" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />

        <xsl:variable name="arguments" select="string-join(($first, $second, $third), '; ')" />

        <xsl:value-of select="concat(o:t($tab), 'for', o:wrap_bracket($arguments), $prefix)" />
    </xsl:function>

    <xsl:function name="cpp:for_each">
        <xsl:param name="first" as="xs:string" />
        <xsl:param name="second" as="xs:string" />
        <xsl:param name="prefix" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />

        <xsl:variable name="arguments" select="string-join(($first, $second), ' : ')" />

        <xsl:value-of select="concat(o:t($tab), 'for', o:wrap_bracket($arguments), $prefix)" />
    </xsl:function>

    <xsl:function name="cpp:compare" as="xs:string">
        <xsl:param name="val1" as="xs:string" />
        <xsl:param name="operator" as="xs:string" />
        <xsl:param name="val2" as="xs:string" />

        <xsl:value-of select="o:join_list(($val1, $operator, $val2), ' ')" />
    </xsl:function>

    <xsl:function name="cpp:plusplus" as="xs:string">
        <xsl:value-of select="'++'" />
    </xsl:function>

    <xsl:function name="cpp:increment" as="xs:string">
        <xsl:param name="val" as="xs:string" />
        <xsl:value-of select="concat($val, cpp:plusplus())" />
    </xsl:function>

    

    <xsl:function name="cpp:compare_lt" as="xs:string">
        <xsl:param name="val1" as="xs:string" />
        <xsl:param name="val2" as="xs:string" />

        <xsl:value-of select="cpp:compare($val1, o:lt(), $val2)" />
    </xsl:function>

    <xsl:function name="cpp:compare_gt" as="xs:string">
        <xsl:param name="val1" as="xs:string" />
        <xsl:param name="val2" as="xs:string" />

        <xsl:value-of select="cpp:compare($val1, o:gt(), $val2)" />
    </xsl:function>

    <xsl:function name="cpp:array_get" as="xs:string">
        <xsl:param name="array" as="xs:string" />
        <xsl:param name="index" as="xs:string" />

        <xsl:value-of select="concat($array, o:wrap_square($index))" />
    </xsl:function>

    <xsl:function name="cpp:static_cast" as="xs:string">
        <xsl:param name="type" as="xs:string" />
        <xsl:param name="value" as="xs:string" />

        <xsl:value-of select="concat('static_cast', o:wrap_angle($type), o:wrap_bracket($value))" />
    </xsl:function>

    <xsl:function name="cpp:declare_class" as="xs:string">
        <xsl:param name="class_name" as="xs:string" />
        <xsl:param name="sub_classes" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />

        <xsl:variable name="arguments" select="o:join_list(($class_name, $sub_classes), ' : ')" />
        <xsl:value-of select="concat(o:t($tab), 'class ', $arguments, cpp:scope_start(0))" />
    </xsl:function>

    <xsl:function name="cpp:public" as="xs:string">
        <xsl:param name="tab" as="xs:integer" />
        <xsl:value-of select="concat(o:t($tab), 'public:', o:nl(1))" />
    </xsl:function>
    
    <xsl:function name="cpp:private" as="xs:string">
        <xsl:param name="tab" as="xs:integer" />
        <xsl:value-of select="concat(o:t($tab), 'private:', o:nl(1))" />
    </xsl:function>

    <xsl:function name="cpp:protected" as="xs:string">
        <xsl:param name="tab" as="xs:integer" />
        <xsl:value-of select="concat(o:t($tab), 'protected:', o:nl(1))" />
    </xsl:function>

   
</xsl:stylesheet>