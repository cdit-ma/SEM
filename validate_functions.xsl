<xsl:stylesheet version="2.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:xs="http://www.w3.org/2001/XMLSchema"
    xmlns:o="http://github.com/cdit-ma/re_gen/o"
    xmlns:cdit="http://github.com/cdit-ma/re_gen/cdit"
    xmlns:xmlo="http://github.com/cdit-ma/re_gen/xmlo"
    xmlns:gml="http://graphml.graphdrawing.org/xmlns"
    xmlns:graphml="http://github.com/cdit-ma/re_gen/graphml"
    >



    <xsl:function name="cdit:output_test">
        <xsl:param name="test_name" as="xs:string"/>
        <xsl:param name="contents" as="xs:string"/>
        <xsl:param name="tab" as="xs:integer" />

        <xsl:value-of select="xmlo:wrap_tag('test', concat('name=', o:wrap_dblquote($test_name)), $contents, $tab)" />
    </xsl:function>

    <xsl:function name="cdit:output_result">
        <xsl:param name="id" as="xs:string"/>
        <xsl:param name="result" as="xs:boolean"/>
        <xsl:param name="error_string" as="xs:string" />
        <xsl:param name="warning" as="xs:boolean" />
        <xsl:param name="tab" />

        <xsl:variable name="warning_string" select="if($warning = true()) then concat(' warning=', o:wrap_dblquote(string($warning))) else ''" />
        <xsl:variable name="success_string" select="concat(' success=', o:wrap_dblquote(string($result)))" />
        <xsl:variable name="id_string" select="concat('id=', o:wrap_dblquote($id))" />

        <xsl:value-of select="xmlo:wrap_tag('result', concat($id_string, $success_string, $warning_string), if($result = false()) then $error_string else '', $tab)" />
    </xsl:function>

    <xsl:function name="cdit:test_aggregate_requires_key">
        <xsl:param name="aggregates" as="element()*" />

        <xsl:variable name="results">  
            <xsl:for-each select="$aggregates">
                <xsl:variable name="id" select="graphml:get_id(.)" />
                <xsl:variable name="type" select="graphml:get_type(.)" />
                <xsl:variable name="aggregate_keys" select="graphml:get_keys(.)" />
                <xsl:variable name="got_key" select="count($aggregate_keys) > 0" />        
                <xsl:value-of select="cdit:output_result($id, $got_key, o:join_list(('Aggregate', o:wrap_quote($type), 'has no child with data', o:wrap_quote('is_key'), 'set to true'), ' '), false(), 2)" />        
            </xsl:for-each>
        </xsl:variable>

        <xsl:value-of select="cdit:output_test('All Aggregate entities require a direct child to be set as key', $results, 1)" />
    </xsl:function>

    <xsl:function name="cdit:test_aggregate_unique_child_names">
        <xsl:param name="aggregates" as="element()*" />

        <xsl:variable name="results">  
            <xsl:for-each select="$aggregates">
                <xsl:variable name="id" select="graphml:get_id(.)" />
                <xsl:variable name="type" select="graphml:get_type(.)" />
                <xsl:variable name="aggregate_keys" select="graphml:get_keys(.)" />
                <xsl:variable name="got_key" select="count($aggregate_keys) > 0" />        
                <xsl:value-of select="cdit:output_result($id, $got_key, o:join_list(('Aggregate', o:wrap_quote($type), 'has no child with data', o:wrap_quote('is_key'), 'set to true'), ' '), false(), 2)" />        
            </xsl:for-each>
        </xsl:variable>

        <xsl:value-of select="cdit:output_test('All Aggregate entities require a direct child to be set as key', $results, 1)" />
    </xsl:function>

    

    <!-- Tests all that the entity kinds provided contain at least 1 child -->
    <xsl:function name="cdit:test_requires_children">
        <xsl:param name="entities" as="element(gml:node)*"/>
        <xsl:param name="test" as="xs:string"/>

        <xsl:variable name="results">  
            <xsl:for-each select="$entities">
                <xsl:variable name="id" select="graphml:get_id(.)" />
                <xsl:variable name="label" select="graphml:get_label(.)" />
                <xsl:variable name="kind" select="graphml:get_kind(.)" />
                <xsl:variable name="got_children" select="count(graphml:get_child_nodes(.)) > 0" />        
                <xsl:value-of select="cdit:output_result($id, $got_children, o:join_list(($kind, o:wrap_quote($label), 'requires at least one child entity'), ' '), false(), 2)" />        
            </xsl:for-each>
        </xsl:variable>

        <xsl:value-of select="cdit:output_test($test, $results, 1)" />
    </xsl:function>

    <!-- Tests that all aggregates have unique member labels -->
    <xsl:function name="cdit:test_unique_child_labels">
        <xsl:param name="entities" as="element(gml:node)*"/>

        <xsl:for-each select="$entities">
            <xsl:value-of select="cdit:test_unique_labels((., graphml:get_child_nodes(.)), 'has a non-unique label within its scope.', false())" />
        </xsl:for-each>
    </xsl:function>

    <!-- Tests that all entities in list have unique member labels -->
    <xsl:function name="cdit:test_unique_labels">
        <xsl:param name="entities" as="element(gml:node)*"/>
        <xsl:param name="error_str" as="xs:string"/>
        <xsl:param name="warning" as="xs:boolean"/>


        <xsl:variable name="results">  
            <xsl:variable name="all_labels" select="graphml:get_data_values($entities, 'label')" />

            <xsl:for-each select="$entities">
                <xsl:variable name="id" select="graphml:get_id(.)" />
                <xsl:variable name="kind" select="graphml:get_kind(.)" />
                <xsl:variable name="label" select="graphml:get_label(.)" />

                <!-- Check the number of times the type is in the list of all types-->
                <xsl:variable name="match_count" select="o:string_in_list_count($label, $all_labels, true())" />
                <xsl:value-of select="cdit:output_result($id, $match_count = 1, o:join_list(($kind, o:wrap_quote($label), $error_str), ' '), $warning, 2)" />
            </xsl:for-each>
        </xsl:variable>

        <xsl:value-of select="cdit:output_test('Entities require an unique label.', $results, 1)" />
    </xsl:function>

    <!-- Tests that all aggregates have unique types-->
    <xsl:function name="cdit:test_aggregate_unique_type">
        <xsl:param name="aggregates" as="element(gml:node)*"/>

        <xsl:variable name="all_types" select="graphml:get_data_values($aggregates, 'type')" as="xs:string*" />
        
        <xsl:variable name="results">  
            <xsl:for-each select="$aggregates">
                <xsl:variable name="id" select="graphml:get_id(.)" />
                <xsl:variable name="type" select="graphml:get_type(.)" />

                <!-- Check the number of times the type is in the list of all types-->
                <xsl:variable name="match_count" select="o:string_in_list_count($type, $all_types, true())" />

                <xsl:value-of select="cdit:output_result($id, $match_count = 1, o:join_list(('Aggregate', o:wrap_quote($type), ' does not have a unique type'), ' '), false(), 2)" />        
            </xsl:for-each>
        </xsl:variable>
        <xsl:value-of select="cdit:output_test('All Aggregate entities require unique type', $results, 1)" />
    </xsl:function>

    <xsl:function name="cdit:test_components_unique_name">
        <xsl:param name="components" as="element(gml:node)*"/>

        <xsl:variable name="all_labels" select="graphml:get_data_values($components, 'label')" as="xs:string*" />
        
        <xsl:variable name="results">  
            <xsl:for-each select="$components">
                <xsl:variable name="id" select="graphml:get_id(.)" />
                <xsl:variable name="label" select="graphml:get_label(.)" />

                <!-- Check the number of times the type is in the list of all types-->
                <xsl:variable name="match_count" select="o:string_in_list_count($label, $all_labels, true())" />
                
                <xsl:value-of select="cdit:output_result($id, $match_count = 1, o:join_list(('Component', o:wrap_quote($label), ' does not have a unique label'), ' '), false(), 2)" />        
            </xsl:for-each>
        </xsl:variable>
        <xsl:value-of select="cdit:output_test('All Component entities require unique labels', $results, 1)" />
    </xsl:function>
    

    <xsl:function name="cdit:is_data_linked" as="xs:boolean">
        <xsl:param name="entity" as="element(gml:node)"/>
        <xsl:param name="allow_value" as="xs:boolean"/>

        <xsl:variable name="data_sources" select="graphml:get_sources($entity, 'Edge_Data')" />
        <xsl:variable name="parent" select="graphml:get_parent_node($entity)" />
        <xsl:variable name="value" select="graphml:get_value($entity)" />

        <xsl:choose>
            <xsl:when test="count($data_sources) > 0">
                <xsl:value-of select="true()" />        
            </xsl:when>
            <xsl:when test="$allow_value and $value != ''">
                <xsl:value-of select="true()" />        
            </xsl:when>
            <xsl:when test="$parent">
                <xsl:value-of select="cdit:is_data_linked($parent, $allow_value)" />        
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="false()" />
            </xsl:otherwise>
        </xsl:choose>
    </xsl:function>

    <xsl:function name="cdit:get_non_data_linked_entities" as="element(gml:node)*">
        <xsl:param name="entities" as="element(gml:node)*"/>
        <xsl:param name="allow_value" as="xs:boolean"/>
        
        <xsl:for-each select="$entities">
            <xsl:if test="cdit:is_data_linked(., $allow_value) = false()">
                <xsl:sequence select="." />
            </xsl:if>
        </xsl:for-each>
    </xsl:function>


    <!-- Tests that all entities in list have unique member labels 
    <xsl:function name="cdit:test_aggregate_namespace_collisions">
        <xsl:param name="aggregates" as="element(gml:node)*"/>

        <xsl:variable name="results">  
            <xsl:variable name="aggregate_labels" select="graphml:get_data_values($aggregates, 'label')" />

            <xsl:for-each select="$aggregates">
                <xsl:variable name="id" select="graphml:get_id(.)" />
                <xsl:variable name="kind" select="graphml:get_kind(.)" />
                <xsl:variable name="label" select="graphml:get_label(.)" />
                <xsl:variable name="namespace" select="graphml:get_namespace(.)" />

                <xsl:variable name="match_count" select="o:string_in_list_count($label, $aggregate_labels)" />
                <xsl:variable name="is_used_by_dds" select="true()" />

                <xsl:if test="$match_count > 1 and $is_used_by_dds and $namespace = ''">
                    <xsl:value-of select="cdit:output_result($id, false(), o:join_list(($kind, o:wrap_quote($label), 'will collide with other Aggregates that have the same label, as it is defined in the global namespace (only idl driven middlewares). Consider adding a unique namespace.'), ' '), false(), 2)" />
                </xsl:if>
            </xsl:for-each>
        </xsl:variable>

        <xsl:value-of select="cdit:output_test('Aggregate Namespace Collisions.', $results, 1)" />
    </xsl:function>-->

    <!-- Tests that all entities in list have unique member labels -->
    <xsl:function name="cdit:test_namespace_collisions">
        <xsl:param name="aggregates" as="element(gml:node)*"/>

        <xsl:variable name="aggregate_labels" select="graphml:get_data_values($aggregates, 'label')" />

        <xsl:variable name="results">  
            <xsl:for-each select="$aggregates">
                
                <xsl:variable name="aggregate_instances" select="graphml:get_descendant_nodes_of_kind(., 'AggregateInstance')" />
                <xsl:variable name="enum_instances" select="graphml:get_descendant_nodes_of_kind(., 'EnumInstance')" />
                <xsl:variable name="enum_definitions" select="graphml:get_definitions($enum_instances)" />

                <xsl:variable name="enum_labels" select="graphml:get_data_values($enum_definitions, 'label')" />

                <xsl:for-each select="$aggregate_instances">
                    <xsl:variable name="id" select="graphml:get_id(.)" />
                    
                    <xsl:variable name="aggregate_definition" select="graphml:get_definition(.)" />

                    <xsl:variable name="inst_kind" select="graphml:get_kind(.)" />
                    <xsl:variable name="inst_label" select="graphml:get_label(.)" />
                    <xsl:variable name="def_label" select="graphml:get_label($aggregate_definition)" />
                    <xsl:variable name="def_namespace" select="graphml:get_namespace($aggregate_definition)" />

                    <xsl:variable name="match_count" select="o:string_in_list_count($def_label, $aggregate_labels, true())" />
                    <xsl:variable name="is_used_by_dds" select="true()" />

                    <xsl:if test="$match_count > 1 and $is_used_by_dds and $def_namespace = ''">
                        <xsl:value-of select="cdit:output_result($id, false(), o:join_list(($inst_kind, o:wrap_quote($inst_label), 'collides with an ancestor Aggregate that has the same label (DDS compliancy issue). Consider moving the Aggregates definition into a namespace.'), ' '), false(), 2)" />
                    </xsl:if>
                </xsl:for-each>

                <xsl:for-each select="$enum_instances">
                    <xsl:variable name="id" select="graphml:get_id(.)" />
                    
                    <xsl:variable name="enum_definition" select="graphml:get_definition(.)" />

                    <xsl:variable name="inst_kind" select="graphml:get_kind(.)" />
                    <xsl:variable name="inst_label" select="graphml:get_label(.)" />
                    <xsl:variable name="def_label" select="graphml:get_label($enum_definition)" />
                    <xsl:variable name="def_namespace" select="graphml:get_namespace($enum_definition)" />

                    <xsl:variable name="match_count" select="o:string_in_list_count($def_label, $enum_labels, true())" />
                    <xsl:variable name="is_used_by_dds" select="true()" />

                    <xsl:if test="$match_count > 1 and $is_used_by_dds and $def_namespace = ''">
                        <xsl:value-of select="cdit:output_result($id, false(), o:join_list(($inst_kind, o:wrap_quote($inst_label), 'collides with sibling Enum that have the same label (DDS compliancy issue). Consider moving the Enums definition into a namespace.'), ' '), false(), 2)" />
                    </xsl:if>
                </xsl:for-each>
            </xsl:for-each>
        </xsl:variable>

        <xsl:value-of select="cdit:output_test('Aggregate Namespace Collisions.', $results, 1)" />
    </xsl:function>

    

    <!-- Tests that all ComponentImpls have all their parameters set via either data-linking or value -->
    <xsl:function name="cdit:test_componentimpl_data">
        <xsl:param name="component_impls" as="element(gml:node)*"/>

        <xsl:variable name="results">
            <xsl:for-each select="$component_impls">
                <xsl:variable name="outeventportimpls" select="graphml:get_descendant_nodes_of_kind(., ('PublisherPortImpl'))" />
                    
                <xsl:variable name="parameters" select="graphml:get_descendant_nodes_of_kind(., ('InputParameter', 'VariadicParameter', 'VariableParameter'))" />
                <xsl:variable name="out_members" select="graphml:get_descendant_nodes($outeventportimpls)" />


                
                <!-- This should select all non data linked entities -->
                <xsl:for-each select="cdit:get_non_data_linked_entities(($parameters, $out_members), false())">
                    <xsl:variable name="id" select="graphml:get_id(.)" />
                    <xsl:variable name="label" select="graphml:get_label(.)" />
                    <xsl:variable name="kind" select="graphml:get_kind(.)" />
                    <xsl:variable name="type" select="graphml:get_type(.)" />
                    <xsl:variable name="value" select="graphml:get_value(.)" />

                    <xsl:variable name="children_linked" select="count(graphml:get_ancestor_nodes_of_kind(., ('Vector', 'VectorInstance'))) > 0"/>

                    <xsl:variable name="is_valid_kind" select="($kind = 'AggregateInstance' or $kind = 'VectorInstance' or $kind = 'Vector') = false()"/>
                    <xsl:variable name="in_vector" select="count(graphml:get_ancestor_nodes_of_kind(., ('Vector', 'VectorInstance'))) > 0"/>
                    <xsl:variable name="in_outevent" select="count(graphml:get_ancestor_nodes_of_kind(., ('PublisherPortImpl'))) > 0"/>
                    
                    <!-- Don't want to check inside vectors, as they do not need data -->
                    <xsl:if test="$is_valid_kind and $in_vector = false()">
                        <!-- Check for all things which need data, to see whether they have a manual setting or data edge -->
                        <xsl:value-of select="cdit:output_result($id, $value != '', o:join_list(($kind, o:wrap_quote($label), 'requires either a value set or a data connection (Edge_Data)'), ' '), $in_outevent, 2)" />        
                        
                        <xsl:if test="$type = 'String' and $value != ''">
                            <!-- Check if string that is set is double-quote wrapped -->
                            <xsl:variable name="got_valid_string" select="starts-with($value, o:dblquote()) and ends-with($value , o:dblquote())" />
                            <xsl:value-of select="cdit:output_result($id, $got_valid_string = true(), o:join_list(($kind, o:wrap_quote($label), 'has a string value set which is not double quoted. Are you trying to reference a variable'), ' '), true(), 2)" />        
                        </xsl:if>
                    </xsl:if>
                </xsl:for-each>
            </xsl:for-each>
        </xsl:variable>

        <xsl:value-of select="cdit:output_test('ComponentImpl Data-Linking tests', $results, 1)" />
    </xsl:function>

    

    <xsl:function name="cdit:test_logfunction">
        <xsl:param name="component_impls" as="element(gml:node)*"/>

        <xsl:variable name="results">
            <xsl:for-each select="$component_impls">
                <xsl:for-each select="graphml:get_child_nodes(.)">
                   <xsl:variable name="id" select="graphml:get_id(.)" />
                    <xsl:variable name="label" select="graphml:get_label(.)" />
                    <xsl:variable name="kind" select="graphml:get_kind(.)" />
                    
                    <xsl:variable name="ignored_kinds" select="('PeriodicPort', 'SubscriberPortImpl', 'Variable', 'AttributeImpl', 'Header')" />

                    <!-- Check if the $kind is to be ignored -->
                    <xsl:variable name="ignored" select="$kind = $ignored_kinds" />

                    <xsl:if test="not($ignored)">
                        <xsl:variable name="workflow_sources" select="graphml:get_sources(., 'Edge_Workflow')" />
                        <xsl:value-of select="cdit:output_result($id, count($workflow_sources) > 0, o:join_list(($kind, o:wrap_quote($label), 'does not have a Workflow connection (Edge_Workflow)'), ' '), true(), 2)" />        
                    </xsl:if>
                </xsl:for-each>
            </xsl:for-each>
        </xsl:variable>

        <xsl:value-of select="cdit:output_test('ComponentImpl workflow tests', $results, 1)" />
    </xsl:function>

    <xsl:function name="cdit:compare_type_to_printf" as="xs:boolean">
        <xsl:param name="type" as="xs:string"/>
        <xsl:param name="printf_arg" as="xs:string"/>

        <xsl:variable name="regex">
            <xsl:choose>
                <xsl:when test="$type = 'String'">
                    <xsl:value-of select="'^(-?[\d.]+)?[s]'" />
                </xsl:when>
                <xsl:when test="$type = 'Boolean'">
                    <xsl:value-of select="'$^'" />
                </xsl:when>
                <xsl:when test="$type = 'Character'">
                    <xsl:value-of select="'^(-?\+?[\d.]+)?[c]'" />
                </xsl:when>
                <xsl:when test="$type = 'Float'  or $type = 'Double'">
                    <xsl:value-of select="'^(-?\+?[\d.]+)?[ef]'" />
                </xsl:when>
                <xsl:when test="$type = 'Integer'">
                    <xsl:value-of select="'^(-?\+?[\d.]+)?z?[diux]'" />
                </xsl:when>
                <xsl:otherwise>
                    <xsl:value-of select="false()" />
                </xsl:otherwise>
            </xsl:choose>
        </xsl:variable>
        
        <!--<xsl:message><xsl:value-of select="concat('Looking For:', $regex, ' in ', $printf_arg, if($match = true()) then 'TRUE' else 'FALSE', o:nl(1))" /></xsl:message>-->
        <xsl:value-of select="matches($printf_arg, $regex)" />
    </xsl:function>

    
    <xsl:function name="cdit:compare_printf_arg" as="xs:boolean">
        <xsl:param name="variadic_parameter" as="element(gml:node)"/>
        <xsl:param name="printf_arg" as="xs:string"/>

        <xsl:variable name="data_sources" select="graphml:get_sources($variadic_parameter, 'Edge_Data')" />
        
        <xsl:if test="count($data_sources) = 0">
            <!-- Can't validate if we have no type to compare -->
            <xsl:value-of select="true()" />
        </xsl:if>

        <xsl:if test="count($data_sources) > 0">
            <xsl:variable name="data_source" select="$data_sources[1]" />
            <xsl:variable name="type" select="graphml:get_type($data_source)" />

            <!-- Validate the types -->
            <xsl:value-of select="cdit:compare_type_to_printf($type, $printf_arg)" />
        </xsl:if>
    </xsl:function>

    <xsl:function name="cdit:test_componentimpl_workflow">
        <xsl:param name="component_impls" as="element(gml:node)*"/>

        <!--<xsl:variable name="results">-->
            <xsl:for-each select="$component_impls">
                <xsl:for-each select="graphml:get_descendant_nodes_of_kind(., 'WorkerProcess')">
                    <xsl:variable name="id" select="graphml:get_id(.)" />
                    <xsl:variable name="label" select="graphml:get_label(.)" />
                    <xsl:variable name="worker" select="graphml:get_data_value(., 'worker')" />
                    <xsl:variable name="operation" select="graphml:get_data_value(., 'operation')" />
                    

                    <xsl:if test="$worker = 'Utility_Worker' and $operation = 'Log'">
                        <xsl:variable name="input_parameters" select="graphml:get_descendant_nodes_of_kind(., 'InputParameter')" />
                        <xsl:variable name="variadic_parameters" select="graphml:get_descendant_nodes_of_kind(., 'VariadicParameter')" />

                        <xsl:variable name="print_format" select="graphml:get_data_value($input_parameters[1], 'value')" />
                        <xsl:variable name="args" select="tokenize($print_format, '%')" />
                        <xsl:variable name="number_args" select="count($args) - 1" />
                        
                        <xsl:if test="$number_args = count($variadic_parameters)">
                            <xsl:for-each select="$variadic_parameters">
                                <xsl:variable name="variadic_parameter" select="." />
                                <xsl:variable name="pos" as="xs:integer" select="position() + 1"/>
                                <xsl:variable name="arg" select="$args[$pos]" />
                                
                                <xsl:variable name="valid_compare" select="cdit:compare_printf_arg($variadic_parameter, $arg)" />
                                <xsl:value-of select="cdit:output_result(graphml:get_id($variadic_parameter), $valid_compare,  o:join_list(('Variadic Parameter #', string(position()), 'does not appear to match types with the fprintf args', o:wrap_quote($arg)), ' '), true(), 2)" />
                            </xsl:for-each>
                        </xsl:if>

                        <xsl:value-of select="cdit:output_result($id, $number_args = count($variadic_parameters), o:join_list(('Log Function', o:wrap_quote($label), ' appears to have differing numbers of % symbols in the print statemement compared to number of variadic parameters'), ' '), true(), 2)" />        
                    
                    </xsl:if>
                </xsl:for-each>
            </xsl:for-each>
        <!--</xsl:variable>-->

        <xsl:value-of select="cdit:output_test('Utility_Worker Log functions are defined correctly', '', 1)" />
    </xsl:function>


    <xsl:function name="cdit:test_component_relations">
        <xsl:param name="components" as="element(gml:node)*"/>

        <xsl:variable name="results">
            <xsl:for-each select="$components">
                <xsl:variable name="id" select="graphml:get_id(.)" />
                <xsl:variable name="label" select="graphml:get_label(.)" />
                
                <!-- Get the sources -->
                <xsl:variable name="sources" select="graphml:get_sources(., 'Edge_Definition')" />
                <xsl:variable name="definition_kinds" select="graphml:get_data_values($sources, 'kind')" />
                
                <!-- Check if we -->
                <xsl:variable name="impl_count" select="count($definition_kinds[contains(., 'ComponentImpl')])" />
                <xsl:variable name="instance_count" select="count($definition_kinds[contains(., 'ComponentInstance')])" />
                
                <xsl:choose>
                    <xsl:when test="$instance_count > 0">
                        <xsl:value-of select="cdit:output_result($id, $impl_count = 1, o:join_list(('Component', o:wrap_quote($label), ', which has Instances, does not have any defined ComponentImpl'), ' '), false(), 2)" />        
                    </xsl:when>
                    <xsl:otherwise>
                        <xsl:value-of select="cdit:output_result($id, $impl_count = 1, o:join_list(('Component', o:wrap_quote($label), 'does not have any defined ComponentImpl'), ' '), true(), 2)" />        
                    </xsl:otherwise>
                </xsl:choose>
            </xsl:for-each>
        </xsl:variable>

        <xsl:value-of select="cdit:output_test('All Components are defined', $results, 1)" />
    </xsl:function>


    <xsl:function name="cdit:test_eventport_delegates">
        <xsl:param name="model" as="element(gml:node)"/>

        <xsl:variable name="delegates" select="graphml:get_descendant_nodes_of_kind($model, ('SubscriberPortDelegate', 'PublisherPortDelegate'))" />

        <xsl:variable name="results">
            <xsl:for-each select="$delegates">
                <xsl:variable name="id" select="graphml:get_id(.)" />
                <xsl:variable name="label" select="graphml:get_label(.)" />

                <xsl:variable name="port_aggregate_targets" select="graphml:get_targets(., 'Edge_Aggregate')" />
                <xsl:value-of select="cdit:output_result($id, count($port_aggregate_targets) = 1, o:join_list(('PortDelegate', o:wrap_quote($label), 'is not connected (Edge_Aggregate) to an Aggregate'), ' '), false(), 2)" />        
            </xsl:for-each>
        </xsl:variable>
        
        <xsl:value-of select="cdit:output_test('EventportDelegates established correctly', $results, 1)" />
    </xsl:function>

    <xsl:function name="cdit:test_eventport_instances">
        <xsl:param name="model" as="element(gml:node)"/>

        <xsl:variable name="instances" select="graphml:get_descendant_nodes_of_kind($model, ('SubscriberPortInstance', 'PublisherPortInstance'))" />

        <xsl:variable name="results">
            <xsl:for-each select="$instances">
                <xsl:variable name="id" select="graphml:get_id(.)" />
                <xsl:variable name="label" select="graphml:get_label(.)" />
                <xsl:variable name="middleware" select="graphml:get_data_value(., 'middleware')" />

                <xsl:if test="lower-case($middleware) = 'ospl'">
                    <xsl:variable name="port_def" select="graphml:get_definition(.)" />
                    <!-- Get all AggregateInstances used by the port definition -->
                    <xsl:variable name="aggregate_instances" select="graphml:get_descendant_nodes_of_kind($port_def, 'AggregateInstance')" />
                    <xsl:variable name="aggregate_definitions" select="graphml:get_definitions($aggregate_instances)" />

                    <xsl:for-each select="$aggregate_definitions">
                        <xsl:variable name="aggregate_id" select="graphml:get_id(.)" />
                        <xsl:variable name="aggregate_keys" select="graphml:get_keys(.)" />
                        <xsl:variable name="got_key" select="count($aggregate_keys) > 0" />        
                        <xsl:variable name="type" select="graphml:get_type(.)" />        
                        <xsl:value-of select="cdit:output_result($aggregate_id, $got_key, o:join_list(('OpenSplice requires that Aggregate ', o:wrap_quote($type), 'has a child with data', o:wrap_quote('is_key'), 'set to true'), ' '), false(), 2)" />        
                    </xsl:for-each>
                </xsl:if>
            </xsl:for-each>
        </xsl:variable>
        <xsl:value-of select="cdit:output_test('PortInstances established correctly', $results, 1)" />
    </xsl:function>

    

    
    <xsl:function name="cdit:test_eventport_aggregates">
        <xsl:param name="components" as="element(gml:node)*"/>

        <xsl:variable name="results">
            <xsl:for-each select="$components">
                <xsl:variable name="eventports" select="graphml:get_descendant_nodes_of_kind(., ('SubscriberPort', 'PublisherPort'))" />
                
                <xsl:for-each select="$eventports">
                    <xsl:variable name="id" select="graphml:get_id(.)" />
                    <xsl:variable name="label" select="graphml:get_label(.)" />
                    
                    <xsl:variable name="aggregates" select="graphml:get_child_nodes(.)" />
                    
                    <xsl:variable name="linked_aggregates" select="graphml:get_targets(., 'Edge_Aggregate')" />


                    <xsl:choose>
                        <xsl:when test="count($linked_aggregates) = 1 and count($aggregates) = 1">
                            <!-- Compare the Definition ID of the Aggregate contained in the EventPort to the Aggregate the EventPort is connected to via Edge_Aggregate -->
                            <xsl:variable name="aggregate_def" select="graphml:get_definition($aggregates[1])" />

                            <xsl:value-of select="cdit:output_result($id, $aggregate_def = $linked_aggregates[1], o:join_list(('Port', o:wrap_quote($label), 'is connected to an Aggregate different to the AggregateInstance it contains'), ' '), false(), 2)" />        
                        </xsl:when>
                        <xsl:when test="count($linked_aggregates) = 0">
                            <xsl:value-of select="cdit:output_result($id, false(), o:join_list(('Port', o:wrap_quote($label), 'is not connected (Edge_Aggregate) to an Aggregate'), ' '), false(), 2)" />        
                        </xsl:when>
                        <xsl:when test="count($aggregates) = 0">
                            <xsl:value-of select="cdit:output_result($id, false(), o:join_list(('Port', o:wrap_quote($label), 'does not contain an instance of an Aggregate'), ' '), false(), 2)" />        
                        </xsl:when>
                    </xsl:choose>
                </xsl:for-each>
            </xsl:for-each>
        </xsl:variable>

        <xsl:value-of select="cdit:output_test('Eventports established correctly', $results, 1)" />
    </xsl:function>

    <!-- Test that the middleware of all ports that are connected match, and check the topicnames if they need to match also -->
    <xsl:function name="cdit:test_assembly_connections">
        <xsl:param name="component_instances" as="element(gml:node)*"/>

        <xsl:variable name="results">
            <xsl:for-each select="$component_instances">
                <xsl:for-each select="graphml:get_descendant_nodes_of_kind(., 'PublisherPortInstance')">
                    <xsl:variable name="src" select="." />
                    <xsl:variable name="src_id" select="graphml:get_id($src)" />
                    <xsl:variable name="src_label" select="graphml:get_label($src)" />
                    <xsl:variable name="src_kind" select="graphml:get_kind($src)" />
                    <xsl:variable name="src_middleware" select="graphml:get_data_value($src, 'middleware')" />
                    <xsl:variable name="src_topic" select="graphml:get_data_value($src, 'topicName')" />
                    
                    <xsl:variable name="requires_topic" select="cdit:middleware_requires_topic($src_middleware)" />

                    <xsl:if test="$requires_topic">
                        <xsl:value-of select="cdit:output_result($src_id, $src_topic != '', o:join_list(($src_kind, o:wrap_quote($src_label), 'has invalid topicName'), ' '), false(), 2)"/> 
                    </xsl:if>

                    <xsl:for-each select="graphml:get_targets_recurse(., 'Edge_Assembly')">
                        <xsl:variable name="dst" select="." />
                        <xsl:variable name="dst_id" select="graphml:get_id($dst)" />
                        <xsl:variable name="dst_label" select="graphml:get_label($dst)" />
                        <xsl:variable name="dst_kind" select="graphml:get_kind($dst)" />
                        <xsl:variable name="dst_middleware" select="graphml:get_data_value($dst, 'middleware')" />
                        <xsl:variable name="dst_topic" select="graphml:get_data_value($dst, 'topicName')" />
                        
                        <xsl:if test="$dst_kind = 'SubscriberPortInstance'">
                            <xsl:variable name="match_middleware" select="cdit:middlewares_match($src_middleware, $dst_middleware)" />
                            
                            <xsl:if test="$requires_topic">
                                <xsl:value-of select="cdit:output_result($dst_id, $src_topic = $dst_topic, o:join_list(($src_kind, o:wrap_quote($src_label), 'is connected to', $dst_kind, o:wrap_quote($dst_label), 'which have different topicName'), ' '), false(), 2)"/> 
                            </xsl:if>
                            <xsl:value-of select="cdit:output_result($dst_id, $match_middleware, o:join_list(($src_kind, o:wrap_quote($src_label), 'is connected to', $dst_kind, o:wrap_quote($dst_label), 'which have incompatible middlewares'), ' '), false(), 2)"/> 
                        </xsl:if>
                    </xsl:for-each>
                </xsl:for-each>
            </xsl:for-each>
        </xsl:variable>
        <xsl:value-of select="cdit:output_test('Assembly Tests', $results, 1)" />
    </xsl:function>

    
    <!-- Test that all nodes are deployed -->
    <xsl:function name="cdit:test_deployment">
        <xsl:param name="component_instances" as="element(gml:node)*"/>
        
        <xsl:variable name="results">
            <xsl:for-each select="$component_instances">
                <xsl:variable name="id" select="graphml:get_id(.)" />
                <xsl:variable name="label" select="graphml:get_label(.)" />

                <xsl:variable name="deployed_nodes" select="graphml:get_targets(., 'Edge_Deployment')" />
                
                 <xsl:variable name="parent_deployed_nodes" as="element()*">
                    <xsl:for-each select="graphml:get_ancestor_nodes_of_kind(., 'ComponentAssembly')">
                        <xsl:for-each select="graphml:get_targets(., 'Edge_Deployment')">
                            <xsl:sequence select="." />
                        </xsl:for-each>
                    </xsl:for-each>
                </xsl:variable>

                <xsl:variable name="is_directly_deployed" select="count($deployed_nodes) > 0" />
                <xsl:variable name="is_indirectly_deployed" select="count($parent_deployed_nodes) > 0" />
                 
                <xsl:variable name="is_deployed" select="$is_directly_deployed or $is_indirectly_deployed" />
                
                <xsl:value-of select="cdit:output_result($id, $is_deployed, o:join_list(('ComponentInstance', o:wrap_quote($label), 'is not deployed'), ' '), false(), 2)"/> 

                <xsl:if test="count($deployed_nodes) = 1 and count($parent_deployed_nodes) = 1">
                    <xsl:variable name="same_node" select="$deployed_nodes[1] = $parent_deployed_nodes[1]" />
                    <xsl:value-of select="cdit:output_result($id, $is_deployed, o:join_list(('ComponentInstance', o:wrap_quote($label), 'deployed to a different HardwareNode than one if its ancestor ComponentAssembly entities'), ' '), true(), 2)"/> 
                </xsl:if>
            </xsl:for-each>
        </xsl:variable>
        <xsl:value-of select="cdit:output_test('Deployment Tests', $results, 1)" />
    </xsl:function>

    <xsl:function name="cdit:get_cpp_reserved_words" as="xs:string*">
        <xsl:sequence select="('alignas', 'alignof', 'and', 'and_eq', 'asm', 'atomic_cancel', 'atomic_commit', 'atomic_noexcept', 'auto', 'bitand', 'bitor', 'bool', 'break', 'case', 'catch', 'char', 'char16_t', 'char32_t', 'class', 'compl', 'concept', 'const', 'constexpr', 'const_cast', 'continue', 'co_await', 'co_return', 'co_yield', 'decltype', 'default', 'delete', 'do', 'double', 'dynamic_cast', 'else', 'enum', 'explicit', 'export', 'extern', 'false', 'float', 'for', 'friend', 'goto', 'if', 'import ', 'inline', 'int', 'long', 'module ', 'mutable', 'namespace', 'new', 'noexcept', 'not', 'not_eq', 'nullptr', 'operator', 'or', 'or_eq', 'private', 'protected', 'public', 'register', 'reinterpret_cast', 'requires', 'return', 'short', 'signed', 'sizeof', 'static', 'static_assert', 'static_cast', 'struct', 'switch', 'synchronized', 'template', 'this', 'thread_local', 'throw', 'true', 'try', 'typedef', 'typeid', 'typename', 'union', 'unsigned', 'using', 'virtual', 'void', 'volatile', 'wchar_t', 'while', 'xor', 'xor_eq')" />
    </xsl:function>

    <xsl:function name="cdit:get_re_reserved_words" as="xs:string*">
        <xsl:sequence select="('Aggregate', 'Component', 'BaseMessage', 'Activatable', 'Attribute', 'ModelLogger', 'Worker', 'SubscriberPort', 'PublisherPort')" />
    </xsl:function>
    
    <xsl:function name="cdit:test_invalid_label">
        <xsl:param name="entities" as="element(gml:node)*"/>
        <xsl:param name="test"/>

        <xsl:variable name="invalid_characters" select="'\/:*?&quot;&gt;&lt;| '"  />

        <xsl:variable name="replace_map">
             <xsl:for-each select="1 to string-length($invalid_characters)">*</xsl:for-each>
        </xsl:variable>

        <xsl:variable name="invalid_labels" select="(cdit:get_re_reserved_words(), cdit:get_cpp_reserved_words())"  />

        <xsl:variable name="results">  
            <xsl:for-each select="$entities">
                <xsl:variable name="id" select="graphml:get_id(.)" />
                <xsl:variable name="label" select="graphml:get_label(.)" />
                <xsl:variable name="kind" select="graphml:get_kind(.)" />
                
                <xsl:variable name="label_replaced" select="translate($label, $invalid_characters, '')" /> 
                <xsl:variable name="label_print" select="translate($label, $invalid_characters, $replace_map)" />   
                <xsl:variable name="invalid_count" select="count($invalid_labels[$label = .])" />

                <xsl:value-of select="cdit:output_result($id, $label = $label_replaced, o:join_list(($kind, o:wrap_quote($label), 'has an invalid characters in label', o:wrap_quote($label_print), '(Replaced with *)'), ' '), false(), 2)"/> 
                <xsl:value-of select="cdit:output_result($id, $invalid_count = 0, o:join_list(($kind, o:wrap_quote($label), 'has an invalid Class Type which is reserved or a symbol used by runtime environment.'), ' '), false(), 2)"/> 

            </xsl:for-each>
        </xsl:variable>

        <xsl:value-of select="cdit:output_test($test, $results, 1)" />
    </xsl:function>



    <xsl:function name="cdit:aggregate_tests">
        <xsl:param name="model" as="element(gml:node)*"/>

        <xsl:variable name="aggregates" as="element()*" select="graphml:get_descendant_nodes_of_kind($model, 'Aggregate')" />
        <xsl:variable name="enums" as="element()*" select="graphml:get_descendant_nodes_of_kind($model, 'Enum')" />

        <xsl:variable name="members" as="element()*" select="graphml:get_descendant_nodes_of_kind($model, 'Member')" />

        <xsl:variable name="aggregate_descendants" as="element()*">
            <xsl:sequence select="$aggregates" />
            <xsl:for-each select="$aggregates">
                <xsl:sequence select="graphml:get_descendant_nodes(.)" />
            </xsl:for-each>
        </xsl:variable>

        
        <xsl:value-of select="cdit:test_namespace_collisions($aggregates)" />
        <xsl:value-of select="cdit:test_invalid_label($aggregate_descendants, 'Descendants of an Aggregate require valid labels')" />
        <xsl:value-of select="cdit:test_requires_children($aggregates, 'Aggregate entities require at least one child')" />
        <xsl:value-of select="cdit:test_requires_children($enums, 'Enum entities require at least one child')" />
        <xsl:value-of select="cdit:test_invalid_label($enums, 'Enum valid names')" />
        <xsl:value-of select="cdit:test_invalid_label($members, 'Member valid names')" />

        <!-- <xsl:value-of select="cdit:test_aggregate_requires_key($aggregates)" />-->
        
        <xsl:value-of select="cdit:test_unique_child_labels($aggregates)" />
        <xsl:value-of select="cdit:test_aggregate_unique_type($aggregates)" />

        <xsl:variable name="vectors" as="element()*" select="graphml:get_descendant_nodes_of_kind($model, ('Vector', 'VectorInstance'))" />
        <xsl:value-of select="cdit:test_unique_child_labels($vectors)" />
    </xsl:function>

    <xsl:function name="cdit:component_tests">
        <xsl:param name="model" as="element(gml:node)*"/>

        <xsl:variable name="components" as="element()*" select="graphml:get_descendant_nodes_of_kind($model, 'Component')" />
        <xsl:variable name="component_impls" as="element()*" select="graphml:get_descendant_nodes_of_kind($model, 'ComponentImpl')" />

        <xsl:value-of select="cdit:test_unique_labels($components, 'require to have unique labels', false())" />
        <xsl:value-of select="cdit:test_invalid_label($components, 'Component valid names')" />
        <xsl:value-of select="cdit:test_invalid_label(graphml:get_descendant_nodes_of_kind($model, 'Variable'), 'Variable valid names')" />
        <xsl:value-of select="cdit:test_component_relations($components)" />
        <xsl:value-of select="cdit:test_eventport_aggregates($components)" />



        <xsl:value-of select="cdit:test_componentimpl_data($component_impls)" />
        <xsl:value-of select="cdit:test_logfunction($component_impls)" />
        <xsl:value-of select="cdit:test_componentimpl_workflow($component_impls)" />
        
    </xsl:function>

    <xsl:function name="cdit:deployment_tests">
        <xsl:param name="model" as="element(gml:node)*"/>

        <xsl:variable name="component_instances" as="element()*" select="graphml:get_descendant_nodes_of_kind($model, 'ComponentInstance')" />
        <xsl:variable name="eventport_instances" as="element()*" select="graphml:get_descendant_nodes_of_kind($model, ('PublisherPortInstance', 'SubscriberPortInstance'))" />
        
        <xsl:value-of select="cdit:test_assembly_connections($component_instances)" />
        <xsl:value-of select="cdit:test_eventport_instances($model)" />
        <xsl:value-of select="cdit:test_eventport_delegates($model)" />
        <xsl:value-of select="cdit:test_deployment($component_instances)" />
    </xsl:function>

    

    


</xsl:stylesheet>