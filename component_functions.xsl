<xsl:stylesheet version="2.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:xs="http://www.w3.org/2001/XMLSchema"
    xmlns:gml="http://graphml.graphdrawing.org/xmlns"
    xmlns:cdit="http://whatever"
    xmlns:o="http://whatever"
    >

    <xsl:import href="functions.xsl"/>

     <xsl:function name="cdit:generate_OutEventPortImpl">
        <xsl:param name="root"/>
        <xsl:param name="tab"/>

        <xsl:variable name="id" select="cdit:get_node_id($root)" />
        <xsl:variable name="kind" select="cdit:get_key_value($root, 'kind')" />
        <xsl:variable name="label" select="cdit:get_key_value($root, 'label')" />

        <xsl:variable name="function_name" select="cdit:get_outeventport_name($root)" />

        
        <!-- Construct Tx Object -->
        <xsl:value-of select="concat(o:t($tab), '{', o:nl())" />
        <xsl:value-of select="o:tabbed_cpp_comment(concat('generate_OutEventPortImpl: [', $id, '] = ', $label), $tab + 1)" />
        
        <!-- Generate Code for Child-->
        <xsl:for-each select="$root/gml:graph/*">
            <xsl:variable name="var_name" select="cdit:get_var_name(.)" />
            <xsl:value-of select="cdit:generate_workload_cpp(., $root, $tab + 1)" />

            <xsl:value-of select="concat(o:t($tab + 1), $function_name, '(', $var_name, ');', o:nl())" />
        </xsl:for-each>
        <xsl:value-of select="concat(o:t($tab), '}', o:nl())" />
    </xsl:function>

    <xsl:function name="cdit:generate_AggregateInstance">
        <xsl:param name="root"/>
        <xsl:param name="tab"/>

        <xsl:variable name="id" select="cdit:get_node_id($root)" />
        <xsl:variable name="kind" select="cdit:get_key_value($root, 'kind')" />
        <xsl:variable name="type" select="cdit:get_key_value($root, 'type')" />
        <xsl:variable name="label" select="cdit:get_key_value($root, 'label')" />
        <xsl:variable name="var_name" select="cdit:get_var_name($root)" />

        <xsl:value-of select="o:tabbed_cpp_comment(concat('generate_AggregateInstance: [', $id, '] = ', $label), $tab)" />

        <!-- Construct Object -->
        
        <xsl:value-of select="concat(o:t($tab), '::', $type, ' ', $var_name, ';', o:nl())" />
        
        <!-- Get the Source ID's which data link to this element -->
        <xsl:variable name="source_ids" select="cdit:get_edge_source_ids($root, 'Edge_Data', $id)" />

        <xsl:if test="count($source_ids) > 0">
            <xsl:for-each select="$source_ids">
                <xsl:variable name="source_id" select="." />
                <xsl:variable name="source" select="o:get_node_by_id($root, $source_id)" />

                <xsl:variable name="target_value" select="cdit:get_dataedge_value($source)" />

                <xsl:if test="$target_value != ''">
                    <xsl:value-of select="concat(o:t($tab), $var_name, ' = ', $target_value, ';', o:nl())" />
                </xsl:if>
            </xsl:for-each>
        </xsl:if>

        <!-- Generate Code for Child-->
        <xsl:for-each select="$root/gml:graph/*">
            <xsl:value-of select="cdit:generate_workload_cpp(., $root, $tab)" />
        </xsl:for-each>
    </xsl:function>

    <xsl:function name="cdit:generate_Branch">
        <xsl:param name="root"/>
        <xsl:param name="tab"/>

        <xsl:variable name="id" select="cdit:get_node_id($root)" />
        <xsl:variable name="kind" select="cdit:get_key_value($root, 'kind')" />
        <xsl:variable name="type" select="cdit:get_key_value($root, 'type')" />
        <xsl:variable name="label" select="cdit:get_key_value($root, 'label')" />
        <xsl:variable name="var_name" select="cdit:get_var_name($root)" />

        <xsl:value-of select="o:tabbed_cpp_comment(concat('generate_Branch: [', $id, '] = ', $label), $tab)" />

        <!-- Generate Code for Child-->
        <xsl:for-each select="$root/gml:graph/*">
            <xsl:value-of select="cdit:generate_workload_cpp(., $root, $tab)" />
        </xsl:for-each>
    </xsl:function>

    <xsl:function name="cdit:generate_Workload">
        <xsl:param name="root"/>
        <xsl:param name="tab"/>

        <xsl:variable name="id" select="cdit:get_node_id($root)" />
        <xsl:variable name="kind" select="cdit:get_key_value($root, 'kind')" />
        <xsl:variable name="type" select="cdit:get_key_value($root, 'type')" />
        <xsl:variable name="label" select="cdit:get_key_value($root, 'label')" />
        <xsl:variable name="var_name" select="cdit:get_var_name($root)" />

        <xsl:value-of select="o:tabbed_cpp_comment(concat('generate_Workload: [', $id, '] = ', $label), $tab)" />

        <!-- Generate Code for Child-->
        <xsl:for-each select="$root/gml:graph/*">
            <xsl:value-of select="cdit:generate_workload_cpp(., $root, $tab)" />
        </xsl:for-each>
    </xsl:function>


    <xsl:function name="cdit:get_process_return_var">
        <xsl:param name="root"/>
        <xsl:variable name="return_parameters" select="cdit:get_child_entities_of_kind($root, 'ReturnParameter')" />
        <!-- Can only have one -->

        <xsl:for-each select="$return_parameters">
            <xsl:if test="position() = 1">
                <xsl:value-of select="cdit:get_var_name(.)" />
            </xsl:if>
        </xsl:for-each>

    </xsl:function>

    <xsl:function name="cdit:set_cpp_attributevariable">
        <xsl:param name="root"/>
        <xsl:param name="tab"/>
        <xsl:param name="value"/>

        <xsl:variable name="id" select="cdit:get_node_id($root)" />
        <xsl:variable name="kind" select="cdit:get_key_value($root, 'kind')" />
        <xsl:variable name="label" select="cdit:get_key_value($root, 'label')" />
        <xsl:variable name="parent" select="cdit:get_parent_node($root)" />
        <xsl:variable name="parent_kind" select="cdit:get_key_value($parent, 'kind')" />

        <xsl:choose>
            <xsl:when test="$kind = 'Variable'">
                <xsl:value-of select="concat(o:t($tab), $label, '_ = ', $value, ';', o:nl())" />
            </xsl:when>
            <xsl:when test="$kind = 'AttributeImpl'">
                <!-- Setter Function -->
                <xsl:value-of select="concat(o:t($tab), 'set_', $label,'(', $value, ');', o:nl())" />
            </xsl:when>
            <xsl:otherwise>
                <!-- Setter Function -->
                <xsl:value-of select="o:tabbed_cpp_comment($kind, $tab)" />
            </xsl:otherwise>
        </xsl:choose>
    </xsl:function>

    <xsl:function name="cdit:generate_ReturnParameter">
        <xsl:param name="root"/>
        <xsl:param name="tab"/>

        <xsl:variable name="parent_node" select="cdit:get_parent_node($root)" />
        <xsl:variable name="id" select="cdit:get_node_id($root)" />
        <xsl:variable name="parent_id" select="cdit:get_node_id($parent_node)" />
        <xsl:variable name="label" select="cdit:get_key_value($root, 'label')" />

        <xsl:variable name="var_name" select="cdit:get_var_name($root)" />

        <!-- Check for Edge_Data's into this -->
        <xsl:value-of select="o:tabbed_cpp_comment(concat('generate_ReturnParameter: [', $parent_id, ' => ', $id, '] = ', $label), $tab)" />

        <!-- Get the Source ID's which data link to this element -->
        <xsl:variable name="target_ids" select="cdit:get_edge_target_ids($root, 'Edge_Data', $id)" />

        <!-- Use Data Edges first -->
        <xsl:if test="count($target_ids) > 0">
            <xsl:for-each select="$target_ids">
                <xsl:variable name="target_id" select="." />
                <xsl:variable name="target" select="o:get_node_by_id($root, $target_id)" />

                <!-- Calculate the depth to the ComponentImpl, and then back down 2 elements to get the top-most entity in the Impl-->
                <xsl:variable name="depth" select="cdit:get_depth_to_ancestor_node_of_kind($target, 'ComponentImpl') - 2" />
                <xsl:variable name="parent_id" select="cdit:get_nth_ancestor_node_id($target, $depth)" />
                <xsl:variable name="parent" select="cdit:get_node_by_id($target, $parent_id)" />

                <xsl:if test="$parent">
                    <xsl:variable name="parent_label" select="cdit:get_key_value($parent, 'label')" />
                    <xsl:value-of select="o:tabbed_cpp_comment(concat('Parent: @ ', $depth, ' = ', $parent_id, ' = ', $parent_label, o:nl()), $tab)" />
                    <xsl:value-of select="cdit:set_cpp_attributevariable($target, $tab, $var_name)" />
                </xsl:if>
            </xsl:for-each>
        </xsl:if>
    </xsl:function>

    <xsl:function name="cdit:get_depth_of_node" as="xs:integer">
        <xsl:param name="root"/>
        <xsl:value-of select="$root/count(ancestor::*)" />
    </xsl:function>
    

    <xsl:function name="cdit:get_nth_ancestor_node_id">
        <xsl:param name="root"/>
        <xsl:param name="n" as="xs:integer" />
        <xsl:choose>
            <xsl:when test="$n > 0"><xsl:value-of select="cdit:get_node_id(reverse($root/ancestor::*)[$n])" /></xsl:when>
            <xsl:when test="$n = 0"><xsl:value-of select="cdit:get_node_id($root)" /></xsl:when>
            <xsl:otherwise><xsl:value-of select="-1" /></xsl:otherwise>
        </xsl:choose>
    </xsl:function>

    <!-- Find the child of the ancestor of kind 'kind' -->
    <xsl:function name="cdit:get_depth_to_ancestor_node_of_kind" as="xs:integer" >
        <xsl:param name="root"/>
        <xsl:param name="ancestor_kind"/>

        <xsl:variable name="current_depth" select="cdit:get_depth_of_node($root)" />    

        <xsl:variable name="parent_depth">
            <!-- Step up -->
            <xsl:for-each select="reverse($root/ancestor::gml:node)">
                <xsl:variable name="id" select="cdit:get_node_id(.)" />    
                <xsl:variable name="kind" select="cdit:get_key_value(., 'kind')" />    

                <xsl:if test="$kind = $ancestor_kind">
                    <xsl:value-of select="cdit:get_depth_of_node(.)" />
                </xsl:if>
            </xsl:for-each>
        </xsl:variable>

        <xsl:choose>
            <xsl:when test="$parent_depth != '' and $parent_depth > 0">
                <xsl:value-of select="$current_depth - $parent_depth" />
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="-1" />
            </xsl:otherwise>
        </xsl:choose>
    </xsl:function>


    <xsl:function name="cdit:get_mutable_aggregate_path">
        <xsl:param name="root" />

        <xsl:variable name="id" select="cdit:get_node_id($root)" />
        <xsl:variable name="kind" select="cdit:get_key_value($root, 'kind')" />
        <xsl:variable name="label" select="cdit:get_key_value($root, 'label')" />

        <!-- Topmost parent is ComponentImpl - 2 -->
        <xsl:variable name="depth" select="cdit:get_depth_to_ancestor_node_of_kind($root, 'ComponentImpl') - 2" />

        <xsl:variable name="parent_node" select="cdit:get_parent_node($root)" />
        <xsl:variable name="parent_kind" select="cdit:get_key_value($parent_node, 'kind')" />

        <xsl:variable name="get_function">
            <xsl:choose>
                <xsl:when test="$kind='Variable'">
                    <xsl:value-of select="cdit:get_var_name($root)" />
                </xsl:when>
                <xsl:when test="$kind='AggregateInstance'">
                    <xsl:choose>
                        <xsl:when test="$parent_kind = 'InEventPortImpl'">
                            <xsl:value-of select="'m'" />
                        </xsl:when>
                        <xsl:when test="$parent_kind != 'Variable'">
                            <xsl:value-of select="o:inplace_getter_function($label)" />
                        </xsl:when>
                    </xsl:choose>
                </xsl:when>
                <xsl:when test="$kind='VectorInstance' and $parent_kind != 'Variable'">
                    <!-- Use Implace getters -->
                    <xsl:value-of select="cdit:inplace_getter_function($label)" />
                </xsl:when>
                <xsl:when test="$kind='MemberInstance' and $parent_kind != 'Variable'">
                    <!-- Use Implace getters -->
                    <xsl:value-of select="cdit:inplace_getter_function($label)" />
                </xsl:when>
            </xsl:choose>
        </xsl:variable>
        

        <xsl:if test="$depth >= 0">
            <xsl:variable name="parent_node" select="cdit:get_parent_node($root)" />
            <xsl:variable name="parent_var" select="if($depth > 0) then cdit:get_mutable_aggregate_path($parent_node) else ''" />

            <xsl:value-of select="concat($parent_var, if($depth > 0 and $parent_var != '' and $get_function != '') then '.' else '', $get_function)" />
        </xsl:if>
    </xsl:function>

    <xsl:function name="cdit:get_mutable_vector_path">
        <xsl:param name="root" />

        <xsl:variable name="id" select="cdit:get_node_id($root)" />
        <xsl:variable name="source_ids" select="cdit:get_edge_source_ids($root, 'Edge_Data', $id)" />

        <xsl:for-each select="$source_ids">
            <xsl:variable name="source" select="cdit:get_node_by_id($root, .)" />
            <xsl:variable name="parent" select="cdit:get_parent_node($source)" />
            <xsl:variable name="parent_kind" select="cdit:get_key_value($parent, 'kind')" />
            <xsl:choose>
                <xsl:when test="$parent_kind != ''">
                    <!-- A Vector directly inside a vector is referenced by its Variable name-->
                    <xsl:value-of select="cdit:get_mutable_aggregate_path($source)" />
                    <!--<xsl:value-of select="cdit:get_var_name($parent)" />-->
                </xsl:when>
                <xsl:when test="$parent_kind = 'AggregateInstance'">
                    <xsl:value-of select="cdit:get_mutable_aggregate_path($source)" />
                </xsl:when>
                <xsl:otherwise>
                    
                    <!-- Need to implement Inside Aggegates! -->
                </xsl:otherwise>
            </xsl:choose>
        </xsl:for-each>
    </xsl:function>

    <xsl:function name="cdit:generate_VectorProcess">
        <xsl:param name="root"/>
        <xsl:param name="tab"/>

        <xsl:variable name="id" select="cdit:get_node_id($root)" />
        <xsl:variable name="operation" select="cdit:get_key_value($root, 'operation')" />

        <xsl:variable name="var_name" select="cdit:get_var_name($root)" />
        <xsl:variable name="return_var_name" select="cdit:get_process_return_var($root)" />

        <!-- Insert Tab -->
        <xsl:value-of select="o:tabbed_cpp_comment(concat('generate_VectorProcess (', $operation, ') [', $id, ']'), $tab)" />

        <xsl:variable name="input_parameters" select="cdit:get_child_entities_of_kind($root, 'InputParameter')" />
        <xsl:variable name="return_parameters" select="cdit:get_child_entities_of_kind($root, 'ReturnParameter')" />

        <xsl:variable name="vector" select="$input_parameters[1]" />

        <xsl:variable name="vector_var" select="cdit:get_mutable_vector_path($vector)" />
        
        <xsl:choose>
            <xsl:when test="$operation = 'set'">
                <xsl:value-of select="concat(o:t($tab), $vector_var, '.insert(0, test);', o:nl())" />
            </xsl:when>
            <xsl:when test="$operation = 'get' or $operation = 'remove'">
            </xsl:when>
            <xsl:when test="$operation = 'length'">
            </xsl:when>
            <xsl:when test="$operation = 'clear'">
                <xsl:value-of select="concat(o:t($tab), $vector_var, '.clear();', o:nl())" />
            </xsl:when>
        </xsl:choose>


        
        <!-- Generate Code for Child InputParameters-->
        <xsl:for-each select="$input_parameters">
        </xsl:for-each>

        <!-- Generate Code for Child ReturnParameters-->
        <xsl:for-each select="$return_parameters">
            <xsl:value-of select="cdit:generate_ReturnParameter(., $tab)" />
        </xsl:for-each>
    </xsl:function>

    <xsl:function name="cdit:generate_WorkerProcess">
        <xsl:param name="root"/>
        <xsl:param name="tab"/>

        <xsl:variable name="operation" select="cdit:get_key_value($root, 'operation')" />

        <xsl:variable name="var_name" select="cdit:get_var_name($root)" />
        <xsl:variable name="return_var_name" select="cdit:get_process_return_var($root)" />

        <!-- Insert Tab -->
        <xsl:value-of select="o:t($tab)" />
        <!-- Store value -->
        <xsl:if test="$return_var_name != ''">
            <xsl:value-of select="concat('auto ', $return_var_name, ' = ')" />
        </xsl:if>
        <!-- Run function -->
        <xsl:value-of select="concat($var_name, '.', $operation, '(')" />

        <xsl:variable name="input_parameters" select="cdit:get_child_entities_of_kind($root, 'InputParameter')" />
        <xsl:variable name="return_parameters" select="cdit:get_child_entities_of_kind($root, 'ReturnParameter')" />

        <!-- Generate Code for Child InputParameters-->
        <xsl:for-each select="$input_parameters">
            <xsl:value-of select="cdit:generate_InputParameter(., $tab)" />

            <xsl:if test="position() != last()">
                <xsl:value-of select="', '" />
            </xsl:if>
        </xsl:for-each>

        <xsl:value-of select="concat(');', o:nl())" />

        <!-- Generate Code for Child ReturnParameters-->
        <xsl:for-each select="$return_parameters">
            <xsl:value-of select="cdit:generate_ReturnParameter(., $tab)" />
        </xsl:for-each>
    </xsl:function>

    <xsl:function name="cdit:generate_Process">
        <xsl:param name="root"/>
        <xsl:param name="tab"/>

        <xsl:variable name="worker" select="cdit:get_key_value($root, 'worker')" />

        <xsl:choose>
            <!-- Use Data Edges first -->
            <xsl:when test="$worker = 'VectorOperation'">
                <xsl:value-of select="cdit:generate_VectorProcess($root, $tab)" />
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="cdit:generate_WorkerProcess($root, $tab)" />
            </xsl:otherwise>
        </xsl:choose>
    </xsl:function>

    <xsl:function name="cdit:generate_InputParameter">
        <xsl:param name="root"/>
        <xsl:param name="tab"/>

        <xsl:variable name="parent_node" select="cdit:get_parent_node($root)" />
        
        <xsl:variable name="id" select="cdit:get_node_id($root)" />
        <xsl:variable name="parent_id" select="cdit:get_node_id($parent_node)" />

        <xsl:variable name="type" select="cdit:get_key_value($root, 'type')" />
        <xsl:variable name="label" select="cdit:get_key_value($root, 'label')" />
        <xsl:variable name="value" select="cdit:get_key_value($root, 'value')" />

        <!-- Check for Edge_Data's into this -->
        <!--<xsl:value-of select="o:tabbed_cpp_comment(concat('generate_InputParameter: [', $parent_id, ' => ', $id, '] = ', $label), $tab)" />-->

        <!-- Get the Source ID's which data link to this element -->
        <xsl:variable name="source_ids" select="cdit:get_edge_source_ids($root, 'Edge_Data', $id)" />

        <xsl:variable name="data_value">
            <xsl:choose>
                <!-- Use Data Edges first -->
                <xsl:when test="count($source_ids) > 0">
                    <xsl:for-each select="$source_ids">
                        <xsl:variable name="source_id" select="." />
                        <xsl:variable name="source" select="o:get_node_by_id($root, $source_id)" />

                        <xsl:value-of select="cdit:get_mutable_aggregate_path($source)" />
                        
                        <!--
                        <xsl:variable name="target_value" select="cdit:get_dataedge_value($source)" />

                        <xsl:value-of select="$target_value" />
                        -->    
                    </xsl:for-each>
                </xsl:when>
                <!-- Use Value second -->
                <xsl:when test="$value != ''">
                    <xsl:value-of select="$value" />
                </xsl:when>
            </xsl:choose>
        </xsl:variable>

        <xsl:value-of select="if($data_value != '') then $data_value else 'NULL'" />
    </xsl:function>
    

    

    <!-- TODO: DON'T NEED -->
     <xsl:function name="cdit:generate_Termination">
        <xsl:param name="root"/>
        <xsl:param name="previous_root"/>
        <xsl:param name="tab"/>

        <xsl:variable name="id" select="cdit:get_node_id($root)" />
        <xsl:variable name="prev_kind" select="cdit:get_key_value($previous_root, 'kind')" />
        
        <xsl:if test="$prev_kind = 'BranchState'">
            <xsl:value-of select="o:tabbed_cpp_comment(concat('generate_Termination: [', $id, '] ', $prev_kind), $tab)" />

            <xsl:variable name="target_ids" select="cdit:get_edge_target_ids($root, 'Edge_Workflow', $id)" />
            <xsl:message>NUMBER OF EDGES: <xsl:value-of select="count($target_ids)" /> </xsl:message>

            <xsl:for-each select="$target_ids">
                <xsl:variable name="target_id" select="." />
                <xsl:variable name="target" select="o:get_node_by_id($root, $target_id)" />
                
                <xsl:value-of select="cdit:translate_workload($target, $root, $tab)" />
            </xsl:for-each>
        </xsl:if>
    </xsl:function>

    <xsl:function name="cdit:generate_Condition">
        <xsl:param name="root"/>
        <xsl:param name="tab"/>

        <xsl:variable name="id" select="cdit:get_node_id($root)" />
        <xsl:variable name="parent" select="cdit:get_parent_node($root)" />
        <xsl:variable name="parent_kind" select="cdit:get_key_value($parent, 'kind')" />

        <xsl:variable name="label" select="cdit:get_key_value($root, 'label')" />
        <xsl:variable name="sort_order" select="cdit:get_key_value($root, 'sortOrder')" />
        <xsl:variable name="value" select="cdit:get_key_value($root, 'value')" />

        <xsl:variable name="statement">
            <xsl:choose>
                <xsl:when test="$parent_kind = 'BranchState'">
                    <xsl:value-of select="if(number($sort_order) = 0) then 'if' else 'else if'" />
                </xsl:when>
                <xsl:when test="$parent_kind = 'WhileLoop'">
                    <xsl:value-of select="'while'" />
                </xsl:when>
                <xsl:otherwise>
                    <xsl:value-of select="''" />
                </xsl:otherwise>
            </xsl:choose>
        </xsl:variable>

        <xsl:if test="$statement = ''">
            <xsl:value-of select="o:tabbed_cpp_comment(concat('Conditions parent ', $parent_kind, ' has not been implemented!'), $tab)" />
        </xsl:if>

        <!-- Get the target ids -->
        <xsl:variable name="target_ids" select="cdit:get_edge_target_ids($root, 'Edge_Workflow', $id)" />

        <xsl:if test="count($target_ids) > 0 and $statement != ''">
            <xsl:for-each select="$target_ids">
                <xsl:variable name="target_id" select="." />
                <xsl:variable name="target" select="o:get_node_by_id($root, $target_id)" />

                <xsl:value-of select="concat(o:t($tab), $statement, '(', $value, '){', o:nl())" />
                <xsl:value-of select="o:tabbed_cpp_comment(concat('generate_Condition: [', $id, '] = ', $label), $tab + 1)" />
                <!-- Call into the code generation -->
                <xsl:value-of select="cdit:translate_workload($target, $root, $tab + 1)" />
                <xsl:value-of select="concat(o:t($tab), '}', o:nl())" />
            </xsl:for-each>
        </xsl:if>
    </xsl:function>

    <xsl:function name="cdit:get_dataedge_value">
        <xsl:param name="root"/>

        <xsl:variable name="label" select="cdit:get_key_value($root, 'label')" />
        <xsl:variable name="kind" select="cdit:get_key_value($root, 'kind')" />

        <xsl:choose>
            <xsl:when test="$kind = 'AttributeImpl'">
                <!-- Use getter function -->
                <xsl:value-of select="concat(o:cpp_base_get_func($label), '()')" />
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="cdit:get_var_name($root)" />
            </xsl:otherwise>
        </xsl:choose>
    </xsl:function>



    <xsl:function name="cdit:generate_SettableElement">
        <xsl:param name="root"/>
        <xsl:param name="tab"/>

        <xsl:variable name="parent_node" select="cdit:get_parent_node($root)" />
        
        <xsl:variable name="id" select="cdit:get_node_id($root)" />
        <xsl:variable name="parent_id" select="cdit:get_node_id($parent_node)" />
        <xsl:variable name="parent_var_name" select="cdit:get_var_name($parent_node)" />

        <xsl:variable name="type" select="cdit:get_key_value($root, 'type')" />
        <xsl:variable name="label" select="cdit:get_key_value($root, 'label')" />
        <xsl:variable name="value" select="cdit:get_key_value($root, 'value')" />
        

        <!-- Check for Edge_Data's into this -->
        <xsl:value-of select="o:tabbed_cpp_comment(concat('generate_SettableElement: [', $parent_id, ' => ', $id, '] = ', $label), $tab)" />

        <!-- Construct the setter functions -->        
        <xsl:variable name="set_function" select="concat($parent_var_name, '.set_', $label)" />

        <!-- Get the Source ID's which data link to this element -->
        <xsl:variable name="source_ids" select="cdit:get_edge_source_ids($root, 'Edge_Data', $id)" />

        <xsl:choose>
            <!-- Use Data Edges first -->
            <xsl:when test="count($source_ids) > 0">
                <xsl:for-each select="$source_ids">
                    <xsl:variable name="source_id" select="." />
                    <xsl:variable name="source" select="o:get_node_by_id($root, $source_id)" />

                    <xsl:variable name="target_value" select="cdit:get_dataedge_value($source)" />

                    <xsl:if test="$target_value != ''">
                        <xsl:value-of select="concat(o:t($tab), $set_function, '(', $target_value, ');', o:nl())" />
                    </xsl:if>
                </xsl:for-each>
            </xsl:when>
            <!-- Use Value second -->
            <xsl:when test="$value != ''">
                <xsl:value-of select="concat(o:t($tab), $set_function, '(', $value, ');', o:nl())" />
            </xsl:when>
        </xsl:choose>
    </xsl:function>

    <xsl:function name="cdit:get_parent_node">
        <xsl:param name="root"/>
        <xsl:sequence select="$root/../.." />
    </xsl:function>

    <xsl:function name="cdit:is_var_a_class_member" as="xs:boolean">
        <xsl:param name="root"/>
        <xsl:variable name="kind" select="cdit:get_key_value($root, 'kind')" />

        <xsl:choose>
            <xsl:when test="$kind = 'Variable'">
                <xsl:value-of select="true()" />
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="false()" />
            </xsl:otherwise>
        </xsl:choose>
    </xsl:function>

    <xsl:function name="cdit:get_var_name">
        <xsl:param name="root"/>

        <xsl:variable name="id" select="cdit:get_node_id($root)" />
        <xsl:variable name="kind" select="cdit:get_key_value($root, 'kind')" />
        <xsl:variable name="label" select="cdit:get_key_value($root, 'label')" />
        <xsl:variable name="parent" select="cdit:get_parent_node($root)" />
        <xsl:variable name="parent_kind" select="cdit:get_key_value($parent, 'kind')" />

        <xsl:variable name="class_member" select="cdit:is_var_a_class_member($root)" />

        <xsl:choose>
            <xsl:when test="$class_member = true()">
                <xsl:value-of select="concat($label, '_')" />
            </xsl:when>
            <xsl:when test="$kind = 'Process'">
                <!-- Get the workerID -->
                <xsl:variable name="worker_id" select="cdit:get_key_value($root, 'workerID')" />

                <xsl:value-of select="concat($worker_id, '_')" />
            </xsl:when>
            <!--
            <xsl:when test="$kind = 'InEventPortImpl'">
                <xsl:value-of select="'m'" />
            </xsl:when>
            <xsl:when test="($kind = 'AggregateInstance' or $kind = 'MemberInstance') and ($parent_kind != 'InEventPortImpl' and $parent_kind !='OutEventPortImpl')">

                <xsl:value-of select="concat(cdit:get_var_name($parent), '.', $label)" />
            </xsl:when>
            -->
            <xsl:otherwise>
                <xsl:value-of select="lower-case(concat($kind, '_', $id))" />
            </xsl:otherwise>
        </xsl:choose>
    </xsl:function>


    <xsl:function name="cdit:generate_workload_cpp">
        <xsl:param name="root"/>
        <xsl:param name="previous_root"/>
        <xsl:param name="tab"/>
        

        <xsl:variable name="id" select="cdit:get_node_id($root)" />
        <xsl:variable name="kind" select="cdit:get_key_value($root, 'kind')" />
        <xsl:variable name="label" select="cdit:get_key_value($root, 'label')" />

        <!--<xsl:value-of select="o:tabbed_cpp_comment(concat('generate_workload_cpp: ', $kind,' [', $id, '] = ', $label), $tab)" />-->
        <xsl:choose>
            <xsl:when test="$kind = 'OutEventPortImpl'">
                <xsl:value-of select="cdit:generate_OutEventPortImpl($root, $tab)" />
            </xsl:when>
            <xsl:when test="$kind = 'AggregateInstance'">
                <xsl:value-of select="cdit:generate_AggregateInstance($root, $tab)" />
            </xsl:when>
            <xsl:when test="$kind = 'MemberInstance'">
                <xsl:value-of select="cdit:generate_SettableElement($root, $tab)" />
            </xsl:when>
            <xsl:when test="$kind = 'VectorInstance'">
                <xsl:value-of select="cdit:generate_SettableElement($root, $tab)" />
            </xsl:when>
            <xsl:when test="$kind = 'BranchState'">
                <xsl:value-of select="cdit:generate_Branch($root, $tab)" />
            </xsl:when>
            <xsl:when test="$kind = 'WhileLoop'">
                <xsl:value-of select="cdit:generate_Branch($root, $tab)" />
            </xsl:when>
            <xsl:when test="$kind = 'Condition'">
                <xsl:value-of select="cdit:generate_Condition($root, $tab)" />
            </xsl:when>
            <xsl:when test="$kind = 'Workload'">
                <xsl:value-of select="cdit:generate_Workload($root, $tab)" />
            </xsl:when>
            <xsl:when test="$kind = 'Process'">
                <xsl:value-of select="cdit:generate_Process($root, $tab)" />
            </xsl:when>
            <xsl:when test="$kind = 'PeriodicEvent'">
                <!-- Do Nothing -->
            </xsl:when>
            <xsl:when test="$kind = 'InEventPortImpl'">
                <!-- Do Nothing -->
            </xsl:when>
            <xsl:when test="$kind = 'Termination'">
                <!-- Do Nothing -->
            </xsl:when>

            
            <xsl:otherwise>
                <xsl:value-of select="o:tabbed_cpp_comment(concat('generate_workload_cpp(', $kind, ') Not Implemented!'), $tab)" />
            </xsl:otherwise>
        </xsl:choose>
    </xsl:function>



    <xsl:function name="cdit:translate_workload">
        <xsl:param name="root"/>
        <xsl:param name="previous_root"/>
        <xsl:param name="tab"/>

        <xsl:variable name="id" select="cdit:get_node_id($root)" />
        <xsl:value-of select="cdit:generate_workload_cpp($root, $previous_root, $tab)" />

        <!-- Recurse -->
        <xsl:variable name="target_ids" select="cdit:get_edge_target_ids($root, 'Edge_Workflow', $id)" />
        <xsl:for-each select="$target_ids">
            <xsl:variable name="target_id" select="." />
            <xsl:variable name="target" select="o:get_node_by_id($root, $target_id)" />

            <xsl:value-of select="cdit:translate_workload($target, $root, $tab)" />
        </xsl:for-each>
    </xsl:function>

    <xsl:function name="cdit:get_component_impl_h">
        <xsl:param name="component_root"/>

        
        <xsl:variable name="component_def_root" select="cdit:get_components_definition($component_root)" />
        <xsl:variable name="component_id" select="cdit:get_node_id($component_root)" />
        <xsl:variable name="component_label" select="cdit:get_key_value($component_root, 'label')" />
        <xsl:variable name="component_label_cc" select="o:camel_case($component_label)" />
        <xsl:variable name="component_label_lc" select="lower-case($component_label)" />
        <xsl:variable name="component_label_uc" select="upper-case($component_label)" />

        <!-- Get the required datatypes used by this ComponentImpl-->
        <xsl:variable name="required_datatypes" select="cdit:get_component_impls_required_datatypes($component_root)" />
        <xsl:variable name="rel_path" select="'../../'" />
        <xsl:variable name="class_name" select="concat($component_label_cc, 'Impl')" />
        <xsl:variable name="interface_name" select="concat($component_label_cc, 'Int')" />

        <xsl:variable name="define_guard" select="concat('COMPONENTS_', $component_label_uc, '_', $component_label_uc, 'IMPL_H')" />

        <xsl:variable name="periodicevents" as="element()*" select="cdit:get_child_entities_of_kind($component_root, 'PeriodicEvent')" />
        <xsl:variable name="ineventports" as="element()*" select="cdit:get_child_entities_of_kind($component_def_root, 'InEventPort')" />
        <xsl:variable name="processes" as="element()*" select="cdit:get_descendant_entities_of_kind($component_root, 'Process')" />
        
        <!-- Define Guard -->
        <xsl:value-of select="o:define_guard($define_guard)" />

        <!-- Include Base Types -->
        <xsl:value-of select="o:cpp_comment('Include Statements')" />
        <xsl:value-of select="o:local_include(concat(lower-case($interface_name), '.h'))" />
        <xsl:value-of select="o:nl()" />

        <!-- Include datatypes -->
        <xsl:if test="count($processes) > 0">
            <xsl:value-of select="o:cpp_comment('Worker Include Statements')" />

            <!-- Include the workers-->
            <xsl:for-each select="$processes">
                <xsl:variable name="id" select="cdit:get_node_id(.)" />
                <xsl:variable name="label" select="cdit:get_key_value(., 'label')" />
                <xsl:variable name="folder" select="cdit:get_key_value(., 'folder')" />
                <xsl:variable name="file" select="cdit:get_key_value(., 'file')" />
                <xsl:variable name="worker_include" select="concat($folder, '/', lower-case($file), '.h')" />

                <xsl:value-of select="o:local_include($worker_include)" />
                <xsl:value-of select="o:nl()" />
            </xsl:for-each>
        </xsl:if>
        
        
        <xsl:variable name="variables" as="element()*" select="cdit:get_child_entities_of_kind($component_root, 'Variable')" />


        <xsl:value-of select="o:tabbed_cpp_comment(concat('ComponentImpl ', o:square_wrap($component_id), ': ', $class_name), 0)" />

        <!-- Define Class -->
        <xsl:value-of select="concat('class ', $class_name, ' : public ', $interface_name, '{', o:nl())" />
        <xsl:value-of select="concat(o:t(1), 'public:', o:nl())" />
        
        <!-- Declare Constructor-->
        <xsl:value-of select="concat(o:t(2), $class_name, '(std::string name);', o:nl())" />
        
        <!-- PeriodicEvents are declared as pure virtual, and are defined in the Impl-->
        <xsl:for-each select="$periodicevents">
            <xsl:variable name="id" select="cdit:get_node_id(.)" />
            <xsl:variable name="label" select="cdit:get_key_value(., 'label')" />

            <xsl:value-of select="o:nl()" />
            <xsl:value-of select="o:tabbed_cpp_comment(concat('PeriodicEvent ', o:square_wrap($id), ': ', $label), 2)" />
            <xsl:value-of select="concat(o:t(2), 'void PE_', $label, '();', o:nl())" />
        </xsl:for-each>

        <!-- InEventPorts are declared as pure virtual, and are defined in the Impl-->
        <xsl:for-each select="$ineventports">
            <xsl:variable name="id" select="cdit:get_node_id(.)" />
            <xsl:variable name="label" select="cdit:get_key_value(., 'label')" />
            <xsl:variable name="type" select="cdit:get_key_value(., 'type')" />
            <xsl:variable name="namespace" select="'::'" />
            <xsl:variable name="cpp_type" select="concat($namespace, $type)" />
            <xsl:value-of select="o:nl()" />
            <xsl:value-of select="o:tabbed_cpp_comment(concat('InEventPort ', o:square_wrap($id), ': ', $label), 2)" />
            <xsl:value-of select="concat(o:t(2), 'void In_', $label, '(', $cpp_type, ' m);', o:nl())" />
        </xsl:for-each>

        <!-- InEventPorts are declared as pure virtual, and are defined in the Impl-->
        <xsl:for-each select="$variables">
            <xsl:variable name="id" select="cdit:get_node_id(.)" />
            <xsl:variable name="label" select="cdit:get_key_value(., 'label')" />
            <xsl:variable name="type" select="cdit:get_key_value(., 'type')" />
            <xsl:variable name="cpp_type" select="cdit:get_cpp_type($type)" />
            <xsl:variable name="var_name" select="cdit:get_var_name(.)" />

            <xsl:value-of select="o:nl()" />
            <xsl:value-of select="o:tabbed_cpp_comment(concat('Variable ', o:square_wrap($id), ': ', $label), 2)" />
            <xsl:value-of select="concat(o:t(2), $cpp_type, ' ', $var_name, ';', o:nl())" />
        </xsl:for-each>

        <!-- Processes are private variables, and are declared in the Impl-->
        <xsl:for-each select="$processes">
            <xsl:variable name="id" select="cdit:get_node_id(.)" />
            <xsl:variable name="label" select="cdit:get_key_value(., 'label')" />
            <xsl:variable name="worker" select="cdit:get_key_value(., 'worker')" />
            <xsl:variable name="var_name" select="cdit:get_var_name(.)" />
            <xsl:value-of select="o:nl()" />


            <xsl:value-of select="o:tabbed_cpp_comment(concat('Process ', o:square_wrap($id), ': ', $label), 2)" />
            <xsl:value-of select="concat(o:t(2), $worker, ' ', $var_name, ';', o:nl())" />
        </xsl:for-each>

        
        <xsl:value-of select="concat('};', o:nl())" />
        <xsl:value-of select="o:define_guard_end($define_guard)" />
    </xsl:function>


    <xsl:function name="cdit:get_component_impl_cpp">
        <xsl:param name="component_root"/>

        <xsl:variable name="component_id" select="cdit:get_node_id($component_root)" />
        <xsl:variable name="component_label" select="cdit:get_key_value($component_root, 'label')" />
        <xsl:variable name="component_label_cc" select="o:camel_case($component_label)" />
        <xsl:variable name="component_label_lc" select="lower-case($component_label)" />
        <xsl:variable name="component_label_uc" select="upper-case($component_label)" />

        <!-- Get the required datatypes used by this ComponentImpl-->
        <xsl:variable name="required_datatypes" select="cdit:get_component_impls_required_datatypes($component_root)" />
        <xsl:variable name="rel_path" select="'../../'" />
        <xsl:variable name="class_name" select="concat($component_label_cc, 'Impl')" />
        <xsl:variable name="interface_name" select="concat($component_label_cc, 'Int')" />
        


        <!-- Include Base Types -->
        <xsl:value-of select="o:local_include(concat(lower-case($class_name), '.h'))" />
        <xsl:value-of select="o:lib_include('iostream')" />

        <xsl:value-of select="o:nl()" />

        <xsl:variable name="periodicevents" as="element()*" select="cdit:get_child_entities_of_kind($component_root, 'PeriodicEvent')" />
        <xsl:variable name="ineventports" as="element()*" select="cdit:get_child_entities_of_kind($component_root, 'InEventPortImpl')" />

        <xsl:value-of select="o:tabbed_cpp_comment(concat('ComponentImpl ', o:square_wrap($component_id), ': ', $class_name), 0)" />

        <!-- Define Constructor -->
        <xsl:value-of select="concat($class_name, '::', $class_name,'(std::string name): ', $interface_name, '(name){', o:nl())" />
        <xsl:value-of select="concat('};', o:nl())" />

        <!-- PeriodicEvents are declared as pure virtual, and are defined in the Impl-->
        <xsl:for-each select="$periodicevents">
            <xsl:variable name="id" select="cdit:get_node_id(.)" />
            <xsl:variable name="label" select="cdit:get_key_value(., 'label')" />
            <xsl:variable name="function_name" select="cdit:get_periodicevent_name(.)" />

            <xsl:value-of select="o:nl()" />
            <xsl:value-of select="o:tabbed_cpp_comment(concat('PeriodicEvent ', o:square_wrap($id), ': ', $label), 0)" />
            <xsl:value-of select="concat('void ', $class_name, '::', $function_name, '(){', o:nl())" />
            <xsl:value-of select="cdit:translate_workload(.,., 1)" />
            <xsl:value-of select="concat('};', o:nl())" />
        </xsl:for-each>

        <!-- InEventPorts are declared as pure virtual, and are defined in the Impl-->
        <xsl:for-each select="$ineventports">
            <xsl:variable name="id" select="cdit:get_node_id(.)" />
            <xsl:variable name="label" select="cdit:get_key_value(., 'label')" />
            <xsl:variable name="type" select="cdit:get_key_value(., 'type')" />
            <xsl:variable name="namespace" select="'::'" />
            <xsl:variable name="cpp_type" select="concat($namespace, $type)" />
            <xsl:variable name="function_name" select="cdit:get_ineventport_name(.)" />

            <xsl:value-of select="o:nl()" />
            <xsl:value-of select="o:tabbed_cpp_comment(concat('InEventPort ', o:square_wrap($id), ': ', $label), 0)" />
            <xsl:value-of select="concat('void ', $class_name, '::', $function_name, '(', $cpp_type, ' m){', o:nl())" />
            <xsl:value-of select="cdit:translate_workload(.,., 1)" />
            <xsl:value-of select="concat('};', o:nl())" />
        </xsl:for-each>
    </xsl:function>
    
    <xsl:function name="cdit:get_outeventport_name">
        <xsl:param name="root"/>
        <xsl:variable name="label" select="cdit:get_key_value($root, 'label')" />
        <xsl:value-of select="concat('Out_', $label)" />
    </xsl:function>

    <xsl:function name="cdit:get_ineventport_name">
        <xsl:param name="root"/>
        <xsl:variable name="label" select="cdit:get_key_value($root, 'label')" />
        <xsl:value-of select="concat('In_', $label)" />
    </xsl:function>

    <xsl:function name="cdit:get_periodicevent_name">
        <xsl:param name="root"/>
        <xsl:variable name="label" select="cdit:get_key_value($root, 'label')" />
        <xsl:value-of select="concat('PE_', $label)" />
    </xsl:function>

    <xsl:function name="cdit:get_component_int_h">
        <xsl:param name="component_root"/>

        <xsl:variable name="component_def_root" select="cdit:get_components_definition($component_root)" />
        <xsl:variable name="component_id" select="cdit:get_node_id($component_root)" />
        <xsl:variable name="component_label" select="cdit:get_key_value($component_root, 'label')" />
        <xsl:variable name="component_label_cc" select="o:camel_case($component_label)" />
        <xsl:variable name="component_label_lc" select="lower-case($component_label)" />
        <xsl:variable name="component_label_uc" select="upper-case($component_label)" />

        <!-- Get the required datatypes used by this ComponentImpl-->
        <xsl:variable name="required_datatypes" select="cdit:get_component_impls_required_datatypes($component_root)" />
        <xsl:variable name="rel_path" select="'../../'" />
        <xsl:variable name="class_name" select="concat($component_label_cc, 'Int')" />

        <xsl:variable name="define_guard" select="concat('COMPONENTS_', $component_label_uc, '_', $component_label_uc, 'INT_H')" />

        <!-- Define Guard -->
        <xsl:value-of select="o:define_guard($define_guard)" />

        <!-- Include Base Types -->
        <xsl:value-of select="o:cpp_comment('Include Statements')" />
        <xsl:value-of select="o:lib_include('core/component.h')" />
        <xsl:value-of select="o:nl()" />

        <xsl:variable name="attributes" as="element()*" select="cdit:get_child_entities_of_kind($component_def_root, 'Attribute')" />
        <xsl:variable name="periodicevents" as="element()*" select="cdit:get_child_entities_of_kind($component_root, 'PeriodicEvent')" />
        <xsl:variable name="outeventports" as="element()*" select="cdit:get_child_entities_of_kind($component_def_root, 'OutEventPort')" />
        <xsl:variable name="ineventports" as="element()*" select="cdit:get_child_entities_of_kind($component_def_root, 'InEventPort')" />

        <!-- Include datatypes -->
        <xsl:if test="count($required_datatypes) > 0">
            <xsl:value-of select="o:cpp_comment('Include the datatypes used by this Component')" />
            <xsl:for-each-group select="$required_datatypes" group-by=".">
                <xsl:variable name="datatype" select="lower-case(.)" />
                <xsl:value-of select="o:local_include(concat($rel_path, 'datatypes/', $datatype, '/', $datatype, '.h'))" />
            </xsl:for-each-group>
            <xsl:value-of select="o:nl()" />
        </xsl:if>

        <xsl:value-of select="o:tabbed_cpp_comment(concat('ComponentImpl ', o:square_wrap($component_id), ': ', $class_name), 0)" />

        <!-- Define Class -->
        <xsl:value-of select="concat('class ', $class_name, ' : public Component{', o:nl())" />
        <xsl:value-of select="concat(o:t(1), 'public:', o:nl())" />
        
        <!-- Declare Constructor-->
        <xsl:value-of select="concat(o:t(2), $class_name, '(std::string name);', o:nl())" />
        
        <!-- PeriodicEvents are declared as pure virtual, and are defined in the Impl-->
        <xsl:for-each select="$periodicevents">
            <xsl:variable name="id" select="cdit:get_node_id(.)" />
            <xsl:variable name="label" select="cdit:get_key_value(., 'label')" />
            <xsl:variable name="function_name" select="cdit:get_periodicevent_name(.)" />

            <xsl:value-of select="o:nl()" />
            <xsl:value-of select="o:tabbed_cpp_comment(concat('PeriodicEvent ', o:square_wrap($id), ': ', $label), 2)" />
            <xsl:value-of select="concat(o:t(2), 'virtual void ', $function_name, '() = 0;', o:nl())" />
        </xsl:for-each>

        <!-- InEventPorts are declared as pure virtual, and are defined in the Impl-->
        <xsl:for-each select="$ineventports">
            <xsl:variable name="id" select="cdit:get_node_id(.)" />
            <xsl:variable name="label" select="cdit:get_key_value(., 'label')" />
            <xsl:variable name="type" select="cdit:get_key_value(., 'type')" />
            <xsl:variable name="namespace" select="'::'" />
            <xsl:variable name="cpp_type" select="concat($namespace, $type)" />
            <xsl:variable name="function_name" select="cdit:get_ineventport_name(.)" />

            <xsl:value-of select="o:nl()" />
            <xsl:value-of select="o:tabbed_cpp_comment(concat('InEventPort ', o:square_wrap($id), ': ', $label), 2)" />
            <xsl:value-of select="concat(o:t(2), 'virtual void ', $function_name, '(', $cpp_type, ' m) = 0;', o:nl())" />
        </xsl:for-each>

        <!-- OutEventPorts are declared and defined in the interface -->
        <xsl:for-each select="$outeventports">
            <xsl:variable name="id" select="cdit:get_node_id(.)" />
            <xsl:variable name="label" select="cdit:get_key_value(., 'label')" />
            <xsl:variable name="type" select="cdit:get_key_value(., 'type')" />
            <xsl:variable name="namespace" select="'::'" />
            <xsl:variable name="cpp_type" select="concat($namespace, $type)" />
            <xsl:variable name="function_name" select="cdit:get_outeventport_name(.)" />



            <xsl:value-of select="o:nl()" />
            <xsl:value-of select="o:tabbed_cpp_comment(concat('OutEventPort ', o:square_wrap($id), ': ', $label), 2)" />
            <xsl:value-of select="concat(o:t(2), 'void ', $function_name, '(', $cpp_type, ' m);', o:nl())" />
        </xsl:for-each>

        <!-- Process Attributes -->
        <xsl:for-each select="$attributes">
            <xsl:variable name="id" select="cdit:get_node_id(.)" />
            <xsl:variable name="label" select="cdit:get_key_value(., 'label')" />
            <xsl:variable name="type" select="cdit:get_key_value(., 'type')" />
            <xsl:variable name="cpp_type" select="cdit:get_cpp_type($type)" />
            

            <xsl:value-of select="o:nl()" />
            <xsl:value-of select="o:tabbed_cpp_comment(concat('Attribute ', o:square_wrap($id), ': ', $label), 2)" />
            <xsl:value-of select="o:declare_attribute_functions($label, $cpp_type)" />
        </xsl:for-each>

        <xsl:value-of select="concat('};', o:nl())" />
        <xsl:value-of select="o:define_guard_end($define_guard)" />
    </xsl:function>	


    <xsl:function name="cdit:get_component_int_cpp">
        <xsl:param name="component_root"/>

        <xsl:variable name="component_def_root" select="cdit:get_components_definition($component_root)" />
        <xsl:variable name="component_id" select="cdit:get_node_id($component_root)" />
        <xsl:variable name="component_label" select="cdit:get_key_value($component_root, 'label')" />
        <xsl:variable name="component_label_cc" select="o:camel_case($component_label)" />
        <xsl:variable name="component_label_lc" select="lower-case($component_label)" />
        <xsl:variable name="component_label_uc" select="upper-case($component_label)" />

        <!-- Get the required datatypes used by this ComponentImpl-->
        <xsl:variable name="required_datatypes" select="cdit:get_component_impls_required_datatypes($component_root)" />
        <xsl:variable name="rel_path" select="'../../'" />
        <xsl:variable name="class_name" select="concat($component_label_cc, 'Int')" />
        <xsl:variable name="header_path" select="concat(lower-case($class_name), '.h')" />
        <xsl:variable name="namespace" select="concat($class_name, '::')" />

        <!-- Include Base Types -->
        <xsl:value-of select="o:local_include($header_path)" />

        <xsl:variable name="attributes" as="element()*" select="cdit:get_child_entities_of_kind($component_def_root, 'Attribute')" />
        <xsl:variable name="periodicevents" as="element()*" select="cdit:get_child_entities_of_kind($component_root, 'PeriodicEvent')" />
        <xsl:variable name="outeventports" as="element()*" select="cdit:get_child_entities_of_kind($component_def_root, 'OutEventPort')" />
        <xsl:variable name="ineventports" as="element()*" select="cdit:get_child_entities_of_kind($component_def_root, 'InEventPort')" />

        <xsl:if test="count($outeventports) > 0">
            <xsl:value-of select="o:lib_include('core/eventports/outeventport.hpp')" />
        </xsl:if>

        <xsl:if test="count($ineventports) > 0">
            <xsl:value-of select="o:lib_include('core/eventports/ineventport.hpp')" />
        </xsl:if>
        
        <xsl:value-of select="o:nl()" />
        <!-- Define Constructor-->
        <xsl:value-of select="concat($namespace, $class_name, '(std::string name): Component(name){', o:nl())" />

        <!-- Construct Attributes -->
        <xsl:for-each select="$attributes">
            <xsl:variable name="id" select="cdit:get_node_id(.)" />
            <xsl:variable name="label" select="cdit:get_key_value(., 'label')" />
            <xsl:variable name="type" select="cdit:get_key_value(., 'type')" />
            <xsl:variable name="attr_type" select="cdit:get_attr_enum_type($type)" />

            <xsl:value-of select="o:nl()" />
            <xsl:value-of select="o:tabbed_cpp_comment(concat('Attribute ', o:square_wrap($id), ': ', $label), 1)" />
            <xsl:value-of select="concat(o:t(1), 'AddAttribute(new Attribute(',$attr_type, ', ', o:dblquote_wrap($label),'));' ,o:nl())" />
        </xsl:for-each>

        <!-- Construct Attributes -->
        <xsl:for-each select="$periodicevents">
            <xsl:variable name="id" select="cdit:get_node_id(.)" />
            <xsl:variable name="label" select="cdit:get_key_value(., 'label')" />
            <xsl:variable name="type" select="cdit:get_key_value(., 'type')" />

            <xsl:value-of select="o:nl()" />
            <xsl:value-of select="o:tabbed_cpp_comment(concat('PeriodicEvent ', o:square_wrap($id), ': ', $label), 1)" />
            <xsl:value-of select="concat(o:t(1), 'AddCallback(', o:dblquote_wrap($label), ', [this](BaseMessage* m) {PE_', $label, '();});' ,o:nl())" />
        </xsl:for-each>

        <!-- Construct InEventPorts -->
        <xsl:for-each select="$ineventports">
            <xsl:variable name="id" select="cdit:get_node_id(.)" />
            <xsl:variable name="label" select="cdit:get_key_value(., 'label')" />
            <xsl:variable name="type" select="cdit:get_key_value(., 'type')" />
            <xsl:variable name="cpp_type" select="concat('::', $type)" />

            <xsl:value-of select="o:nl()" />
            <xsl:value-of select="o:tabbed_cpp_comment(concat('InEventPort ', o:square_wrap($id), ': ', $label), 1)" />
            <xsl:value-of select="concat(o:t(1), 'AddCallback(', o:dblquote_wrap($label), ', [this](BaseMessage* m) {auto t = ', o:bracket_wrap(concat($cpp_type, '*')), 'm; In_', $label,'(*t);});' ,o:nl())" />
        </xsl:for-each>
        <xsl:value-of select="concat('};', o:nl())" />

        <!-- OutEventPorts -->
        <xsl:for-each select="$outeventports">
            <xsl:variable name="id" select="cdit:get_node_id(.)" />
            <xsl:variable name="label" select="cdit:get_key_value(., 'label')" />
            <xsl:variable name="type" select="cdit:get_key_value(., 'type')" />
            
            <xsl:variable name="cpp_type" select="concat('::', $type)" />

            <xsl:value-of select="o:nl()" />
            <xsl:value-of select="o:tabbed_cpp_comment(concat('OutEventPort ', o:square_wrap($id), ': ', $label), 0)" />
            
            <!--Define Function-->
            <xsl:value-of select="concat('void ', $namespace, 'Out_', $label, '(', $cpp_type, ' m){', o:nl())" />
            
            <xsl:value-of select="concat(o:t(1), 'auto p = GetEventPort(', o:dblquote_wrap($label), ');', o:nl())" />
            <xsl:value-of select="concat(o:t(1), 'if(p){', o:nl())" />
            <xsl:value-of select="concat(o:t(2), 'auto typed_p = (::OutEventPort', o:angle_wrap($cpp_type), ' *) p;', o:nl())" />
            <xsl:value-of select="concat(o:t(2), 'if(typed_p){', o:nl())" />
            <xsl:value-of select="concat(o:t(3), 'typed_p', o:fp(), 'tx(', o:and(), 'm);', o:nl())" />
            <xsl:value-of select="concat(o:t(2), '}', o:nl())" />
            <xsl:value-of select="concat(o:t(1), '}', o:nl())" />
            
            <xsl:value-of select="concat('};', o:nl())" />
        </xsl:for-each>


        <!-- Getters/Setters Attributes -->
        <xsl:for-each select="$attributes">
            <xsl:variable name="id" select="cdit:get_node_id(.)" />
            <xsl:variable name="label" select="cdit:get_key_value(., 'label')" />
            <xsl:variable name="type" select="cdit:get_key_value(., 'type')" />
            

            <xsl:value-of select="o:nl()" />
            <xsl:value-of select="o:tabbed_cpp_comment(concat('Attribute ', o:square_wrap($id), ': ', $label), 1)" />
            <xsl:value-of select="o:define_attribute_functions($label, $type, $class_name)" />
        </xsl:for-each>
    </xsl:function>	

     <xsl:function name="cdit:get_components_definition">
        <xsl:param name="component_impl_root" />
        <xsl:variable name="id" select="cdit:get_node_id($component_impl_root)" />
        <xsl:variable name="target_ids" select="cdit:get_edge_target_ids($component_impl_root, 'Edge_Definition', $id)" />

        <xsl:for-each select="$target_ids">
            <xsl:variable name="target_id" select="." />
            <xsl:sequence select="cdit:get_node_by_id($component_impl_root, $target_id)" />
        </xsl:for-each>
    </xsl:function>

    <xsl:function name="cdit:get_component_impls_required_datatypes">
        <xsl:param name="component_impl_root" />

        <xsl:variable name="component_def" select="cdit:get_components_definition($component_impl_root)" />

        <xsl:variable name="def_aggregates" as="element()*" select="cdit:get_entities_of_kind($component_def, 'AggregateInstance')" />
        <xsl:variable name="impl_instances" as="element()*" select="cdit:get_entities_of_kind($component_impl_root, 'AggregateInstance')" />

        <xsl:for-each select="$def_aggregates">
            <xsl:variable name="type" select="lower-case(cdit:get_key_value(., 'type'))" />
            <xsl:value-of select="$type" />
        </xsl:for-each>
        <xsl:for-each select="$impl_instances">
            <xsl:variable name="type" select="lower-case(cdit:get_key_value(., 'type'))" />
            <xsl:value-of select="$type" />
        </xsl:for-each>
    </xsl:function>

    <xsl:function name="cdit:get_libcomponent_export_cpp">
        <xsl:param name="component_impl_root" />

        <xsl:variable name="label" select ="cdit:get_key_value($component_impl_root, 'label')" />
        <xsl:variable name="class_name" select ="concat(o:camel_case($label), 'Impl')" />
        <xsl:variable name="header" select ="concat(lower-case($class_name), '.h')" />

        <!-- Include core headers -->            
        <xsl:value-of select="o:lib_include('core/libcomponentexports.h')" />
        <xsl:value-of select="o:lib_include('core/component.h')" />
        <xsl:value-of select="o:nl()" />

        <xsl:value-of select="o:local_include($header)" />
        <xsl:value-of select="o:nl()" />

        <xsl:value-of select="concat('Component* ConstructComponent(std::string name){', o:nl())" />
        <xsl:value-of select="concat(o:t(1), 'return new ', $class_name, o:bracket_wrap('name'),';', o:nl())" />
        <xsl:value-of select="concat('};', o:nl())" />
    </xsl:function>

    <xsl:function name="cdit:get_component_cmake">
        <xsl:param name="component_impl_root" />

        

        <xsl:variable name="label" select ="cdit:get_key_value($component_impl_root, 'label')" />
        <xsl:variable name="proj_name" select ="lower-case(concat('components_', $label))" />
        <xsl:variable name="class_name" select ="o:camel_case($label)" />
        <xsl:variable name="class_name_lc" select ="lower-case($class_name)" />

        <xsl:variable name="required_datatypes" select="cdit:get_component_impls_required_datatypes($component_impl_root)" />


        <xsl:value-of select="o:cmake_set_re_path()" />

        <xsl:variable name="PROJ_NAME" select="o:cmake_var_wrap('PROJ_NAME')" />
        <xsl:value-of select="o:cmake_set_proj_name($proj_name)" />
        <xsl:value-of select="o:nl()" />
        <xsl:value-of select="o:cmake_find_re_core_library()" />

        <!-- Set Source files -->
        <xsl:value-of select="concat('set(SOURCE', o:nl())" />
        <xsl:value-of select="concat(o:t(1), '${CMAKE_CURRENT_SOURCE_DIR}/', $class_name_lc, 'int.cpp', o:nl())" />
        <xsl:value-of select="concat(o:t(1), '${CMAKE_CURRENT_SOURCE_DIR}/', $class_name_lc, 'impl.cpp', o:nl())" />
        <xsl:value-of select="concat(o:t(1), ')', o:nl())" />
        <xsl:value-of select="o:nl()" />

        <!-- Set Headers files -->
        <xsl:value-of select="concat('set(HEADERS', o:nl())" />
        <xsl:value-of select="concat(o:t(1), '${CMAKE_CURRENT_SOURCE_DIR}/', $class_name_lc, 'int.h', o:nl())" />
        <xsl:value-of select="concat(o:t(1), '${CMAKE_CURRENT_SOURCE_DIR}/', $class_name_lc, 'impl.h', o:nl())" />
        <xsl:value-of select="concat(o:t(1), ')', o:nl())" />
        <xsl:value-of select="o:nl()" />

        <xsl:value-of select="o:cmake_include_re_core()" />

        <!-- add library/link -->
        <xsl:value-of select="concat('add_library(${PROJ_NAME} SHARED ${SOURCE} ${HEADERS})', o:nl())" />
        <xsl:value-of select="o:nl()" />
        <xsl:value-of select="o:cmake_link_re_core($PROJ_NAME)" />
        <xsl:value-of select="o:nl()" />
        
        <xsl:for-each-group select="$required_datatypes" group-by=".">
            <xsl:variable name="datatype" select="concat('datatype_', lower-case(.))" />
            <xsl:value-of select="o:cmake_target_link_libraries($PROJ_NAME, $datatype)" />
        </xsl:for-each-group>
    </xsl:function>


    

    


    

    
</xsl:stylesheet>