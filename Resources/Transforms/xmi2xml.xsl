<?xml version="1.0"?>
<xsl:stylesheet version="2.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:xs="http://www.w3.org/2001/XMLSchema"
	xmlns:uml="http://schema.omg.org/spec/UML/2.1"
	xmlns:xmi="http://schema.omg.org/spec/XMI/2.1"
	xmlns:xalan="http://xml.apache.org/xslt"
	exclude-result-prefixes="xmi uml xalan"
>
<!-- Runtime parameters -->
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
                            <xsl:variable name="class_attributes" select="ownedAttribute[@xmi:type='uml:Property' and @visibility='public']" />
                            <xsl:variable name="class_name" select="@name" />
                            <xsl:variable name="class_id" select="@xmi:id" />
                            <class name="{$class_name}" id="{$class_id}">
                                <xsl:for-each select="$class_attributes">
                                    <xsl:variable name="attribute_name" select="./@name" />
                                    <attribute name="{$attribute_name}"/>
                                </xsl:for-each>
                            </class>
                        </xsl:for-each>
                    </package>
                    </xsl:when>
                </xsl:choose>
            </xsl:for-each>
        </doc>
    </xsl:template>

    <xsl:template name="get_property_kind">
        <xsl:param name="type" />
        <xsl:variable name="kind" select="//packagedElement[@xmi:id=$type]/@name" />
        <xsl:choose>
            <xsl:when test="$kind">
                <xsl:value-of select="$kind"/>
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="$type"/>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>
</xsl:stylesheet>

