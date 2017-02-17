<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="2.0" 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:gml="http://graphml.graphdrawing.org/xmlns"
    xmlns:exsl="http://exslt.org/common"
    xmlns:xalan="http://xml.apache.org/xslt"	
    xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" 
    xmlns:cdit="http://whatever"
    xmlns:o="http://whatever"
    exclude-result-prefixes="gml exsl xalan">
		
    
    <xsl:import href="functions.xsl"/>
    <xsl:output method="text" 
        omit-xml-declaration="yes"
        indent="yes" 
		standalone="no"
		xalan:indent-amount="4"/>
	<xsl:strip-space elements="*" />
	
    <xsl:variable name="aggregates" as="element()*" select="cdit:get_entities_of_kind(.,'Aggregate')" />

    <!-- apply all templates starting from the root -->
    <xsl:template match="/">
        <xsl:for-each select="$aggregates">
            <!-- Get the name of the IDL Parent -->
            <xsl:variable name="idl_name" select="cdit:get_key_value(../..,'label')" />

            <!-- Camel Case the Namespace -->
            <xsl:variable name="namespace" select="o:camel_case($idl_name)" />

            <!-- Get all the children Members, VectorInstance, AggregateInstance -->
            <xsl:variable name="members" as="element()*" select="cdit:get_child_entities_of_kind(., 'Member')" />
            <xsl:variable name="vectors" as="element()*" select="cdit:get_child_entities_of_kind(., 'VectorInstance')" />
            <xsl:variable name="aggregates" as="element()*" select="cdit:get_child_entities_of_kind(., 'AggregateInstance')" />
            
            <xsl:variable name="required_datatypes" select="cdit:get_required_datatypes($vectors, $aggregates)" />

            <xsl:variable name="aggregate_label" select="cdit:get_key_value(.,'label')" />
            <xsl:variable name="aggregate_label_cc" select="o:camel_case($aggregate_label)" />
            <xsl:variable name="aggregate_label_lc" select="lower-case($aggregate_label)" />

            
            <xsl:variable name="middleware" select="'rti'" />
            <xsl:variable name="mw" select="$middleware" />
            <xsl:variable name="base_mw" select="'base'" />

            
            <xsl:variable name="base_type" select="concat('::', $aggregate_label_cc)" />
            <xsl:variable name="mw_type" select="concat($namespace, '::', $aggregate_label_cc)" />
            
            <!-- OPEN middleware/proto/label/label.idl -->
            <xsl:result-document href="{concat('middleware/', $middleware, '/', $aggregate_label_lc, '.idl')}">
                <!-- Import other IDLs -->
                <xsl:for-each-group select="$required_datatypes" group-by=".">
                    <xsl:variable name="datatype" select="lower-case(.)" />
                    <xsl:variable name="idl_path" select="concat($datatype, '.idl')" />

                    <xsl:value-of select="o:lib_include($idl_path)" />
                </xsl:for-each-group>
                
                <!-- Module Name -->
                <xsl:value-of select="concat('module ', $idl_name, '{', o:nl())" />
                 
                <!-- Struct Name -->
                <xsl:value-of select="concat(o:t(1), 'struct ', $aggregate_label_cc, ' {', o:nl())" />

                <!-- Process Members -->
                <xsl:for-each select="$members">
                    <!-- TODO: Need to handle members/vectors/aggregates out of order -->
                    <xsl:variable name="sort_order" select="number(cdit:get_key_value(., 'sortOrder')) + 1" />
                    
                    <xsl:variable name="member_label" select="cdit:get_key_value(.,'label')" />
                    <xsl:variable name="member_type" select="cdit:get_key_value(.,'type')" />

                    <xsl:variable name="member_is_key" select="cdit:is_key_value_true(.,'key')" />
                    <xsl:variable name="member_cpp_type" select="cdit:get_cpp_type($member_type)" />
                    <xsl:variable name="member_dds_type" select="cdit:get_cpp2dds_type($member_cpp_type)" />
                    
                    <xsl:value-of select="concat(o:t(2), $member_dds_type, ' ', $member_label, ';')" />

                    <xsl:if test="$member_is_key">
                        <xsl:value-of select="' //@key'" />
                    </xsl:if>
                    <xsl:value-of select="o:nl()" />
                </xsl:for-each>

                <!-- Process Vectors -->
                <xsl:for-each select="$vectors">
                    <xsl:variable name="sort_order" select="number(cdit:get_key_value(., 'sortOrder')) + 1" />
                    
                    <xsl:variable name="vector_label" select="cdit:get_key_value(., 'label')" />
                    <xsl:variable name="vector_cpp_type" select="cdit:get_vector_type(.)" />
                    <xsl:variable name="vector_dds_type" select="cdit:get_cpp2dds_type($vector_cpp_type)" />
                    
                    <xsl:variable name="is_vector_complex" select="cdit:is_vector_complex(.)" />
                    
                    <xsl:choose>
                        <xsl:when test="$is_vector_complex">
                            <!-- Complex types -->
                            <xsl:variable name="aggregate_namespace" select="cdit:get_namespace(.)" />
                            <xsl:value-of select="concat(o:t(2), 'sequence', o:angle_wrap(concat($aggregate_namespace, '::', $vector_cpp_type)), ' ', $vector_label, ';', o:nl())" />
                        </xsl:when>
                        <xsl:otherwise>
                            <!-- Primitive types -->
                            <xsl:value-of select="concat(o:t(2), 'sequence', o:angle_wrap($vector_dds_type), ' ', $vector_label, ';', o:nl())" />
                        </xsl:otherwise>
                    </xsl:choose>
                </xsl:for-each>

                <!-- Process Aggregates -->
                <xsl:for-each select="$aggregates">
                    <xsl:variable name="sort_order" select="number(cdit:get_key_value(., 'sortOrder')) + 1" />
                    
                    <xsl:variable name="aggregate_label" select="cdit:get_key_value(., 'label')" />
                    <xsl:variable name="aggregate_type" select="cdit:get_key_value(.,'type')" />

                    <!-- Get the Namespace -->
                    <xsl:variable name="aggregate_namespace" select="cdit:get_namespace(.)" />
                    
                    <xsl:value-of select="concat(o:t(2), $aggregate_namespace, '::', $aggregate_type, ' ', $aggregate_label, ';', o:nl())" />
                </xsl:for-each>

                <xsl:value-of select="concat(o:t(1),'};', o:nl())" />
                
                <xsl:for-each select="$members">
                    <xsl:variable name="member_label" select="cdit:get_key_value(.,'label')" />
                    <xsl:variable name="member_is_key" select="cdit:is_key_value_true(.,'key')" />

                    <xsl:if test="$member_is_key">
                        <xsl:value-of select="concat(o:t(1),'#pragma keylist ', $aggregate_label_lc, ' ', $member_label, o:nl())" />
                    </xsl:if>
                </xsl:for-each>
                
                <xsl:value-of select="concat('};', o:nl())" />
            </xsl:result-document>


            <!-- OPEN middleware/rti/label/libportexports.cpp -->
            <xsl:result-document href="{concat('middleware/', $middleware, '/', $aggregate_label_lc, '/libportexports.cpp')}">
                <xsl:value-of select="o:get_libport_export(., $middleware, $base_type, $mw_type)" />
            </xsl:result-document>

            <!-- OPEN middleware/rti/label/convert.h -->
            <xsl:result-document href="{concat('middleware/', $middleware, '/', $aggregate_label_lc, '/convert.h')}">
                <xsl:variable name="define_guard" select="upper-case(concat(upper-case($middleware), '_', $aggregate_label, '_CONVERT_H'))" />

                <!-- Define Guard -->
                <xsl:value-of select="o:define_guard($define_guard)" />

                <!-- Include the base message type -->
                <xsl:value-of select="o:cpp_comment('Include the base type')" />
                <!-- path back to root -->
                <xsl:variable name="rel_path" select="'../../../'" />
                <xsl:value-of select="o:local_include(concat($rel_path, 'datatypes/', $aggregate_label_lc, '/', $aggregate_label_lc, '.h'))" />
                <xsl:value-of select="o:nl()" />

                <!-- Forward declare the new type -->
                <xsl:value-of select="o:cpp_comment('Forward declare the concrete type')" />
                <xsl:value-of select="o:forward_declare_class($namespace, $aggregate_label_cc)" />

                <xsl:value-of select="o:nl()" />
                <xsl:value-of select="o:namespace($middleware)" />

                <!-- translate functions -->
                <xsl:value-of select="o:tabbed_cpp_comment('Translate Functions', 1)" />
                <xsl:value-of select="concat(o:t(1), $mw_type, '* translate(const ', $base_type ,'* val);', o:nl())" />
                <xsl:value-of select="concat(o:t(1), $base_type, '* translate(const ', $mw_type ,'* val);', o:nl())" />
                <xsl:value-of select="concat('};', o:nl())" />

                <!-- End Define Guard -->
                <xsl:value-of select="o:define_guard_end($define_guard)" />
            </xsl:result-document>

            <!-- OPEN middleware/rti/label/convert.cpp -->
            <xsl:result-document href="{concat('middleware/', $middleware, '/', $aggregate_label_lc, '/convert.cpp')}">
                <xsl:variable name="in_var" select="'val'" />
                <xsl:variable name="out_var" select="'out_'" />

                <!-- Include the header -->
                <xsl:value-of select="o:local_include('convert.h')" />
                <!-- Include the generated header -->
                <xsl:value-of select="o:local_include(concat($aggregate_label_lc, '.hpp'))" />
                <xsl:value-of select="o:nl()" />

                <!-- Link against the base libraries which this message contains -->
                <xsl:for-each-group select="$required_datatypes" group-by=".">
                    <xsl:variable name="datatype" select="lower-case(.)" />
                    <xsl:value-of select="o:local_include(concat('../', $datatype, '/convert.h'))" />
                </xsl:for-each-group>

                <!-- Translate Function from BASE to MIDDLEWARE-->
                <xsl:value-of select="o:cpp_comment('Translate from Base -> Middleware')" />
                <xsl:value-of select="o:get_translate_cpp($members, $vectors, $aggregates, $base_type, $mw_type, $base_mw, $mw, $middleware)" />

                <xsl:value-of select="o:cpp_comment('Translate from Middleware -> Base')" />
                <xsl:value-of select="o:get_translate_cpp($members, $vectors, $aggregates, $mw_type, $base_type, $mw, $base_mw, $middleware)" />
            </xsl:result-document>

            <!-- Output middleware/rti/label/CMakeLists.txt -->
            <xsl:result-document href="{concat('middleware/', $middleware, '/', $aggregate_label_lc, '/CMakeLists.txt')}">
                <!-- set RE_PATH -->
                <xsl:value-of select="o:cmake_comment('Get the RE_PATH')" />
                <xsl:value-of select="o:cmake_set_env('RE_PATH', '$ENV{RE_PATH}')" />
                <xsl:value-of select="o:cmake_set_env('CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}', '${RE_PATH}/cmake/modules')" />
                <xsl:value-of select="o:nl()" />

                <!-- Set PROJ_NAME -->
                <xsl:variable name="proj_name" select="concat($middleware, '_', $aggregate_label_lc)" />
                <xsl:variable name="lib_name" select="concat($middleware, '_', $aggregate_label_lc, '_lib')" />
                
                <xsl:value-of select="o:cmake_set_env('SHARED_LIB_NAME', $lib_name)" />
                <xsl:value-of select="o:cmake_set_proj_name($proj_name)" />
                
                
                  <!-- Find re_core -->
                <xsl:value-of select="o:nl()" />
                <xsl:value-of select="o:cmake_find_package('RTIDDS')" />
                <xsl:value-of select="o:nl()" />

                <xsl:value-of select="o:cmake_comment(concat('Generate the middleware files for ', $aggregate_label_lc, '.idl'))" />
                <xsl:value-of select="concat('RTI_GENERATE_CPP(DDS_SOURCE DDS_HEADERS ', $aggregate_label_lc, '.idl)', o:nl())" />
                <xsl:value-of select="o:nl()" />
                
                <!-- Copy the other .IDL files required -->
                <xsl:if test="count($required_datatypes)">
                    <xsl:value-of select="o:cmake_comment('Copy required IDLs into the binary directory so the dds generation can succeed')" />
                    <xsl:for-each-group select="$required_datatypes" group-by=".">
                        <xsl:variable name="datatype" select="lower-case(.)" />
                        <xsl:value-of select="concat('configure_file(../', $datatype, '/', $datatype, '.idl ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)', o:nl())" />
                    </xsl:for-each-group>
                    <xsl:value-of select="o:nl()" />
                </xsl:if>

                <!-- Find re_core -->
                <xsl:value-of select="o:cmake_find_library('re_core', 'RE_CORE_LIBRARIES', '${RE_PATH}/lib')" />
                <xsl:value-of select="o:nl()" />

                <!-- Find 0MQ Helper -->
                <xsl:variable name="mw_helper_libs" select="concat(upper-case($middleware), '_HELPER_LIBRARIES')" />
                <xsl:value-of select="o:cmake_find_library(concat($middleware,'_helper'), $mw_helper_libs , '${RE_PATH}/lib')" />
                <xsl:value-of select="o:nl()" />

                <!-- Set Source files -->
                <xsl:value-of select="concat('set(SOURCE', o:nl())" />
                <xsl:value-of select="concat(o:t(1), '${CMAKE_CURRENT_SOURCE_DIR}/convert.cpp', o:nl())" />
                <xsl:value-of select="concat(o:t(1), ')', o:nl())" />
                <xsl:value-of select="o:nl()" />

                <!-- Set Source files -->
                <xsl:value-of select="concat('set(HEADERS', o:nl())" />
                <xsl:value-of select="concat(o:t(1), '${CMAKE_CURRENT_SOURCE_DIR}/convert.h', o:nl())" />
                <xsl:value-of select="concat(o:t(1), ')', o:nl())" />
                <xsl:value-of select="o:nl()" />

                <!-- Include directories -->
                <xsl:value-of select="o:cmake_comment('Include the RE_PATH directory')" />
                <xsl:value-of select="o:cmake_include_dir('${RE_PATH}/src')" />
                <xsl:value-of select="o:nl()" />
                
                <xsl:value-of select="o:cmake_comment('Include the RTI DDS directory')" />
                <xsl:value-of select="o:cmake_include_dir('${DDS_INCLUDE_DIRS}')" />
                <xsl:value-of select="o:nl()" />

                <xsl:value-of select="o:cmake_comment('Include the current binary directory to enable linking to the autogenerated files')" />
                <xsl:value-of select="o:cmake_include_dir('${CMAKE_CURRENT_BINARY_DIR}')" />
                <xsl:value-of select="o:nl()" />

                <!-- Link against other proto libraries which this message contains -->
                <xsl:if test="count($required_datatypes)">
                    <xsl:value-of select="o:cmake_comment(concat('Include the directories the required ', $middleware, ' shared libraries are built into'))" />
                    <xsl:for-each-group select="$required_datatypes" group-by=".">
                        <xsl:variable name="datatype" select="lower-case(.)" />
                        <xsl:value-of select="o:cmake_include_dir(concat('${CMAKE_CURRENT_BINARY_DIR}/../', $datatype, '/'))" />
                    </xsl:for-each-group>
                    <xsl:value-of select="o:nl()" />
                </xsl:if>

                <!-- Build Module Library -->
                <xsl:value-of select="o:cmake_comment('Build the actual Module library that will be dynamically loaded.')" />
                <xsl:value-of select="concat('add_library(${PROJ_NAME} MODULE ${CMAKE_CURRENT_SOURCE_DIR}/libportexports.cpp)', o:nl())" />
                <xsl:value-of select="o:nl()" />

                <!-- Build Module Library -->
                <xsl:value-of select="o:cmake_comment('Build the shared library that will be loaded at compile time.')" />
                <xsl:value-of select="concat('add_library(${SHARED_LIB_NAME} SHARED ${SOURCE} ${HEADERS} ${DDS_SOURCE} ${DDS_HEADERS})', o:nl())" />
                <xsl:value-of select="o:nl()" />

                <xsl:value-of select="o:cmake_comment('This ensures DDS uses the correct types')" />
                <xsl:value-of select="concat('target_compile_definitions(${PROJ_NAME} PRIVATE -DRTI_UNIX -DRTI_64BIT)', o:nl())" />
                <xsl:value-of select="concat('target_compile_definitions(${SHARED_LIB_NAME} PRIVATE -DRTI_UNIX -DRTI_64BIT)', o:nl())" />
                <xsl:value-of select="o:nl()" />

                <!-- Link against Core Libraries -->
                <xsl:value-of select="concat('target_link_libraries(${SHARED_LIB_NAME} ${RE_CORE_LIBRARIES})', o:nl())" />
                <xsl:value-of select="concat('target_link_libraries(${SHARED_LIB_NAME} ${DDS_LIBRARIES})', o:nl())" />
                <xsl:value-of select="concat('target_link_libraries(${SHARED_LIB_NAME} ${', $mw_helper_libs, '})', o:nl())" />
                <xsl:value-of select="o:nl()" />

                <!-- Link against base datatype libraries -->
                <xsl:value-of select="o:cmake_comment('Link the BASE datatype library which represents this type')" />
                <xsl:value-of select="concat('target_link_libraries(${SHARED_LIB_NAME} datatype_', $aggregate_label_lc, ')', o:nl())" />
                <xsl:value-of select="o:nl()" />

                <!-- Link against other proto libraries which this message contains -->
                <xsl:if test="count($required_datatypes)">
                    <xsl:value-of select="o:cmake_comment(concat('Link the ', $middleware, ' shared libraries which are used in this type'))" />
                    <xsl:for-each-group select="$required_datatypes" group-by=".">
                        <xsl:variable name="datatype" select="lower-case(.)" />
                        <xsl:value-of select="concat('target_link_libraries(${SHARED_LIB_NAME} ', $middleware, '_', $datatype, '_lib)', o:nl())" />
                    </xsl:for-each-group>
                    <xsl:value-of select="o:nl()" />
                </xsl:if>

                <!-- Link the module against the shared library -->
                <xsl:value-of select="o:cmake_comment('Link the Module against the Shared library')" />
                <xsl:value-of select="concat('target_link_libraries(${PROJ_NAME} ${SHARED_LIB_NAME})', o:nl())" />
            </xsl:result-document>
        </xsl:for-each>
    </xsl:template>
</xsl:stylesheet>
