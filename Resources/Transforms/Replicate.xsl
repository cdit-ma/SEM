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

		<xsl:variable name="transformNodeSortOrderKey">
			<xsl:call-template name="findNodeKey">
				<xsl:with-param name="attrName" select="'sortOrder'" />
				<xsl:with-param name="defaultId" select="$nodeSortOrderKey" />
			</xsl:call-template>	
		</xsl:variable>

		<xsl:variable name="assemblyDefs" select="/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'AssemblyDefinitions']/.." />
		
	    <xsl:apply-templates select="gml:graph">
		    <xsl:with-param name="graphID" select="'G'" />
		    <xsl:with-param name="replicateID" select="''" />
			<xsl:with-param name="assemblyDefs" select="$assemblyDefs"/>
			<xsl:with-param name="transformNodeReplicateCountKey" select="$transformNodeReplicateCountKey" />
			<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey" />
			<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey" />
			<xsl:with-param name="transformNodeSortOrderKey" select="$transformNodeSortOrderKey" />
			<xsl:with-param name="transformNodeXKey" select="$transformNodeXKey" />
			<xsl:with-param name="transformNodeYKey" select="$transformNodeYKey" />
		</xsl:apply-templates>
    </xsl:template>

	<!-- select global graph for this file -->
	<xsl:template match="gml:graph">
	    <xsl:param name="graphID" />
	    <xsl:param name="replicateID" />
	    <xsl:param name="assemblyDefs" />
		<xsl:param name="transformNodeReplicateCountKey" />
		<xsl:param name="transformNodeKindKey" />
		<xsl:param name="transformNodeLabelKey" />
		<xsl:param name="transformNodeSortOrderKey" />
		<xsl:param name="transformNodeXKey" />
		<xsl:param name="transformNodeYKey" />
		
		<graph edgedefault="directed" id="{$graphID}">
		
		<xsl:apply-templates select="gml:node">
			<xsl:with-param name="replicateID" select="$replicateID" />
			<xsl:with-param name="assemblyDefs" select="$assemblyDefs"/>
			<xsl:with-param name="transformNodeReplicateCountKey" select="$transformNodeReplicateCountKey" />
			<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey" />
			<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey" />
			<xsl:with-param name="transformNodeSortOrderKey" select="$transformNodeSortOrderKey" />
			<xsl:with-param name="transformNodeXKey" select="$transformNodeXKey" />
			<xsl:with-param name="transformNodeYKey" select="$transformNodeYKey" />
		</xsl:apply-templates>
		
		<xsl:apply-templates select="gml:edge" mode="copy" />
		
		</graph>
		
	</xsl:template>
	
   	<!-- select nodes in graph -->
	<xsl:template match="gml:node">
		<xsl:param name="replicateID" />
		<xsl:param name="assemblyDefs" />
		<xsl:param name="transformNodeReplicateCountKey" />
		<xsl:param name="transformNodeKindKey" />
		<xsl:param name="transformNodeLabelKey" />
		<xsl:param name="transformNodeSortOrderKey" />
		<xsl:param name="transformNodeXKey" />
		<xsl:param name="transformNodeYKey" />
		
		<xsl:variable name="node" select="." />		
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

		<!-- This recursive template is used for both original and replicated nodes to traverse assembly hierachy -->
		<!-- Need different behaviour if replicated to avoid duplicating node id values -->
		<xsl:variable name="thisReplicateID">
			<xsl:choose>
			<xsl:when test="$replicateCount &gt; 0 and not($replicateID = '')">
				<xsl:value-of select="concat($replicateID, '_R1')" />
			</xsl:when>
			<xsl:when test="not($replicateID = '')">
				<xsl:value-of select="$replicateID" />
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="''" />
			</xsl:otherwise>
			</xsl:choose>
		</xsl:variable>
		
		<!-- output the original ComponentAssembly, with some changes to data values -->
	    <xsl:variable name="nodeID" select="concat($node/@id, $thisReplicateID)" />
		
		<node id="{$nodeID}"> 
			<xsl:choose>
				<xsl:when test="$replicateCount &gt; 0">
					<!-- modify replicate_count when replicates are produced, copy all other data -->
					<xsl:for-each select="$node/gml:data">
						<xsl:choose>
							<xsl:when test="./@key = $transformNodeReplicateCountKey" >
								<!-- overwriten as below -->
							</xsl:when>
							<xsl:when test="not($thisReplicateID = '') and ./@key = $transformNodeLabelKey" >
								<data key="{$transformNodeLabelKey}">
									<xsl:value-of select="concat(./text(), $thisReplicateID)" />
								</data>
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
				<xsl:with-param name="graphID" select="concat($nodeID,':')" />
				<xsl:with-param name="replicateID" select="$thisReplicateID" />
				<xsl:with-param name="assemblyDefs" select="$assemblyDefs"/>
				<xsl:with-param name="transformNodeReplicateCountKey" select="$transformNodeReplicateCountKey" />
				<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey" />
				<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey" />
				<xsl:with-param name="transformNodeSortOrderKey" select="$transformNodeSortOrderKey" />
				<xsl:with-param name="transformNodeXKey" select="$transformNodeXKey" />
				<xsl:with-param name="transformNodeYKey" select="$transformNodeYKey" />
			</xsl:apply-templates>
			
			<!-- replicate edges for this and all descendant nodes -->
			<xsl:if test="$replicateCount &gt; 0 and not($thisReplicateID = '')" >
				<xsl:variable name="replicaDescendants" select="$node | $node/descendant::node()/gml:node" />
				<xsl:for-each select="$replicaDescendants">
					<xsl:call-template name="replicaEdges">
						<xsl:with-param name="node" select="." />
						<xsl:with-param name="origNode" select="$node" />
						<xsl:with-param name="replicateID" select="$thisReplicateID" />
						<xsl:with-param name="transformNodeReplicateCountKey" select="$transformNodeReplicateCountKey"/>
						<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey" />
					</xsl:call-template> 
				</xsl:for-each> 
			</xsl:if>
		</node>
		
		<!-- found a ComponentAssembly to replicate so add R2, R3 etc -->
		<xsl:if test="$replicateCount &gt; 0" >

			<!-- find sibling count, add replicates at end of list of siblings -->
			<xsl:variable name="siblingCount" select="count($node/../gml:node)" />

			<!-- recurse until replicateCount -->
			<xsl:call-template name="replicate">
				<xsl:with-param name="replicateID" select="$replicateID" />
				<xsl:with-param name="assemblyDefs" select="$assemblyDefs"/>
				<xsl:with-param name="node" select="$node"/>
				<xsl:with-param name="count" select="'1'" />
				<xsl:with-param name="replicateCount" select="$replicateCount" />
				<xsl:with-param name="startSortOrder" select="$siblingCount" />
				<xsl:with-param name="transformNodeReplicateCountKey" select="$transformNodeReplicateCountKey"/>
				<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey" />
				<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey" />
				<xsl:with-param name="transformNodeSortOrderKey" select="$transformNodeSortOrderKey" />
				<xsl:with-param name="transformNodeXKey" select="$transformNodeXKey" />
				<xsl:with-param name="transformNodeYKey" select="$transformNodeYKey" />
			</xsl:call-template>
		</xsl:if>
		
    </xsl:template>
	
	<!-- Recurse until replicate count is exhausted -->
	<xsl:template name="replicate">
		<xsl:param name="replicateID" />
		<xsl:param name="assemblyDefs" />
		<xsl:param name="node" />
		<xsl:param name="count" />
		<xsl:param name="replicateCount" />
		<xsl:param name="startSortOrder" />
		<xsl:param name="transformNodeReplicateCountKey" />
		<xsl:param name="transformNodeKindKey" />
		<xsl:param name="transformNodeLabelKey" />
		<xsl:param name="transformNodeSortOrderKey" />
		<xsl:param name="transformNodeXKey" />
		<xsl:param name="transformNodeYKey" />
		
		<!-- create nodeID with this replicate postpend -->
		<xsl:variable name="thisReplicateID" select="concat($replicateID, '_R', ($count + 1))" />
		<xsl:variable name="nodeID" select="concat($node/@id, $thisReplicateID)" />
		<xsl:if test="$count &lt; $replicateCount" >
			<node id="{$nodeID}"> 
				<xsl:for-each select="$node/gml:data">
					<xsl:choose>
						<xsl:when test="./@key = $transformNodeReplicateCountKey" >
							<!-- overwriten as below -->
						</xsl:when>
						<xsl:when test="./@key = $transformNodeLabelKey" >
							<data key="{$transformNodeLabelKey}">
								<xsl:value-of select="concat(./text(), $thisReplicateID)" />
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
						<xsl:when test="./@key = $transformNodeSortOrderKey" >
							<data key="{$transformNodeSortOrderKey}">
								<xsl:value-of select="$startSortOrder + $count - 1" />
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
				
				<!-- replicate all child nodes -->
				<xsl:for-each select="$node/gml:graph">
				<xsl:apply-templates select=".">
					<xsl:with-param name="graphID" select="concat($nodeID,':')" />
					<xsl:with-param name="replicateID" select="$thisReplicateID" />
					<xsl:with-param name="transformNodeReplicateCountKey" select="$transformNodeReplicateCountKey" />
					<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey" />
					<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey" />
					<xsl:with-param name="transformNodeSortOrderKey" select="$transformNodeSortOrderKey" />
					<xsl:with-param name="transformNodeXKey" select="$transformNodeXKey" />
					<xsl:with-param name="transformNodeYKey" select="$transformNodeYKey" />
				</xsl:apply-templates>
				</xsl:for-each>
			</node>

			<!-- replicate edges for this and all descendant nodes -->
			<xsl:variable name="replicaDescendants" select="$node | $node/descendant::node()/gml:node" />
			<xsl:for-each select="$replicaDescendants">
				<xsl:variable name="rep" select="." />
				<xsl:variable name="assm" select="$rep/ancestor-or-self::node()[gml:data[@key=$transformNodeKindKey][text() = 'ComponentAssembly']][1]" />
				<!-- exclude nodes that are part of another replicate assembly, ie the nearest ancestor assembly is not the current replicate assembly -->
				<xsl:if test="$assm/@id = $node/@id">
					<xsl:call-template name="replicaEdges">
						<xsl:with-param name="node" select="." />
						<xsl:with-param name="origNode" select="$node" />
						<xsl:with-param name="replicateID" select="$thisReplicateID" />
						<xsl:with-param name="transformNodeReplicateCountKey" select="$transformNodeReplicateCountKey"/>
						<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey" />
					</xsl:call-template> 
				</xsl:if> 
			</xsl:for-each>
			
			<!-- Recurse for the number of replica's required -->			
			<xsl:call-template name="replicate">
				<xsl:with-param name="replicateID" select="$replicateID" />
				<xsl:with-param name="assemblyDefs" select="$assemblyDefs"/>
				<xsl:with-param name="node" select="$node"/>
				<xsl:with-param name="count" select="$count + 1" />
				<xsl:with-param name="replicateCount" select="$replicateCount" />
				<xsl:with-param name="startSortOrder" select="$startSortOrder" />
				<xsl:with-param name="transformNodeReplicateCountKey" select="$transformNodeReplicateCountKey"/>
				<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey" />
				<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey" />
				<xsl:with-param name="transformNodeSortOrderKey" select="$transformNodeSortOrderKey" />
				<xsl:with-param name="transformNodeXKey" select="$transformNodeXKey" />
				<xsl:with-param name="transformNodeYKey" select="$transformNodeYKey" />
			</xsl:call-template> 
		</xsl:if>
	</xsl:template>

	<!-- Replicate edges for a replicated node -->
	<xsl:template name="replicaEdges">
		<xsl:param name="node" />
		<xsl:param name="origNode" />
		<xsl:param name="replicateID" />
		<xsl:param name="transformNodeReplicateCountKey" />
		<xsl:param name="transformNodeKindKey" />
		
		<!-- replicate edges to given node -->
		<xsl:variable name="nodeID" select="concat($node/@id, $replicateID)" />
		<xsl:variable name="nodeSourceEdges" select="/descendant::*/gml:edge[@source=$node/@id]" />
		<xsl:for-each select="$nodeSourceEdges">
			<xsl:variable name="edge" select="." />
			<xsl:variable name="edgeID" select="concat($edge/@id, $replicateID)" />
			<xsl:variable name="targetNode" select="/descendant::*/gml:node[@id=$edge/@target]" />
			<xsl:variable name="targetReplicate" select="$targetNode/ancestor-or-self::node()/gml:data[@key=$transformNodeKindKey][text() = 'ComponentAssembly'][1]/.." />
			<xsl:variable name="targetReplicateCount">
				<xsl:choose>
					<xsl:when test="number($targetReplicate/gml:data[@key=$transformNodeReplicateCountKey]/text()) &gt; 1">
						<!-- found a ComponentAssembly ancestor that is replicate -->
						<xsl:value-of select="$targetReplicate/gml:data[@key=$transformNodeReplicateCountKey]/text()" />
					</xsl:when>
					<xsl:otherwise>
						<xsl:value-of select="'0'" />
					</xsl:otherwise>
				</xsl:choose>
			</xsl:variable>
			<xsl:variable name="targetID">
				<xsl:choose> 
				<!-- edge to node within this replicate assembly -->
				<xsl:when test="$targetReplicate/@id = $origNode/@id">  
					<xsl:value-of select="concat($targetNode/@id, $replicateID)" />
				</xsl:when>
				<!-- edge to node one level up in replication -->
				<xsl:when test="$targetReplicateCount &gt; 1">
					<xsl:variable name="parentReplicateID">
						<xsl:call-template name="substring-before-last">
						  <xsl:with-param name="string1" select="$replicateID" />
						  <xsl:with-param name="string2" select="'_'" />
						</xsl:call-template>
					</xsl:variable>
					<xsl:value-of select="concat($targetNode/@id, $parentReplicateID)" />
				</xsl:when>
				<!-- edge to node outside replicate -->
				<xsl:otherwise>
					<xsl:value-of select="$targetNode/@id" />
				</xsl:otherwise>
				</xsl:choose>
			</xsl:variable>
			<edge id="{$edgeID}" source="{$nodeID}" target="{$targetID}">
				<xsl:for-each select="$edge/gml:data">
					<xsl:apply-templates select="." mode="copy" />
				</xsl:for-each>			
			</edge>
		</xsl:for-each>

		<xsl:variable name="nodeTargetEdges" select="/descendant::*/gml:edge[@target=$node/@id]" />
		<xsl:for-each select="$nodeTargetEdges">
			<xsl:variable name="edge" select="." />
			<xsl:variable name="edgeID" select="concat($edge/@id, $replicateID)" />
			<xsl:variable name="sourceNode" select="/descendant::*/gml:node[@id=$edge/@source]" />
			<xsl:variable name="sourceReplicate" select="$sourceNode/ancestor-or-self::node()/gml:data[@key=$transformNodeKindKey][text() = 'ComponentAssembly'][1]/.." />
			<xsl:variable name="sourceReplicateCount">
				<xsl:choose>
					<xsl:when test="number($sourceReplicate/gml:data[@key=$transformNodeReplicateCountKey]/text()) &gt; 1">
						<!-- found a ComponentAssembly ancestor that is replicate -->
						<xsl:value-of select="$sourceReplicate/gml:data[@key=$transformNodeReplicateCountKey]/text()" />
					</xsl:when>
					<xsl:otherwise>
						<xsl:value-of select="'0'" />
					</xsl:otherwise>
				</xsl:choose>
			</xsl:variable>

			<xsl:choose> 
			<!-- edge from node of any replicate, edge already created from nodeSourceEdges above -->
			<xsl:when test="$sourceNode/ancestor-or-self::node()[@id=$origNode/@id]
							or $sourceReplicateCount &gt; 1">
			</xsl:when>
			<!-- edge from node outside replicate -->
			<xsl:otherwise>
				<edge id="{$edgeID}" source="{$sourceNode/@id}" target="{$nodeID}">
					<xsl:for-each select="$edge/gml:data">
						<xsl:apply-templates select="." mode="copy" />
					</xsl:for-each>			
				</edge>
			</xsl:otherwise>
			</xsl:choose>
		</xsl:for-each>		
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
		</xsl:element>
	</xsl:template> 
	
	<xsl:template match="@*|text()|comment()" mode="copy">
		<xsl:copy/>
	</xsl:template>	
	
	<!-- find the key for specific attribute -->
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
	
	<!-- return string removing last delimited value -->
	<xsl:template name="substring-before-last">
		<xsl:param name="string1" select="''" />
		<xsl:param name="string2" select="''" />

		<xsl:if test="$string1 != '' and $string2 != ''">
			<xsl:variable name="head" select="substring-before($string1, $string2)" />
			<xsl:variable name="tail" select="substring-after($string1, $string2)" />
			<xsl:value-of select="$head" />
			<xsl:if test="contains($tail, $string2)">
				<xsl:value-of select="$string2" />
				<xsl:call-template name="substring-before-last">
				<xsl:with-param name="string1" select="$tail" />
				<xsl:with-param name="string2" select="$string2" />
				</xsl:call-template>
			</xsl:if>
		</xsl:if>
	</xsl:template>

</xsl:stylesheet>