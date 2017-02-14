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

            <xsl:variable name="aggregate_label" select ="cdit:get_key_value(.,'label')" />
            <xsl:variable name="aggregate_label_cc" select="o:camel_case($aggregate_label)" />
            <xsl:variable name="aggregate_label_lc" select="lower-case($aggregate_label)" />

            <xsl:variable name="middleware" select="'zmq'" />

            
            <xsl:variable name="base_type" select="concat('::', $aggregate_label_cc)" />
            <xsl:variable name="mw_type" select="concat($namespace, '::', $aggregate_label_cc)" />
            
            <!-- OPEN middleware/zmq/label/libportexports.cpp -->
            <xsl:result-document href="{concat('middleware/', $middleware, '/', $aggregate_label_lc, '/libportexports.cpp')}">
                <xsl:value-of select="o:get_libport_export(., $middleware, $base_type, $mw_type)" />
            </xsl:result-document>

            <!-- Output CMakeLists.txt -->
            <xsl:result-document href="{concat('middleware/', $middleware, '/', $aggregate_label_lc, '/CMakeLists.txt')}">
                <!-- set RE_PATH -->
                <xsl:value-of select="o:cmake_comment('Get the RE_PATH')" />
                <xsl:value-of select="o:cmake_set_env('RE_PATH', '$ENV{RE_PATH}')" />
                <xsl:value-of select="o:cmake_set_env('CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}', '${RE_PATH}/cmake/modules')" />
                <xsl:value-of select="o:nl()" />

                <!-- Set PROJ_NAME -->
                <xsl:variable name="proj_name" select="concat($middleware, '_', $aggregate_label_lc)" />
                <xsl:value-of select="o:cmake_set_proj_name($proj_name)" />

                <!-- Find re_core -->
                <xsl:value-of select="o:nl()" />
                <xsl:value-of select="o:cmake_find_library('re_core', 'RE_CORE_LIBRARIES', '${RE_PATH}/lib')" />
                <xsl:value-of select="o:nl()" />

                <!-- Find 0MQ Helper -->
                <xsl:variable name="mw_helper_libs" select="concat(upper-case($middleware), '_HELPER_LIBRARIES')" />
                <xsl:value-of select="o:cmake_find_library(concat($middleware,'_helper'), $mw_helper_libs , '${RE_PATH}/lib')" />
                <xsl:value-of select="o:nl()" />

                <!-- Find 0MQ -->
                <xsl:value-of select="o:cmake_find_package('ZeroMQ')" />
                <xsl:value-of select="o:nl()" />
                
                <!-- Set Source files -->
                <xsl:value-of select="concat('set(SOURCE', o:nl())" />
                <xsl:value-of select="concat(o:t(1), '${CMAKE_CURRENT_SOURCE_DIR}/libportexports.cpp', o:nl())" />
                <xsl:value-of select="concat(o:t(1), ')', o:nl())" />
                <xsl:value-of select="o:nl()" />

                <!-- Include directories -->
                <xsl:value-of select="o:cmake_comment('Include the RE_PATH directory')" />
                <xsl:value-of select="o:cmake_include_dir('${RE_PATH}/src')" />
                <xsl:value-of select="o:nl()" />
                
                <xsl:value-of select="o:cmake_comment('Include the 0MQ directory')" />
                <xsl:value-of select="o:cmake_include_dir('${ZeroMQ_INCLUDE_DIRS}')" />
                <xsl:value-of select="o:nl()" />

                <xsl:value-of select="o:cmake_comment('Include the current binary directory to enable linking of pb.h')" />
                <xsl:value-of select="o:cmake_include_dir('${CMAKE_CURRENT_BINARY_DIR}/../proto')" />
                <xsl:value-of select="o:nl()" />

                <!-- Link against Core Libraries -->
                <xsl:value-of select="concat('add_library(${PROJ_NAME} MODULE ${SOURCE} ${HEADERS})', o:nl())" />
                <xsl:value-of select="concat('target_link_libraries(${PROJ_NAME} ${RE_CORE_LIBRARIES})', o:nl())" />
                <xsl:value-of select="concat('target_link_libraries(${PROJ_NAME} ${ZeroMQ_LIBRARIES})', o:nl())" />
                <xsl:value-of select="concat('target_link_libraries(${PROJ_NAME} ${', $mw_helper_libs, '})', o:nl())" />
                <xsl:value-of select="o:nl()" />

                <!-- Link against base datatype libraries -->
                <xsl:value-of select="o:cmake_comment('Include the base datatype library which represents this type')" />
                <xsl:value-of select="concat('target_link_libraries(${PROJ_NAME} datatype_', $aggregate_label_lc, ')', o:nl())" />
                <xsl:value-of select="o:cmake_comment('Include the proto datatype library which represents this type')" />
                <xsl:value-of select="concat('target_link_libraries(${PROJ_NAME} proto_', $aggregate_label_lc, ')', o:nl())" />
                <xsl:value-of select="o:nl()" />
            </xsl:result-document>
        </xsl:for-each>
    </xsl:template>
</xsl:stylesheet>
