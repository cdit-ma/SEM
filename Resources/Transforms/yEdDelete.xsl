<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:gml="http://graphml.graphdrawing.org/xmlns"
    xmlns:y="http://www.yworks.com/xml/graphml"
	xmlns="http://graphml.graphdrawing.org/xmlns"
	xmlns:exsl="http://exslt.org/common"
	xmlns:xalan="http://xml.apache.org/xslt"
	exclude-result-prefixes="gml #default y exsl xalan">
	
    <xsl:output method="xml" 
        version="1.0" 
        indent="yes" 
		xalan:indent-amount="4"/>
	<xsl:strip-space elements="*" />
 
    <!--
        Purpose:

        This template will convert the yEd.graphml project to a raw.graphml
        file. The transform removes yEd specific decorations and assigns x,y
		and label modifications to the key data fields. x and y translated back
		to relative values and rounded to three decimal places.
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
			<xsl:variable name="urlnodeFound" select="./gml:graphml/gml:key[@attr.name='url']" />
			<xsl:if test="not($urlnodeFound)">
				<key attr.name="url" attr.type="string" for="node" id="{$nodeURLKey}"/>
			</xsl:if>
			
			<xsl:variable name="descriptionnodeFound" select="./gml:graphml/gml:key[@attr.name='description']" />
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
			
			<!-- PICML specific attributes, keep original file key ids -->
			<xsl:for-each select="./gml:graphml/gml:key">
				<xsl:if test="not(./@yfiles.type)">
					<xsl:apply-templates mode="copy" select="."/>
				</xsl:if>
			</xsl:for-each> 
			
			<!-- Output all nodes from original graphml, removing yEd specific formatting -->		
			<xsl:apply-templates select="gml:graphml"/>

		<!-- Output end of graph -->
		</graphml>
    </xsl:template>
	
 	<!-- copy all existing keys without namespaces -->
	<xsl:template match="gml:key" mode="copy">
		<xsl:if test="not(./@yfiles.type)">
			<xsl:element name="{local-name(.)}" >
				<xsl:apply-templates select="@*|node()" mode="copy" />
			</xsl:element>
		</xsl:if>
    </xsl:template>
	
	 <!-- copy key default tag without namespaces -->
	<xsl:template match="gml:default" mode="copy">
		<xsl:element name="{local-name(.)}" >
			<xsl:apply-templates select="@*|node()" mode="copy" />
		</xsl:element>
    </xsl:template>
	
	<xsl:template match="gml:graphml">
		<!-- Assign the transform node key ids for X Y Label and Graphics from existing keys -->
		<xsl:variable name="xnodeFound" select="./gml:key[@attr.name='x']" />
		<xsl:variable name="transformNodeXKey">
			<xsl:choose>
				<xsl:when test="not($xnodeFound)">
					<xsl:value-of select="$nodeXKey"/>
				</xsl:when>
				<xsl:otherwise>
					<xsl:value-of select="$xnodeFound/@id"/>
				</xsl:otherwise>
			</xsl:choose>
		</xsl:variable>
			
		<xsl:variable name="ynodeFound" select="./gml:key[@attr.name='y']" />
		<xsl:variable name="transformNodeYKey">
			<xsl:choose>
				<xsl:when test="not($ynodeFound)">
					<xsl:value-of select="$nodeYKey"/>
				</xsl:when>
				<xsl:otherwise>
					<xsl:value-of select="$ynodeFound/@id"/>
				</xsl:otherwise>
			</xsl:choose>
		</xsl:variable>
		
		<xsl:variable name="labelnodeFound" select="./gml:key[@attr.name='label']" />
		<xsl:variable name="transformNodeLabelKey">
			<xsl:choose>
				<xsl:when test="not($labelnodeFound)">
					<xsl:value-of select="$nodeLabelKey"/>
				</xsl:when>
				<xsl:otherwise>
					<xsl:value-of select="$labelnodeFound/@id"/>
				</xsl:otherwise>
			</xsl:choose>
		</xsl:variable>
		
		<xsl:variable name="nodegraphicsFound" select="./gml:key[@yfiles.type='nodegraphics']" />
		<xsl:variable name="transformNodeGraphicsKey">
			<xsl:choose>
				<xsl:when test="not($nodegraphicsFound)">
					<xsl:value-of select="$nodeGraphicsKey"/>
				</xsl:when>
				<xsl:otherwise>
					<xsl:value-of select="$nodegraphicsFound/@id"/>
				</xsl:otherwise>
			</xsl:choose>
		</xsl:variable>	
				
		<xsl:variable name="edgegraphicsFound" select="./gml:key[@yfiles.type='edgegraphics'][@for='edge']" />
		<xsl:variable name="transformEdgeGraphicsKey">
			<xsl:choose>
				<xsl:when test="not($edgegraphicsFound)">
					<xsl:value-of select="$edgeGraphicsKey"/>
				</xsl:when>
				<xsl:otherwise>
					<xsl:value-of select="$edgegraphicsFound/@id"/>
				</xsl:otherwise>
			</xsl:choose>
		</xsl:variable>	

		<xsl:variable name="widthnodeFound" select="./gml:key[@attr.name='width']" />
		<xsl:variable name="transformNodeWidthKey">
			<xsl:choose>
				<xsl:when test="not($widthnodeFound)">
					<xsl:value-of select="$nodeWidthKey"/>
				</xsl:when>
				<xsl:otherwise>
					<xsl:value-of select="$widthnodeFound/@id"/>
				</xsl:otherwise>
			</xsl:choose>
		</xsl:variable>

		<xsl:variable name="heightnodeFound" select="./gml:key[@attr.name='height']" />
		<xsl:variable name="transformNodeHeightKey">
			<xsl:choose>
				<xsl:when test="not($heightnodeFound)">
					<xsl:value-of select="$nodeHeightKey"/>
				</xsl:when>
				<xsl:otherwise>
					<xsl:value-of select="$heightnodeFound/@id"/>
				</xsl:otherwise>
			</xsl:choose>
		</xsl:variable>
		
	    <xsl:apply-templates select="gml:graph">
		    <xsl:with-param name="graphId" select="'G'" />
			<xsl:with-param name="graphLabel" />
			<xsl:with-param name="graphX" select="0" />
			<xsl:with-param name="graphY" select="0" />
			<xsl:with-param name="transformNodeXKey" select="$transformNodeXKey" />
			<xsl:with-param name="transformNodeYKey" select="$transformNodeYKey" />
			<xsl:with-param name="transformNodeWidthKey" select="$transformNodeWidthKey" />
			<xsl:with-param name="transformNodeHeightKey" select="$transformNodeHeightKey" />
			<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey" />
			<xsl:with-param name="transformNodeGraphicsKey" select="$transformNodeGraphicsKey" />
			<xsl:with-param name="transformEdgeGraphicsKey" select="$transformEdgeGraphicsKey" />
   	    </xsl:apply-templates>
    </xsl:template>

	<!-- select global graph for this file -->
	<xsl:template match="gml:graph">
	    <xsl:param name="graphId" />
	    <xsl:param name="graphLabel" />
	    <xsl:param name="graphX" />
	    <xsl:param name="graphY" />
		<xsl:param name="transformNodeXKey" />
		<xsl:param name="transformNodeYKey" />
		<xsl:param name="transformNodeWidthKey" />
		<xsl:param name="transformNodeHeightKey" />
		<xsl:param name="transformNodeLabelKey" />
		<xsl:param name="transformNodeGraphicsKey" />
		<xsl:param name="transformEdgeGraphicsKey" />
		
		<graph edgedefault="directed" id="{$graphId}">
		
		<xsl:apply-templates select="gml:node">
			<xsl:with-param name="parentX" select="$graphX" />
			<xsl:with-param name="parentY" select="$graphY" />
			<xsl:with-param name="transformNodeXKey" select="$transformNodeXKey" />
			<xsl:with-param name="transformNodeYKey" select="$transformNodeYKey" />
			<xsl:with-param name="transformNodeWidthKey" select="$transformNodeWidthKey" />
			<xsl:with-param name="transformNodeHeightKey" select="$transformNodeHeightKey" />
			<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey" />
			<xsl:with-param name="transformNodeGraphicsKey" select="$transformNodeGraphicsKey" />
			<xsl:with-param name="transformEdgeGraphicsKey" select="$transformEdgeGraphicsKey" />
		</xsl:apply-templates>
		
		<xsl:for-each select="gml:edge">
			<xsl:apply-templates select="." mode="copy">
				<xsl:with-param name="transformNodeGraphicsKey" select="$transformNodeGraphicsKey" />
				<xsl:with-param name="transformEdgeGraphicsKey" select="$transformEdgeGraphicsKey" />
			</xsl:apply-templates>
		</xsl:for-each>         
		
		</graph>
		
	</xsl:template>
	
   	<!-- select nodes in graph -->
	<xsl:template match="gml:node">
		<xsl:param name="parentX" />
	    <xsl:param name="parentY" />
		<xsl:param name="transformNodeXKey" />
		<xsl:param name="transformNodeYKey" />
		<xsl:param name="transformNodeWidthKey" />
		<xsl:param name="transformNodeHeightKey" />
		<xsl:param name="transformNodeLabelKey" />
		<xsl:param name="transformNodeGraphicsKey" />
		<xsl:param name="transformEdgeGraphicsKey" />
		
	    <xsl:variable name="nodeID" select="@id" />
	    <node id="{$nodeID}">
			<xsl:variable name="xValue"><xsl:value-of select=".//y:Geometry/@x"/></xsl:variable>
			<xsl:variable name="yValue"><xsl:value-of select=".//y:Geometry/@y"/></xsl:variable>
			<xsl:variable name="widthValue"><xsl:value-of select="round((.//y:Geometry/@width) * 1000) div 1000"/></xsl:variable>
			<xsl:variable name="heightValue"><xsl:value-of select="round((.//y:Geometry/@height) * 1000) div 1000"/></xsl:variable>
			<xsl:variable name="label"><xsl:value-of select=".//y:NodeLabel/text()"/></xsl:variable>
			<xsl:for-each select="gml:data">
				<xsl:choose>
					<xsl:when test="./@key = $transformNodeWidthKey" >
						<data key="{$transformNodeWidthKey}">
						<xsl:value-of select="$widthValue" />
						</data>
					</xsl:when>
					<xsl:when test="./@key = $transformNodeHeightKey" >
						<data key="{$transformNodeHeightKey}">
						<xsl:value-of select="$heightValue" />
						</data>
					</xsl:when>
					<xsl:when test="./@key = $transformNodeLabelKey" >
						<data key="{$transformNodeLabelKey}">
						<xsl:value-of select="$label" />
						</data>
					</xsl:when>
					<xsl:when test="./@key = $transformNodeXKey" >
						<data key="{$transformNodeXKey}">
						<xsl:value-of select="round(($xValue - $parentX) * 1000) div 1000" />
						</data>
					</xsl:when>
					<xsl:when test="./@key = $transformNodeYKey" >
						<data key="{$transformNodeYKey}">
						<xsl:value-of select="round(($yValue - $parentY) * 1000) div 1000" />
						</data>
					</xsl:when>
					<!-- nodes created in yEd may not have the x y width and height data keys, so make sure we create them -->
					<xsl:when test="./@key = $transformNodeGraphicsKey" >
						<xsl:if test="not(../gml:data[@key=$transformNodeXKey]) or (../gml:data[@key=$transformNodeXKey] = '' ) " >
							<data key="{$transformNodeXKey}">
							<xsl:value-of select="round(($xValue - $parentX) * 1000) div 1000" />
							</data>
						</xsl:if>
						<xsl:if test="not(../gml:data[@key=$transformNodeYKey]) or (../gml:data[@key=$transformNodeYKey] = '' ) " >
							<data key="{$transformNodeYKey}">
							<xsl:value-of select="round(($yValue - $parentY) * 1000) div 1000" />
							</data>
						</xsl:if>
						<xsl:if test="not(../gml:data[@key=$transformNodeWidthKey]) or (../gml:data[@key=$transformNodeWidthKey] = '' ) " >
							<data key="{$transformNodeWidthKey}">
							<xsl:value-of select="$widthValue" />
							</data>
						</xsl:if>
