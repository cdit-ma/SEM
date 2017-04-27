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
        <xsl:param name="tab" />
        <xsl:value-of select="o:xml_wrap('result',concat('id=', o:dblquote_wrap($id), ' success=', o:dblquote_wrap(string($result))), if($result = false()) then $error_string else '', $tab)" />
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
                <xsl:value-of select="cdit:output_result($id, $got_key, concat('Aggregate ', o:quote_wrap($type), ' has no child with data ', o:quote_wrap('key'), ' set to true.'), 2)" />        
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
                <xsl:value-of select="cdit:output_result($id, $got_key, concat($entity_kind, ' requires a child entity'), 2)" />        
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
                <xsl:value-of select="cdit:output_result($id, $matched_types = 1, concat('Aggregate type ', o:quote_wrap($type), ' is not unique in model'), 2)" />        
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
                <xsl:value-of select="cdit:output_result($id, $matched_labels = 1, concat('Component label ', o:quote_wrap($label), ' not unique in model'), 2)" />        
            </xsl:for-each>
        </xsl:variable>

        <xsl:value-of select="cdit:output_test('All Component entities require unique labels', $results, 1)" />
    </xsl:function>

    

    <xsl:function name="cdit:aggregate_tests">
        <xsl:param name="root" />

        <xsl:variable name="aggregates" as="element()*" select="cdit:get_entities_of_kind($root, 'Aggregate')" />

        <xsl:value-of select="cdit:test_requires_children($root, 'Aggregate', 'Aggregate entities require at least one child')" />
        <xsl:value-of select="cdit:test_aggregate_requires_key($root, $aggregates)" />
        <xsl:value-of select="cdit:test_aggregate_unique_type($root, $aggregates)" />

        <xsl:value-of select="cdit:test_requires_children($root, 'Vector', 'Vector entities require at least one child')" />
        
    </xsl:function>

    <xsl:function name="cdit:component_tests">
        <xsl:param name="root" />

        <xsl:variable name="components" as="element()*" select="cdit:get_entities_of_kind($root, 'Component')" />

        <xsl:value-of select="cdit:test_components_unique_name($root, $components)" />
    </xsl:function>

    


</xsl:stylesheet>