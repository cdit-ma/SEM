<?xml version="1.0"?>
<xsl:stylesheet version="2.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:gml="http://graphml.graphdrawing.org/xmlns"
	xmlns="http://graphml.graphdrawing.org/xmlns"
	xmlns:uml="http://schema.omg.org/spec/UML/2.1"
	xmlns:xmi="http://schema.omg.org/spec/XMI/2.1"
	xmlns:exsl="http://exslt.org/common"
	xmlns:xalan="http://xml.apache.org/xslt"
	exclude-result-prefixes="gml #default exsl xalan">

<xsl:output method="xml" version="1.0" indent="yes" standalone="no" xalan:indent-amount="4"/>
    <xsl:template match="/">
        <model>
            <!-- Select all 'uml:Package' packagedElement elements from the document -->
            <xsl:for-each select="//packagedElement[@xmi:type='uml:Package']">
                <xsl:variable name="name" select="@name" />
                <xsl:variable name="id" select="@xmi:id" />
                <xsl:variable name="classes" select="packagedElement[@xmi:type='uml:Class']" />
                <xsl:if test="count($classes) > 0">
                    <package name="{$name}" id="{$id}">
                        <!-- Select all child 'uml:Class' packagedElement elements -->
                        <xsl:for-each select="$classes">
                            <xsl:variable name="class_name" select="@name" />
                            <xsl:variable name="class_id" select="@xmi:id" />
                            <xsl:variable name="class_attributes" select="ownedAttribute[@xmi:type='uml:Property' and @visibility='public']" />
                            <class name="{$class_name}" id="{$class_id}" attribute_count="{count($class_attributes)}" />
                        </xsl:for-each>
                    </package>
                </xsl:if>
            </xsl:for-each>
        </model>
    </xsl:template>
</xsl:stylesheet>
