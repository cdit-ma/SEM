<?xml version="1.0"?>
<xsl:stylesheet version="2.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns:uml="http://schema.omg.org/spec/UML/2.1"
	xmlns:xmi="http://schema.omg.org/spec/XMI/2.1"
	xmlns:xalan="http://xml.apache.org/xslt"
	xmlns:exsl="http://exslt.org/common"
	xmlns:gml="http://graphml.graphdrawing.org/xmlns"
	exclude-result-prefixes="xmi uml xalan gml #default exsl"
>
<!-- Runtime parameters -->
<xsl:param name="class_ids" />
<xsl:include href="graphmlKeyVariables.xsl" />

<xsl:output method="xml" 
        version="1.0" 
        indent="yes" xalan:indent-amount="4"/>
	
	<xsl:strip-space elements="*" />
	
	
    <xsl:template match="/">
    <graphml
			xmlns="http://graphml.graphdrawing.org/xmlns"  
			xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
			xsi:schemaLocation="http://graphml.graphdrawing.org/xmlns http://graphml.graphdrawing.org/xmlns/1.0/graphml.xsd" >

			<!-- PICML specific attributes -->
			<key attr.name="description" attr.type="string" for="node" id="{$nodeDescriptionKey}"/>
			<key attr.name="label" attr.type="string" for="node" id="{$nodeLabelKey}"/>
			<key attr.name="x" attr.type="double" for="node" id="{$nodeXKey}"/>
			<key attr.name="y" attr.type="double" for="node" id="{$nodeYKey}"/>
			<key attr.name="kind" attr.type="string" for="node" id="{$nodeKindKey}"/>
			<key attr.name="type" attr.type="string" for="node" id="{$nodeTypeKey}"/>
			<key attr.name="value" attr.type="string" for="node" id="{$nodeValueKey}"/>
			<key attr.name="key" attr.type="boolean" for="node" id="{$nodeKeyMemberKey}"/>
			<key attr.name="width" attr.type="double" for="node" id="{$nodeWidthKey}"/>
			<key attr.name="height" attr.type="double" for="node" id="{$nodeHeightKey}"/>

			<key attr.name="sortOrder" attr.type="double" for="node" id="{$nodeSortOrderKey}"/>
			<graph edgedefault="directed" id="G">
				<!-- Select all packagedElement from document -->
				<xsl:for-each select="//packagedElement[@xmi:type='uml:Package']">
					<xsl:variable name="package_name" select="@name" />
					<xsl:variable name="package_id" select="@xmi:id" />
					<xsl:variable name="package_classes" select="packagedElement[@xmi:type='uml:Class']" />
					
					<xsl:call-template name="write_idl">
						<xsl:with-param name="id" select="$package_id" />
						<xsl:with-param name="label" select="$package_name" />
					</xsl:call-template>
				</xsl:for-each>
			</graph>
		</graphml>
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
    
    
    <xsl:template name="splitList">
		<xsl:param name="list" />
		<xsl:param name="delim" />
		
		<xsl:if test="string-length($list) > 0">
			<xsl:variable name="element" select="substring-before(concat($list, $delim), $delim)"/>
			<xsl:value-of select="concat('  ', $element, '&#xA;')"/>
			<xsl:call-template name="splitList">
				<xsl:with-param name="list" select="substring-after($list, $delim)"/>
				<xsl:with-param name="delim" select="$delim" />
			</xsl:call-template> 
		</xsl:if>
	</xsl:template>
	
	
	<xsl:template name="write_idl">
		<xsl:param name="id" />
		<xsl:param name="label" />
		<xsl:param name="aggregates" select="packagedElement[@xmi:type='uml:Class']"/>
		<xsl:if test="count($aggregates) > 0">
			<node id="{$id}">
				<data key="{$nodeKindKey}">IDL</data>
				<data key="{$nodeLabelKey}"><xsl:value-of select="$label"/></data>
				<xsl:if test="count($aggregates) > 0">
				<graph id="{$id}g">
					<xsl:for-each select="$aggregates">
						<xsl:variable name="aggregate_label" select="@name" />
						<xsl:variable name="aggregate_id" select="@xmi:id" />
						<xsl:variable name="aggregate_members" select="ownedAttribute[@xmi:type='uml:Property' and @visibility='public']" />
						<xsl:call-template name="write_aggregate">
								<xsl:with-param name="id" select="$aggregate_id" />
								<xsl:with-param name="label" select="$aggregate_label" />
								<xsl:with-param name="members" select="$aggregate_members" />
						</xsl:call-template>
					</xsl:for-each>
					
				</graph>
				</xsl:if>
			</node>
		</xsl:if>

	</xsl:template>
	
	
	<xsl:template name="write_aggregate">
		<xsl:param name="id" />
		<xsl:param name="label" />
		<xsl:param name="members" />
		<xsl:if test="count($members) > 0">
			<xsl:if test="contains($class_ids, $id)">
				<node id="{$id}">
					<data key="{$nodeKindKey}">Aggregate</data>
					<data key="{$nodeLabelKey}"><xsl:value-of select="$label"/></data>
					<graph id="{$id}g">
						<xsl:for-each select="$members">
							<xsl:variable name="member_label" select="@name" />
							<xsl:variable name="member_id" select="@xmi:id" />
							<xsl:variable name="member_type">
								<xsl:call-template name="get_property_kind">
									<xsl:with-param name="type" select="type/@xmi:idref" />
								</xsl:call-template>
							</xsl:variable>
							<xsl:call-template name="write_member">
									<xsl:with-param name="id" select="$member_id" />
									<xsl:with-param name="label" select="$member_label" />
									<xsl:with-param name="type" select="$member_type" />
							</xsl:call-template>
						</xsl:for-each>
					</graph>
				</node>
			</xsl:if>
		</xsl:if>
	</xsl:template>

	<xsl:template name="write_member">
		<xsl:param name="id" />
		<xsl:param name="label" />
		<xsl:param name="type" />
		<node id="{$id}">
			<data key="{$nodeKindKey}">Member</data>
			<data key="{$nodeLabelKey}"><xsl:value-of select="$label"/></data>
			<data key="{$nodeTypeKey}"><xsl:value-of select="$type"/></data>
		</node>
	</xsl:template>
	

</xsl:stylesheet>

