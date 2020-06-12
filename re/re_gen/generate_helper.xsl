<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="2.0" 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:xs="http://www.w3.org/2001/XMLSchema"
    xmlns:gml="http://graphml.graphdrawing.org/xmlns"
    xmlns:o="http://github.com/cdit-ma/re_gen/o"
    xmlns:cdit="http://github.com/cdit-ma/re_gen/cdit"
    xmlns:graphml="http://github.com/cdit-ma/re_gen/graphml"
    xmlns:cmake="http://github.com/cdit-ma/re_gen/cmake"
    xmlns:cpp="http://github.com/cdit-ma/re_gen/cpp"
    xmlns:idl="http://github.com/cdit-ma/re_gen/idl"
    exclude-result-prefixes="gml">

    <xsl:output method="text" omit-xml-declaration="yes" indent="yes" standalone="no" />

    <!-- Load in Functions -->
    <xsl:import href="libraries/general_functions.xsl"/>
    <xsl:import href="libraries/cpp_functions.xsl"/>
    <xsl:import href="libraries/cmake_functions.xsl"/>
    <xsl:import href="libraries/graphml_functions.xsl"/>
    <xsl:import href="libraries/cdit_functions.xsl"/> 
    <xsl:import href="libraries/cdit_cmake_functions.xsl"/>
    <xsl:import href="libraries/component_functions.xsl"/>
    <xsl:import href="libraries/proto_functions.xsl"/>
    <xsl:import href="libraries/idl_functions.xsl"/>
    <xsl:import href="libraries/datatype_functions.xsl"/>

    <xsl:param name="library_prefix" as="xs:string" select="''" />
    <xsl:variable name="debug_mode" as="xs:boolean" select="true()" />

    <xsl:function name="o:var">
        <xsl:param name="var_name" as="xs:string" />
        <xsl:value-of select="concat('$', o:wrap_curly($var_name))" />
    </xsl:function>

    <xsl:template match="/*">
        <!-- All outputs for cpp_functions -->
        <xsl:result-document href="{o:write_file('cpp_functions.cpp')}">
            <!-- cpp:comment -->
            <xsl:value-of select="cpp:comment('cpp:comment', 0)" />
            <xsl:value-of select="cpp:comment(o:var('text'), 0)" />
            <xsl:value-of select="cpp:comment(o:join_list((o:var('text_1'), o:var('text_2')), o:nl(1)), 1)" />
            <xsl:value-of select="o:nl(1)" />

            <!-- cpp:comment_inline -->
            <xsl:value-of select="cpp:comment('cpp:comment_inline', 0)" />
            <xsl:value-of select="cpp:comment_inline(o:var('text'))" />
            <xsl:value-of select="o:nl(2)" />

            <!-- cpp:warning -->
            <xsl:value-of select="cpp:comment('cpp:warning', 0)" />
            <xsl:value-of select="cpp:warning(o:var('text'), 0)" />
            <xsl:value-of select="o:nl(2)" />

            <!-- cpp:arrow / cpp:dot / cpp:void -->
            <xsl:value-of select="cpp:comment('cpp:arrow / cpp:dot / cpp:void', 0)" />
            <xsl:value-of select="o:join_list((cpp:arrow(), cpp:dot(), cpp:void(), o:nl(1)), ' ')" />
            <xsl:value-of select="o:nl(1)" />

            <!-- cpp:wrap_template -->
            <xsl:value-of select="cpp:comment('cpp:wrap_template', 0)" />
            <xsl:value-of select="cpp:wrap_template(o:var('type'))" />
            <xsl:value-of select="o:nl(1)" />

            <!-- cpp:include_library_header -->
            <xsl:value-of select="cpp:comment('cpp:include_library_header', 0)" />
            <xsl:value-of select="cpp:include_library_header(o:var('library_path'))" />
            <xsl:value-of select="o:nl(1)" />

            <!-- cpp:include_local_header -->
            <xsl:value-of select="cpp:comment('cpp:include_local_header', 0)" />
            <xsl:value-of select="cpp:include_local_header(o:var('library_path'))" />
            <xsl:value-of select="o:nl(1)" />

            <!-- cpp:include_local_header / cpp:define_guard_end -->
            <xsl:value-of select="cpp:comment('cpp:define_guard_start / cpp:define_guard_end', 0)" />
            <xsl:value-of select="cpp:define_guard_start(o:var('class_name'))" />
            <xsl:value-of select="cpp:define_guard_end(o:var('class_name'))" />
            <xsl:value-of select="o:nl(1)" />

            <!-- cpp:define / cpp:undef -->
            <xsl:value-of select="cpp:comment('cpp:define / cpp:undef', 0)" />
            <xsl:value-of select="cpp:define(o:var('define_name'), o:var('define_val'))" />
            <xsl:value-of select="cpp:undef(o:var('define_name'))" />
            <xsl:value-of select="o:nl(1)" />

            <!-- cpp:declare_function -->
            <xsl:value-of select="cpp:comment('cpp:declare_function', 0)" />
            <xsl:value-of select="cpp:declare_function(o:var('return_type'), o:var('function_name'), o:var('parameters'), o:var('suffix'), 0)" />
            <xsl:value-of select="o:nl(1)" />

            <!-- cpp:declare_templated_function -->
            <xsl:value-of select="cpp:comment('cpp:declare_templated_function', 0)" />
            <xsl:value-of select="cpp:declare_templated_function(o:var('template_type_name'), o:var('return_type'), o:var('function_name'), o:var('parameters'), o:var('suffix'), 0)" />
            <xsl:value-of select="o:nl(1)" />

            <!-- cpp:declare_templated_function_specialisation -->
            <xsl:value-of select="cpp:comment('cpp:declare_templated_function_specialisation', 0)" />
            <xsl:value-of select="cpp:declare_templated_function_specialisation(o:var('template_type_name'), o:var('return_type'), o:var('function_name'), o:var('parameters'), o:var('suffix'), 0)" />
            <xsl:value-of select="o:nl(1)" />

            <!-- cpp:declare_templated_class_function_specialisation -->
            <xsl:value-of select="cpp:comment('cpp:declare_templated_class_function_specialisation', 0)" />
            <xsl:value-of select="cpp:declare_templated_class_function_specialisation(o:var('template_type_name'), o:var('return_type'), o:var('class_name'), o:var('function_name'), o:var('parameters'), o:var('suffix'), 0)" />
            <xsl:value-of select="o:nl(1)" />

            <!-- cpp:invoke_templated_static_function -->
            <xsl:value-of select="cpp:comment('cpp:invoke_templated_static_function', 0)" />
            <xsl:value-of select="cpp:invoke_templated_static_function(o:var('template_type_name'), o:var('function_name'), o:var('parameters'), o:var('suffix'), 0)" />
            <xsl:value-of select="o:nl(2)" />

            <!-- cpp:invoke_templated_class_static_function -->
            <xsl:value-of select="cpp:comment('cpp:invoke_templated_class_static_function', 0)" />
            <xsl:value-of select="cpp:invoke_templated_class_static_function(o:var('template_type_name'), o:var('class_name'), o:var('function_name'), o:var('parameters'), o:var('suffix'), 0)" />
            <xsl:value-of select="o:nl(2)" />

            <!-- cpp:define_function -->
            <xsl:value-of select="cpp:comment('cpp:define_function', 0)" />
            <xsl:value-of select="cpp:define_function(o:var('return_type'), o:var('namespace'), o:var('function_name'), o:var('parameters'), o:var('suffix'))" />
            <xsl:value-of select="o:nl(2)" />

            <!-- cpp:define_templated_function_specialisation -->
            <xsl:value-of select="cpp:comment('cpp:define_templated_function_specialisation', 0)" />
            <xsl:value-of select="cpp:define_templated_function_specialisation(o:var('template_type_name'), o:var('return_type'), o:var('namespace'), o:var('function_name'), o:var('parameters'), o:var('suffix'))" />
            <xsl:value-of select="o:nl(2)" />

            <!-- cpp:declare_variable -->
            <xsl:value-of select="cpp:comment('cpp:declare_variable', 0)" />
            <xsl:value-of select="cpp:declare_variable(o:var('variable_type'), o:var('variable_name'), o:var('suffix'), 0)" />
            <xsl:value-of select="o:nl(2)" />

            <!-- cpp:define_variable -->
            <xsl:value-of select="cpp:comment('cpp:define_variable', 0)" />
            <xsl:value-of select="cpp:define_variable(o:var('variable_type'), o:var('variable_name'), o:var('value'), o:var('suffix'), 0)" />
            <xsl:value-of select="o:nl(2)" />

            <!-- cpp:shared_ptr -->
            <xsl:value-of select="cpp:comment('cpp:shared_ptr', 0)" />
            <xsl:value-of select="cpp:shared_ptr(o:var('type'))" />
            <xsl:value-of select="o:nl(2)" />

            <!-- cpp:weak_ptr -->
            <xsl:value-of select="cpp:comment('cpp:weak_ptr', 0)" />
            <xsl:value-of select="cpp:weak_ptr(o:var('type'))" />
            <xsl:value-of select="o:nl(2)" />

            <!-- cpp:make_shared_ptr -->
            <xsl:value-of select="cpp:comment('cpp:make_shared_ptr', 0)" />
            <xsl:value-of select="cpp:make_shared_ptr(o:var('type'), o:var('args'))" />
            <xsl:value-of select="o:nl(2)" />

            <!-- cpp:namespace_start / cpp:namespace_end -->
            <xsl:value-of select="cpp:comment('cpp:namespace_start / cpp:namespace_end', 0)" />
            <xsl:value-of select="cpp:namespace_start(o:var('namespace'), 0)" />
            <xsl:value-of select="cpp:namespace_end(o:var('namespace'), 0)" />
            <xsl:value-of select="o:nl(2)" />

            <!-- cpp:forward_declare_class -->
            <xsl:value-of select="cpp:comment('cpp:forward_declare_class', 0)" />
            <xsl:value-of select="cpp:forward_declare_class((o:var('namespace_1'), o:var('namespace_2')), o:var('class_name'), 0)" />
            <xsl:value-of select="o:nl(2)" />

            <!-- cpp:ref_var -->
            <xsl:value-of select="cpp:comment('cpp:ref_var', 0)" />
            <xsl:value-of select="cpp:ref_var(o:var('var'))" />
            <xsl:value-of select="o:nl(2)" />

            <!-- cpp:ref_var_def -->
            <xsl:value-of select="cpp:comment('cpp:ref_var_def', 0)" />
            <xsl:value-of select="cpp:ref_var_def(o:var('type'), o:var('value'))" />
            <xsl:value-of select="o:nl(2)" />

            <!-- cpp:var_def -->
            <xsl:value-of select="cpp:comment('cpp:var_def', 0)" />
            <xsl:value-of select="cpp:var_def(o:var('type'), o:var('value'))" />
            <xsl:value-of select="o:nl(2)" />

            <!-- cpp:var_def -->
            <xsl:value-of select="cpp:comment('cpp:const_ref_var_def', 0)" />
            <xsl:value-of select="cpp:const_ref_var_def(o:var('type'), o:var('value'))" />
            <xsl:value-of select="o:nl(2)" />

            <!-- cpp:pointer_var_def -->
            <xsl:value-of select="cpp:comment('cpp:pointer_var_def', 0)" />
            <xsl:value-of select="cpp:pointer_var_def(o:var('type'), o:var('value'))" />
            <xsl:value-of select="o:nl(2)" />

            <!-- cpp:unique_ptr -->
            <xsl:value-of select="cpp:comment('cpp:unique_ptr', 0)" />
            <xsl:value-of select="cpp:unique_ptr(o:var('type'))" />
            <xsl:value-of select="o:nl(2)" />

            <!-- cpp:unique_ptr_def -->
            <xsl:value-of select="cpp:comment('cpp:unique_ptr_def', 0)" />
            <xsl:value-of select="cpp:unique_ptr_def(o:var('type'), o:var('value'))" />
            <xsl:value-of select="o:nl(2)" />

            <!-- cpp:unique_ptr_object -->
            <xsl:value-of select="cpp:comment('cpp:unique_ptr_object', 0)" />
            <xsl:value-of select="cpp:unique_ptr_object((o:var('namespace1'), o:var('namespace2'), o:var('type')), o:var('value'))" />
            <xsl:value-of select="o:nl(2)" />

            <!-- cpp:dereference_var -->
            <xsl:value-of select="cpp:comment('cpp:dereference_var', 0)" />
            <xsl:value-of select="cpp:dereference_var(o:var('var'))" />
            <xsl:value-of select="o:nl(2)" />

            <!-- cpp:const_ref_auto -->
            <xsl:value-of select="cpp:comment('cpp:const_ref_auto', 0)" />
            <xsl:value-of select="cpp:const_ref_auto()" />
            <xsl:value-of select="o:nl(2)" />

            <!-- cpp:scope_start + cpp:scope_end -->
            <xsl:value-of select="cpp:comment('cpp:scope_start + cpp:scope_end', 0)" />
            <xsl:value-of select="cpp:scope_start(0)" />
            <xsl:value-of select="cpp:scope_end(0)" />
            <xsl:value-of select="o:nl(2)" />

            <!-- cpp:nl -->
            <xsl:value-of select="cpp:comment('cpp:nl', 0)" />
            <xsl:value-of select="cpp:nl()" />
            <xsl:value-of select="o:nl(1)" />

            <!-- cpp:invoke_function -->
            <xsl:value-of select="cpp:comment('cpp:invoke_function', 0)" />
            <xsl:value-of select="cpp:invoke_function(o:var('obj'), o:var('operator'), o:var('function_name'), o:var('parameters'), 0)" />
            <xsl:value-of select="o:nl(2)" />

            <!-- cpp:invoke_static_function -->
            <xsl:value-of select="cpp:comment('cpp:invoke_static_function', 0)" />
            <xsl:value-of select="cpp:invoke_static_function((o:var('namespace_1'),o:var('namespace_2')), o:var('function_name'), o:var('parameters'), o:var('suffix'), 0)" />
            <xsl:value-of select="o:nl(2)" />

            <!-- cpp:delete -->
            <xsl:value-of select="cpp:comment('cpp:delete', 0)" />
            <xsl:value-of select="cpp:delete(o:var('var'), 0)" />
            <xsl:value-of select="o:nl(1)" />

            <!-- cpp:return -->
            <xsl:value-of select="cpp:comment('cpp:return', 0)" />
            <xsl:value-of select="cpp:return(o:var('var'), 0)" />
            <xsl:value-of select="o:nl(1)" />

            <!-- cpp:combine_namespaces -->
            <xsl:value-of select="cpp:comment('cpp:combine_namespaces', 0)" />
            <xsl:value-of select="cpp:combine_namespaces((o:var('namespace_1'),o:var('namespace_2')))" />
            <xsl:value-of select="o:nl(2)" />

            <!-- cpp:new_object -->
            <xsl:value-of select="cpp:comment('cpp:new_object', 0)" />
            <xsl:value-of select="cpp:new_object((o:var('namespace_1'), o:var('namespace_2'), o:var('function')), o:var('parameters'))" />
            <xsl:value-of select="o:nl(2)" />

            <!-- cpp:for -->
            <xsl:value-of select="cpp:comment('cpp:for', 0)" />
            <xsl:value-of select="cpp:for(o:var('first'), o:var('second'), o:var('third'), o:var('suffix'), 0)" />
            <xsl:value-of select="o:nl(2)" />

            <!-- cpp:for_each -->
            <xsl:value-of select="cpp:comment('cpp:for_each', 0)" />
            <xsl:value-of select="cpp:for_each(o:var('first'), o:var('second'), o:var('suffix'), 0)" />
            <xsl:value-of select="o:nl(2)" />

            <!-- cpp:compare -->
            <xsl:value-of select="cpp:comment('cpp:compare', 0)" />
            <xsl:value-of select="cpp:compare(o:var('val1'), o:var('operator'), o:var('val2'))" />
            <xsl:value-of select="o:nl(2)" />

            <!-- cpp:compare_lt -->
            <xsl:value-of select="cpp:comment('cpp:compare_lt', 0)" />
            <xsl:value-of select="cpp:compare_lt(o:var('val1'), o:var('val2'))" />
            <xsl:value-of select="o:nl(2)" />

            <!-- cpp:compare_gt -->
            <xsl:value-of select="cpp:comment('cpp:compare_gt', 0)" />
            <xsl:value-of select="cpp:compare_gt(o:var('val1'), o:var('val2'))" />
            <xsl:value-of select="o:nl(2)" />

            <!-- cpp:plusplus -->
            <xsl:value-of select="cpp:comment('cpp:plusplus', 0)" />
            <xsl:value-of select="cpp:plusplus()" />
            <xsl:value-of select="o:nl(2)" />

            <!-- cpp:increment -->
            <xsl:value-of select="cpp:comment('cpp:increment', 0)" />
            <xsl:value-of select="cpp:increment(o:var('val'))" />
            <xsl:value-of select="o:nl(2)" />

            <!-- cpp:array_get -->
            <xsl:value-of select="cpp:comment('cpp:array_get', 0)" />
            <xsl:value-of select="cpp:array_get(o:var('array'), o:var('index'))" />
            <xsl:value-of select="o:nl(2)" />

            <!-- cpp:static_cast -->
            <xsl:value-of select="cpp:comment('cpp:static_cast', 0)" />
            <xsl:value-of select="cpp:static_cast(o:var('type'), o:var('value'))" />
            <xsl:value-of select="o:nl(2)" />

            <!-- cpp:static_cast -->
            <xsl:value-of select="cpp:comment('cpp:cast', 0)" />
            <xsl:value-of select="cpp:cast(o:var('type'), o:var('value'))" />
            <xsl:value-of select="o:nl(2)" />

            <!-- cpp:declare_class -->
            <xsl:value-of select="cpp:comment('cpp:declare_class', 0)" />
            <xsl:value-of select="cpp:declare_class(o:var('class_name'), o:var('sub_classes'), 0)" />
            <xsl:value-of select="o:nl(1)" />

            <!-- cpp:public -->
            <xsl:value-of select="cpp:comment('cpp:public', 0)" />
            <xsl:value-of select="cpp:public(0)" />
            <xsl:value-of select="o:nl(1)" />

            <!-- cpp:private -->
            <xsl:value-of select="cpp:comment('cpp:private', 0)" />
            <xsl:value-of select="cpp:private(0)" />
            <xsl:value-of select="o:nl(1)" />

            <!-- cpp:protected -->
            <xsl:value-of select="cpp:comment('cpp:protected', 0)" />
            <xsl:value-of select="cpp:protected(0)" />
            <xsl:value-of select="o:nl(1)" />

            <!-- cpp:join_args -->
            <xsl:value-of select="cpp:comment('cpp:join_args', 0)" />
            <xsl:value-of select="cpp:join_args((o:var('args1'), o:var('args2'), o:var('args3')))" />
            <xsl:value-of select="o:nl(2)" />

            <!-- cpp:templated_type -->
            <xsl:value-of select="cpp:comment('cpp:templated_type', 0)" />
            <xsl:value-of select="cpp:templated_type(o:var('class'), o:var('template_type'))" />
            <xsl:value-of select="o:nl(2)" />

            <!-- cpp:enum -->
            <xsl:value-of select="cpp:comment('cpp:enum', 0)" />
            <xsl:value-of select="cpp:enum(o:var('label'), 0)" />

            <!-- cpp:enum_value -->
            <xsl:value-of select="cpp:comment('cpp:enum_value', 1)" />
            <xsl:value-of select="cpp:enum_value(o:var('label'), 0, false(), 1)" />

            <!-- cpp:enum_value (last) -->
            <xsl:value-of select="cpp:comment('cpp:enum_value (last)', 1)" />
            <xsl:value-of select="cpp:enum_value(o:var('label_2'), 1, true(), 1)" />
            
            <!-- cpp:enum_value_end -->
            <xsl:value-of select="cpp:enum_end(0)" />
            <xsl:value-of select="o:nl(1)" />

            <!-- cpp:switch -->
            <xsl:value-of select="cpp:comment('cpp:switch', 0)" />
            <xsl:value-of select="cpp:switch(o:var('label'), 0)" />

            <!-- cpp:switch_case -->
            <xsl:value-of select="cpp:comment('cpp:switch_case', 1)" />
            <xsl:value-of select="cpp:switch_case(o:var('label'), 1)" />
            <!-- cpp:break -->
            <xsl:value-of select="cpp:break(2)" />
            <!-- cpp:switch_case_end -->
            <xsl:value-of select="cpp:switch_case_end(1)" />
            <xsl:value-of select="o:nl(1)" />

            <!-- cpp:switch_default -->
            <xsl:value-of select="cpp:comment('cpp:switch_default', 1)" />
            <xsl:value-of select="cpp:switch_default(1)" />
            <!-- cpp:break -->
            <xsl:value-of select="cpp:break(2)" />
            <!-- cpp:switch_case_end -->
            <xsl:value-of select="cpp:switch_case_end(1)" />
            
            <!-- cpp:switch_end -->
            <xsl:value-of select="cpp:switch_end(0)" />
            <xsl:value-of select="o:nl(1)" />

            <!-- cpp:if -->
            <xsl:value-of select="cpp:comment('cpp:if', 0)" />
            <xsl:value-of select="cpp:if(o:var('condition'), o:var('suffix'), 0)" />
            <xsl:value-of select="o:nl(2)" />

            <!-- cpp:if_not -->
            <xsl:value-of select="cpp:comment('cpp:if_not', 0)" />
            <xsl:value-of select="cpp:if_not(o:var('condition'), o:var('suffix'), 0)" />
            <xsl:value-of select="o:nl(2)" />

            <!-- cpp:try -->
            <xsl:value-of select="cpp:comment('cpp:try', 0)" />
            <xsl:value-of select="cpp:try(0)" />
            <xsl:value-of select="o:nl(1)" />

            <!-- cpp:catch -->
            <xsl:value-of select="cpp:comment('cpp:catch', 0)" />
            <xsl:value-of select="cpp:catch(o:var('prefix'), o:var('exception'), 0)" />
            <xsl:value-of select="o:nl(1)" />

            <!-- cpp:throw -->
            <xsl:value-of select="cpp:comment('cpp:throw', 0)" />
            <xsl:value-of select="cpp:throw(o:var('exception_type'), o:var('exception_value'), 0)" />
            <xsl:value-of select="o:nl(1)" />

            <!-- cpp:catch_exception -->
            <xsl:value-of select="cpp:comment('cpp:catch_exception', 0)" />
            <xsl:value-of select="cpp:catch_exception(o:var('prefix'), o:var('exception_type'), o:var('exception_name'), 0)" />
            <xsl:value-of select="o:nl(1)" />

            <!-- cpp:define_namespaces -->
            <xsl:value-of select="cpp:comment('cpp:define_namespaces', 0)" />
            <xsl:value-of select="cpp:define_namespaces((o:var('namespace_1'), o:var('namespace_2'), o:var('namespace_3')))" />

            <!-- cpp:close_namespaces -->
            <xsl:value-of select="cpp:comment('cpp:close_namespaces', 2)" />
            <xsl:value-of select="cpp:close_namespaces((o:var('namespace_1'), o:var('namespace_2'), o:var('namespace_3')))" />
            <xsl:value-of select="o:nl(1)" />

            <!-- cpp:and -->
            <xsl:value-of select="cpp:comment('cpp:and', 0)" />
            <xsl:value-of select="cpp:and()" />
            <xsl:value-of select="o:nl(2)" />

            <!-- cpp:or -->
            <xsl:value-of select="cpp:comment('cpp:or', 0)" />
            <xsl:value-of select="cpp:or()" />
            <xsl:value-of select="o:nl(2)" />

            <!-- cpp:cerr -->
            <xsl:value-of select="cpp:comment('cpp:cerr', 0)" />
            <xsl:value-of select="cpp:cerr((o:var('message_1'), o:var('message_2'), o:var('message_3')), 0)" />
            <xsl:value-of select="o:nl(1)" />

            <!-- cpp:define_pair -->
            <xsl:value-of select="cpp:comment('cpp:define_pair', 0)" />
            <xsl:value-of select="cpp:define_pair(o:var('message_1'), o:var('message_2'))" />
            <xsl:value-of select="o:nl(2)" />

            <!-- cpp:get_placeholders -->
            <xsl:value-of select="cpp:comment('cpp:get_placeholders(3)', 0)" />
            <xsl:value-of select="cpp:get_placeholders(3)" />
        </xsl:result-document>

        <!-- All outputs for cmake_functions -->
        <xsl:result-document href="{o:write_file('cmake_functions.cmake')}">
            <!-- cmake:comment -->
            <xsl:value-of select="cmake:comment('cmake:comment', 0)" />
            <xsl:value-of select="cmake:comment(o:var('text'), 0)" />
            <xsl:value-of select="o:nl(1)" />

            <!-- cmake:wrap_variable -->
            <xsl:value-of select="cmake:comment('cmake:wrap_variable', 0)" />
            <xsl:value-of select="cmake:wrap_variable('variable_name')" />
            <xsl:value-of select="o:nl(2)" />

            <!-- cmake:target_compile_definitions -->
            <xsl:value-of select="cmake:comment('cmake:target_compile_definitions', 0)" />
            <xsl:value-of select="cmake:target_compile_definitions('target', o:var('scope'), o:var('args'), 0)" />
            <xsl:value-of select="o:nl(1)" />

            <!-- cmake:target_include_directories -->
            <xsl:value-of select="cmake:comment('cmake:target_include_directories', 0)" />
            <xsl:value-of select="cmake:target_include_directories('target', o:var('scope'), o:var('directory'), 0)" />
            <xsl:value-of select="o:nl(1)" />

            <!-- cmake:target_link_libraries -->
            <xsl:value-of select="cmake:comment('cmake:target_link_libraries', 0)" />
            <xsl:value-of select="cmake:target_link_libraries('target', o:var('scope'), o:var('directory'), 0)" />
            <xsl:value-of select="o:nl(1)" />

            <!-- cmake:add_subdirectory -->
            <xsl:value-of select="cmake:comment('cmake:add_subdirectory', 0)" />
            <xsl:value-of select="cmake:add_subdirectory( o:var('directory') )" />
            <xsl:value-of select="o:nl(1)" />

            <!-- cmake:find_library -->
            <xsl:value-of select="cmake:comment('cmake:find_library', 0)" />
            <xsl:value-of select="cmake:find_library( o:var('library_variable'), o:var('library_name'), o:var('library_path'), 0 )" />
            <xsl:value-of select="o:nl(1)" />

            <!-- cmake:find_package -->
            <xsl:value-of select="cmake:comment('cmake:find_package', 0)" />
            <xsl:value-of select="cmake:find_package( o:var('package_name'), o:var('args'), 0 )" />
            <xsl:value-of select="o:nl(1)" />

            <!-- cmake:set_variable -->
            <xsl:value-of select="cmake:comment('cmake:set_variable', 0)" />
            <xsl:value-of select="cmake:set_variable( o:var('variable'), o:var('value'), 0 )" />
            <xsl:value-of select="o:nl(1)" />

            <!-- cmake:set_variable_if_not_set -->
            <xsl:value-of select="cmake:comment('cmake:set_variable_if_not_set', 0)" />
            <xsl:value-of select="cmake:set_variable_if_not_set( o:var('variable'), o:var('value'), 0 )" />
            <xsl:value-of select="o:nl(1)" />

            <!-- cmake:set_project_name -->
            <xsl:value-of select="cmake:comment('cmake:set_project_name', 0)" />
            <xsl:value-of select="cmake:set_project_name( o:var('proj_name') )" />

            <!-- cmake:add_library -->
            <xsl:value-of select="cmake:comment('cmake:add_library', 0)" />
            <xsl:value-of select="cmake:add_library( 'library_name', o:var('library_type'), o:var('args') )" />
            <xsl:value-of select="o:nl(1)" />

            <!-- cmake:if_start / cmake:if_elseif / cmake:if_else / cmake:if_end -->
            <xsl:value-of select="cmake:comment('cmake:if_start / cmake:if_elseif / cmake:if_else / cmake:if_end', 0)" />
            <xsl:value-of select="cmake:if_start(o:var('condition'), 0)" />
            <xsl:value-of select="cmake:if_elseif(o:var('condition'), 0)" />
            <xsl:value-of select="cmake:if_else(0)" />
            <xsl:value-of select="cmake:if_end(o:var('condition'), 0)" />
            <xsl:value-of select="o:nl(1)" />

            <!-- cmake:message -->
            <xsl:value-of select="cmake:comment('cmake:message', 0)" />
            <xsl:value-of select="cmake:message(o:var('message'), 0)" />
            <xsl:value-of select="o:nl(1)" />

            <!-- cmake:return -->
            <xsl:value-of select="cmake:comment('cmake:return', 0)" />
            <xsl:value-of select="cmake:return(0)" />
            <xsl:value-of select="o:nl(1)" />

            <!-- cmake:configure_file -->
            <xsl:value-of select="cmake:comment('cmake:configure_file', 0)" />
            <xsl:value-of select="cmake:configure_file(o:var('input_file'), o:var('output_dir'))" />
            <xsl:value-of select="o:nl(1)" />

            <!-- cmake:current_binary_dir_var -->
            <xsl:value-of select="cmake:comment('cmake:current_binary_dir_var', 0)" />
            <xsl:value-of select="cmake:current_binary_dir_var()" />
            <xsl:value-of select="o:nl(2)" />

            <!-- cmake:current_source_dir_var -->
            <xsl:value-of select="cmake:comment('cmake:current_source_dir_var', 0)" />
            <xsl:value-of select="cmake:current_source_dir_var()" />
            <xsl:value-of select="o:nl(2)" />

            <!-- cmake:current_source_dir_var -->
            <xsl:value-of select="cmake:comment('cmake:cmake_file', 0)" />
            <xsl:value-of select="cmake:cmake_file()" />
            <xsl:value-of select="o:nl(2)" />

            <!-- cmake:add_subdirectories -->
            <xsl:value-of select="cmake:comment('cmake:add_subdirectories', 0)" />
            <xsl:value-of select="cmake:add_subdirectories((o:var('sub_directory_1'), o:var('sub_directory_2')))" />
            <xsl:value-of select="o:nl(1)" />

            <!-- cmake:cmake_minimum_required -->
            <xsl:value-of select="cmake:comment('cmake:cmake_minimum_required', 0)" />
            <xsl:value-of select="cmake:cmake_minimum_required(o:var('version'))" />
            <xsl:value-of select="o:nl(1)" />

            <!-- cmake:set_cpp17 -->
            <xsl:value-of select="cmake:comment('cmake:set_cpp17', 0)" />
            <xsl:value-of select="cmake:set_cpp17()" />
            <xsl:value-of select="o:nl(1)" />

            <!-- cmake:set_output_directory -->
            <xsl:value-of select="cmake:comment('cmake:set_output_directory', 0)" />
            <xsl:value-of select="cmake:set_output_directory(o:var('var'), o:var('output_directory'), 0)" />

            <!-- cmake:set_library_output_directory -->
            <xsl:value-of select="cmake:comment('cmake:set_library_output_directory', 0)" />
            <xsl:value-of select="cmake:set_library_output_directory(o:var('output_directory'), 0)" />

            <!-- cmake:set_runtime_output_directory -->
            <xsl:value-of select="cmake:comment('cmake:set_runtime_output_directory', 0)" />
            <xsl:value-of select="cmake:set_runtime_output_directory(o:var('output_directory'), 0)" />

            <!-- cmake:set_archive_output_directory -->
            <xsl:value-of select="cmake:comment('cmake:set_archive_output_directory', 0)" />
            <xsl:value-of select="cmake:set_archive_output_directory(o:var('output_directory'), 0)" />

            <!-- cmake:set_property -->
            <xsl:value-of select="cmake:comment('cmake:set_property', 0)" />
            <xsl:value-of select="cmake:set_property(o:var('property'), 0)" />
            <xsl:value-of select="o:nl(1)" />

            <!-- cmake:set_property -->
            <xsl:value-of select="cmake:comment('cmake:get_env_var', 0)" />
            <xsl:value-of select="cmake:get_env_var('variable_name')" />
            <xsl:value-of select="o:nl(2)" />

            <!-- cmake:use_ccache -->
            <xsl:value-of select="cmake:comment('cmake:use_ccache', 0)" />
            <xsl:value-of select="cmake:use_ccache(0)" />
        </xsl:result-document>

        <!-- All outputs for idl_functions.xsl -->
        <xsl:result-document href="{o:write_file('idl_functions.idl')}">
            <!-- idl:include -->
            <xsl:value-of select="idl:comment('idl:include', 0)" />
            <xsl:value-of select="idl:include(o:var('idl_file'))" />
            <xsl:value-of select="o:nl(1)" />

            <!-- idl:module_start / idl:module_end -->
            <xsl:value-of select="idl:comment('idl:module_start / idl:module_end', 0)" />
            <xsl:value-of select="idl:module_start(o:var('namespace'), 0)" />
            <xsl:value-of select="idl:module_end(o:var('namespace'), 0)" />
            <xsl:value-of select="o:nl(1)" />

            <!-- idl:enum / idl:enum_value -->
            <xsl:value-of select="idl:comment('idl:enum', 0)" />
            <xsl:value-of select="idl:enum(o:var('label'), 0)" />
            <xsl:value-of select="idl:comment('idl:enum_value', 1)" />
            <xsl:value-of select="idl:enum_value(o:var('label'), false(), 1)" />
            <xsl:value-of select="idl:enum_value(o:var('label'), true(), 1)" />
            <xsl:value-of select="cpp:scope_end(0)" />
            <xsl:value-of select="o:nl(1)" />

            <!-- idl:struct -->
            <xsl:value-of select="idl:comment('idl:struct', 0)" />
            <xsl:value-of select="idl:struct(o:var('label'), 0)" />
            <xsl:value-of select="o:nl(1)" />

            <!-- idl:struct -->
            <xsl:value-of select="idl:comment('idl:typedef', 0)" />
            <xsl:value-of select="idl:typedef(o:var('type'), o:var('label'), 0)" />
            <xsl:value-of select="o:nl(1)" />

            <!-- idl:union / idl:union_member -->
            <xsl:value-of select="idl:comment('idl:union', 0)" />
            <xsl:value-of select="idl:union(o:var('label'), o:var('union_type'), 0)" />
            <xsl:value-of select="idl:comment('idl:union_member', 1)" />
            <xsl:value-of select="idl:union_member(o:var('case_index'), o:var('type'), o:var('label'), 1)" />
            <xsl:value-of select="o:nl(1)" />

            <!-- idl:interface -->
            <xsl:value-of select="idl:comment('idl:interface', 0)" />
            <xsl:value-of select="idl:interface(o:var('label'), 0)" />
            <xsl:value-of select="o:nl(1)" />

            <!-- idl:function -->
            <xsl:value-of select="idl:comment('idl:function', 0)" />
            <xsl:value-of select="idl:function(o:var('function_name'), o:var('return_type'), o:var('input_parameters'), false(), 0)" />
            <xsl:value-of select="idl:comment('idl:function (One-way)', 0)" />
            <xsl:value-of select="idl:function(o:var('function_name'), o:var('return_type'), o:var('input_parameters'), true(), 0)" />
            <xsl:value-of select="o:nl(1)" />

            <!-- idl:key -->
            <xsl:value-of select="idl:comment('idl:key', 0)" />
            <xsl:value-of select="idl:key(true())" />
            <xsl:value-of select="o:nl(1)" />

            <!-- idl:key_pragma -->
            <xsl:value-of select="idl:comment('idl:key_pragma', 0)" />
            <xsl:value-of select="idl:key_pragma(o:var('type'), o:var('key'), 0)" />
            <xsl:value-of select="o:nl(1)" />

            <!-- idl:member -->
            <xsl:value-of select="idl:comment('idl:member', 0)" />
            <xsl:value-of select="idl:member(o:var('type'), o:var('label'), true(), 0)" />
            <xsl:value-of select="o:nl(1)" />

            <!-- idl:member -->
            <xsl:value-of select="idl:comment('idl:sequence_type', 0)" />
            <xsl:value-of select="idl:sequence_type(o:var('inner_type'), o:var('size'))" />
            <xsl:value-of select="o:nl(2)" />

            <!-- idl:get_type -->
            <xsl:value-of select="idl:comment('idl:get_type (std::string)', 0)" />
            <xsl:value-of select="idl:get_type('std::string')" />
            <xsl:value-of select="o:nl(1)" />


        </xsl:result-document>

    </xsl:template>
</xsl:stylesheet>