<!-- causing two height tags in output file ?
						<xsl:if test="not(../gml:data[@key=transformNodeHeightKey]) or (../gml:data[@key=$transformNodeHeightKey] = '' ) " >
							<data key="{$transformNodeHeightKey}">
							<xsl:value-of select="$heightValue" />
							</data>
						</xsl:if>
-->
						</xsl:when>
					<xsl:otherwise>
						<xsl:apply-templates select="." mode="copy" >
							<xsl:with-param name="transformNodeGraphicsKey" select="$transformNodeGraphicsKey" />
							<xsl:with-param name="transformEdgeGraphicsKey" select="$transformEdgeGraphicsKey" />
						</xsl:apply-templates>
					</xsl:otherwise>
				</xsl:choose>
			</xsl:for-each>
			
			<xsl:apply-templates select="gml:graph">
				<xsl:with-param name="graphId" select="concat($nodeID,':')" />
				<xsl:with-param name="graphLabel" select="$label" />
				<xsl:with-param name="graphX" select="$xValue" />
				<xsl:with-param name="graphY" select="$yValue" />
				<xsl:with-param name="transformNodeXKey" select="$transformNodeXKey" />
				<xsl:with-param name="transformNodeYKey" select="$transformNodeYKey" />
				<xsl:with-param name="transformNodeWidthKey" select="$transformNodeWidthKey" />
				<xsl:with-param name="transformNodeHeightKey" select="$transformNodeHeightKey" />
				<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey" />
				<xsl:with-param name="transformNodeGraphicsKey" select="$transformNodeGraphicsKey" />
				<xsl:with-param name="transformEdgeGraphicsKey" select="$transformEdgeGraphicsKey" />
			</xsl:apply-templates>
        </node>
    </xsl:template>
	
	<!-- copy all existing node data -->
    <xsl:template match="gml:data" mode="copy">
		<xsl:param name="transformNodeGraphicsKey" />
		<xsl:param name="transformEdgeGraphicsKey" />

		<xsl:if test="not(contains(./@key,$transformNodeGraphicsKey)) and not(contains(./@key,$transformEdgeGraphicsKey)) and (./text() != '')" >
			<xsl:element name="{local-name(.)}" >
				<xsl:apply-templates select="@*|node()" mode="copy" />
			</xsl:element>
		</xsl:if>
    </xsl:template>
	
   	<!-- copy edges in graph -->
   	<xsl:template match="gml:edge" mode="copy" >
		<xsl:param name="transformNodeGraphicsKey" />
		<xsl:param name="transformEdgeGraphicsKey" />
		
		<xsl:element name="{local-name(.)}" >
			<xsl:apply-templates select="@*|node()" mode="copy" />
			<xsl:apply-templates select="./gml:data" mode="copy" >
				<xsl:with-param name="transformNodeGraphicsKey" select="$transformNodeGraphicsKey" />
				<xsl:with-param name="transformEdgeGraphicsKey" select="$transformEdgeGraphicsKey" />
			</xsl:apply-templates>
		</xsl:element>
	</xsl:template> 
	
	<xsl:template match="@*|text()|comment()" mode="copy">
		<xsl:copy/>
	</xsl:template>

</xsl:stylesheet>