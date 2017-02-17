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

    <!-- Import functions -->	
    <xsl:import href="functions.xsl"/>
    
    <xsl:output method="text" 
        omit-xml-declaration="yes"
        indent="yes" 
		standalone="no"
		xalan:indent-amount="4"/>
	<xsl:strip-space elements="*" />
	
    <!-- Get the Aggregates -->
    <xsl:variable name="aggregates" as="element()*" select="cdit:get_entities_of_kind(., 'Aggregate')" />

    <xsl:template match="/">
        <xsl:for-each select="$aggregates">
            <!-- Get all the children Members, VectorInstance, AggregateInstance -->
            <xsl:variable name="members" as="element()*" select="cdit:get_entities_of_kind(., 'Member')" />
            <xsl:variable name="vectors" as="element()*" select="cdit:get_entities_of_kind(., 'VectorInstance')" />
            <xsl:variable name="aggregates" as="element()*" select="cdit:get_entities_of_kind(., 'AggregateInstance')" />
            <xsl:variable name="required_datatypes" select="cdit:get_required_datatypes($vectors, $aggregates)" />

            <!-- Precalculate aggregate names -->
            <xsl:variable name="aggregate_name" select="cdit:get_key_value(., 'label')" />
            <xsl:variable name="aggregate_name_lc" select="lower-case($aggregate_name)" />
            <xsl:variable name="class_name" select="o:camel_case($aggregate_name)" />

            <!-- Write Header File -->
            <xsl:result-document href="{concat('datatypes/', $aggregate_name_lc, '/', $aggregate_name_lc, '.h')}">
                <!-- Define Guard -->
                <xsl:variable name="define_guard" select="upper-case(concat('PORTS_', $aggregate_name, '_H'))" />
                <xsl:value-of select="o:define_guard($define_guard)" />

                <!-- Include Functions -->
                <xsl:value-of select="o:cpp_comment('Include Statements')" />
                <xsl:value-of select="o:lib_include('core/basemessage.h')" />
                <xsl:value-of select="o:lib_include('string')" />

                <xsl:variable name="rel_dir" select="'../'" />

                <!-- Include std vector if we need -->
                <xsl:if test ="count($vectors) > 0">
                    <xsl:value-of select="o:lib_include('vector')" />
                    <xsl:value-of select="o:nl()" />
                </xsl:if>

                <!-- Include required base types -->
                <xsl:if test ="count($required_datatypes) > 0">
                    <xsl:value-of select="o:cpp_comment('Include required datatypes')" />
                    <xsl:for-each-group select="$required_datatypes" group-by=".">
                        <xsl:variable name="datatype" select="lower-case(.)" />
                        <xsl:value-of select="o:local_include(concat($rel_dir, $datatype, '/', $datatype, '.h'))" />
                    </xsl:for-each-group>
                    <xsl:value-of select="o:nl()" />
                </xsl:if>

                <!-- Define the class, Inherits from BaseMessage -->
                <xsl:value-of select="concat('class ', $class_name, ' : public BaseMessage{', o:nl())" />
                
                <!-- Handle Members -->
                <xsl:for-each select="$members">
                    <xsl:variable name="member_label" select="cdit:get_key_value(., 'label')" />
                    <xsl:variable name="member_type" select="cdit:get_key_value(., 'type')" />
                    <xsl:variable name="member_cpp_type" select="cdit:get_cpp_type($member_type)" />

                    <xsl:value-of select="concat(o:nl(), o:t(1))" />
                    <xsl:value-of select="o:cpp_comment(concat('Basic Member: ', $member_label, ' ', o:angle_wrap($member_type)))" />
                    <xsl:value-of select="o:declare_variable_functions($member_label, $member_cpp_type)" />
                </xsl:for-each>

                <!-- Handle Vectors -->
                <xsl:for-each select="$vectors">
                    <xsl:variable name="vector_label" select="cdit:get_key_value(., 'label')" />
                    <xsl:variable name="vector_type" select="cdit:get_vector_type(.)" />
                    <xsl:variable name="is_vector_complex" select="cdit:is_vector_complex(.)" />

                    <xsl:variable name="vector_type_cpp">
                        <xsl:choose>
                            <xsl:when test="$is_vector_complex">
                                <xsl:value-of select="concat('::', $vector_type)" />
                            </xsl:when>
                            <xsl:otherwise>
                                <xsl:value-of select="$vector_type"/>
                            </xsl:otherwise>
                        </xsl:choose>
                    </xsl:variable>

                    <xsl:variable name="vector_cpp_type" select="concat('std::vector', o:angle_wrap($vector_type_cpp))" />


                    <xsl:value-of select="concat(o:nl(), o:t(1))" />
                    <xsl:value-of select="o:cpp_comment(concat('Vector Member: ', $vector_label, ' ',$vector_cpp_type))" />
                    <xsl:value-of select="o:declare_variable_functions($vector_label, $vector_cpp_type)" />
                </xsl:for-each>

                <!-- Handle Aggregates -->
                <xsl:for-each select="$aggregates">
                    <xsl:variable name="aggregate_label" select="cdit:get_key_value(., 'label')" />
                    <xsl:variable name="aggregate_type" select="cdit:get_key_value(., 'type')" />
                    <xsl:variable name="aggregate_cpp_type" select="concat('::', $aggregate_type)" />

                    <xsl:value-of select="concat(o:nl(), o:t(1))" />
                    <xsl:value-of select="o:cpp_comment(concat('Aggregate Member: ', $aggregate_label, ' ', o:angle_wrap($aggregate_type)))" />
                    <xsl:value-of select="o:declare_variable_functions($aggregate_label, $aggregate_cpp_type)" />
                </xsl:for-each>

                <xsl:value-of select="concat('};', o:nl())" />
                <xsl:value-of select="o:define_guard_end($define_guard)" />

             </xsl:result-document>

            <!-- Write *.cpp -->
            <xsl:result-document href="{concat('datatypes/', $aggregate_name_lc, '/', $aggregate_name_lc, '.cpp')}">
                <xsl:value-of select="o:local_include(concat($aggregate_name_lc, '.h'))" />
                <xsl:value-of select="o:nl()" />

                <!-- Handle Members -->
                <xsl:for-each select="$members">
                    <xsl:variable name="member_label" select="cdit:get_key_value(., 'label')" />
                    <xsl:variable name="member_type" select="cdit:get_key_value(., 'type')" />
                    <xsl:variable name="member_cpp_type" select="cdit:get_cpp_type($member_type)" />

                    <xsl:value-of select="o:define_variable_functions($member_label, $member_cpp_type, $class_name)" />
                </xsl:for-each>

                <!-- Handle Vectors -->
                <xsl:for-each select="$vectors">
                    <xsl:variable name="first_child" select="./gml:graph[1]/gml:node[1]" />
                    <xsl:variable name="vector_label" select="cdit:get_key_value(., 'label')" />

                    <!-- Get the Vector Type -->
                    <xsl:variable name="vector_child_kind" select="cdit:get_key_value($first_child, 'kind')" />
                    <xsl:variable name="vector_child_type" select="cdit:get_key_value($first_child, 'type')" />
                    
                    <!-- Get the type of the vector -->
                    <xsl:variable name="vector_type" select="if($vector_child_kind = 'AggregateInstance') then concat('::', $vector_child_type) else cdit:get_cpp_type($vector_child_type)" />

                    <xsl:variable name="vector_cpp_type" select="concat('std::vector', o:angle_wrap($vector_type))" />

                    <xsl:value-of select="o:define_variable_functions($vector_label, $vector_cpp_type, $class_name)" />
                </xsl:for-each>

                <!-- Handle Aggregates -->
                <xsl:for-each select="$aggregates">
                    <xsl:variable name="aggregate_label" select="cdit:get_key_value(., 'label')" />
                    <xsl:variable name="aggregate_type" select="cdit:get_key_value(., 'type')" />
                    <xsl:variable name="aggregate_cpp_type" select="concat('::', $aggregate_type)" />

                    <xsl:value-of select="o:define_variable_functions($aggregate_label, $aggregate_cpp_type, $class_name)" />
                </xsl:for-each>
            </xsl:result-document>
            <!-- Write CMakeLists.txt -->
            <xsl:result-document href="{concat('datatypes/', $aggregate_name_lc, '/', 'CMakeLists.txt')}">
                <!-- set RE_PATH -->
                <xsl:value-of select="o:cmake_comment('Get the RE_PATH')" />
                <xsl:value-of select="o:cmake_set_env('RE_PATH', '$ENV{RE_PATH}')" />
                <xsl:value-of select="o:nl()" />

                <!-- Set PROJ_NAME -->
                <xsl:variable name="proj_name" select="concat('datatype_', $aggregate_name_lc)" />
                <xsl:value-of select="o:cmake_set_proj_name($proj_name)" />

                <!-- Find re_core -->
                <xsl:value-of select="o:nl()" />
                <xsl:value-of select="o:cmake_find_library('re_core', 'RE_CORE_LIBRARIES', '${RE_PATH}/lib')" />
                <xsl:value-of select="o:nl()" />

                <!-- Set Source files -->
                <xsl:value-of select="concat('set(SOURCE', o:nl())" />
                <xsl:value-of select="concat(o:t(1), '${CMAKE_CURRENT_SOURCE_DIR}/', $aggregate_name_lc, '.cpp', o:nl())" />
                <xsl:value-of select="concat(o:t(1), ')', o:nl())" />
                <xsl:value-of select="o:nl()" />

                <!-- Set Headers files -->
                <xsl:value-of select="concat('set(HEADERS', o:nl())" />
                <xsl:value-of select="concat(o:t(1), '${CMAKE_CURRENT_SOURCE_DIR}/', $aggregate_name_lc, '.h', o:nl())" />
                <xsl:value-of select="concat(o:t(1), ')', o:nl())" />
                <xsl:value-of select="o:nl()" />

                <!-- Include directories -->
                <xsl:value-of select="o:cmake_comment('Include the RE_PATH directory')" />
                <xsl:value-of select="o:cmake_include_dir('${RE_PATH}/src')" />
                <xsl:value-of select="o:nl()" />

                <!-- add library/link -->
                <xsl:value-of select="concat('add_library(${PROJ_NAME} SHARED ${SOURCE} ${HEADERS})', o:nl())" />
                <xsl:value-of select="concat('target_link_libraries(${PROJ_NAME} ${RE_CORE_LIBRARIES})', o:nl())" />

                <xsl:for-each-group select="$required_datatypes" group-by=".">
                    <xsl:variable name="datatype" select="lower-case(.)" />
                    <xsl:value-of select="concat('target_link_libraries(${PROJ_NAME} datatype_', $datatype, ')', o:nl())" />
                </xsl:for-each-group>
            </xsl:result-document>
        </xsl:for-each>
    </xsl:template>
</xsl:stylesheet>
