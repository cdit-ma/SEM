<xsl:stylesheet version="2.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:xs="http://www.w3.org/2001/XMLSchema"
    xmlns:gml="http://graphml.graphdrawing.org/xmlns"
    xmlns:cdit="http://github.com/cdit-ma"
    xmlns:o="http://github.com/cdit-ma"
    >

    <xsl:import href="functions.xsl"/>


    <xsl:function name="cdit:output_test">
        <xsl:param name="test_name" />
        <xsl:param name="contents" />
        <xsl:param name="tab" />

        <xsl:value-of select="o:xml_wrap('test',concat('name=', o:dblquote_wrap($test_name)), $contents, $tab)" />
    </xsl:function>

    <xsl:function name="cdit:output_result">
        <xsl:param name="id" as="xs:string"/>
        <xsl:param name="result" as="xs:boolean"/>
        <xsl:param name="error_string" as="xs:string" />
        <xsl:param name="warning" as="xs:boolean" />
        <xsl:param name="tab" />

        <xsl:variable name="warning_string" select="if($warning = true()) then concat(' warning=', o:dblquote_wrap(string($warning))) else ''" />
        <xsl:variable name="success_string" select="concat(' success=', o:dblquote_wrap(string($result)))" />
        <xsl:variable name="id_string" select="concat('id=', o:dblquote_wrap($id))" />

        <xsl:value-of select="o:xml_wrap('result',concat($id_string, $success_string, $warning_string), if($result = false()) then $error_string else '', $tab)" />
    </xsl:function>

    <xsl:function name="cdit:test_aggregate_requires_key">
        <xsl:param name="root"/>
        <xsl:param name="aggregates" as="element()*" />

        <xsl:variable name="results">  
            <xsl:for-each select="$aggregates">
                <xsl:variable name="id" select="cdit:get_node_id(.)" />
                <xsl:variable name="key_id" select="cdit:get_key_id(., 'key')" />
                <xsl:variable name="type" select="cdit:get_key_value(., 'type')" />
                <xsl:variable name="got_key" select="count(./gml:graph/gml:node/gml:data[@key=$key_id and lower-case(text()) = 'true']) > 0" />        
                <xsl:value-of select="cdit:output_result($id, $got_key, concat('Aggregate ', o:quote_wrap($type), ' has no child with data ', o:quote_wrap('key'), ' set to true'), false(), 2)" />        
            </xsl:for-each>
        </xsl:variable>

        <xsl:value-of select="cdit:output_test('All Aggregate entities require a direct child to be set as key', $results, 1)" />
    </xsl:function>

    <xsl:function name="cdit:test_requires_children">
        <xsl:param name="root"/>
        <xsl:param name="entity_kind" />
        <xsl:param name="test"/>

        <xsl:variable name="entities" as="element()*" select="cdit:get_entities_of_kind($root, $entity_kind)" />
        

        <xsl:variable name="results">  
            <xsl:for-each select="$entities">
                <xsl:variable name="id" select="cdit:get_node_id(.)" />
                <xsl:variable name="got_key" select="count(./gml:graph/gml:node) > 0" />        
                <xsl:value-of select="cdit:output_result($id, $got_key, concat($entity_kind, ' requires a child entity'), false(), 2)" />        
            </xsl:for-each>
        </xsl:variable>

        <xsl:value-of select="cdit:output_test($test, $results, 1)" />
    </xsl:function>

    <xsl:function name="cdit:test_aggregate_unique_type">
        <xsl:param name="root"/>
        <xsl:param name="aggregates" as="element()*" />

        <xsl:variable name="type_key_id" select="cdit:get_key_id($root, 'type')" />
        
        <xsl:variable name="all_types" select="$aggregates/gml:data[@key=$type_key_id]" />        
        
        <xsl:variable name="results">  
            <xsl:for-each select="$aggregates">
                <xsl:variable name="id" select="cdit:get_node_id(.)" />
                <xsl:variable name="type" select="./gml:data[@key=$type_key_id]/text()" />
                <!-- Check the number of times the type is in the list of all types-->
                <xsl:variable name="matched_types" select="count($all_types[text() = $type])" />        
                <xsl:value-of select="cdit:output_result($id, $matched_types = 1, concat('Aggregate type ', o:quote_wrap($type), ' is not unique in model'), false(), 2)" />        
            </xsl:for-each>
        </xsl:variable>

        <xsl:value-of select="cdit:output_test('All Aggregate entities require unique type', $results, 1)" />
    </xsl:function>

    <xsl:function name="cdit:test_components_unique_name">
        <xsl:param name="root"/>
        <xsl:param name="components" as="element()*" />

        <xsl:variable name="label_key_id" select="cdit:get_key_id($root, 'label')" />
        
        <xsl:variable name="all_labels" select="$components/gml:data[@key=$label_key_id]" />        
        
        <xsl:variable name="results">  
            <xsl:for-each select="$components">
                <xsl:variable name="id" select="cdit:get_node_id(.)" />
                <xsl:variable name="label" select="./gml:data[@key=$label_key_id]/text()" />
                <!-- Check the number of times the type is in the list of all types-->
                <xsl:variable name="matched_labels" select="count($all_labels[text() = $label])" />        
                <xsl:value-of select="cdit:output_result($id, $matched_labels = 1, concat('Component label ', o:quote_wrap($label), ' not unique in model'), false(), 2)" />        
            </xsl:for-each>
        </xsl:variable>

        <xsl:value-of select="cdit:output_test('All Component entities require unique labels', $results, 1)" />
    </xsl:function>

    <xsl:function name="cdit:test_component_impls">
        <xsl:param name="root"/>
        <xsl:param name="components" as="element()*" />

        <xsl:variable name="kind_key_id" select="cdit:get_key_id($root, 'kind')" />
        
        <xsl:variable name="results">
            <xsl:for-each select="$components">
                <xsl:variable name="id" select="cdit:get_node_id(.)" />
                <xsl:variable name="label" select="cdit:get_key_value(., 'label')" />
                
                <!-- Get the sources -->
                <xsl:variable name="sources" select="cdit:get_edge_sources(., 'Edge_Definition', $id)" />
                
                <!-- Check if we -->
                <xsl:variable name="got_impl" select="count($sources/gml:data[@key=$kind_key_id and text() = 'ComponentImpl']) = 1" />        
                <xsl:variable name="got_instances" select="count($sources/gml:data[@key=$kind_key_id and text() = 'ComponentInstance']) > 0" />        
                
                <xsl:choose>
                    <xsl:when test="$got_instances">
                        <xsl:value-of select="cdit:output_result($id, $got_impl, concat('Component ', o:quote_wrap($label), ' which has instances, does not have any defined ComponentImpl'), false(), 2)" />        
                    </xsl:when>
                    <xsl:otherwise>
                        <xsl:value-of select="cdit:output_result($id, $got_impl, concat('Component ', o:quote_wrap($label), ' does not have any defined ComponentImpl'), true(), 2)" />        
                    </xsl:otherwise>
                </xsl:choose>
            </xsl:for-each>
        </xsl:variable>

        <xsl:value-of select="cdit:output_test('All Components are defined', $results, 1)" />
    </xsl:function>


    <xsl:function name="cdit:test_eventport_delegates">
        <xsl:param name="root"/>

        <xsl:variable name="indelegates" as="element()*" select="cdit:get_entities_of_kind($root, 'InEventPortDelegate')" />   
        <xsl:variable name="outdelegates" as="element()*" select="cdit:get_entities_of_kind($root, 'OutEventPortDelegate')" />   
        <xsl:variable name="eventportdelegates" as="element()*" select="$indelegates, $outdelegates" />   

        <xsl:variable name="results">
            <xsl:for-each select="$eventportdelegates">
                <xsl:variable name="id" select="cdit:get_node_id(.)" />
                <xsl:variable name="label" select="cdit:get_key_value(., 'label')" />
                <xsl:variable name="port_aggregate_targets" select="cdit:get_edge_targets(., 'Edge_Aggregate', $id)" />
                <xsl:value-of select="cdit:output_result($id, count($port_aggregate_targets) = 1, concat('EventPortDelegate ', o:quote_wrap($label), ' is not connected (Edge_Aggregate) to an Aggregate'), false(), 2)" />        
            </xsl:for-each>
        </xsl:variable>

        <xsl:value-of select="cdit:output_test('EventportDelegates established correctly', $results, 1)" />
    </xsl:function>

    
    <xsl:function name="cdit:test_eventport_aggregates">
        <xsl:param name="root"/>
        <xsl:param name="components" as="element()*" />

        <xsl:variable name="results">
            <xsl:for-each select="$components">
                <xsl:variable name="ineventports" as="element()*" select="cdit:get_child_entities_of_kind(., 'InEventPort')" />   
                <xsl:variable name="outeventports" as="element()*" select="cdit:get_child_entities_of_kind(., 'OutEventPort')" />   
                <xsl:variable name="eventports" as="element()*" select="$ineventports, $outeventports" />   

                <xsl:for-each select="$eventports">
                    <xsl:variable name="id" select="cdit:get_node_id(.)" />
                    <xsl:variable name="label" select="cdit:get_key_value(., 'label')" />
                    
                    <xsl:variable name="aggregates" select="cdit:get_child_nodes(.)" />

                    

                    <xsl:variable name="port_aggregate_targets" select="cdit:get_edge_targets(., 'Edge_Aggregate', $id)" />


                    <xsl:choose>
                        <xsl:when test="count($port_aggregate_targets) = 1 and count($aggregates) = 1">
                            <!-- Compare the Definition ID of the Aggregate contained in the EventPort to the Aggregate the EventPort is connected to via Edge_Aggregate -->
                            <xsl:variable name="aggregate" select="$aggregates[1]" />
                            <xsl:variable name="aggregate_def" select="cdit:get_definition($aggregate)" />
                            <xsl:variable name="aggregate_def_id" select="cdit:get_node_id($aggregate_def)" />

                            <xsl:value-of select="cdit:output_result($id, $port_aggregate_targets[1] = $aggregate_def_id, concat('EventPort ', o:quote_wrap($label), ' is connected to an Aggregate different to the AggregateInstance it contains'), false(), 2)" />        
                        </xsl:when>
                        <xsl:when test="count($port_aggregate_targets) = 0">
                            <xsl:value-of select="cdit:output_result($id, false(), concat('EventPort ', o:quote_wrap($label), ' is not connected (Edge_Aggregate) to an Aggregate'), false(), 2)" />        
                        </xsl:when>
                        <xsl:when test="count($aggregates) = 0">
                            <xsl:value-of select="cdit:output_result($id, false(), concat('EventPort ', o:quote_wrap($label), ' does not contain an instance of an Aggregate'), false(), 2)" />        
                        </xsl:when>
                    </xsl:choose>
                </xsl:for-each>
            </xsl:for-each>
        </xsl:variable>

        <xsl:value-of select="cdit:output_test('Eventports established correctly', $results, 1)" />
    </xsl:function>

    <xsl:function name="cdit:test_assembly_connections">
        <xsl:param name="root"/>
        <xsl:param name="component_instances" as="element()*" />

        <xsl:variable name="kind_key_id" select="cdit:get_key_id($root, 'kind')" />
        
        <xsl:variable name="results">
            <xsl:for-each select="$component_instances">
                <xsl:variable name="out_event_ports" as="element()*" select="cdit:get_entities_of_kind(., 'OutEventPortInstance')" />
                <xsl:for-each select="$out_event_ports">
                    <xsl:variable name="source_id" select="cdit:get_node_id(.)" />
                    <xsl:variable name="source_label" select="cdit:get_key_value(., 'label')" />
                    <xsl:variable name="source_kind" select="cdit:get_key_value(., 'kind')" />
                    <xsl:variable name="source_middleware" select="cdit:get_key_value(., 'middleware')" />
                    <xsl:variable name="source_topic" select="cdit:get_key_value(., 'topicName')" />
                    <xsl:variable name="requires_topic" select="cdit:middleware_requires_topic($source_middleware)" />

                    <xsl:if test="$requires_topic">
                        <xsl:variable name="valid_topic" select="$source_topic != ''" />
                        <xsl:value-of select="cdit:output_result($source_id, $valid_topic, concat($source_kind, ' ', o:quote_wrap($source_label), ' has invalid topicName'), false(), 2)"/> 
                    </xsl:if>

                    <!-- Get all assembly connections -->
                    <xsl:variable name="targets" select="cdit:get_all_edge_targets($root, 'Edge_Assembly', $source_id)" />
                    <xsl:for-each select="$targets">
                        <xsl:variable name="target_id" select="cdit:get_node_id(.)" />
                        <xsl:variable name="target_kind" select="cdit:get_key_value(., 'kind')" />
                        <xsl:variable name="target_label" select="cdit:get_key_value(., 'label')" />
                        <xsl:variable name="target_middleware" select="cdit:get_key_value(., 'middleware')" />
                        <xsl:variable name="target_topic" select="cdit:get_key_value(., 'topicName')" />
                        <xsl:if test="$target_kind = 'InEventPortInstance'">
                            <xsl:variable name="match_middleware" select="cdit:middlewares_match($source_middleware, $target_middleware)" />
                            
                            <xsl:if test="$requires_topic">
                                <xsl:variable name="match_topics" select="$source_topic = $target_topic" />
                                <xsl:value-of select="cdit:output_result($target_id, $match_topics, concat($source_kind, ' ', o:quote_wrap($source_label), ' is connected to ', $target_kind, ' ', o:quote_wrap($target_label), ' which have different topicName'), false(), 2)"/> 
                            </xsl:if>
                            <xsl:value-of select="cdit:output_result($target_id, $match_middleware, concat($source_kind, ' ', o:quote_wrap($source_label), ' is connected to ', $target_kind, ' ', o:quote_wrap($target_label), ' which have incompatible middlewares'), false(), 2)"/> 
                        </xsl:if>
                    </xsl:for-each>
                </xsl:for-each>
            </xsl:for-each>
        </xsl:variable>
        <xsl:value-of select="cdit:output_test('Assembly Tests', $results, 1)" />
    </xsl:function>

    <xsl:function name="cdit:test_deployment">
        <xsl:param name="root"/>
        <xsl:param name="component_instances" as="element()*" />

        <xsl:variable name="kind_id" select="cdit:get_key_id($root, 'kind')" />
        
        <xsl:variable name="results">
            <xsl:for-each select="$component_instances">
                <xsl:variable name="id" select="cdit:get_node_id(.)" />
                <xsl:variable name="kind" select="cdit:get_key_value(., 'kind')" />
                <xsl:variable name="label" select="cdit:get_key_value(., 'label')" />

                <xsl:variable name="deployed_nodes" select="cdit:get_edge_targets($root, 'Edge_Deployment', $id)" as="element()*"/>
                
                 <xsl:variable name="parent_deployed_nodes" as="element()*">
                    <xsl:for-each select="cdit:get_parents_of_kind(., 'ComponentAssembly')">
                        <xsl:for-each select="cdit:get_edge_targets($root, 'Edge_Deployment', cdit:get_node_id(.))">
                            <xsl:sequence select="." />
                        </xsl:for-each>
                    </xsl:for-each>
                </xsl:variable>

                <xsl:variable name="is_directly_deployed" select="count($deployed_nodes) > 0" />
                <xsl:variable name="is_indirectly_deployed" select="count($parent_deployed_nodes) > 0" />
                 
                <xsl:variable name="is_deployed" select="$is_directly_deployed or $is_indirectly_deployed" />
                
                <xsl:value-of select="cdit:output_result($id, $is_deployed, concat($kind, ' ', o:quote_wrap($label), ' is not deployed to a HardwareNode'), false(), 2)"/> 

                <xsl:if test="$is_directly_deployed and $is_indirectly_deployed">
                    <xsl:variable name="same_node" select="$deployed_nodes[1] = $parent_deployed_nodes[1]" />
                    <xsl:value-of select="cdit:output_result($id, $same_node, concat($kind, ' ', o:quote_wrap($label), ' is deployed to a different HardwareNode than one if its ancestor ComponentAssembly entities'), true(), 2)"/> 
                </xsl:if>        
            </xsl:for-each>
        </xsl:variable>
        <xsl:value-of select="cdit:output_test('Deployment Tests', $results, 1)" />
    </xsl:function>

    
    
    <xsl:function name="cdit:test_invalid_label">
        <xsl:param name="root" />
        <xsl:param name="entity_kind" />
        <xsl:param name="test"/>

        <xsl:variable name="invalid_characters" select="'\/:*?&quot;&gt;&lt;| '"  />
        <xsl:variable name="entities" as="element()*" select="cdit:get_entities_of_kind($root, $entity_kind)" />

        <xsl:variable name="replace_map">
             <xsl:for-each select="1 to string-length($invalid_characters)">*</xsl:for-each>
        </xsl:variable>

        <xsl:variable name="results">  
            <xsl:for-each select="$entities">
                <xsl:variable name="id" select="cdit:get_node_id(.)" />
                <xsl:variable name="kind" select="cdit:get_key_value(., 'kind')" />
                <xsl:variable name="label" select="cdit:get_key_value(., 'label')" />
                <xsl:variable name="label_replaced" select="translate($label, $invalid_characters, '')" /> 
                <xsl:variable name="label_print" select="translate($label, $invalid_characters, $replace_map)" />   

                <xsl:variable name="got_valid_label" select="$label = $label_replaced" />        
                <xsl:value-of select="cdit:output_result($id, $got_valid_label, concat($kind, ' ', o:quote_wrap($label), ' has an invalid characters in label ', o:quote_wrap($label_print), ' (Replaced with *)'), false(), 2)"/> 
            </xsl:for-each>
        </xsl:variable>

        <xsl:value-of select="cdit:output_test($test, $results, 1)" />
    </xsl:function>



    <xsl:function name="cdit:aggregate_tests">
        <xsl:param name="root" />

        <xsl:variable name="aggregates" as="element()*" select="cdit:get_entities_of_kind($root, 'Aggregate')" />

        <xsl:value-of select="cdit:test_invalid_label($root, 'Aggregate', 'Aggregate valid names')" />
        <xsl:value-of select="cdit:test_invalid_label($root, 'Member', 'Member valid names')" />
        <xsl:value-of select="cdit:test_invalid_label($root, 'Vector', 'Vector valid names')" />
        
        <xsl:value-of select="cdit:test_requires_children($root, 'Aggregate', 'Aggregate entities require at least one child')" />

        <xsl:value-of select="cdit:test_aggregate_requires_key($root, $aggregates)" />
        <xsl:value-of select="cdit:test_aggregate_unique_type($root, $aggregates)" />

        

        <xsl:value-of select="cdit:test_requires_children($root, 'Vector', 'Vector entities require at least one child')" />
        
    </xsl:function>

    <xsl:function name="cdit:component_tests">
        <xsl:param name="root" />

        <xsl:variable name="components" as="element()*" select="cdit:get_entities_of_kind($root, 'Component')" />
        

        <xsl:value-of select="cdit:test_components_unique_name($root, $components)" />
        <xsl:value-of select="cdit:test_invalid_label($root, 'Component', 'Component valid names')" />
        <xsl:value-of select="cdit:test_invalid_label($root, 'Variable', 'Vector valid names')" />
        <xsl:value-of select="cdit:test_component_impls($root, $components)" />
        <xsl:value-of select="cdit:test_eventport_aggregates($root, $components)" />
    </xsl:function>

    <xsl:function name="cdit:deployment_tests">
        <xsl:param name="root" />

        <xsl:variable name="component_instances" as="element()*" select="cdit:get_entities_of_kind($root, 'ComponentInstance')" />
        
        <xsl:value-of select="cdit:test_assembly_connections($root, $component_instances)" />
        <xsl:value-of select="cdit:test_eventport_delegates($root)" />
        <xsl:value-of select="cdit:test_deployment($root, $component_instances)" />
    </xsl:function>

    

    


</xsl:stylesheet>