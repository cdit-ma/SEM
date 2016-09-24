<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="2.0" 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:gml="http://graphml.graphdrawing.org/xmlns"

	xmlns:exsl="http://exslt.org/common"
	xmlns:xalan="http://xml.apache.org/xslt"	
	xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" 
	xsi:schemaLocation="http://www.omg.org/Deployment Deployment.xsd"
	xmlns:redirect = "org.apache. xalan.xslt.extensions.Redirect"
	
	exclude-result-prefixes="gml exsl xalan">
		
    <xsl:output method="xml" 
        version="1.0" 
        indent="yes" 
		standalone="no"
		xalan:indent-amount="4"/>
	<xsl:strip-space elements="*" />
	
    <!--
        Purpose:

        This template will convert the .graphml project to a quality of service .ddd 
        file. The transform creates a xml file used for system deployment for dds type middleware.
		This produced file may be extended in the future to allow user settings for each Hardware Node.
    -->
	
	<!-- Runtime parameters -->
	<xsl:param name="HardwareNode">MainNode</xsl:param>

	<!-- Assign default data keys as used by yEd 3.12.2 -->
	<xsl:include href="graphmlKeyVariables.xsl" />
	
	<!-- apply all templates starting from the root -->
    <xsl:template match="/">
		
		<xsl:variable name="transformNodeLabelKey">
			<xsl:call-template name="findNodeKey">
				<xsl:with-param name="attrName" select="'label'" />
				<xsl:with-param name="defaultId" select="$nodeLabelKey" />
			</xsl:call-template>	
		</xsl:variable>
		
		<xsl:variable name="transformNodeKindKey">
			<xsl:call-template name="findNodeKey">
				<xsl:with-param name="attrName" select="'kind'" />
				<xsl:with-param name="defaultId" select="$nodeKindKey" />
			</xsl:call-template>	
		</xsl:variable>	
		
		<!-- Calculate number of participants and size the max_objects_per_thread to correct size -->
		<xsl:variable name="maxObjects">
			<xsl:choose>
			<xsl:when test="not(contains($HardwareNode, 'MainNode'))" >
			
				<!-- Search all components on this HardwareNode to find number of participants -->
				<xsl:variable name="thisNode" select="./descendant::*/gml:node/gml:data[@key=$transformNodeLabelKey][text() = $HardwareNode]/.." />
				<xsl:variable name="nodeId" select="$thisNode/@id"/>
				<xsl:variable name="componentIds" select="./descendant::*/gml:edge[@target=$nodeId]/@source" />
				<xsl:variable name="componentNodes" select="./descendant::*/gml:node[@id=$componentIds]/gml:data[@key=$transformNodeKindKey][text() = 'ComponentInstance']/.." />
								
				<!-- Count all OutEventPort of the Assembly --> 
				<xsl:variable name="countLocalOutEventPorts" select="count($componentNodes/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'OutEventPortInstance']/..)" />
				<!-- Count all InEventPort of the Assembly --> 
				<xsl:variable name="countLocalInEventPorts" select="count($componentNodes/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'OutEventPortInstance']/..)" />
				<xsl:value-of select="(round((($countLocalOutEventPorts + $countLocalInEventPorts + 11) * 100) div 1024) * 1024)" />  				  
			</xsl:when>
			
			<xsl:otherwise>
				<!-- Count all OutEventPort of the Assembly --> 
				<xsl:variable name="countOutEventPorts" select="count(./gml:graphml/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'OutEventPortInstance']/..)" />
				<!-- Count all InEventPort of the Assembly --> 
				<xsl:variable name="countInEventPorts" select="count(./gml:graphml/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'OutEventPortInstance']/..)" />
				<xsl:value-of select="(round((($countOutEventPorts + $countInEventPorts + 11) * 100) div 1024) * 1024)" /> 
	
			</xsl:otherwise>
			
			</xsl:choose>
		</xsl:variable>
		
		<!-- Output start of dds:domain -->
		<dds:domain xmlns="http://cuts.cs.iupui.edu/iccm" xmlns:dds="http://cuts.cs.iupui.edu/iccm" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:schemaLocation="http://cuts.cs.iupui.edu/iccm dds.xsd">

			<entity_factory autoenable_created_entities="true"/>
		
			<!-- default for RTIDDS is 1024 which allows for 11 participants per process -->
			<!-- Increase this max limit to increase the number of participants on each hardware node. -->
			<resource_limits max_objects_per_thread="{$maxObjects}"/>
			
		<!-- Output end of dds:domain -->
		</dds:domain>
			
    </xsl:template>
		

	<!-- find the key for specific attribute,  -->
	<xsl:template name="findNodeKey">
		<xsl:param name="attrName" />
		<xsl:param name="defaultId" />
		
                <xsl:variable name="found" select="/gml:graphml/gml:key[@attr.name=$attrName]" />
		<xsl:choose>
			<xsl:when test="not($found)">
				<xsl:value-of select="$defaultId"/>
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="$found/@id"/>
			</xsl:otherwise>
		</xsl:choose>
    </xsl:template>	
		
</xsl:stylesheet>
