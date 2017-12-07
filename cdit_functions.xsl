<!-- Functions for cpp syntax output -->
<xsl:stylesheet version="2.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:xs="http://www.w3.org/2001/XMLSchema"
    xmlns:o="http://github.com/cdit-ma/o"
    xmlns:graphml="http://github.com/cdit-ma/graphml"
    xmlns:cdit="http://github.com/cdit-ma/cdit"
    xmlns:cpp="http://github.com/cdit-ma/cpp"
    >

    <!--
        Gets the path to the aggregate folder
    -->
    <xsl:function name="cdit:get_datatype_path" as="xs:string">
        <xsl:param name="middleware" as="xs:string" />
        <xsl:param name="aggregate" as="element()" />
        <xsl:param name="file" as="xs:string" />

        <xsl:variable name="aggregate_namespace" select="lower-case(graphml:get_data_value($aggregate, 'namespace'))" />
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

    <xsl:function name="cdit:get_aggregate_qualified_type" as="xs:string">
        <xsl:param name="aggregate" as="element()" />
        <xsl:param name="middleware" as="xs:string" />

        <xsl:variable name="aggregate_namespace" select="graphml:get_data_value($aggregate, 'namespace')" />
        <xsl:variable name="aggregate_label" select="o:title_case(graphml:get_label($aggregate))" />
        
        <xsl:variable name="extra_namespace" as="xs:string*">
            <xsl:if test="$middleware = 'base'">
                <!-- DDS implementations use set via accessors -->
                <xsl:value-of select="'Base'" />
            </xsl:if>
        </xsl:variable>
        <xsl:value-of select="cpp:get_qualified_type(($extra_namespace, $aggregate_namespace, $aggregate_label))" />
    </xsl:function>



    




</xsl:stylesheet>