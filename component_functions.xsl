<!-- Functions for Generating Components -->
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
        Gets the Component Interface Header
    -->
    <xsl:function name="cdit:get_component_int_h">
        <xsl:param name="component" as="element()" />
        
        <!-- Get their required aggregates used to express Component -->
        <xsl:variable name="required_aggregates" select="cdit:get_required_aggregates($component)" />
        
        
        <xsl:variable name="namespaces" select="o:trim_list(graphml:get_namespace($component))" />
        <xsl:variable name="label" select="graphml:get_label($component)" />
        <xsl:variable name="class_name" select="concat(o:title_case($label), 'Int')" />
        <xsl:variable name="tab" select="count($namespaces)" />

        <xsl:variable name="qualified_type" select="cpp:combine_namespaces(($namespaces, $label))" />

        <xsl:variable name="define_guard_name" select="upper-case(o:join_list(('COMPONENT', $namespaces, $label, 'int'), '_'))" />

        <!-- Define Guard -->
        <xsl:value-of select="cpp:define_guard_start($define_guard_name)" />

        <!-- Library Includes-->
        <xsl:value-of select="cpp:include_library_header(o:join_paths(('core', 'component.h')))" />
        <xsl:value-of select="cpp:include_library_header(o:join_paths(('core', 'eventports', 'ineventport.hpp')))" />
        <xsl:value-of select="cpp:include_library_header(o:join_paths(('core', 'eventports', 'outeventport.hpp')))" />
        <xsl:value-of select="cpp:include_library_header('string')" />
        <xsl:value-of select="o:nl(1)" />

        <!-- Import the headers of each aggregate used -->
        <xsl:for-each select="$required_aggregates">
            <xsl:if test="position() = 1">
                <xsl:value-of select="cpp:comment('Include required base Aggregate header files', 0)" />
            </xsl:if>
            <xsl:variable name="required_file" select="cdit:get_base_aggregate_h_path(.)" />
            <xsl:value-of select="cpp:include_local_header(o:join_paths($required_file))" />
            
            <xsl:if test="position() = last()">
                <xsl:value-of select="o:nl(1)" />
            </xsl:if>
        </xsl:for-each>

        <!-- Define the namespaces -->
        <xsl:for-each select="$namespaces">
            <xsl:value-of select="cpp:namespace_start(., position() - 1)" />
        </xsl:for-each>

        <!-- Define the class -->
        <xsl:value-of select="cdit:comment_graphml_node($component, $tab)" />
        <xsl:value-of select="cpp:declare_class($class_name, 'public ::Component', $tab)" />
        <!-- Public Declarations -->
        <xsl:value-of select="cpp:public($tab + 1)" />
        <xsl:value-of select="cpp:declare_function('', $class_name, cpp:const_ref_var_def('std::string', 'name'), ';', $tab + 2)" />
        <xsl:value-of select="o:nl(1)" />

        <!-- Handle OutEventPorts -->
        <xsl:for-each select="graphml:get_child_nodes_of_kind($component, 'InEventPort')">
            <xsl:if test="position() = 1">
                <xsl:value-of select="cpp:protected($tab + 1)" />
            </xsl:if>

            <xsl:variable name="function_name" select="cdit:get_eventport_function_name(.)" />
            <xsl:variable name="port_type" select="cpp:get_qualified_type(graphml:get_port_aggregate(.))" />

            <xsl:value-of select="cdit:comment_graphml_node(., $tab + 2)" />
            <xsl:value-of select="cpp:declare_function('virtual void', $function_name, cpp:ref_var_def($port_type, 'm'), ' = 0;', $tab + 2)" />
            
            <xsl:if test="position() = last()">
                <xsl:value-of select="o:nl(1)" />
            </xsl:if>
        </xsl:for-each>

        <!-- Handle InEventPorts -->
        <xsl:for-each select="graphml:get_child_nodes_of_kind($component, 'OutEventPort')">
            <xsl:if test="position() = 1">
                <xsl:value-of select="cpp:protected($tab + 1)" />
            </xsl:if>
            
            <xsl:variable name="function_name" select="cdit:get_eventport_function_name(.)" />
            <xsl:variable name="port_type" select="cpp:get_qualified_type(graphml:get_port_aggregate(.))" />
            
            <xsl:value-of select="cdit:comment_graphml_node(., $tab + 2)" />
            <xsl:value-of select="cpp:declare_function('bool', $function_name, cpp:const_ref_var_def($port_type, 'm'), ';', $tab + 2)" />
            
            <xsl:if test="position() = last()">
                <xsl:value-of select="o:nl(1)" />
            </xsl:if>
        </xsl:for-each>

        <!-- Handle Attributes -->
        <xsl:for-each select="graphml:get_child_nodes_of_kind($component, 'Attribute')">
            <xsl:value-of select="cdit:declare_datatype_functions(., $tab + 1)" />
        </xsl:for-each>
        
        <xsl:value-of select="cpp:scope_end($tab)" />

        <!-- End the namespaces -->
        <xsl:for-each select="$namespaces">
            <xsl:value-of select="cpp:namespace_end(., position() - 1)" />
        </xsl:for-each>

        <xsl:value-of select="cpp:define_guard_end($define_guard_name)" />
    </xsl:function>

    <!--
        Gets the Component Interface CPP file
    -->
    <xsl:function name="cdit:get_component_int_cpp">
        <xsl:param name="component" as="element()" />
        
        <!-- Get their required aggregates used to express Component -->
        <xsl:variable name="required_aggregates" select="cdit:get_required_aggregates($component)" />

        <xsl:variable name="in_ports" select="graphml:get_child_nodes_of_kind($component, 'InEventPort')" />
        <xsl:variable name="out_ports" select="graphml:get_child_nodes_of_kind($component, 'OutEventPort')" />
        <xsl:variable name="attributes" select="graphml:get_child_nodes_of_kind($component, 'Attribute')" />

        
        
        <xsl:variable name="namespaces" select="o:trim_list(graphml:get_namespace($component))" />
        <xsl:variable name="label" select="graphml:get_label($component)" />
        <xsl:variable name="class_name" select="concat(o:title_case($label), 'Int')" />

        <xsl:variable name="qualified_type" select="cpp:combine_namespaces(($namespaces, $label))" />
        <xsl:variable name="qualified_class_name" select="cpp:combine_namespaces(($namespaces, $class_name))" />
        <xsl:variable name="tab" select="0" />

        <!-- Library Includes-->
        <xsl:value-of select="cpp:include_local_header(lower-case(concat($class_name, '.h')))" />
        <xsl:value-of select="o:nl(1)" />

        <!-- Define the class -->

        <xsl:variable name="inherited_constructor" select="' : Component(name)'" />
        
        <!-- Define the Constructor-->
        <xsl:value-of select="cpp:define_function('', $qualified_class_name, $class_name, cpp:const_ref_var_def('std::string', 'name'), concat($inherited_constructor, cpp:scope_start(0)))" />
            <!-- Handle InEventPorts -->
            <xsl:for-each select="$in_ports">
                <xsl:variable name="port_label" select="graphml:get_label(.)" />
                
                <xsl:variable name="function_name" select="cdit:get_eventport_function_name(.)" />
                <xsl:variable name="qualified_function_name" select="cpp:combine_namespaces(($class_name, $function_name))" />
                <xsl:variable name="port_type" select="cpp:get_qualified_type(graphml:get_port_aggregate(.))" />

                <xsl:value-of select="cdit:comment_graphml_node(., $tab + 1)" />
                <!-- Register the callback -->
                <xsl:variable name="bind" select="cpp:invoke_static_function('std', 'bind', cpp:join_args((cpp:ref_var($qualified_function_name), 'this', 'std::placeholders::_1')), '', 0)" />
                <xsl:variable name="args" select="cpp:join_args((o:wrap_dblquote($port_label), $bind))" />
                <xsl:value-of select="cpp:invoke_templated_static_function($port_type, 'AddCallback', $args, cpp:nl(), $tab + 1) " />
            </xsl:for-each>
            <!-- Handle Attributes -->
            <xsl:for-each select="$attributes">
                <xsl:variable name="label" select="graphml:get_label(.)" />
                <xsl:variable name="type" select="cdit:get_attribute_enum_type(.)" />
                
                <xsl:value-of select="cdit:comment_graphml_node(., $tab + 1)" />
                <!-- Add the Attribute -->
                <xsl:variable name="args" select="cpp:join_args(($type, o:wrap_dblquote($label)))" />
                <xsl:variable name="get_attribute" select="cpp:invoke_static_function('', 'ConstructAttribute', $args, '', 0)" />
                <xsl:value-of select="cpp:define_variable('', cdit:get_variable_label(.), cpp:invoke_function($get_attribute, cpp:dot(), 'lock', '', 0), cpp:nl(), $tab + 1)" />
            </xsl:for-each>
            <xsl:value-of select="cpp:scope_end(0)" />
        <xsl:value-of select="o:nl(1)" />

        <!-- Handle OutEventPorts -->
        <xsl:for-each select="$out_ports">
            <xsl:variable name="port_label" select="graphml:get_label(.)" />
            <xsl:variable name="function_name" select="cdit:get_eventport_function_name(.)" />
            <xsl:variable name="port_type" select="cpp:get_qualified_type(graphml:get_port_aggregate(.))" />
            <xsl:variable name="get_port" select="cpp:invoke_templated_static_function(cpp:templated_type('OutEventPort', $port_type), 'GetTypedEventPort', o:wrap_dblquote($port_label), '', 0) " />

            <!-- Define the tx function -->
            <xsl:value-of select="cpp:define_function('bool', $qualified_class_name, $function_name, cpp:const_ref_var_def($port_type, 'm'), cpp:scope_start(0))" />
                <xsl:value-of select="cpp:define_variable(cpp:auto(), 'p', $get_port, cpp:nl(), $tab + 1)" />
                <xsl:value-of select="cpp:if('p', cpp:scope_start(0), $tab + 1)" />
                    <xsl:value-of select="cpp:invoke_function('p', cpp:arrow(), 'tx', 'm', $tab + 2)" />
                    <xsl:value-of select="cpp:nl()" />
                    <xsl:value-of select="cpp:return('true', $tab + 1)" />
                <xsl:value-of select="cpp:scope_end($tab + 1)" />
            <xsl:value-of select="cpp:return('false', $tab)" />
            <xsl:value-of select="cpp:scope_end(0)" />
            <xsl:value-of select="o:nl(1)" />
        </xsl:for-each>

        <!-- Handle Attributes -->
        <xsl:for-each select="graphml:get_child_nodes_of_kind($component, 'Attribute')">
            <xsl:value-of select="cdit:define_datatype_functions(., $qualified_class_name)" />
        </xsl:for-each>
    </xsl:function>

    <!--
        Gets the Component Implementation Header
    -->
    <xsl:function name="cdit:get_component_impl_h">
        <xsl:param name="component_impl" as="element()" />
        
        <xsl:variable name="component_definition" select="graphml:get_definition($component_impl)" />
        
        <xsl:variable name="namespaces" select="o:trim_list(graphml:get_namespace($component_definition))" />
        <xsl:variable name="component_impl_label" select="graphml:get_label($component_impl)" />
        <xsl:variable name="component_label" select="graphml:get_label($component_definition)" />

        <xsl:variable name="impl_class_name" select="concat(o:title_case($component_impl_label), 'Impl')" />
        <xsl:variable name="int_class_name" select="concat(o:title_case($component_label), 'Int')" />
        <xsl:variable name="tab" select="count($namespaces)" />

        <xsl:variable name="qualified_int_type" select="cpp:combine_namespaces(($namespaces, $int_class_name))" />
        <xsl:variable name="define_guard_name" select="upper-case(o:join_list(('COMPONENT', $namespaces, $component_impl_label, 'impl'), '_'))" />
        
        <xsl:variable name="periodic_events" select="graphml:get_child_nodes_of_kind($component_impl, 'PeriodicEvent')" />
        <xsl:variable name="in_ports" select="graphml:get_child_nodes_of_kind($component_impl, 'InEventPortImpl')" />
        <xsl:variable name="variables" select="graphml:get_child_nodes_of_kind($component_impl, 'Variable')" />
        <xsl:variable name="workers" select="cdit:get_unique_workers($component_impl)" />

        <!-- Define Guard -->
        <xsl:value-of select="cpp:define_guard_start($define_guard_name)" />

        <!-- Library Includes-->
        <xsl:value-of select="cpp:include_local_header(concat(lower-case($int_class_name), '.h'))" />

         <!-- Include the headers once for each worker type -->
         <xsl:for-each-group select="$workers" group-by="graphml:get_data_value(., 'worker')">
            <xsl:if test="position() = 1">
                <xsl:value-of select="cpp:comment('Include Worker Header Files', 0)" />
            </xsl:if>
            <xsl:value-of select="cpp:include_library_header(cdit:get_worker_header(.))" />
        </xsl:for-each-group>
        <xsl:value-of select="o:nl(1)" />

        <!-- Define the namespaces -->
        <xsl:for-each select="$namespaces">
            <xsl:value-of select="cpp:namespace_start(., position() - 1)" />
        </xsl:for-each>

        <!-- Define the class -->
        <xsl:value-of select="cdit:comment_graphml_node($component_impl, $tab)" />
        <xsl:value-of select="cpp:declare_class($impl_class_name, concat('public ', $qualified_int_type), $tab)" />

        <!-- Public Declarations -->
        <xsl:value-of select="cpp:public($tab + 1)" />
        <xsl:value-of select="cpp:declare_function('', $impl_class_name, cpp:const_ref_var_def('std::string', 'name'), ';', $tab + 2)" />

        <!-- Use Protected functions for callbacks -->
        <xsl:if test="count(($in_ports, $periodic_events)) > 0">
            <xsl:value-of select="cpp:protected($tab + 1)" />
        </xsl:if>

        <!-- Handle PeriodicEvent -->
        <xsl:for-each select="$periodic_events">
            <xsl:variable name="function_name" select="cdit:get_eventport_function_name(.)" />

            <xsl:value-of select="cdit:comment_graphml_node(., $tab + 2)" />
            <xsl:value-of select="cpp:declare_function('void', $function_name, '', ';', $tab + 2)" />
        </xsl:for-each>

        <!-- Handle InEventPorts -->
        <xsl:for-each select="$in_ports">
            <xsl:variable name="function_name" select="cdit:get_eventport_function_name(.)" />
            <xsl:variable name="port_type" select="cpp:get_qualified_type(graphml:get_port_aggregate(.))" />

            <xsl:value-of select="cdit:comment_graphml_node(., $tab + 2)" />
            <xsl:value-of select="cpp:declare_function('void', $function_name, cpp:ref_var_def($port_type, 'm'), ';', $tab + 2)" />
        </xsl:for-each>

        <!-- Handle variables -->
        <xsl:for-each select="$variables">
            <xsl:value-of select="cdit:declare_datatype_functions(., $tab + 1)" />
        </xsl:for-each>
        
        <!-- Handle Worker variables -->
        <xsl:for-each select="$workers">
            <xsl:if test="position() = 1">
                <xsl:value-of select="cpp:private($tab + 1)" />
                <xsl:value-of select="cpp:comment('Declare Worker Variables', $tab + 2)" />
            </xsl:if>
            <xsl:variable name="worker_type" select="graphml:get_data_value(., 'worker')" />
            <xsl:variable name="worker_variable" select="cdit:variablize_value(graphml:get_data_value(., 'workerID'))" />

            <xsl:value-of select="cdit:comment_graphml_node(., $tab + 2)" />
            <xsl:value-of select="cpp:declare_variable(cpp:shared_ptr($worker_type), $worker_variable, cpp:nl(), $tab + 2)" />
        </xsl:for-each>

        <xsl:value-of select="cpp:scope_end($tab)" />

        <!-- End the namespaces -->
        <xsl:for-each select="$namespaces">
            <xsl:value-of select="cpp:namespace_end(., position() - 1)" />
        </xsl:for-each>

        <xsl:value-of select="cpp:define_guard_end($define_guard_name)" />
    </xsl:function>

    <!--
        Gets the Component Implementation Cpp Code
    -->
    <xsl:function name="cdit:get_component_impl_cpp">
        <xsl:param name="component_impl" as="element()" />
        
        <xsl:variable name="component_definition" select="graphml:get_definition($component_impl)" />
        
        <xsl:variable name="namespaces" select="o:trim_list(graphml:get_namespace($component_definition))" />
        <xsl:variable name="component_impl_label" select="graphml:get_label($component_impl)" />
        <xsl:variable name="component_label" select="graphml:get_label($component_definition)" />

        <xsl:variable name="impl_class_name" select="concat(o:title_case($component_impl_label), 'Impl')" />
        <xsl:variable name="int_class_name" select="concat(o:title_case($component_label), 'Int')" />
        <xsl:variable name="tab" select="count($namespaces)" />

        <xsl:variable name="qualified_int_type" select="cpp:combine_namespaces(($namespaces, $int_class_name))" />
        <xsl:variable name="qualified_impl_type" select="cpp:combine_namespaces(($namespaces, $impl_class_name))" />
        <xsl:variable name="define_guard_name" select="upper-case(o:join_list(('COMPONENT', $namespaces, $component_impl_label, 'impl'), '_'))" />
        
        <xsl:variable name="periodic_events" select="graphml:get_child_nodes_of_kind($component_impl, 'PeriodicEvent')" />
        <xsl:variable name="in_ports" select="graphml:get_child_nodes_of_kind($component_impl, 'InEventPortImpl')" />
        <xsl:variable name="variables" select="graphml:get_child_nodes_of_kind($component_impl, 'Variable')" />
        <xsl:variable name="workers" select="cdit:get_unique_workers($component_impl)" />

        <!-- Library Includes-->
        <xsl:value-of select="cpp:include_local_header(lower-case(concat($impl_class_name, '.h')))" />
        <xsl:value-of select="o:nl(1)" />

        <!-- Include Header elements in model files -->
        <xsl:for-each select="graphml:get_descendant_nodes_of_kind($component_impl, 'Header')">
            <xsl:value-of select="cdit:generate_workflow_code(., ., 0)" />
        </xsl:for-each>

        <!-- Define the Interfaces constructor -->
        <xsl:variable name="inherited_constructor" select="concat(' : ', $qualified_int_type, '(name)')" />
        
        <!-- Define the Constructor-->
        <xsl:value-of select="cpp:define_function('', $qualified_impl_type, $impl_class_name, cpp:const_ref_var_def('std::string', 'name'), concat($inherited_constructor, cpp:scope_start(0)))" />
            <!-- Handle InEventPorts -->
            <xsl:for-each select="$periodic_events">
                <xsl:variable name="port_label" select="graphml:get_label(.)" />
                
                <xsl:variable name="function_name" select="cdit:get_eventport_function_name(.)" />
                <xsl:variable name="qualified_function_name" select="cpp:combine_namespaces(($impl_class_name, $function_name))" />
                <xsl:variable name="port_type" select="cpp:get_qualified_type(graphml:get_port_aggregate(.))" />

                <xsl:value-of select="cdit:comment_graphml_node(., $tab + 1)" />
                <!-- Register the callback -->
                <xsl:variable name="bind" select="cpp:invoke_static_function('std', 'bind', cpp:join_args((cpp:ref_var($qualified_function_name), 'this')), '', 0)" />
                <xsl:variable name="args" select="cpp:join_args((o:wrap_dblquote($port_label), $bind))" />
                <xsl:value-of select="cpp:invoke_static_function('', 'AddPeriodicCallback', $args, cpp:nl(), $tab + 1) " />
            </xsl:for-each>

            <!-- Handle Worker variables -->
            <xsl:for-each select="$workers">
                <xsl:if test="position() = 1">
                    <xsl:value-of select="cpp:comment('Declare Worker Variables', $tab + 1)" />
                </xsl:if>
                <xsl:variable name="worker_type" select="graphml:get_data_value(., 'worker')" />
                <xsl:variable name="worker_id" select="graphml:get_data_value(., 'workerID')" />
                <xsl:variable name="worker_variable" select="cdit:variablize_value($worker_id)" />
                <xsl:variable name="worker_params" select="cpp:join_args(('*this', o:wrap_dblquote($worker_id)))" />
                <xsl:variable name="worker_getter" select="cpp:invoke_templated_static_function($worker_type, 'AddTypedWorker', $worker_params, '', 0)" />
                <xsl:value-of select="cpp:define_variable('', $worker_variable, $worker_getter, cpp:nl(), $tab + 1)" />
            </xsl:for-each>
        <xsl:value-of select="cpp:scope_end(0)" />
        <xsl:value-of select="o:nl(1)" />

        <!-- Handle in_ports -->
        <xsl:for-each select="$in_ports">
            <xsl:variable name="port_label" select="graphml:get_label(.)" />
            <xsl:variable name="function_name" select="cdit:get_eventport_function_name(.)" />
            <xsl:variable name="port_type" select="cpp:get_qualified_type(graphml:get_port_aggregate(.))" />
            <xsl:variable name="get_port" select="cpp:invoke_templated_static_function(cpp:templated_type('::InEventPort', $port_type), 'GetTypedEventPort', o:wrap_dblquote($port_label), '', 0) " />

            <!-- Define the callback function -->
            <xsl:value-of select="cpp:define_function('void', $qualified_impl_type, $function_name, cpp:ref_var_def($port_type, 'm'), cpp:scope_start(0))" />
            <xsl:value-of select="cdit:generate_workflow_code(., ., 1)" />
            <xsl:value-of select="cpp:scope_end(0)" />
            <xsl:value-of select="o:nl(1)" />
        </xsl:for-each>

        <!-- Handle in_ports -->
        <xsl:for-each select="$periodic_events">
            <xsl:variable name="port_label" select="graphml:get_label(.)" />
            <xsl:variable name="function_name" select="cdit:get_eventport_function_name(.)" />
            
            <!-- Define the callback function -->
            <xsl:value-of select="cpp:define_function('void', $qualified_impl_type, $function_name, '', cpp:scope_start(0))" />
            <xsl:value-of select="cdit:generate_workflow_code(., ., 1)" />
            <xsl:value-of select="cpp:scope_end(0)" />
            <xsl:value-of select="o:nl(1)" />
        </xsl:for-each>

        <!-- Handle variables -->
        <xsl:for-each select="$variables">
            <xsl:value-of select="cdit:define_datatype_functions(., $qualified_impl_type)" />
        </xsl:for-each>
    </xsl:function>

    <xsl:function name="cdit:get_components_cmake">
        <xsl:param name="component_impls" as="element()*" />

        <xsl:variable name="sub_directories" as="xs:string*">
            <xsl:for-each select="$component_impls">
                <xsl:variable name="namespace" select="lower-case(graphml:get_namespace(.))" />
                <xsl:variable name="label" select="lower-case(graphml:get_label(.))" />
                <xsl:value-of select="o:join_paths(($namespace, $label))" />
            </xsl:for-each>
        </xsl:variable>
        
        <xsl:value-of select="cmake:add_subdirectories($sub_directories)" />
    </xsl:function>

    

    <xsl:function name="cdit:generate_worker_process_code">
        <xsl:param name="node" as="element()"/>
        <xsl:param name="tab" as="xs:integer"/>
        <xsl:variable name="worker" select="graphml:get_data_value($node, 'worker')" />

        <xsl:choose>
            <!-- Handle Vector Operations -->
            <xsl:when test="$worker = 'Vector_Operations'">
                <xsl:value-of select="cdit:generate_vector_operation($node, $tab)" />
            </xsl:when>
            <xsl:otherwise>
                <xsl:variable name="worker_name" select="cdit:get_variable_name($node)" />
                <xsl:variable name="operation" select="graphml:get_data_value($node, 'operation')" />
                <xsl:variable name="variable_name" select="cdit:get_variable_name(graphml:get_child_nodes_of_kind($node, 'ReturnParameter'))" />

                <xsl:variable name="function" select="cpp:invoke_function($worker_name, cpp:arrow(), $operation, cdit:get_parameters_as_args($node), 0)" />
                
                <xsl:choose>
                    <xsl:when test="$variable_name = ''">
                        <xsl:value-of select="concat(o:t($tab), $function, cpp:nl())"/>
                    </xsl:when>
                    <xsl:otherwise>
                        <xsl:value-of select="cpp:define_variable(cpp:auto(), $variable_name, $function, cpp:nl(), $tab)"/>
                    </xsl:otherwise>
                </xsl:choose>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:function>

    <xsl:function name="cdit:generate_vector_operation">
        <xsl:param name="node" as="element()"/>
        <xsl:param name="tab" as="xs:integer"/>

        <!-- Get the list of InputParameters-->
        <xsl:variable name="input_parameters" select="graphml:get_child_nodes_of_kind($node, 'InputParameter')" />
        <!-- Get the list of ReturnParameters-->
        <xsl:variable name="return_parameters" select="graphml:get_child_nodes_of_kind($node, 'ReturnParameter')" />

        <xsl:variable name="operation" select="graphml:get_data_value($node, 'operation')" />

        <!-- The Vector is always the first Parameter-->
        <xsl:variable name="vector" select="$input_parameters[1]" />

        <xsl:variable name="return_variable_name" select="cdit:get_variable_name($return_parameters[1])" />

        

        
        <xsl:variable name="vector_var" select="cdit:get_resolved_getter_function($vector, true(), false())" />
        
        <xsl:variable name="vector_operation">
                <xsl:choose>
                    <xsl:when test="$operation = 'Set'">
                        <xsl:variable name="index_var" select="cdit:get_resolved_getter_function($input_parameters[2], false(), false())" />
                        <xsl:variable name="value_var" select="cdit:get_resolved_getter_function($input_parameters[3], false(), false())" />

                        <xsl:variable name="at_fn" select="cpp:invoke_function($vector_var, cpp:dot(), 'at', $index_var, 0)" />
                        <xsl:value-of select="concat($at_fn, ' = ', $value_var)" />
                    </xsl:when>
                    <xsl:when test="$operation = 'Get'">
                        <xsl:variable name="index_var" select="cdit:get_resolved_getter_function($input_parameters[2], false(), false())" />
                        <xsl:variable name="value_var" select="cdit:get_resolved_getter_function($input_parameters[3], false(), false())" />

                        <xsl:variable name="at_fn" select="cpp:invoke_function($vector_var, cpp:dot(), 'at', $index_var, 0)" />
                        <xsl:value-of select="$at_fn" />
                    </xsl:when>
                    
                    <xsl:when test="$operation = 'Resize'">
                        <xsl:variable name="size_var" select="cdit:get_resolved_getter_function($input_parameters[2], false(), false())" />
                        <xsl:variable name="default_value_var" select="cdit:get_resolved_getter_function($input_parameters[3], false(), true())" />

                        <xsl:variable name="set_fn" select="cpp:invoke_function($vector_var, cpp:dot(), 'resize', cpp:join_args(($size_var, $default_value_var)), 0)" />
                        <xsl:value-of select="$set_fn" />
                    </xsl:when>
                    
                    <xsl:when test="$operation = 'Insert'">
                        <xsl:variable name="index_var" select="cdit:get_resolved_getter_function($input_parameters[2], false(), false())" />
                        <xsl:variable name="value_var" select="cdit:get_resolved_getter_function($input_parameters[3], false(), false())" />

                        <xsl:variable name="begin_fn" select="o:join_list((cpp:invoke_function($vector_var, cpp:dot(), 'begin', '', 0), '+', $index_var), ' ')" />
                        <xsl:variable name="insert_fn" select="cpp:invoke_function($vector_var, cpp:dot(), 'insert', cpp:join_args(($begin_fn, $value_var)), 0)" />
                        <xsl:value-of select="$insert_fn" />
                    </xsl:when>
                    <xsl:when test="$operation = 'Remove'">
                        <xsl:variable name="index_var" select="cdit:get_resolved_getter_function($input_parameters[2], false(), false())" />
                        <xsl:variable name="value_var" select="cdit:get_resolved_getter_function($input_parameters[3], false(), false())" />

                        <xsl:variable name="at_fn" select="cpp:invoke_function($vector_var, cpp:dot(), 'at', $index_var, 0)" />
                        <xsl:variable name="begin_fn" select="o:join_list((cpp:invoke_function($vector_var, cpp:dot(), 'begin', '', 0), '+', $index_var), ' ')" />

                        <xsl:variable name="erase_fn" select="cpp:invoke_function($vector_var, cpp:dot(), 'erase', $begin_fn, 0)" />
                        <xsl:value-of select="concat($at_fn, cpp:nl(), o:t($tab), $erase_fn)" />
                    </xsl:when>
                    <xsl:when test="$operation = 'Clear'">
                        <xsl:value-of select="cpp:invoke_function($vector_var, cpp:dot(), 'clear', '', 0)" />
                    </xsl:when>
                    <xsl:when test="$operation = 'Length'">
                        <xsl:value-of select="cpp:invoke_function($vector_var, cpp:dot(), 'size', '', 0)" />
                    </xsl:when>
                    <xsl:when test="$operation = 'Swap'">
                        <xsl:variable name="vector2_var" select="cdit:get_resolved_getter_function($input_parameters[2], true(), false())" />
                        <xsl:value-of select="cpp:invoke_function($vector_var, cpp:dot(), 'swap', $vector2_var, 0)" />
                    </xsl:when>
                    <xsl:otherwise>
                        <xsl:value-of select="cpp:warning(('cdit:generate_vector_operation()', 'Operation:', o:wrap_quote($operation), 'Not Implemented'), 0)" />
                    </xsl:otherwise>
                </xsl:choose>
            </xsl:variable>
            
            <xsl:choose>
                <xsl:when test="$return_variable_name = ''">
                    <xsl:value-of select="concat(o:t($tab), $vector_operation, cpp:nl())"/>
                </xsl:when>
                <xsl:otherwise>
                    <xsl:value-of select="cpp:define_variable(cpp:auto(), $return_variable_name, $vector_operation, cpp:nl(), $tab)"/>
                </xsl:otherwise>
            </xsl:choose>
    </xsl:function>

    <xsl:function name="cdit:generate_branch_code">
        <xsl:param name="node" as="element()"/>
        <xsl:param name="tab" as="xs:integer"/>

        <xsl:variable name="kind" select="graphml:get_kind($node)" />
        
        
        <xsl:for-each select="graphml:get_child_nodes($node)">
            <xsl:variable name="child_kind" select="graphml:get_kind(.)" />

            <xsl:variable name="prefix">
                <xsl:choose>
                    <xsl:when test="$kind = 'BranchState'">
                        <xsl:value-of select="if(position() = 1) then 'if' else 'else if'" />
                    </xsl:when>
                    <xsl:when test="$kind = 'WhileLoop' and $child_kind = 'Condition'">
                        <xsl:value-of select="'while'" />
                    </xsl:when>
                    <xsl:when test="$kind = 'WhileLoop' and $child_kind = 'ForCondition'">
                        <xsl:value-of select="'for'" />
                    </xsl:when>
                </xsl:choose>
            </xsl:variable>

            <!-- Calculate the Statement-->
            <xsl:variable name="statement">
                <xsl:choose>
                    <xsl:when test="$child_kind = 'Condition'">
                        <xsl:value-of select="cdit:get_resolved_getter_function(., false(), false())" />
                    </xsl:when>
                    <xsl:when test="$child_kind = 'ForCondition'">
                        <xsl:variable name="for_condition_children" select="graphml:get_child_nodes(.)" />
                            <xsl:choose>
                                <xsl:when test="count($for_condition_children) = 3">
                                    <xsl:variable name="first" select="$for_condition_children[1]" />
                                    <xsl:variable name="second" select="$for_condition_children[2]" />
                                    <xsl:variable name="third" select="$for_condition_children[3]" />

                                    <xsl:variable name="first_arg" select="cpp:define_variable('int', graphml:get_label($first), cdit:get_resolved_getter_function($first, false(), false()), '', 0)" />
                                    <xsl:variable name="second_arg" select="cdit:get_resolved_getter_function($second, false(), false())" />
                                    <xsl:variable name="third_arg" select="cdit:get_resolved_getter_function($third, false(), false())" />
                                    <xsl:value-of select="o:join_list(($first_arg, $second_arg, $third_arg), '; ')" />
                                </xsl:when>
                                <xsl:otherwise>
                                    <xsl:value-of select="o:warning(('Got unexpected number of children in ForCondition'))" />
                                </xsl:otherwise>
                            </xsl:choose>
                    </xsl:when>
                    <xsl:otherwise>
                        <xsl:value-of select="o:warning(('Unimplemented kind:', $child_kind))" />
                    </xsl:otherwise>
                </xsl:choose>
            </xsl:variable>

            <xsl:if test="position() = 1">
                <xsl:value-of select="o:t($tab)" />
            </xsl:if>

            <xsl:value-of select="concat($prefix, o:wrap_bracket($statement), cpp:scope_start(0))" />
            <!-- Callback into the generate workflow code to generate the code for this branch condition -->
            <xsl:value-of select="cdit:generate_workflow_code(., $node, $tab + 1)" />
            <xsl:value-of select="concat(o:t($tab), '}')" />
            <xsl:if test="position() = last()">
                <xsl:value-of select="o:nl(1)" />
            </xsl:if>
        </xsl:for-each>
    </xsl:function>

    <xsl:function name="cdit:generate_condition_code">
        <xsl:param name="node" as="element()"/>
        <xsl:param name="tab" as="xs:integer"/>
        
        <xsl:for-each select="graphml:get_child_nodes($node)">
            <xsl:value-of select="cdit:generate_workflow_code(., $node, $tab)" />
        </xsl:for-each>
    </xsl:function>

    <xsl:function name="cdit:generate_cpp_code">
        <xsl:param name="node" as="element()"/>
        <xsl:param name="tab" as="xs:integer"/>
        
        <xsl:variable name="code" select="graphml:get_data_value($node, 'code')" />

        <xsl:value-of select="cpp:scope_start($tab)" />
        <xsl:variable name="joined_code" select="o:join_list($code, ' ')" />
        
        <xsl:value-of select="cdit:comment_graphml_node($node, $tab + 1)" />
        <xsl:for-each select="tokenize($joined_code, '\n\r?')[.]">
            <xsl:value-of select="concat(o:t($tab + 1), normalize-space(.), o:nl(1))" />
        </xsl:for-each>
        <xsl:value-of select="cpp:scope_end($tab)" />
    </xsl:function>

    <xsl:function name="cdit:generate_header_code">
        <xsl:param name="node" as="element()"/>
        <xsl:param name="tab" as="xs:integer"/>
        
        <xsl:variable name="code" select="graphml:get_data_value($node, 'code')" />

        <xsl:variable name="joined_code" select="o:join_list($code, ' ')" />
        <xsl:value-of select="cdit:comment_graphml_node($node, $tab)" />
        <xsl:for-each select="tokenize($joined_code, '\n\r?')[.]">
            <xsl:value-of select="concat(o:t($tab), normalize-space(.), o:nl(1))" />
        </xsl:for-each>
    </xsl:function>

    <xsl:function name="cdit:generate_outeventportimpl_code">
        <xsl:param name="node" as="element()"/>
        <xsl:param name="tab" as="xs:integer"/>
        
        <xsl:variable name="children" select="graphml:get_child_nodes($node)" />
        <!-- An outeventportimpl should only have 1 child -->
        <xsl:if test="count($children) = 1">
            <xsl:value-of select="cdit:comment_graphml_node($node, $tab)" />
            
            <xsl:variable name="aggregate_instance" select="$children[1]" />

            <!-- Prefill the object -->
            <xsl:value-of select="cdit:generate_workflow_code($aggregate_instance, $node, $tab)" />
            <!-- Get the TX Function -->
            <xsl:variable name="tx_function" select="cdit:get_eventport_function_name($node)" />
            <!-- Get the variable we will send -->
            <xsl:variable name="message_var" select="cdit:get_mutable_get_function($aggregate_instance, false())" />

            <!-- Call the function -->
            <xsl:value-of select="cpp:invoke_static_function('', $tx_function, $message_var, cpp:nl(), $tab)" />
        </xsl:if>
    </xsl:function>

    <xsl:function name="cdit:generate_aggregateinstance_code">
        <xsl:param name="node" as="element()"/>
        <xsl:param name="tab" as="xs:integer"/>

        <xsl:variable name="aggregate" select="graphml:get_definition($node)" />

        <xsl:variable name="parent_node" select="graphml:get_parent_node($node)" />
        <xsl:variable name="parent_kind" select="graphml:get_kind($parent_node)" />

        <xsl:if test="$parent_kind = 'OutEventPortImpl'">
            <xsl:variable name="aggregate_type" select="cpp:get_aggregate_qualified_type($aggregate, 'base')" />
            <xsl:variable name="variable_name" select="cdit:get_variable_name($node)" />
            <xsl:value-of select="cpp:define_variable($aggregate_type, $variable_name, cdit:get_resolved_getter_function($node, false(), true()), cpp:nl(), $tab)" />
        </xsl:if>

        
        <xsl:for-each select="graphml:get_child_nodes($node)">
            <xsl:variable name="setter_function" select="cdit:get_set_function(.)" />
            <xsl:variable name="value" select="cdit:get_resolved_getter_function(., true(), true())" />
            <xsl:if test="$value != ''">
                <xsl:value-of select="cpp:invoke_static_function('', $setter_function, $value, cpp:nl(), $tab)" />
            </xsl:if>
            <xsl:value-of select="cdit:generate_workflow_code(., $node, $tab)" />
        </xsl:for-each>
    </xsl:function>
    
    <xsl:function name="cdit:generate_setter_code">
        <xsl:param name="node" as="element()"/>
        <xsl:param name="tab" as="xs:integer"/>

        <xsl:variable name="operator" select="graphml:get_data_value($node, 'operator')" />

        <xsl:variable name="input_parameters" select="graphml:get_child_nodes_of_kind($node, 'InputParameter')" />
        <xsl:variable name="variadic_parameters" select="graphml:get_child_nodes_of_kind($node, 'VariadicParameter')" />

        <xsl:if test="count($input_parameters) = 1 and count($variadic_parameters) = 1">
            <xsl:variable name="var_setter">    
                <xsl:value-of select="cdit:get_resolved_getter_function($input_parameters[1], true(), false())" />
            </xsl:variable>

            <xsl:variable name="value_setter">    
                <xsl:value-of select="cdit:get_resolved_getter_function($variadic_parameters[1], false(), false())" />
            </xsl:variable>

            <xsl:if test="$var_setter and $value_setter">
                <xsl:value-of select="concat(o:t($tab), $var_setter, ' ', $operator, ' ', $value_setter, cpp:nl())" />
            </xsl:if>
        </xsl:if>
    </xsl:function>

    <xsl:function name="cdit:get_resolved_getter_function">
        <xsl:param name="node"/>
        <xsl:param name="mutable" as="xs:boolean"/>
        <xsl:param name="allow_blank" as="xs:boolean"/>

        <xsl:variable name="label" select="graphml:get_label($node)" />
        <xsl:variable name="type" select="graphml:get_type($node)" />
        <xsl:variable name="value" select="graphml:get_value($node)" />

        <!-- Get the Source ID's which data link to this element -->
        <xsl:variable name="sources" select="graphml:get_sources($node, 'Edge_Data')" />

        <xsl:choose>
            <!-- Use Data Edges first -->
            <xsl:when test="count($sources) > 0">
                <xsl:value-of select="cdit:get_mutable_get_function($sources[1], $mutable)" />
            </xsl:when>
            <!-- Use Value second -->
            <xsl:when test="$value != ''">
                <xsl:value-of select="$value" />
            </xsl:when>
            <xsl:when test="$allow_blank">
                <xsl:value-of select="''" />
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="cpp:warning(('ID:', graphml:get_id($node), 'No Value Set'), 0)" />
            </xsl:otherwise>
        </xsl:choose>
    </xsl:function>

    <xsl:function name="cdit:get_set_function">
        <xsl:param name="node" as="element()"/>
        
        <xsl:variable name="parent_node" select="graphml:get_parent_node($node)" />
        
        <xsl:variable name="setter" select="cdit:get_setter_function_name($node)" />
        <xsl:variable name="parent_getter" select="if($parent_node) then cdit:get_mutable_get_function($parent_node, true()) else ''" />
        <xsl:value-of select="o:join_list(($parent_getter, $setter), cpp:dot())" />
    </xsl:function>


    <xsl:function name="cdit:get_mutable_get_function">
        <xsl:param name="node" as="element()"/>
        <xsl:param name="mutable" as="xs:boolean"/>
        
        <xsl:variable name="parent_node" select="graphml:get_parent_node($node)" />
        <xsl:variable name="kind" select="graphml:get_kind($node)" />
        <xsl:variable name="parent_kind" select="graphml:get_kind($parent_node)" />
    
        <xsl:variable name="getter">
            <xsl:choose>
                <xsl:when test="$kind = 'Variable'">
                    <xsl:value-of select="cdit:get_inplace_getter($node, $mutable)" />
                </xsl:when>
                <xsl:when test="$kind = 'VariableParameter'">
                    <xsl:value-of select="graphml:get_label($node)" />
                </xsl:when>
                <xsl:when test="$kind = 'AttributeImpl'">
                    <xsl:value-of select="cdit:get_inplace_getter($node, $mutable)" />
                </xsl:when>
                <xsl:when test="$kind = 'EnumInstance'">
                    <xsl:value-of select="cdit:get_inplace_getter($node, $mutable)" />
                </xsl:when>
                <xsl:when test="$kind= 'AggregateInstance'">
                     <xsl:choose>
                        <xsl:when test="$parent_kind = 'OutEventPortImpl'">
                            <!-- We have defined a variable based on the Top Level Aggregate -->
                            <xsl:value-of select="cdit:get_variable_name($node)" />
                        </xsl:when>
                        <xsl:when test="$parent_kind = 'InEventPortImpl'" />
                        <xsl:when test="$parent_kind = 'Variable'" />
                        <xsl:otherwise>
                            <xsl:value-of select="cdit:get_inplace_getter($node, $mutable)" />
                        </xsl:otherwise>
                    </xsl:choose>
                </xsl:when>
                <xsl:when test="$kind = 'EnumMember'">
                    <xsl:value-of select="cdit:get_resolved_enum_member_type($node)" />
                </xsl:when>
                <xsl:when test="$kind = 'MemberInstance'">
                    <xsl:value-of select="cdit:get_inplace_getter($node, $mutable)" />
                </xsl:when>
                <xsl:when test="$kind = 'VectorInstance'">
                    <xsl:value-of select="cdit:get_inplace_getter($node, $mutable)" />
                </xsl:when>
                <xsl:when test="$kind = 'ReturnParameter'">
                    <xsl:value-of select="cdit:get_variable_name($node)" />
                </xsl:when>
                <xsl:when test="$kind = 'InEventPortImpl'">
                    <xsl:value-of select="'m'" />
                </xsl:when>
                <xsl:otherwise>
                    <xsl:value-of select="''" />
                </xsl:otherwise>
            </xsl:choose>
        </xsl:variable>

        <xsl:variable name="parent_getter" select="if($parent_node) then cdit:get_mutable_get_function($parent_node, $mutable) else ''" />
        <xsl:value-of select="o:join_list(($parent_getter, $getter), cpp:dot())" />
    </xsl:function>

    <xsl:function name="cdit:get_inplace_getter" as="xs:string">
        <xsl:param name="node" as="element()*" />
        <xsl:param name="mutable" as="xs:boolean"/>

        <xsl:variable name="get_func" select="concat(if($mutable) then '' else 'get_', graphml:get_label($node))" />

        <xsl:value-of select="cpp:invoke_function('', '', $get_func, '', 0)"/>
    </xsl:function>

    <xsl:function name="cdit:get_setter_function_name" as="xs:string">
        <xsl:param name="node" as="element()*" />

        <xsl:value-of select="concat('set_', graphml:get_label($node))" />
    </xsl:function>


    <xsl:function name="cdit:get_variable_name">
        <xsl:param name="node" as="element()*"/>
        <xsl:variable name="kind" select="graphml:get_kind($node)" />

        <xsl:choose>
            <xsl:when test="$kind = 'WorkerProcess'">
                <xsl:value-of select="cdit:variablize_value(graphml:get_data_value($node, 'workerID'))"/>
            </xsl:when>
            <xsl:when test="$kind = 'Variable' or $kind = 'AttributeImpl'">
                <xsl:value-of select="cdit:variablize_value(graphml:get_label($node))"/>
            </xsl:when>

            <xsl:otherwise>
                <xsl:variable name="id" select="graphml:get_id($node)" />
                <xsl:value-of select="lower-case(o:join_list((graphml:get_label($node), $id), '_'))"/>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:function>

    <xsl:function name="cdit:get_parameters_as_args" as="xs:string*">
        <xsl:param name="node" as="element()"/>

        <xsl:variable name="args" as="xs:string*">
            <xsl:for-each select="graphml:get_child_nodes($node)">
                <xsl:variable name="kind" select="graphml:get_kind(.)" />
                <xsl:choose>
                    <!-- Handle Vector Operations -->
                    <xsl:when test="$kind = 'InputParameter' or $kind = 'VariadicParameter'">
                        <xsl:variable name="value" select="graphml:get_data_value(., 'value')" />
                        <xsl:variable name="getter" select="cdit:get_resolved_getter_function(., true(), false())" />

                        <xsl:variable name="suffix">
                            <xsl:if test="$kind = 'VariadicParameter' and $value = 'String'">
                                <xsl:value-of select="'.c_str()'" />
                            </xsl:if>
                        </xsl:variable>
                        <xsl:sequence select="o:join_list(($getter, $suffix), '')"/>
                    </xsl:when>
                </xsl:choose>
            </xsl:for-each>
        </xsl:variable>

        <xsl:value-of select="cpp:join_args($args)" />
    </xsl:function>
    
    <xsl:function name="cdit:uses_branch" as="xs:boolean">
        <xsl:param name="node" as="element()"/>
        <xsl:variable name="kind" select="graphml:get_kind($node)" />
        <xsl:value-of select="$kind = 'BranchState' or $kind = 'WhileLoop'" />
    </xsl:function>

    <xsl:function name="cdit:generate_workflow_code">
        <xsl:param name="node" as="element()"/>
        <xsl:param name="previous_node" as="element()"/>
        <xsl:param name="tab" as="xs:integer"/>
        
        <xsl:variable name="kind" select="graphml:get_kind($node)" />
        <xsl:variable name="comment" select="graphml:get_data_value($node, 'comment')" />
        
        <xsl:if test="$comment != ''">
            <xsl:value-of select="cpp:comment(('Model Comment:', o:nl(1), $comment), $tab)" />
        </xsl:if>
        
        <xsl:variable name="should_traverse" as="xs:boolean">
            <xsl:choose>
                <xsl:when test="$kind = 'Termination'">
                    <!-- Continue code generating on Termination, Only if our last node was a branch element -->
                    <xsl:value-of select="cdit:uses_branch($previous_node)" />
                </xsl:when>
                <xsl:otherwise> 
                    <xsl:value-of select="true()" />
                </xsl:otherwise>
            </xsl:choose>
        </xsl:variable>

        <xsl:choose>
            <xsl:when test="$kind = 'PeriodicEvent'" />
            <xsl:when test="$kind = 'InEventPortImpl'" />
            <xsl:when test="$kind = 'Condition'" />
            <xsl:when test="$kind = 'ForCondition'" />
            <xsl:when test="$kind = 'Termination'" />
            <xsl:when test="$kind = 'MemberInstance'" />
            <xsl:when test="$kind = 'VectorInstance'" />
            <xsl:when test="$kind = 'EnumInstance'" />
            <xsl:when test="$kind = 'WorkerProcess'">
                <xsl:value-of select="cdit:generate_worker_process_code($node, $tab)" />
            </xsl:when>
            <xsl:when test="$kind = 'Setter'">
                <xsl:value-of select="cdit:generate_setter_code($node, $tab)" />
            </xsl:when>
            <xsl:when test="cdit:uses_branch($node)">
                <xsl:value-of select="cdit:generate_branch_code($node, $tab)" />
            </xsl:when>
            <xsl:when test="$kind = 'Code'">
                <xsl:value-of select="cdit:generate_cpp_code($node, $tab)" />
            </xsl:when>
            <xsl:when test="$kind = 'Header'">
                <xsl:value-of select="cdit:generate_header_code($node, $tab)" />
            </xsl:when>
            <xsl:when test="$kind = 'OutEventPortImpl'">
                <xsl:value-of select="cdit:generate_outeventportimpl_code($node, $tab)" />
            </xsl:when>
            <xsl:when test="$kind = 'AggregateInstance'">
                <xsl:value-of select="cdit:generate_aggregateinstance_code($node, $tab)" />
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="o:warning(('cdit:generate_workflow_code()', 'Node Kind:', o:wrap_quote($kind), 'Not Implemented'))" />
            </xsl:otherwise>
        </xsl:choose>

        <xsl:if test="$should_traverse">
            <xsl:for-each select="graphml:get_targets($node, 'Edge_Workflow')">
                <xsl:value-of select="cdit:generate_workflow_code(., $node, $tab)" />
            </xsl:for-each>
        </xsl:if>
    </xsl:function>

    <xsl:function name="cdit:get_component_export">
        <xsl:param name="component_impl" as="element()" />
        
        <xsl:value-of select="cpp:include_library_header(o:join_paths(('core', 'libcomponentexport.h')))" />
        <xsl:value-of select="o:nl(1)" />

        <xsl:variable name="component" select="graphml:get_definition($component_impl)" />
        <xsl:variable name="namespaces" select="o:trim_list(graphml:get_namespace($component))" />
        <xsl:variable name="component_impl_label" select="graphml:get_label($component_impl)" />
        <xsl:variable name="impl_class_name" select="concat(o:title_case($component_impl_label), 'Impl')" />
        <xsl:variable name="qualified_impl_type" select="cpp:combine_namespaces(($namespaces, $impl_class_name))" />
        
        <xsl:variable name="component_header_path" select="lower-case(concat($impl_class_name, '.h'))" />

        <xsl:value-of select="cpp:comment(('Include the component impl'), 0)" />
        <xsl:value-of select="cpp:include_local_header($component_header_path)" />
        <xsl:value-of select="o:nl(1)" />

        <xsl:value-of select="cpp:define_function(cpp:pointer_var_def('Component', ''), '', 'ConstructComponent', cpp:const_ref_var_def('std::string', 'name'), cpp:scope_start(0))" />
        <xsl:variable name="constructor" select="cpp:invoke_static_function('', $qualified_impl_type, 'name', '', 0)" />
        <xsl:value-of select="cpp:return(concat('new ', $constructor), 1)" />
        <xsl:value-of select="cpp:scope_end(0)" />
    </xsl:function>

    <xsl:function name="cdit:get_component_cmake">
        <xsl:param name="component_impl" as="element()" />

        <xsl:variable name="component" select="graphml:get_definition($component_impl)" />
        <xsl:variable name="component_label" select="graphml:get_label($component_impl)" />
        <xsl:variable name="component_label_lc" select="lower-case($component_label)" />
        <xsl:variable name="namespace" select="graphml:get_namespace($component)" />
        
        
        <xsl:variable name="module_lib_name" select="lower-case(o:join_list(('component', $namespace, $component_label), '_'))" />
        <xsl:variable name="proj_name" select="$module_lib_name" />

        <xsl:variable name="binary_dir_var" select=" cmake:current_binary_dir_var()" />
        <xsl:variable name="source_dir_var" select=" cmake:current_source_dir_var()" />

        <xsl:value-of select="cmake:set_project_name($proj_name)" />

        <!-- Find re_core -->
        <xsl:value-of select="cmake:find_re_core_library()" />


        <xsl:variable name="component_impl_h" select="concat($component_label_lc, 'impl.h')" />
        <xsl:variable name="component_impl_cpp" select="concat($component_label_lc, 'impl.cpp')" />
        <xsl:variable name="component_int_h" select="concat($component_label_lc, 'int.h')" />
        <xsl:variable name="component_int_cpp" select="concat($component_label_lc, 'int.cpp')" />
        
        
        
         <!-- Set Source files -->
        <xsl:value-of select="concat('set(SOURCE', o:nl(1))" />
        <xsl:value-of select="concat(o:t(1), o:join_paths(($source_dir_var, $component_int_cpp)), o:nl(1))" />
        <xsl:value-of select="concat(o:t(1), o:join_paths(($source_dir_var, $component_impl_cpp)), o:nl(1))" />
        <xsl:value-of select="concat(o:t(1), o:join_paths(($source_dir_var, 'libcomponentexport.cpp')), o:nl(1))" />
        <xsl:value-of select="concat(o:t(0), ')', o:nl(1))" />
        <xsl:value-of select="o:nl(1)" />

        <!-- Set Header files -->
        <xsl:value-of select="concat('set(HEADERS', o:nl(1))" />
        <xsl:value-of select="concat(o:t(1), o:join_paths(($source_dir_var, $component_int_h)), o:nl(1))" />
        <xsl:value-of select="concat(o:t(1), o:join_paths(($source_dir_var, $component_impl_h)), o:nl(1))" />
        <xsl:value-of select="concat(o:t(0), ')', o:nl(1))" />
        <xsl:value-of select="o:nl(1)" />

        <xsl:variable name="args" select="o:join_list((cmake:wrap_variable('SOURCE'), cmake:wrap_variable('HEADERS')), ' ')" />
        <xsl:value-of select="cmake:add_shared_library('PROJ_NAME', 'MODULE', $args)" />
        <xsl:value-of select="o:nl(1)" />

        <xsl:value-of select="cmake:comment('Include the runtime environment directory', 0)" />
        <xsl:value-of select="cmake:target_include_directories('PROJ_NAME', cmake:get_re_path('src'), 0)" />

        <xsl:value-of select="cmake:comment('Link against runtime environment', 0)" />
        <xsl:value-of select="cmake:target_link_libraries('PROJ_NAME', cmake:wrap_variable('RE_CORE_LIBRARIES'), 0)" />
        <xsl:value-of select="o:nl(1)" />

        <xsl:variable name="relative_path" select="cmake:get_relative_path(('component', $namespace, $component_label))" />
        <xsl:variable name="required_aggregates" select="cdit:get_required_aggregates($component_impl)" />

        <xsl:variable name="workers" select="cdit:get_unique_workers($component_impl)" />

         <!-- Include the headers once for each worker type -->
         <xsl:for-each-group select="$workers" group-by="graphml:get_data_value(., 'worker')">
            <xsl:if test="position() = 1">
                <xsl:value-of select="cmake:comment('Include Worker Header Files', 0)" />
            </xsl:if>
            <xsl:variable name="worker_lib_name" select="graphml:get_data_value(., 'file')" />
            <xsl:variable name="worker_lib_var" select="upper-case(concat($worker_lib_name, '_LIBRARIES'))" />
            <xsl:value-of select="cmake:find_library($worker_lib_name, $worker_lib_var, cmake:get_re_path('lib'))" />
            <xsl:value-of select="cmake:target_link_libraries('PROJ_NAME', cmake:wrap_variable($worker_lib_var), 0)" />
            <xsl:if test="position() = last()">
                <xsl:value-of select="o:nl(1)" />
            </xsl:if>
        </xsl:for-each-group>

        <!-- Include the required aggregate files -->
        <xsl:if test="count($required_aggregates) > 0">
            <xsl:value-of select="cmake:comment('Include required aggregates source dirs', 0)" />
            <xsl:variable name="required_path" select="o:join_paths(($source_dir_var, $relative_path, 'datatypes', 'base'))" />
            <xsl:value-of select="cmake:target_include_directories('PROJ_NAME', $required_path, 0)" />
            <xsl:value-of select="o:nl(1)" />
        </xsl:if>

        <!-- Include the required aggregate files -->
        <xsl:for-each select="$required_aggregates">
            <xsl:if test="position() = 1">
                <xsl:value-of select="cmake:comment('Link against required aggregates libraries', 0)" />
            </xsl:if>
            <xsl:variable name="required_lib_name" select="cmake:get_aggregates_middleware_shared_library_name(., 'base')" />
            <xsl:value-of select="cmake:target_link_libraries('PROJ_NAME', $required_lib_name, 0)" />
            <xsl:if test="position() = last()">
                <xsl:value-of select="o:nl(1)" />
            </xsl:if>
        </xsl:for-each>
    </xsl:function>

</xsl:stylesheet>