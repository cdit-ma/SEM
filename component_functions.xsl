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

    <xsl:function name="cdit:define_attribute">
        <xsl:param name="attribute" as="element()" />
        <xsl:param name="tab" as="xs:integer" />
        
        <xsl:variable name="label" select="graphml:get_label($attribute)" />
        <xsl:variable name="type" select="cdit:get_attribute_enum_type($attribute)" />
            
        <xsl:variable name="args" select="cpp:join_args(($type, o:wrap_dblquote($label)))" />
        <xsl:variable name="get_attribute" select="cpp:invoke_static_function('', 'ConstructAttribute', $args, '', 0)" />
            
        <xsl:value-of select="cdit:comment_graphml_node($attribute, $tab)" />
        <xsl:value-of select="cpp:define_variable('', cdit:get_variable_label($attribute), cpp:invoke_function($get_attribute, cpp:dot(), 'lock', '', 0), cpp:nl(), $tab)" />
    </xsl:function>

    <xsl:function name="cdit:define_workload_function">
        <xsl:param name="entity" as="element()*" />
        <xsl:param name="qualified_class" as="xs:string" />
        
        <xsl:variable name="function_name" select="cdit:get_function_name($entity)" />
        
        <xsl:variable name="kind" select="graphml:get_kind($entity)" />

        <xsl:variable name="return_parameter">
            <xsl:choose>
                <xsl:when test="$kind = 'SubscriberPortImpl' or $kind = 'PeriodicPort'">
                    <xsl:value-of select="cpp:void()" />
                </xsl:when>
                <xsl:when test="$kind = 'Function'">
                    <xsl:value-of select="cdit:get_function_return_parameter_declarations($entity)" />
                </xsl:when>
                <xsl:when test="$kind = 'ReplierPortImpl'">
                    <xsl:value-of select="cdit:get_function_return_parameter_declarations($entity)" />
                </xsl:when>
                <xsl:otherwise>
                    <xsl:value-of select="cpp:warning(('cdit:define_workload_function()', 'Kind:', o:wrap_quote($kind), 'Not Implemented'), 0)" />
                </xsl:otherwise>
            </xsl:choose>
        </xsl:variable>

        <xsl:variable name="input_parameters">
            <xsl:choose>
                <xsl:when test="$kind = 'SubscriberPortImpl'">
                    <xsl:variable name="parameter_type" select="cpp:get_qualified_type(graphml:get_port_aggregate($entity))" />
                    <xsl:value-of select="cpp:ref_var_def($parameter_type, 'm')" />
                </xsl:when>
                <xsl:when test="$kind = 'PeriodicPort'">
                    <xsl:value-of select="''" />
                </xsl:when>
                <xsl:when test="$kind = 'Function'">
                    <xsl:value-of select="cdit:get_function_input_parameter_declarations($entity)" />
                </xsl:when>
                <xsl:when test="$kind = 'ReplierPortImpl'">
                    <xsl:value-of select="cdit:get_function_input_parameter_declarations($entity)" />
                </xsl:when>
                <xsl:otherwise>
                    <xsl:value-of select="cpp:warning(('cdit:define_workload_function()', 'Kind:', o:wrap_quote($kind), 'Not Implemented'), 0)" />
                </xsl:otherwise>
            </xsl:choose>
        </xsl:variable>
        
        <!-- Define the Workload function -->
        <xsl:value-of select="cpp:define_function($return_parameter, $qualified_class, $function_name, $input_parameters, cpp:scope_start(0))" />
        <xsl:value-of select="cdit:generate_workflow_code($entity, $entity, 1)" />
        <xsl:value-of select="cpp:scope_end(0)" />
        <xsl:value-of select="o:nl(1)" />
    </xsl:function>

    

    <xsl:function name="cdit:include_worker_headers">
        <xsl:param name="worker_instances" as="element()*" />

        <xsl:for-each-group select="$worker_instances" group-by="graphml:get_definition(.)">
            <xsl:if test="position() = 1">
                <xsl:value-of select="cpp:comment('Include Worker Header Files', 0)" />
            </xsl:if>

            <xsl:variable name="header_file" select="cdit:get_class_header(.)" />
            
            <xsl:value-of select="cpp:include_library_header($header_file)" />
            <xsl:value-of select="if (position() = last()) then o:nl(1) else ''" />
        </xsl:for-each-group>
    </xsl:function>

    <xsl:function name="cdit:get_custom_class_path" as="xs:string">
        <xsl:param name="custom_class_instance" as="element()" />
        
        <xsl:variable name="custom_class_def" select="graphml:get_definition($custom_class_instance)" />
        <xsl:variable name="label" select="graphml:get_label($custom_class_def)" />
        <xsl:variable name="namespace" select="graphml:get_namespace($custom_class_def)" />
        
        <xsl:value-of select="lower-case(o:join_paths(('classes', $namespace, $label)))" />
    </xsl:function>

    <xsl:function name="cdit:get_custom_class_header_path" as="xs:string">
        <xsl:param name="custom_class_instance" as="element()" />
        
        <xsl:variable name="custom_class_def" select="graphml:get_definition($custom_class_instance)" />
        <xsl:variable name="label" select="graphml:get_label($custom_class_def)" />

        <xsl:variable name="path" select="cdit:get_custom_class_path($custom_class_instance)" />

        <xsl:variable name="header_file" select="concat($label, '.h')" />
        
        
        <xsl:value-of select="lower-case(o:join_paths(($path, $header_file)))" />
    </xsl:function>


    <xsl:function name="cdit:include_custom_class_headers">
        <xsl:param name="custom_class_instances" as="element()*" />

        <xsl:for-each-group select="$custom_class_instances" group-by="graphml:get_definition(.)">
            <xsl:if test="position() = 1">
                <xsl:value-of select="cpp:comment('Include Custom Class Header Files', 0)" />
            </xsl:if>

            <!-- TODO Include correct path -->
            
            <xsl:variable name="header_file" select="cdit:get_custom_class_header_path(.)" />

            <xsl:value-of select="cpp:include_local_header($header_file)" />
            <xsl:value-of select="if (position() = last()) then o:nl(1) else ''" />
        </xsl:for-each-group>
    </xsl:function>

    

    <xsl:function name="cdit:declare_function">
        <xsl:param name="function" as="element()" />
        <xsl:param name="tab" as="xs:integer" />

        <xsl:variable name="function_name" select="cdit:get_function_name($function)" />
        <xsl:variable name="return_parameter" select="cdit:get_function_return_parameter_declarations($function)" />
        <xsl:variable name="input_parameters" select="cdit:get_function_input_parameter_declarations($function)" />
        
        <xsl:value-of select="cdit:comment_graphml_node($function, $tab)" />
        <xsl:value-of select="cpp:declare_function($return_parameter, $function_name, $input_parameters, ';', $tab)" />
        
    </xsl:function>

    <!--
        Gets the Component Interface Header
    -->
    <xsl:function name="cdit:get_component_int_h">
        <xsl:param name="component" as="element()" />
        
        <!-- Get the Namespace -->
        <xsl:variable name="namespaces" select="graphml:get_namespace($component)" />
        <xsl:variable name="tab" select="count($namespaces)" />

        <!-- Get the label -->
        <xsl:variable name="component_label" select="graphml:get_label($component)" />
        <xsl:variable name="class_type" select="concat(o:title_case($component_label), 'Int')" />
        <xsl:variable name="qualified_class_type" select="cpp:combine_namespaces(($namespaces, $class_type))" />

        <xsl:variable name="define_guard_name" select="upper-case(o:join_list(('COMPONENT', $namespaces, $class_type), '_'))" />

        <!-- Get the required aggregates, exclude Aggregates for the Interface -->
        <xsl:variable name="aggregates" select="cdit:get_required_aggregates($component, true())" />

        <!-- Get the children required for generation -->
        <xsl:variable name="sub_ports" select="graphml:get_child_nodes_of_kind($component, 'SubscriberPort')" />
        <xsl:variable name="pub_ports" select="graphml:get_child_nodes_of_kind($component, 'PublisherPort')" />
        <xsl:variable name="req_ports" select="graphml:get_child_nodes_of_kind($component, 'RequesterPort')" />
        <xsl:variable name="rep_ports" select="graphml:get_child_nodes_of_kind($component, 'ReplierPort')" />

        <xsl:variable name="attributes" select="graphml:get_child_nodes_of_kind($component, 'Attribute')" />

        <!-- Preamble -->
        <xsl:value-of select="cpp:print_regen_version('component_functions.xsl', 'cdit:get_component_int_h', 0)" />

        <!-- Define Guard -->
        <xsl:value-of select="cpp:define_guard_start($define_guard_name)" />

        <!-- Include the Component header -->
        <xsl:value-of select="cpp:include_library_header(o:join_paths(('core', 'component.h')))" />
        
        <!-- Conditionally include the Subscriber Port header -->
        <xsl:if test="count($sub_ports) > 0">
            <xsl:variable name="header_file" select="o:join_paths(('core', 'ports', 'pubsub', 'subscriberport.hpp'))" />
            <xsl:value-of select="cpp:include_library_header($header_file)" />
        </xsl:if>
        
        <!-- Conditionally include Publisher Port header -->
        <xsl:if test="count($pub_ports) > 0">
            <xsl:variable name="header_file" select="o:join_paths(('core', 'ports','pubsub', 'publisherport.hpp'))" />
            <xsl:value-of select="cpp:include_library_header($header_file)" />
        </xsl:if>

        <!-- Conditionally include the Requester Port header -->
        <xsl:if test="count($req_ports) > 0">
            <xsl:variable name="header_file" select="o:join_paths(('core', 'ports', 'requestreply', 'requesterport.hpp'))" />
            <xsl:value-of select="cpp:include_library_header($header_file)" />
        </xsl:if>
        
        <!-- Conditionally include Replier Port header -->
        <xsl:if test="count($rep_ports) > 0">
            <xsl:variable name="header_file" select="o:join_paths(('core', 'ports', 'requestreply', 'replierport.hpp'))" />
            <xsl:value-of select="cpp:include_library_header($header_file)" />
        </xsl:if>
        
        <xsl:value-of select="o:nl(1)" />

        <!-- Include the Aggregates -->
        <xsl:value-of select="cdit:include_aggregate_headers('Base', $aggregates)" />

        <!-- Define the Namespaces -->
        <xsl:value-of select="cpp:define_namespaces($namespaces)" />

        <!-- Class Declaration -->
        <xsl:value-of select="cdit:comment_graphml_node($component, $tab)" />
        <xsl:value-of select="cpp:declare_class($class_type, 'public ::Component', $tab)" />

        <!-- Public Declarations -->
        <xsl:value-of select="cpp:public($tab + 1)" />

        <!-- Constructor Declaration -->
        <xsl:value-of select="cpp:declare_function('', $class_type, cpp:const_ref_var_def('std::string', 'name'), ';', $tab + 2)" />

        <!-- Protected -->
        <xsl:if test="count(($sub_ports, $pub_ports)) > 0">
            <xsl:value-of select="cpp:protected($tab + 1)" />
        </xsl:if>

        <!-- Subscriber Ports -->
        <xsl:for-each select="$sub_ports">
            <xsl:if test="position() = 1">
                <xsl:value-of select="cpp:comment('SubscriberPort callback declarations', $tab + 2)" />
            </xsl:if>
            <xsl:variable name="function_name" select="cdit:get_function_name(.)" />
            <xsl:variable name="port_type" select="cpp:get_qualified_type(graphml:get_port_aggregate(.))" />

            <xsl:value-of select="cdit:comment_graphml_node(., $tab + 2)" />
            <xsl:value-of select="cpp:declare_function('virtual void', $function_name, cpp:ref_var_def($port_type, 'm'), ' = 0;', $tab + 2)" />
            <xsl:value-of select="if(position() = last()) then o:nl(1) else ''" />
        </xsl:for-each>

        <!-- Publisher Ports -->
        <xsl:for-each select="$pub_ports">
            <xsl:if test="position() = 1">
                <xsl:value-of select="cpp:comment('PublisherPort declarations', $tab + 2)" />
            </xsl:if>
            <xsl:variable name="function_name" select="cdit:get_function_name(.)" />
            <xsl:variable name="port_type" select="cpp:get_qualified_type(graphml:get_port_aggregate(.))" />
            
            <xsl:value-of select="cdit:comment_graphml_node(., $tab + 2)" />
            <xsl:value-of select="cpp:declare_function('bool', $function_name, cpp:const_ref_var_def($port_type, 'm'), ';', $tab + 2)" />
            <xsl:value-of select="if(position() = last()) then o:nl(1) else ''" />
        </xsl:for-each>

        <!-- Requester Ports -->
        <xsl:for-each select="$req_ports">
            <xsl:if test="position() = 1">
                <xsl:value-of select="cpp:comment('RequesterPort declarations', $tab + 2)" />
            </xsl:if>
            
            <xsl:variable name="server_interface" select="graphml:get_port_aggregate(.)" />
            <xsl:variable name="function_name" select="cdit:get_function_name(.)" />

            <xsl:variable name="return_type" select="cdit:get_requester_port_return_type($server_interface)" />
            <xsl:variable name="request_params" select="cdit:get_requester_port_parameters($server_interface)" />

            <xsl:value-of select="cdit:comment_graphml_node(., $tab + 2)" />
            <xsl:value-of select="cpp:declare_function($return_type, $function_name, $request_params, ';', $tab + 2)" />
            <xsl:value-of select="if(position() = last()) then o:nl(1) else ''" />
        </xsl:for-each>

        <!-- Replier Ports -->
        <xsl:for-each select="$rep_ports">
            <xsl:if test="position() = 1">
                <xsl:value-of select="cpp:comment('ReplierPort callback declarations', $tab + 2)" />
            </xsl:if>
            <xsl:variable name="server_interface" select="graphml:get_port_aggregate(.)" />
            <xsl:variable name="function_name" select="cdit:get_function_name(.)" />

            <xsl:variable name="return_type" select="cdit:get_replier_port_return_type($server_interface)" />
            <xsl:variable name="request_params" select="cdit:get_replier_port_parameters($server_interface)" />

            <xsl:value-of select="cdit:comment_graphml_node(., $tab + 2)" />
            <xsl:value-of select="cpp:declare_function(concat('virtual ', $return_type), $function_name, $request_params, ' = 0;', $tab + 2)" />
            <xsl:value-of select="if(position() = last()) then o:nl(1) else ''" />
        </xsl:for-each>

        <!-- Attributes Declarations -->
        <xsl:for-each select="$attributes">
            <xsl:value-of select="cdit:declare_datatype_functions(., $tab + 1)" />
        </xsl:for-each>
        
        <!-- End the Class Declaration -->
        <xsl:value-of select="cpp:scope_end($tab)" />

        <!-- End Namespaces -->
        <xsl:value-of select="cpp:close_namespaces($namespaces)" />

        <!-- Define Guard -->
        <xsl:value-of select="cpp:define_guard_end($define_guard_name)" />
    </xsl:function>

    <!--
        Gets the Component Interface CPP file
    -->
    <xsl:function name="cdit:get_component_int_cpp">
        <xsl:param name="component" as="element()" />

        <!-- Get the Namespace -->
        <xsl:variable name="namespaces" select="graphml:get_namespace($component)" />
        <xsl:variable name="tab" select="0" />

        <!-- Get the label -->
        <xsl:variable name="component_label" select="graphml:get_label($component)" />
        <xsl:variable name="class_type" select="concat(o:title_case($component_label), 'Int')" />
        <xsl:variable name="qualified_class_type" select="cpp:combine_namespaces(($namespaces, $class_type))" />

        <!-- Get the children required for generation -->
        <xsl:variable name="sub_ports" select="graphml:get_child_nodes_of_kind($component, 'SubscriberPort')" />
        <xsl:variable name="pub_ports" select="graphml:get_child_nodes_of_kind($component, 'PublisherPort')" />
        <xsl:variable name="req_ports" select="graphml:get_child_nodes_of_kind($component, 'RequesterPort')" />
        <xsl:variable name="rep_ports" select="graphml:get_child_nodes_of_kind($component, 'ReplierPort')" />
        <xsl:variable name="attributes" select="graphml:get_child_nodes_of_kind($component, 'Attribute')" />

        <!-- Preamble -->
        <xsl:value-of select="cpp:print_regen_version('component_functions.xsl', 'cdit:get_component_int_cpp', 0)" />

        <!-- Include Header -->
        <xsl:variable name="header_file" select="lower-case(concat($class_type, '.h'))" />
        <xsl:value-of select="cpp:include_local_header($header_file)" />
        <xsl:value-of select="o:nl(1)" />

        <!-- Constructor Definition -->
        <xsl:variable name="inherited_constructor" select="' : ::Component(name)'" />
        <xsl:value-of select="cpp:define_function('', $qualified_class_type, $class_type, cpp:const_ref_var_def('std::string', 'name'), concat($inherited_constructor, cpp:scope_start(0)))" />
            <!-- Attach In Event Port Callback Functions -->
            <xsl:for-each select="$sub_ports">
                <xsl:if test="position() = 1">
                    <xsl:value-of select="cpp:comment('Attach Subscriber Port Callbacks', $tab + 1)" />
                </xsl:if>
                <xsl:variable name="port_label" select="graphml:get_label(.)" />
                
                <xsl:variable name="qualified_function_name" select="cpp:combine_namespaces(($qualified_class_type, cdit:get_function_name(.)))" />
                <xsl:variable name="port_type" select="cpp:join_args(('void', cpp:get_qualified_type(graphml:get_port_aggregate(.)) ))" />

                <xsl:variable name="call_back" select="cpp:invoke_static_function('std', 'bind', cpp:join_args((cpp:ref_var($qualified_function_name), 'this', 'std::placeholders::_1')), '', 0)" />
                <xsl:variable name="args" select="cpp:join_args((o:wrap_dblquote($port_label), $call_back))" />
                
                <xsl:value-of select="cdit:comment_graphml_node(., $tab + 1)" />
                <xsl:value-of select="cpp:invoke_templated_static_function($port_type, 'AddCallback', $args, cpp:nl(), $tab + 1) " />
                <xsl:value-of select="if (position() = last()) then o:nl(1) else ''" />
            </xsl:for-each>

            <!-- Attach Replier Port Callback Functions -->
            <xsl:for-each select="$rep_ports">
                <xsl:if test="position() = 1">
                    <xsl:value-of select="cpp:comment('Attach Replier Port Callbacks', $tab + 1)" />
                </xsl:if>
                <xsl:variable name="port_label" select="graphml:get_label(.)" />
                <xsl:variable name="server_interface" select="graphml:get_port_aggregate(.)" />

                <xsl:variable name="qualified_function_name" select="cpp:combine_namespaces(($qualified_class_type, cdit:get_function_name(.)))" />
                <xsl:variable name="port_type" select="cdit:get_base_server_interface_type($server_interface)" />

                <xsl:variable name="call_back" select="cpp:invoke_static_function('std', 'bind', cpp:join_args((cpp:ref_var($qualified_function_name), 'this', 'std::placeholders::_1')), '', 0)" />
                <xsl:variable name="args" select="cpp:join_args((o:wrap_dblquote($port_label), $call_back))" />
                
                <xsl:value-of select="cdit:comment_graphml_node(., $tab + 1)" />
                <xsl:value-of select="cpp:invoke_templated_static_function($port_type, 'AddCallback', $args, cpp:nl(), $tab + 1) " />
                <xsl:value-of select="if (position() = last()) then o:nl(1) else ''" />
            </xsl:for-each>

            <!-- Define Attributes -->
            <xsl:for-each select="$attributes">
                <xsl:if test="position() = 1">
                    <xsl:value-of select="cpp:comment('Construct Attributes', $tab + 1)" />
                </xsl:if>
                <xsl:value-of select="cdit:define_attribute(., $tab + 1)" />
                <xsl:value-of select="if (position() = last()) then o:nl(1) else ''" />
            </xsl:for-each>
        <xsl:value-of select="cpp:scope_end(0)" />
        <xsl:value-of select="o:nl(1)" />

        <!-- Publisher Ports -->
        <xsl:for-each select="$pub_ports">
            <xsl:variable name="port_label" select="graphml:get_label(.)" />
            <xsl:variable name="function_name" select="cdit:get_function_name(.)" />
            <xsl:variable name="port_type" select="cpp:get_qualified_type(graphml:get_port_aggregate(.))" />
            <xsl:variable name="get_port" select="cpp:invoke_templated_static_function(cpp:templated_type('PublisherPort', $port_type), 'GetTypedPort', o:wrap_dblquote($port_label), '', 0) " />

            <!-- Define the Send function -->
            <xsl:value-of select="cpp:define_function('bool', $qualified_class_type, $function_name, cpp:const_ref_var_def($port_type, 'm'), cpp:scope_start(0))" />
                <xsl:value-of select="cpp:define_variable(cpp:auto(), 'p', $get_port, cpp:nl(), $tab + 1)" />
                <xsl:value-of select="cpp:if('p', cpp:scope_start(0), $tab + 1)" />
                    <xsl:variable name="send_request" select="cpp:invoke_function('p', cpp:arrow(), 'Send', 'm', 0)" />
                    <xsl:value-of select="cpp:return($send_request, $tab + 2)" />
                <xsl:value-of select="cpp:scope_end($tab + 1)" />
            <xsl:value-of select="cpp:return('false', $tab + 1)" />
            <xsl:value-of select="cpp:scope_end(0)" />
            <xsl:value-of select="o:nl(1)" />
        </xsl:for-each>

        <!-- Requester Ports -->
        <xsl:for-each select="$req_ports">
            <xsl:variable name="port_label" select="graphml:get_label(.)" />
            <xsl:variable name="server_interface" select="graphml:get_port_aggregate(.)" />
            <xsl:variable name="function_name" select="cdit:get_function_name(.)" />

            <xsl:variable name="reply_type" select="cdit:get_qualified_server_interface_reply_type($server_interface, 'base')" />
            <xsl:variable name="request_type" select="cdit:get_qualified_server_interface_request_type($server_interface, 'base')" />

            <xsl:variable name="return_type" select="cdit:get_requester_port_return_type($server_interface)" />
            <xsl:variable name="request_params" select="cdit:get_requester_port_parameters($server_interface)" />

            <xsl:variable name="port_type" select="cdit:get_base_server_interface_type($server_interface)" />
            
            <xsl:variable name="get_port" select="cpp:invoke_templated_static_function(cpp:templated_type('RequesterPort', $port_type), 'GetTypedPort', o:wrap_dblquote($port_label), '', 0) " />

            <xsl:variable name="failed_return">
                <xsl:choose>
                    <xsl:when test="$reply_type = 'void'">
                        <xsl:value-of select="'false'" />
                    </xsl:when>
                    <xsl:otherwise>
                        <xsl:variable name="args" select = "cpp:join_args(('false', concat($reply_type, '()')))" />
                        <xsl:value-of select="o:wrap_curly($args)" />
                    </xsl:otherwise>
                </xsl:choose>
            </xsl:variable>

            <xsl:variable name="send_args" as="xs:string*">
                <xsl:if test="$request_type != 'void'">
                    <xsl:sequence select="'m'" />
                </xsl:if>
                <xsl:sequence select="'timeout'" />
            </xsl:variable>

            <!-- Define the Send function -->
            <xsl:value-of select="cpp:define_function($return_type, $qualified_class_type, $function_name, $request_params, cpp:scope_start(0))" />
                <xsl:value-of select="cpp:define_variable(cpp:auto(), 'p', $get_port, cpp:nl(), $tab + 1)" />
                <xsl:value-of select="cpp:if('p', cpp:scope_start(0), $tab + 1)" />
                    <xsl:variable name="send_request" select="cpp:invoke_function('p', cpp:arrow(), 'SendRequest', cpp:join_args($send_args), 0)" />
                    <xsl:value-of select="cpp:return($send_request, $tab + 2)" />
                <xsl:value-of select="cpp:scope_end($tab + 1)" />
                <xsl:value-of select="cpp:return($failed_return, $tab + 1)" />
            <xsl:value-of select="cpp:scope_end(0)" />
            <xsl:value-of select="o:nl(1)" />
        </xsl:for-each>
        

        <!-- Define Attribute Functions -->
        <xsl:for-each select="$attributes">
            <xsl:value-of select="cdit:define_datatype_functions($component, ., $qualified_class_type)" />
        </xsl:for-each>
    </xsl:function>

    <!--
        Gets the Component Implementation Header
    -->
    <xsl:function name="cdit:get_component_impl_h">
        <xsl:param name="component_impl" as="element()" />

        <!-- Get the Component Definition -->
        <xsl:variable name="component_definition" select="graphml:get_definition($component_impl)" />

        <!-- Get the Namespace -->
        <xsl:variable name="namespaces" select="graphml:get_namespace($component_definition)" />
        <xsl:variable name="tab" select="count($namespaces)" />

        <!-- Get the label -->
        <xsl:variable name="component_label" select="graphml:get_label($component_definition)" />

        <!-- Compute the name of the Impl/Int classes -->
        <xsl:variable name="impl_class_type" select="concat(o:title_case($component_label), 'Impl')" />
        <xsl:variable name="int_class_type" select="concat(o:title_case($component_label), 'Int')" />
        
        <!-- Produce the Qualified Int Class -->
        <xsl:variable name="qualified_int_class_type" select="cpp:combine_namespaces(($namespaces, $int_class_type))" />
        
        <!-- Compute the define guard -->
        <xsl:variable name="define_guard_name" select="upper-case(o:join_list(('COMPONENT', $namespaces, $impl_class_type), '_'))" />
        
        <!-- Get the required aggregates, exclude Aggregates for the Interface -->
        <xsl:variable name="aggregates" select="cdit:get_required_aggregates($component_impl, true()) except cdit:get_required_aggregates($component_definition, false())" />
        <xsl:variable name="enums" select="cdit:get_required_enums($component_impl)" />

        <!-- Get the children required for generation -->
        <xsl:variable name="periodic_events" select="graphml:get_child_nodes_of_kind($component_impl, 'PeriodicPort')" />
        <xsl:variable name="sub_ports" select="graphml:get_child_nodes_of_kind($component_impl, 'SubscriberPortImpl')" />
        <xsl:variable name="rep_ports" select="graphml:get_child_nodes_of_kind($component_impl, 'ReplierPortImpl')" />

        <xsl:variable name="variables" select="graphml:get_child_nodes_of_kind($component_impl, 'Variable')" />
        <xsl:variable name="functions" select="graphml:get_child_nodes_of_kind($component_impl, 'Function')" />
        <xsl:variable name="worker_instances" select="graphml:get_worker_instances($component_impl)" />
        <xsl:variable name="custom_class_instances" select="graphml:get_custom_class_instances($component_impl)" />
        <xsl:variable name="headers" select="graphml:get_child_nodes_of_kind($component_impl, 'Header')" />

        <!-- Preamble -->
        <xsl:value-of select="cpp:print_regen_version('component_functions.xsl', 'cdit:get_component_impl_h', 0)" />

        <!-- Define Guard -->
        <xsl:value-of select="cpp:define_guard_start($define_guard_name)" />

        <!-- Include the Interface -->
        <xsl:value-of select="cpp:include_local_header(concat(lower-case($int_class_type), '.h'))" />
        <xsl:value-of select="o:nl(1)" />

        <!-- Include the Aggregates -->
        <xsl:value-of select="cdit:include_aggregate_headers('Base', $aggregates)" />

        <!-- Include the Eums -->
        <xsl:value-of select="cdit:include_enum_headers($enums)" />
        
        <!-- Include the Worker Headers -->
        <xsl:value-of select="cdit:include_worker_headers($worker_instances)" />
        
        <!-- Include the Custom Class Headers -->
        <xsl:value-of select="cdit:include_custom_class_headers($custom_class_instances)" />

        <!-- Inject the Code in Header of type [Header] -->
        <xsl:for-each select="$headers[graphml:get_data_value(., 'header_location') = 'Header']">
            <xsl:if test="position() = 1">
                <xsl:value-of select="cpp:comment('Injecting Header Code [header_location = Header]', 0)" />
            </xsl:if>
            <xsl:value-of select="cdit:generate_workflow_code(., ., 0)" />
            <xsl:value-of select="if (position() = last()) then o:nl(1) else ''" />
        </xsl:for-each>

        <!-- Define the Namespaces -->
        <xsl:value-of select="cpp:define_namespaces($namespaces)" />
        
        <!-- Class Declaration -->
        <xsl:value-of select="cdit:comment_graphml_node($component_impl, $tab)" />
        <xsl:value-of select="cpp:declare_class($impl_class_type, concat('public ::', $qualified_int_class_type), $tab)" />

        <!-- Public Declarations -->
        <xsl:value-of select="cpp:public($tab + 1)" />
        
        <!-- Constructor Declaration -->
        <xsl:value-of select="cpp:declare_function('', $impl_class_type, cpp:const_ref_var_def('std::string', 'name'), ';', $tab + 2)" />

        <!-- Protected -->
        <xsl:if test="count(($sub_ports, $periodic_events, $functions)) > 0">
            <xsl:value-of select="cpp:protected($tab + 1)" />
        </xsl:if>

        <!-- Periodic Event Declarations -->
        <xsl:for-each select="$periodic_events">
            <xsl:variable name="function_name" select="cdit:get_function_name(.)" />

            <xsl:value-of select="cdit:comment_graphml_node(., $tab + 2)" />
            <xsl:value-of select="cpp:declare_function('void', $function_name, '', ';', $tab + 2)" />
            <xsl:value-of select="if (position() = last()) then o:nl(1) else ''" />
        </xsl:for-each>

        <!-- In Event Port Declarations -->
        <xsl:for-each select="$sub_ports">
            <xsl:variable name="function_name" select="cdit:get_function_name(.)" />
            <xsl:variable name="port_type" select="cpp:get_qualified_type(graphml:get_port_aggregate(.))" />

            <xsl:value-of select="cdit:comment_graphml_node(., $tab + 2)" />
            <xsl:value-of select="cpp:declare_function('void', $function_name, cpp:ref_var_def($port_type, 'm'), ';', $tab + 2)" />
            <xsl:value-of select="if (position() = last()) then o:nl(1) else ''" />
        </xsl:for-each>

        <!-- Replier vent Port Declarations -->
        <xsl:for-each select="$rep_ports">
            <xsl:variable name="server_interface" select="graphml:get_port_aggregate(.)" />
            <xsl:variable name="function_name" select="cdit:get_function_name(.)" />

            <xsl:variable name="reply_type" select="cdit:get_qualified_server_interface_reply_type($server_interface, 'base')" />
            <xsl:variable name="request_type" select="cdit:get_qualified_server_interface_request_type($server_interface, 'base')" />

            <xsl:variable name="args">
                <xsl:if test="$request_type != 'void'">
                    <xsl:value-of select="cpp:ref_var_def($request_type, 'm')" />
                </xsl:if>
            </xsl:variable>

            <xsl:value-of select="cdit:comment_graphml_node(., $tab + 2)" />
            <xsl:value-of select="cpp:declare_function($reply_type, $function_name, $args, ';', $tab + 2)" />
            <xsl:value-of select="if (position() = last()) then o:nl(1) else ''" />
        </xsl:for-each>

        <!-- Function Declarations -->
        <xsl:for-each select="$functions">
            <xsl:value-of select="cdit:declare_function(., $tab + 2)" />
            <xsl:value-of select="if (position() = last()) then o:nl(1) else ''" />
        </xsl:for-each>

        <!-- Private -->
        <xsl:if test="count(($worker_instances, $variables)) > 0">
            <xsl:value-of select="cpp:private($tab + 1)" />
        </xsl:if>

        <!-- Worker Declarations -->
        <xsl:for-each select="$worker_instances">
            <xsl:value-of select="cdit:declare_class_variable(., $tab + 2)" />
            <xsl:value-of select="if (position() = last()) then o:nl(1) else ''" />
        </xsl:for-each>

        <!-- Custom Class Declarations -->
        <xsl:for-each select="$custom_class_instances">
            <xsl:value-of select="cdit:declare_class_variable(., $tab + 2)" />
            <xsl:value-of select="if (position() = last()) then o:nl(1) else ''" />
        </xsl:for-each>
        
        <!-- Variable Declarations -->
        <xsl:for-each select="$variables">
            <xsl:value-of select="cdit:declare_variable(., $tab + 2)" />
        </xsl:for-each>

        <!-- Inject the Code in Header of type [Class Declaration] -->
        <xsl:for-each select="$headers[graphml:get_data_value(., 'header_location') = 'Class Declaration']">
            <xsl:if test="position() = 1">
                <xsl:value-of select="o:nl(1)" />
                <xsl:value-of select="cpp:comment('Injecting Header Code [header_location = Class Declaration]', $tab + 2)" />
            </xsl:if>
            <xsl:value-of select="cdit:generate_workflow_code(., ., $tab + 2)" />
            <xsl:value-of select="if (position() = last()) then o:nl(1) else ''" />
        </xsl:for-each>

        <!-- End the Class Declaration -->
        <xsl:value-of select="cpp:scope_end($tab)" />

        <!-- End Namespaces -->
        <xsl:value-of select="cpp:close_namespaces($namespaces)" />

        <!-- Define Guard -->
        <xsl:value-of select="cpp:define_guard_end($define_guard_name)" />
    </xsl:function>

    <!--
        Gets the Class Cpp Code
    -->
    <xsl:function name="cdit:get_class_cpp">
        <xsl:param name="class" as="element()" />

        <!-- Get the Namespace -->
        <xsl:variable name="namespaces" select="graphml:get_namespace($class)" />
        <xsl:variable name="tab" select="0" />

        <!-- Get the label -->
        <xsl:variable name="class_label" select="graphml:get_label($class)" />

        <!-- Compute the name of the classes -->
        <xsl:variable name="class_type" select="o:title_case($class_label)" />
        <xsl:variable name="qualified_class_type" select="cpp:combine_namespaces(($namespaces, $class_type))" />
        
        <!-- Get the children required for generation -->
        <xsl:variable name="functions" select="graphml:get_child_nodes_of_kind($class, 'Function')" />
        <xsl:variable name="attributes" select="graphml:get_child_nodes_of_kind($class, 'Attribute')" />
        <xsl:variable name="worker_instances" select="graphml:get_worker_instances($class)" />
        <xsl:variable name="custom_class_instances" select="graphml:get_custom_class_instances($class)" />
        <xsl:variable name="headers" select="graphml:get_child_nodes_of_kind($class, 'Header')" />

        <!-- Preamble -->
        <xsl:value-of select="cpp:print_regen_version('component_functions.xsl', 'cdit:get_class_cpp', 0)" />

        <!-- Include Header -->
        <xsl:variable name="header_file" select="lower-case(concat($qualified_class_type, '.h'))" />
        <xsl:value-of select="cpp:include_local_header($header_file)" />
        <xsl:value-of select="o:nl(1)" />

        <!-- Inject the Code in Header of type [CPP] -->
        <xsl:for-each select="$headers[graphml:get_data_value(., 'header_location') = 'CPP']">
            <xsl:if test="position() = 1">
                <xsl:value-of select="cpp:comment('Injecting Header Code [header_location = CPP]', 0)" />
            </xsl:if>
            <xsl:value-of select="cdit:generate_workflow_code(., ., 0)" />
            <xsl:value-of select="if (position() = last()) then o:nl(1) else ''" />
        </xsl:for-each>

        <!-- Define the Constructor-->
        <xsl:variable name="inherited_constructor" select="concat(' : ::Worker(container, ', o:wrap_dblquote($qualified_class_type), ', inst_name, false)')" />
        <xsl:variable name="args" select="cpp:join_args((cpp:const_ref_var_def('::BehaviourContainer', 'container'), cpp:const_ref_var_def('std::string', 'inst_name')))" />
        <xsl:value-of select="cpp:define_function('', $qualified_class_type, $class_type, $args, concat($inherited_constructor, cpp:scope_start(0)))" />
            <!-- Define Worker Variables -->
            <xsl:for-each select="$worker_instances">
                <xsl:if test="position() = 1">
                    <xsl:value-of select="cpp:comment('Declare Worker Variables', $tab + 1)" />
                </xsl:if>
                <xsl:value-of select="cdit:define_class_variable(., $tab + 1)" />
                <xsl:value-of select="if (position() = last()) then o:nl(1) else ''" />
            </xsl:for-each>

            <!-- Define Custom Class Variables -->
            <xsl:for-each select="$custom_class_instances">
                <xsl:if test="position() = 1">
                    <xsl:value-of select="cpp:comment('Declare Custom Class Variables', $tab + 1)" />
                </xsl:if>
                <xsl:value-of select="cdit:define_class_variable(., $tab + 1)" />
                <xsl:value-of select="if (position() = last()) then o:nl(1) else ''" />
            </xsl:for-each>

            <!-- Define Attributes -->
            <xsl:for-each select="$attributes">
                <xsl:if test="position() = 1">
                    <xsl:value-of select="cpp:comment('Construct Attributes', $tab + 1)" />
                </xsl:if>
                <xsl:value-of select="cdit:define_attribute(., $tab + 1)" />
                <xsl:value-of select="if (position() = last()) then o:nl(1) else ''" />
            </xsl:for-each>
        <xsl:value-of select="cpp:scope_end(0)" />
        <xsl:value-of select="o:nl(1)" />

        <!-- Define Functions -->
        <xsl:for-each select="$functions">
            <xsl:value-of select="cdit:define_custom_function(., $qualified_class_type, 0)" />
        </xsl:for-each>

        <!-- Define Attribute Functions -->
        <xsl:for-each select="$attributes">
            <xsl:value-of select="cdit:define_datatype_functions($class, ., $qualified_class_type)" />
        </xsl:for-each>
    </xsl:function>

    <!--
        Gets the Class H Code
    -->
    <xsl:function name="cdit:get_class_h">
        <xsl:param name="class" as="element()" />

        <!-- Get the Namespace -->
        <xsl:variable name="namespaces" select="graphml:get_namespace($class)" />
        <xsl:variable name="tab" select="count($namespaces)" />

        <!-- Get the label -->
        <xsl:variable name="label" select="graphml:get_label($class)" />

        <!-- Compute the name of the Impl/Int classes -->
        <xsl:variable name="class_type" select="o:title_case($label)" />
        <xsl:variable name="qualified_class_type" select="cpp:combine_namespaces(($namespaces, $class_type))" />

        <!-- Compute the define guard -->
        <xsl:variable name="define_guard_name" select="upper-case(o:join_list(('CLASS', $namespaces, $class_type), '_'))" />

        <!-- Get the children required for generation -->
        <xsl:variable name="variables" select="graphml:get_child_nodes_of_kind($class, 'Variable')" />
        <xsl:variable name="functions" select="graphml:get_child_nodes_of_kind($class, 'Function')" />
        <xsl:variable name="worker_instances" select="graphml:get_worker_instances($class)" />
        <xsl:variable name="custom_class_instances" select="graphml:get_custom_class_instances($class)" />
        <xsl:variable name="attributes" select="graphml:get_child_nodes_of_kind($class, 'Attribute')" />

        <!-- Get the required aggregates -->
        <xsl:variable name="aggregates" select="cdit:get_required_aggregates($class, true())" />

        <!-- Get the children required for generation -->
        <xsl:variable name="class_instances" select="graphml:get_child_nodes_of_kind($class, 'ClassInstance')" />
        <xsl:variable name="worker_instances" select="graphml:get_worker_instances($class)" />
        <xsl:variable name="custom_class_instances" select="graphml:get_custom_class_instances($class)" />
        <xsl:variable name="headers" select="graphml:get_child_nodes_of_kind($class, 'Header')" />

        <!-- Preamble -->
        <xsl:value-of select="cpp:print_regen_version('component_functions.xsl', 'cdit:get_class_h', 0)" />

        <!-- Define Guard -->
        <xsl:value-of select="cpp:define_guard_start($define_guard_name)" />

        <!-- Include the Worker Header -->
        <xsl:value-of select="cpp:include_library_header(o:join_paths(('core', 'worker.h')))" />
        <xsl:value-of select="o:nl(1)" />

        <!-- Include the Aggregates -->
        <xsl:value-of select="cdit:include_aggregate_headers('Base', $aggregates)" />
        
        <!-- Include the Worker Headers -->
        <xsl:value-of select="cdit:include_worker_headers($worker_instances)" />
        
        <!-- Include the Custom Class Headers -->
        <xsl:value-of select="cdit:include_custom_class_headers($custom_class_instances)" />

        <!-- Inject the Code in Header of type [Header] -->
        <xsl:for-each select="$headers[graphml:get_data_value(., 'header_location') = 'Header']">
            <xsl:if test="position() = 1">
                <xsl:value-of select="cpp:comment('Injecting Header Code [header_location = Header]', 0)" />
            </xsl:if>
            <xsl:value-of select="cdit:generate_workflow_code(., ., 0)" />
            <xsl:value-of select="if (position() = last()) then o:nl(1) else ''" />
        </xsl:for-each>

        <!-- Define the Namespaces -->
        <xsl:value-of select="cpp:define_namespaces($namespaces)" />

        <!-- Class Declaration -->
        <xsl:value-of select="cdit:comment_graphml_node($class, $tab)" />
        <xsl:value-of select="cpp:declare_class($class_type, 'public ::Worker', $tab)" />

        <!-- Public Declarations -->
        <xsl:value-of select="cpp:public($tab + 1)" />

        <xsl:value-of select="cdit:comment_graphml_node($class, $tab)" />


        
        
        <!-- Constructor Declaration -->
        <xsl:variable name="args" select="cpp:join_args((cpp:const_ref_var_def('::BehaviourContainer', 'container'), cpp:const_ref_var_def('std::string', 'inst_name')))" />
        <xsl:value-of select="cpp:declare_function('', $class_type, $args, ';', $tab + 2)" />

        <!-- Function Declarations -->
        <xsl:for-each select="$functions">
            <xsl:value-of select="cdit:declare_function(., $tab + 2)" />
            <xsl:value-of select="if (position() = last()) then o:nl(1) else ''" />
        </xsl:for-each>

        <!-- Attributes Declarations -->
        <xsl:for-each select="$attributes">
            <xsl:value-of select="cdit:declare_datatype_functions(., $tab + 1)" />
        </xsl:for-each>

        <!-- Private -->
        <xsl:if test="count(($worker_instances, $custom_class_instances, $variables)) > 0">
            <xsl:value-of select="cpp:private($tab + 1)" />
        </xsl:if>

        <!-- Worker Declarations -->
        <xsl:for-each select="$worker_instances">
            <xsl:value-of select="cdit:declare_class_variable(., $tab + 2)" />
            <xsl:value-of select="if (position() = last()) then o:nl(1) else ''" />
        </xsl:for-each>

        <!-- Custom Class Declarations -->
        <xsl:for-each select="$custom_class_instances">
            <xsl:value-of select="cdit:declare_class_variable(., $tab + 2)" />
            <xsl:value-of select="if (position() = last()) then o:nl(1) else ''" />
        </xsl:for-each>

        <!-- Variable Declarations -->
        <xsl:for-each select="$variables">
            <xsl:value-of select="cdit:declare_variable(., $tab + 2)" />
        </xsl:for-each>

        <!-- Inject the Code in Header of type [Class Declaration] -->
        <xsl:for-each select="$headers[graphml:get_data_value(., 'header_location') = 'Class Declaration']">
            <xsl:if test="position() = 1">
                <xsl:value-of select="o:nl(1)" />
                <xsl:value-of select="cpp:comment('Injecting Header Code [header_location = Class Declaration]', $tab + 2)" />
            </xsl:if>
            <xsl:value-of select="cdit:generate_workflow_code(., ., $tab + 2)" />
            <xsl:value-of select="if (position() = last()) then o:nl(1) else ''" />
        </xsl:for-each>
        
        <!-- End the Class Declaration -->
        <xsl:value-of select="cpp:scope_end($tab)" />

        <!-- End Namespaces -->
        <xsl:value-of select="cpp:close_namespaces($namespaces)" />

        <!-- Define Guard -->
        <xsl:value-of select="cpp:define_guard_end($define_guard_name)" />
    </xsl:function>

    <!--
        Gets the Component Implementation Cpp Code
    -->
    <xsl:function name="cdit:get_component_impl_cpp">
        <xsl:param name="component_impl" as="element()" />
        
        <!-- Get the Component Definition -->
        <xsl:variable name="component_definition" select="graphml:get_definition($component_impl)" />

        <!-- Get the Namespace -->
        <xsl:variable name="namespaces" select="graphml:get_namespace($component_definition)" />
        <xsl:variable name="tab" select="count($namespaces)" />

        <!-- Get the label -->
        <xsl:variable name="component_label" select="graphml:get_label($component_definition)" />

        <!-- Compute the name of the Impl/Int classes -->
        <xsl:variable name="int_class_type" select="concat(o:title_case($component_label), 'Int')" />
        <xsl:variable name="impl_class_type" select="concat(o:title_case($component_label), 'Impl')" />

        <xsl:variable name="qualified_int_class_type" select="cpp:combine_namespaces(($namespaces, $int_class_type))" />
        <xsl:variable name="qualified_impl_class_type" select="cpp:combine_namespaces(($namespaces, $impl_class_type))" />


        <!-- Get the children required for generation -->
        <xsl:variable name="periodic_events" select="graphml:get_child_nodes_of_kind($component_impl, 'PeriodicPort')" />
        <xsl:variable name="sub_ports" select="graphml:get_child_nodes_of_kind($component_impl, 'SubscriberPortImpl')" />
        <xsl:variable name="rep_ports" select="graphml:get_child_nodes_of_kind($component_impl, 'ReplierPortImpl')" />
        <xsl:variable name="functions" select="graphml:get_child_nodes_of_kind($component_impl, 'Function')" />
        <xsl:variable name="worker_instances" select="graphml:get_worker_instances($component_impl)" />
        <xsl:variable name="custom_class_instances" select="graphml:get_custom_class_instances($component_impl)" />
        <xsl:variable name="headers" select="graphml:get_child_nodes_of_kind($component_impl, 'Header')" />

        <!-- Preamble -->
        <xsl:value-of select="cpp:print_regen_version('component_functions.xsl', 'cdit:get_component_impl_cpp', 0)" />

        <!-- Include Header -->
        <xsl:variable name="header_file" select="lower-case(concat($impl_class_type, '.h'))" />
        <xsl:value-of select="cpp:include_local_header($header_file)" />
        <xsl:value-of select="o:nl(1)" />

        <!-- Inject the Code in Header of type [CPP] -->
        <xsl:for-each select="$headers[graphml:get_data_value(., 'header_location') = 'CPP']">
            <xsl:if test="position() = 1">
                <xsl:value-of select="cpp:comment('Injecting Header Code [header_location = CPP]', 0)" />
            </xsl:if>
            <xsl:value-of select="cdit:generate_workflow_code(., ., 0)" />
            <xsl:value-of select="if (position() = last()) then o:nl(1) else ''" />
        </xsl:for-each>

        <!-- Define the Constructor-->
        <xsl:variable name="inherited_constructor" select="concat(' : ', $qualified_int_class_type, '(name)')" />
        <xsl:value-of select="cpp:define_function('', $qualified_impl_class_type, $impl_class_type, cpp:const_ref_var_def('std::string', 'name'), concat($inherited_constructor, cpp:scope_start(0)))" />
            
            <!-- Attach In Event Port Callback Functions -->
            <xsl:for-each select="$periodic_events">
                <xsl:if test="position() = 1">
                    <xsl:value-of select="cpp:comment('Attach Periodic Event Port Callbacks', $tab + 1)" />
                </xsl:if>
                <xsl:variable name="port_label" select="graphml:get_label(.)" />
                
                <xsl:variable name="qualified_function_name" select="cpp:combine_namespaces(($qualified_impl_class_type, cdit:get_function_name(.)))" />

                <xsl:variable name="call_back" select="cpp:invoke_static_function('std', 'bind', cpp:join_args((cpp:ref_var($qualified_function_name), 'this')), '', 0)" />
                <xsl:variable name="args" select="cpp:join_args((o:wrap_dblquote($port_label), $call_back))" />
                
                <xsl:value-of select="cdit:comment_graphml_node(., $tab + 1)" />
                <xsl:value-of select="cpp:invoke_static_function('', 'AddPeriodicEventCallback', $args, cpp:nl(), $tab + 1) " />
                <xsl:value-of select="if (position() = last()) then o:nl(1) else ''" />
            </xsl:for-each>


            <!-- Define Worker Variables -->
            <xsl:for-each select="$worker_instances">
                <xsl:if test="position() = 1">
                    <xsl:value-of select="cpp:comment('Declare Worker Variables', $tab + 1)" />
                </xsl:if>
                <xsl:value-of select="cdit:define_class_variable(., $tab + 1)" />
                <xsl:value-of select="if (position() = last()) then o:nl(1) else ''" />
            </xsl:for-each>

            <!-- Define Custom Class Variables -->
            <xsl:for-each select="$custom_class_instances">
                <xsl:if test="position() = 1">
                    <xsl:value-of select="cpp:comment('Declare Custom Class Variables', $tab + 1)" />
                </xsl:if>
                <xsl:value-of select="cdit:define_class_variable(., $tab + 1)" />
                <xsl:value-of select="if (position() = last()) then o:nl(1) else ''" />
            </xsl:for-each>
        <xsl:value-of select="cpp:scope_end(0)" />
        <xsl:value-of select="o:nl(1)" />

        <!-- In Event Port Callback Definitions -->
        <xsl:for-each select="$sub_ports">
            <xsl:value-of select="cdit:define_workload_function(., $qualified_impl_class_type)" />
        </xsl:for-each>

        <!-- In Event Port Declarations -->
        <xsl:for-each select="$rep_ports">
            <xsl:value-of select="cdit:define_workload_function(., $qualified_impl_class_type)" />
        </xsl:for-each>


        <!-- In Event Port Callback Definitions -->
        <xsl:for-each select="$periodic_events">
            <xsl:value-of select="cdit:define_workload_function(., $qualified_impl_class_type)" />
        </xsl:for-each>

        <!-- Handle Functions -->
        <xsl:for-each select="$functions">
            <xsl:value-of select="cdit:define_workload_function(., $qualified_impl_class_type)" />
        </xsl:for-each>
    </xsl:function>

    <xsl:function name="cdit:get_components_cmake">
        <xsl:param name="component_impls" as="element()*" />

        <xsl:variable name="sub_directories" as="xs:string*">
            <xsl:for-each select="$component_impls">
                <xsl:variable name="namespace" select="graphml:get_namespace(.)" />
                <xsl:variable name="label" select="graphml:get_label(.)" />
                <xsl:value-of select="lower-case(o:join_paths(($namespace, $label)))" />
            </xsl:for-each>
        </xsl:variable>

        <!-- Version Number -->
        <xsl:value-of select="cmake:print_regen_version('component_functions.xsl', 'cdit:get_components_cmake', 0)" />
        
        <xsl:value-of select="cmake:add_subdirectories($sub_directories)" />
    </xsl:function>


    <xsl:function name="cdit:define_custom_function">
        <xsl:param name="function" as="element()" />
        <xsl:param name="qualified_class_type" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />

        <xsl:variable name="name" select="cdit:get_function_name($function)" />
        <xsl:variable name="return_parameter" select="cdit:get_function_return_parameter_declarations($function)" />
        <xsl:variable name="input_parameters" select="cdit:get_function_input_parameter_declarations($function)" />
        
        <xsl:value-of select="cdit:comment_graphml_node($function, $tab)" />
        <xsl:value-of select="cpp:define_function($return_parameter, $qualified_class_type, $name, $input_parameters, cpp:scope_start(0))" />
        <xsl:value-of select="cdit:generate_workflow_code($function, $function, $tab + 1)" />
        <xsl:value-of select="cpp:scope_end(0)" />
        <xsl:value-of select="o:nl(1)" />
    </xsl:function>

    
    <xsl:function name="cdit:generate_vector_operation">
        <xsl:param name="function_call" as="element()"/>
        <xsl:param name="tab" as="xs:integer"/>

        <!-- Get the children of InputParameterGroupInstance-->
        <xsl:variable name="input_parameters" select="graphml:get_child_nodes(graphml:get_child_nodes_of_kind($function_call, 'InputParameterGroupInstance'))" />
        <xsl:variable name="return_parameters" select="graphml:get_child_nodes(graphml:get_child_nodes_of_kind($function_call, 'ReturnParameterGroupInstance'))" />
        <xsl:variable name="operation" select="graphml:get_label($function_call)" />

        <!-- The Vector is always the first input parameter-->
        <xsl:variable name="vector" select="$input_parameters[1]" />
        
        <!-- Construct a temp variable to store the result in -->
        <xsl:variable name="return_variable_name" select="cdit:get_unique_variable_name($return_parameters[1])" />
        
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
                    <!-- Run the Function -->
                    <xsl:value-of select="concat(o:t($tab), $vector_operation, cpp:nl())"/>
                </xsl:when>
                <xsl:otherwise>
                    <xsl:value-of select="cpp:define_variable(cpp:auto(), $return_variable_name, $vector_operation, cpp:nl(), $tab)"/>
                </xsl:otherwise>
            </xsl:choose>
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

    <xsl:function name="cdit:generate_requester_code">
        <xsl:param name="node" as="element()"/>
        <xsl:param name="tab" as="xs:integer"/>
      
        <xsl:variable name="timeout_parameter" select="graphml:get_child_nodes_of_kind($node, 'InputParameter')[1]" />
        <!--<xsl:variable name="success_parameter" select="graphml:get_child_nodes_of_kind($node, 'ReturnParameter')[1]" />-->

        <xsl:variable name="input_parameters" select="graphml:get_child_nodes(graphml:get_child_nodes_of_kind($node, 'InputParameterGroupInstance'))" />
        <xsl:variable name="return_parameters" select="graphml:get_child_nodes(graphml:get_child_nodes_of_kind($node, 'ReturnParameterGroupInstance'))" />

        <xsl:if test="count($input_parameters) = 1">
            <!-- Define a variable for the input parameter-->
            <xsl:value-of select="cdit:generate_workflow_code($input_parameters[1], $node, $tab)" />
        </xsl:if>
        
        <xsl:variable name="result_var" select="cdit:get_unique_variable_name($node)" />
        <xsl:variable name="message_var" select="cdit:get_unique_variable_name($input_parameters[1])" />
        
        <xsl:variable name="timeout" select="concat('std::chrono::milliseconds(', cdit:get_resolved_getter_function($timeout_parameter, false(), false()), ')')" />
        <xsl:variable name="req_function" select="cdit:get_function_name($node)" />

        <xsl:variable name="send_request" select="cpp:invoke_static_function('', $req_function, cpp:join_args(($message_var, $timeout)), '', 0)" />

        <xsl:value-of select="cpp:define_variable(cpp:auto(), $result_var, $send_request, cpp:nl(), $tab)" /> 
    </xsl:function>

    <xsl:function name="cdit:generate_publisherportimpl_code">
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
            <xsl:variable name="tx_function" select="cdit:get_function_name($node)" />
            <!-- Get the variable we will send -->
            <xsl:variable name="message_var" select="cdit:get_mutable_get_function($aggregate_instance, false())" />

            <!-- Call the function -->
            <xsl:value-of select="cpp:invoke_static_function('', $tx_function, $message_var, cpp:nl(), $tab)" />
        </xsl:if>
    </xsl:function>
    

    <xsl:function name="cdit:generate_member_code">
        <xsl:param name="node" as="element()"/>
        <xsl:param name="tab" as="xs:integer"/>

        <xsl:variable name="parent_node" select="graphml:get_parent_node($node)" />
        <xsl:variable name="parent_kind" select="graphml:get_kind($parent_node)" />


        <xsl:variable name="cpp_type" select="cpp:get_qualified_type($node)" />
        <xsl:variable name="var_label" select="cdit:get_variable_name($node)" />

        <xsl:variable name="getter" select="cdit:get_resolved_getter_function($node, false(), true())" />

        <xsl:if test="$getter != ''">
            <xsl:value-of select="cpp:define_variable($cpp_type, $var_label, $getter, cpp:nl(), $tab)" />
        </xsl:if>
    </xsl:function>


    <xsl:function name="cdit:generate_aggregateinstance_code">
        <xsl:param name="node" as="element()"/>
        <xsl:param name="tab" as="xs:integer"/>

        <xsl:variable name="aggregate" select="graphml:get_definition($node)" />

        <xsl:variable name="parent_node" select="graphml:get_parent_node($node)" />
        <xsl:variable name="parent_kind" select="graphml:get_kind($parent_node)" />

        <xsl:variable name="aggregate_type" select="cpp:get_aggregate_qualified_type($aggregate, 'base')" />
        <xsl:variable name="variable_name" select="cdit:get_variable_name($node)" />
        <xsl:value-of select="cpp:define_variable($aggregate_type, $variable_name, cdit:get_resolved_getter_function($node, false(), true()), cpp:nl(), $tab)" />

        
        <xsl:for-each select="graphml:get_child_nodes($node)">
            <xsl:variable name="setter_function" select="cdit:get_set_function(.)" />
            <xsl:variable name="value" select="cdit:get_resolved_getter_function(., true(), true())" />
            <xsl:if test="$value != ''">
                <xsl:value-of select="cpp:invoke_static_function('', $setter_function, $value, cpp:nl(), $tab)" />
            </xsl:if>
            <xsl:value-of select="cdit:generate_workflow_code(., $node, $tab)" />
        </xsl:for-each>
    </xsl:function>
    

    <xsl:function name="cdit:generate_for_loop_code">
        <xsl:param name="node" as="element()"/>
        <xsl:param name="tab" as="xs:integer"/>

        <xsl:variable name="required_args" select="3" />
        <xsl:variable name="children" select="graphml:get_child_nodes($node)" />

        

        <xsl:if test="count($children) >= $required_args">
            

            <xsl:variable name="var_set">
                <xsl:value-of select="cpp:define_variable('int', graphml:get_label($children[1]), cdit:get_resolved_getter_function($children[1], false(), false()), '', 0)" />
            </xsl:variable>

            <xsl:variable name="condition">
                <xsl:value-of select="cdit:generate_boolean_expression_code($children[2], 0, true())" />
            </xsl:variable>

            <xsl:variable name="value_setter">    
                <xsl:value-of select="cdit:generate_setter_code($children[3], 0, true())" />
            </xsl:variable>
            
            <xsl:variable name="statement">
                <xsl:value-of select="o:wrap_bracket(o:join_list(($var_set, $condition, $value_setter), '; '))" />
            </xsl:variable>
            
            <xsl:value-of select="concat(o:t($tab), 'for', $statement, cpp:scope_start(0))" />
            
            <xsl:value-of select="cdit:generate_scoped_variables($node, $tab + 1)" />
            
            <xsl:for-each select="$children">
                <xsl:if test="position() > $required_args">
                    <xsl:value-of select="cdit:generate_workflow_code(., $node, $tab + 1)" />
                </xsl:if>
            </xsl:for-each>

            <xsl:value-of select="cpp:scope_end($tab)" />
        </xsl:if>

    </xsl:function>

    <xsl:function name="cdit:generate_scoped_variables">
        <xsl:param name="node" as="element()"/>
        <xsl:param name="tab" as="xs:integer"/>

        <xsl:variable name="variables" select="graphml:get_child_nodes_of_kind($node, 'Variable')" />

        <xsl:for-each select="$variables">
            <xsl:if test="position() = 1">
                <xsl:value-of select="cpp:comment('Declaring scoped variables', $tab)" />
            </xsl:if>
        
            <xsl:variable name="cpp_type" select="cpp:get_qualified_type(.)" />
            <xsl:variable name="var_label" select="cdit:get_variable_name(.)" />
            <xsl:variable name="value" select="cdit:get_resolved_getter_function(., false(), true())" />
            
            <xsl:value-of select="cpp:define_variable($cpp_type, $var_label, $value, cpp:nl(), $tab)" />

            <xsl:if test="position() = last()">
                <xsl:value-of select="o:nl(1)" />
            </xsl:if>
        </xsl:for-each>

    </xsl:function>


    <xsl:function name="cdit:generate_while_loop_code">
        <xsl:param name="node" as="element()"/>
        <xsl:param name="tab" as="xs:integer"/>

        <xsl:variable name="required_args" select="1" />
        <xsl:variable name="children" select="graphml:get_child_nodes($node)" />


        <xsl:if test="count($children) >= $required_args">
            <xsl:variable name="condition">
                <xsl:value-of select="cdit:generate_boolean_expression_code($children[1], 0, true())" />
            </xsl:variable>
            
            <xsl:variable name="statement">
                <xsl:value-of select="o:wrap_bracket($condition)" />
            </xsl:variable>
            
            <xsl:value-of select="concat(o:t($tab), 'while', $statement, cpp:scope_start(0))" />
            <xsl:value-of select="cdit:generate_scoped_variables($node, $tab + 1)" />
            
            <xsl:for-each select="$children">
                <xsl:if test="position() > $required_args">
                    <xsl:value-of select="cdit:generate_workflow_code(., $node, $tab + 1)" />
                </xsl:if>
            </xsl:for-each>

            <xsl:value-of select="cpp:scope_end($tab)" />
        </xsl:if>

    </xsl:function>

    
    <xsl:function name="cdit:generate_boolean_expression_code">
        <xsl:param name="node" as="element()"/>
        <xsl:param name="tab" as="xs:integer"/>
        <xsl:param name="inline" as="xs:boolean"/>
        


        <xsl:variable name="statement">
            <xsl:variable name="input_parameters" select="graphml:get_child_nodes_of_kind($node, 'InputParameter')" />
            
            <xsl:choose>
                <xsl:when test="count(graphml:get_sources($node, 'Edge_Data')) = 1">
                    <xsl:value-of select ="cdit:get_resolved_getter_function($node, true(), false())" />
                </xsl:when>
                <xsl:when test="count($input_parameters) = 3">
                    <xsl:variable name="var_setter">
                        <xsl:value-of select="cdit:get_resolved_getter_function($input_parameters[1], true(), false())" />
                    </xsl:variable>

                    <xsl:variable name="operator">    
                        <xsl:value-of select="graphml:get_label($input_parameters[2])" />
                    </xsl:variable>

                    <xsl:variable name="value_setter">    
                        <xsl:value-of select="cdit:get_resolved_getter_function($input_parameters[3], false(), false())" />
                    </xsl:variable>
                    
                    <xsl:value-of select="concat($var_setter, ' ', $operator, ' ', $value_setter)" />
                </xsl:when>
            </xsl:choose>
        </xsl:variable>
        
        <xsl:choose>
            <xsl:when test="$inline">
                <xsl:value-of select="$statement" />
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="concat(o:t($tab), $statement, cpp:nl())" />
            </xsl:otherwise>
        </xsl:choose>
    </xsl:function>

    

    <xsl:function name="cdit:generate_if_condition_code">
        <xsl:param name="node" as="element()"/>
        <xsl:param name="tab" as="xs:integer"/>

        <xsl:variable name="kind" select="graphml:get_kind($node)" />

        <xsl:variable name="siblings" select="graphml:get_siblings($node)" />
        <xsl:variable name="sibling_count" select="count($siblings)" />
        <xsl:variable name="sibling_index" select="count($node/preceding-sibling::*)+1" />

        

        <xsl:variable name="prefix">
            <xsl:choose>
                <xsl:when test="$kind = 'IfCondition'">
                    <xsl:value-of select="'if'" />
                </xsl:when>
                <xsl:when test="$kind = 'ElseIfCondition'">
                    <xsl:value-of select="'else if'" />
                </xsl:when>
                <xsl:when test="$kind = 'ElseCondition'">
                    <xsl:value-of select="'else'" />
                </xsl:when>
            </xsl:choose>
        </xsl:variable>

    
        
        <xsl:for-each select="graphml:get_child_nodes($node)">
            <xsl:variable name="child_kind" select="graphml:get_kind(.)" />

            <xsl:if test="position() = 1">
                <xsl:variable name="statement">
                    <xsl:if test="$child_kind = 'BooleanExpression'">
                        <xsl:value-of select="o:wrap_bracket(cdit:generate_boolean_expression_code(., 0, true()))" />
                    </xsl:if>
                </xsl:variable> 
                <xsl:if test="$sibling_index = 1">
                    <!-- First If should Indent -->
                    <xsl:value-of select="o:t($tab)" />
                </xsl:if>
                <xsl:value-of select="concat($prefix, $statement, cpp:scope_start(0))" />

                <xsl:value-of select="cdit:generate_scoped_variables($node, $tab + 1)" />
            </xsl:if>


            <xsl:if test="position() > 1 or $kind = 'ElseCondition'">
                <xsl:value-of select="cdit:generate_workflow_code(., $node, $tab + 1)" />
            </xsl:if>

            <xsl:if test="position() = last()">
                <xsl:value-of select="concat(o:t($tab), '}')" />

                <xsl:if test="$sibling_index = $sibling_count">
                    <!-- First If should Indent -->
                    <xsl:value-of select="o:nl(1)" />
                </xsl:if>
            </xsl:if>
        </xsl:for-each>
    </xsl:function>

    

    <xsl:function name="cdit:generate_return_parameter_code">
        <xsl:param name="node" as="element()"/>
        <xsl:param name="tab" as="xs:integer"/>

        <xsl:variable name="children" select="graphml:get_child_nodes($node)" />
        <!-- An outeventportimpl should only have 1 child -->
        <xsl:if test="count($children) = 1">
            <xsl:value-of select="cdit:comment_graphml_node($node, $tab)" />
            
            <xsl:variable name="child" select="$children[1]" />
            
            <!-- Define the variable -->
            <xsl:value-of select="cdit:generate_workflow_code($child, $node, $tab)" />
            
            <!-- Get the variable we will send -->
            <xsl:variable name="message_var" select="cdit:get_variable_name($child)" />

            <!-- Call the function -->
            <xsl:value-of select="cpp:return($message_var, $tab)" />
        </xsl:if>
    </xsl:function>

    

    



        
    <xsl:function name="cdit:generate_function_call_code">
        <xsl:param name="node" as="element()"/>
        <xsl:param name="tab" as="xs:integer"/>

        <xsl:variable name="worker_function_inst" select="graphml:get_first_definition($node)" />
        <xsl:variable name="worker_inst" select="graphml:get_parent_node($worker_function_inst)" />
        <xsl:variable name="worker_function_def" select="graphml:get_definition($node)" />
        <xsl:variable name="worker_def" select="graphml:get_definition($worker_inst)" />
        <xsl:variable name="function_parent" select="graphml:get_parent_node($worker_function_def)" />
        <xsl:variable name="function_parent_kind" select="graphml:get_kind($function_parent)" />

        <xsl:variable name="is_worker" select="graphml:is_class_instance_worker($function_parent)" />

        

        <xsl:variable name="worker_type" select="graphml:get_data_value($worker_def, 'worker')" />

        <xsl:choose>
            <!-- Handle Vector Operations -->
            <xsl:when test="$worker_type = 'Vector_Operations'">
                <xsl:value-of select="cdit:generate_vector_operation($node, $tab)" />
            <!-- TODO -->
            </xsl:when>
            <xsl:otherwise>
                <xsl:variable name="operation">
                    <xsl:choose>
                        <xsl:when test="$is_worker">
                            <xsl:value-of select="graphml:get_data_value($worker_function_def, 'operation')" />
                        </xsl:when>
                        <xsl:otherwise>
                            <xsl:value-of select="concat('Fn_', graphml:get_data_value($worker_function_def, 'operation'))" />
                        </xsl:otherwise>
                    </xsl:choose>
                </xsl:variable>

                

                <xsl:variable name="arguments" as="xs:string*">
                    <xsl:choose>
                        <xsl:when test="$worker_type = 'Utility_Worker' and $operation = 'EvaluateComplexity'">
                            <xsl:sequence select="cdit:get_function_parameters($node, 'double')" />
                        </xsl:when>
                        <xsl:otherwise>
                            <xsl:sequence select="cdit:get_function_parameters($node, '')" />
                        </xsl:otherwise>
                    </xsl:choose>
                </xsl:variable>

                <xsl:variable name="variable_name" select="cdit:get_variable_name(cdit:get_function_return_parameters($node))" />

                <xsl:variable name="worker_variable">
                    <xsl:choose>
                        <!-- Invoking a function contained within the ComponentImpl doesn't need scope -->
                        <xsl:when test="$function_parent_kind = 'ComponentImpl'">
                            <xsl:value-of select="''" />
                        </xsl:when>
                        <!-- Invoking a function on a class needs to point against the class variable -->
                        <xsl:when test="$function_parent_kind = 'Class'">
                            <xsl:value-of select="cdit:get_variable_name($node)" />
                        </xsl:when>
                    </xsl:choose>
                </xsl:variable>

                <xsl:variable name="function" select="cpp:invoke_function($worker_variable, cpp:arrow(), $operation, $arguments , 0)" />

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
    


    <xsl:function name="cdit:generate_setter_code">
        <xsl:param name="node" as="element()"/>
        <xsl:param name="tab" as="xs:integer"/>
        <xsl:param name="inline" as="xs:boolean"/>

        <xsl:variable name="input_parameters" select="graphml:get_child_nodes_of_kind($node, 'InputParameter')" />

        <xsl:if test="count($input_parameters) = 3">
            <xsl:variable name="var_setter">    
                <xsl:value-of select="cdit:get_resolved_getter_function($input_parameters[1], true(), false())" />
            </xsl:variable>

            <xsl:variable name="operator">    
                <xsl:value-of select="graphml:get_label($input_parameters[2])" />
            </xsl:variable>

            <xsl:variable name="value_setter">    
                <xsl:value-of select="cdit:get_resolved_getter_function($input_parameters[3], false(), false())" />
            </xsl:variable>

            <xsl:variable name="statement">    
                <xsl:value-of select="concat($var_setter, ' ', $operator, ' ', $value_setter)" />
            </xsl:variable>
            <xsl:choose>
                <xsl:when test="$inline">
                    <xsl:value-of select="$statement" />
                </xsl:when>
                <xsl:otherwise>
                    <xsl:value-of select="concat(o:t($tab), $statement, cpp:nl())" />
                </xsl:otherwise>
            </xsl:choose>
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
        <xsl:variable name="parents_parent_node" select="graphml:get_parent_node($parent_node)" />
        <xsl:variable name="kind" select="graphml:get_kind($node)" />
        <xsl:variable name="parent_kind" select="graphml:get_kind($parent_node)" />
        <xsl:variable name="parents_parent_kind" select="graphml:get_kind($parents_parent_node)" />
    
        <xsl:variable name="getter">
            <xsl:choose>
                <xsl:when test="$kind = 'Variable'">
                    <xsl:value-of select="cdit:get_variable_name($node)" />
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
                <xsl:when test="$kind= 'AggregateInstance' or
                                $kind = 'MemberInstance' or
                                $kind = 'Member'">
                     <xsl:choose>
                        <xsl:when test="$parent_kind = 'ReturnParameterGroupInstance' and
                                        $parents_parent_kind = 'RequesterPortImpl'">
                            <xsl:value-of select="concat(cdit:get_variable_name($parents_parent_node), '.second')" />
                        </xsl:when>
                        <xsl:when test="$parent_kind = 'PublisherPortImpl' or
                                        $parent_kind = 'InputParameterGroup' or
                                        $parent_kind = 'InputParameterGroupInstance' or
                                        $parent_kind = 'ReturnParameterGroup' or
                                        $parent_kind = 'ReturnParameterGroupInstance'">
                            <!-- We have defined a variable based on the Top Level Aggregate -->
                            
                            <xsl:value-of select="cdit:get_variable_name($node)" />
                        </xsl:when>
                        <xsl:when test="$parent_kind = 'SubscriberPortImpl'" />
                        <xsl:when test="$parent_kind = 'Variable'" />
                        <xsl:otherwise>
                            <xsl:value-of select="cdit:get_inplace_getter($node, $mutable)" />
                        </xsl:otherwise>
                    </xsl:choose>
                </xsl:when>
                <xsl:when test="$kind = 'EnumMember'">
                    <xsl:value-of select="cdit:get_resolved_enum_member_type($node)" />
                </xsl:when>
                
                <xsl:when test="$kind = 'VectorInstance'">
                    <xsl:value-of select="cdit:get_inplace_getter($node, $mutable)" />
                </xsl:when>
                <xsl:when test="$kind = 'ReturnParameter'">
                    <xsl:choose>
                        <xsl:when test="$parent_kind = 'RequesterPortImpl'">
                            <!-- We have defined a variable based on the Top Level Aggregate -->
                            <xsl:variable name="is_void" select="cdit:does_function_return_void($parent_node)" />
                            <xsl:value-of select="concat(cdit:get_variable_name($parent_node), if($is_void) then '' else '.first')" />
                        </xsl:when>
                        <xsl:otherwise>
                            <xsl:value-of select="cdit:get_variable_name($node)" />
                        </xsl:otherwise>
                    </xsl:choose>
                </xsl:when>
                <xsl:when test="$kind = 'SubscriberPortImpl'">
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
        <xsl:variable name="parent_kind" select="graphml:get_kind(graphml:get_parent_node($node))" />

        <xsl:choose>
            <xsl:when test="$kind = 'FunctionCall'">
                <xsl:variable name="worker_function_inst" select="graphml:get_first_definition($node)" />
                <xsl:variable name="worker_inst" select="graphml:get_parent_node($worker_function_inst)" />

                <xsl:if test="not(graphml:is_descendant_of($worker_inst, $node))">
                    <xsl:value-of select="cdit:variablize_value(graphml:get_data_value($worker_inst, 'label'))"/>
                </xsl:if>

            </xsl:when>
            <xsl:when test="$kind = 'Variable' and ($parent_kind = 'Class' or $parent_kind = 'ComponentImpl')">
                <xsl:value-of select="cdit:variablize_value(graphml:get_label($node))"/>
            </xsl:when>
            <xsl:when test="$kind = 'AttributeImpl'">
                <xsl:value-of select="cdit:variablize_value(graphml:get_label($node))"/>
            </xsl:when>
            <xsl:when test="$kind = 'VoidType'">
                <xsl:value-of select="''"/>
            </xsl:when>

            <xsl:otherwise>
                <xsl:value-of select="cdit:get_unique_variable_name($node)" />
            </xsl:otherwise>
        </xsl:choose>
    </xsl:function>

    <xsl:function name="cdit:get_function_parameters" as="xs:string*">
        <xsl:param name="node" as="element()"/>
        <xsl:param name="cast_type" as="xs:string"/>

        <xsl:variable name="input_parameter_group" select="graphml:get_child_nodes_of_kind($node, ('InputParameterGroup', 'InputParameterGroupInstance'))" />

        <xsl:variable name="resolved_args" as="xs:string*">
            <xsl:for-each select="graphml:get_child_nodes($input_parameter_group[1])">
                <xsl:variable name="kind" select="graphml:get_kind(.)" />
                <xsl:variable name="value" select="graphml:get_data_value(., 'value')" />
                <xsl:variable name="getter" select="cdit:get_resolved_getter_function(., true(), false())" />

                <xsl:variable name="suffix">
                    <xsl:variable name="data_source" select="graphml:get_sources(., 'Edge_Data')" />

                    <!-- Use of Variadic Parameters, having string parameters should have a c_str casting -->
                    <xsl:if test="$kind = 'VariadicParameter' and count($data_source) > 0">
                        <xsl:variable name="type" select="graphml:get_type($data_source)" />
                        <xsl:if test="$type = 'String'">
                            <xsl:value-of select="'.c_str()'" />
                        </xsl:if>
                    </xsl:if>
                </xsl:variable>

                <xsl:variable name="resolved_getter" select="o:join_list(($getter, $suffix), '')" />

                <xsl:choose>
                    <!-- Handle Vector Operations -->
                    <xsl:when test="$kind = 'VariadicParameter' and $cast_type != ''">
                        <xsl:sequence select="cpp:cast($cast_type, $resolved_getter)"/>
                    </xsl:when>
                    <xsl:otherwise>
                        <xsl:sequence select="$resolved_getter"/>
                    </xsl:otherwise>
                </xsl:choose>
            </xsl:for-each>
        </xsl:variable>

        <xsl:sequence select="cpp:join_args($resolved_args)" />
    </xsl:function>

   

    <xsl:function name="cdit:get_function_return_parameters">
        <xsl:param name="node" as="element()"/>

        <xsl:variable name="return_parameter_group" select="graphml:get_child_nodes_of_kind($node, ('ReturnParameterGroup', 'ReturnParameterGroupInstance'))" />

        <xsl:for-each select="graphml:get_child_nodes($return_parameter_group[1])">
            <xsl:sequence select="."/>
        </xsl:for-each>
    </xsl:function>

    
    <xsl:function name="cdit:uses_branch" as="xs:boolean">
        <xsl:param name="node" as="element()"/>
        <xsl:variable name="kind" select="graphml:get_kind($node)" />
        <xsl:value-of select="$kind = 'BranchState' or $kind = 'WhileLoop'" />
    </xsl:function>

    <!--
        Gets the child nodes of the node provided. Sorted by 'index'
    -->
    <xsl:function name="cdit:get_workflow_child_nodes" as="element(gml:node)*">
        <xsl:param name="node" as="element(gml:node)" />

        <xsl:for-each select="graphml:get_child_nodes($node)">
            <xsl:variable name="row" select="graphml:get_data_value(., 'row')" />
            <xsl:if test="$row = '0' or $row = ''">
                <xsl:sequence select="." />
            </xsl:if>
        </xsl:for-each>
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
        
        <xsl:value-of select="cdit:comment_graphml_node($node, $tab)" />

        <xsl:choose>
            <xsl:when test="$kind = 'PeriodicPort' or $kind = 'SubscriberPortImpl' or $kind = 'IfStatement' or $kind = 'Function' or $kind = 'ReplierPortImpl'">
                <xsl:value-of select="cdit:generate_scoped_variables($node, $tab)" />
                <xsl:for-each select="cdit:get_workflow_child_nodes($node)">
                    <xsl:value-of select="cdit:generate_workflow_code(., $node, $tab)" />
                </xsl:for-each>
            </xsl:when>

            <xsl:when test="$kind = 'ForLoop'">
                <xsl:value-of select="cdit:generate_for_loop_code($node, $tab)" />
            </xsl:when>
            <xsl:when test="$kind = 'WhileLoop'">
                <xsl:value-of select="cdit:generate_while_loop_code($node, $tab)" />
            </xsl:when>
            <xsl:when test="$kind = 'IfCondition' or $kind = 'ElseIfCondition' or $kind = 'ElseCondition'">
                <xsl:value-of select="cdit:generate_if_condition_code($node, $tab)" />
            </xsl:when>
            <xsl:when test="$kind = 'BooleanExpression'">
                <xsl:value-of select="cdit:generate_boolean_expression_code($node, $tab, false())" />
            </xsl:when>
            <xsl:when test="$kind = 'Setter'">
                <xsl:value-of select="cdit:generate_setter_code($node, $tab, false())" />
            </xsl:when>
            <xsl:when test="$kind = 'FunctionCall'">
                <xsl:value-of select="cdit:generate_function_call_code($node, $tab)" />
            </xsl:when>
            <xsl:when test="$kind = 'RequesterPortImpl'">
                <xsl:value-of select="cdit:generate_requester_code($node, $tab)" />
            </xsl:when>
            <xsl:when test="$kind = 'ReturnParameterGroup' or $kind = 'ReturnParameterGroupInstance'">
                <xsl:value-of select="cdit:generate_return_parameter_code($node, $tab)" />
            </xsl:when>

            <xsl:when test="$kind = 'Code'">
                <xsl:value-of select="cdit:generate_cpp_code($node, $tab)" />
            </xsl:when>
            <xsl:when test="$kind = 'Header'">
                <xsl:value-of select="cdit:generate_header_code($node, $tab)" />
            </xsl:when>
            <xsl:when test="$kind = 'PublisherPortImpl'">
                <xsl:value-of select="cdit:generate_publisherportimpl_code($node, $tab)" />
            </xsl:when>
            
            <xsl:when test="$kind = 'AggregateInstance'">
                <xsl:value-of select="cdit:generate_aggregateinstance_code($node, $tab)" />
            </xsl:when>
            
            <!--
            <xsl:when test="$kind = 'Member' or $kind = 'MemberInstance'">
                <xsl:value-of select="cdit:generate_member_code($node, $tab)" />
            </xsl:when>
            <xsl:when test="$kind = 'Vector' or $kind = 'VectorInstance'">
                <xsl:value-of select="cdit:generate_member_code($node, $tab)" />
            </xsl:when>-->


            <xsl:when test="$kind = 'InputParameterGroupInstance'" />
            <xsl:when test="$kind = 'SubscriberPortImpl'" />
            <xsl:when test="$kind = 'MemberInstance'" />
            <xsl:when test="$kind = 'VectorInstance'" />
            <xsl:when test="$kind = 'EnumInstance'" />

            
            

            <xsl:otherwise>
                <xsl:value-of select="o:warning(('cdit:generate_workflow_code()', 'Node Kind:', o:wrap_quote($kind), 'Not Implemented'))" />
            </xsl:otherwise>
        </xsl:choose>
    </xsl:function>

    <xsl:function name="cdit:get_component_export">
        <xsl:param name="component_impl" as="element()" />

        <!-- Version Number -->
        <xsl:value-of select="cpp:print_regen_version('component_functions.xsl', 'cdit:get_component_export', 0)" />
        
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

    <xsl:function name="cdit:get_source_files" as="xs:string*">
        <xsl:param name="entity" as="element()" />

        <xsl:variable name="definition" select="graphml:get_definition($entity)" />
        <xsl:variable name="kind" select="graphml:get_kind($definition)" />
        <xsl:variable name="label" select="lower-case(graphml:get_label($definition))" />

        <xsl:choose>
          <xsl:when test="$kind = 'Component'">
            <xsl:sequence select = "concat($label, 'Impl.cpp')" />
            <xsl:sequence select = "concat($label, 'Int.cpp')" />
          </xsl:when>
          <xsl:when test="$kind = 'Class'">
            <xsl:sequence select = "concat($label, '.cpp')" />
          </xsl:when>
        </xsl:choose>
    </xsl:function>

    <!--
        Used by ComponentImpl/Class
    -->
    <xsl:function name="cdit:get_class_cmake">
        <xsl:param name="entity" as="element()" />

        <!-- Get the label and Namespace -->
        <xsl:variable name="definition" select="graphml:get_definition($entity)" />
        <xsl:variable name="kind" select="graphml:get_kind($definition)" />
        <xsl:variable name="label" select="lower-case(graphml:get_label($definition))" />
        <xsl:variable name="namespace" select="graphml:get_namespace($definition)" />

        <!-- Set the Project Name -->
        <xsl:variable name="module_lib_name" select="lower-case(o:join_list(($kind, $namespace, $label), '_'))" />
        <xsl:variable name="proj_name" select="$module_lib_name" />

        <xsl:variable name="binary_dir_var" select="cmake:current_binary_dir_var()" />
        <xsl:variable name="source_dir_var" select="cmake:current_source_dir_var()" />

        <!-- Preamble -->
        <xsl:value-of select="cmake:print_regen_version('component_functions.xsl', 'cdit:get_class_cmake', 0)" />

        <xsl:variable name="relative_path" select="cmake:get_relative_path(($kind, $namespace, $label))" />

        <xsl:variable name="top_source_dir" select="cmake:wrap_variable('TOP_SOURCE_DIR')" />
        <xsl:variable name="top_binary_dir" select="cmake:wrap_variable('TOP_BINARY_DIR')" />

        <!-- Set the TOP_SOURCE TOP_BINARY vars -->
        <xsl:value-of select="cmake:set_variable('TOP_SOURCE_DIR', o:join_paths(($source_dir_var, $relative_path)), 0)" />
        <xsl:value-of select="cmake:set_variable('TOP_BINARY_DIR', o:join_paths(($binary_dir_var, $relative_path)), 0)" />
        <xsl:value-of select="o:nl(1)" />
        
        <!-- Set the project name -->
        <xsl:value-of select="cmake:set_project_name($proj_name)" />

        <!-- Find re_core -->
        <xsl:value-of select="cmake:find_re_core_library()" />

        <!-- Get the source files -->
        <xsl:variable name="source_files" as="xs:string*">
            <xsl:choose>
            <xsl:when test="$kind = 'Component'">
                <xsl:sequence select = "concat($label, 'impl.cpp')" />
                <xsl:sequence select = "concat($label, 'int.cpp')" />
                <xsl:sequence select = "'libcomponentexport.cpp'" />
            </xsl:when>
            <xsl:when test="$kind = 'Class'">
                <xsl:sequence select = "concat($label, '.cpp')" />
            </xsl:when>
            </xsl:choose>
        </xsl:variable>

        <!-- Get the header files -->
        <xsl:variable name="header_files" as="xs:string*">
            <xsl:choose>
            <xsl:when test="$kind = 'Component'">
                <xsl:sequence select = "concat($label, 'impl.h')" />
                <xsl:sequence select = "concat($label, 'int.h')" />
            </xsl:when>
            <xsl:when test="$kind = 'Class'">
                <xsl:sequence select = "concat($label, '.h')" />
            </xsl:when>
            </xsl:choose>
        </xsl:variable>

        <!-- Set Source files -->
        <xsl:value-of select="concat('set(SOURCE', o:nl(1))" />
        <xsl:for-each select="$source_files">
            <xsl:value-of select="concat(o:t(1), o:join_paths(($source_dir_var, .)), o:nl(1))" />
        </xsl:for-each>
        <xsl:value-of select="concat(o:t(0), ')', o:nl(1))" />

        <!-- Set Header files -->
        <xsl:value-of select="concat('set(HEADERS', o:nl(1))" />
        <xsl:for-each select="$header_files">
            <xsl:value-of select="concat(o:t(1), o:join_paths(($source_dir_var, .)), o:nl(1))" />
        </xsl:for-each> 
        <xsl:value-of select="concat(o:t(0), ')', o:nl(1))" />

        <!-- Get the header files -->
        <xsl:variable name="library_type">
            <xsl:choose>
            <xsl:when test="$kind = 'Component'">
                <xsl:value-of select = "'MODULE'" />
            </xsl:when>
            <xsl:when test="$kind = 'Class'">
                <xsl:value-of select = "'SHARED'" />
            </xsl:when>
            </xsl:choose>
        </xsl:variable>

        <xsl:variable name="args" select="o:join_list((cmake:wrap_variable('SOURCE'), cmake:wrap_variable('HEADERS')), ' ')" />
        <xsl:value-of select="cmake:add_shared_library('PROJ_NAME', $library_type, $args)" />
        <xsl:value-of select="o:nl(1)" />

        
        <xsl:value-of select="cmake:target_include_directories('PROJ_NAME', $top_source_dir, 0)" />
        <xsl:value-of select="o:nl(1)" />

        <!-- Include Runtime Environment -->
        <xsl:value-of select="cmake:comment('Include/Link against runtime environment', 0)" />
        <xsl:value-of select="cmake:target_include_directories('PROJ_NAME', cmake:get_re_path('src'), 0)" />
        <xsl:value-of select="cmake:target_link_libraries('PROJ_NAME', cmake:wrap_variable('RE_CORE_LIBRARIES'), 0)" />
        <xsl:value-of select="o:nl(1)" />


        <!-- Include Required Aggregates -->
        <xsl:value-of select="cmake:target_link_aggregate_libraries($entity)" />

        <!-- Link Worker Libraries -->
        <xsl:value-of select="cmake:target_link_worker_libraries($entity)" />

        <!-- Link Custom Class Libraries -->
        <xsl:value-of select="cmake:target_link_custom_libraries($entity)" />
    </xsl:function>

</xsl:stylesheet>