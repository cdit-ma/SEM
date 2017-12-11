<!-- Functions for cpp syntax output -->
<xsl:stylesheet version="2.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:xs="http://www.w3.org/2001/XMLSchema"
    xmlns:o="http://github.com/cdit-ma/o"
    xmlns:graphml="http://github.com/cdit-ma/graphml"
    xmlns:cdit="http://github.com/cdit-ma/cdit"
    xmlns:cpp="http://github.com/cdit-ma/cpp"
    xmlns:proto="http://github.com/cdit-ma/proto"
    xmlns:idl="http://github.com/cdit-ma/idl"
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
        <xsl:value-of select="cpp:invoke_function($obj, $operator, $function_name, '')" />
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
                            <xsl:value-of select="concat(cpp:invoke_function('', '', concat('mutable_', $variable_syntax), ''), cpp:arrow(), 'Swap')" />
                        </xsl:when>
                        <xsl:when test="$node_kind = 'Vector'">
                            <xsl:variable name="vector_child" select="graphml:get_child_node($node, 1)" />
                            <xsl:variable name="vector_kind" select="graphml:get_kind($vector_child)" />

                            <xsl:choose>
                                <xsl:when test="$vector_kind = 'AggregateInstance'">
                                    <!-- Vectors with Objects in it can use a mutable_swap function -->
                                    <xsl:value-of select="concat(cpp:invoke_function('', '', concat('add_', $variable_syntax), ''), cpp:arrow(), 'Swap')" />
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
                <xsl:value-of select="concat(cpp:invoke_function($obj, $operator, $function_name, ''), ' = ', $value)" />
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="cpp:invoke_function($obj, $operator, $function_name, $value)" />
            </xsl:otherwise>
        </xsl:choose>
    </xsl:function>

    <xsl:function name="cpp:get_qualified_type" as="xs:string">
        <xsl:param name="node" as="element()" />
        <xsl:variable name="kind" select="graphml:get_kind($node)" />
        
        <xsl:choose>    
            <xsl:when test="$kind = 'Member'">
                <xsl:value-of select="cpp:get_member_type($node)" />
            </xsl:when>
            <xsl:when test="$kind = 'EnumInstance'">
                <xsl:value-of select="cpp:get_enum_qualified_type($node)" />
            </xsl:when>
            <xsl:when test="$kind = 'AggregateInstance'">
                <xsl:value-of select="cpp:get_aggregate_qualified_type($node, 'base')" />
            </xsl:when>
            <xsl:when test="$kind = 'Vector'">
                <xsl:value-of select="cpp:get_vector_qualified_type($node)" />
            </xsl:when>
            <xsl:otherwise>
                <xsl:message>Node Kind <xsl:value-of select="o:wrap_quote($kind)"/> Not Implemented</xsl:message>
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

        <xsl:variable name="enum_definition" select="graphml:get_definition($enum)" />
        <xsl:variable name="namespace" select="graphml:get_namespace($enum_definition)" />
        <xsl:variable name="label" select="o:title_case(graphml:get_label($enum_definition))" />

        <xsl:value-of select="cpp:combine_namespaces(($namespace, $label))" />
    </xsl:function>

    <xsl:function name="cpp:get_vector_qualified_type" as="xs:string">
        <xsl:param name="vector" as="element()" />

        <xsl:variable name="vector_child" select="graphml:get_vector_child($vector)" />
        <xsl:variable name="child_type" select="cpp:get_qualified_type($vector_child)" />
        <xsl:if test="$child_type != ''">
            <xsl:value-of select="concat('std::vector', o:wrap_angle($child_type))" />
        </xsl:if>
    </xsl:function>

    <xsl:function name="cdit:get_variable_label" as="xs:string">
        <xsl:param name="variable" as="element()" />

        <xsl:variable name="label" select="o:title_case(graphml:get_label($variable))" />

        <xsl:value-of select="concat(lower-case($label), '_')" />
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

    <xsl:function name="cdit:get_aggregates_path" as="xs:string">
        <xsl:param name="aggregate" as="element()" />
        
        <xsl:variable name="aggregate_label" select="lower-case(graphml:get_label($aggregate))" />
        <xsl:variable name="aggregate_namespace" select="lower-case(graphml:get_namespace($aggregate))" />
        
        <xsl:value-of select="o:join_paths(($aggregate_namespace, $aggregate_label))" />
    </xsl:function>

    <xsl:function name="cdit:get_base_aggregates_cpp_path" as="xs:string">
        <xsl:param name="aggregate" as="element()" />
        
        <xsl:variable name="aggregate_label" select="lower-case(graphml:get_label($aggregate))" />
        <xsl:variable name="aggregate_namespace" select="lower-case(graphml:get_namespace($aggregate))" />

        <xsl:variable name="file" select="concat($aggregate_label,'.cpp')" />
        
        <xsl:value-of select="o:join_paths(($aggregate_namespace, $aggregate_label, $file))" />
    </xsl:function>

    <xsl:function name="cdit:get_base_aggregate_h_name" as="xs:string">
        <xsl:param name="aggregate" as="element()" />
        
        <xsl:variable name="aggregate_label" select="lower-case(graphml:get_label($aggregate))" />
        <xsl:value-of select="concat($aggregate_label,'.h')" />
    </xsl:function>

    <xsl:function name="cdit:get_base_aggregate_h_path" as="xs:string">
        <xsl:param name="aggregate" as="element()" />
        
        <xsl:variable name="aggregate_label" select="lower-case(graphml:get_label($aggregate))" />
        <xsl:variable name="aggregate_namespace" select="lower-case(graphml:get_namespace($aggregate))" />

        <xsl:variable name="file" select="cdit:get_base_aggregate_h_name($aggregate)" />
        
        <xsl:value-of select="o:join_paths(($aggregate_namespace, $aggregate_label, $file))" />
    </xsl:function>

    <!-- Get all required aggregates -->
    <xsl:function name="cdit:get_required_aggregates" as="element()*">
        <xsl:param name="aggregate" as="element()" />

        <xsl:variable name="aggregate_instances" select="graphml:get_descendant_nodes_of_kind($aggregate, 'AggregateInstance')" />
        <xsl:sequence select="graphml:get_definitions($aggregate_instances)" />
    </xsl:function>



</xsl:stylesheet>