<!--
    A set of XSLT2.0 Functions for interpretting and reading GraphML XML documents as used by MEDEA
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
        Gets the id attribute for an entity
    -->
    <xsl:function name="graphml:get_id" as="xs:string">
        <xsl:param name="entity" as="element()?" />
        <xsl:value-of select="$entity/@id" />
    </xsl:function>

    <!--
        Gets a node entity (by id) from the graphml document
    -->
    <xsl:function name="graphml:get_node_by_id" as="element(gml:node)?">
        <xsl:param name="entity" as="element()?" />
        <xsl:param name="id"  as="xs:string" />
        <xsl:sequence select="$entity/key('get_node_by_id', $id)" />
    </xsl:function>

    <!--
        Gets a edge entity (by id) from the graphml document
    -->
    <xsl:function name="graphml:get_edge_by_id" as="element(gml:edge)?">
        <xsl:param name="entity" as="element()?" />
        <xsl:param name="id"  as="xs:string" />
        <xsl:sequence select="$entity/key('get_edge_by_id', $id)" />
    </xsl:function>

    <!--
        Gets the model entity from the graphml document
        (The model is the top level node in the graph)
    -->
    <xsl:function name="graphml:get_model" as="element(gml:node)?">
        <xsl:param name="entity" as="element(gml:graphml)" />
        <xsl:sequence select="$entity/gml:graph/gml:node" />
    </xsl:function>

    <!--
        Gets a key id by looking up it's name attribute
    -->
    <xsl:function name="graphml:get_key_id"  as="xs:string?">
        <xsl:param name="entity" as="element()?" />
        <xsl:param name="key_name"  as="xs:string"/>

        <xsl:value-of select="$entity/key('get_key_by_name', $key_name)/@id" />
    </xsl:function>

    <!--
        Gets a data value for an entity by it's key name
    -->
    <xsl:function name="graphml:get_data_value" as="xs:string?">
        <xsl:param name="entity" as="element()?" />
        <xsl:param name="key_name" as="xs:string"/>

        <xsl:variable name="key_id" select="graphml:get_key_id($entity, $key_name)" />
        <xsl:value-of select="$entity/gml:data[@key=$key_id]/text()" />
    </xsl:function>

    <!--
        Tests if a entity has attached data by it's key name
    -->
    <xsl:function name="graphml:got_data" as="xs:boolean">
        <xsl:param name="entity" as="element()?" />
        <xsl:param name="key_name" as="xs:string"/>

        <xsl:variable name="key_id" select="graphml:get_key_id($entity, $key_name)" />
        <xsl:value-of select="boolean($entity/gml:data[@key=$key_id])" />
    </xsl:function>

    <!--
        Gets a list of entities data values by key name
    -->
    <xsl:function name="graphml:get_data_values" as="xs:string*">
        <xsl:param name="entities" as="element()*" />
        <xsl:param name="key_name" as="xs:string"/>

        <xsl:for-each select="$entities">
            <xsl:sequence select="graphml:get_data_value(., $key_name)" />
        </xsl:for-each>
    </xsl:function>

    <!--
        Gets the data value 'kind' for an entity
    -->
    <xsl:function name="graphml:get_kind" as="xs:string?">
        <xsl:param name="entity" as="element()?" />
        <xsl:value-of select="graphml:get_data_value($entity, 'kind')" />
    </xsl:function>

    <!--
        Gets the data value 'label' for an entity
    -->
    <xsl:function name="graphml:get_label" as="xs:string?">
        <xsl:param name="entity" as="element()?" />
        <xsl:value-of select="graphml:get_data_value($entity, 'label')" />
    </xsl:function>

    <!--
        Gets the data value 'middleware' for an entity
    -->
    <xsl:function name="graphml:get_middleware" as="xs:string?">
        <xsl:param name="entity" as="element()?" />
        <xsl:value-of select="lower-case(graphml:get_data_value($entity, 'middleware'))" />
    </xsl:function>
    
    <!--
        Gets the data value 'type' for an entity
    -->
    <xsl:function name="graphml:get_type" as="xs:string">
        <xsl:param name="entity" as="element()?" />
        <xsl:value-of select="graphml:get_data_value($entity, 'type')" />
    </xsl:function>

    <!--
        Gets the data value 'value' for an entity
    -->
    <xsl:function name="graphml:get_value" as="xs:string?">
        <xsl:param name="entity" as="element()?" />
        <xsl:value-of select="graphml:get_data_value($entity, 'value')" />
    </xsl:function>

    <!--
        Gets the data value 'namespace' for an entity
    -->
    <xsl:function name="graphml:get_namespace_data" as="xs:string?">
        <xsl:param name="entity" as="element()?" />

        <xsl:value-of select="graphml:get_data_value($entity, 'namespace')" />
    </xsl:function>

    <!--
        Gets the data value 'index' for an entity, cast as an integer
    -->
    <xsl:function name="graphml:get_index" as="xs:integer?">
        <xsl:param name="entity" as="element()?" />
        <xsl:value-of select="number(graphml:get_data_value($entity, 'index'))" />
    </xsl:function>

    <!--
        Gets the data value 'row' for an entity, cast as an integer
        defaults to 0
    -->
    <xsl:function name="graphml:get_row" as="xs:integer?">
        <xsl:param name="entity" as="element()?" />
        <xsl:variable name="row" select="graphml:get_data_value($entity, 'row')" />
        <xsl:value-of select="if($row) then number($row) else 0" />
    </xsl:function>


    <!--
        Gets the data value 'key' for an entity, cast as a boolean
    -->
    <xsl:function name="graphml:is_key" as="xs:boolean?">
        <xsl:param name="entity" as="element()?" />
        <xsl:value-of select="graphml:evaluate_data_value_as_boolean($entity, 'key')" />
    </xsl:function>

    <!--
        Gets the list of children entities which are tagged as a 'key'
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
        Gets the data value for an entity, by it's key name, and converts it to a boolean
    -->
    <xsl:function name="graphml:evaluate_data_value_as_boolean" as="xs:boolean?">
        <xsl:param name="entity" as="element()" />
        <xsl:param name="key_name" as="xs:string"/>
        <xsl:value-of select="lower-case(graphml:get_data_value($entity, $key_name)) = 'true'" />
    </xsl:function>

    <!--
        Gets the direct children nodes of the node(/s) provided
        * Sorted by 'index'
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
        Gets the direct children nodes of a particular list of 'kinds' of the node(/s) provided
        * Sorted by 'index'
    -->
    <xsl:function name="graphml:get_child_nodes_of_kind" as="element(gml:node)*">
        <xsl:param name="nodes" as="element(gml:node)*" />
        <xsl:param name="kind" as="xs:string*" />

        <xsl:if test="count($nodes)">
            <!-- Get the key id of the 'kind' -->
            <xsl:variable name="kind_id" select="graphml:get_key_id($nodes[1], 'kind')" />
            <!-- Get the all children nodes and return the list which 'kind' key matches any kind -->
            <xsl:sequence select="graphml:get_child_nodes($nodes)/gml:data[@key=$kind_id and text() = $kind]/.." />
        </xsl:if>
    </xsl:function>

    <!--
        Gets a direct children node by index of the ordered children
        * Index starting at 1
    -->
    <xsl:function name="graphml:get_child_node_by_index" as="element(gml:node)?">
        <xsl:param name="node" as="element(gml:node)?" />
        <xsl:param name="index" as="xs:integer" />

        <xsl:variable name="children" select="graphml:get_child_nodes($node)" />

        <xsl:if test="$index > 0 and not($index > count($children))">
            <xsl:sequence select="$children[$index]" />
        </xsl:if>
    </xsl:function>

    <!--
        Gets the parent node of the entity provided
    -->
    <xsl:function name="graphml:get_parent_node" as="element(gml:node)?">
        <xsl:param name="entity" as="element()?" />

        <!-- Step up through the graph into the node hence two steps -->
        <xsl:variable name="parent" select="$entity/../.." />
        <!-- Test if the parent is a node -->
        <xsl:if test="local-name($parent) = 'node'">
            <xsl:sequence select="$parent" />
        </xsl:if>
    </xsl:function>

    <!--
        Gets the 'kind' of the parent node of the entity provided
    -->
    <xsl:function name="graphml:get_parent_kind" as="xs:string?">
        <xsl:param name="entity" as="element()?" />
        <xsl:sequence select="graphml:get_kind(graphml:get_parent_node($entity))" />
    </xsl:function>

    <!--
        Gets a sequence of parent nodes for the entity provided
        * Recursive (Top-most parent first)
    -->
    <xsl:function name="graphml:get_ancestor_nodes" as="element(gml:node)*">
        <xsl:param name="entity" as="element()?" />
    
        <xsl:variable name="parent" select="graphml:get_parent_node($entity)" />
        <xsl:if test="$parent">
            <!-- Recurse up and append to sequence -->
            <xsl:sequence select="graphml:get_ancestor_nodes($parent)" />
            <xsl:sequence select="$parent" />
        </xsl:if>
    </xsl:function>

    <!--
        Gets a sequence of parent nodes for the entity provided which are all descendants of the provided top_ancestor
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
        Gets a sequence of siblings node of the entity provided
        * Includes the entity provided
    -->
    <xsl:function name="graphml:get_siblings" as="element(gml:node)*">
        <xsl:param name="entity" as="element(gml:node)?" />
        <xsl:variable name="parent" select="graphml:get_parent_node($entity)" />
        <xsl:sequence select="graphml:get_child_nodes($parent)" />
    </xsl:function>

    <!--
        Gets a sequence of all descendant nodes of the node provided
    -->
    <xsl:function name="graphml:get_descendant_nodes" as="element(gml:node)*">
        <xsl:param name="node" as="element(gml:node)*" />
        <xsl:sequence select="$node//gml:node" />
    </xsl:function>

    <!--
        Tests if node_1 is a descendant of node_2
    -->
    <xsl:function name="graphml:is_descendant_of" as="xs:boolean">
        <xsl:param name="node_1" as="element(gml:node)" />
        <xsl:param name="node_2" as="element(gml:node)" />

        <xsl:value-of select="$node_1 = graphml:get_descendant_nodes($node_2)" />
    </xsl:function>

    <!--
        Gets all edges of a particular 'kind'
        * Looks in entire model
    -->
    <xsl:function name="graphml:get_edges_of_kind" as="element(gml:edge)*">
        <xsl:param name="node" as="element(gml:node)?" />
        <xsl:param name="kinds" as="xs:string+" />
        
        <xsl:variable name="kind_id" select="graphml:get_key_id($node, 'kind')" />
        <xsl:sequence select="root($node)//gml:edge/gml:data[@key=$kind_id and text() = $kinds]/.." />
    </xsl:function>

    <!--
        Gets all nodes of a particular 'kind'
        * Looks in entire model
    -->
    <xsl:function name="graphml:get_nodes_of_kind" as="element(gml:node)*">
        <xsl:param name="node" as="element(gml:node)+" />
        <xsl:param name="kinds" as="xs:string+" />
        
        <xsl:variable name="kind_id" select="graphml:get_key_id($node, 'kind')" />
        <xsl:sequence select="root($node)//gml:node/gml:data[@key=$kind_id and text() = $kinds]/.." />
    </xsl:function>


    <!--
        Gets a all of descendant nodes of a particular set of 'kinds' of the node provided
    -->
    <xsl:function name="graphml:get_descendant_nodes_of_kind" as="element(gml:node)*">
        <xsl:param name="node" as="element(gml:node)*" />
        <xsl:param name="kinds" as="xs:string+" />
        
        <xsl:variable name="kind_id" select="graphml:get_key_id($node[1], 'kind')" />
        <xsl:sequence select="$node//gml:node/gml:data[@key=$kind_id and text() = $kinds]/.." />
    </xsl:function>

    <!--
        Gets a all of ancestors nodes of a particular set of 'kinds' of the node provided
    -->
    <xsl:function name="graphml:get_ancestor_nodes_of_kind" as="element(gml:node)*">
        <xsl:param name="node" as="element(gml:node)?" />
        <xsl:param name="kinds" as="xs:string+" />
        
        <xsl:variable name="kind_id" select="graphml:get_key_id($node, 'kind')" />
        <xsl:sequence select="$node/ancestor::gml:node/gml:data[@key=$kind_id and text() = $kinds]/.." />
    </xsl:function>

    <!--
        Finds the shared ancestor of two nodes
    -->
    <xsl:function name="graphml:get_shared_ancestor" as="element(gml:node)*">
        <xsl:param name="node_1" as="element(gml:node)?" />
        <xsl:param name="node_2" as="element(gml:node)?" />

        <xsl:variable name="node_1_anc" select="graphml:get_ancestor_nodes($node_1)" />
        <xsl:variable name="node_2_anc" select="graphml:get_ancestor_nodes($node_2)" />

        <xsl:variable name="shared_anc" select="$node_1_anc intersect $node_2_anc" />
        <xsl:sequence select="$shared_anc[last()]" />
    </xsl:function>

    <!--
        Finds all edges of a particular 'kind' which terminate on a provided node.
        * Returns a sequence of all 'source' of the edges
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
        Finds all edges of a particular 'kind' which originate from a provided node.
        * Returns a sequence of all 'target' of the edges
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
        Gets the sequence of all nodes that recursively originate from the provided node.
    -->
    <xsl:function name="graphml:get_targets_recurse">
        <xsl:param name="node" as="element(gml:node)?" />
        <xsl:param name="edge_kinds" as="xs:string+" />
        
        <xsl:for-each select="graphml:get_targets($node, $edge_kinds)">
            <xsl:sequence select="." />
            <xsl:sequence select="graphml:get_targets_recurse(., $edge_kinds)" />
        </xsl:for-each>
   </xsl:function>

    <!--
        Gets all nodes from the provided sequence that have a particular data value for a particular key
    -->
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
        Gets the definition of the node provided
        * Recurses through the definitions definitions
        * Will return node if it has no definitions
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
        Gets the impl of the node provided
        * Will get the impl of the definition
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
        Gets the first definition of the node provided
    -->
    <xsl:function name="graphml:get_first_definition" as="element(gml:node)?">
        <xsl:param name="node" as="element(gml:node)?" />
        
        <!-- 
            We get targets because 'Edge_Definition' are connected s.t
            node is instance of definition
            e.g source is instance of target
            -->
        <xsl:variable name="targets" select="graphml:get_targets($node, 'Edge_Definition')" />

        <!-- Gets the first definition of the node -->
        <xsl:if test="count($targets) > 0">
            <xsl:sequence select="$targets[1]" />
        </xsl:if>
    </xsl:function>

    <!--
        Gets all definitions of a provided sequence of nodes
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
        Gets the first child of a node of kind 'Vector' definition
    -->
    <xsl:function name="graphml:get_vector_child" as="element(gml:node)?">
        <xsl:param name="vector" as="element(gml:node)?" />

        <xsl:variable name="definition" select="graphml:get_definition($vector)" />
        <xsl:sequence select="graphml:get_child_node_by_index($definition, 1)" />
    </xsl:function>

    <!--
        Gets the kind of the vector, by returning the kind of the vector_child
    -->
    <xsl:function name="graphml:get_vector_kind" as="xs:string">
        <xsl:param name="vector" as="element(gml:node)?" />
        <xsl:value-of select="graphml:get_kind(graphml:get_vector_child($vector))" />
    </xsl:function>

    <!--
        Gets the 'Aggregate' entity, used by a port, by returning the port's definition's child
    -->
    <xsl:function name="graphml:get_port_aggregate" as="element(gml:node)?">
        <xsl:param name="port" as="element(gml:node)?" />
        <xsl:variable name="port_definition" select="graphml:get_definition($port)" />
        
        <!-- Get the aggregate edges for the definition -->
        <xsl:variable name="aggregates" select="graphml:get_targets($port_definition, 'Edge_Aggregate')" />

        <!-- Return the first -->
        <xsl:if test="count($aggregates) = 1">
            <xsl:sequence select="$aggregates[1]" />
        </xsl:if>
    </xsl:function>

    <!--
        Tests if a 'ClassInstance' node is a worker
    -->
    <xsl:function name="graphml:is_class_instance_worker" as="xs:boolean">
        <xsl:param name="class_instance" as="element()"  />

        <xsl:variable name="class_def" select="graphml:get_definition($class_instance)" />
        <xsl:value-of select="graphml:got_data($class_def, 'worker')" />
    </xsl:function>

    <!--
        Tests if a 'ClassInstance' node should be ignored as a worker
        Vector_Operations and Component_Info workers aren't actually implemented as compiled workers
        and are implemented by code-gen inline in the model
    -->
    <xsl:function name="graphml:is_ignored_worker_type" as="xs:boolean">
        <xsl:param name="class" as="element()"  />

        <xsl:variable name="worker_type" select="graphml:get_class_worker_type($class)" />
        <xsl:value-of select="$worker_type = 'Vector_Operations' or $worker_type = 'Component_Info'" />
    </xsl:function>

    <!--
        Gets the The 'ClassInstance' children of a 'ComponentImpl' node which are MEDEA provided workers
    -->
    <xsl:function name="graphml:get_worker_instances" as="element(gml:node)*">
        <xsl:param name="component" as="element(gml:node)?" />

        <xsl:sequence select="graphml:get_child_nodes_of_kind($component, 'ClassInstance')[graphml:is_class_instance_worker(.) = true() and graphml:is_ignored_worker_type(.) = false()]" />
    </xsl:function>

    <!--
        Gets the The 'ClassInstance' children of a 'ComponentImpl' node which are custom 'Class'
    -->
    <xsl:function name="graphml:get_custom_class_instances" as="element(gml:node)*">
        <xsl:param name="component" as="element(gml:node)?" />
        <xsl:sequence select="graphml:get_child_nodes_of_kind($component, 'ClassInstance')[graphml:is_class_instance_worker(.) = false()]" />
    </xsl:function>

    <!--
        Gets the 'namespace' of an the Aggregate
        * Steps up all ancestors of an Aggregate and produces a full namespace sequence
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
        Tests to see if the entity provided is deployed
    -->
    <xsl:function name="graphml:is_deployed" as="xs:boolean">
        <xsl:param name="entity" as="element(gml:node)" />
        <xsl:variable name="deployed_nodes" select="graphml:get_targets($entity, 'Edge_Deployment')" />
        <xsl:variable name="parent_node" select="graphml:get_parent_node($entity)" />

        <xsl:choose>
            <xsl:when test="count($deployed_nodes) > 0">
                <xsl:value-of select="true()" />
            </xsl:when>
            <xsl:when test="graphml:get_kind($parent_node) = 'ComponentAssembly'">
                <xsl:value-of select="graphml:is_deployed($parent_node)" />
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="false()" />
            </xsl:otherwise>
        </xsl:choose>
    </xsl:function>

    <!--
        Gets the node which is the source of a provide edge
    -->
    <xsl:function name="graphml:get_edges_source" as="element(gml:node)?">
        <xsl:param name="edge" as="element(gml:edge)?" />
        <xsl:sequence select="graphml:get_node_by_id($edge, $edge/@source)" />
    </xsl:function>

    <!--
        Gets the node which is the target of a provide edge
    -->
    <xsl:function name="graphml:get_edges_target" as="element(gml:node)?">
        <xsl:param name="edge" as="element(gml:edge)?" />
        <xsl:sequence select="graphml:get_node_by_id($edge, $edge/@target)" />
    </xsl:function>

    <!--
        Gets all port instance 'kind's
    -->
    <xsl:function name="graphml:get_port_instance_kinds" as="xs:string*">
        <xsl:sequence select="graphml:get_pubsub_port_instance_kinds() , graphml:get_reqrep_port_instance_kinds()" />
    </xsl:function>

    <!--
        Gets all pub/sub port definition 'kind's
    -->
    <xsl:function name="graphml:get_pubsub_port_kinds" as="xs:string*">
        <xsl:sequence select="'SubscriberPort', 'PublisherPort'"/>
    </xsl:function>

    <!--
        Gets all pub/sub port instance 'kind's
    -->
    <xsl:function name="graphml:get_pubsub_port_instance_kinds" as="xs:string*">
        <xsl:sequence select="'SubscriberPortInstance', 'PublisherPortInstance'"/>
    </xsl:function>

    <!--
        Gets all req/rep port definition 'kind's
    -->
    <xsl:function name="graphml:get_reqrep_port_kinds" as="xs:string*">
        <xsl:sequence select="'RequesterPort', 'ReplierPort'"/>
    </xsl:function>
    
    <!--
        Gets all req/rep port instance 'kind's
    -->
    <xsl:function name="graphml:get_reqrep_port_instance_kinds" as="xs:string*">
        <xsl:sequence select="'RequesterPortInstance', 'ReplierPortInstance'"/>
    </xsl:function>

    <!--
        Gets a list of all deployed 'ComponentInstance' entities
    -->
    <xsl:function name="graphml:get_deployed_component_instances" as="element(gml:node)*">
        <xsl:param name="model" as="element(gml:node)" />
        <xsl:sequence select="graphml:get_nodes_of_kind($model, 'ComponentInstance')[graphml:is_deployed(.)]" />
    </xsl:function>

    <!--
        Gets a list of all deployed 'ComponentInstance' entities children port instances
    -->
    <xsl:function name="graphml:get_deployed_port_instances" as="element(gml:node)*">
        <xsl:param name="model" as="element(gml:node)" />
        <xsl:variable name="deployed_components" select="graphml:get_deployed_component_instances($model)" />
        <xsl:sequence select="graphml:get_child_nodes_of_kind($deployed_components, graphml:get_port_instance_kinds())"/>
    </xsl:function>

    <!--
        Gets the 'type' of a 'Class' entity
    -->
    <xsl:function name="graphml:get_class_worker_type" as="xs:string?">
        <xsl:param name="class" as="element()"  />

        <xsl:if test="graphml:is_class_a_worker($class)">
            <xsl:variable name="class_def" select="graphml:get_definition($class)" />
            <xsl:value-of select="graphml:get_data_value($class_def, 'worker')" />
        </xsl:if>
    </xsl:function>

    <!--
        Tests if a 'Class' entity is a worker
    -->
    <xsl:function name="graphml:is_class_a_worker" as="xs:boolean">
        <xsl:param name="class" as="element(gml:node)"  />

        <xsl:variable name="class_def" select="graphml:get_definition($class)" />
        <xsl:value-of select="graphml:got_data($class_def, 'worker')" />
    </xsl:function>
</xsl:stylesheet>