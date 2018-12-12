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

        <xsl:variable name="test_results" select="xmlo:wrap_tag('results', '', $contents, $tab)" as="xs:string"/>
        <xsl:variable name="results" select="parse-xml-fragment($contents)" as="document-node()" />

        <xsl:variable name="tests" select="$results//result" />
        <xsl:variable name="failed_tests" select="$tests[@success='false']" />
        <xsl:variable name="warning_tests" select="$tests[@success='false' and @warning='true']" />
        <xsl:variable name="passed_tests" select="$tests[@success='true']" />

        <xsl:variable name="args" as="xs:string*">
            <xsl:sequence select="xmlo:attribute('name', $test_name)" />
            <xsl:sequence select="xmlo:attribute('tests', string(count($tests)))" />
            <xsl:sequence select="xmlo:attribute('passed', string(count($passed_tests)))" />
            <xsl:sequence select="xmlo:attribute('failed', string(count($failed_tests)))" />
            <xsl:sequence select="xmlo:attribute('warnings', string(count($warning_tests)))" />
        </xsl:variable>

        <xsl:variable name="failed_test_string" as="xs:string*">
            <xsl:for-each select="$failed_tests">
                <xsl:sequence select="concat(o:t($tab + 1), serialize(.), o:nl(1))" />
            </xsl:for-each>
        </xsl:variable>


        <xsl:value-of select="xmlo:wrap_tag('test', o:join_list($args, ' '), $failed_test_string, $tab)" />
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
        <xsl:variable name="sanitized_error" select="xmlo:sanitize_xml($error_string)" />

        

        <xsl:value-of select="xmlo:wrap_tag('result', concat($id_string, $success_string, $warning_string), if($result = false()) then $sanitized_error else '', $tab)" />
    </xsl:function>

    <xsl:function name="cdit:test_ospl_aggregate_requires_key">
        <xsl:param name="aggregates" as="element()*" />

        <xsl:variable name="results">  
            <xsl:for-each select="$aggregates">
                <xsl:variable name="id" select="graphml:get_id(.)" />
                <xsl:variable name="type" select="graphml:get_type(.)" />
                <xsl:variable name="aggregate_keys" select="graphml:get_keys(.)" />
                <xsl:variable name="got_key" select="count($aggregate_keys) > 0" />        
                <xsl:value-of select="cdit:output_result($id, $got_key, o:join_list(('Aggregate', o:wrap_quote($type), ' used by OSPL has no child with data', o:wrap_quote('is_key'), 'set to true'), ' '), false(), 2)" />        
            </xsl:for-each>
        </xsl:variable>

        <xsl:value-of select="cdit:output_test('All Aggregate entities used by OSPL require a direct child to be set as key', $results, 1)" />
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
    <xsl:function name="cdit:test_valid_server_interfaces">
        <xsl:param name="server_interfaces" as="element(gml:node)*"/>
        

        <xsl:variable name="results">  
            <xsl:for-each select="$server_interfaces">
                <xsl:variable name="id" select="graphml:get_id(.)" />
                <xsl:variable name="label" select="graphml:get_label(.)" />
                <xsl:variable name="kind" select="graphml:get_kind(.)" />

                <xsl:variable name="function_name" select="graphml:get_data_value(., 'function_name')" />
                <xsl:variable name="interface_name" select="graphml:get_data_value(., 'interface_name')" />
                
                <xsl:value-of select="cdit:output_result($id, string-length($function_name) > 0, o:join_list(($kind, o:wrap_quote($label), 'requires a valid function name'), ' '), false(), 2)" />
                <xsl:value-of select="cdit:output_result($id, string-length($interface_name) > 0, o:join_list(($kind, o:wrap_quote($label), 'requires a valid interface name'), ' '), false(), 2)" />
            </xsl:for-each>
        </xsl:variable>

        <xsl:value-of select="cdit:output_test('Test ServerInterfaces', $results, 1)" />
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
        <xsl:param name="test" as="xs:string"/>
        <xsl:param name="entities" as="element(gml:node)*"/>

        <xsl:variable name="results">
            <xsl:for-each select="$entities">
                <xsl:value-of select="cdit:test_unique_labels((., graphml:get_child_nodes(.)), 'has a non-unique label within its scope.', false())" />
            </xsl:for-each>
        </xsl:variable>

        <xsl:value-of select="cdit:output_test($test, $results, 1)" />
    </xsl:function>

    <!-- Tests that all entities in list have unique member labels -->
    <xsl:function name="cdit:test_unique_labels" as="xs:string*">
        <xsl:param name="entities" as="element(gml:node)*"/>
        <xsl:param name="error_str" as="xs:string"/>
        <xsl:param name="warning" as="xs:boolean"/>


        <xsl:variable name="all_labels" select="graphml:get_data_values($entities, 'label')" />

        <xsl:for-each select="$entities">
            <xsl:variable name="id" select="graphml:get_id(.)" />
            <xsl:variable name="kind" select="graphml:get_kind(.)" />
            <xsl:variable name="label" select="graphml:get_label(.)" />

            <!-- Check the number of times the type is in the list of all types-->
            <xsl:variable name="match_count" select="o:string_in_list_count($label, $all_labels, true())" />
            <xsl:sequence select="cdit:output_result($id, $match_count = 1, o:join_list(($kind, o:wrap_quote($label), $error_str, string($match_count)), ' '), $warning, 2)" />
        </xsl:for-each>
    </xsl:function>

    <!-- Tests that all aggregates have unique types-->
    <xsl:function name="cdit:test_unique_types">
        <xsl:param name="test" as="xs:string"/>
        <xsl:param name="entities" as="element(gml:node)*"/>

        <xsl:variable name="all_types" select="graphml:get_data_values($entities, 'type')" as="xs:string*" />
        
        <xsl:variable name="results">  
            <xsl:for-each select="$entities">
                <xsl:variable name="id" select="graphml:get_id(.)" />
                <xsl:variable name="kind" select="graphml:get_kind(.)" />
                <xsl:variable name="type" select="graphml:get_type(.)" />
                <xsl:variable name="label" select="graphml:get_label(.)" />

                <xsl:variable name="check_value">
                    <xsl:choose>
                        <xsl:when test="$kind = 'Class'">
                            <xsl:value-of select="$type" />
                        </xsl:when>
                        <xsl:otherwise>
                            <xsl:value-of select="$label" />
                        </xsl:otherwise>
                    </xsl:choose>
                </xsl:variable>

                <!-- Check the number of times the type is in the list of all types-->
                <xsl:variable name="match_count" select="o:string_in_list_count($check_value, $all_types, true())" />

                <xsl:value-of select="cdit:output_result($id, $match_count = 1 , o:join_list(($kind, o:wrap_quote($type), ' does not have a unique qualifed class name', string($match_count)), ' '), false(), 2)" />        
            </xsl:for-each>
        </xsl:variable>
        <xsl:value-of select="cdit:output_test($test, $results, 1)" />
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


    <xsl:function name="cdit:is_node_data_connected" as="xs:boolean">
        <xsl:param name="entity" as="element(gml:node)"/>

        <xsl:variable name="data_sources" select="graphml:get_sources($entity, 'Edge_Data')" />
        <xsl:variable name="value" select="graphml:get_value($entity)" />

        <xsl:variable name="children" select="graphml:get_child_nodes($entity)" />

        <xsl:choose>
            <xsl:when test="count($data_sources) > 0">
                <xsl:value-of select="true()" />
            </xsl:when>
            <xsl:when test="$value != ''">
                <xsl:value-of select="true()" />
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="false()" />
            </xsl:otherwise>
        </xsl:choose>
    </xsl:function>
    
    <xsl:function name="cdit:are_all_children_linked" as="xs:boolean">
        <xsl:param name="entity" as="element(gml:node)"/>
        <xsl:param name="allow_value" as="xs:boolean"/>

        <xsl:variable name="data_sources" select="graphml:get_sources($entity, 'Edge_Data')" />
        <xsl:variable name="value" select="graphml:get_value($entity)" />

        <xsl:variable name="children" select="graphml:get_child_nodes($entity)" />
        <xsl:variable name="child_count" select="count($children)" />

        <xsl:choose>
            <xsl:when test="cdit:is_node_data_connected($entity) = true()">
                <xsl:value-of select="true()" />
            </xsl:when>
            <xsl:when test="$child_count > 0">
                <xsl:variable name="connected_children_count" as="element(gml:node)*">
                    <xsl:for-each select="$children">
                        <xsl:if test="cdit:are_all_children_linked(., $allow_value)">
                            <xsl:sequence select="."/>
                        </xsl:if>
                    </xsl:for-each>
                </xsl:variable>
                <xsl:value-of select="$child_count = count($connected_children_count)" />
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="false()" />
            </xsl:otherwise>
        </xsl:choose>
    </xsl:function>

    <xsl:function name="cdit:is_data_linked" as="xs:boolean">
        <xsl:param name="entity" as="element(gml:node)"/>
        <xsl:param name="allow_value" as="xs:boolean"/>

        <xsl:variable name="parent" select="graphml:get_parent_node($entity)" />

        <xsl:choose>
            <xsl:when test="$parent and cdit:is_node_data_connected($parent)">
                <xsl:value-of select="true()" />        
            </xsl:when>
            <xsl:when test="cdit:are_all_children_linked($entity, $allow_value)">
                <xsl:value-of select="true()" />        
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

    
    <xsl:function name="cdit:test_transition_functions">
        <xsl:param name="component_impls" as="element(gml:node)*"/>

        <xsl:variable name="results">
            <xsl:for-each select="$component_impls">
                <xsl:variable name="component" select="." />
                <xsl:variable name="transition_functions" select="graphml:get_child_nodes_of_kind($component, ('TransitionFunction'))" />
                <xsl:variable name="types" select="for $var in $transition_functions return graphml:get_data_value($var, 'type')" />

                <!-- This should select all non data linked entities -->
                <xsl:for-each select="$transition_functions">
                    <xsl:variable name="id" select="graphml:get_id(.)" />
                    <xsl:variable name="type" select="graphml:get_type(.)" />
                    <xsl:variable name="duplicate_types" select="count($types[$type = .])" />
                    <xsl:value-of select="cdit:output_result($id, $duplicate_types = 1, o:join_list(('Got duplicate TransitionFunction of Type ', $type), ''), false(), 2)" />        
                </xsl:for-each>
            </xsl:for-each>
        </xsl:variable>

        <xsl:value-of select="cdit:output_test('ComponentImpl Duplicate Transition Functions', $results, 1)" />
    </xsl:function>

    <xsl:function name="cdit:requires_data_link" as="xs:boolean">
        <xsl:param name="entity" as="element(gml:node)?"/>

        <xsl:variable name="parent" select="graphml:get_parent_node($entity)" />
        <xsl:variable name="parent_kind" select="graphml:get_kind($parent)" />
        <xsl:variable name="kind" select="graphml:get_kind($entity)" />
        <xsl:variable name="is_optional" select="graphml:evaluate_data_value_as_boolean($entity, 'is_optional_param')" />

        <xsl:choose>
            <xsl:when test="$is_optional">
                <xsl:value-of select="false()" />
            </xsl:when>
            <!-- Input Parameters in Functions/ReplierPortImpls are exempt-->
            <xsl:when test="$kind = ('InputParameterGroup', 'InputParameterGroupInstance') and $parent_kind = ('Function', 'ReplierPortImpl')">
                <xsl:value-of select="false()" />
            </xsl:when>
            <!-- Return Parameters from Function Calls/RequesterPortImpls are exempt -->
            <xsl:when test="$kind = 'ReturnParameterGroupInstance' and $parent_kind = ('FunctionCall', 'RequesterPortImpl')">
                <xsl:value-of select="false()" />
            </xsl:when>
            <!-- Entities contained within variables are exempt -->
            <xsl:when test="$kind = 'Variable'">
                <xsl:value-of select="false()" />
            </xsl:when>
            <!-- Entities contained within an AggregateInstance inside a SubscriberPortImpl are exempt -->
            <xsl:when test="$kind = 'AggregateInstance' and $parent_kind = 'SubscriberPortImpl'">
                <xsl:value-of select="false()" />
            </xsl:when>
            <!-- VectorInstances used in InputParameterGroupInstance are requiring a data-link -->
            <xsl:when test="$kind = 'VectorInstance' and $parent_kind = 'InputParameterGroupInstance'">
                <xsl:value-of select="true()" />
            </xsl:when>
            <!-- Entities contained within AggregateInstance/VectorInstance/Vector are checked by checking the parent -->
            <xsl:when test="$parent_kind = ('AggregateInstance', 'VectorInstance', 'Vector')">
                <xsl:value-of select="false()" />
            </xsl:when>
            <xsl:when test="$parent_kind = ('Variable', 'AggregateInstance', 'InputParameterGroup', 'InputParameterGroupInstance', 'ReturnParameterGroup', 'ReturnParameterGroupInstance')">
                <xsl:value-of select="cdit:requires_data_link($parent)" />
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="true()" />
            </xsl:otherwise>
        </xsl:choose>
    </xsl:function>

    <xsl:function name="cdit:is_data_mislink_warning" as="xs:boolean">
        <xsl:param name="entity" as="element(gml:node)?"/>

        <xsl:variable name="kind" select="graphml:get_kind($entity)" />
        <xsl:variable name="in_aggregate_instance" select="count(graphml:get_ancestor_nodes_of_kind($entity, ('AggregateInstance'))) > 0 or $kind = 'AggregateInstance'"/>
        <xsl:variable name="in_function_call" select="count(graphml:get_ancestor_nodes_of_kind($entity, ('FunctionCall'))) > 0"/>

        <xsl:choose>
            <xsl:when test="$in_aggregate_instance and $in_function_call">
                <xsl:value-of select="false()" />
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="$in_aggregate_instance" />
            </xsl:otherwise>
        </xsl:choose>
    </xsl:function>


    <!-- Tests that all ComponentImpls have all their parameters set via either data-linking or value -->
    <xsl:function name="cdit:test_componentimpl_data">
        <xsl:param name="component_impls" as="element(gml:node)*"/>



        <xsl:variable name="results">
            <xsl:variable name="functions" select="graphml:get_descendant_nodes_of_kind($component_impls, 'Function')" />
            <xsl:value-of select="cdit:test_invalid_label('Functions valid names', $functions, 'operation')" />

            <xsl:for-each select="$component_impls">
                <xsl:variable name="parameters" select="graphml:get_descendant_nodes_of_kind(.,(
                'AggregateInstance',
                'InputParameter',
                'VariadicParameter',
                'VariableParameter',
                'MemberInstance',
                'Member',
                'EnumInstance',
                'VectorInstance',
                'Vector')
                )" />

                <!-- This should select all non data linked entities -->
                <xsl:for-each select="$parameters">
                    <xsl:variable name="id" select="graphml:get_id(.)" />
                    <xsl:variable name="label" select="graphml:get_label(.)" />
                    <xsl:variable name="kind" select="graphml:get_kind(.)" />
                    <xsl:variable name="type" select="graphml:get_type(.)" />
                    <xsl:variable name="value" select="graphml:get_value(.)" />
                        
                    <xsl:if test="cdit:requires_data_link(.)">
                        <xsl:variable name="is_data_linked" select="cdit:is_data_linked(., true())" />
                        <xsl:variable name="is_warning" select="cdit:is_data_mislink_warning(.)"/>

                        <xsl:variable name="description">
                            <xsl:value-of select="o:join_list(($kind, o:wrap_quote($label), if($kind = 'AggregateInstance') then '(or all children of)' else '', 'should have a connected Edge_Data or an explicit value set'), ' ')" />
                        </xsl:variable>
                        <xsl:value-of select="cdit:output_result($id, $is_data_linked, $description, $is_warning, 2)" />        
                    </xsl:if>
                        
                    <xsl:if test="$type = 'String' and $value != ''">
                        <xsl:if test="count(graphml:get_sources(., 'Edge_Data')) = 0">
                            <!-- Check if string that is set is double-quote wrapped -->
                            <xsl:variable name="got_valid_string" select="starts-with($value, o:dblquote()) and ends-with($value , o:dblquote())" />
                            <xsl:value-of select="cdit:output_result($id, $got_valid_string = true(), o:join_list(($kind, o:wrap_quote($label), 'has an explicit string value set which is not double quoted. Are you trying to reference a variable?'), ' '), true(), 2)" />
                        </xsl:if>
                    </xsl:if>
                </xsl:for-each>
            </xsl:for-each>
        </xsl:variable>

        <xsl:value-of select="cdit:output_test('ComponentImpl Data-Linking tests', $results, 1)" />
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

    <xsl:function name="cdit:test_log_function">
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

    <xsl:function name="cdit:get_kinds_requiring_instances" as="xs:string*">
        <xsl:variable name="impl_kinds" select="('ReplierPort', 'RequesterPort', 'Component', 'Class', 'PublisherPort', 'SubscriberPort', 'Attribute')" />
        <xsl:variable name="instance_kinds" select="($impl_kinds, 'Aggregate', 'Enum', 'InputParameterGroup', 'ReturnParameterGroup', 'Member', 'Vector')" />

        <xsl:for-each select="$impl_kinds">
            <xsl:sequence select="concat(., 'Impl')" />
        </xsl:for-each>
        <xsl:for-each select="$instance_kinds">
            <xsl:sequence select="concat(., 'Instance')" />
        </xsl:for-each>
    </xsl:function>

    <xsl:function name="cdit:get_instance_kinds" as="xs:string*">
        <xsl:param name="kind" as="xs:string"/>
        <xsl:choose>
            <xsl:when test="starts-with($kind, 'ReplierPort')">
                <xsl:sequence select="'ReplierPort'" />
            </xsl:when>
            <xsl:when test="starts-with($kind, 'RequesterPort')">
                <xsl:sequence select="'RequesterPort'" />
            </xsl:when>
            <xsl:when test="starts-with($kind, 'PublisherPort')">
                <xsl:sequence select="'PublisherPort'" />
            </xsl:when>
            <xsl:when test="starts-with($kind, 'SubscriberPort')">
                <xsl:sequence select="'SubscriberPort'" />
            </xsl:when>
            <xsl:when test="starts-with($kind, 'Component')">
                <xsl:sequence select="'Component'" />
            </xsl:when>
            <xsl:when test="starts-with($kind, 'Class')">
                <xsl:sequence select="'Class'" />
            </xsl:when>
            <xsl:when test="starts-with($kind, 'Attribute')">
                <xsl:sequence select="'Attribute'" />
            </xsl:when>
            <xsl:when test="$kind = 'PeriodicPortInstance'">
                <xsl:sequence select="'PeriodicPort'" />
            </xsl:when>
            <xsl:when test="$kind = 'AggregateInstance'">
                <xsl:sequence select="'Aggregate'" />
                <xsl:sequence select="$kind" />
            </xsl:when>
            <xsl:when test="$kind = 'VectorInstance'">
                <xsl:sequence select="'Vector'" />
                <xsl:sequence select="$kind" />
            </xsl:when>
            <xsl:when test="$kind = 'EnumInstance'">
                <xsl:sequence select="'Enum'" />
                <xsl:sequence select="$kind" />
            </xsl:when>
            <xsl:when test="$kind = 'MemberInstance'">
                <xsl:sequence select="'Member'" />
                <xsl:sequence select="$kind" />
            </xsl:when>
            <xsl:when test="$kind = 'InputParameterGroupInstance'">
                <xsl:sequence select="'InputParameterGroup'" />
                <xsl:sequence select="$kind" />
            </xsl:when>
            <xsl:when test="$kind = 'ReturnParameterGroupInstance'">
                <xsl:value-of select="'ReturnParameterGroup'" />
                <xsl:sequence select="$kind" />
            </xsl:when>
        </xsl:choose>
    </xsl:function>

   

    <xsl:function name="cdit:test_instances">
        <xsl:param name="model" as="element(gml:node)"/>

        <xsl:variable name="instances" select="graphml:get_descendant_nodes_of_kind($model, cdit:get_kinds_requiring_instances())" />

        <xsl:variable name="results">
            <xsl:for-each select="$instances">
                <xsl:variable name="id" select="graphml:get_id(.)" />
                <xsl:variable name="label" select="graphml:get_label(.)" />
                <xsl:variable name="kind" select="graphml:get_kind(.)" />
                <xsl:variable name="definition" select="graphml:get_definition(.)" />
                <xsl:variable name="definition_kind" select="graphml:get_kind($definition)" />
                <xsl:variable name="required_kind" select="cdit:get_instance_kinds($kind)" />


                <xsl:variable name="port_aggregate_targets" select="graphml:get_targets(., 'Edge_Aggregate')" />
                <xsl:value-of select="cdit:output_result($id, $definition_kind = $required_kind and $definition != ., o:join_list(($kind, o:wrap_quote($label), 'is not connected (Edge_Definition) to an entity of kind', $required_kind[1]), ' '), false(), 2)" />        
            </xsl:for-each>
        </xsl:variable>
        
        <xsl:value-of select="cdit:output_test('Test Instances', $results, 1)" />
    </xsl:function>



    <xsl:function name="cdit:test_unique_topic_names">
        <xsl:param name="model" as="element(gml:node)"/>

        <xsl:variable name="port_instances" select="graphml:get_nodes_of_kind($model, ('SubscriberPortInstance', 'PublisherPortInstance', 'RequesterPortInstance', 'ReplierPortInstance'))" />
        <xsl:variable name="topic_names" select="graphml:get_data_values($port_instances, 'topic_name')" as="xs:string*" />

        <xsl:variable name="results">
            <xsl:for-each-group select="$topic_names" group-by=".">
                <xsl:variable name="topic_name" select="." />
                    <xsl:if test="$topic_name != ''">
                        <xsl:variable name="topics_ports" select="graphml:filter_nodes_which_have_data_value($port_instances, 'topic_name', $topic_name)" />
                        <xsl:variable name="topics_ports_types" select="graphml:get_data_values($topics_ports, 'type')" as="xs:string*" />
                        <xsl:variable name="topic_name_is_valid" select="count(distinct-values($topics_ports_types)) = 1" />

                        <xsl:for-each select="$topics_ports">
                            <xsl:variable name="id" select="graphml:get_id(.)" />
                            <xsl:value-of select="cdit:output_result($id, $topic_name_is_valid, o:join_list(('topic_name', o:wrap_quote($topic_name), 'has multiple types utilising it'), ' '), false(), 2)" />        
                        </xsl:for-each>
                    </xsl:if>
            </xsl:for-each-group>
        </xsl:variable>
        <xsl:value-of select="cdit:output_test('topic_names are unique', $results, 1)" />
    </xsl:function>

    <xsl:function name="cdit:test_data_edges">
        <xsl:param name="model" as="element(gml:node)*"/>

        <xsl:variable name="behaviour_definitions" as="element()*" select="graphml:get_nodes_of_kind($model, 'BehaviourDefinitions')" />
        <xsl:variable name="data_edges" as="element()*" select="graphml:get_edges_of_kind($behaviour_definitions, 'Edge_Data')" />


        <xsl:variable name="results">
            <xsl:for-each select="$data_edges">
                <xsl:variable name="id" select="graphml:get_id(.)" />
                <xsl:variable name="source" select="graphml:get_edges_source(.)" />
                <xsl:variable name="target" select="graphml:get_edges_target(.)" />
                

                <xsl:variable name="source_id" select="graphml:get_id($source)" />
                <xsl:variable name="source_kind" select="graphml:get_kind($source)" />
                <xsl:variable name="source_type" select="graphml:get_type($source)" />
                <xsl:variable name="source_label" select="graphml:get_label($source)" />

                <xsl:variable name="target_id" select="graphml:get_id($target)" />
                <xsl:variable name="target_kind" select="graphml:get_kind($target)" />
                <xsl:variable name="target_type" select="graphml:get_type($target)" />
                <xsl:variable name="target_label" select="graphml:get_label($target)" />

                <xsl:variable name="comparable_types" select="cdit:compare_nodes_types($source, $target)" />
                <xsl:value-of select="cdit:output_result($target_id, $comparable_types, o:join_list(($target_kind, o:wrap_quote($target_label), 'with type', o:wrap_quote($target_type), 'has an invalid type', o:wrap_quote($source_type), 'connected to it (DataEdge)'), ' '), false(), 2)" />        

                <xsl:if test="$source_kind != 'DeploymentAttribute' and $target_kind != 'DeploymentAttribute'">
                    <xsl:variable name="shared_ancestor" select="graphml:get_shared_ancestor($source, $target)" />
                    
                    <xsl:variable name="source_ancestor" as="element(gml:node)?">
                        <xsl:variable name="source_ancestors" select="graphml:get_ancestor_nodes_until($source, $shared_ancestor)" />
                        <xsl:choose>
                            <xsl:when test="count($source_ancestors) = 0">
                                <xsl:sequence select="$source" />
                            </xsl:when>
                            <xsl:otherwise>
                                <xsl:sequence select="$source_ancestors[1]" />
                            </xsl:otherwise>
                        </xsl:choose>
                    </xsl:variable>

                    <xsl:variable name="target_ancestor" as="element(gml:node)?">
                        <xsl:variable name="target_ancestors" select="graphml:get_ancestor_nodes_until($target, $shared_ancestor)" />
                        <xsl:choose>
                            <xsl:when test="count($target_ancestors) = 0">
                                <xsl:sequence select="$target" />
                            </xsl:when>
                            <xsl:otherwise>
                                <xsl:sequence select="$target_ancestors[1]" />
                            </xsl:otherwise>
                        </xsl:choose>
                    </xsl:variable>

                    <xsl:if test="graphml:get_row($source_ancestor) = graphml:get_row($target_ancestor)">
                        <xsl:variable name="source_anc_index" select="graphml:get_index($source_ancestor)" />
                        <xsl:variable name="target_anc_index" select="graphml:get_index($target_ancestor)" />
                        <xsl:value-of select="cdit:output_result($source_id, $source_anc_index lt $target_anc_index, o:join_list(($source_kind, o:wrap_quote($source_label), 'is connected to', $target_kind, o:wrap_quote($target_label), 'before it would have been declared'), ' '), false(), 2)" />        
                    </xsl:if>
                </xsl:if>
            </xsl:for-each>
        </xsl:variable>
        <xsl:value-of select="cdit:output_test('Data Edges established correctly', $results, 1)" />
    </xsl:function>

    
    <xsl:function name="cdit:test_variables">
        <xsl:param name="components" as="element(gml:node)*"/>
        <xsl:variable name="results">
            <xsl:variable name="all_variables" select="graphml:get_descendant_nodes_of_kind($components, ('Variable'))" />
            
            <!-- Test for children -->
            <xsl:value-of select="cdit:test_requires_children($all_variables, 'Variable entities require at least one child')" />
            

            <xsl:for-each select="$components">
                <xsl:variable name="component" select="." />
                <xsl:variable name="variables" select="graphml:get_descendant_nodes_of_kind($component, ('Variable'))" />
                
                <xsl:for-each select="$variables">
                    <xsl:variable name="variable" select="." />
                    <xsl:variable name="id" select="graphml:get_id($variable)" />
                    <xsl:variable name="label" select="cdit:get_variable_name($variable)" />

                    <xsl:variable name="all_ancestors" select="graphml:get_ancestor_nodes_until($variable, $component)" />            
                    <xsl:variable name="variables_in_scope" select="graphml:get_child_nodes_of_kind($all_ancestors, ('Variable')) except $variable" />
                    <xsl:variable name="labels" select="for $var in $variables_in_scope return cdit:get_variable_name($var)" />

                    <xsl:variable name="duplicate_count" select="count($labels[$label = .])" />
                    <xsl:value-of select="cdit:output_result($id, $duplicate_count = 0, o:join_list(('Variable with generated label', o:wrap_quote($label), 'is not unique with scope'), ' '), false(), 2)" />
                </xsl:for-each>
            </xsl:for-each>
        </xsl:variable>

        <xsl:value-of select="cdit:output_test('Variables established correctly', $results, 1)" />
    </xsl:function>
    
    <xsl:function name="cdit:test_eventport_aggregates">
        <xsl:param name="components" as="element(gml:node)*"/>

        <xsl:variable name="results">
            <xsl:for-each select="$components">
                <xsl:variable name="pubsub_ports" select="graphml:get_descendant_nodes_of_kind(., ('SubscriberPort', 'PublisherPort'))" />
                <xsl:variable name="reqrep_ports" select="graphml:get_descendant_nodes_of_kind(., ('RequesterPort', 'ReplierPort'))" />

                <xsl:variable name="pubsub_port_inst" select="graphml:get_descendant_nodes_of_kind(., ('SubscriberPortImpl', 'SubscriberPortInstance', 'PublisherPortImpl', 'PublisherPortInstance'))" />
                <xsl:variable name="reqrep_port_inst" select="graphml:get_descendant_nodes_of_kind(., ('RequesterPortImpl', 'RequesterPortInstance', 'ReplierPortImpl', 'ReplierPortImpl'))" />
                
                <xsl:for-each select="$pubsub_ports">
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
                            <xsl:value-of select="cdit:output_result($id, false(), o:join_list(('PubSub Port', o:wrap_quote($label), 'is not connected (Edge_Aggregate) to an Aggregate'), ' '), false(), 2)" />        
                        </xsl:when>
                        <xsl:when test="count($aggregates) = 0">
                            <xsl:value-of select="cdit:output_result($id, false(), o:join_list(('PubSub Port', o:wrap_quote($label), 'does not contain an instance of an Aggregate'), ' '), false(), 2)" />        
                        </xsl:when>
                    </xsl:choose>
                </xsl:for-each>

                <xsl:for-each select="$reqrep_ports">
                    <xsl:variable name="id" select="graphml:get_id(.)" />
                    <xsl:variable name="label" select="graphml:get_label(.)" />  
                    <xsl:variable name="linked_interfaces" select="graphml:get_targets(., 'Edge_Aggregate')" />

                    <xsl:value-of select="cdit:output_result($id, count($linked_interfaces) = 1, o:join_list(('ReqRep Port', o:wrap_quote($label), 'is not connected (Edge_Aggregate) to a ServerInterface'), ' '), false(), 2)" />        
                </xsl:for-each>
                
                <xsl:for-each select="$pubsub_port_inst, $reqrep_port_inst">
                    <xsl:variable name="id" select="graphml:get_id(.)" />
                    <xsl:variable name="label" select="graphml:get_label(.)" />
                    <xsl:variable name="definition" select="graphml:get_definition(.)" />

                    <xsl:value-of select="cdit:output_result($id, $definition != ., o:join_list(('Port', o:wrap_quote($label), 'is not linked correctly to a Definition'), ' '), false(), 2)" />
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
                <xsl:for-each select="graphml:get_descendant_nodes_of_kind(., ('PublisherPortInstance', 'RequesterPortInstance'))">
                    <xsl:variable name="src" select="." />
                    <xsl:variable name="src_id" select="graphml:get_id($src)" />
                    <xsl:variable name="src_label" select="graphml:get_label($src)" />
                    <xsl:variable name="src_kind" select="graphml:get_kind($src)" />
                    <xsl:variable name="src_middleware" select="graphml:get_data_value($src, 'middleware')" />
                    <xsl:variable name="src_topic" select="graphml:get_data_value($src, 'topic_name')" />

                    <xsl:variable name="valid_dst_kinds" as="xs:string*">
                        <xsl:if test="$src_kind = 'PublisherPortInstance'">
                            <xsl:sequence select="'SubscriberPortInstance'" />
                            <xsl:sequence select="'PubSubPortDelegate'" />
                        </xsl:if>
                        <xsl:if test="$src_kind = 'RequesterPortInstance'">
                            <xsl:sequence select="'ReplierPortInstance'" />
                            <xsl:sequence select="'RequestPortDelegate'" />
                        </xsl:if>
                    </xsl:variable>
                    
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
                        <xsl:variable name="dst_topic" select="graphml:get_data_value($dst, 'topic_name')" />
                        
                        <xsl:if test="$dst_kind = $valid_dst_kinds">
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
        <xsl:param name="entities" as="element(gml:node)*"/>
        
        <xsl:variable name="results">
            <xsl:for-each select="$entities">
                <xsl:variable name="id" select="graphml:get_id(.)" />
                <xsl:variable name="label" select="graphml:get_label(.)" />
                <xsl:variable name="kind" select="graphml:get_kind(.)" />

                <xsl:variable name="is_component" select="$kind='ComponentInstance'" />

                <xsl:variable name="deployed_nodes" select="graphml:get_targets(., 'Edge_Deployment')" />
                
                 <xsl:variable name="parent_deployed_nodes" as="element()*">
                    <xsl:for-each select="graphml:get_ancestor_nodes_of_kind(., 'ComponentAssembly')">
                        <xsl:for-each select="graphml:get_targets(., 'Edge_Deployment')">
                            <xsl:sequence select="." />
                        </xsl:for-each>
                    </xsl:for-each>
                </xsl:variable>

                <xsl:variable name="is_directly_deployed" select="count($deployed_nodes) > 0" />
                <xsl:variable name="is_indirectly_deployed" select="$is_component and count($parent_deployed_nodes) > 0" />
                 
                <xsl:variable name="is_deployed" select="$is_directly_deployed or $is_indirectly_deployed" />

                
                <xsl:value-of select="cdit:output_result($id, $is_deployed, o:join_list(($kind, o:wrap_quote($label), 'is not deployed'), ' '), true(), 2)"/> 

                <xsl:if test="$is_component">
                    <xsl:if test="count($deployed_nodes) = 1 and count($parent_deployed_nodes) = 1">
                        <xsl:variable name="same_node" select="$deployed_nodes[1] = $parent_deployed_nodes[1]" />
                        <xsl:value-of select="cdit:output_result($id, $is_deployed, o:join_list(($kind, o:wrap_quote($label), 'deployed to a different HardwareNode than one if its ancestor ComponentAssembly entities'), ' '), true(), 2)"/> 
                    </xsl:if>
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
        <xsl:param name="test" as="xs:string*" />
        <xsl:param name="entities" as="element(gml:node)*" />
        <xsl:param name="key_to_test" as="xs:string" />

        <xsl:variable name="invalid_characters" select="'\/:*?&quot;&gt;&lt;| ()-=&amp;^%$#@!+.,:;[]{}'"  />

        <xsl:variable name="replace_map">
             <xsl:for-each select="1 to string-length($invalid_characters)">
             <xsl:value-of select="'*'"/>
             </xsl:for-each>
        </xsl:variable>

        <xsl:variable name="invalid_labels" select="(cdit:get_re_reserved_words(), cdit:get_cpp_reserved_words())"  />

        <xsl:variable name="results">  
            <xsl:for-each select="$entities">
                <xsl:variable name="id" select="graphml:get_id(.)" />
                <xsl:variable name="label" select="graphml:get_data_value(., $key_to_test)" />
                <xsl:variable name="kind" select="graphml:get_kind(.)" />
                
                <xsl:variable name="label_replaced" select="translate($label, $invalid_characters, '')" /> 
                <xsl:variable name="label_print" select="translate($label, $invalid_characters, $replace_map)" />   
                <xsl:variable name="invalid_count" select="count($invalid_labels[$label = .])" />

                <xsl:value-of select="cdit:output_result($id, $label = $label_replaced, o:join_list(($kind, o:wrap_quote($label_print), 'has an invalid characters in label', o:wrap_quote($label), '(Replaced with *)'), ' '), false(), 2)"/> 
                <xsl:value-of select="cdit:output_result($id, $invalid_count = 0, o:join_list(($kind, o:wrap_quote($label), 'has an invalid Class Type which is reserved or a symbol used by runtime environment.'), ' '), false(), 2)"/> 
            </xsl:for-each>
        </xsl:variable>

        <xsl:value-of select="cdit:output_test($test, $results, 1)" />
    </xsl:function>

     <xsl:function name="cdit:general_tests">
        <xsl:param name="model" as="element(gml:node)"/>
        <xsl:value-of select="cdit:test_instances($model)" />
    </xsl:function>


    <xsl:function name="cdit:aggregate_tests">
        <xsl:param name="model" as="element(gml:node)*"/>

        <xsl:variable name="aggregates" as="element()*" select="graphml:get_descendant_nodes_of_kind($model, 'Aggregate')" />
        <xsl:variable name="server_interfaces" as="element()*" select="graphml:get_descendant_nodes_of_kind($model, 'ServerInterface')" />

        <xsl:variable name="enums" as="element()*" select="graphml:get_descendant_nodes_of_kind($model, 'Enum')" />

        <xsl:variable name="members" as="element()*" select="graphml:get_descendant_nodes_of_kind($aggregates, 'Member')" />

        <xsl:variable name="aggregate_descendants" as="element()*">
            <xsl:sequence select="$aggregates" />
            <xsl:for-each select="$aggregates">
                <xsl:sequence select="graphml:get_descendant_nodes(.)" />
            </xsl:for-each>
        </xsl:variable>

        
        <xsl:value-of select="cdit:test_namespace_collisions($aggregates)" />
        <xsl:value-of select="cdit:test_invalid_label('Descendants of an Aggregate require valid labels', $aggregate_descendants, 'label')" />
        <xsl:value-of select="cdit:test_requires_children($aggregates, 'Aggregate entities require at least one child')" />
        <xsl:value-of select="cdit:test_requires_children($enums, 'Enum entities require at least one child')" />
        
        <xsl:value-of select="cdit:test_invalid_label('Enum valid names', $enums, 'label')" />
        <xsl:value-of select="cdit:test_valid_server_interfaces($server_interfaces)" />

        

        
        
        <xsl:value-of select="cdit:test_unique_child_labels('Aggregates must have unique labels', $aggregates)" />
        

        <xsl:variable name="vectors" as="element()*" select="graphml:get_descendant_nodes_of_kind($model, ('Vector', 'VectorInstance'))" />
        <xsl:value-of select="cdit:test_unique_child_labels('Vectors must have unique labels', $vectors)" />
    </xsl:function>

    <xsl:function name="cdit:component_tests">
        <xsl:param name="model" as="element(gml:node)*"/>

        <xsl:variable name="aggregates" as="element()*" select="graphml:get_nodes_of_kind($model, 'Aggregate')" />
        <xsl:variable name="components" as="element()*" select="graphml:get_nodes_of_kind($model, 'Component')" />
        

        

        <xsl:variable name="behaviour_definitions" as="element()*" select="graphml:get_nodes_of_kind($model, 'BehaviourDefinitions')" />
        
        <xsl:variable name="classes" as="element()*" select="graphml:get_descendant_nodes_of_kind($behaviour_definitions, 'Class')" />
        <xsl:variable name="component_impls" as="element()*" select="graphml:get_descendant_nodes_of_kind($behaviour_definitions, 'ComponentImpl')" />

        <xsl:value-of select="cdit:test_unique_types('All Compilable Elements Must have unique kinds', ($aggregates, $components, $classes))" />
        <xsl:value-of select="cdit:test_invalid_label('Compilable Elements must have valid names', ($aggregates, $classes, $components), 'label')" />
        

        <xsl:value-of select="cdit:test_component_relations($components)" />
        <xsl:value-of select="cdit:test_eventport_aggregates($components)" />
        <xsl:value-of select="cdit:test_variables(($component_impls, $classes))" />


        <xsl:value-of select="cdit:test_data_edges($model)" />

        <xsl:value-of select="cdit:test_componentimpl_data(($classes, $component_impls))" />
        <xsl:value-of select="cdit:test_log_function(($classes, $component_impls))" />
        <xsl:value-of select="cdit:test_transition_functions(($classes, $component_impls))" />
        
    </xsl:function>

    <xsl:function name="cdit:deployment_tests">
        <xsl:param name="model" as="element(gml:node)*"/>

        <xsl:variable name="component_instances" as="element()*" select="graphml:get_descendant_nodes_of_kind($model, 'ComponentInstance')" />
        <xsl:variable name="eventport_instances" as="element()*" select="graphml:get_descendant_nodes_of_kind($model, ('PublisherPortInstance', 'SubscriberPortInstance'))" />
        <xsl:variable name="deployment_containers" as="element()*" select="graphml:get_descendant_nodes_of_kind($model, 'DeploymentContainer')" />

        
        <xsl:variable name="ospl_aggregates" as="element()*" >
            <xsl:for-each select="$eventport_instances">
                <xsl:if test="graphml:get_data_value(., 'middleware') = 'OSPL'">
                    <xsl:variable name="port_definition" select="graphml:get_definition(.)" />
                    <xsl:sequence select="graphml:get_definition(graphml:get_child_node($port_definition, 1))" />
                </xsl:if>
            </xsl:for-each>
        </xsl:variable>

        <xsl:value-of select="cdit:test_ospl_aggregate_requires_key(o:remove_duplicates($ospl_aggregates))" />

        
        
        <xsl:value-of select="cdit:test_assembly_connections($component_instances)" />
        <xsl:value-of select="cdit:test_unique_topic_names($model)" />
        <xsl:value-of select="cdit:test_eventport_delegates($model)" />
        <xsl:value-of select="cdit:test_deployment(($component_instances, $deployment_containers))" />
    </xsl:function>


    <xsl:function name="cdit:middlewares_match" as="xs:boolean">
        <xsl:param name="middleware1" as="xs:string"/>
        <xsl:param name="middleware2" as="xs:string"/>

        <xsl:variable name="middleware1_lc" select="lower-case($middleware1)" />
        <xsl:variable name="middleware2_lc" select="lower-case($middleware2)" />

        <xsl:choose>
            <xsl:when test="$middleware1_lc = $middleware2_lc">
                <xsl:value-of select="true()" />
            </xsl:when>
            <xsl:when test="($middleware1_lc = 'rti' or $middleware1_lc = 'ospl') and ($middleware2_lc = 'ospl' or $middleware2_lc = 'rti')">
                <xsl:value-of select="true()" />
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="false()" />
            </xsl:otherwise>
        </xsl:choose>
   </xsl:function>

       <xsl:function name="cdit:middleware_requires_topic" as="xs:boolean">
        <xsl:param name="middleware" as="xs:string"/>

        <xsl:variable name="middleware_lc" select="lower-case($middleware)" />

        <xsl:value-of select="$middleware_lc='qpid' or $middleware_lc='rti' or $middleware_lc='ospl'" />
   </xsl:function>


    

    


</xsl:stylesheet>