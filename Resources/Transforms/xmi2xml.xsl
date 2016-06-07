<?xml version="1.0"?>
<xsl:stylesheet version="2.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:xs="http://www.w3.org/2001/XMLSchema"
	xmlns:uml="http://schema.omg.org/spec/UML/2.1"
	xmlns:xmi="http://schema.omg.org/spec/XMI/2.1"
	xmlns:xalan="http://xml.apache.org/xslt"
	exclude-result-prefixes="xmi uml xalan"
>

<!-- Represents the list of ID's that should be included in the transform. (CSV ID's) -->
<xsl:param name="class_ids"></xsl:param>
	
<xsl:output method="xml" version="1.0" indent="yes" standalone="no" xalan:indent-amount="4"/>
    <xsl:template match="/">
        <!-- Select all packagedElement from document -->
        <doc>
            <xsl:for-each select="//packagedElement[@xmi:type='uml:Package']">
                <xsl:variable name="package_name" select="@name" />
                <xsl:variable name="package_id" select="@xmi:id" />
                <xsl:variable name="package_classes" select="packagedElement[@xmi:type='uml:Class']" />

                <xsl:choose>
                    <xsl:when test="count($package_classes) > 0">
                    <package name="{$package_name}" id="{$package_id}">
                        <xsl:for-each select="$package_classes">
                            <xsl:variable name="class_name" select="@name" />
                            <xsl:variable name="class_id" select="@xmi:id" />
                            <xsl:variable name="required_class_ids" select="../packagedElement[@xmi:type='uml:Dependency' and @client=$class_id]" />
                            <xsl:variable name="class_attributes" select="ownedAttribute[@xmi:type='uml:Property' and @visibility='public']" />
                            <class name="{$class_name}" id="{$class_id}" attribute_count="{count($class_attributes)}">
                                <xsl:for-each select="$required_class_ids">
                                    <requires id="{@supplier}"/>
                                </xsl:for-each>
                            </class>
                        </xsl:for-each>
                    </package>
                    </xsl:when>
                </xsl:choose>
            </xsl:for-each>
        </doc>
    </xsl:template>
</xsl:stylesheet>

