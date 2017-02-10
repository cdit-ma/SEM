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
                <xsl:value-of select="concat('#Get the RE_PATH', o:nl())" />
                <xsl:value-of select="concat('set(RE_PATH ', o:dblquote_wrap('$ENV{RE_PATH}'), ')', o:nl())" />
                <xsl:value-of select="o:nl()" />

                <!-- Set PROJ_NAME -->
                <xsl:value-of select="concat('set(PROJ_NAME datatypes_', $l_label, ')', o:nl())" />
                <xsl:value-of select="concat('project(${PROJ_NAME})', o:nl())" />

                <!-- Find re_core -->
                <xsl:value-of select="o:nl()" />
                <xsl:value-of select="concat('#Find re_core library', o:nl())" />
                <xsl:value-of select="concat('find_library(RE_CORE_LIBRARIES re_core ', o:dblquote_wrap('${RE_PATH}/lib'), ')', o:nl())" />
                <xsl:value-of select="o:nl()" />

                <!-- SOURCE -->
                <xsl:value-of select="concat('set(SOURCE', o:nl())" />
                <xsl:value-of select="concat(o:t(1), '${CMAKE_CURRENT_SOURCE_DIR}/', $l_label, '.cpp', o:nl())" />
                <xsl:value-of select="concat(o:t(1), ')', o:nl())" />
                <xsl:value-of select="o:nl()" />

                <!-- HEADERS -->
                <xsl:value-of select="concat('set(HEADERS', o:nl())" />
                <xsl:value-of select="concat(o:t(1), '${CMAKE_CURRENT_SOURCE_DIR}/', $l_label, '.h', o:nl())" />
                <xsl:value-of select="concat(o:t(1), ')', o:nl())" />
                <xsl:value-of select="o:nl()" />

                <!-- Include RE_PATH/src -->
                <xsl:value-of select="concat('#Add RE_PATH to include directories', o:nl())" />
                <xsl:value-of select="concat('include_directories(', o:dblquote_wrap('${RE_PATH}/src'), ')', o:nl())" />
                <xsl:value-of select="o:nl()" />

                <!-- add library/link -->
                <xsl:value-of select="concat('add_library(${PROJ_NAME} SHARED ${SOURCE} ${HEADERS})', o:nl())" />
                <xsl:value-of select="concat('target_link_libraries(${PROJ_NAME} ${RE_CORE_LIBRARIES})', o:nl())" />
            </xsl:result-document>

            <!-- OPEN FILE *.h -->
            <xsl:result-document href="{concat('datatypes/', $l_label, '/', $l_label, '.h')}">

            <xsl:variable name="define_guard" select="upper-case(concat('PORTS_', $aggregate_label, '_H'))" />

            <xsl:value-of select="concat('#ifndef ', $define_guard, o:nl())" />
            <xsl:value-of select="concat('#define ', $define_guard, o:nl())" />


            <xsl:value-of select="concat(o:nl(), '//Include Statements', o:nl())" />
            <xsl:value-of select="concat('#include ', o:lib_include('core/basemessage.h'), o:nl())" />
            <xsl:value-of select="concat('#include ', o:lib_include('string'), o:nl())" />


            <xsl:value-of select="concat(o:nl(), 'class ', o:camel_case($aggregate_label), ' : public BaseMessage{', o:nl())" />
            
            <!-- Public Functions -->
            <xsl:for-each select="$members">
                <xsl:variable name="member_label" select="cdit:get_key_value(.,'label')" />
                <xsl:variable name="member_type" select="cdit:get_key_value(.,'type')" />

                <xsl:variable name="member_cpp_type" select="cdit:get_cpp_type($member_type)" />

                <xsl:value-of select="concat(o:nl(), o:t(1), '//Basic Member: ', $member_label, '[', $member_type, ']', o:nl())" />
                <xsl:value-of select="concat(o:t(1), 'public:', o:nl())" />
                <!-- SETTER -->
                <xsl:value-of select="concat(o:t(2), 'void set_', $member_label, '(const ', $member_cpp_type, ' val);', o:nl())" />
                <!-- Copy Getter -->
                <xsl:value-of select="concat(o:t(2), $member_cpp_type, ' get_', $member_label, '();', o:nl())" />
                <!-- Inplace Getter -->
                <xsl:value-of select="concat(o:t(2), $member_cpp_type, '* ', $member_label, '();', o:nl())" />
                
                <xsl:value-of select="concat(o:t(1), 'private:', o:nl())" />
                <!-- GETTER -->
                <xsl:value-of select="concat(o:t(2), $member_cpp_type, ' ', $member_label, '_;', o:nl())" />
            </xsl:for-each>

            <xsl:value-of select="concat('};', o:nl())" />
            <xsl:value-of select="concat(o:nl(), '#endif //', $define_guard, o:nl())" />

             </xsl:result-document>
        </xsl:for-each>
    </xsl:template>
</xsl:stylesheet>
