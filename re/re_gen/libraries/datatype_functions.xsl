<!-- Functions for reading/interpretting graphml xml -->
<xsl:stylesheet version="2.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:xs="http://www.w3.org/2001/XMLSchema"
    xmlns:gml="http://graphml.graphdrawing.org/xmlns"
    xmlns:cdit="http://github.com/cdit-ma/re_gen/cdit"
    xmlns:o="http://github.com/cdit-ma/re_gen/o"
    xmlns:graphml="http://github.com/cdit-ma/re_gen/graphml"
    xmlns:cpp="http://github.com/cdit-ma/re_gen/cpp"
    xmlns:cmake="http://github.com/cdit-ma/re_gen/cmake"
    xmlns:proto="http://github.com/cdit-ma/re_gen/proto"
    xmlns:idl="http://github.com/cdit-ma/re_gen/idl"
    >

    <!--
        Gets the translate_cpp file
    -->
    <xsl:function name="cdit:get_translator_cpp">
        <xsl:param name="aggregate" />
        <xsl:param name="middleware" as="xs:string" />

        <xsl:variable name="aggregate_namespace" select="graphml:get_namespace($aggregate)" />
        <xsl:variable name="aggregate_label" select="graphml:get_label($aggregate)" />
        
        <xsl:variable name="middleware_type" select="cdit:get_aggregate_qualified_type($aggregate, $middleware)" />
        <xsl:variable name="base_type" select="cdit:get_aggregate_qualified_type($aggregate, 'base')" />
        <xsl:variable name="middleware_type" select="cdit:get_aggregate_qualified_type($aggregate, $middleware)" />

        <!-- Preamble -->
        <xsl:value-of select="cpp:print_regen_version('datatype_functions.xsl', 'cdit:get_translator_cpp', 0)" />
        
        <!-- Include the translator headers -->
        <xsl:value-of select="cpp:include_library_header(o:join_paths(('core', 'ports', 'translator.h')))" />
        <xsl:value-of select="cpp:include_library_header(o:join_paths(('core', 'ports', 'primitivetranslator.hpp')))" />
        <xsl:value-of select="o:nl(1)" />

        <!-- Include the base message type -->
        <xsl:value-of select="cdit:include_aggregate_headers($aggregate, 'base')" />

        <!-- Include the Middleware message type -->
        <xsl:value-of select="cdit:include_aggregate_headers($aggregate, $middleware)" />

        <!-- Define the namespace -->
        <xsl:variable name="function_namespace" select="('Base')" />
        <xsl:variable name="tab" select="count($function_namespace)" />

        <xsl:value-of select="cpp:define_namespaces($function_namespace)" />



        <xsl:variable name="template_type" select="cpp:join_args(($base_type, $middleware_type))"/>
        
        <xsl:value-of select="cpp:declare_templated_class_function_specialisation($template_type, cpp:unique_ptr($middleware_type), 'Translator', 'BaseToMiddleware', cpp:const_ref_var_def($base_type, 'value'), '{', $tab)" />
        <xsl:value-of select="cdit:get_translate_function_cpp($aggregate, $middleware, 'base', $middleware, $tab + 1)" />
        <xsl:value-of select="cpp:scope_end($tab)" />
        
        <xsl:value-of select="o:nl(1)" />
        
        <xsl:value-of select="cpp:declare_templated_class_function_specialisation($template_type, cpp:unique_ptr($base_type), 'Translator', 'MiddlewareToBase', cpp:const_ref_var_def($middleware_type, 'value'), '{', $tab)" />
        <xsl:value-of select="cdit:get_translate_function_cpp($aggregate, $middleware, $middleware, 'base', $tab + 1)" />
        <xsl:value-of select="cpp:scope_end($tab)" />

        <!-- End the namespace -->
        <xsl:value-of select="cpp:close_namespaces($function_namespace)" />
        <xsl:value-of select="o:nl(1)" />
    </xsl:function>

    
    <xsl:function name="cdit:get_enum_proto_file">
        <xsl:param name="enum" />

        <xsl:variable name="label" select="graphml:get_label($enum)" />
        <xsl:variable name="namespace" select="graphml:get_namespace($enum)" />
        <xsl:variable name="proto_label" select="o:title_case($label)" />

        <!-- Version Number -->
        <xsl:value-of select="cpp:print_regen_version('datatype_functions.xsl', 'cdit:get_enum_proto_file', 0)" />


        <!-- Using Protobuf 3 -->
        <xsl:value-of select="proto:syntax('proto3')" />

        <xsl:variable name="flattened_namespace" select="o:join_list($namespace, '.')" />
        <xsl:value-of select="proto:package($flattened_namespace)" />

        <xsl:value-of select="proto:enum($proto_label)" />
        <xsl:for-each select="graphml:get_child_nodes_of_kind($enum, 'EnumMember')">
            <xsl:variable name="member_label" select="cdit:get_enum_member_type(., 'proto')" />
            <xsl:value-of select="proto:enum_value($member_label, position() - 1)" />
        </xsl:for-each>
        <xsl:value-of select="cpp:scope_end(0)" />
    </xsl:function>

    <xsl:function name="cdit:get_proto_file">
        <xsl:param name="aggregate" />

        <!-- Get the definitions of the Data Libraries used in this Aggregate -->
        <xsl:variable name="aggregate_definitions" select="cdit:get_required_aggregates($aggregate, true())" />
        <xsl:variable name="enum_definitions" select="cdit:get_required_enums($aggregate)" />
        <xsl:variable name="required_datatypes" select="($aggregate_definitions, $enum_definitions)" />

        
        <xsl:variable name="aggregate_label" select="graphml:get_label($aggregate)" />
        <xsl:variable name="aggregate_namespace" select="graphml:get_namespace($aggregate)" />
        <xsl:variable name="proto_label" select="o:title_case($aggregate_label)" />



        <!-- Version Number -->
        <xsl:value-of select="cpp:print_regen_version('datatype_functions.xsl', 'cdit:get_proto_file', 0)" />

        <!-- Using Protobuf 3 -->
        <xsl:value-of select="proto:syntax('proto3')" />

        <!-- Import the definitions of each aggregate instance used -->
        <xsl:for-each select="$required_datatypes">
            <xsl:if test="position() = 1">
                <xsl:value-of select="cpp:comment('Import required .proto files', 0)" />
            </xsl:if>
            
            <xsl:variable name="required_file" select="cdit:get_aggregates_middleware_file_name(., 'proto')" />
            <xsl:value-of select="proto:import($required_file)" />
            
            <xsl:if test="position() = last()">
                <xsl:value-of select="o:nl(1)" />
            </xsl:if>
        </xsl:for-each>

        <xsl:variable name="flattened_namespace" select="o:join_list($aggregate_namespace, '.')" />
        <xsl:value-of select="proto:package($flattened_namespace)" />

        <xsl:value-of select="proto:message($proto_label)" />
        
        <xsl:for-each select="graphml:get_child_nodes($aggregate)">
            <xsl:variable name="kind" select="graphml:get_kind(.)" />
            <xsl:variable name="index" select="graphml:get_index(.) + 1" />
            <xsl:variable name="label" select="lower-case(graphml:get_label(.))" />
            <xsl:variable name="type" select="graphml:get_type(.)" />
            
            <xsl:variable name="is_key" select="graphml:is_key(.)" />

            <xsl:choose>    
                <xsl:when test="$kind = 'Member'">
                    
                    <xsl:variable name="cpp_type" select="cpp:get_primitive_type($type)" />
                    <xsl:variable name="proto_type" select="proto:get_type($cpp_type)" />
                    <xsl:value-of select="proto:member($proto_type, $label, $index)" />
                </xsl:when>
                <xsl:when test="$kind = 'EnumInstance'">
                    <xsl:variable name="proto_type" select="proto:get_aggregate_qualified_type(graphml:get_definition(.))" />
                    <xsl:value-of select="proto:member($proto_type, $label, $index)" />
                </xsl:when>
                <xsl:when test="$kind = 'AggregateInstance'">
                    <xsl:variable name="proto_type" select="proto:get_aggregate_qualified_type(graphml:get_definition(.))" />
                    <xsl:value-of select="proto:member($proto_type, $label, $index)" />
                </xsl:when>
                 <xsl:when test="$kind = 'Vector'">
                    <xsl:variable name="vector_child" select="graphml:get_vector_child(.)" />
                    <xsl:variable name="vector_child_kind" select="graphml:get_kind($vector_child)" />
                    <xsl:variable name="vector_child_type" select="graphml:get_type($vector_child)" />

                    <xsl:variable name="proto_type">
                        <xsl:choose>
                            <xsl:when test="$vector_child_kind = 'AggregateInstance' or $vector_child_kind = 'EnumInstance'">
                                <xsl:value-of select="proto:get_aggregate_qualified_type(graphml:get_definition($vector_child))" />
                            </xsl:when>
                            <xsl:when test="$vector_child_kind = 'Member'">
                                <xsl:variable name="cpp_type" select="cpp:get_primitive_type($vector_child_type)" />
                                <xsl:value-of select="proto:get_type($cpp_type)" />
                            </xsl:when>
                            <xsl:when test="$vector_child_kind = 'EnumInstance'">
                                <xsl:value-of select="cdit:get_enum_type($vector_child, 'proto')" />
                            </xsl:when>
                        </xsl:choose>
                    </xsl:variable>
                    <xsl:value-of select="proto:repeated_member($proto_type, $label, $index)" />
                </xsl:when>
            </xsl:choose>
        </xsl:for-each>
        <xsl:value-of select="cpp:scope_end(0)" />
    </xsl:function>


    <xsl:function name="cdit:get_idl_enum">
        <xsl:param name="enum" />
        <xsl:param name="tab" />
           
        <xsl:variable name="enum_namespace" select="graphml:get_namespace($enum)" />
        <xsl:variable name="enum_label" select="o:title_case(graphml:get_label($enum))" />

        <xsl:variable name="enum_members" as="xs:string*">
            
            <xsl:for-each select="graphml:get_child_nodes_of_kind($enum, 'EnumMember')">
                <xsl:sequence select="upper-case(graphml:get_label(.))" />
            </xsl:for-each>
        </xsl:variable>

        <xsl:value-of select="idl:get_enum($enum_namespace, $enum_label, $enum_members, $tab)" />
    </xsl:function>

    <xsl:function name="cdit:get_idl_enum_file">
        <xsl:param name="enum" />
        <xsl:param name="middleware" />

        <!-- Preamble -->
        <xsl:value-of select="cpp:print_regen_version('datatype_functions.xsl', 'cdit:get_idl_enum_file', 0)" />
        <xsl:value-of select="cdit:get_idl_enum($enum, 0)" />
    </xsl:function>

    

    <xsl:function name="cdit:get_idl_datatype">
        <xsl:param name="aggregate" />
        <xsl:param name="middleware" />

        <!-- Get the definitions of the AggregateInstances used in this Aggregate -->
        <!-- Get the definitions of the Data Libraries used in this Aggregate -->
        <xsl:variable name="aggregate_definitions" select="cdit:get_required_aggregates($aggregate, false())" />
        <xsl:variable name="enum_definitions" select="cdit:get_required_enums($aggregate)" />
        <xsl:variable name="required_datatypes" select="($aggregate_definitions, $enum_definitions)" />

        <xsl:variable name="vectors" select="graphml:get_child_nodes_of_kind($aggregate, 'Vector')" />

        <xsl:variable name="aggregate_label" select="graphml:get_label($aggregate)" />
        <xsl:variable name="aggregate_namespace" select="graphml:get_namespace($aggregate)" />
        <xsl:variable name="label" select="o:title_case($aggregate_label)" />

        <xsl:variable name="is_union" select="graphml:evaluate_data_value_as_boolean($aggregate, 'is_union')" />

        <xsl:variable name="define_guard_name" select="upper-case(o:join_list(($aggregate_namespace, $aggregate_label, 'DATATYPE_IDL'), '_'))" />

        <!-- Preamble -->
        <xsl:value-of select="cpp:print_regen_version('datatype_functions.xsl', 'cdit:get_idl_datatype', 0)" />

        <!-- Define Guard -->
        <xsl:value-of select="cpp:define_guard_start($define_guard_name)" />

        <!-- Import the definitions of each aggregate instance used -->
        <xsl:for-each select="$required_datatypes">
            <xsl:if test="position() = 1">
                <xsl:value-of select="cpp:comment('Import required .idl files', 0)" />
            </xsl:if>
            <xsl:variable name="required_file" select="cdit:get_aggregates_middleware_file_name(., $middleware)" />
            <xsl:value-of select="idl:include($required_file)" />
            
            <xsl:if test="position() = last()">
                <xsl:value-of select="o:nl(1)" />
            </xsl:if>
        </xsl:for-each>

        <xsl:variable name="tab" select="count($aggregate_namespace)" />

        <!-- Define Namespaces -->
        <xsl:for-each select="$aggregate_namespace">
            <xsl:value-of select="idl:module_start(., position() - 1)" />
        </xsl:for-each>


        <xsl:choose>
            <xsl:when test="$is_union">
                <xsl:value-of select="cdit:get_idl_union($aggregate, $tab)" />
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="cdit:get_idl_struct($aggregate, $tab)" />
            </xsl:otherwise>
        </xsl:choose>

        <xsl:for-each select="$aggregate_namespace">
            <xsl:value-of select="idl:module_end(., position() - 1)" />
        </xsl:for-each>

        <!-- Define Guard -->
        <xsl:value-of select="cpp:define_guard_end($define_guard_name)" />
    </xsl:function>

    <xsl:function name="cdit:get_idl_type_def_name">
        <xsl:param name="vector" />

        <xsl:variable name="vector_child" select="graphml:get_vector_child($vector)" />
        <xsl:variable name="vector_child_kind" select="graphml:get_kind($vector_child)" />
        <xsl:variable name="vector_child_type" select="graphml:get_type($vector_child)" />
        <xsl:variable name="is_array" select="graphml:evaluate_data_value_as_boolean($vector, 'is_array')" />

        <xsl:variable name="idl_type">
            <xsl:choose>
                <xsl:when test="$vector_child_kind = 'AggregateInstance' or $vector_child_kind = 'EnumInstance'">
                    <xsl:variable name="namespace" select="graphml:get_namespace($vector_child)" />
                    <xsl:variable name="label" select="o:title_case(graphml:get_label($vector_child))" />
                    <xsl:value-of select="o:join_list(($namespace, $label), '_')" />
                </xsl:when>
                <xsl:when test="$vector_child_kind = 'Member'">
                    <xsl:variable name="cpp_type" select="cpp:get_primitive_type($vector_child_type)" />
                    <xsl:value-of select="idl:get_type($cpp_type)" />
                </xsl:when>
            </xsl:choose>
        </xsl:variable>

        <xsl:choose>
            <xsl:when test="$is_array">
                <xsl:variable name="array_size" select="graphml:get_data_value($vector, 'array_size')" />
                <xsl:value-of select="lower-case(o:join_list(('array', $vector_child_kind, $idl_type, $array_size), '_'))" />
            </xsl:when>
            <xsl:otherwise>
                <xsl:variable name="sequence_size" select="graphml:get_data_value($vector, 'sequence_size')" />
                <xsl:value-of select="lower-case(o:join_list(('sequence', $vector_child_kind, $idl_type, $sequence_size), '_'))" />
            </xsl:otherwise>
        </xsl:choose>
    </xsl:function>

    <xsl:function name="cdit:get_vector_inner_idl_type">
        <xsl:param name="vector" />
        <xsl:variable name="vector_child" select="graphml:get_vector_child($vector)" />
        <xsl:variable name="kind" select="graphml:get_kind($vector_child)" />
        <xsl:variable name="type" select="graphml:get_type($vector_child)" />

        <xsl:choose>
            <xsl:when test="$kind = 'AggregateInstance'">
                <xsl:value-of select="idl:get_aggregate_qualified_type(graphml:get_definition($vector_child))" />
            </xsl:when>
            <xsl:when test="$kind = 'Member'">
                <xsl:variable name="cpp_type" select="cpp:get_primitive_type($type)" />
                <xsl:value-of select="idl:get_type($cpp_type)" />
            </xsl:when>
            <xsl:when test="$kind = 'EnumInstance'">
                <xsl:value-of select="idl:get_aggregate_qualified_type(graphml:get_definition($vector_child))" />
            </xsl:when>
        </xsl:choose>
    </xsl:function>


    <xsl:function name="cdit:get_idl_struct">
        <xsl:param name="aggregate" />
        <xsl:param name="tab" as="xs:integer" />

        <xsl:variable name="aggregate_label" select="graphml:get_label($aggregate)" />
        <xsl:variable name="aggregate_namespace" select="graphml:get_namespace($aggregate)" />
        <xsl:variable name="label" select="o:title_case($aggregate_label)" />

        <xsl:for-each select="graphml:get_child_nodes($aggregate)">
            <xsl:variable name="kind" select="graphml:get_kind(.)" />
            <xsl:if test="$kind = 'Vector'">
                <xsl:variable name="vector_inner_type" select="cdit:get_vector_inner_idl_type(.)" />
                <xsl:variable name="is_array" select="graphml:evaluate_data_value_as_boolean(., 'is_array')" />
                
                <xsl:variable name="sequence_label" select="cdit:get_idl_type_def_name(.)" />
                <xsl:variable name="define_guard_name" select="upper-case(o:join_list(('TYPEDEF', $sequence_label), '_'))" />

                <!-- Define Guard -->
                <xsl:value-of select="cpp:define_guard_start($define_guard_name)" />
                <xsl:choose>
                    <xsl:when test="$is_array">
                        <xsl:variable name="array_size" select="graphml:get_data_value(., 'array_size')" />
                        <xsl:value-of select="idl:typedef($vector_inner_type, concat($sequence_label, o:wrap_square($array_size)), $tab)" />
                    </xsl:when>
                    <xsl:otherwise>
                        <xsl:variable name="sequence_size" select="graphml:get_data_value(., 'sequence_size')" />
                        <xsl:value-of select="idl:typedef(idl:sequence_type($vector_inner_type, $sequence_size), $sequence_label, $tab)" />
                    </xsl:otherwise>
                </xsl:choose>
                <xsl:value-of select="cpp:define_guard_end($define_guard_name)" />
                <xsl:value-of select="o:nl(1)" />
            </xsl:if>
        </xsl:for-each>
        

        <xsl:value-of select="idl:struct($label, $tab)" />
        
        <xsl:for-each select="graphml:get_child_nodes($aggregate)">
            <xsl:variable name="kind" select="graphml:get_kind(.)" />
            <xsl:variable name="index" select="graphml:get_index(.) + 1" />
            <xsl:variable name="label" select="lower-case(graphml:get_label(.))" />
            <xsl:variable name="type" select="graphml:get_type(.)" />
            <xsl:variable name="is_key" select="graphml:is_key(.)" />
            <xsl:variable name="primitive_type" select="graphml:get_data_value(., 'primitive_type')" />

            <xsl:choose>    
                <xsl:when test="$kind = 'Member' and $primitive_type">
                    <xsl:value-of select="idl:member($primitive_type, $label, $is_key, $tab + 1)" />
                </xsl:when>
                <xsl:when test="$kind = 'Member'">
                    <xsl:variable name="cpp_type" select="cpp:get_primitive_type($type)" />
                    <xsl:variable name="idl_type" select="idl:get_type($cpp_type)" />
                    <xsl:value-of select="idl:member($idl_type, $label, $is_key, $tab + 1)" />
                </xsl:when>
                <xsl:when test="$kind = 'EnumInstance'">
                    <xsl:variable name="idl_type" select="idl:get_aggregate_qualified_type(graphml:get_definition(.))" />
                    <xsl:value-of select="idl:member($idl_type, $label, $is_key, $tab + 1)" />
                </xsl:when>
                <xsl:when test="$kind = 'AggregateInstance'">
                    <xsl:variable name="idl_type" select="idl:get_aggregate_qualified_type(graphml:get_definition(.))" />
                    <xsl:value-of select="idl:member($idl_type, $label, $is_key, $tab + 1)" />
                </xsl:when>
                <xsl:when test="$kind = 'Vector'">
                    <xsl:variable name="vector_child" select="graphml:get_vector_child(.)" />

                    <xsl:variable name="sequence_type" select="cdit:get_idl_type_def_name(.)" />
                    <xsl:value-of select="idl:member($sequence_type, $label, $is_key, $tab + 1)" />
                </xsl:when>
            </xsl:choose>
        </xsl:for-each>
        
        <xsl:value-of select="cpp:scope_end($tab)" />

        <!-- Set the pragma of the keys -->
        <xsl:for-each select="graphml:get_child_nodes($aggregate)">
            <xsl:variable name="is_key" select="graphml:is_key(.)" />
            <xsl:variable name="child_label" select="lower-case(graphml:get_label(.))" />
            <xsl:if test="$is_key">
                <xsl:value-of select="idl:key_pragma($label, $child_label, $tab)" />
            </xsl:if>
        </xsl:for-each>

        <xsl:value-of select="o:nl(1)" />
    </xsl:function>

    <xsl:function name="cdit:get_union_descriminator_case">
        <xsl:param name="union_member" as="element()" />
        <xsl:param name="middleware" as="xs:string" />
        
        <xsl:variable name="parent_node" select="graphml:get_parent_node($union_member)" />
        <xsl:variable name="enum_type" select="cdit:get_qualified_union_descrimantor_type($parent_node, $middleware)" />
        <xsl:variable name="case_label">
            <xsl:choose>
                <xsl:when test="lower-case($middleware) = 'base'">
                    <xsl:value-of select="upper-case(graphml:get_label($union_member))" />
                </xsl:when>
                <xsl:otherwise>
                    <xsl:value-of select="cdit:get_idl_union_case($union_member)" />
                </xsl:otherwise>
            </xsl:choose>
        </xsl:variable>

        <xsl:value-of select="concat('::', cpp:combine_namespaces(($enum_type, $case_label)))" />

    </xsl:function>

    <xsl:function name="cdit:get_idl_union_enum_name">
        <xsl:param name="aggregate" />

        <xsl:variable name="aggregate_label" select="o:title_case(graphml:get_label($aggregate))" />
        <xsl:variable name="aggregate_namespace" select="graphml:get_namespace($aggregate)" />
        <xsl:value-of select="o:join_list(($aggregate_namespace, $aggregate_label, 'Descriminator'), '_')" />
    </xsl:function>
    
    <xsl:function name="cdit:get_idl_union_prefix">
        <xsl:param name="aggregate" />
        <xsl:value-of select="upper-case(cdit:get_idl_union_enum_name($aggregate))" />
    </xsl:function>

    <xsl:function name="cdit:get_idl_union_case">
        <xsl:param name="union_member" />

        <xsl:variable name="aggregate" select="graphml:get_parent_node($union_member)" />
        
        <xsl:variable name="enum_val_prefix" select="cdit:get_idl_union_prefix($aggregate)" />
        <xsl:variable name="member_label" select="upper-case(graphml:get_label($union_member))" />

        <xsl:value-of select="o:join_list(($enum_val_prefix, $member_label), '_')" />
    </xsl:function>


    <xsl:function name="cdit:get_idl_union">
        <xsl:param name="aggregate" />
        <xsl:param name="tab" as="xs:integer" />

        <xsl:variable name="label" select="o:title_case(graphml:get_label($aggregate))" />
        <xsl:variable name="enum_label" select="cdit:get_idl_union_enum_name($aggregate)" />
        <xsl:variable name="enum_val_prefix" select="cdit:get_idl_union_prefix($aggregate)" />


        <xsl:variable name="enum_members" as="xs:string*">
            <xsl:variable name="unset_label" select="o:join_list(($enum_val_prefix, 'UNSET'), '_')" />
            <xsl:sequence select="$unset_label" />
            <xsl:for-each select="graphml:get_child_nodes($aggregate)">
                <xsl:sequence select="cdit:get_idl_union_case(.)" />
            </xsl:for-each>
        </xsl:variable>

        <xsl:value-of select="idl:get_enum('', $enum_label, $enum_members, $tab)" />

        <xsl:value-of select="idl:union($label, $enum_label, $tab)" />
            <xsl:for-each select="graphml:get_child_nodes($aggregate)">
                <xsl:variable name="kind" select="graphml:get_kind(.)" />
                <xsl:variable name="index" select="graphml:get_index(.) + 1" />
                <xsl:variable name="label" select="lower-case(graphml:get_label(.))" />
                <xsl:variable name="type" select="graphml:get_type(.)" />
                <xsl:variable name="is_key" select="graphml:is_key(.)" />

                <xsl:variable name="enum_val" select="upper-case(o:join_list(($enum_val_prefix, $label), '_'))" />
                

                <xsl:choose>    
                    <xsl:when test="$kind = 'Member'">
                        <xsl:variable name="cpp_type" select="cpp:get_primitive_type($type)" />
                        <xsl:variable name="idl_type" select="idl:get_type($cpp_type)" />
                        <xsl:value-of select="idl:union_member($enum_val, $idl_type, $label, $tab + 1)" />
                    </xsl:when>
                    <xsl:when test="$kind = 'EnumInstance'">
                        <xsl:variable name="idl_type" select="idl:get_aggregate_qualified_type(graphml:get_definition(.))" />
                        <xsl:value-of select="idl:union_member($enum_val, $idl_type, $label, $tab + 1)" />
                    </xsl:when>
                    <xsl:when test="$kind = 'AggregateInstance'">
                        <xsl:variable name="idl_type" select="idl:get_aggregate_qualified_type(graphml:get_definition(.))" />
                        <xsl:value-of select="idl:union_member($enum_val, $idl_type, $label, $tab + 1)" />
                    </xsl:when>
                </xsl:choose>
            </xsl:for-each>
        <xsl:value-of select="cpp:scope_end($tab)" />

        <!-- Set the pragma of the keys -->
        <xsl:for-each select="graphml:get_child_nodes($aggregate)">
            <xsl:variable name="is_key" select="graphml:is_key(.)" />
            <xsl:variable name="child_label" select="lower-case(graphml:get_label(.))" />
            <xsl:if test="$is_key">
                <xsl:value-of select="idl:key_pragma($label, $child_label, $tab)" />
            </xsl:if>
        </xsl:for-each>

        <xsl:value-of select="o:nl(1)" />
    </xsl:function>

    <xsl:function name="cdit:get_aggregate_base_h">
        <xsl:param name="aggregate" as="element()" />

        <xsl:variable name="aggregate_namespace" select="('Base', graphml:get_namespace($aggregate))" />
        <xsl:variable name="aggregate_label" select="graphml:get_label($aggregate)" />
        <xsl:variable name="class_name" select="o:title_case($aggregate_label)" />
        <xsl:variable name="tab" select="count($aggregate_namespace)" />

        <xsl:variable name="is_union" select="graphml:evaluate_data_value_as_boolean($aggregate, 'is_union')" />
        
        

        <xsl:variable name="relative_path" select="cmake:get_relative_path(($aggregate_namespace, $aggregate_label))" />
        

        <!-- Version Number -->
        <xsl:value-of select="cpp:print_regen_version('datatype_functions.xsl', 'cdit:get_aggregate_base_h', 0)" />

        <!-- Get all required aggregates -->
        <xsl:variable name="required_aggregates" select="cdit:get_required_aggregates($aggregate, true())" />

        <xsl:variable name="define_guard_name" select="upper-case(o:join_list(($aggregate_namespace, $aggregate_label), '_'))" />

        <xsl:variable name="children" select="graphml:get_child_nodes($aggregate)" />
        <xsl:variable name="enums" select="cdit:get_required_enums($aggregate)" />

        <!-- Define Guard -->
        <xsl:value-of select="cpp:define_guard_start($define_guard_name)" />
        
        <!-- Library Includes-->
        <xsl:value-of select="cpp:include_library_header(o:join_paths(('core', 'basemessage.h')))" />
        <xsl:value-of select="cpp:include_library_header('string')" />

        <!-- Include Vector -->
        <xsl:if test="count(graphml:get_child_nodes_of_kind($aggregate, 'Vector')) > 0">
            <xsl:value-of select="cpp:include_library_header('vector')" />
        </xsl:if>

        <xsl:value-of select="cdit:include_aggregate_headers($required_aggregates, 'base')" />
        <xsl:value-of select="cdit:include_enum_headers($enums)" />
        <xsl:value-of select="o:nl(1)" />

        <!-- Define Namespaces -->
        <xsl:for-each select="$aggregate_namespace">
            <xsl:value-of select="cpp:namespace_start(., position() - 1)" />
        </xsl:for-each>
        
        <xsl:value-of select="cpp:declare_class($class_name, 'public ::BaseMessage', $tab + 1)" />

        <!-- Define Union functions -->
        <xsl:if test="$is_union">
            <xsl:value-of select="cdit:declare_union_functions($aggregate, $tab + 1)" />
        </xsl:if>
        
        <!-- Define functions -->
        <xsl:for-each select="$children">
            <xsl:value-of select="cdit:declare_datatype_functions(., $tab + 1)" />
        </xsl:for-each>

        <xsl:value-of select="cpp:scope_end($tab + 1)" />

        <!-- End Namespaces -->
        <xsl:for-each select="$aggregate_namespace">
            <xsl:sort select="position()" data-type="number" order="descending"/>
            <xsl:value-of select="cpp:namespace_end(., count($aggregate_namespace) - position())" />
        </xsl:for-each>

        <xsl:value-of select="o:nl(1)" />
        <xsl:value-of select="cpp:define_guard_end($define_guard_name)" />
    </xsl:function>

    

    

    <xsl:function name="cdit:get_port_pubsub_export">
        <xsl:param name="aggregate" as="element()" />
        <xsl:param name="middleware" as="xs:string" />

        <xsl:variable name="aggregate_label" select="lower-case(graphml:get_label($aggregate))" />
        <xsl:variable name="aggregate_namespace" select="graphml:get_namespace($aggregate)" />
        <xsl:variable name="datatype_middleware" select="cdit:get_datatype_middleware($middleware)" />
        

        <!-- Preamble -->
        <xsl:value-of select="cpp:print_regen_version('datatype_functions.xsl', 'cdit:get_port_pubsub_export', 0)" />
        
        <xsl:value-of select="cpp:include_library_header(o:join_paths(('core', 'ports', 'libportexport.h')))" />
        <xsl:value-of select="o:nl(1)" />
        <!-- TODO -->

        <!-- Include the base message type -->
        <xsl:value-of select="cdit:include_aggregate_headers($aggregate, 'base')" />
        <xsl:value-of select="cdit:include_aggregate_headers($aggregate, $datatype_middleware)" />
        <xsl:value-of select="o:nl(1)" />

        <xsl:value-of select="cpp:comment(('Include the', $middleware, 'specific templated classes'), 0)" />
        <xsl:value-of select="cpp:include_library_header(o:join_paths(('middleware', $middleware, 'pubsub', 'publisherport.hpp')))" />
        <xsl:value-of select="cpp:include_library_header(o:join_paths(('middleware', $middleware, 'pubsub', 'subscriberport.hpp')))" />
        <xsl:value-of select="o:nl(1)" />

        <xsl:variable name="base_type" select="cdit:get_aggregate_qualified_type($aggregate, 'base')" />
        <xsl:variable name="middleware_type" select="cdit:get_aggregate_qualified_type($aggregate, $middleware)" />
        <xsl:variable name="port_type" select="cpp:join_args(($base_type, $middleware_type))" />

        <xsl:variable name="func_args" select="cpp:join_args((cpp:const_ref_var_def('std::string', 'port_name'), cpp:declare_variable(cpp:weak_ptr('Component'), 'component', '', 0)))" />


        <xsl:variable name="subscriber_port_type" select="cpp:templated_type(cpp:combine_namespaces(($middleware, 'SubscriberPort')), $port_type)" />
        <xsl:variable name="publisher_port_type" select="cpp:templated_type(cpp:combine_namespaces(($middleware, 'PublisherPort')), $port_type)" />

        <xsl:value-of select="cpp:define_function(cpp:pointer_var_def('Port', ''), '', 'ConstructSubscriberPort', $func_args, cpp:scope_start(0))" />
        <xsl:variable name="inport_func" select="cpp:invoke_templated_static_function($subscriber_port_type, 'ConstructSubscriberPort', cpp:join_args(('port_name', 'component')), '', 0)" />
        <xsl:value-of select="cpp:return($inport_func, 1)" />
        <xsl:value-of select="cpp:scope_end(0)" />
        <xsl:value-of select="o:nl(1)" />

        <xsl:value-of select="cpp:define_function(cpp:pointer_var_def('Port', ''), '', 'ConstructPublisherPort', $func_args, cpp:scope_start(0))" />
        <xsl:variable name="outport_func" select="cpp:invoke_templated_static_function($publisher_port_type, 'ConstructPublisherPort', cpp:join_args(('port_name', 'component')), '', 0)" />
        <xsl:value-of select="cpp:return($outport_func, 1)" />
        <xsl:value-of select="cpp:scope_end(0)" />
    </xsl:function>

   
    <xsl:function name="cdit:get_port_requestreply_export">
        <xsl:param name="server_interface" as="element()" />
        <xsl:param name="middleware" as="xs:string" />

        
        <xsl:variable name="datatype_middleware" select="cdit:get_datatype_middleware($middleware)" />

        <xsl:variable name="server_label" select="lower-case(graphml:get_label($server_interface))" />
        <xsl:variable name="server_namespace" select="graphml:get_namespace($server_interface)" />

        <xsl:variable name="server_requests" select="cdit:get_server_interface_request_aggregates($server_interface)" />
        <xsl:variable name="server_replies" select="cdit:get_server_interface_reply_aggregates($server_interface)" />

        <xsl:variable name="aggregate_instances" select="graphml:get_definitions(($server_requests, $server_replies))" />

        
        <!-- Preamble -->
        <xsl:value-of select="cpp:print_regen_version('datatype_functions.xsl', 'cdit:get_port_requestreply_export', 0)" />
        
        <xsl:value-of select="cpp:include_library_header(o:join_paths(('core', 'ports', 'libportexport.h')))" />
        <xsl:value-of select="o:nl(1)" />

        
        <xsl:value-of select="cdit:include_aggregate_headers($aggregate_instances, 'base')" />
        <xsl:value-of select="cdit:include_aggregate_headers($aggregate_instances, $datatype_middleware)" />


        <xsl:if test="$middleware = 'tao'">
            <xsl:variable name="server_header" select="cdit:get_middleware_generated_header_name($server_interface, $middleware)" />
            <xsl:value-of select="cpp:include_local_header($server_header)" />
        </xsl:if>




        <xsl:value-of select="o:nl(1)" />
        <xsl:value-of select="cpp:comment(('Include the', $middleware, 'specific templated classes'), 0)" />
        <xsl:if test="$middleware = 'tao'">
            <xsl:variable name="function_name" select="graphml:get_data_value($server_interface, 'function_name')" />
            <xsl:value-of select="cpp:define('TAO_SERVER_FUNC_NAME', $function_name)" />
        </xsl:if>
        <xsl:value-of select="cpp:include_library_header(o:join_paths(('middleware', $middleware, 'requestreply', 'requesterport.hpp')))" />
        <xsl:value-of select="cpp:include_library_header(o:join_paths(('middleware', $middleware, 'requestreply', 'replierport.hpp')))" />
        <xsl:if test="$middleware = 'tao'">
            <xsl:value-of select="cpp:undef('TAO_SERVER_FUNC_NAME')" />
        </xsl:if>
        <xsl:value-of select="o:nl(1)" />


         <!--
        <xsl:variable name="base_type" select="cdit:get_aggregate_qualified_type($aggregate, 'base')" />
        <xsl:variable name="middleware_type" select="cdit:get_aggregate_qualified_type($aggregate, $middleware)" />
        $base_type, $middleware_type))" /> -->
        
        <xsl:variable name="port_types" as="xs:string*">
            <xsl:sequence select="cdit:get_qualified_reply_type_for_server_interface($server_interface, 'base')" />
            <xsl:sequence select="cdit:get_qualified_reply_type_for_server_interface($server_interface, $datatype_middleware)" />
            <xsl:sequence select="cdit:get_qualified_request_type_for_server_interface($server_interface, 'base')" />
            <xsl:sequence select="cdit:get_qualified_request_type_for_server_interface($server_interface, $datatype_middleware)" />
        </xsl:variable>

        <xsl:variable name="requester_port_types" as="xs:string*">
            <xsl:sequence select="$port_types" />

            <xsl:if test="$middleware = 'tao'">
                <xsl:sequence select="cpp:get_client_qualified_type($server_interface, $datatype_middleware)" />
            </xsl:if>
        </xsl:variable>
        
        <xsl:variable name="replier_port_types" as="xs:string*">
            <xsl:sequence select="$port_types" />

            <xsl:if test="$middleware = 'tao'">
                <xsl:sequence select="cpp:get_server_qualified_type($server_interface, $datatype_middleware)" />
            </xsl:if>
        </xsl:variable>

        


        <xsl:variable name="port_type" select="cpp:join_args($port_types)" />
        <xsl:variable name="func_args" select="cpp:join_args((cpp:const_ref_var_def('std::string', 'port_name'), cpp:declare_variable(cpp:weak_ptr('Component'), 'component', '', 0)))" />


        <xsl:variable name="requester_port_type" select="cpp:templated_type(cpp:combine_namespaces(($middleware, 'RequesterPort')), cpp:join_args($requester_port_types))" />
        <xsl:variable name="replier_port_type" select="cpp:templated_type(cpp:combine_namespaces(($middleware, 'ReplierPort')),cpp:join_args($replier_port_types))" />

        <xsl:value-of select="cpp:define_function(cpp:pointer_var_def('Port', ''), '', 'ConstructRequesterPort', $func_args, cpp:scope_start(0))" />
        <xsl:variable name="inport_func" select="cpp:invoke_templated_static_function($requester_port_type, 'ConstructRequesterPort', cpp:join_args(('port_name', 'component')), '', 0)" />
        <xsl:value-of select="cpp:return($inport_func, 1)" />
        <xsl:value-of select="cpp:scope_end(0)" />
        <xsl:value-of select="o:nl(1)" />

        <xsl:value-of select="cpp:define_function(cpp:pointer_var_def('Port', ''), '', 'ConstructReplierPort', $func_args, cpp:scope_start(0))" />
        <xsl:variable name="outport_func" select="cpp:invoke_templated_static_function($replier_port_type, 'ConstructReplierPort', cpp:join_args(('port_name', 'component')), '', 0)" />
        <xsl:value-of select="cpp:return($outport_func, 1)" />
        <xsl:value-of select="cpp:scope_end(0)" />
    </xsl:function>




    <xsl:function name="cdit:get_aggregate_base_cpp">
        <xsl:param name="aggregate" as="element()" />

        <xsl:variable name="aggregate_label" select="graphml:get_label($aggregate)" />
        <xsl:variable name="class_type" select="cdit:get_aggregate_qualified_type($aggregate, 'base')" />
        <xsl:variable name="is_union" select="graphml:evaluate_data_value_as_boolean($aggregate, 'is_union')" />
        
        <xsl:variable name="header_file" select="cdit:get_base_aggregate_h_name($aggregate)" />
        
        <!-- Version Number -->
        <xsl:value-of select="cpp:print_regen_version('datatype_functions.xsl', 'cdit:get_aggregate_base_cpp', 0)" />

        <xsl:value-of select="cpp:include_local_header($header_file)" />
        <xsl:value-of select="o:nl(1)" />

        <xsl:if test="$is_union">
            <xsl:value-of select="cpp:include_library_header('iostream')" />
            <xsl:value-of select="o:nl(1)" />
            <xsl:value-of select="cdit:define_union_functions($aggregate, $class_type)" />
        </xsl:if>

        <!-- Define functions -->
        <xsl:for-each select="graphml:get_child_nodes($aggregate)">
            <xsl:value-of select="cdit:define_datatype_functions($aggregate, ., $class_type)" />
        </xsl:for-each>


    </xsl:function>

    <xsl:function name="cdit:get_aggregate_base_cmake">
        <xsl:param name="aggregate" as="element()" />

        <xsl:variable name="aggregate_label" select="graphml:get_label($aggregate)" />
        <xsl:variable name="aggregate_namespace" select="graphml:get_namespace($aggregate)" />
        
        <xsl:variable name="class_type" select="cdit:get_aggregate_qualified_type($aggregate, 'base')" />
        
        <xsl:variable name="file_name" select="cdit:get_aggregate_file_prefix($aggregate, 'base')" />

        <xsl:variable name="aggregate_h" select="concat($file_name, '.h')" />
        <xsl:variable name="aggregate_cpp" select="concat($file_name, '.cpp')" />

        <xsl:variable name="shared_lib_name" select="cmake:get_aggregate_shared_library_name($aggregate, 'base')" />
        <xsl:variable name="proj_name" select="$shared_lib_name" />

        <xsl:variable name="source_dir_var" select="cmake:current_source_dir_var()" />


        <!-- Version Number -->
        <xsl:value-of select="cmake:print_regen_version('datatype_functions.xsl', 'cdit:get_aggregate_base_cmake', 0)" />
        <xsl:value-of select="cmake:set_project_name($proj_name)" />

        <!-- Find boost -->
        <xsl:value-of select="cmake:find_package('Boost', 'COMPONENTS thread system filesystem REQUIRED', 0)" />
        <xsl:value-of select="o:nl(1)" />

        <!-- Set Source files -->
        <xsl:value-of select="concat('set(SOURCE', o:nl(1))" />
        <xsl:value-of select="concat(o:t(1), o:join_paths(($source_dir_var, $aggregate_cpp)), o:nl(1))" />
        <xsl:value-of select="concat(o:t(0), ')', o:nl(1))" />
        <xsl:value-of select="o:nl(1)" />

        <!-- Set Header files -->
        <xsl:value-of select="concat('set(HEADERS', o:nl(1))" />
        <xsl:value-of select="concat(o:t(1), o:join_paths(($source_dir_var, $aggregate_h)), o:nl(1))" />
        <xsl:value-of select="concat(o:t(0), ')', o:nl(1))" />
        <xsl:value-of select="o:nl(1)" />

        <!-- Include export files -->
        <xsl:value-of select="cmake:include_zmq_exports(0)"/>
        <xsl:value-of select="cmake:include_cppzmq_exports(0)"/>
        <xsl:value-of select="cmake:include_protobuf_exports(0)"/>
        <xsl:value-of select="cmake:include_sem_exports(0)"/>

        <!--  Create library -->
        <xsl:variable name="args" select="o:join_list((cmake:wrap_variable('SOURCE'), cmake:wrap_variable('HEADERS')), ' ')" />
        <xsl:value-of select="cmake:add_library('PROJ_NAME', 'STATIC', $args)" />
        <xsl:value-of select="o:nl(1)" />

        <!-- Include Runtime Environment -->
        <xsl:value-of select="cmake:comment('Include Top Level Dirs', 0)" />
        <xsl:value-of select="cmake:target_include_directories('PROJ_NAME', 'PRIVATE', cmake:wrap_variable('MODEL_SOURCE_DIR'), 0)" />
        <xsl:value-of select="cmake:target_include_directories('PROJ_NAME', 'PRIVATE', cmake:wrap_variable('RE_SOURCE_DIR'), 0)" />

        <!-- Link Runtime Environment -->
        <xsl:value-of select="cmake:comment('Link against re_core', 0)" />
        <xsl:value-of select="cmake:target_link_libraries('PROJ_NAME', 'PRIVATE', 'sem::re_core', 0)" />
        <xsl:value-of select="cmake:target_link_libraries('PROJ_NAME', 'PRIVATE', 'sem::re_core_singletons', 0)" />
        <xsl:value-of select="o:nl(1)" />

        <!-- Include the required aggregate files -->
        <xsl:for-each select="cdit:get_required_aggregates($aggregate, true())">
            <xsl:if test="position() = 1">
                <xsl:value-of select="cmake:comment('Link against required aggregates libraries', 0)" />
            </xsl:if>
            <xsl:variable name="required_lib_name" select="cmake:get_aggregate_shared_library_name(., 'base')" />
            <xsl:value-of select="cmake:target_link_libraries('PROJ_NAME', 'PUBLIC', $required_lib_name, 0)" />
            <xsl:if test="position() = last()">
                <xsl:value-of select="o:nl(1)" />
            </xsl:if>
        </xsl:for-each>
    </xsl:function>

    <xsl:function name="cdit:get_middleware_cmake">
        <xsl:param name="aggregates" as="element()*" />

        <!-- Include the required aggregate folders -->
        <xsl:variable name="sub_directories" as="xs:string*">
            <xsl:for-each select="$aggregates">
                <xsl:sequence select="cdit:get_namespace_type_path(.)" />
            </xsl:for-each>
        </xsl:variable>

        <!-- Version Number -->
        <xsl:value-of select="cmake:print_regen_version('datatype_functions.xsl', 'cdit:get_middleware_cmake', 0)" />

        <xsl:value-of select="cmake:add_subdirectories($sub_directories)" />
    </xsl:function>

    <xsl:function name="cdit:get_directories_cmake">
        <xsl:param name="folders" as="xs:string*" />

        <!-- Version Number -->
        <xsl:value-of select="cmake:print_regen_version('datatype_functions.xsl', 'cdit:get_directories_cmake', 0)" />

        <xsl:value-of select="cmake:add_subdirectories($folders)" />
    </xsl:function>
    

    <xsl:function name="cdit:get_enum_h">
        <xsl:param name="enum" as="element()" />
        
        <!-- Enum's are always in the namespace 'Base' -->
        <xsl:variable name="namespaces" select="('Base', graphml:get_namespace($enum))" />
        <xsl:variable name="tab" select="count($namespaces)" />
        
        <!-- Get the label -->
        <xsl:variable name="label" select="graphml:get_label($enum)" />
        <xsl:variable name="class_type" select="o:title_case($label)" />
        <xsl:variable name="qualified_class_type" select="cpp:combine_namespaces(($namespaces, $class_type))" />

        <xsl:variable name="enum_members" select="graphml:get_child_nodes_of_kind($enum, 'EnumMember')" />

        
        <xsl:variable name="define_guard_name" select="upper-case(o:join_list(('ENUM', $namespaces, $class_type), '_'))" />

        <!-- Preamble -->
        <xsl:value-of select="cpp:print_regen_version('datatype_functions.xsl', 'cdit:get_enum_h', 0)" />

        <!-- Define Guard -->
        <xsl:value-of select="cpp:define_guard_start($define_guard_name)" />
        <!-- Include string -->
        <xsl:value-of select="cpp:include_library_header('string')" />
        <xsl:value-of select="o:nl(1)" />

        <!-- Define the Namespaces -->
        <xsl:value-of select="cpp:define_namespaces($namespaces)" />


        <!-- Define the Enum -->
        <xsl:value-of select="cpp:enum($class_type, $tab)" />

        <xsl:for-each select="$enum_members">
            <xsl:variable name="member_label" select="upper-case(graphml:get_label(.))" />
            <xsl:value-of select="cpp:enum_value($member_label, position() - 1, position() = last(), $tab + 1)" />
        </xsl:for-each>

        <xsl:value-of select="cpp:scope_end($tab)" />

        <!-- End Namespaces -->
        <xsl:value-of select="cpp:close_namespaces($namespaces)" />

        <!-- Define to_string function -->
        <xsl:value-of select="o:nl(1)" />
        <xsl:value-of select="cpp:define_function('inline std::string', '', 'to_string', cpp:const_ref_var_def($qualified_class_type, 'value'), cpp:scope_start(0))" />
        <xsl:value-of select="cpp:switch('value', 1)" />

        <xsl:for-each select="$enum_members">
            <xsl:variable name="enum_type" select="cdit:get_qualified_enum_member_type(.)" />
            <xsl:variable name="enum_label" select="upper-case(graphml:get_label(.))" />
            <xsl:value-of select="cpp:switch_case($enum_type, 2)" />
            <xsl:value-of select="cpp:return(o:wrap_dblquote($enum_label), 3)" />
            <xsl:value-of select="cpp:scope_end(2)" />
        </xsl:for-each>
        <xsl:value-of select="cpp:switch_default(2)" />
        <xsl:value-of select="cpp:return(o:wrap_dblquote('UNKNOWN_TYPE'), 3)" />
        <xsl:value-of select="cpp:scope_end(2)" />
        <xsl:value-of select="cpp:scope_end(1)" />
        <xsl:value-of select="cpp:scope_end(0)" />
        
        <!-- Define to_integer function -->
        <xsl:value-of select="o:nl(1)" />
        <xsl:value-of select="cpp:define_function('inline int', '', 'to_integer', cpp:const_ref_var_def($qualified_class_type, 'value'), cpp:scope_start(0))" />
        <xsl:value-of select="cpp:return(cpp:static_cast('int', 'value'), 1)" />
        <xsl:value-of select="cpp:scope_end(0)" />

        <xsl:value-of select="cpp:define_guard_end($define_guard_name)" />
    </xsl:function>

    <xsl:function name="cpp:declare_aggregate_member_functions">
        <xsl:param name="node" as="element()" />
        <xsl:param name="tab" />

        <xsl:variable name="label" select="graphml:get_label($node)" />
        <xsl:variable name="kind" select="graphml:get_kind($node)" />
        <xsl:variable name="cpp_type" select="cpp:get_qualified_type($node)" />
        <xsl:variable name="var_label" select="cdit:get_variable_label($label, $node)" />

        <xsl:value-of select="cpp:comment((o:wrap_square($kind), ':', $label, o:wrap_angle(graphml:get_id($node))), $tab)" />
        <xsl:choose>
            <xsl:when test="$cpp_type != ''">
                <!-- Public Declarations -->
                <xsl:value-of select="cpp:public($tab)" />
                <xsl:value-of select="cpp:declare_function('void', concat('set_', $label), cpp:const_ref_var_def($cpp_type, 'value'), ';', $tab + 1)" />
                <xsl:value-of select="cpp:declare_function(cpp:const_ref_var_def($cpp_type, ''), concat('get_', $label), '', ' const;', $tab + 1)" />
                <xsl:value-of select="cpp:declare_function(cpp:ref_var_def($cpp_type, ''), $label, '', ';', $tab + 1)" />
                <!-- Private Declarations -->
                <xsl:value-of select="cpp:private($tab)" />
                <xsl:value-of select="cpp:declare_variable($cpp_type, $var_label, cpp:nl(), $tab + 1)" />
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="cpp:comment('Cannot find valid CPP type for this element', $tab)" />
            </xsl:otherwise>
        </xsl:choose>
    </xsl:function>

    <xsl:function name="cdit:translate_primitive_member">
        <xsl:param name="member" as="element()" />
        <xsl:param name="get_func" as="xs:string" />
        <xsl:param name="middleware" as="xs:string" />
        <xsl:param name="source_middleware" as="xs:string" />
        <xsl:param name="target_middleware" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />

        <xsl:variable name="temp_variable" select="cdit:get_variable_name($member)" />

        <xsl:variable name="primitive_type" select="graphml:get_data_value($member, 'primitive_type')" />
        
        <xsl:variable name="base_type" select="cpp:get_primitive_type(graphml:get_type($member))" />
        <xsl:variable name="other_type" select="cpp:get_corba_primitive_type($primitive_type)" />
        
        <xsl:variable name="value">
            <xsl:choose>
                <xsl:when test="$base_type != $other_type and $other_type != ''">
                    <xsl:variable name="func_type" select="if($source_middleware = 'base') then cpp:join_args(($base_type, $other_type)) else cpp:join_args(($other_type, $base_type))" />
                    <xsl:value-of select="cpp:invoke_templated_static_function($func_type, 'TranslatePrimitive', $get_func, '', 0)" />
                </xsl:when>
                <xsl:otherwise>
                    <xsl:value-of select="$get_func" />
                </xsl:otherwise>
            </xsl:choose>
        </xsl:variable>
        
        <xsl:value-of select="cpp:define_variable(cpp:const_ref_auto(), $temp_variable, $value, cpp:nl(), $tab)" />
    </xsl:function>


    <xsl:function name="cdit:translate_member">
        <xsl:param name="member" as="element()" />
        <xsl:param name="middleware" as="xs:string" />
        <xsl:param name="source_middleware" as="xs:string" />
        <xsl:param name="target_middleware" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />
        
        <xsl:variable name="temp_variable" select="cdit:get_variable_name($member)" />
        <xsl:variable name="get_func" select="cdit:invoke_middleware_get_function('value', cpp:dot(), $member, $source_middleware)" />
        
        <xsl:value-of select="cpp:try($tab)" />

        <xsl:value-of select="cdit:translate_primitive_member($member, $get_func, $middleware, $source_middleware, $target_middleware, $tab + 1)" />
        <xsl:variable name="set_func" select="cdit:invoke_middleware_set_function('out', cpp:arrow(), $member, $target_middleware, $temp_variable)" />
        <xsl:value-of select="concat(o:t($tab +1), $set_func, cpp:nl())" />
        
        <!-- Catch Translation of Exceptions -->
        <xsl:value-of select="cpp:catch_exception('}', 'std::exception', 'ex', $tab)" />
        <!-- Define a string to throw -->
        <xsl:variable name="str_args" as="xs:string*">
            <xsl:sequence select="
            o:wrap_dblquote(concat(
            'Translating Member ',
            o:wrap_quote(graphml:get_label($member)),
            ' ',
            o:wrap_square(graphml:get_type($member)),
            ' Failed: '
            ))" />
        </xsl:variable>

        <xsl:value-of select="cpp:define_variable('std::string', 'error_str', o:join_list($str_args, ' + '), cpp:nl(), $tab + 1)" />
        <xsl:value-of select="cpp:throw('std::invalid_argument', 'error_str + ex.what()', $tab + 1)" />
        <xsl:value-of select="cpp:scope_end($tab)" />
        <xsl:value-of select="o:nl(1)" />
    </xsl:function>

    <xsl:function name="cdit:translate_enum_instance">
        <xsl:param name="enum_instance" as="element()" />
        <xsl:param name="middleware" as="xs:string" />
        <xsl:param name="source_middleware" as="xs:string" />
        <xsl:param name="target_middleware" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />

        <!-- Get the type of the Enum -->
        <xsl:variable name="get_func" select="cdit:invoke_middleware_get_function('value', cpp:dot(), $enum_instance, $source_middleware)" />
        
        <xsl:value-of select="cpp:scope_start($tab)" />
        
        <xsl:value-of select="cdit:cast_enum_instance($enum_instance, $get_func, $middleware, $source_middleware, $target_middleware, $tab + 1)" />
        
        <xsl:variable name="temp_variable" select="cdit:get_variable_name($enum_instance)" />
        <xsl:variable name="set_func" select="cdit:invoke_middleware_set_function('out', cpp:arrow(), $enum_instance, $target_middleware, $temp_variable)" />
        <xsl:value-of select="concat(o:t($tab + 1), $set_func, cpp:nl())" />
        <xsl:value-of select="cpp:scope_end($tab)" />
    </xsl:function>

    <xsl:function name="cdit:cast_enum_instance">
        <xsl:param name="enum_instance" as="element()" />
        <xsl:param name="get_func" as="xs:string" />

        <xsl:param name="middleware" as="xs:string" />
        <xsl:param name="source_middleware" as="xs:string" />
        <xsl:param name="target_middleware" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />

        <!-- Get the type of the Enum -->
        <xsl:variable name="enum_cast_type" select="cdit:get_qualified_enum_type($enum_instance, $target_middleware)" />

        <xsl:variable name="cast_enum_value" select="o:join_list((o:wrap_bracket($enum_cast_type), cpp:static_cast('int', $get_func)), ' ')" />
        <xsl:variable name="temp_variable" select="cdit:get_variable_name($enum_instance)" />

        <xsl:value-of select="cpp:comment('Cast the enums integer representation', $tab)" />
        <xsl:value-of select="cpp:define_variable(cpp:auto(), $temp_variable, $cast_enum_value, cpp:nl(), $tab)" />
    </xsl:function>


    <xsl:function name="cdit:invoke_translate_function">
        <xsl:param name="aggregate" as="element()" />
        <xsl:param name="get_func" as="xs:string" />
        <xsl:param name="src_middleware" as="xs:string" />
        <xsl:param name="target_middleware" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />
        
        
        <xsl:variable name="other_middleware" select="if($src_middleware = 'base') then $target_middleware else $src_middleware" />
        <xsl:variable name="base_type" select="cdit:get_aggregate_qualified_type($aggregate, 'base')" />
        <xsl:variable name="middleware_type" select="cdit:get_aggregate_qualified_type($aggregate, $other_middleware)" />
        
        <xsl:variable name="translate_func" select="if($src_middleware = 'base') then 'BaseToMiddleware' else 'MiddlewareToBase'" />

        <xsl:variable name="template_type" select="cpp:join_args(($base_type, $middleware_type))"/>

        <xsl:value-of select="cpp:invoke_templated_class_static_function($template_type, cpp:combine_namespaces(('Base', 'Translator')),  $translate_func, $get_func, '', $tab)" />
    </xsl:function>

    <xsl:function name="cdit:translate_aggregate_instance">
        <xsl:param name="aggregate_instance" as="element()" />
        <xsl:param name="middleware" as="xs:string" />
        <xsl:param name="source_middleware" as="xs:string" />
        <xsl:param name="target_middleware" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />


        <xsl:variable name="middleware_namespace" select="cdit:get_middleware_namespace($middleware)" />

        <xsl:variable name="get_func" select="cdit:invoke_middleware_get_function('value', cpp:dot(), $aggregate_instance, $source_middleware)" />
        <xsl:variable name="temp_variable" select="lower-case(concat('value_', graphml:get_label($aggregate_instance)))" />
        <xsl:variable name="translate_func" select="cdit:invoke_translate_function($aggregate_instance,  $get_func, $source_middleware, $target_middleware, 0)" />

        <xsl:variable name="value">
            <xsl:choose>
                <xsl:when test="$target_middleware = 'proto'">
                    <!-- Protobuf has a swap function which takes a pointer -->
                    <xsl:value-of select="concat($temp_variable, '.release()')" />
                </xsl:when>
                <xsl:otherwise>
                    <xsl:value-of select="concat('*', $temp_variable)" />
                </xsl:otherwise>
            </xsl:choose>
        </xsl:variable>

        <xsl:variable name="set_func" select="cdit:invoke_middleware_set_function('out', cpp:arrow(), $aggregate_instance, $target_middleware, $value)" />

        
        <xsl:value-of select="cpp:comment(('Member', o:wrap_square(graphml:get_kind($aggregate_instance)), 'Type', o:wrap_angle(graphml:get_type($aggregate_instance))), $tab)" />
        <xsl:value-of select="cpp:comment('Set and cleanup translated Aggregate', $tab)" />
        <xsl:value-of select="cpp:define_variable(cpp:auto(), $temp_variable, $translate_func, cpp:nl(), $tab)" />
        <xsl:value-of select="concat(o:t($tab), $set_func, cpp:nl())" />
    </xsl:function>

    <xsl:function name="cdit:translate_vector">
        <xsl:param name="vector" as="element()" />
        <xsl:param name="middleware" as="xs:string" />
        <xsl:param name="source_middleware" as="xs:string" />
        <xsl:param name="target_middleware" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />

        <xsl:variable name="middleware_namespace" select="cdit:get_middleware_namespace($middleware)" />
        <xsl:variable name="vector_child" select="graphml:get_vector_child($vector)" />
        <xsl:variable name="vector_child_type" select="graphml:get_type($vector_child)" />
        <xsl:variable name="vector_child_kind" select="graphml:get_kind($vector_child)" />
        <xsl:variable name="is_array" select="graphml:evaluate_data_value_as_boolean($vector, 'is_array')" />

        <xsl:variable name="get_func" select="cdit:invoke_middleware_get_function('value', cpp:dot(), $vector, $source_middleware)" />
        <xsl:variable name="temp_variable" select="concat(cdit:get_variable_name($vector), '_element')" />
        <xsl:variable name="temp_element_variable" select="o:join_list(($target_middleware, $temp_variable), '_')" />

        <xsl:variable name="set_func">
            <xsl:choose>
                <xsl:when test="$vector_child_kind = 'AggregateInstance'">
                    <xsl:value-of select="cdit:invoke_middleware_add_vector_function('out', cpp:arrow(), $vector, $target_middleware, $temp_element_variable, $tab + 3)" />
                </xsl:when>
                <xsl:when test="$vector_child_kind = 'Member'">
                    <xsl:value-of select="cdit:invoke_middleware_add_vector_function('out', cpp:arrow(), $vector, $target_middleware, $temp_variable, $tab + 3)" />
                </xsl:when>
                <xsl:when test="$vector_child_kind = 'EnumInstance'">
                    <xsl:variable name="variable_name" select="cdit:get_variable_name($vector_child)" />

                    <xsl:value-of select="cdit:invoke_middleware_add_vector_function('out', cpp:arrow(), $vector, $target_middleware, $variable_name, $tab + 3)" />
                </xsl:when>
            </xsl:choose>
        </xsl:variable>

        <xsl:value-of select="cpp:try($tab)" />
        <xsl:value-of select="cpp:comment(('Vector', o:wrap_square($vector_child_kind), 'Type', o:wrap_angle($vector_child_type)), $tab + 1)" />
        <xsl:value-of select="cpp:comment('Iterate and set all elements in vector', $tab + 1)" />

        
        <!-- Define an array_size -->
        
        <xsl:variable name="size_var" select="'size'" />
        <xsl:variable name="array_size" select="graphml:get_data_value($vector, 'array_size')" />
        <xsl:variable name="sequence_size" select="graphml:get_data_value($vector, 'sequence_size')" />

        <xsl:variable name="array_size_value">
            <xsl:choose>
                <xsl:when test="$is_array and cdit:middleware_requires_idl_file($source_middleware)">
                    <xsl:value-of select="$array_size" />
                </xsl:when>
                <xsl:when test="$source_middleware = 'tao'">
                    <xsl:value-of select="concat($get_func,'.length()')" />
                </xsl:when>
                <xsl:when test="cdit:middleware_uses_protobuf($source_middleware)">
                    <xsl:variable name="variable_syntax" select="cdit:get_middleware_variable_syntax($vector, $middleware)" />
                    <xsl:value-of select="concat('value', cpp:dot(), $variable_syntax, '_size()')" />
                </xsl:when>
                <xsl:otherwise>
                    <xsl:value-of select="concat($get_func,'.size()')" />
                </xsl:otherwise>
            </xsl:choose>
        </xsl:variable>
        <xsl:value-of select="cpp:define_variable(cpp:const_ref_auto(), $size_var, $array_size_value, cpp:nl(), $tab + 1)" />

        <xsl:variable name="test_size" select="($is_array and $array_size) or $sequence_size" as="xs:boolean" />

        <!-- Do some tests on size -->
        <xsl:if test="cdit:middleware_requires_idl_file($target_middleware) and $test_size">
            <xsl:value-of select="cpp:comment('Translating into a fixed length array/sequence', $tab + 1)" />

            <xsl:variable name="max_size" select="if($is_array) then $array_size else $sequence_size" />
            
            <xsl:value-of select="concat(o:t($tab + 1), 'if(', $size_var, ' ', o:gt(), ' ', $max_size, ')', cpp:scope_start(0))" />
            <xsl:variable name="str_args" as="xs:string*">
                <xsl:sequence select="o:wrap_dblquote('Trying to fit: ')" />
                <xsl:sequence select="'std::to_string(size)'" />
                <xsl:sequence select="o:wrap_dblquote(concat(
                ' elements when max length is: ',
                $max_size))" />
            </xsl:variable>
            <xsl:value-of select="cpp:throw('std::invalid_argument', o:join_list($str_args, ' + '), $tab + 2)" />
            <xsl:value-of select="cpp:scope_end($tab + 1)" />
        </xsl:if>

        <!-- Change the size of the target Vector -->

        <!-- Define for loop -->
        <xsl:value-of select="cpp:for('auto i = 0', concat('i ' , o:lt(), ' ', $size_var), 'i ++', cpp:scope_start(0), $tab + 1)" />
        <xsl:value-of select="cpp:try($tab + 2)" />

        <!-- take a reference copy -->
        <xsl:value-of select="cpp:define_variable(cpp:const_ref_auto(), $temp_variable, concat($get_func, o:wrap_square('i')), cpp:nl(), $tab + 3)" />

        <xsl:choose>
            <xsl:when test="$vector_child_kind = 'AggregateInstance'">
                <xsl:variable name="translate_func" select="cdit:invoke_translate_function($vector_child, $temp_variable, $source_middleware, $target_middleware, 0)" />

                <!-- Vector aggregates require translation -->
                <xsl:value-of select="cpp:comment('Set and cleanup translated Aggregate', $tab + 3)" />
                <xsl:value-of select="cpp:define_variable(cpp:auto(), $temp_element_variable, $translate_func, cpp:nl(), $tab + 3)" />
                <xsl:value-of select="$set_func" />
            </xsl:when>
            <xsl:when test="$vector_child_kind = 'Member'">
                <!-- Member aggregates require translation -->
                <xsl:value-of select="$set_func" />
            </xsl:when>
            <xsl:when test="$vector_child_kind = 'EnumInstance'">
                <xsl:variable name="get_func">
                    <xsl:choose>
                        <xsl:when test="$source_middleware = 'rti'">
                            <xsl:value-of select="concat($temp_variable, '.underlying()')" />
                        </xsl:when>
                        <xsl:otherwise>
                            <xsl:value-of select="$temp_variable" />
                        </xsl:otherwise>
                    </xsl:choose>
                </xsl:variable>

                <xsl:value-of select="cdit:cast_enum_instance($vector_child, $get_func, $middleware, $source_middleware, $target_middleware, $tab + 3)" />
                <xsl:value-of select="$set_func" />
            </xsl:when>
        </xsl:choose>

        <!-- Catch Translation of Exceptions -->
        <xsl:value-of select="cpp:catch_exception('}', 'std::exception', 'ex', $tab + 2)" />
        <!-- Define a string to throw -->
        <xsl:variable name="str_args" as="xs:string*">
            <xsl:sequence select="
            o:wrap_dblquote(concat(
            'Translating Vector ',
            $vector_child_kind,
            ' @ i = '))" />
            <xsl:sequence select="'std::to_string(i)'" />
            <xsl:sequence select="o:wrap_dblquote('Failed: ')"/>
            
        </xsl:variable>

        <xsl:value-of select="cpp:define_variable('std::string', 'error_str', o:join_list($str_args, ' + '), cpp:nl(), $tab + 3)" />
        <xsl:value-of select="cpp:throw('std::invalid_argument', 'error_str + ex.what()', $tab + 3)" />
        <xsl:value-of select="cpp:scope_end($tab + 2)" />

        <xsl:value-of select="cpp:scope_end($tab + 1)" />

        <xsl:value-of select="cpp:catch_exception('}', 'std::exception', 'ex', $tab)" />
        <!-- Catch Exceptions -->
        <xsl:variable name="str_args" as="xs:string*">
            <xsl:sequence select="o:wrap_dblquote(concat(
            'Translating Vector ',
            o:wrap_quote(graphml:get_label($vector)),
            ' From: ',
            o:wrap_square($source_middleware),
            ' To: ',
            o:wrap_square($target_middleware),
            ': Failed: '
            ))"/>
        </xsl:variable>
        <xsl:value-of select="cpp:define_variable('std::string', 'error_str', o:join_list($str_args, ' + '), cpp:nl(), $tab + 1)" />
        <xsl:value-of select="cpp:throw('std::invalid_argument', 'error_str + ex.what()', $tab + 1)" />
        <xsl:value-of select="cpp:scope_end($tab)" />
        <xsl:value-of select="o:nl(1)" />
    </xsl:function>

    <xsl:function name="cdit:get_middleware_union_descriminator_function">
        <xsl:param name="middleware" as="xs:string" />

        <xsl:variable name="middleware_lc" select="lower-case($middleware)" />

        <xsl:choose>
            <xsl:when test="$middleware_lc = 'base'">
                <xsl:value-of select="'get_descriminator'" />
            </xsl:when>
            <xsl:when test="$middleware_lc = 'tao'">
                <xsl:value-of select="'_d'" />
            </xsl:when>
            <xsl:when test="$middleware_lc = 'rti'">
                <xsl:value-of select="'_d().underlying'" />
            </xsl:when>
        </xsl:choose>
    </xsl:function>

    <xsl:function name="cdit:get_translate_function_cpp">
        <xsl:param name="aggregate" />
        <xsl:param name="middleware" as="xs:string" />
        <xsl:param name="source_middleware" as="xs:string" />
        <xsl:param name="target_middleware" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />

        <xsl:variable name="is_union" select="graphml:evaluate_data_value_as_boolean($aggregate, 'is_union') and cdit:middleware_handles_union($source_middleware)" />
        <xsl:variable name="target_type" select="cdit:get_aggregate_qualified_type($aggregate, $target_middleware)" />


            
        <xsl:value-of select="cpp:define_variable('auto', 'out', cpp:unique_ptr_object($target_type, cpp:new_object($target_type, '')), cpp:nl(), $tab)" />


        <xsl:variable name="get_descrim" select="concat('value.', cdit:get_middleware_union_descriminator_function($source_middleware), '()')" />

        
        <xsl:if test="$is_union">
            <xsl:value-of select="cpp:switch($get_descrim, $tab)" />
        </xsl:if>

        <!-- Handle the children of the aggregate -->
        <xsl:for-each select="graphml:get_child_nodes($aggregate)">
            <xsl:variable name="kind" select="graphml:get_kind(.)" />

            <xsl:variable name="interior_tab" select="if($is_union) then $tab + 1 else $tab" />

            <xsl:if test="$is_union">
                <xsl:variable name="enum_case" select="cdit:get_union_descriminator_case(., $source_middleware)" />

                <xsl:value-of select="cpp:switch_case($enum_case, $tab)" />
            </xsl:if>

            <xsl:choose>    
                <xsl:when test="$kind = 'Member'">
                    <xsl:value-of select="cdit:translate_member(., $middleware, $source_middleware, $target_middleware, $interior_tab)" />
                </xsl:when>
                <xsl:when test="$kind = 'EnumInstance'">
                    <xsl:value-of select="cdit:translate_enum_instance(., $middleware, $source_middleware, $target_middleware, $interior_tab)" />
                </xsl:when>
                <xsl:when test="$kind = 'AggregateInstance'">
                    <xsl:value-of select="cdit:translate_aggregate_instance(., $middleware, $source_middleware, $target_middleware, $interior_tab)" />
                </xsl:when>
                <xsl:when test="$kind = 'Vector'">
                    <xsl:value-of select="cdit:translate_vector(., $middleware, $source_middleware, $target_middleware, $interior_tab)" />
                </xsl:when>
                <xsl:otherwise>
                    <xsl:value-of select="cpp:comment(('Kind ', o:wrap_quote($kind), 'Not implemented!'), $interior_tab)" />
                </xsl:otherwise>
            </xsl:choose>

            <xsl:if test="$is_union">
                <xsl:value-of select="concat(o:t($tab + 1), 'break', cpp:nl())" />
                <xsl:value-of select="cpp:scope_end($tab)" />
            </xsl:if>
        </xsl:for-each>

        <xsl:if test="$is_union">
            <!-- Handle the Default Case -->
            <xsl:value-of select="cpp:switch_default($tab)" />
            <xsl:value-of select="concat(o:t($tab + 1), 'break', cpp:nl())" />
            <xsl:value-of select="cpp:scope_end($tab)" />

            <xsl:value-of select="cpp:scope_end($tab)" />
        </xsl:if>

        <xsl:value-of select="cpp:return('out', $tab)" />
    </xsl:function>

    <xsl:function name="cdit:get_server_interface_idl">
        <xsl:param name="server_interface" />
        <xsl:param name="middleware" />

        <xsl:variable name="namespaces" select="graphml:get_namespace($server_interface)" />
        <xsl:variable name="label" select="graphml:get_label($server_interface)" />
        <xsl:variable name="function_name" select="graphml:get_data_value($server_interface, 'function_name')" />
        <xsl:variable name="interface_name" select="graphml:get_data_value($server_interface, 'interface_name')" />
        <xsl:variable name="tab" select="count($namespaces)" />

        <!-- Compute the define guard -->
        <xsl:variable name="define_guard_name" select="upper-case(o:join_list(($middleware, $namespaces, $label, 'SERVER_IDL'), '_'))" />

        <!-- Preamble -->
        <xsl:value-of select="cpp:print_regen_version('datatype_functions.xsl', 'cdit:get_server_interface_idl', 0)" />

        <!-- Define Guard -->
        <xsl:value-of select="cpp:define_guard_start($define_guard_name)" />

        <xsl:variable name="server_requests" select="cdit:get_server_interface_request_aggregates($server_interface)" />
        <xsl:variable name="server_replies" select="cdit:get_server_interface_reply_aggregates($server_interface)" />
        <xsl:variable name="aggregate_instances" select="graphml:get_definitions(($server_requests, $server_replies))" />

        <xsl:for-each select="$aggregate_instances">
            <xsl:if test="position() = 1">
                <xsl:value-of select="cpp:comment('Import required .idl files', 0)" />
            </xsl:if>
            <xsl:variable name="required_file" select="cdit:get_aggregates_middleware_file_name(., $middleware)" />
            <xsl:value-of select="idl:include($required_file)" />
            
            <xsl:if test="position() = last()">
                <xsl:value-of select="o:nl(1)" />
            </xsl:if>
        </xsl:for-each>

        <xsl:variable name="tab" select="count($namespaces)" />

        <!-- Define Namespaces -->
        <xsl:for-each select="$namespaces">
            <xsl:value-of select="idl:module_start(., position() - 1)" />
        </xsl:for-each>
        
        <!-- Define the Interface -->
        <xsl:value-of select="idl:interface($interface_name, $tab)" />

        <xsl:variable name="request_definition" select="graphml:get_definition($server_requests[1])" />
        <xsl:variable name="reply_definition" select="graphml:get_definition($server_replies[1])" />

        <xsl:variable name="qualified_reply_type" as="xs:string">
            <xsl:choose>
                <xsl:when test="count($reply_definition) = 1">
                    <xsl:value-of select="idl:get_aggregate_qualified_type($reply_definition)" />
                </xsl:when>
                <xsl:otherwise>
                    <xsl:value-of select="'void'" />
                </xsl:otherwise>
            </xsl:choose>
        </xsl:variable>

        <xsl:variable name="qualified_request_type" as="xs:string">
            <xsl:choose>
                <xsl:when test="count($request_definition) = 1">
                    <xsl:value-of select="idl:get_aggregate_qualified_type($request_definition)" />
                </xsl:when>
                <xsl:otherwise>
                    <xsl:value-of select="''" />
                </xsl:otherwise>
            </xsl:choose>
        </xsl:variable>

        <xsl:variable name="function_args" as="xs:string">
            <xsl:choose>
                <xsl:when test="$qualified_request_type != ''">
                    <xsl:value-of select="concat('in ', $qualified_request_type, ' message_')" />
                </xsl:when>
                <xsl:otherwise>
                    <xsl:value-of select="''" />
                </xsl:otherwise>
            </xsl:choose>
        </xsl:variable>

        <xsl:value-of select="cpp:declare_function($qualified_reply_type, $function_name, $function_args, ';', $tab + 1)" />

        <xsl:value-of select="cpp:scope_end($tab)" />

         <!-- Define Namespaces -->
        <xsl:for-each select="$namespaces">
            <xsl:value-of select="idl:module_end(., position() - 1)" />
        </xsl:for-each>

        <!-- Define Guard -->
        <xsl:value-of select="cpp:define_guard_end($define_guard_name)" />
    </xsl:function>
</xsl:stylesheet>