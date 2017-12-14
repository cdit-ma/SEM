<!-- Functions for reading/interpretting graphml xml -->
<xsl:stylesheet version="2.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:xs="http://www.w3.org/2001/XMLSchema"
    xmlns:gml="http://graphml.graphdrawing.org/xmlns"
    xmlns:graphml="http://github.com/cdit-ma/graphml"
    >

    <!-- Use keys to speed up parsing -->
    <xsl:key name="get_key_by_name" match="gml:key" use="@attr.name"/>
    <xsl:key name="get_key_by_id" match="gml:key" use="@id"/>
    <xsl:key name="get_node_by_id" match="gml:node" use="@id"/>
    
    <xsl:key name="get_data_by_key_id" match="gml:data" use="@key"/>

    <xsl:key name="get_edge_by_id" match="gml:edge" use="@id"/>
    <xsl:key name="get_edge_by_source_id" match="gml:edge" use="@source"/>
    <xsl:key name="get_edge_by_target_id" match="gml:edge" use="@target"/>

    <!--
        Gets the id attribute from the element
    -->
    <xsl:function name="graphml:get_id" as="xs:string">
        <xsl:param name="root" />
        <xsl:value-of select="$root/@id" />
    </xsl:function>

    <!--
        Gets the top most graphml element
    -->
    <xsl:function name="graphml:get_graphml" as="element()">
        <xsl:param name="root" />
        
        <xsl:choose>
            <xsl:when test="count($root/ancestor::gml:graphml) = 1">
                <xsl:sequence select="$root/ancestor::gml:graphml" />
            </xsl:when>
            <xsl:otherwise>
                <xsl:sequence select="$root" />
            </xsl:otherwise>
        </xsl:choose>
    </xsl:function>

    <!--
        Gets a node by id
    -->
    <xsl:function name="graphml:get_node_by_id" as="element()">
        <xsl:param name="root" />
        <xsl:param name="id"  as="xs:string"/>

        <xsl:sequence select="$root/key('get_node_by_id', $id)" />
    </xsl:function>

    <!--
        Gets a node by id
    -->
    <xsl:function name="graphml:get_key_id"  as="xs:string">
        <xsl:param name="root" />
        <xsl:param name="key_name"  as="xs:string"/>

        <xsl:value-of select="$root/key('get_key_by_name', $key_name)/@id" />
    </xsl:function>

    <!--
        Returns the data value with the key: ${key_name} in a node/edge
    -->
    <xsl:function name="graphml:get_data_value" as="xs:string">
        <xsl:param name="root" as="element()*" />
        <xsl:param name="key_name" as="xs:string"/>

        <xsl:variable name="key_id" select="graphml:get_key_id($root, $key_name)" />
        <xsl:value-of select="$root/gml:data[@key=$key_id]/text()" />
    </xsl:function>

    <!--
        Returns the data value with key: kind in a node/edge
    -->
    <xsl:function name="graphml:get_kind" as="xs:string">
        <xsl:param name="root" />
        <xsl:value-of select="graphml:get_data_value($root, 'kind')" />
    </xsl:function>

    <!--
        Returns the data value with key: label in a node/edge
    -->
    <xsl:function name="graphml:get_label" as="xs:string">
        <xsl:param name="root" />
        <xsl:value-of select="graphml:get_data_value($root, 'label')" />
    </xsl:function>
    
    <!--
        Returns the data value with key: type in a node/edge
    -->
    <xsl:function name="graphml:get_type" as="xs:string">
        <xsl:param name="root" />
        <xsl:value-of select="graphml:get_data_value($root, 'type')" />
    </xsl:function>

    <!--
        Returns the data value with key: value in a node/edge
    -->
    <xsl:function name="graphml:get_value" as="xs:string">
        <xsl:param name="root" />
        <xsl:value-of select="graphml:get_data_value($root, 'value')" />
    </xsl:function>

    <!--
        Returns the data value with key: index in a node/edge
    -->
    <xsl:function name="graphml:get_index" as="xs:integer">
        <xsl:param name="root" />
        <xsl:value-of select="number(graphml:get_data_value($root, 'index'))" />
    </xsl:function>

    <!--
        Returns the data value with key: is_key in a node/edge as boolean
    -->
    <xsl:function name="graphml:is_key" as="xs:boolean">
        <xsl:param name="root" />
        <xsl:value-of select="graphml:evaluate_data_value_as_boolean($root, 'key')" />
    </xsl:function>

    <!--
        Returns the data value with key: namespace in a node/edge
    -->
    <xsl:function name="graphml:get_namespace" as="xs:string">
        <xsl:param name="root" />
        <xsl:value-of select="graphml:get_data_value($root, 'namespace')" />
    </xsl:function>

    <!--
        Compares the data value with key ${key_name} against the string representation of true
    -->
    <xsl:function name="graphml:evaluate_data_value_as_boolean" as="xs:boolean">
        <xsl:param name="root" />
        <xsl:param name="key_name" as="xs:string"/>

        <xsl:value-of select="lower-case(graphml:get_data_value($root, $key_name)) = 'true'" />
    </xsl:function>

    <!--
        Checks to see if a node/edge contains a data value with the key: ${key_name}
    -->
    <xsl:function name="graphml:got_key_value" as="xs:boolean">
        <xsl:param name="root" />
        <xsl:param name="key_name" as="xs:string"/>

        <xsl:variable name="key_id" select="graphml:get_key_id($root, $key_name)" />
        <xsl:value-of select="boolean($root/gml:data[@key=$key_id])" />
    </xsl:function>

    <!--
        Gets the child nodes of the current node, sorted by index
    -->
    <xsl:function name="graphml:get_child_nodes" as="element()*">
        <xsl:param name="root" />

        <xsl:for-each select="$root/gml:graph/gml:node">
            <!-- Sort by the index -->
            <xsl:sort select="number(graphml:get_data_value(., 'index'))"/>
            <xsl:sequence select="." />
        </xsl:for-each>
    </xsl:function>

    <!--
        Gets any descendant child of ${root} of a particular kind
    -->
    <xsl:function name="graphml:get_child_nodes_of_kind" as="element()*">
        <xsl:param name="root" />
        <xsl:param name="kind" as="xs:string*" />
        <xsl:variable name="kind_id" select="graphml:get_key_id($root, 'kind')" />

        <xsl:for-each select="$root/gml:graph/gml:node/gml:data[@key=$kind_id and text() = $kind]/..">
            <!-- Sort by the index -->
            <xsl:sort select="number(graphml:get_data_value(., 'index'))"/>
            <xsl:sequence select="." />
        </xsl:for-each>
    </xsl:function>

    <!--
        Gets the child node of the current node, based on the index
    -->
    <xsl:function name="graphml:get_child_node" as="element()*">
        <xsl:param name="root" />
        <xsl:param name="index" as="xs:integer" />
        <xsl:sequence select="graphml:get_child_nodes($root)[$index]" />
    </xsl:function>

    <!--
        Gets the parent of the current node, by looking two levels up
    -->
    <xsl:function name="graphml:get_parent_node" as="element()*">
        <xsl:param name="root" as="element()" />
        <xsl:sequence select="$root/../.." />
    </xsl:function>

    <!--
        Gets any descendant child of ${root} of a particular kind
    -->
    <xsl:function name="graphml:get_descendant_nodes_of_kind" as="element()*">
        <xsl:param name="root" />
        <xsl:param name="kind" as="xs:string" />
        <xsl:variable name="kind_id" select="graphml:get_key_id($root, 'kind')" />
        <xsl:sequence select="$root//gml:node/gml:data[@key=$kind_id and text() = $kind]/.." />
    </xsl:function>


    <!--
        Gets any edges of kind ${edge_kind} which ends at the node ${node}
    -->
    <xsl:function name="graphml:get_sources" as="element()*">
        <xsl:param name="node" as="element()" />
        <xsl:param name="edge_kind" as="xs:string" />

        <xsl:variable name="node_id" select="graphml:get_id($node)" />
        <xsl:variable name="kind_key" select="graphml:get_key_id($node, 'kind')" />

        <xsl:for-each select="$node/key('get_edge_by_target_id', $node_id)/gml:data[@key=$kind_key and text()=$edge_kind]/..">
            <xsl:sequence select="graphml:get_node_by_id(., @source)" />
        </xsl:for-each>
    </xsl:function>

    <!--
        Gets any edges of kind ${edge_kind} which starts at the node ${node}
    -->
    <xsl:function name="graphml:get_targets" as="element()*">
        <xsl:param name="node" as="element()" />
        <xsl:param name="edge_kind" as="xs:string" />

        <xsl:variable name="node_id" select="graphml:get_id($node)" />
        <xsl:variable name="kind_key" select="graphml:get_key_id($node, 'kind')" />

        <xsl:for-each select="$node/key('get_edge_by_source_id', $node_id)/gml:data[@key=$kind_key and text()=$edge_kind]/..">
            <xsl:sequence select="graphml:get_node_by_id(., @target)" />
        </xsl:for-each>
    </xsl:function>

    <!--
        Gets the definition of the provided node
    -->
    <xsl:function name="graphml:get_definition" as="element()?">
        <xsl:param name="node" as="element()" />

        <xsl:variable name="targets" select="graphml:get_targets($node, 'Edge_Definition')" />

        <xsl:for-each select="$targets">
            <xsl:variable name="definition" select="graphml:get_definition(.)" />
            <xsl:if test="count($definition) > 0">
                <xsl:sequence select="$definition" />
            </xsl:if>
        </xsl:for-each>

        <xsl:if test="count($targets) = 0">
            <xsl:sequence select="$node" />
        </xsl:if>
    </xsl:function>

    <!--
        Gets all of the definitions of the provided node list
    -->
    <xsl:function name="graphml:get_definitions" as="element()*">
        <xsl:param name="nodes" as="element()*" />

        <xsl:variable name="definitions" as="element()*">
            <xsl:for-each select="$nodes">
                <xsl:variable name="definition" select="graphml:get_definition(.)" />
                    <xsl:if test="count($definition) > 0">
                        <xsl:sequence select="$definition" />
                    </xsl:if>
            </xsl:for-each>
        </xsl:variable>

        <xsl:for-each-group select="$definitions" group-by="@id">
            <xsl:sequence select="." />
        </xsl:for-each-group>
    </xsl:function>

    <!--
        Gets the child of the vector provided
    -->
    <xsl:function name="graphml:get_vector_child" as="element()">
        <xsl:param name="vector" as="element()" />

        <xsl:variable name="definition" as="element()" select="graphml:get_definition($vector)" />
        <xsl:sequence select="graphml:get_child_node($definition, 1)" />
    </xsl:function>

    <!--
        Gets the kind of the vector provided
    -->
    <xsl:function name="graphml:get_vector_kind" as="xs:string">
        <xsl:param name="vector" as="element()" />
        <xsl:value-of select="graphml:get_kind(graphml:get_vector_child($vector))" />
    </xsl:function>

    <!--
        Gets the child of the vector provided
    -->
    <xsl:function name="graphml:get_port_aggregate" as="element()">
        <xsl:param name="port" as="element()" />

        <xsl:variable name="definition" as="element()" select="graphml:get_definition($port)" />
        <xsl:variable name="child" as="element()" select="graphml:get_child_node($definition, 1)" />
        <xsl:sequence select="graphml:get_definition($child)" />
    </xsl:function>

    
</xsl:stylesheet>