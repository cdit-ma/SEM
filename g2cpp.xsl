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

    <xsl:template match="/">
        <xsl:for-each select="$aggregates">
            <xsl:variable name="members" as="element()*" select="cdit:get_entities_of_kind(.,'Member')" />
            <xsl:variable name="aggregate_label" select ="cdit:get_key_value(.,'label')" />
            <xsl:variable name="l_label" select="lower-case($aggregate_label)" />


            <!-- OPEN FILE *.cpp -->
            <xsl:result-document href="{concat('datatypes/',$l_label, '/', $l_label, '.cpp')}">

            <xsl:value-of select="concat('#include ', o:local_include(concat($l_label, '.h')), o:nl())" />

            <!-- Public Functions -->
            <xsl:for-each select="$members">
                <xsl:variable name="member_label" select="cdit:get_key_value(.,'label')" />
                <xsl:variable name="member_type" select="cdit:get_key_value(.,'type')" />
                <xsl:variable name="member_cpp_type" select="cdit:get_cpp_type($member_type)" />

                <xsl:value-of select="concat(o:nl(), '//Basic Member: ', $member_label, '[', $member_type, ']', o:nl())" />
                <!-- SETTER -->
                <xsl:value-of select="concat('void ', $aggregate_label, '::set_', $member_label, '(const ', $member_cpp_type, ' val){', o:nl())" />
                <xsl:value-of select="concat(o:t(1), 'this->', $member_label, '_ = val;', o:nl())" />
                <xsl:value-of select="'};'" />

                <xsl:value-of select="o:nl()" />

                <!-- Copy Getter -->
                <xsl:value-of select="concat(o:nl(), $member_cpp_type, ' ', $aggregate_label, '::get_', $member_label, '(){', o:nl())" />
                <xsl:value-of select="concat(o:t(1), 'return this->', $member_label, '_;', o:nl())" />
                <xsl:value-of select="'};'" />

                <xsl:value-of select="o:nl()" />

                <!-- Copy Getter -->
                <xsl:value-of select="concat(o:nl(), $member_cpp_type, '* ', $aggregate_label, '::', $member_label, '(){', o:nl())" />
                <xsl:value-of select="concat(o:t(1), 'return ', o:and(), '(this->', $member_label, '_);', o:nl())" />
                <xsl:value-of select="'};'" />



                <xsl:value-of select="o:nl()" />

                <!-- GETTER
                <xsl:value-of select="concat(o:t(2), $member_cpp_type, ' get_', $member_label, '();', o:nl())" />
                
                <xsl:value-of select="concat(o:t(1), 'private:', o:nl())" />
                <xsl:value-of select="concat(o:t(2), $member_cpp_type, ' __', $member_label, '__;', o:nl())" /> -->
            </xsl:for-each>

             </xsl:result-document>
        </xsl:for-each>
    </xsl:template>
</xsl:stylesheet>
