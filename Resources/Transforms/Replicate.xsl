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

        This template will convert a .graphml project to a ASM.graphml 
		that replicates all assembly children to produce a flat assembly.
		This allows the user to set a replicate_count for any ComponentAssembly
		which simplifies the assembly definition in a model. This preprocess 
		transform should be used before the Deploy.xsl that makes the explicit
		connections from components to the hardware nodes.
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
			
			<!-- need a replicate_count key -->
			<xsl:variable name="replicateCountnodeFound" select="./gml:graphml/gml:key[@attr.name='replicate_count'][@for='node']" />
			<xsl:if test="not($replicateCountnodeFound)">
				<key attr.name="replicate_count" attr.type="string" for="node" id="{$nodeReplicateCountKey}"/>
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
		<!-- Assign the transform node key ids for replicateCount from existing keys -->
		<xsl:variable name="transformNodeReplicateCountKey">
			<xsl:call-template name="findNodeKey">
				<xsl:with-param name="attrName" select="'replicate_count'" />
				<xsl:with-param name="defaultId" select="$nodeReplicateCountKey" />
			</xsl:call-template>	
		</xsl:variable>

		<xsl:variable name="transformNodeKindKey">
			<xsl:call-template name="findNodeKey">
				<xsl:with-param name="attrName" select="'kind'" />
				<xsl:with-param name="defaultId" select="$nodeKindKey" />
			</xsl:call-template>	
		</xsl:variable>	

		<xsl:variable name="transformNodeLabelKey">
			<xsl:call-template name="findNodeKey">
				<xsl:with-param name="attrName" select="'label'" />
				<xsl:with-param name="defaultId" select="$nodeLabelKey" />
			</xsl:call-template>	
		</xsl:variable>

		<xsl:variable name="transformNodeXKey">
			<xsl:call-template name="findNodeKey">
				<xsl:with-param name="attrName" select="'x'" />
				<xsl:with-param name="defaultId" select="$nodeXKey" />
			</xsl:call-template>	
		</xsl:variable>
		
		<xsl:variable name="transformNodeYKey">
			<xsl:call-template name="findNodeKey">
				<xsl:with-param name="attrName" select="'y'" />
				<xsl:with-param name="defaultId" select="$nodeYKey" />
			</xsl:call-template>	
		</xsl:variable>
		
	    <xsl:apply-templates select="gml:graph">
		    <xsl:with-param name="graphId" select="'G'" />
			<xsl:with-param name="transformNodeReplicateCountKey" select="$transformNodeReplicateCountKey" />
			<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey" />
			<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey" />
			<xsl:with-param name="transformNodeXKey" select="$transformNodeXKey" />
			<xsl:with-param name="transformNodeYKey" select="$transformNodeYKey" />
		</xsl:apply-templates>
    </xsl:template>

	<!-- select global graph for this file -->
	<xsl:template match="gml:graph">
	    <xsl:param name="graphId" />
		<xsl:param name="transformNodeReplicateCountKey" />
		<xsl:param name="transformNodeKindKey" />
		<xsl:param name="transformNodeLabelKey" />
		<xsl:param name="transformNodeXKey" />
		<xsl:param name="transformNodeYKey" />
		
		<graph edgedefault="directed" id="{$graphId}">
		
		<xsl:apply-templates select="gml:node">
			<xsl:with-param name="transformNodeReplicateCountKey" select="$transformNodeReplicateCountKey" />
			<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey" />
			<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey" />
			<xsl:with-param name="transformNodeXKey" select="$transformNodeXKey" />
			<xsl:with-param name="transformNodeYKey" select="$transformNodeYKey" />
		</xsl:apply-templates>
		
		<xsl:for-each select="gml:edge">
			<xsl:apply-templates select="." mode="copy" />
		</xsl:for-each>         
		
		</graph>
		
	</xsl:template>
	
   	<!-- select nodes in graph -->
	<xsl:template match="gml:node">
		<xsl:param name="transformNodeReplicateCountKey" />
		<xsl:param name="transformNodeKindKey" />
		<xsl:param name="transformNodeLabelKey" />
		<xsl:param name="transformNodeXKey" />
		<xsl:param name="transformNodeYKey" />
		
		<xsl:variable name="node" select="." />		
	    <xsl:variable name="nodeID" select="$node/@id" />
		<xsl:variable name="replicateCount">
			<xsl:choose>
				<xsl:when test="($node/gml:data[@key=$transformNodeKindKey]/text() = 'ComponentAssembly')">
					<!-- found a ComponentAssembly to replicate -->
					<xsl:value-of select="$node/gml:data[@key=$transformNodeReplicateCountKey]/text()" />
				</xsl:when>
				<xsl:otherwise>
					<xsl:value-of select="'0'" />
				</xsl:otherwise>
			</xsl:choose>
		</xsl:variable>

		<!-- found a ComponentAssembly to replicate -->
		<xsl:if test="$replicateCount &gt; 0" >
			<xsl:variable name="assemblyDefs" select="/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'AssemblyDefinitions']/.." />

			<!-- what to do ??? -->

			<!-- recurse until replicateCount = 1 -->
			<xsl:call-template name="replicate">
				<xsl:with-param name="assemblyDefs" select="$assemblyDefs"/>
				<xsl:with-param name="node" select="$node"/>
				<xsl:with-param name="count" select="$replicateCount" />
				<xsl:with-param name="transformNodeReplicateCountKey" select="$transformNodeReplicateCountKey"/>
				<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey" />
				<xsl:with-param name="transformNodeXKey" select="$transformNodeXKey" />
				<xsl:with-param name="transformNodeYKey" select="$transformNodeYKey" />
			</xsl:call-template>
		</xsl:if>

		<!-- now output the original ComponentAssembly -->
	    <node id="{$nodeID}"> 
			<xsl:choose>
				<xsl:when test="$replicateCount &gt; 0">
					<!-- modify replicate_count when replicates are produced, copy all other data -->
					<xsl:for-each select="$node/gml:data">
						<xsl:choose>
							<xsl:when test="./@key = $transformNodeReplicateCountKey" >
								<!-- overwriten as below -->
							</xsl:when>
							<xsl:otherwise>
								<xsl:apply-templates select="." mode="copy" />
							</xsl:otherwise>
						</xsl:choose>
					</xsl:for-each>
					<!-- if replicate_count is not present in graphml add -->
					<data key="{$transformNodeReplicateCountKey}">
					<xsl:value-of select="'1'" />
					</data>
				</xsl:when>
				
				<xsl:otherwise>
					<xsl:for-each select="$node/gml:data">
						<xsl:apply-templates select="." mode="copy" />
					</xsl:for-each>
				</xsl:otherwise>
			</xsl:choose>
			
			<xsl:apply-templates select="$node/gml:graph">
				<xsl:with-param name="graphId" select="concat($nodeID,':')" />
				<xsl:with-param name="transformNodeReplicateCountKey" select="$transformNodeReplicateCountKey" />
				<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey" />
				<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey" />
				<xsl:with-param name="transformNodeXKey" select="$transformNodeXKey" />
				<xsl:with-param name="transformNodeYKey" select="$transformNodeYKey" />
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

	<!-- Recurse copy until replicate count is exhausted -->
	 <xsl:template name="replicate">
		<xsl:param name="assemblyDefs" />
		<xsl:param name="node" />
		<xsl:param name="count" />
		<xsl:param name="transformNodeReplicateCountKey" />
		<xsl:param name="transformNodeLabelKey" />
		<xsl:param name="transformNodeXKey" />
		<xsl:param name="transformNodeYKey" />
		
		<!-- create nodeID with this replicate postpend -->
		<xsl:variable name="nodeID" select="concat($node/@id, '_R', $count)" />
		<xsl:choose>
			<xsl:when test="$count &gt; 1" >
				<node id="{$nodeID}"> 
					<xsl:for-each select="$node/gml:data">
						<xsl:choose>
							<xsl:when test="./@key = $transformNodeReplicateCountKey" >
								<!-- overwriten as below -->
							</xsl:when>
							<xsl:when test="./@key = $transformNodeLabelKey" >
								<data key="{$transformNodeLabelKey}">
									<xsl:value-of select="concat(./text(), '_R', $count)" />
								</data>
							</xsl:when>
							<xsl:when test="./@key = $transformNodeXKey" >
								<data key="{$transformNodeXKey}">
									<xsl:value-of select="number(./text()) + ($count * 5)" />
								</data>
							</xsl:when>
							<xsl:when test="./@key = $transformNodeYKey" >
								<data key="{$transformNodeYKey}">
									<xsl:value-of select="number(./text()) + ($count * 5)" />
								</data>
							</xsl:when>
							<xsl:otherwise>
								<xsl:apply-templates select="." mode="copy" />
							</xsl:otherwise>
						</xsl:choose>
					</xsl:for-each>			
					<data key="{$transformNodeReplicateCountKey}">
						<xsl:value-of select="'1'" />
					</data>
				</node>
				
				<xsl:call-template name="replicate">
					<xsl:with-param name="assemblyDefs" select="$assemblyDefs"/>
					<xsl:with-param name="node" select="$node"/>
					<xsl:with-param name="count" select="$count - 1" />
					<xsl:with-param name="transformNodeReplicateCountKey" select="$transformNodeReplicateCountKey"/>
					<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey" />
					<xsl:with-param name="transformNodeXKey" select="$transformNodeXKey" />
					<xsl:with-param name="transformNodeYKey" select="$transformNodeYKey" />
				</xsl:call-template> 
			</xsl:when>
			<xsl:otherwise>
			</xsl:otherwise>
		</xsl:choose>

	</xsl:template>
	
	
	<!-- find the key for specific attribute,  -->
	<xsl:template name="findNodeKey">
		<xsl:param name="attrName" />
		<xsl:param name="defaultId" />
		
		<xsl:variable name="found" select="/gml:graphml/gml:key[@attr.name=$attrName][@for='node']" />
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