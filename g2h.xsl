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
	
    <xsl:variable name="doc_root">
        <xsl:value-of select="/"/>
    </xsl:variable>

    <xsl:variable name="aggregates" as="element()*" select="cdit:get_entities_of_kind(., 'Aggregate')" />

    <!-- apply all templates starting from the root -->
    <xsl:template match="/">
        <xsl:for-each select="$aggregates">
            <xsl:variable name="members" as="element()*" select="cdit:get_entities_of_kind(.,'Member')" />

            <xsl:variable name="aggregate_label" select ="cdit:get_key_value(.,'label')" />
            <xsl:variable name="l_label" select="lower-case($aggregate_label)" />


            <!-- OPEN CMakeLists.txt -->
            <xsl:result-document href="{concat('datatypes/', $l_label, '/', 'CMakeLists.txt')}">
                <!-- set RE_PATH -->
                <xsl:value-of select="o:cmake_comment('Get the RE_PATH')" />
                <xsl:value-of select="o:cmake_set_env('RE_PATH', '$ENV{RE_PATH}')" />
                <xsl:value-of select="o:nl()" />

                <!-- Set PROJ_NAME -->
                <xsl:variable name="proj_name" select="concat('datatype_', $l_label)" />
                <xsl:value-of select="o:cmake_set_proj_name($proj_name)" />

                <!-- Find re_core -->
                <xsl:value-of select="o:nl()" />
                <xsl:value-of select="o:cmake_find_library('re_core', 'RE_CORE_LIBRARIES', '${RE_PATH}/lib')" />
                <xsl:value-of select="o:nl()" />

                <!-- Set Source files -->
                <xsl:value-of select="concat('set(SOURCE', o:nl())" />
                <xsl:value-of select="concat(o:t(1), '${CMAKE_CURRENT_SOURCE_DIR}/', $l_label, '.cpp', o:nl())" />
                <xsl:value-of select="concat(o:t(1), ')', o:nl())" />
                <xsl:value-of select="o:nl()" />

                <!-- Set Headers files -->
                <xsl:value-of select="concat('set(HEADERS', o:nl())" />
                <xsl:value-of select="concat(o:t(1), '${CMAKE_CURRENT_SOURCE_DIR}/', $l_label, '.h', o:nl())" />
                <xsl:value-of select="concat(o:t(1), ')', o:nl())" />
                <xsl:value-of select="o:nl()" />

                <!-- Include directories -->
                <xsl:value-of select="o:cmake_comment('Include the RE_PATH directory')" />
                <xsl:value-of select="o:cmake_include_dir('${RE_PATH}/src')" />
                <xsl:value-of select="o:nl()" />

                <!-- add library/link -->
                <xsl:value-of select="concat('add_library(${PROJ_NAME} SHARED ${SOURCE} ${HEADERS})', o:nl())" />
                <xsl:value-of select="concat('target_link_libraries(${PROJ_NAME} ${RE_CORE_LIBRARIES})', o:nl())" />
            </xsl:result-document>

            <!-- OPEN FILE *.h -->
            <xsl:result-document href="{concat('datatypes/', $l_label, '/', $l_label, '.h')}">

            <xsl:variable name="class_name" select="o:camel_case($aggregate_label)" />
            <xsl:variable name="define_guard" select="upper-case(concat('PORTS_', $aggregate_label, '_H'))" />

            <!-- Define Guard -->
            <xsl:value-of select="o:define_guard($define_guard)" />

            <!-- Include Functions -->
            <xsl:value-of select="o:cpp_comment('Include Statements')" />
            <xsl:value-of select="o:lib_include('core/basemessage.h')" />
            <xsl:value-of select="o:lib_include('string')" />
            <xsl:value-of select="o:nl()" />

            <!-- Define the class, Inherits from BaseMessage -->
            <xsl:value-of select="concat('class ', $class_name, ' : public BaseMessage{', o:nl())" />
            
            <!-- Public Functions -->
            <xsl:for-each select="$members">
                <xsl:variable name="member_label" select="cdit:get_key_value(.,'label')" />
                <xsl:variable name="member_type" select="cdit:get_key_value(.,'type')" />

                <xsl:variable name="member_cpp_type" select="cdit:get_cpp_type($member_type)" />

                <xsl:value-of select="concat(o:nl(), o:t(1))" />
                <xsl:value-of select="o:cpp_comment(concat('Basic Member: ', $member_label, ' [', $member_type, ']'))" />

                <!-- Public Declarations -->
                <xsl:value-of select="concat(o:t(1), 'public:', o:nl())" />
                <!-- Setter -->
                <xsl:value-of select="o:cpp_func_decl('void', concat('set_', $member_label), concat('const ', $member_cpp_type, ' val'))" />
                <!-- Copy Getter -->
                <xsl:value-of select="o:cpp_func_decl($member_cpp_type, concat('get_', $member_label), '')" />
                <!-- Inplace Getter -->
                <xsl:value-of select="o:cpp_func_decl(concat($member_cpp_type, '*'), $member_label, '')" />

                <!-- Private Declarations -->
                <xsl:value-of select="concat(o:t(1), 'private:', o:nl())" />
                <!-- Variable -->
                <xsl:value-of select="o:cpp_var_decl($member_cpp_type, concat($member_label,'_'))" />
            </xsl:for-each>

            <xsl:value-of select="concat('};', o:nl())" />
            <xsl:value-of select="o:define_guard_end($define_guard)" />

             </xsl:result-document>
        </xsl:for-each>
    </xsl:template>
</xsl:stylesheet>
