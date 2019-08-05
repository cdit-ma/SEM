<!--
    A set of XSLT2.0 Functions for interpretting and reading GraphML XML documents used by MEDEA
-->
<xsl:stylesheet version="2.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:xs="http://www.w3.org/2001/XMLSchema"
    xmlns:gml="http://graphml.graphdrawing.org/xmlns"
    xmlns:graphml="http://github.com/cdit-ma/re_gen/graphml"
    >

    <!-- xsl:key provide enormous speed ups for repeat searching -->
    <xsl:key name="get_key_by_name" match="gml:key" use="@attr.name"/>
    <xsl:key name="get_key_by_id" match="gml:key" use="@id"/>
    <xsl:key name="get_node_by_id" match="gml:node" use="@id"/>
    <xsl:key name="get_edge_by_id" match="gml:edge" use="@id"/>
    
    <xsl:key name="get_edge_by_source_id" match="gml:edge" use="@source"/>
    <xsl:key name="get_edge_by_target_id" match="gml:edge" use="@target"/>

    <!--
        Gets the id attribute from the entity
    -->
    <xsl:function name="graphml:get_id" as="xs:string">
        <xsl:param name="entity" as="element()?" />
        <xsl:value-of select="$entity/@id" />
    </xsl:function>

    <!--
        Gets a Graphml Node entity by id
    -->
    <xsl:function name="graphml:get_node_by_id" as="element(gml:node)?">
        <xsl:param name="entity" as="element()?" />
        <xsl:param name="id"  as="xs:string" />
        <xsl:sequence select="$entity/key('get_node_by_id', $id)" />
    </xsl:function>

    <!--
        Gets a Graphml Edge entity by id
    -->
    <xsl:function name="graphml:get_edge_by_id" as="element(gml:edge)?">
        <xsl:param name="entity" as="element()?" />
        <xsl:param name="id"  as="xs:string" />
        <xsl:sequence select="$entity/key('get_edge_by_id', $id)" />
    </xsl:function>

    <!--
        Gets the top level graphml model entity
    -->
    <xsl:function name="graphml:get_model" as="element(gml:node)?">
        <xsl:param name="entity" as="element(gml:graphml)" />
        <xsl:sequence select="$entity/gml:graph/gml:node" />
    </xsl:function>

    <!--
        Gets a Graphml Key id, by looking up it's name
    -->
    <xsl:function name="graphml:get_key_id"  as="xs:string?">
        <xsl:param name="entity" as="element()?" />
        <xsl:param name="key_name"  as="xs:string"/>

        <xsl:value-of select="$entity/key('get_key_by_name', $key_name)/@id" />
    </xsl:function>

    <!--
        Gets the data value from the entity, by it's key name
    -->
    <xsl:function name="graphml:get_data_value" as="xs:string?">
        <xsl:param name="entity" as="element()?" />
        <xsl:param name="key_name" as="xs:string"/>

        <xsl:variable name="key_id" select="graphml:get_key_id($entity, $key_name)" />
        <xsl:value-of select="$entity/gml:data[@key=$key_id]/text()" />
    </xsl:function>

    <!--
        Checks if the data value exists, by it's key name
    -->
    <xsl:function name="graphml:got_data" as="xs:boolean">
        <xsl:param name="entity" as="element()?" />
        <xsl:param name="key_name" as="xs:string"/>

        <xsl:variable name="key_id" select="graphml:get_key_id($entity, $key_name)" />
        <xsl:value-of select="boolean($entity/gml:data[@key=$key_id])" />
    </xsl:function>

    <!--
        Gets the data value from all entities, by its keyname
    -->
    <xsl:function name="graphml:get_data_values" as="xs:string*">
        <xsl:param name="entities" as="element()*" />
        <xsl:param name="key_name" as="xs:string"/>

        <xsl:for-each select="$entities">
            <xsl:sequence select="graphml:get_data_value(., $key_name)" />
        </xsl:for-each>
    </xsl:function>

    <!--
        Gets the 'kind' data value from the entity
    -->
    <xsl:function name="graphml:get_kind" as="xs:string?">
        <xsl:param name="entity" as="element()?" />
        <xsl:value-of select="graphml:get_data_value($entity, 'kind')" />
    </xsl:function>

    <!--
        Gets the 'label' data value from the entity
    -->
    <xsl:function name="graphml:get_label" as="xs:string?">
        <xsl:param name="entity" as="element()?" />
        <xsl:value-of select="graphml:get_data_value($entity, 'label')" />
    </xsl:function>

    <!--
        Gets the 'middleware' data value from the entity
    -->
    <xsl:function name="graphml:get_middleware" as="xs:string?">
        <xsl:param name="entity" as="element()?" />
        <xsl:value-of select="lower-case(graphml:get_data_value($entity, 'middleware'))" />
    </xsl:function>
    
    <!--
        Gets the 'type' data value from the entity
    -->
    <xsl:function name="graphml:get_type" as="xs:string">
        <xsl:param name="entity" as="element()?" />
        <xsl:value-of select="graphml:get_data_value($entity, 'type')" />
    </xsl:function>

    <!--
        Gets the 'value' data value from the entity
    -->
    <xsl:function name="graphml:get_value" as="xs:string?">
        <xsl:param name="entity" as="element()?" />
        <xsl:value-of select="graphml:get_data_value($entity, 'value')" />
    </xsl:function>

    <!--
        Gets the 'namespace' data value from the entity
    -->
    <xsl:function name="graphml:get_namespace_data" as="xs:string?">
        <xsl:param name="entity" as="element()?" />

        <xsl:value-of select="graphml:get_data_value($entity, 'namespace')" />
    </xsl:function>

    <!--
        Gets the 'index' data value from the entity, casts as an integer
    -->
    <xsl:function name="graphml:get_index" as="xs:integer?">
        <xsl:param name="entity" as="element()?" />
        <xsl:value-of select="number(graphml:get_data_value($entity, 'index'))" />
    </xsl:function>

    <!--
        Gets the 'index' data value from the entity, casts as an integer
    -->
    <xsl:function name="graphml:get_row" as="xs:integer?">
        <xsl:param name="entity" as="element()?" />
        <xsl:variable name="row" select="graphml:get_data_value($entity, 'row')" />


        <xsl:value-of select="if($row) then number($row) else 0" />
    </xsl:function>


    <!--
        Gets the 'is_key' data value from the entity, converts it to a boolean
    -->
    <xsl:function name="graphml:is_key" as="xs:boolean?">
        <xsl:param name="entity" as="element()?" />
        <xsl:value-of select="graphml:evaluate_data_value_as_boolean($entity, 'key')" />
    </xsl:function>

    <!--
        Gets the children of the entity, which are key's converts it to a boolean
    -->
    <xsl:function name="graphml:get_keys" as="element(gml:node)*">
        <xsl:param name="aggregate" as="element(gml:node)?" />
        <xsl:for-each select="graphml:get_child_nodes($aggregate)">
            <xsl:if test="graphml:is_key(.)">
                <xsl:sequence select="." />
            </xsl:if>
        </xsl:for-each>
    </xsl:function>

    <!--
        Gets the data value from the entity, by it's key name, and compares it against a string boolean
    -->
    <xsl:function name="graphml:evaluate_data_value_as_boolean" as="xs:boolean?">
        <xsl:param name="entity" as="element()" />
        <xsl:param name="key_name" as="xs:string"/>
        <xsl:value-of select="lower-case(graphml:get_data_value($entity, $key_name)) = 'true'" />
    </xsl:function>

    <!--
        Gets the child nodes of the node provided. Sorted by 'index'
    -->
    <xsl:function name="graphml:get_child_nodes" as="element(gml:node)*">
        <xsl:param name="nodes" as="element(gml:node)*" />

        <xsl:for-each select="$nodes/gml:graph/gml:node">
            <!-- Sort by the index -->
            <xsl:sort select="graphml:get_index(.)"/>
            <xsl:sequence select="." />
        </xsl:for-each>
    </xsl:function>

    <!--
        Gets child nodes of the node of the node provided, of a particular 'kind'(s). Sorted by 'index'
    -->
    <xsl:function name="graphml:get_child_nodes_of_kind" as="element(gml:node)*">
        <xsl:param name="nodes" as="element(gml:node)*" />
        <xsl:param name="kind" as="xs:string*" />

        <xsl:if test="count($nodes)">
            <xsl:variable name="kind_id" select="graphml:get_key_id($nodes[1], 'kind')" />
            <xsl:sequence select="graphml:get_child_nodes($nodes)/gml:data[@key=$kind_id and text() = $kind]/.." />
        </xsl:if>
    </xsl:function>

    <!--
        Gets a child node of the node provided, by 'index' (Starting at 1)
    -->
    <xsl:function name="graphml:get_child_node" as="element(gml:node)?">
        <xsl:param name="node" as="element(gml:node)?" />
        <xsl:param name="index" as="xs:integer" />
        <xsl:sequence select="graphml:get_child_nodes($node)[$index]" />
    </xsl:function>

    <!--
        Gets the parent node of the entity provided (Looks two levels up)
    -->
    <xsl:function name="graphml:get_parent_node" as="element(gml:node)?">
        <xsl:param name="entity" as="element()?" />
        <xsl:variable name="parent" select="$entity/../.." />
        <xsl:if test="local-name($parent) = 'node'">
            <xsl:sequence select="$parent" />
        </xsl:if>
    </xsl:function>

    <!--
        Gets the parent node of the entity provided (Looks two levels up)
    -->
    <xsl:function name="graphml:get_parent_kind" as="xs:string?">
        <xsl:param name="entity" as="element()?" />
        <xsl:sequence select="graphml:get_kind(graphml:get_parent_node($entity))" />
    </xsl:function>

    <!--
        Gets the parent node of the entity provided (Looks two levels up)
    -->
    <xsl:function name="graphml:get_ancestor_nodes" as="element(gml:node)*">
        <xsl:param name="entity" as="element()?" />
    
        <xsl:variable name="parent" select="graphml:get_parent_node($entity)" />
        <xsl:if test="$parent">
            <xsl:sequence select="graphml:get_ancestor_nodes($parent)" />
            <xsl:sequence select="$parent" />
        </xsl:if>
        
    </xsl:function>

    <!--
        Gets the parent node of the entity provided (Looks two levels up)
    -->
    <xsl:function name="graphml:get_ancestor_nodes_until" as="element(gml:node)*">
        <xsl:param name="entity" as="element()?" />
        <xsl:param name="top_ancestor" as="element()?" />
    
        <xsl:variable name="ancestors" select="graphml:get_ancestor_nodes($entity)" />
        <xsl:for-each select="$ancestors">
            <xsl:if test="graphml:is_descendant_of(., $top_ancestor)">
                <xsl:sequence select="." />
            </xsl:if>
        </xsl:for-each>
    </xsl:function>

      <!--
        Gets the siblings node of the entity provided (Looks two levels up)
    -->
    <xsl:function name="graphml:get_siblings" as="element(gml:node)*">
        <xsl:param name="entity" as="element(gml:node)?" />
        <xsl:variable name="parent" select="graphml:get_parent_node($entity)" />
        <xsl:sequence select="graphml:get_child_nodes($parent)" />
    </xsl:function>

    <!--
        Gets descendant nodes of the node provided
    -->
    <xsl:function name="graphml:get_descendant_nodes" as="element(gml:node)*">
        <xsl:param name="node" as="element(gml:node)*" />
        <xsl:sequence select="$node//gml:node" />
    </xsl:function>

    <!--
        Checks to see if node1 is a descendant of $node2
    -->
    <xsl:function name="graphml:is_descendant_of" as="xs:boolean">
        <xsl:param name="node1" as="element(gml:node)" />
        <xsl:param name="node2" as="element(gml:node)" />

        <xsl:value-of select="$node1 = graphml:get_descendant_nodes($node2)" />
    </xsl:function>

    <!--
        Gets descendant edges of the node provided, of a particular 'kind'(s)
    -->
    <xsl:function name="graphml:get_edges_of_kind" as="element(gml:edge)*">
        <xsl:param name="node" as="element(gml:node)?" />
        <xsl:param name="kinds" as="xs:string+" />
        
        <xsl:variable name="kind_id" select="graphml:get_key_id($node, 'kind')" />
        <xsl:sequence select="root($node)//gml:edge/gml:data[@key=$kind_id and text() = $kinds]/.." />
    </xsl:function>

    <!--
        Gets descendant edges of the node provided, of a particular 'kind'(s)
    -->
    <xsl:function name="graphml:get_nodes_of_kind" as="element(gml:node)*">
        <xsl:param name="node" as="element(gml:node)+" />
        <xsl:param name="kinds" as="xs:string+" />
        
        <xsl:variable name="kind_id" select="graphml:get_key_id($node, 'kind')" />
        <xsl:sequence select="root($node)//gml:node/gml:data[@key=$kind_id and text() = $kinds]/.." />
    </xsl:function>


    <!--
        Gets descendant nodes of the node provided, of a particular 'kind'(s)
    -->
    <xsl:function name="graphml:get_descendant_nodes_of_kind" as="element(gml:node)*">
        <xsl:param name="node" as="element(gml:node)*" />
        <xsl:param name="kinds" as="xs:string+" />
        
        <xsl:variable name="kind_id" select="graphml:get_key_id($node[1], 'kind')" />
        <xsl:sequence select="$node//gml:node/gml:data[@key=$kind_id and text() = $kinds]/.." />
    </xsl:function>

    <!--
        Gets ancestors nodes of the node provided, of a particular 'kind'(s)
    -->
    <xsl:function name="graphml:get_ancestor_nodes_of_kind" as="element(gml:node)*">
        <xsl:param name="node" as="element(gml:node)?" />
        <xsl:param name="kinds" as="xs:string+" />
        
        <xsl:variable name="kind_id" select="graphml:get_key_id($node, 'kind')" />
        <xsl:sequence select="$node/ancestor::gml:node/gml:data[@key=$kind_id and text() = $kinds]/.." />
    </xsl:function>

    <!--
        Gets ancestors nodes of the node provided, of a particular 'kind'(s)
    -->
    <xsl:function name="graphml:get_shared_ancestor" as="element(gml:node)*">
        <xsl:param name="node1" as="element(gml:node)?" />
        <xsl:param name="node2" as="element(gml:node)?" />

        <xsl:variable name="node1_anc" select="graphml:get_ancestor_nodes($node1)" />
        <xsl:variable name="node2_anc" select="graphml:get_ancestor_nodes($node2)" />

        <xsl:variable name="shared_anc" select="$node1_anc intersect $node2_anc" />
        <xsl:sequence select="$shared_anc[last()]" />
    </xsl:function>


    <!--
        Gets any edge that terminate at the node provided, of a particular 'kind'(s)
    -->
    <xsl:function name="graphml:get_sources" as="element(gml:node)*">
        <xsl:param name="node" as="element(gml:node)?" />
        <xsl:param name="edge_kinds" as="xs:string+" />

        <xsl:variable name="node_id" select="graphml:get_id($node)" />
        <xsl:variable name="kind_id" select="graphml:get_key_id($node, 'kind')" />

        <xsl:for-each select="$node/key('get_edge_by_target_id', $node_id)/gml:data[@key=$kind_id and text()=$edge_kinds]/..">
            <xsl:sequence select="graphml:get_node_by_id(., @source)" />
        </xsl:for-each>
    </xsl:function>

    <!--
        Gets any edge that start at the node provided, of a particular 'kind'(s)
    -->
    <xsl:function name="graphml:get_targets" as="element(gml:node)*">
        <xsl:param name="node" as="element(gml:node)?" />
        <xsl:param name="edge_kinds" as="xs:string+" />

        <xsl:variable name="node_id" select="graphml:get_id($node)" />
        <xsl:variable name="kind_id" select="graphml:get_key_id($node, 'kind')" />

        <xsl:for-each select="$node/key('get_edge_by_source_id', $node_id)/gml:data[@key=$kind_id and text()=$edge_kinds]/..">
            <xsl:sequence select="graphml:get_node_by_id(., @target)" />
        </xsl:for-each>
    </xsl:function>

    <!--
        Gets all targets that start at the node provided, of a particualr 'kind'(s)
        Recurses to each target and find's their matching targets
    -->

    <xsl:function name="graphml:get_targets_recurse">
        <xsl:param name="node" as="element(gml:node)?" />
        <xsl:param name="edge_kinds" as="xs:string+" />
        
        <xsl:for-each select="graphml:get_targets($node, $edge_kinds)">
            <xsl:sequence select="." />
            <xsl:sequence select="graphml:get_targets_recurse(., $edge_kinds)" />
        </xsl:for-each>
   </xsl:function>

   <xsl:function name="graphml:filter_nodes_which_have_data_value" as="element(gml:node)*">
        <xsl:param name="nodes" as="element(gml:node)*" />
        <xsl:param name="data_key" as="xs:string" />
        <xsl:param name="value" as="xs:string" />

        <xsl:for-each select="$nodes">
            <xsl:if test="graphml:get_data_value(., $data_key) = $value">
                <xsl:sequence select="." />
            </xsl:if>
        </xsl:for-each>
    </xsl:function>


    <!--
        Gets the definition of the node provided, will recurse to the first definition
    -->
    <xsl:function name="graphml:get_definition" as="element(gml:node)?">
        <xsl:param name="node" as="element(gml:node)?" />

        <xsl:variable name="targets" select="graphml:get_targets($node, 'Edge_Definition')" />

        <!-- Recurse all the way until we don't have any definitions -->
        <xsl:for-each select="$targets">
            <xsl:variable name="definition" select="graphml:get_definition(.)" />
            <xsl:if test="count($definition) > 0">
                <xsl:sequence select="$definition" />
            </xsl:if>
        </xsl:for-each>

        <!-- If we don't have any edges left, this is the definition -->
        <xsl:if test="count($targets) = 0">
            <xsl:sequence select="$node" />
        </xsl:if>
    </xsl:function>

    <!--
        Gets the Impl of the node provided, will recurse to the first definition
    -->
    <xsl:function name="graphml:get_impl" as="element(gml:node)?">
        <xsl:param name="node" as="element(gml:node)?" />

        <xsl:variable name="definition" select="graphml:get_definition($node)" />

        <xsl:for-each select="graphml:get_sources($definition, 'Edge_Definition')">
            <xsl:variable name="kind" select="graphml:get_kind(.)" />
            
            <xsl:if test="ends-with($kind, 'Impl')">
                <xsl:sequence select="." />
            </xsl:if>
        </xsl:for-each>
    </xsl:function>

    <!--
        Gets the definition of the node provided, will recurse to the first definition
    -->
    <xsl:function name="graphml:get_first_definition" as="element(gml:node)?">
        <xsl:param name="node" as="element(gml:node)?" />

        <xsl:variable name="targets" select="graphml:get_targets($node, 'Edge_Definition')" />

        <!-- Recurse all the way until we don't have any definitions -->
        <xsl:for-each select="$targets">
            <xsl:sequence select="." />
        </xsl:for-each>
    </xsl:function>

    <!--
        Gets the set of definitions of the nodes provided, no duplicates.
    -->
    <xsl:function name="graphml:get_definitions" as="element(gml:node)*">
        <xsl:param name="nodes" as="element(gml:node)*" />

        <!-- Iterate through all nodes, and get their definitions -->
        <xsl:variable name="definitions" as="element()*">
            <xsl:for-each select="$nodes">
                <xsl:variable name="definition" select="graphml:get_definition(.)" />
                    <xsl:if test="count($definition) > 0">
                        <xsl:sequence select="$definition" />
                    </xsl:if>
            </xsl:for-each>
        </xsl:variable>

        <!-- Only return each definition once -->
        <xsl:for-each-group select="$definitions" group-by="@id">
            <xsl:sequence select="." />
        </xsl:for-each-group>
    </xsl:function>

    <!--
        Gets the child of the vector node's definition
    -->
    <xsl:function name="graphml:get_vector_child" as="element(gml:node)?">
        <xsl:param name="vector" as="element(gml:node)?" />

        <xsl:variable name="definition" select="graphml:get_definition($vector)" />
        <xsl:sequence select="graphml:get_child_node($definition, 1)" />
    </xsl:function>

    <!--
        Gets the kind of the vector, by inspecting the vectors child node
    -->
    <xsl:function name="graphml:get_vector_kind" as="xs:string">
        <xsl:param name="vector" as="element(gml:node)?" />
        <xsl:value-of select="graphml:get_kind(graphml:get_vector_child($vector))" />
    </xsl:function>

    <!--
        Gets the Aggregate entity, used by a port, by returning the port's definition's child
    -->
    <xsl:function name="graphml:get_port_aggregate" as="element(gml:node)?">
        <xsl:param name="port" as="element(gml:node)?" />

        <xsl:variable name="port_definition" select="graphml:get_definition($port)" />
        <!-- Check for an Aggregate Edge -->

        <xsl:variable name="aggregates" select="graphml:get_targets($port_definition, 'Edge_Aggregate')" />

        <xsl:if test="count($aggregates) = 1">
            <xsl:sequence select="$aggregates[1]" />
        </xsl:if>
    </xsl:function>

    <xsl:function name="graphml:is_class_instance_worker" as="xs:boolean">
        <xsl:param name="class_instance" as="element()"  />

        <xsl:variable name="class_def" select="graphml:get_definition($class_instance)" />
        <xsl:value-of select="graphml:got_data($class_def, 'worker')" />
    </xsl:function>

    <!--
        Gets the The Worker Instances
    -->
    <xsl:function name="graphml:is_ignored_worker_type" as="xs:boolean">
        <xsl:param name="class" as="element()"  />

        <xsl:variable name="worker_type" select="graphml:get_class_worker_type($class)" />
        <xsl:value-of select="$worker_type = 'Vector_Operations' or $worker_type = 'Component_Info'" />
    </xsl:function>


    <!--
        Gets the The Worker Instances
    -->
    <xsl:function name="graphml:get_worker_instances" as="element(gml:node)*">
        <xsl:param name="component" as="element(gml:node)?" />

        <xsl:sequence select="graphml:get_child_nodes_of_kind($component, 'ClassInstance')[graphml:is_class_instance_worker(.) = true() and graphml:is_ignored_worker_type(.) = false()]" />
    </xsl:function>

    <!--
        Gets the The Custom Class Instances
    -->
    <xsl:function name="graphml:get_custom_class_instances" as="element(gml:node)*">
        <xsl:param name="component" as="element(gml:node)?" />
        <xsl:sequence select="graphml:get_child_nodes_of_kind($component, 'ClassInstance')[graphml:is_class_instance_worker(.) = false()]" />
    </xsl:function>

    <!--
        Gets the 'namespace' from the aggregate/
    -->
    <xsl:function name="graphml:get_namespace" as="xs:string*">
        <xsl:param name="entity" as="element()?" />

        <xsl:variable name="definition" select="graphml:get_definition($entity)" />

        <xsl:for-each select="graphml:get_ancestor_nodes($definition)">
            <xsl:variable name="kind" select="graphml:get_kind(.)" />
            <xsl:if test="$kind = 'Namespace'">
                <xsl:sequence select="graphml:get_label(.)" />
            </xsl:if>
        </xsl:for-each>
    </xsl:function>

    <!--
        Checks to see if the entity is deployed
    -->
    <xsl:function name="graphml:is_deployed" as="xs:boolean">
        <xsl:param name="entity" as="element(gml:node)" />
        
        <xsl:variable name="deployed_nodes" select="graphml:get_targets($entity, 'Edge_Deployment')" />
        <xsl:variable name="parent_deployed_nodes" as="element()*">
            <xsl:for-each select="graphml:get_ancestor_nodes_of_kind($entity, 'ComponentAssembly')">
                <xsl:for-each select="graphml:get_targets(., 'Edge_Deployment')">
                    <xsl:sequence select="." />
                </xsl:for-each>
            </xsl:for-each>
        </xsl:variable>

        <xsl:value-of select="count(($deployed_nodes, $parent_deployed_nodes)) > 0" />
    </xsl:function>

    <!--
        Gets a child node of the node provided, by 'index' (Starting at 1)
    -->
    <xsl:function name="graphml:get_edges_source" as="element(gml:node)?">
        <xsl:param name="edge" as="element(gml:edge)?" />
        <xsl:sequence select="graphml:get_node_by_id($edge, $edge/@source)" />
    </xsl:function>

    <!--

    -->
    <xsl:function name="graphml:get_edges_target" as="element(gml:node)?">
        <xsl:param name="edge" as="element(gml:edge)?" />
        <xsl:sequence select="graphml:get_node_by_id($edge, $edge/@target)" />
    </xsl:function>

    <xsl:function name="graphml:get_port_instance_kinds" as="xs:string*">
        <xsl:sequence select="graphml:get_pubsub_port_instance_kinds() , graphml:get_reqrep_port_instance_kinds()" />
    </xsl:function>

    <xsl:function name="graphml:get_pubsub_port_instance_kinds" as="xs:string*">
        <xsl:sequence select="'SubscriberPortInstance', 'PublisherPortInstance'"/>
    </xsl:function>

    <xsl:function name="graphml:get_pubsub_port_kinds" as="xs:string*">
        <xsl:sequence select="'SubscriberPort', 'PublisherPort'"/>
    </xsl:function>

    <xsl:function name="graphml:get_reqrep_port_kinds" as="xs:string*">
        <xsl:sequence select="'RequesterPort', 'ReplierPort'"/>
    </xsl:function>
    
    <xsl:function name="graphml:get_reqrep_port_instance_kinds" as="xs:string*">
        <xsl:sequence select="'RequesterPortInstance', 'ReplierPortInstance'"/>
    </xsl:function>

    <!--
        Gets a list of Deployed Component Instances
    -->
    <xsl:function name="graphml:get_deployed_component_instances" as="element(gml:node)*">
        <xsl:param name="model" as="element(gml:node)" />
        
        
        <!-- Get all Component Instances -->
        <xsl:for-each select="graphml:get_nodes_of_kind($model, 'ComponentInstance')">
            <xsl:if test="graphml:is_deployed(.)">
                <xsl:sequence select="." />
            </xsl:if>
        </xsl:for-each>
    </xsl:function>

    <!--
        Gets a list of Deployed Port Instances
    -->
    <xsl:function name="graphml:get_deployed_port_instances" as="element(gml:node)*">
        <xsl:param name="model" as="element(gml:node)" />
        
        <xsl:variable name="deployed_components" select="graphml:get_deployed_component_instances($model)" />
        <xsl:sequence select="graphml:get_child_nodes_of_kind($deployed_components, graphml:get_port_instance_kinds())"/>
    </xsl:function>

    <!--
        Gets the Worker Type of a Class
    -->
    <xsl:function name="graphml:get_class_worker_type" as="xs:string?">
        <xsl:param name="class" as="element()"  />

        <xsl:if test="graphml:is_class_a_worker($class)">
            <xsl:variable name="class_def" select="graphml:get_definition($class)" />
            <xsl:value-of select="graphml:get_data_value($class_def, 'worker')" />
        </xsl:if>
    </xsl:function>

    <!--
        Checks if a Class is a Worker
    -->
    <xsl:function name="graphml:is_class_a_worker" as="xs:boolean">
        <xsl:param name="class" as="element(gml:node)"  />

        <xsl:variable name="class_def" select="graphml:get_definition($class)" />
        <xsl:value-of select="graphml:got_data($class_def, 'worker')" />
    </xsl:function>

    

   
</xsl:stylesheet>