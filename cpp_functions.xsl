<!--
    A set of XSLT2.0 Functions for outputting C++11 code.
-->
<xsl:stylesheet version="2.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:xs="http://www.w3.org/2001/XMLSchema"
    xmlns:o="http://github.com/cdit-ma/re_gen/o"
    xmlns:cpp="http://github.com/cdit-ma/re_gen/cpp"
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
        <xsl:param name="text" as="xs:string*" />

        <xsl:variable name="joined_text" select="o:join_list($text, ' ')" />
        <xsl:value-of select="o:join_list(('/*', $joined_text, '*/'), ' ')" />
    </xsl:function>

    <!--
        Places the text in an inline comment block, useful for returning errors into code
    -->
    <xsl:function name="cpp:warning">
        <xsl:param name="text" as="xs:string*" />
        <xsl:param name="tab" as="xs:integer" />

        <!--<xsl:variable name="warning_suppress">-->
        <xsl:value-of select="o:warning($text)" />
        <xsl:value-of select="concat(o:t($tab), cpp:comment_inline(('RE_GEN Warning:', $text)))" />
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
        Returns a void
    -->
    <xsl:function name="cpp:void">
        <xsl:value-of select="'void'" />
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
        Produces a define guard
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
        Terminates a define guard
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
        <xsl:param name="suffix" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />

        <xsl:value-of select="concat(o:t($tab), o:join_list(($return_type, $function_name), ' '), '(', $parameters, ')', $suffix, o:nl(1))" />
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
        <xsl:param name="suffix" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />

        <xsl:variable name="template_return_type" select="concat('template ', cpp:wrap_template($template_type_name), ' ', $return_type)" />

        <xsl:value-of select="cpp:declare_function($template_return_type, $function_name, $parameters, $suffix, $tab)" />
    </xsl:function>

    <!--
        Produces a forward declare template function declaration
        ie. template<>
        void function<ConcreteType>(int value);
    -->
    <xsl:function name="cpp:declare_templated_function_specialisation">
        <xsl:param name="concrete_template_type" as="xs:string" />
        <xsl:param name="return_type" as="xs:string" />
        <xsl:param name="function_name" as="xs:string" />
        <xsl:param name="parameters" as="xs:string" />
        <xsl:param name="suffix" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />

        <xsl:value-of select="cpp:declare_templated_class_function_specialisation($concrete_template_type, $return_type, '', $function_name, $parameters, $suffix, $tab)" />
    </xsl:function>

    <!--
        Produces a forward declare template function declaration
        ie. template<>
        void class<ConcreteType>::function(int value);
    -->
    <xsl:function name="cpp:declare_templated_class_function_specialisation">
        <xsl:param name="concrete_template_type" as="xs:string" />
        <xsl:param name="return_type" as="xs:string" />
        <xsl:param name="class_name" as="xs:string" />
        <xsl:param name="function_name" as="xs:string" />
        <xsl:param name="parameters" as="xs:string" />
        <xsl:param name="suffix" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />

        <xsl:variable name="template_return_type" select="concat('template ', cpp:wrap_template(''), o:nl(1), o:t($tab), $return_type)" />


        <xsl:variable name="template_function_name">
            <xsl:choose>
                <xsl:when test="$class_name != ''">
                    <xsl:value-of select="concat($class_name, cpp:wrap_template($concrete_template_type), '::', $function_name)" />
                </xsl:when>
                <xsl:otherwise>
                    <xsl:value-of select="concat($function_name, cpp:wrap_template($concrete_template_type))" />
                </xsl:otherwise>
            </xsl:choose>
        </xsl:variable>

        <xsl:value-of select="cpp:declare_function($template_return_type, $template_function_name, $parameters, $suffix, $tab)" />
    </xsl:function>

    <!--
        Calls a templated function
        ie. function<ConcreteType>(int value);
    -->
    <xsl:function name="cpp:invoke_templated_static_function">
        <xsl:param name="concrete_template_type" as="xs:string" />
        <xsl:param name="function_name" as="xs:string" />
        <xsl:param name="parameters" as="xs:string" />
        <xsl:param name="suffix" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />

        <xsl:variable name="template_function_name" select="concat($function_name, cpp:wrap_template($concrete_template_type))" />

        <xsl:value-of select="cpp:invoke_templated_class_static_function($concrete_template_type, '', $function_name, $parameters, $suffix, $tab)" />
    </xsl:function>

    <!--
        Calls a templated function in a class
        ie. class<ConcreteType>::function(args);
    -->
    <xsl:function name="cpp:invoke_templated_class_static_function">
        <xsl:param name="concrete_template_type" as="xs:string" />
        <xsl:param name="class_name" as="xs:string" />
        <xsl:param name="function_name" as="xs:string" />
        <xsl:param name="parameters" as="xs:string" />
        <xsl:param name="suffix" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />

        <xsl:variable name="template_function_name">
            <xsl:choose>
                <xsl:when test="$class_name != ''">
                    <xsl:value-of select="concat($class_name, cpp:wrap_template($concrete_template_type), '::', $function_name)" />
                </xsl:when>
                <xsl:otherwise>
                    <xsl:value-of select="concat($function_name, cpp:wrap_template($concrete_template_type))" />
                </xsl:otherwise>
            </xsl:choose>
        </xsl:variable>

        <xsl:value-of select="cpp:invoke_static_function('', $template_function_name, $parameters, $suffix, $tab)" />
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
        <xsl:param name="suffix" as="xs:string" />

        <!-- Attach the namespace to the function_name -->
        <xsl:variable name="namespaced_function" select="cpp:combine_namespaces(($namespace, $function_name))" />
        <xsl:variable name="function_call" select="concat($namespaced_function, '(', $parameters, ')', $suffix)" />
        
        <xsl:value-of select="o:join_list(($return_type, $function_call), ' ')" />
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
        <xsl:param name="suffix" as="xs:string" />

        <xsl:variable name="template_return_type" select="concat('template ', cpp:wrap_template(''), o:nl(1), $return_type)" />
        <xsl:variable name="template_function_name" select="concat($function_name, cpp:wrap_template($concrete_template_type))" />

        <xsl:value-of select="cpp:define_function($template_return_type, $namespace, $template_function_name, $parameters, $suffix)" />
    </xsl:function>
    
    <!--
        Produces a variable declaration
        ie. std::string message
    -->
    <xsl:function name="cpp:declare_variable">
        <xsl:param name="variable_type" as="xs:string" />
        <xsl:param name="variable_name" as="xs:string" />
        <xsl:param name="suffix" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />
        
        <xsl:value-of select="concat(o:t($tab), o:join_list(($variable_type, $variable_name), ' '), $suffix)" />
    </xsl:function>

    <!--
        Produces a variable definition
        ie. std::string message
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
        ie namespace ${namespace}{
    -->
    <xsl:function name="cpp:namespace_start">
        <xsl:param name="namespace" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />

        <xsl:value-of select="concat(o:t($tab), 'namespace ', $namespace, cpp:scope_start(0))" />
    </xsl:function>

    <!--
        Used to close a namespace
        ie };
    -->
    <xsl:function name="cpp:namespace_end">
        <xsl:param name="namespace" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />

        <xsl:value-of select="concat(o:t($tab), '}; ', cpp:comment($namespace, 0))" />
    </xsl:function>

    <!--
        Used to forward declare a class, handles multiple namespaces
        ie namespace ${namespace}{
            class ${class_name};
        };
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
        ie. ${var}&
    -->
    <xsl:function name="cpp:ref_var">
        <xsl:param name="var" as="xs:string"/>

        <xsl:value-of select="concat(o:and(), $var)" />
    </xsl:function>

    <!--
        Used to define a type as a reference
        ie. ${type}& ${value}
    -->
    <xsl:function name="cpp:ref_var_def">
        <xsl:param name="type" as="xs:string"/>
        <xsl:param name="value" as="xs:string"/>

        <xsl:value-of select="o:join_list((concat($type, o:and()), $value), ' ')" />
    </xsl:function>

    <!--
        Used to define a type as a const reference
        ie const ${type}& ${value}
    -->
    <xsl:function name="cpp:const_ref_var_def">
        <xsl:param name="type" as="xs:string"/>
        <xsl:param name="value" as="xs:string"/>

        <xsl:value-of select="concat('const ', cpp:ref_var_def($type, $value))" />
    </xsl:function>

    <!--
        Used to define a type as a pointer
        ie. ${type}* ${value}
    -->
    <xsl:function name="cpp:pointer_var_def">
        <xsl:param name="type" as="xs:string"/>
        <xsl:param name="value" as="xs:string"/>
        <xsl:value-of select="o:join_list((concat($type, '*'), $value), ' ')" />
    </xsl:function>

    <!--
        Used to dereference a pointed object
        ie. *${var}
    -->
    <xsl:function name="cpp:dereference_var">
        <xsl:param name="var" as="xs:string"/>
        <xsl:value-of select="concat('*', $var)" />
    </xsl:function>

    <!--
        Returns a const auto reference
        ie. const auto&
    -->
    <xsl:function name="cpp:const_ref_auto">
        <xsl:value-of select="cpp:const_ref_var_def(cpp:auto(), '')" />
    </xsl:function>

    <!--
        returns auto
        ie. auto
    -->
    <xsl:function name="cpp:auto">
        <xsl:value-of select="'auto'" />
    </xsl:function>

    <!--
        Used to start a scoped bracketted section
        ie {
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
        ie ;\n
    -->
    <xsl:function name="cpp:nl">
        <xsl:value-of select="concat(';', o:nl(1))" />
    </xsl:function>

    <!--
        Used to invoke a function on an object
        ie. ${obj}${operator}${function_name}(${parameters})
    -->
    <xsl:function name="cpp:invoke_function">
        <xsl:param name="obj" as="xs:string"/>
        <xsl:param name="operator" as="xs:string"/>
        <xsl:param name="function_name" as="xs:string"/>
        <xsl:param name="parameters" as="xs:string"/>
        <xsl:param name="tab" as="xs:integer"/>

        <xsl:if test="$function_name != ''">
            <xsl:value-of select="concat(o:t($tab), o:join_list(($obj, $function_name), $operator), o:wrap_bracket($parameters))" />
        </xsl:if>
    </xsl:function>

    <!--
         Used to invoke a static function
        ie. ${namespaces}::${function_name}(${parameters})${suffix}
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
        <xsl:param name="var" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />

        <xsl:value-of select="concat(o:t($tab), o:join_list(('return', $var), ' '), cpp:nl())" />
    </xsl:function>

    <!--
        Used to combine a list of namespace elements into CPP namespace syntax
        ie. namespace::namespace::function
    -->
    <xsl:function name="cpp:combine_namespaces">
        <xsl:param name="name_list" as="xs:string*" />
        <xsl:value-of select="o:join_list($name_list, '::')" />
    </xsl:function>

    <!--
        Constructs a new object of the provided type (Combines the list into namespace syntax)
        ie. new namespace::class(${parameters})
    -->
    <xsl:function name="cpp:new_object">
        <xsl:param name="name_list" as="xs:string*" />
        <xsl:param name="parameters" as="xs:string" />

        <xsl:value-of select="concat('new ', cpp:invoke_function('', '', cpp:combine_namespaces($name_list), $parameters, 0))" />
    </xsl:function>

    <!--
        Declares a for loop.
        ie. for($first, $second, $third)$suffix
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
        ie. for($first : $second)$suffix
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
    -->
    <xsl:function name="cpp:compare" as="xs:string">
        <xsl:param name="val1" as="xs:string" />
        <xsl:param name="operator" as="xs:string" />
        <xsl:param name="val2" as="xs:string" />

        <xsl:value-of select="o:join_list(($val1, $operator, $val2), ' ')" />
    </xsl:function>

    <!--
        Compares ${val1} is less than ${val2}
        ie ${val1} < ${val2}
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
        produces a ++
    -->
    <xsl:function name="cpp:plusplus" as="xs:string">
        <xsl:value-of select="'++'" />
    </xsl:function>

    <!--
        Increments a val by 1
        ie. ${val}++
    -->
    <xsl:function name="cpp:increment" as="xs:string">
        <xsl:param name="val" as="xs:string" />
        <xsl:value-of select="concat($val, cpp:plusplus())" />
    </xsl:function>

    <!--
        Gets the value of an array at a position
        ie. ${array}[${index}]
    -->
    <xsl:function name="cpp:array_get" as="xs:string">
        <xsl:param name="array" as="xs:string" />
        <xsl:param name="index" as="xs:string" />

        <xsl:value-of select="concat($array, o:wrap_square($index))" />
    </xsl:function>

    <!--
        static casts a value as a type
        ie. static_cast<int>(value)
    -->
    <xsl:function name="cpp:static_cast" as="xs:string">
        <xsl:param name="type" as="xs:string" />
        <xsl:param name="value" as="xs:string" />

        <xsl:value-of select="cpp:invoke_templated_static_function($type, 'static_cast', $value, '', 0)" />
    </xsl:function>

    <!--
        cast a value as a type
        ie. (type)(value)
    -->
    <xsl:function name="cpp:cast" as="xs:string">
        <xsl:param name="type" as="xs:string" />
        <xsl:param name="value" as="xs:string" />

        <xsl:value-of select="concat(o:wrap_bracket($type), o:wrap_bracket($value))" />
    </xsl:function>

    <!--
        Declares a class, handles optional inheritted classes
        ie class ${class_name} : ${sub_classes}
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
        ie public:
    -->
    <xsl:function name="cpp:public" as="xs:string">
        <xsl:param name="tab" as="xs:integer" />
        <xsl:value-of select="concat(o:t($tab), 'public:', o:nl(1))" />
    </xsl:function>
    
    <!--
        Produces a private access specifier
        ie private:
    -->
    <xsl:function name="cpp:private" as="xs:string">
        <xsl:param name="tab" as="xs:integer" />
        <xsl:value-of select="concat(o:t($tab), 'private:', o:nl(1))" />
    </xsl:function>

    <!--
        Produces a protected access specifier
        ie protected:
    -->
    <xsl:function name="cpp:protected" as="xs:string">
        <xsl:param name="tab" as="xs:integer" />
        <xsl:value-of select="concat(o:t($tab), 'protected:', o:nl(1))" />
    </xsl:function>

    <!--
        Joins a list of arguments seperating them with commas
    -->
    <xsl:function name="cpp:join_args" as="xs:string">
        <xsl:param name="args" as="xs:string*" />
        <xsl:value-of select="o:join_list($args, ', ')" />
    </xsl:function>

    <!--
        Used to acces a templated type
        ie ${class}<${template_type}>
    -->
    <xsl:function name="cpp:templated_type" as="xs:string">
        <xsl:param name="class" as="xs:string" />
        <xsl:param name="template_type" as="xs:string" />
        <xsl:value-of select="concat($class, cpp:wrap_template($template_type))" />
    </xsl:function>

    <!--
        Produces an enum declaration (strongly typed)
        enum class ${label} {
    -->
    <xsl:function name="cpp:enum" as="xs:string">
        <xsl:param name="label" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />
        <xsl:value-of select="concat(o:t($tab), 'enum class ', $label, cpp:scope_start(0))" />
    </xsl:function>

    <!--
        Produces a enum member declaration (strongly typed)
        $label = $value,
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
        ie. switch(${label}){
    -->
    <xsl:function name="cpp:switch" as="xs:string">
        <xsl:param name="label" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />
        <xsl:value-of select="concat(o:t($tab), 'switch', o:wrap_bracket($label), cpp:scope_start(0))" />
    </xsl:function>

    <!--
        Produces a switch statement case declaration
        ie. case ${label}:{
    -->
    <xsl:function name="cpp:switch_case" as="xs:string">
        <xsl:param name="label" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />
        <xsl:value-of select="concat(o:t($tab), 'case ', $label, ':', cpp:scope_start(0))" />
    </xsl:function>

    <!--
        Produces a switch statement default case declaration
        ie. default:{
    -->
    <xsl:function name="cpp:switch_default" as="xs:string">
        <xsl:param name="tab" as="xs:integer" />
        <xsl:value-of select="concat(o:t($tab), 'default:', cpp:scope_start(0))" />
    </xsl:function>

    <!--
        Produces a an if statement
        ie. if(${val}(){
    -->
    <xsl:function name="cpp:if" as="xs:string">
        <xsl:param name="condition" as="xs:string" />
        <xsl:param name="suffix" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />
        <xsl:value-of select="concat(o:t($tab), 'if', o:wrap_bracket($condition), $suffix)" />
    </xsl:function>

    <!--
        For (A,B,C)
        produces
        namespace A{
            namespace B{
                namespace C{
    -->
    <xsl:function name="cpp:define_namespaces">
        <xsl:param name="namespaces" as="xs:string*" />

        <xsl:for-each select="$namespaces">
            <xsl:value-of select="cpp:namespace_start(., position() - 1)" />
        </xsl:for-each>
    </xsl:function>

    <!--
        For (A,B,C)
        produces
                } //C
            } //B
        } //A
    -->

    <xsl:function name="cpp:close_namespaces">
        <xsl:param name="namespaces" as="xs:string*" />

        <xsl:for-each select="$namespaces">
            <xsl:sort select="position()" data-type="number" order="descending"/>
            <xsl:value-of select="cpp:namespace_end(., count($namespaces) - position())" />
        </xsl:for-each>
    </xsl:function>

    <!-->
        Produces a an if statement
        ie. if(!(${val})(){
    -->
    <xsl:function name="cpp:if_not" as="xs:string">
        <xsl:param name="condition" as="xs:string" />
        <xsl:param name="suffix" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />
        <xsl:value-of select="cpp:if(concat('!', o:wrap_bracket($condition)), $suffix, $tab)" />
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
</xsl:stylesheet>