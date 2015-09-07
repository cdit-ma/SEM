<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="2.0" 
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

        This template will convert a .graphml project to a deploy.graphml 
		that has explicit edges from each ComponentInstance directly to the HardwareNode
		This allows the user to select deployment to a HardwareCluster,
		then visualise the result explicit deployment generated.
		This will also connect ManagementComponents if the modeler has not deployed them
    -->
	
	<!-- Runtime parameters -->
	<xsl:param name="Algorithm">1</xsl:param>
	
	<!-- Assign data keys standard values, defaults back to original document ids for required transforms -->
	<xsl:include href="graphmlKeyVariables.xsl" />
	
	<!-- apply all templates starting from the root -->
    <xsl:template match="/">
	    <!-- Output start of graph -->
		<graphml xmlns="http://graphml.graphdrawing.org/xmlns" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:schemaLocation="http://graphml.graphdrawing.org/xmlns" >
														<!-- xsi:schemaLocation="http://graphml.graphdrawing.org/xmlns/1.0/graphml.xsd" ? -->
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
	
		<xsl:variable name="transformNodeKindKey">
			<xsl:call-template name="findNodeKey">
				<xsl:with-param name="forType" select="'node'" />
				<xsl:with-param name="attrName" select="'kind'" />
				<xsl:with-param name="defaultId" select="$nodeKindKey" />
			</xsl:call-template>	
		</xsl:variable>	
		
		<xsl:variable name="transformNodeLabelKey">
			<xsl:call-template name="findNodeKey">
				<xsl:with-param name="forType" select="'node'" />
				<xsl:with-param name="attrName" select="'label'" />
				<xsl:with-param name="defaultId" select="$nodeLabelKey" />
			</xsl:call-template>	
		</xsl:variable>
		
		<xsl:variable name="transformEdgeDescriptionKey">
			<xsl:call-template name="findNodeKey">
				<xsl:with-param name="forType" select="'edge'" />
				<xsl:with-param name="attrName" select="'description'" />
				<xsl:with-param name="defaultId" select="$edgeDescriptionKey" />
			</xsl:call-template>	
		</xsl:variable>

	    <xsl:apply-templates select="gml:graph">
		    <xsl:with-param name="graphId" select="'G'" />
			<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey" />
			<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey" />
			<xsl:with-param name="transformEdgeDescriptionKey" select="$transformEdgeDescriptionKey" />
   	    </xsl:apply-templates>
    </xsl:template>

	<!-- find the key for specific attribute,  -->
	<xsl:template name="findNodeKey">
		<xsl:param name="forType" />
		<xsl:param name="attrName" />
		<xsl:param name="defaultId" />
		
		<xsl:variable name="found" select="./gml:key[@attr.name=$attrName][@for=$forType]" />
		<xsl:choose>
			<xsl:when test="not($found)">
				<xsl:value-of select="$defaultId"/>
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="$found/@id"/>
			</xsl:otherwise>
		</xsl:choose>
    </xsl:template>	

	<!-- select global graph for this file -->
	<xsl:template match="gml:graph">
	    <xsl:param name="graphId" />
		<xsl:param name="transformNodeKindKey" />
		<xsl:param name="transformNodeLabelKey" />
		<xsl:param name="transformEdgeDescriptionKey" />
		
		<graph edgedefault="directed" id="{$graphId}">
		
		<xsl:apply-templates select="gml:node">
			<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey" />
			<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey" />
		</xsl:apply-templates>
		
		<xsl:apply-templates select="gml:edge">
			<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey" />
			<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey" />
		</xsl:apply-templates>         
		
		<!-- after all nodes have been copied, insert extra edges if required for deployment -->
		<!-- redefine edges for both ComponentInstance and ComponentAssembly assigned to a HardwareCluster -->
		<xsl:if test="contains($graphId, 'G')">
			<xsl:call-template name="findAllComponentInstancesForHardwareClusters">
				<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey" />
				<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey" />
				<xsl:with-param name="transformEdgeDescriptionKey" select="$transformEdgeDescriptionKey" />
			</xsl:call-template>	
		</xsl:if>
		<!-- redefine edges for ComponentAssembly assigned to HardwareNode -->
		<xsl:if test="contains($graphId, 'G')">
			<xsl:call-template name="findAllComponentInstancesForHardwareNode">
				<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey" />
				<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey" />
				<xsl:with-param name="transformEdgeDescriptionKey" select="$transformEdgeDescriptionKey" />
			</xsl:call-template>	
		</xsl:if>
		<!-- also default deployment for the ManagementComponents -->
		<xsl:if test="contains($graphId, 'G')">
			<xsl:call-template name="findAllManagementComponentsToBeDeployed">
				<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey" />
				<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey" />
				<xsl:with-param name="transformEdgeDescriptionKey" select="$transformEdgeDescriptionKey" />
			</xsl:call-template>	
		</xsl:if>
		
		</graph>
		
	</xsl:template>
	
   	<!-- select nodes in graph -->
	<xsl:template match="gml:node">
		<xsl:param name="transformNodeKindKey" />
		<xsl:param name="transformNodeLabelKey" />
		
	    <xsl:variable name="nodeID" select="@id" />
	    <node id="{$nodeID}">
			<xsl:for-each select="gml:data">
				<xsl:apply-templates select="." mode="copy" />
			</xsl:for-each>
			
			<xsl:apply-templates select="gml:graph">
				<xsl:with-param name="graphId" select="concat($nodeID,':')" />
				<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey" />
				<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey" />
			</xsl:apply-templates>
        </node>
    </xsl:template>
	
	<!-- select nodes in graph -->
	<xsl:template match="gml:edge">
		<xsl:param name="transformNodeKindKey" />
		<xsl:param name="transformNodeLabelKey" />
		
	    <xsl:variable name="edgeID" select="@id" />
	    <xsl:variable name="edgeTarget" select="@target" />
	    <xsl:variable name="edgeSource" select="@source" />
		<xsl:variable name="hardwareDefinitions" select="/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'HardwareDefinitions']/.." />
		<xsl:variable name="ComponentAssemblys" select="/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'ComponentAssembly']/.." /> 

		<xsl:choose>
			<xsl:when test="$hardwareDefinitions/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'HardwareCluster']/../@id = $edgeTarget">
				<!-- do not copy this edge, will redefine as edges to Hardware Nodes --> 
			</xsl:when>
			<xsl:when test="$ComponentAssemblys/@id = $edgeSource">
				<!-- do not copy this edge, will redefine as edges from ComponentInstance Entities --> 
			</xsl:when>
			<xsl:otherwise>
				<xsl:apply-templates select="." mode="copy" />
			</xsl:otherwise>
		</xsl:choose>
		
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
			
	<!-- search for ComponentInstance associated with HardwareCluster -->
	<xsl:template name="findAllComponentInstancesForHardwareClusters">
		<xsl:param name="transformNodeKindKey" />
		<xsl:param name="transformNodeLabelKey" />
		<xsl:param name="transformEdgeDescriptionKey" />
		
		<xsl:variable name="hardwareDefinitions" select="/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'HardwareDefinitions']/.." />

		<xsl:for-each select="$hardwareDefinitions/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'HardwareCluster']/..">
			<xsl:variable name="hardwareCluster" select="." />

			<!-- Create list of all ComponentInstance nodes associated to this hardwareCluster  -->
			<xsl:variable name="ComponentInstanceList" > 
				<xsl:for-each select="/descendant::*/gml:edge[@target = $hardwareCluster/@id]" >
					<xsl:variable name="deployToEdge" select="." />
					<xsl:variable name="sourceId" select="./@source" />
					<xsl:variable name="sourceNode" select="/descendant::*/gml:node[@id = $sourceId]" />

					<xsl:choose>
					<xsl:when test="$sourceNode/gml:data[@key=$transformNodeKindKey]/text() = 'ComponentInstance'">
						<xsl:value-of select="concat($sourceNode/@id, ',')" />
					</xsl:when>
					<xsl:when test="$sourceNode/gml:data[@key=$transformNodeKindKey]/text() = 'BlackBoxInstance'">
						<xsl:value-of select="concat($sourceNode/@id, ',')" />
					</xsl:when>
					<xsl:when test="$sourceNode/gml:data[@key=$transformNodeKindKey]/text() = 'ComponentAssembly'">
						<xsl:call-template name="MapEdgeComponentInstances" >
							<xsl:with-param name="sourceNode" select="$sourceNode" />
							<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey" />
						</xsl:call-template>	
					</xsl:when>
					<xsl:otherwise>  
					</xsl:otherwise>
					</xsl:choose>
				
				</xsl:for-each>
			</xsl:variable> 
			<xsl:variable name="componentCount" select="string-length($ComponentInstanceList) - string-length(translate($ComponentInstanceList, ',', ''))" />
			<xsl:variable name="componentsPerNode" select="ceiling($componentCount div count($hardwareCluster/gml:graph/gml:node)) " />

			<!-- For each HardwareNode contained in this HardwareCluster, create links from ComponentInstance to HardwareNode -->
			<xsl:for-each select="$hardwareCluster/gml:graph/gml:node" >
				<xsl:variable name="hardwareNode" select="." />

				<!-- hardware position starts at 1, component list Idx starts at 0 -->
				<xsl:variable name="startIdx" select="((position() - 1) * $componentsPerNode)" />
				<xsl:variable name="endIdx" select="(position() * $componentsPerNode) - 1" />

				<xsl:if test="($startIdx + 1) &lt;= $componentCount" >

					<xsl:call-template name="ComponentInstanceOnNode" >
						<xsl:with-param name="ComponentInstanceList" select="$ComponentInstanceList" />
						<xsl:with-param name="hardwareNodeId" select="$hardwareNode/@id" />
						<xsl:with-param name="hardwareNodePosition" select="position()" />
						<xsl:with-param name="startIdx" select="$startIdx" />
						<xsl:with-param name="endIdx" select="$endIdx" />
						<xsl:with-param name="transformEdgeDescriptionKey" select="$transformEdgeDescriptionKey" />
					</xsl:call-template>	
					
				</xsl:if>
			</xsl:for-each>
			
		</xsl:for-each>
	</xsl:template> 

		<!-- search for ComponentInstance associated with HardwareNode -->
	<xsl:template name="findAllComponentInstancesForHardwareNode">
		<xsl:param name="transformNodeKindKey" />
		<xsl:param name="transformNodeLabelKey" />
		<xsl:param name="transformEdgeDescriptionKey" />
		
		<xsl:variable name="hardwareDefinitions" select="/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'HardwareDefinitions']/.." />

		<xsl:for-each select="$hardwareDefinitions/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'HardwareNode']/..">
			<xsl:variable name="hardwareNode" select="." />

			<!-- Create list of all ComponentInstance nodes associated to this hardwareNode  -->
			<xsl:variable name="ComponentInstanceList" > 
				<xsl:for-each select="/descendant::*/gml:edge[@target = $hardwareNode/@id]" >
					<xsl:variable name="deployToEdge" select="." />
					<xsl:variable name="sourceId" select="./@source" />
					<xsl:variable name="sourceNode" select="/descendant::*/gml:node[@id = $sourceId]" />

					<xsl:choose>
					<xsl:when test="$sourceNode/gml:data[@key=$transformNodeKindKey]/text() = 'ComponentAssembly'">
						<xsl:call-template name="MapEdgeComponentInstances" >
							<xsl:with-param name="sourceNode" select="$sourceNode" />
							<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey" />
						</xsl:call-template>	
					</xsl:when>
					<xsl:otherwise>  
					</xsl:otherwise>
					</xsl:choose>
				
				</xsl:for-each>
			</xsl:variable> 
			<xsl:variable name="componentCount" select="string-length($ComponentInstanceList) - string-length(translate($ComponentInstanceList, ',', ''))" />

			<!-- Create edge from ComponentInstance to HardwareNode -->
			<xsl:call-template name="ComponentInstanceOnNode" >
				<xsl:with-param name="ComponentInstanceList" select="$ComponentInstanceList" />
				<xsl:with-param name="hardwareNodeId" select="$hardwareNode/@id" />
				<xsl:with-param name="hardwareNodePosition" select="1" />
				<xsl:with-param name="startIdx" select="0" />
				<xsl:with-param name="endIdx" select="$componentCount" />
				<xsl:with-param name="transformEdgeDescriptionKey" select="$transformEdgeDescriptionKey" />
			</xsl:call-template>	
			
		</xsl:for-each>
	</xsl:template> 

	<!-- Distribute ComponentInstances to Hardware Node -->
    <xsl:template name="ComponentInstanceOnNode">
		<xsl:param name="ComponentInstanceList" />
		<xsl:param name="hardwareNodeId" />
		<xsl:param name="hardwareNodePosition" />
		<xsl:param name="startIdx" />
		<xsl:param name="endIdx" />
		<xsl:param name="transformEdgeDescriptionKey" />

		<xsl:variable name="componentCount" select="string-length($ComponentInstanceList) - string-length(translate($ComponentInstanceList, ',', ''))" />
		
		<xsl:if test="$startIdx &lt;= $endIdx">
			<xsl:variable name="componentInstance" >
				<xsl:call-template name="splitComponentInstance" >
					<xsl:with-param name="pText" select="$ComponentInstanceList" />
					<xsl:with-param name="componentCount" select="$componentCount" />
					<xsl:with-param name="idx" select="$startIdx" />
				</xsl:call-template>	
			</xsl:variable>
			
			<xsl:if test="$componentInstance != ''" >
				<edge id="{$componentInstance}::e0" source="{$componentInstance}" target="{$hardwareNodeId}" >
					<data key="{$transformEdgeDescriptionKey}">deploy to</data>
				</edge>
			</xsl:if>
			
			<!-- Continue to put components on this hardware node, ie from startIdx to endIdx -->
			<xsl:call-template name="ComponentInstanceOnNode" >
				<xsl:with-param name="ComponentInstanceList" select="$ComponentInstanceList" />
				<xsl:with-param name="hardwareNodeId" select="$hardwareNodeId" />
				<xsl:with-param name="hardwareNodePosition" select="$hardwareNodePosition" />
				<xsl:with-param name="startIdx" select="$startIdx + 1" />
				<xsl:with-param name="endIdx" select="$endIdx" />
				<xsl:with-param name="transformEdgeDescriptionKey" select="$transformEdgeDescriptionKey" />
			</xsl:call-template>	
		</xsl:if>
		
	</xsl:template> 

	<!-- Drill down into ComponentAssembly nodes to find all ComponentInstances-->
    <xsl:template name="MapEdgeComponentInstances">
		<xsl:param name="sourceNode" />
		<xsl:param name="transformNodeKindKey" />
			
		<!-- exclude ComponentInstance nodes that have already been deployed to a HardwareNode -->
		<xsl:choose>
			<xsl:when test="$sourceNode/gml:data[@key=$transformNodeKindKey]/text() = 'ComponentInstance'
						 and /descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'HardwareNode']/../@id   
							 = /descendant::*/gml:edge[@source = $sourceNode/@id]/@target" />
			<xsl:when test="$sourceNode/gml:data[@key=$transformNodeKindKey]/text() = 'BlackBoxInstance'
						 and /descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'HardwareNode']/../@id   
							 = /descendant::*/gml:edge[@source = $sourceNode/@id]/@target" />
			<xsl:otherwise>
				<xsl:choose>
				<xsl:when test="$sourceNode/gml:data[@key=$transformNodeKindKey]/text() = 'ComponentInstance'" >
					<xsl:value-of select="concat(./@id, ',')" />
				</xsl:when>
				<xsl:when test="$sourceNode/gml:data[@key=$transformNodeKindKey]/text() = 'BlackBoxInstance'" >
					<xsl:value-of select="concat(./@id, ',')" />
				</xsl:when>
				<xsl:when test="$sourceNode/gml:data[@key=$transformNodeKindKey]/text() = 'ComponentAssembly'" >
					<xsl:for-each select="$sourceNode/gml:graph/gml:node" >
						<xsl:call-template name="MapEdgeComponentInstances" >
							<xsl:with-param name="sourceNode" select="." />
							<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey" />
						</xsl:call-template>	
					</xsl:for-each>
				</xsl:when>
				</xsl:choose>
			</xsl:otherwise>
		</xsl:choose>
		
	</xsl:template>
	
	<!-- Split delimited list and return required item -->
	 <xsl:template name="splitComponentInstance">
		<xsl:param name="pText" />
		<xsl:param name="componentCount" />
		<xsl:param name="idx" />

		<xsl:if test="string-length($pText) > 0">
			<xsl:variable name="pCount" select="string-length($pText) - string-length(translate($pText, ',', ''))" />

			<xsl:if test="($componentCount - $pCount) = $idx" >
				<xsl:value-of select="substring-before(concat($pText, ','), ',')"/>
			</xsl:if>
			
			<xsl:call-template name="splitComponentInstance">
				<xsl:with-param name="pText" select="substring-after($pText, ',')"/>
				<xsl:with-param name="componentCount" select="$componentCount" />
				<xsl:with-param name="idx" select="$idx" />
			</xsl:call-template>
		</xsl:if>
	</xsl:template>

				
	<!-- search for ManagementComponent -->
	<xsl:template name="findAllManagementComponentsToBeDeployed">
		<xsl:param name="transformNodeKindKey" />
		<xsl:param name="transformNodeLabelKey" />
		<xsl:param name="transformEdgeDescriptionKey" />

		<xsl:variable name="ManagementComponentList" select="/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'ManagementComponent']/.." /> 
		<xsl:variable name="ComponentList" select="/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'ComponentInstance']/..
												 | /descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'ComponentAssembly']/.." /> 

		<!-- Find first hardware node in first cluster in HardwareDefinitions -->
		<xsl:variable name="hardwareDefinitions" select="/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'HardwareDefinitions']/.." />
		<xsl:variable name="allHardwareNodes" select="$hardwareDefinitions/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'HardwareNode']/.." />
		<xsl:variable name="nodeLinksToMC" select="$allHardwareNodes[@id = /descendant::*/gml:edge[@source = $ManagementComponentList/@id]/@target]" />
		<xsl:variable name="nodeLinksToCCM" select="$allHardwareNodes[@id = /descendant::*/gml:edge[@source = $ComponentList/@id]/@target]" />
		<xsl:variable name="allHardwareClusters" select="$hardwareDefinitions/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'HardwareCluster']/.." />
		<xsl:variable name="clusterLinksToMC" select="$allHardwareClusters[@id = /descendant::*/gml:edge[@source = $ManagementComponentList/@id]/@target]" />
		<xsl:variable name="clusterLinksToCCM" select="$allHardwareClusters[@id = /descendant::*/gml:edge[@source = $ComponentList/@id]/@target]" />

		<!-- Find best choice in cluster for this deployment -->
		<xsl:variable name="hardwareClusterId">
			<xsl:choose>
			<xsl:when test="count($clusterLinksToMC) &gt; 0">
				<xsl:value-of select="($clusterLinksToMC)[1]/@id" />
			</xsl:when>
			<xsl:when test="count($clusterLinksToCCM) &gt; 0">
				<xsl:value-of select="($clusterLinksToCCM)[1]/@id" />
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="($hardwareDefinitions/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'HardwareCluster']/..)[1]/@id" />
			</xsl:otherwise>
			</xsl:choose>
		</xsl:variable>
		
		<!-- first choice for ManagementComponent deployment to hardware node  -->
		<xsl:variable name="firstHardwareNodeId">
			<xsl:choose>
			<xsl:when test="count($nodeLinksToMC) &gt; 0">
				<xsl:value-of select="($nodeLinksToMC)[1]/@id" />
			</xsl:when>
			<xsl:when test="count($nodeLinksToCCM) &gt; 0">
				<xsl:choose>
				<xsl:when test="$nodeLinksToCCM[1]/gml:data[@key=$transformNodeKindKey]/text() = 'ComponentAssembly'">
					<xsl:value-of select="($nodeLinksToCCM[1]/gml:graph/gml:node)[1]/@id" />
				</xsl:when>
				<xsl:otherwise>
					<xsl:value-of select="($nodeLinksToCCM[1]/../../gml:graph/gml:node)[1]/@id" />
				</xsl:otherwise>
				</xsl:choose>
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="($allHardwareClusters[@id=$hardwareClusterId]/gml:graph/gml:node)[1]/@id" />
			</xsl:otherwise>
			</xsl:choose>
		</xsl:variable>
		
		<!-- Create list of all ManagementComponent nodes check for no edge to HardwareNode -->
		<xsl:variable name="UndeployedManagementComponentList" select="$ManagementComponentList[@id != /descendant::*/gml:edge/@source]" /> 

		<xsl:for-each select="$ManagementComponentList">
			<xsl:variable name="MC" select="." />
			<xsl:choose>
				<xsl:when test="$allHardwareNodes[@id = /descendant::*/gml:edge[@source = $MC/@id]/@target]">
				</xsl:when>
				<xsl:otherwise>
					<edge id="{$MC/@id}::e0" source="{$MC/@id}" target="{$firstHardwareNodeId}" >
						<data key="{$transformEdgeDescriptionKey}">deploy to</data>
					</edge>	
				</xsl:otherwise>
			</xsl:choose>
		</xsl:for-each>
		
	</xsl:template> 

	
	<xsl:template match="@*|text()|comment()" mode="copy">
		<xsl:copy/>
	</xsl:template>

</xsl:stylesheet>