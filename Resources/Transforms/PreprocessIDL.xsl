<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:gml="http://graphml.graphdrawing.org/xmlns"
	xmlns="http://graphml.graphdrawing.org/xmlns"
	xmlns:exsl="http://exslt.org/common"
	xmlns:xalan="http://xml.apache.org/xslt"
	exclude-result-prefixes="gml #default exsl xalan">
	
    <xsl:output method="xml" 
        version="1.0" 
        indent="yes" 
		xalan:indent-amount="4"/>
	<xsl:strip-space elements="*" />
 
    <!--
        Purpose:

        This template will convert a .graphml project to a IDL.graphml 
		that includes a SortOrder for Attributes. This is required for 
		the graphml2IDL since the dependency order of the IDL information
		is important for correct processing of the IDL.
    -->
	
	<!-- Assign data keys standard values, defaults back to original document ids for required transforms -->
	<xsl:include href="graphmlKeyVariables.xsl" />
	
	<!-- apply all templates starting from the root -->
    <xsl:template match="/">
	    <!-- Output start of graph -->
		<graphml xmlns="http://graphml.graphdrawing.org/xmlns" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:schemaLocation="http://graphml.graphdrawing.org/xmlns" >
			<!-- Assign id's for data keys -->
			<!--    convention is to have id="d0..." but yEd or MEDEA may reassign ... -->
			
			<!-- Default yEd data keys, keep original file ids if already existing -->
			<xsl:variable name="urlnodeFound" select="./gml:graphml/gml:key[@attr.name='url'][@for='node']" />
			<xsl:if test="not($urlnodeFound)">
				<key attr.name="url" attr.type="string" for="node" id="{$nodeURLKey}"/>
			</xsl:if>
			
			<xsl:variable name="descriptionnodeFound" select="./gml:graphml/gml:key[@attr.name='description'][@for='node']" />
			<xsl:if test="not($descriptionnodeFound)">
				<key attr.name="description" attr.type="string" for="node" id="{$nodeDescriptionKey}"/>
			</xsl:if>
			
			<xsl:variable name="DescriptiongraphFound" select="./gml:graphml/gml:key[@attr.name='Description'][@for='graph']" />
			<xsl:if test="not($DescriptiongraphFound)">
				<key attr.name="Description" attr.type="string" for="graph" id="{$graphDescriptionKey}"/>
			</xsl:if>

			<xsl:variable name="urledgeFound" select="./gml:graphml/gml:key[@attr.name='url'][@for='edge']" />
			<xsl:if test="not($urledgeFound)">
				<key attr.name="url" attr.type="string" for="edge" id="{$edgeURLKey}"/>
			</xsl:if>
			
			<xsl:variable name="descriptionedgeFound" select="./gml:graphml/gml:key[@attr.name='description'][@for='edge']" />
			<xsl:if test="not($descriptionedgeFound)">
				<key attr.name="description" attr.type="string" for="edge" id="{$edgeDescriptionKey}"/>
			</xsl:if>
			
			<!-- need a sortOrder key -->
			<xsl:variable name="sortOrdernodeFound" select="./gml:graphml/gml:key[@attr.name='sortOrder'][@for='node']" />
			<xsl:if test="not($sortOrdernodeFound)">
				<key attr.name="sortOrder" attr.type="string" for="node" id="{$nodeSortOrderKey}"/>
			</xsl:if>
			
			<!-- PICML specific attributes, keep original file key ids -->
			<xsl:for-each select="./gml:graphml/gml:key">
				<xsl:apply-templates mode="copy" select="."/>
			</xsl:for-each> 
			
			<!-- Output all nodes from original graphml, removing yEd specific formatting -->		
			<xsl:apply-templates select="gml:graphml"/>

		<!-- Output end of graph -->
		</graphml>
    </xsl:template>
	
 	<!-- copy all existing keys without namespaces -->
	<xsl:template match="gml:key" mode="copy">
		<xsl:element name="{local-name(.)}" >
			<xsl:apply-templates select="@*|node()" mode="copy" />
		</xsl:element>
    </xsl:template>
	
	 <!-- copy key default tag without namespaces -->
	<xsl:template match="gml:default" mode="copy">
		<xsl:element name="{local-name(.)}" >
			<xsl:apply-templates select="@*|node()" mode="copy" />
		</xsl:element>
    </xsl:template>
	
	<xsl:template match="gml:graphml">
		<!-- Assign the transform node key ids for sortOrder from existing keys -->
		<xsl:variable name="sortOrderNodeFound" select="./gml:key[@attr.name='sortOrder'][@for='node']" />
		<xsl:variable name="transformNodeSortOrderKey">
			<xsl:choose>
				<xsl:when test="not($sortOrderNodeFound)">
					<xsl:value-of select="$nodeSortOrderKey"/>
				</xsl:when>
				<xsl:otherwise>
					<xsl:value-of select="$sortOrderNodeFound/@id"/>
				</xsl:otherwise>
			</xsl:choose>
		</xsl:variable>
			
		<xsl:variable name="sortKindNodeFound" select="./gml:key[@attr.name='kind'][@for='node']" />
		<xsl:variable name="transformNodeKindKey">
			<xsl:choose>
				<xsl:when test="not($sortKindNodeFound)">
					<xsl:value-of select="$nodeKindKey"/>
				</xsl:when>
				<xsl:otherwise>
					<xsl:value-of select="$sortKindNodeFound/@id"/>
				</xsl:otherwise>
			</xsl:choose>
		</xsl:variable>
		
	    <xsl:apply-templates select="gml:graph">
		    <xsl:with-param name="graphId" select="'G'" />
			<xsl:with-param name="transformNodeSortOrderKey" select="$transformNodeSortOrderKey" />
			<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey" />
   	    </xsl:apply-templates>
    </xsl:template>

	<!-- select global graph for this file -->
	<xsl:template match="gml:graph">
	    <xsl:param name="graphId" />
		<xsl:param name="transformNodeSortOrderKey" />
		<xsl:param name="transformNodeKindKey" />
		
		<graph edgedefault="directed" id="{$graphId}">
		
		<xsl:apply-templates select="gml:node">
			<xsl:with-param name="transformNodeSortOrderKey" select="$transformNodeSortOrderKey" />
			<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey" />
		</xsl:apply-templates>
		
		<xsl:for-each select="gml:edge">
			<xsl:apply-templates select="." mode="copy" />
		</xsl:for-each>         
		
		</graph>
		
	</xsl:template>
	
   	<!-- select nodes in graph -->
	<xsl:template match="gml:node">
		<xsl:param name="transformNodeSortOrderKey" />
		<xsl:param name="transformNodeKindKey" />
		
	    <xsl:variable name="nodeID" select="@id" />
	    <node id="{$nodeID}"> 

			<xsl:choose>
				<!-- if sortOrder is not present in graphml add -->
				<xsl:when test="(./gml:data[@key=$transformNodeKindKey]/text() = 'Aggregate') ">
					<xsl:variable name="interfaceDefs" select="/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'InterfaceDefinitions']/.." />

					<xsl:variable name="DepthOrder" >
						<!-- Need to produce a sortOrder such that the following statement orders the IDL generation transform -->
						<!-- <xsl:sort select="./gml:data[@key=$transformNodeSortOrderKey]" data-type="number" order="ascending" /> -->
						<xsl:call-template name="dependencyDepth">
							<xsl:with-param name="interfaceDefs" select="$interfaceDefs"/>
							<xsl:with-param name="node" select="."/>
							<xsl:with-param name="depth" select="0" />
							<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey" />
						</xsl:call-template> 
					</xsl:variable> 
					<xsl:variable name="SortOrder" select="9 - $DepthOrder" />
					<!-- modify sortOrder if present in graphml, copy all other data -->
					<xsl:for-each select="gml:data">
						<xsl:choose>
							<xsl:when test="./@key = $transformNodeSortOrderKey" >
								<!-- overwriten as below -->
							</xsl:when>
							<xsl:otherwise>
								<xsl:apply-templates select="." mode="copy" />
							</xsl:otherwise>
						</xsl:choose>
					</xsl:for-each>
					<data key="{$transformNodeSortOrderKey}">
					<xsl:value-of select="$SortOrder" />
					</data>
				</xsl:when>
				<xsl:otherwise>
					<xsl:for-each select="gml:data">
						<xsl:apply-templates select="." mode="copy" />
					</xsl:for-each>
				</xsl:otherwise>
			</xsl:choose>
			
			<xsl:apply-templates select="gml:graph">
				<xsl:with-param name="graphId" select="concat($nodeID,':')" />
				<xsl:with-param name="transformNodeSortOrderKey" select="$transformNodeSortOrderKey" />
				<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey" />
			</xsl:apply-templates>
        </node>
    </xsl:template>
	
	<!-- copy all existing node data -->
    <xsl:template match="gml:data" mode="copy">

		<xsl:element name="{local-name(.)}" >
			<xsl:apply-templates select="@*|node()" mode="copy" />
		</xsl:element>
    </xsl:template>
	
   	<!-- copy edges in graph -->
   	<xsl:template match="gml:edge" mode="copy" >
		
		<xsl:element name="{local-name(.)}" >
			<xsl:apply-templates select="@*|node()" mode="copy" />
			<!-- <xsl:apply-templates select="./gml:data" mode="copy" /> -->
		</xsl:element>
	</xsl:template> 
	
	<xsl:template match="@*|text()|comment()" mode="copy">
		<xsl:copy/>
	</xsl:template>

	<!-- Follow dependency edges and return depth of this Aggregate -->
	 <xsl:template name="dependencyDepth">
		<xsl:param name="interfaceDefs" />
		<xsl:param name="node" />
		<xsl:param name="depth" />
		<xsl:param name="transformNodeKindKey" />
		
		<xsl:variable name="refNodeIds" select="/descendant::*/gml:edge[@target=$node/@id]/@source" />
		<xsl:variable name="refParentNodes" select="$interfaceDefs/descendant::*/gml:node[@id=$refNodeIds]/gml:data[@key=$transformNodeKindKey][text() = 'AggregateInstance']/../../.." />
		<xsl:choose>
			<xsl:when test="$depth > 9" >
				<xsl:value-of select="'9'"/>
			</xsl:when>
			<xsl:when test="count($refParentNodes) = 0" >
				<xsl:value-of select="$depth"/>
			</xsl:when>
			<xsl:otherwise>
				<xsl:call-template name="dependencyDepth">
					<xsl:with-param name="interfaceDefs" select="$interfaceDefs"/>
					<xsl:with-param name="node" select="$refParentNodes"/>
					<xsl:with-param name="depth" select="$depth + 1" />
					<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey" />
				</xsl:call-template> 
			</xsl:otherwise>
		</xsl:choose>

	</xsl:template>
	
</xsl:stylesheet>