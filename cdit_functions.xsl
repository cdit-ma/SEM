<!-- Functions for cpp syntax output -->
<xsl:stylesheet version="2.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:xs="http://www.w3.org/2001/XMLSchema"
    xmlns:o="http://github.com/cdit-ma/re_gen/o"
    xmlns:graphml="http://github.com/cdit-ma/re_gen/graphml"
    xmlns:cdit="http://github.com/cdit-ma/re_gen/cdit"
    xmlns:cpp="http://github.com/cdit-ma/re_gen/cpp"
    xmlns:proto="http://github.com/cdit-ma/re_gen/proto"
    xmlns:idl="http://github.com/cdit-ma/re_gen/idl"
    xmlns:cmake="http://github.com/cdit-ma/re_gen/cmake"
    >

    <!--
        Gets the path to the aggregate folder
    -->
    <xsl:function name="cdit:get_datatype_path" as="xs:string">
        <xsl:param name="middleware" as="xs:string" />
        <xsl:param name="aggregate" as="element()" />
        <xsl:param name="file" as="xs:string" />

        <xsl:variable name="aggregate_namespace" select="lower-case(graphml:get_namespace($aggregate))" />
        <xsl:variable name="aggregate_label" select="lower-case(graphml:get_label($aggregate))" />

         <xsl:value-of select="o:join_paths((lower-case($middleware), $aggregate_namespace, $aggregate_label, $file))" />
    </xsl:function>
    
    <!--
        Gets the name of the middleware_generated_header_name
    -->
    <xsl:function name="cdit:get_middleware_generated_header_name" as="xs:string">
        <xsl:param name="aggregate" />
        <xsl:param name="middleware" as="xs:string" />

        <xsl:variable name="middleware_extension">
            <xsl:choose>
                <xsl:when test="$middleware = 'rti'">
                    <xsl:value-of select="'.hpp'" />
                </xsl:when>
                <xsl:when test="$middleware = 'ospl'">
                    <xsl:value-of select="'_DCPS.hpp'" />
                </xsl:when>
                <xsl:when test="$middleware = 'proto'">
                    <xsl:value-of select="'.pb.h'" />
                </xsl:when>
                <xsl:otherwise>
                    <xsl:value-of select="o:warning(concat('Middleware ', $middleware, ' not implemented'))" />
                </xsl:otherwise>
            </xsl:choose>
        </xsl:variable>
        <xsl:value-of select="concat(lower-case(graphml:get_label($aggregate)), $middleware_extension)" />
    </xsl:function>

    <xsl:function name="cdit:middleware_requires_idl_file" as="xs:boolean">
        <xsl:param name="middleware" as="xs:string"/>
        <xsl:variable name="middleware_lc" select="lower-case($middleware)" />
        <xsl:value-of select="$middleware_lc = 'rti' or $middleware_lc = 'ospl'" />
    </xsl:function>

    <xsl:function name="cdit:middleware_requires_proto_file" as="xs:boolean">
        <xsl:param name="middleware" as="xs:string"/>
        <xsl:variable name="middleware_lc" select="lower-case($middleware)" />
        <xsl:value-of select="$middleware_lc = 'proto'" />
    </xsl:function>



    <!--
        Gets the name of the middleware_generated_header_name
    -->
    <xsl:function name="cdit:middleware_uses_ref_setter" as="xs:boolean">
        <xsl:param name="middleware" as="xs:string" />
        
        <xsl:value-of select="$middleware = 'rti' or $middleware = 'ospl' or cdit:middleware_uses_protobuf($middleware)" />
    </xsl:function>

    <xsl:function name="cdit:get_middleware_variable_syntax" as="xs:string">
        <xsl:param name="node" as="element()" />
        <xsl:param name="middleware" as="xs:string" />

        <xsl:variable name="label" select="graphml:get_label($node)" />

        <xsl:choose>
            <xsl:when test="$middleware = 'rti' or $middleware = 'ospl'">
                <!-- DDS uses exact case -->
                <xsl:value-of select="$label" />
            </xsl:when>
            <xsl:when test="$middleware = 'base'">
                <!-- Base uses exact case -->
                <xsl:value-of select="$label" />
            </xsl:when>
            <xsl:when test="cdit:middleware_uses_protobuf($middleware)">
                <!-- Protobuf uses lowercase -->
                <xsl:value-of select="lower-case($label)" />
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="cpp:comment_inline(concat('Middleware ', $middleware, ' not implemented'))" />
            </xsl:otherwise>
        </xsl:choose>
    </xsl:function>

    <xsl:function name="cdit:middleware_uses_protobuf" as="xs:boolean">
        <xsl:param name="middleware" as="xs:string" />
        <xsl:value-of select="$middleware='qpid' or $middleware='zmq' or $middleware='proto'" />
    </xsl:function>

    <xsl:function name="cdit:get_middleware_namespace" as="xs:string">
        <xsl:param name="middleware" as="xs:string" />
        <xsl:value-of select="lower-case($middleware)" />
    </xsl:function>

    <xsl:function name="cdit:invoke_middleware_get_function" as="xs:string">
        <xsl:param name="obj" as="xs:string"/>
        <xsl:param name="operator" as="xs:string"/>
        <xsl:param name="node" as="element()" />
        <xsl:param name="middleware" as="xs:string" />

        <xsl:variable name="variable_syntax" select="cdit:get_middleware_variable_syntax($node, $middleware)" />
        <xsl:variable name="function_name">
            <xsl:choose>
                <xsl:when test="$middleware = 'rti' or $middleware = 'ospl'">
                    <!-- DDS implementations use get/set via accessors -->
                    <xsl:value-of select="$variable_syntax" />
                </xsl:when>
                <xsl:when test="$middleware = 'base'">
                    <!-- Base uses both get/set functions both via accessors and functions -->
                    <xsl:value-of select="concat('get_', $variable_syntax)" />
                </xsl:when>
                <xsl:when test="$middleware = 'proto'">
                    <!-- Protobuf offers a const ref getter -->
                    <xsl:value-of select="lower-case($variable_syntax)" />
                </xsl:when>
                <xsl:otherwise>
                    <xsl:value-of select="cpp:comment_inline(concat('Middleware ', $middleware, ' not implemented'))" />
                </xsl:otherwise>
            </xsl:choose>
        </xsl:variable>
        <xsl:value-of select="cpp:invoke_function($obj, $operator, $function_name, '', 0)" />
    </xsl:function>

    <xsl:function name="cdit:invoke_middleware_add_vector_function" as="xs:string">
        <xsl:param name="obj" as="xs:string"/>
        <xsl:param name="operator" as="xs:string"/>
        <xsl:param name="node" as="element()" />
        <xsl:param name="middleware" as="xs:string" />
        <xsl:param name="value" as="xs:string" />

        <xsl:variable name="node_kind" select="graphml:get_kind($node)" />
        <xsl:variable name="variable_syntax" select="cdit:get_middleware_variable_syntax($node, $middleware)" />

        <xsl:variable name="vector_child" select="graphml:get_vector_child($node)" />
        <xsl:variable name="vector_kind" select="graphml:get_kind($vector_child)" />

        <xsl:variable name="function_value">
            <xsl:choose>
                <xsl:when test="$middleware = 'base' and $vector_kind = 'AggregateInstance'">
                    <xsl:value-of select="cpp:dereference_var($value)" />
                </xsl:when>
                <xsl:otherwise>
                    <xsl:value-of select="$value" />
                </xsl:otherwise>
            </xsl:choose>
        </xsl:variable>
        <xsl:variable name="function_name">
            <xsl:choose>
                <xsl:when test="$middleware = 'rti' or $middleware = 'ospl'">
                    <!-- DDS implementations use set via accessors -->
                    <xsl:value-of select="$variable_syntax" />
                </xsl:when>
                <xsl:when test="$middleware = 'base'">
                    <xsl:value-of select="concat(cpp:invoke_function('', '', $variable_syntax, '', 0), cpp:dot(), 'emplace_back')" />
                </xsl:when>
                <xsl:when test="$middleware = 'proto'">
                    <xsl:choose>
                        <xsl:when test="$vector_kind = 'AggregateInstance'">
                            <!-- Vectors with Objects in it can use a mutable_swap function -->
                            <xsl:value-of select="concat(cpp:invoke_function('', '', concat('add_', $variable_syntax), '', 0), cpp:arrow(), 'Swap')" />
                        </xsl:when>
                        <xsl:when test="$vector_kind = 'Member'">
                            <!-- Protobuf uses swap for Instances -->
                            <xsl:value-of select="concat('add_', $variable_syntax)" />
                        </xsl:when>
                        <xsl:otherwise>
                            <xsl:value-of select="cpp:comment_inline(concat('Protobuf doesnt support vector with child kind ', o:wrap_quote($vector_kind)))" />
                        </xsl:otherwise>
                    </xsl:choose>
                </xsl:when>
                <xsl:otherwise>
                    <xsl:value-of select="cpp:comment_inline(concat('Middleware ', $middleware, ' not implemented'))" />
                </xsl:otherwise>
            </xsl:choose>
        </xsl:variable>

        <xsl:value-of select="cpp:invoke_function($obj, $operator, $function_name, $function_value, 0)" />
    </xsl:function>
    

    <xsl:function name="cdit:invoke_middleware_set_function" as="xs:string">
        <xsl:param name="obj" as="xs:string"/>
        <xsl:param name="operator" as="xs:string"/>
        <xsl:param name="node" as="element()" />
        <xsl:param name="middleware" as="xs:string" />
        <xsl:param name="value" as="xs:string" />

        <xsl:variable name="node_kind" select="graphml:get_kind($node)" />
        
        <xsl:variable name="variable_syntax" select="cdit:get_middleware_variable_syntax($node, $middleware)" />

        <xsl:variable name="function_name">
            <xsl:choose>
                <xsl:when test="$middleware = 'rti' or $middleware = 'ospl'">
                    <!-- DDS implementations use set via accessors -->
                    <xsl:value-of select="$variable_syntax" />
                </xsl:when>
                <xsl:when test="$middleware = 'base'">
                    <!-- Base uses both get/set functions both via accessors and functions -->
                    <xsl:value-of select="concat('set_', $variable_syntax)" />
                </xsl:when>
                <xsl:when test="$middleware = 'proto'">
                    <xsl:choose>
                        <xsl:when test="$node_kind = 'AggregateInstance'">
                            <!-- Protobuf uses swap for Instances -->
                            <xsl:value-of select="concat(cpp:invoke_function('', '', concat('mutable_', $variable_syntax), '', 0), cpp:arrow(), 'Swap')" />
                        </xsl:when>
                        <xsl:when test="$node_kind = 'Member' or $node_kind = 'EnumInstance'">
                            <!-- Protobuf implementations use set via functions -->
                            <xsl:value-of select="concat('set_', $variable_syntax)" />
                        </xsl:when>
                        <xsl:otherwise>
                            <xsl:value-of select="cpp:comment_inline(concat('Kind ', $node_kind, ' not implemented'))" />
                        </xsl:otherwise>
                    </xsl:choose>
                </xsl:when>
                <xsl:otherwise>
                    <xsl:value-of select="cpp:comment_inline(concat('Middleware ', $middleware, ' not implemented'))" />
                </xsl:otherwise>
            </xsl:choose>
        </xsl:variable>

        <xsl:choose>
            <xsl:when test="$middleware = 'rti' or $middleware = 'ospl'">
                <!-- DDS implementations use set via accessors -->
                <xsl:value-of select="concat(cpp:invoke_function($obj, $operator, $function_name, '', 0), ' = ', $value)" />
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="cpp:invoke_function($obj, $operator, $function_name, $value, 0)" />
            </xsl:otherwise>
        </xsl:choose>
    </xsl:function>

    <xsl:function name="cpp:get_qualified_type" as="xs:string">
        <xsl:param name="node" as="element()?" />
        <xsl:variable name="kind" select="graphml:get_kind($node)" />
        
        <xsl:choose>    
            <xsl:when test="$kind = 'Member' or $kind = 'Attribute'">
                <xsl:value-of select="cpp:get_member_type($node)" />
            </xsl:when>
            <xsl:when test="$kind = 'Variable'">
                <xsl:variable name="child" select="graphml:get_child_node($node, 1)" />
                <xsl:choose>
                    <xsl:when test="$child">
                        <xsl:value-of select="cpp:get_qualified_type($child)" />
                    </xsl:when>
                    <xsl:otherwise>
                        <xsl:value-of select="cpp:get_member_type($node)" />
                    </xsl:otherwise>
                </xsl:choose> 
            </xsl:when>
            <xsl:when test="$kind = 'EnumInstance'">
                <xsl:value-of select="cpp:get_enum_qualified_type($node, 'base')" />
            </xsl:when>
            <xsl:when test="$kind = 'AggregateInstance' or $kind = 'Aggregate'">
                <xsl:value-of select="cpp:get_aggregate_qualified_type($node, 'base')" />
            </xsl:when>
            <xsl:when test="$kind = 'Vector'">
                <xsl:value-of select="cpp:get_vector_qualified_type($node)" />
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="o:warning(('cpp:get_qualified_type()', 'Node Kind:', o:wrap_quote($kind), 'Not Implemented'))" />
            </xsl:otherwise>
        </xsl:choose>
    </xsl:function>

    <xsl:function name="cpp:get_member_type" as="xs:string">
        <xsl:param name="member" as="element()" />

        <xsl:variable name="type" select="graphml:get_type($member)" />
        <xsl:value-of select="cpp:get_primitive_type($type)" />
    </xsl:function>

    <xsl:function name="cpp:get_enum_qualified_type" as="xs:string">
        <xsl:param name="enum" as="element()" />
        <xsl:param name="middleware" as="xs:string" />

        <xsl:variable name="enum_definition" select="graphml:get_definition($enum)" />
        <xsl:variable name="namespace" select="graphml:get_namespace($enum_definition)" />
        <xsl:variable name="label" select="o:title_case(graphml:get_label($enum_definition))" />

        <xsl:variable name="extra_namespace" as="xs:string*">
            <xsl:if test="$middleware = 'base'">
                <!-- DDS implementations use set via accessors -->
                <xsl:value-of select="'Base'" />
            </xsl:if>
        </xsl:variable>

        <xsl:value-of select="cpp:combine_namespaces(($extra_namespace, $namespace, $label))" />
    </xsl:function>

    <xsl:function name="cpp:get_vector_qualified_type" as="xs:string">
        <xsl:param name="vector" as="element()" />

        <xsl:variable name="vector_child" select="graphml:get_vector_child($vector)" />
        <xsl:variable name="child_type" select="cpp:get_qualified_type($vector_child)" />
        <xsl:if test="$child_type != ''">
            <xsl:value-of select="concat('std::vector', o:wrap_angle($child_type))" />
        </xsl:if>
    </xsl:function>

    <xsl:function name="cdit:variablize_value" as="xs:string">
        <xsl:param name="variable" as="xs:string" />
        <xsl:value-of select="if($variable != '') then concat($variable, '_') else ''" />
    </xsl:function>

    <xsl:function name="cdit:get_variable_label" as="xs:string">
        <xsl:param name="variable" as="element()" />
        <xsl:value-of select="cdit:variablize_value(graphml:get_label($variable))" />
    </xsl:function>

    <xsl:function name="cpp:get_aggregate_type_name" as="xs:string">
        <xsl:param name="aggregate_inst" as="element()" />
        <xsl:variable name="aggregate" select="graphml:get_definition($aggregate_inst)" />
        <xsl:value-of select="o:title_case(graphml:get_label($aggregate))" />
    </xsl:function>

    <xsl:function name="cpp:get_aggregate_qualified_type" as="xs:string">
        <xsl:param name="aggregate_inst" as="element()" />
        <xsl:param name="middleware" as="xs:string" />

        <xsl:variable name="aggregate" select="graphml:get_definition($aggregate_inst)" />

        <xsl:variable name="aggregate_namespace" select="graphml:get_namespace($aggregate)" />
        <xsl:variable name="aggregate_label" select="cpp:get_aggregate_type_name($aggregate)" />
        
        <xsl:variable name="extra_namespace" as="xs:string*">
            <xsl:if test="$middleware = 'base'">
                <!-- DDS implementations use set via accessors -->
                <xsl:value-of select="'Base'" />
            </xsl:if>
        </xsl:variable>
        <xsl:value-of select="cpp:combine_namespaces(($extra_namespace, $aggregate_namespace, $aggregate_label))" />
    </xsl:function>
    
    <!-- Converts from the Aggregate Types into primitive CPP types -->
    <xsl:function name="cpp:get_primitive_type" as="xs:string">
        <xsl:param name="type" as="xs:string"  />

        <xsl:choose>
            <xsl:when test="$type = 'String'">
                <xsl:value-of select="'std::string'" />
            </xsl:when>
            <xsl:when test="$type = 'Boolean'">
                <xsl:value-of select="'bool'" />
            </xsl:when>
            <xsl:when test="$type = 'Character'">
                <xsl:value-of select="'char'" />
            </xsl:when>
            <xsl:when test="$type = 'FloatNumber' or $type = 'Float'">
                <xsl:value-of select="'float'" />
            </xsl:when>
            <xsl:when test="$type = 'DoubleNumber' or $type = 'LongDoubleNumber' or $type = 'Double'">
                <xsl:value-of select="'double'" />
            </xsl:when>
            <xsl:when test="$type = 'LongInteger' or $type = 'Integer'">
                <xsl:value-of select="'int'" />
            </xsl:when>
            <xsl:when test="$type = 'UnsignedLongInteger'">
                <xsl:value-of select="'unsigned int'" />
            </xsl:when>
            <xsl:otherwise>
                <xsl:message>Warning: Unknown Type <xsl:value-of select="o:wrap_quote($type)" /></xsl:message>
                <xsl:value-of select="''" />
            </xsl:otherwise>
        </xsl:choose>
    </xsl:function>

    <xsl:function name="proto:get_aggregate_qualified_type" as="xs:string">
        <xsl:param name="aggregate" as="element()"/>

        <xsl:variable name="aggregate_namespace" select="graphml:get_namespace($aggregate)" />
        <xsl:variable name="aggregate_label" select="o:title_case(graphml:get_label($aggregate))" />
        
        <xsl:value-of select="o:join_list(($aggregate_namespace, $aggregate_label), '.')" />
    </xsl:function>

    <xsl:function name="idl:get_aggregate_qualified_type" as="xs:string">
        <xsl:param name="aggregate" as="element()"/>

        <xsl:variable name="aggregate_namespace" select="graphml:get_data_value($aggregate, 'namespace')" />
        <xsl:variable name="aggregate_label" select="o:title_case(graphml:get_label($aggregate))" />


        <xsl:choose>
            <xsl:when test="$aggregate_namespace = ''">
                <xsl:value-of select="concat('::', $aggregate_label)" />
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="cpp:combine_namespaces(($aggregate_namespace, $aggregate_label))" />
            </xsl:otherwise>
        </xsl:choose>
    </xsl:function>

    <xsl:function name="cdit:parse_middlewares" as="xs:string*">
        <xsl:param name="middlewares" as="xs:string*"/>

        <xsl:variable name="token_middlewares" select="tokenize(normalize-space(lower-case($middlewares)), ',')" /> 

        <xsl:variable name="middlewares_list" as="xs:string*">
            <xsl:sequence select="$token_middlewares" />
            <xsl:if test="'zmq' = $token_middlewares">
                <xsl:sequence select="'proto'" />
            </xsl:if>
            <xsl:if test="'qpid' = $token_middlewares">
                <xsl:sequence select="'proto'" />
            </xsl:if>
            <xsl:if test="count($token_middlewares) > 0">
                <xsl:sequence select="'base'" />
            </xsl:if>
        </xsl:variable>

        <xsl:sequence select="distinct-values($middlewares_list)"/>
    </xsl:function>

    <xsl:function name="cdit:parse_components" as="xs:string*">
        <xsl:param name="components" as="xs:string*"/>

        <xsl:variable name="token_middlewares" select="tokenize(normalize-space(lower-case($components)), ',')" />
        <xsl:sequence select="distinct-values($token_middlewares)"/>
    </xsl:function>

    <xsl:function name="cdit:get_aggregates_path" as="xs:string">
        <xsl:param name="aggregate" as="element()" />
        
        <xsl:variable name="aggregate_label" select="lower-case(graphml:get_label($aggregate))" />
        <xsl:variable name="aggregate_namespace" select="lower-case(graphml:get_namespace($aggregate))" />
        
        <xsl:value-of select="o:join_paths(($aggregate_namespace, $aggregate_label))" />
    </xsl:function>

    <xsl:function name="cdit:get_base_aggregates_cpp_path" as="xs:string">
        <xsl:param name="aggregate" as="element()" />
        
        <xsl:variable name="path" select="cdit:get_aggregates_path($aggregate)" />
        <xsl:variable name="file" select="cdit:get_base_aggregate_cpp_name($aggregate)" />
        <xsl:value-of select="o:join_paths(($path, $file))" />
    </xsl:function>

    <xsl:function name="cdit:get_base_aggregate_h_name" as="xs:string">
        <xsl:param name="aggregate" as="element()" />
        
        <xsl:variable name="aggregate_label" select="lower-case(graphml:get_label($aggregate))" />
        <xsl:value-of select="concat($aggregate_label,'.h')" />
    </xsl:function>

    <xsl:function name="cdit:get_base_aggregate_cpp_name" as="xs:string">
        <xsl:param name="aggregate" as="element()" />
        
        <xsl:variable name="aggregate_label" select="lower-case(graphml:get_label($aggregate))" />
        <xsl:value-of select="concat($aggregate_label,'.cpp')" />
    </xsl:function>

    <xsl:function name="cdit:get_base_aggregate_h_path" as="xs:string">
        <xsl:param name="aggregate" as="element()" />

        <xsl:variable name="path" select="cdit:get_aggregates_path($aggregate)" />
        <xsl:variable name="file" select="cdit:get_base_aggregate_h_name($aggregate)" />
        <xsl:value-of select="o:join_paths(($path, $file))" />
    </xsl:function>

    <xsl:function name="cdit:get_base_enum_h_path" as="xs:string">
        <xsl:param name="enum" as="element()" />

        <xsl:variable name="path" select="cdit:get_aggregates_path($enum)" />
        <xsl:variable name="file" select="cdit:get_base_aggregate_h_name($enum)" />
        <xsl:value-of select="o:join_paths(('enums', $path, $file))" />
    </xsl:function>

    <!-- Get all required aggregates -->
    <xsl:function name="cdit:get_required_aggregates" as="element()*">
        <xsl:param name="aggregate" as="element()" />

        <xsl:variable name="aggregate_instances" select="graphml:get_descendant_nodes_of_kind($aggregate, 'AggregateInstance')" />
        <xsl:sequence select="graphml:get_definitions($aggregate_instances)" />
    </xsl:function>

    <xsl:function name="cdit:get_eventport_function_name" as="xs:string">
        <xsl:param name="port" as="element()"/>
        <xsl:variable name="port_def" select="graphml:get_definition($port)" />
        <xsl:variable name="label" select="graphml:get_label($port_def)" />
        <xsl:variable name="kind" select="graphml:get_kind($port_def)" />

        <xsl:variable name="prefix">
            <xsl:choose>
                <xsl:when test="$kind = 'InEventPort'">
                    <xsl:value-of select="'In'" />
                </xsl:when>
                <xsl:when test="$kind = 'OutEventPort'">
                    <xsl:value-of select="'Out'" />
                </xsl:when>
                <xsl:when test="$kind = 'PeriodicEvent'">
                    <xsl:value-of select="'Periodic'" />
                </xsl:when>
                <xsl:otherwise>
                    <xsl:value-of select="o:warning(('Kind:', $kind, 'Not supported'))" />
                </xsl:otherwise>
            </xsl:choose>
        </xsl:variable>
        <xsl:value-of select="o:join_list(($prefix, $label), '_')" />
    </xsl:function>
    

    <xsl:function name="cdit:comment_graphml_node">
        <xsl:param name="element" as="element()" />
        <xsl:param name="tab" as="xs:integer" />

        <xsl:value-of select="cpp:comment((o:wrap_square(graphml:get_kind($element)), graphml:get_label($element), o:wrap_angle(graphml:get_id($element))), $tab)" />
    </xsl:function>

    <xsl:function name="cdit:declare_datatype_functions">
        <xsl:param name="aggregate" as="element()" />
        <xsl:param name="tab" as="xs:integer" />
        
        <xsl:variable name="label" select="graphml:get_label($aggregate)" />
        <xsl:variable name="kind" select="graphml:get_kind($aggregate)" />
        <xsl:variable name="cpp_type" select="cpp:get_qualified_type($aggregate)" />
        <xsl:variable name="var_label" select="cdit:get_variable_label($aggregate)" />
        <xsl:variable name="value" select="graphml:get_data_value($aggregate, 'value')" />

        
        <xsl:choose>
            <xsl:when test="$cpp_type != ''">
                <!-- Public Declarations -->
                <xsl:value-of select="cpp:public($tab)" />
                <xsl:value-of select="cdit:comment_graphml_node($aggregate, $tab + 1)" />
                <xsl:value-of select="cpp:declare_function('void', concat('set_', $label), cpp:const_ref_var_def($cpp_type, 'value'), ';', $tab + 1)" />
                <xsl:value-of select="cpp:declare_function(cpp:const_ref_var_def($cpp_type, ''), concat('get_', $label), '', ' const;', $tab + 1)" />
                <xsl:value-of select="cpp:declare_function(cpp:ref_var_def($cpp_type, ''), $label, '', ';', $tab + 1)" />
                <!-- Private Declarations -->
                <xsl:value-of select="cpp:private($tab)" />
                <xsl:choose>
                    <xsl:when test="$kind != 'Attribute'">
                        <xsl:value-of select="cpp:define_variable($cpp_type, $var_label, $value, cpp:nl(), $tab + 1)" />
                    </xsl:when>
                    <xsl:otherwise>
                        <xsl:value-of select="cpp:declare_variable(cpp:shared_ptr('Attribute'), $var_label, cpp:nl(), $tab + 1)" />
                    </xsl:otherwise>
                </xsl:choose>
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="cdit:comment_graphml_node($aggregate, $tab)" />
                <xsl:value-of select="cpp:comment('Cannot find valid CPP type for this element', $tab)" />
            </xsl:otherwise>
        </xsl:choose>
    </xsl:function>

    <xsl:function name="cdit:define_datatype_functions">
        <xsl:param name="aggregate" as="element()" />
        <xsl:param name="class_name" as="xs:string" />
        
        <xsl:variable name="label" select="graphml:get_label($aggregate)" />
        <xsl:variable name="cpp_type" select="cpp:get_qualified_type($aggregate)" />
        <xsl:variable name="var_label" select="cdit:get_variable_label($aggregate)" />
        <xsl:variable name="kind" select="graphml:get_kind($aggregate)" />

        <xsl:if test="$cpp_type != ''">
                <!-- Define Setter Function -->
                <xsl:value-of select="cpp:define_function('void', $class_name, concat('set_', $label), cpp:const_ref_var_def($cpp_type, 'value'), cpp:scope_start(0))" />
                    <xsl:choose>
                        <xsl:when test="$kind != 'Attribute'">
                            <xsl:value-of select="cpp:define_variable('', $var_label, 'value', cpp:nl(), 1)" />
                        </xsl:when>
                        <xsl:otherwise>
                            <xsl:value-of select="cpp:invoke_function($var_label, cpp:arrow(), cdit:get_attribute_set_function($aggregate), 'value', 1)" />
                            <xsl:value-of select="cpp:nl()" />
                        </xsl:otherwise>
                    </xsl:choose>
                <xsl:value-of select="cpp:scope_end(0)" />
                <xsl:value-of select="o:nl(1)" />

                <!-- Define Getter Function -->
                <xsl:value-of select="cpp:define_function(cpp:const_ref_var_def($cpp_type, ''), $class_name, concat('get_', $label), '', concat(' const', cpp:scope_start(0)))" />
                    <xsl:choose>
                        <xsl:when test="$kind != 'Attribute'">
                            <xsl:value-of select="cpp:return($var_label, 1)" />
                        </xsl:when>
                        <xsl:otherwise>
                            <xsl:variable name="get_var" select="cpp:invoke_function($var_label, cpp:arrow(), cdit:get_attribute_ref_function($aggregate), '', 0)" />
                            <xsl:value-of select="cpp:return($get_var, 1)" />
                        </xsl:otherwise>
                    </xsl:choose>
                <xsl:value-of select="cpp:scope_end(0)" />
                <xsl:value-of select="o:nl(1)" />

                <xsl:value-of select="cpp:define_function(cpp:ref_var_def($cpp_type, ''), $class_name, $label, '', cpp:scope_start(0))" />
                    <xsl:choose>
                        <xsl:when test="$kind != 'Attribute'">
                            <xsl:value-of select="cpp:return($var_label, 1)" />
                        </xsl:when>
                        <xsl:otherwise>
                            <xsl:variable name="get_var" select="cpp:invoke_function($var_label, cpp:arrow(), cdit:get_attribute_ref_function($aggregate), '', 0)" />
                            <xsl:value-of select="cpp:return($get_var, 1)" />
                        </xsl:otherwise>
                    </xsl:choose>
                    <xsl:value-of select="cpp:scope_end(0)" />
                <xsl:value-of select="o:nl(1)" />
            </xsl:if>
    </xsl:function>

    <xsl:function name="cdit:get_attribute_enum_type" as="xs:string">
        <xsl:param name="attribute" as="element()"  />
        <xsl:value-of select="cpp:combine_namespaces(('ATTRIBUTE_TYPE', upper-case(graphml:get_type($attribute))))" />
    </xsl:function>	

    <xsl:function name="cdit:get_attribute_set_function" as="xs:string">
        <xsl:param name="attribute" as="element()"  />
        <xsl:value-of select="o:join_list(('set', graphml:get_type($attribute)), '_')" />
    </xsl:function>	

    <xsl:function name="cdit:get_attribute_get_function" as="xs:string">
        <xsl:param name="attribute" as="element()"  />
        <xsl:value-of select="o:join_list(('get', graphml:get_type($attribute)), '_')" />
    </xsl:function>

    <xsl:function name="cdit:get_attribute_ref_function" as="xs:string">
        <xsl:param name="attribute" as="element()"  />
        <xsl:value-of select="graphml:get_type($attribute)" />
    </xsl:function>


    <xsl:function name="cdit:get_unique_workers" as="element()*">
        <xsl:param name="component_impl" as="element()"  />

        <xsl:variable name="workers" select="graphml:get_child_nodes_of_kind($component_impl, 'WorkerProcess')" />

        <xsl:for-each-group select="$workers" group-by="graphml:get_data_value(., 'workerID')">
            <xsl:variable name="worker" select="graphml:get_data_value(., 'worker')" />

            <xsl:choose>
                <!-- Ignore Vector Operations -->
                <xsl:when test="$worker = 'Vector_Operations'" />
                <xsl:otherwise>
                    <xsl:sequence select="." />
                </xsl:otherwise>
            </xsl:choose>
        </xsl:for-each-group>
    </xsl:function>

    <xsl:function name="cdit:get_worker_path" as="xs:string">
        <xsl:param name="worker" as="element()"  />
        <xsl:variable name="worker_folder" select="lower-case(graphml:get_data_value($worker, 'folder'))" />
        
        <xsl:variable name="rel_folder" select="replace($worker_folder, '\$\{re_path\}/src/', '')" />
        <xsl:value-of select="$rel_folder" />
    </xsl:function>

    <xsl:function name="cdit:get_worker_header" as="xs:string">
        <xsl:param name="worker" as="element()"  />

        <xsl:variable name="worker_name" select="graphml:get_data_value($worker, 'worker')" />

        <xsl:choose>
            <xsl:when test="$worker_name = 'Vector_Operations'">
                <xsl:value-of select="''" />
            </xsl:when>
            <xsl:otherwise>
                <xsl:variable name="worker_file" select="lower-case(graphml:get_data_value($worker, 'file'))" />
                <xsl:value-of select="o:join_paths((cdit:get_worker_path($worker), concat($worker_file, '.h')))" />
            </xsl:otherwise>
        </xsl:choose>
    </xsl:function>

    <xsl:function name="cdit:get_resolved_enum_member_type" as="xs:string">
        <xsl:param name="enum_member" as="element()"/>

        <xsl:variable name="member_label" select="upper-case(graphml:get_label($enum_member))" />
        <xsl:variable name="enum" select="graphml:get_parent_node($enum_member)" />
        <xsl:variable name="enum_namespaces" select="o:trim_list(('Base', graphml:get_namespace($enum)))" />
        <xsl:variable name="enum_label" select="graphml:get_label($enum)" />

        <xsl:value-of select="cpp:combine_namespaces(($enum_namespaces, $enum_label, $member_label))" />
    </xsl:function>


    
    <xsl:function name="cmake:setup_re_path">
        <xsl:value-of select="cmake:comment('CDIT Runtime Paths', 0)" />
        <xsl:value-of select="cmake:set_variable('RE_PATH', cmake:get_env_var('RE_PATH'), 0)" />
        <xsl:value-of select="cmake:set_variable('CMAKE_MODULE_PATH', o:join_paths((cmake:wrap_variable('RE_PATH'), 'cmake_modules')), 0)" />
        <xsl:value-of select="o:nl(1)" />
    </xsl:function>

    <xsl:function name="cdit:get_top_cmake">
        <xsl:value-of select="cmake:cmake_minimum_required('3.1')" />
        <xsl:value-of select="cmake:set_cpp11()" />
        <xsl:value-of select="cmake:setup_re_path()" />

        <xsl:variable name="lib_dir" select="o:join_paths((cmake:current_source_dir_var(), 'lib'))" />

        <xsl:value-of select="cmake:set_library_output_directory($lib_dir)" />
        <xsl:value-of select="cmake:set_archive_output_directory($lib_dir)" />
        
        <xsl:value-of select="cmake:add_subdirectories(('datatypes', 'components'))" />
    </xsl:function>

    
</xsl:stylesheet>