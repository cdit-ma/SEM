<xsl:stylesheet version="2.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:xs="http://www.w3.org/2001/XMLSchema"
    xmlns:gml="http://graphml.graphdrawing.org/xmlns"
    xmlns:cdit="http://whatever"
    xmlns:o="http://whatever"
    >

    <xsl:import href="functions.xsl"/>

    
    <xsl:function name="cdit:get_component_int_h">
        <xsl:param name="component_root"/>

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

        <xsl:variable name="attributes" as="element()*" select="cdit:get_child_entities_of_kind($component_root, 'AttributeImpl')" />
        <xsl:variable name="periodicevents" as="element()*" select="cdit:get_child_entities_of_kind($component_root, 'PeriodicEvent')" />
        <xsl:variable name="outeventports" as="element()*" select="cdit:get_child_entities_of_kind($component_root, 'OutEventPortImpl')" />
        <xsl:variable name="ineventports" as="element()*" select="cdit:get_child_entities_of_kind($component_root, 'InEventPortImpl')" />

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

            <xsl:value-of select="o:nl()" />
            <xsl:value-of select="o:tabbed_cpp_comment(concat('PeriodicEvent ', o:square_wrap($id), ': ', $label), 2)" />
            <xsl:value-of select="concat(o:t(2), 'virtual void PE_', $label, '() = 0;', o:nl())" />
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
            <xsl:value-of select="concat(o:t(2), 'virtual void In_', $label, '(', $cpp_type, ' m) = 0;', o:nl())" />
        </xsl:for-each>

        <!-- OutEventPorts are declared and defined in the interface -->
        <xsl:for-each select="$outeventports">
            <xsl:variable name="id" select="cdit:get_node_id(.)" />
            <xsl:variable name="label" select="cdit:get_key_value(., 'label')" />
            <xsl:variable name="type" select="cdit:get_key_value(., 'type')" />
            <xsl:variable name="namespace" select="'::'" />
            <xsl:variable name="cpp_type" select="concat($namespace, $type)" />

            <xsl:value-of select="o:nl()" />
            <xsl:value-of select="o:tabbed_cpp_comment(concat('OutEventPort ', o:square_wrap($id), ': ', $label), 2)" />
            <xsl:value-of select="concat(o:t(2), 'void Out_', $label, '(', $cpp_type, ' m);', o:nl())" />
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

        <xsl:variable name="attributes" as="element()*" select="cdit:get_child_entities_of_kind($component_root, 'AttributeImpl')" />
        <xsl:variable name="periodicevents" as="element()*" select="cdit:get_child_entities_of_kind($component_root, 'PeriodicEvent')" />
        <xsl:variable name="outeventports" as="element()*" select="cdit:get_child_entities_of_kind($component_root, 'OutEventPortImpl')" />
        <xsl:variable name="ineventports" as="element()*" select="cdit:get_child_entities_of_kind($component_root, 'InEventPortImpl')" />

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
            <xsl:value-of select="concat(o:t(1), 'AddCallback(', o:dblquote_wrap($label), ', [this](BaseMessage* m) {PE_', $label, '(); delete m;});' ,o:nl())" />
        </xsl:for-each>

        <!-- Construct InEventPorts -->
        <xsl:for-each select="$ineventports">
            <xsl:variable name="id" select="cdit:get_node_id(.)" />
            <xsl:variable name="label" select="cdit:get_key_value(., 'label')" />
            <xsl:variable name="type" select="cdit:get_key_value(., 'type')" />
            <xsl:variable name="cpp_type" select="concat('::', $type)" />

            <xsl:value-of select="o:nl()" />
            <xsl:value-of select="o:tabbed_cpp_comment(concat('InEventPort ', o:square_wrap($id), ': ', $label), 1)" />
            <xsl:value-of select="concat(o:t(1), 'AddCallback(', o:dblquote_wrap($label), ', [this](BaseMessage* m) {auto t = ', o:bracket_wrap(concat($cpp_type, '*')), 'm; In_', $label,'(*t); delete m;});' ,o:nl())" />
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


</xsl:stylesheet>