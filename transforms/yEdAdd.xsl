<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:gml="http://graphml.graphdrawing.org/xmlns"
    xmlns:y="http://www.yworks.com/xml/graphml"
	xmlns="http://graphml.graphdrawing.org/xmlns"
	xmlns:exsl="http://exslt.org/common"
	xmlns:xalan="http://xml.apache.org/xslt"
	exclude-result-prefixes="gml #default" >
	
    <xsl:output method="xml" 
        version="1.0" 
        indent="yes" 
		xalan:indent-amount="4"/>
	<xsl:strip-space elements="*" />
 
    <!--
        Purpose:

        This template will convert the raw.graphml project to a yEd.graphml 
        file. The transform adds yEd specific decorations for use in the editor.
    -->
	
	<!-- Assign data keys as used by yEd 3.12.2 -->
	<xsl:include href="graphmlKeyVariables.xsl" />
	
	<!-- cannot be sure that the standard nodeGraphicsKey is unique, so assign another id -->
	<xsl:variable name="transformNodeGraphicsKey"><xsl:value-of select="concat(generate-id(),'n')" /></xsl:variable>
	<xsl:variable name="transformEdgeGraphicsKey"><xsl:value-of select="concat(generate-id(),'e')" /></xsl:variable>
	
	<!-- Assign Image refids as defined in WEDMLpalette -->
    <xsl:variable name="imageInEventPort" select="document('./IconPalette/WEDMLpalette.graphml')/gml:graphml/descendant::*/y:NodeLabel[text()='InEventPort']/../y:Image/@refid"/>
    <xsl:variable name="imageOutEventPort" select="document('./IconPalette/WEDMLpalette.graphml')/gml:graphml/descendant::*/y:NodeLabel[text()='OutEventPort']/../y:Image/@refid"/>
    <xsl:variable name="imageAttribute" select="document('./IconPalette/WEDMLpalette.graphml')/gml:graphml/descendant::*/y:NodeLabel[text()='Attribute']/../y:Image/@refid"/>
    <xsl:variable name="imageIDL" select="document('./IconPalette/WEDMLpalette.graphml')/gml:graphml/descendant::*/y:NodeLabel[text()='IDL']/../y:Image/@refid"/>
    <xsl:variable name="imageComponent" select="document('./IconPalette/WEDMLpalette.graphml')/gml:graphml/descendant::*/y:NodeLabel[text()='Component']/../y:Image/@refid"/>
    <xsl:variable name="imageVariable" select="document('./IconPalette/WEDMLpalette.graphml')/gml:graphml/descendant::*/y:NodeLabel[text()='Variable']/../y:Image/@refid"/>
    <xsl:variable name="imagePeriodicEvent" select="document('./IconPalette/WEDMLpalette.graphml')/gml:graphml/descendant::*/y:NodeLabel[text()='PeriodicEvent']/../y:Image/@refid"/>
    <xsl:variable name="imageCondition" select="document('./IconPalette/WEDMLpalette.graphml')/gml:graphml/descendant::*/y:NodeLabel[text()='Condition']/../y:Image/@refid"/>
    <xsl:variable name="imageTermination" select="document('./IconPalette/WEDMLpalette.graphml')/gml:graphml/descendant::*/y:NodeLabel[text()='Termination']/../y:Image/@refid"/>
    <xsl:variable name="imageEvent" select="document('./IconPalette/WEDMLpalette.graphml')/gml:graphml/descendant::*/y:NodeLabel[text()='Event']/../y:Image/@refid"/>
    <xsl:variable name="imageAggregate" select="document('./IconPalette/WEDMLpalette.graphml')/gml:graphml/descendant::*/y:NodeLabel[text()='Aggregate']/../y:Image/@refid"/>
    <xsl:variable name="imageArrayMember" select="document('./IconPalette/WEDMLpalette.graphml')/gml:graphml/descendant::*/y:NodeLabel[text()='ArrayMember']/../y:Image/@refid"/>
    <xsl:variable name="imageMember" select="document('./IconPalette/WEDMLpalette.graphml')/gml:graphml/descendant::*/y:NodeLabel[text()='Member']/../y:Image/@refid"/>
    <xsl:variable name="imageWorkload" select="document('./IconPalette/WEDMLpalette.graphml')/gml:graphml/descendant::*/y:NodeLabel[text()='Workload']/../y:Image/@refid"/>
    <xsl:variable name="imageCPU" select="document('./IconPalette/WEDMLpalette.graphml')/gml:graphml/descendant::*/y:NodeLabel[text()='CPU']/../y:Image/@refid"/>
    <xsl:variable name="imageDB" select="document('./IconPalette/WEDMLpalette.graphml')/gml:graphml/descendant::*/y:NodeLabel[text()='DB']/../y:Image/@refid"/>
    <xsl:variable name="imageHDD" select="document('./IconPalette/WEDMLpalette.graphml')/gml:graphml/descendant::*/y:NodeLabel[text()='HDD']/../y:Image/@refid"/>
    <xsl:variable name="imageRAM" select="document('./IconPalette/WEDMLpalette.graphml')/gml:graphml/descendant::*/y:NodeLabel[text()='RAM']/../y:Image/@refid"/>
    <xsl:variable name="imageLOG" select="document('./IconPalette/WEDMLpalette.graphml')/gml:graphml/descendant::*/y:NodeLabel[text()='LOG']/../y:Image/@refid"/>
    <xsl:variable name="imageComplexity" select="document('./IconPalette/WEDMLpalette.graphml')/gml:graphml/descendant::*/y:NodeLabel[text()='Complexity']/../y:Image/@refid"/>
    <xsl:variable name="imageProcess" select="document('./IconPalette/WEDMLpalette.graphml')/gml:graphml/descendant::*/y:NodeLabel[text()='Process']/../y:Image/@refid"/>
    <xsl:variable name="imageAssembly" select="document('./IconPalette/WEDMLpalette.graphml')/gml:graphml/descendant::*/y:NodeLabel[text()='Assembly']/../y:Image/@refid"/>
    <xsl:variable name="imageBranchState" select="document('./IconPalette/WEDMLpalette.graphml')/gml:graphml/descendant::*/y:NodeLabel[text()='BranchState']/../y:Image/@refid"/>
    <xsl:variable name="imageWhileLoop" select="document('./IconPalette/WEDMLpalette.graphml')/gml:graphml/descendant::*/y:NodeLabel[text()='WhileLoop']/../y:Image/@refid"/>
    <xsl:variable name="imageInputParameter" select="document('./IconPalette/WEDMLpalette.graphml')/gml:graphml/descendant::*/y:NodeLabel[text()='InputParameter']/../y:Image/@refid"/>
    <xsl:variable name="imageReturnParameter" select="document('./IconPalette/WEDMLpalette.graphml')/gml:graphml/descendant::*/y:NodeLabel[text()='ReturnParameter']/../y:Image/@refid"/>
    <xsl:variable name="imageVector" select="document('./IconPalette/WEDMLpalette.graphml')/gml:graphml/descendant::*/y:NodeLabel[text()='Vector']/../y:Image/@refid"/>
    <xsl:variable name="imageHardwareNode" select="document('./IconPalette/WEDMLpalette.graphml')/gml:graphml/descendant::*/y:NodeLabel[text()='HardwareNode']/../y:Image/@refid"/>
    <xsl:variable name="imageHardwareCluster" select="document('./IconPalette/WEDMLpalette.graphml')/gml:graphml/descendant::*/y:NodeLabel[text()='HardwareCluster']/../y:Image/@refid"/>
    <xsl:variable name="imageProvidedRequestPort" select="document('./IconPalette/WEDMLpalette.graphml')/gml:graphml/descendant::*/y:NodeLabel[text()='ProvidedRequestPort']/../y:Image/@refid"/>
    <xsl:variable name="imageRequiredRequestPort" select="document('./IconPalette/WEDMLpalette.graphml')/gml:graphml/descendant::*/y:NodeLabel[text()='RequiredRequestPort']/../y:Image/@refid"/>
    <xsl:variable name="imageCallback" select="document('./IconPalette/WEDMLpalette.graphml')/gml:graphml/descendant::*/y:NodeLabel[text()='Callback']/../y:Image/@refid"/>
	
	<!-- apply all templates starting from the root -->
    <xsl:template match="/">
	    <!-- Output start of graph -->
		<graphml xmlns="http://graphml.graphdrawing.org/xmlns" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:y="http://www.yworks.com/xml/graphml" xmlns:yed="http://www.yworks.com/xml/yed/3" xsi:schemaLocation="http://graphml.graphdrawing.org/xmlns http://www.yworks.com/xml/schema/graphml/1.1/ygraphml.xsd">
			<!-- Assign id's for data keys -->
			<!--    convention is to have id="d0..." but yEd may reassign, see Properties Mapper... -->
			<!-- Default yEd data keys -->
			<key for="graphml" id="{$resourcesKey}" yfiles.type="resources"/>
			<key for="port" id="{$portGraphicsKey}" yfiles.type="portgraphics"/>
			<key for="port" id="{$portGeometryKey}" yfiles.type="portgeometry"/>
			<key for="port" id="{$portUserDataKey}" yfiles.type="portuserdata"/>
			
			<!-- Default yEd data keys, keep original if already existing -->
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
			
			<!-- Specific yEd data keys for graphics -->
			<key for="node" id="{$transformNodeGraphicsKey}" yfiles.type="nodegraphics"/>
			<key for="edge" id="{$transformEdgeGraphicsKey}" yfiles.type="edgegraphics"/>
			
			<!-- PICML specific attributes, keep original file key ids -->
			<xsl:for-each select="./gml:graphml/gml:key">
				<xsl:copy>
					<xsl:apply-templates select="@*|node()"/>
				</xsl:copy>
			</xsl:for-each>
			
			<!-- Output all nodes from original graphml, inserting yEd specific formatting -->		
			<xsl:apply-templates />

			<!-- Insert resources -->
			<xsl:variable name="docResourcesKey" select="document('./IconPalette/WEDMLpalette.graphml')/gml:graphml/gml:key[@yfiles.type='resources']/@id"/>
			<xsl:variable name="palette" select="document('./IconPalette/WEDMLpalette.graphml')/gml:graphml/gml:data[@key=$docResourcesKey]" /> 
			<data key="{$resourcesKey}">
			   <y:Resources>
					<!-- Copy resource images from document -->
					<xsl:for-each select="$palette/y:Resources/y:Resource">
						<xsl:copy-of select="." />
					</xsl:for-each>
					<!-- If using NodeLabel icons must have Scaled Icon resource that points to a Image -->
					<!--<xsl:variable name="itemsAreResources" select="$palette/y:Resources/y:Resource" />
						<xsl:variable name="countResources" select="count($itemsAreResources)" /> -->
					<xsl:variable name="countResources" select="100"/>  <!-- Assume no more than 100 Image Resources -->
					<xsl:for-each select="$palette/y:Resources/y:Resource">
						<xsl:variable name="id" select="./@id" />
						<xsl:variable name="incId" select="$id + $countResources" />
						<y:Resource id="{$incId}">
						<yed:ScaledIcon xScale="1.0" yScale="1.0">
						<yed:ImageIcon image="{$id}"/>
						</yed:ScaledIcon>
						</y:Resource>
					</xsl:for-each>
				</y:Resources>
			</data>
		<!-- Output end of graph -->
		</graphml>
    </xsl:template>
 
	<xsl:template match="gml:graphml">
		<!-- Assign the transform node keys for X Y Label and Kind from existing keys -->
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
		
		<xsl:variable name="kindnodeFound" select="./gml:key[@attr.name='kind']" />
		<xsl:variable name="transformNodeKindKey">
			<xsl:choose>
				<xsl:when test="not($kindnodeFound)">
					<xsl:value-of select="$nodeKindKey"/>
				</xsl:when>
				<xsl:otherwise>
					<xsl:value-of select="$kindnodeFound/@id"/>
				</xsl:otherwise>
			</xsl:choose>
		</xsl:variable>	
	
		<xsl:variable name="workernodeFound" select="./gml:key[@attr.name='worker']" />
		<xsl:variable name="transformNodeWorkerKey">
			<xsl:choose>
				<xsl:when test="not($workernodeFound)">
					<xsl:value-of select="$nodeWorkerKey"/>
				</xsl:when>
				<xsl:otherwise>
					<xsl:value-of select="$workernodeFound/@id"/>
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
			<xsl:with-param name="graphWidth" select="41" />
			<xsl:with-param name="graphHeight" select="41" />
			<xsl:with-param name="graphKind" />
			<xsl:with-param name="transformNodeXKey" select="$transformNodeXKey" />
			<xsl:with-param name="transformNodeYKey" select="$transformNodeYKey" />
			<xsl:with-param name="transformNodeWidthKey" select="$transformNodeWidthKey" />
			<xsl:with-param name="transformNodeHeightKey" select="$transformNodeHeightKey" />
			<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey" />
			<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey" />
			<xsl:with-param name="transformNodeWorkerKey" select="$transformNodeWorkerKey" />
   	    </xsl:apply-templates>
    </xsl:template>

	<!-- select global graph for this file -->
	<xsl:template match="gml:graph">
	    <xsl:param name="graphId" />
	    <xsl:param name="graphLabel" />
	    <xsl:param name="graphX" />
	    <xsl:param name="graphY" />
	    <xsl:param name="graphWidth" />
	    <xsl:param name="graphHeight" />
		<xsl:param name="graphKind" />
		<xsl:param name="transformNodeXKey" />
		<xsl:param name="transformNodeYKey" />
		<xsl:param name="transformNodeWidthKey" />
		<xsl:param name="transformNodeHeightKey" />
		<xsl:param name="transformNodeLabelKey" />
		<xsl:param name="transformNodeKindKey" />
		<xsl:param name="transformNodeWorkerKey" />
		
        <xsl:if test="not(contains($graphId,'G'))">  <!-- assumes top level graph id contains G -->
			<data key="{$transformNodeGraphicsKey}">
			<xsl:call-template name="icon">
				<xsl:with-param name="graphLabel" select="$graphLabel" />
				<xsl:with-param name="graphX" select="$graphX" />
				<xsl:with-param name="graphY" select="$graphY" />
				<xsl:with-param name="graphWidth" select="$graphWidth" />
				<xsl:with-param name="graphHeight" select="$graphHeight" />
				<xsl:with-param name="graphKind" select="$graphKind" />
			</xsl:call-template>
			</data>
		</xsl:if>
		
		<graph edgedefault="directed" id="{$graphId}">

		<xsl:apply-templates select="gml:node">
			<xsl:with-param name="parentX" select="$graphX" />
			<xsl:with-param name="parentY" select="$graphY" />
			<xsl:with-param name="graphWidth" select="$graphWidth" />
			<xsl:with-param name="graphHeight" select="$graphHeight" />
			<xsl:with-param name="transformNodeXKey" select="$transformNodeXKey" />
			<xsl:with-param name="transformNodeYKey" select="$transformNodeYKey" />
			<xsl:with-param name="transformNodeWidthKey" select="$transformNodeWidthKey" />
			<xsl:with-param name="transformNodeHeightKey" select="$transformNodeHeightKey" />
			<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey" />
			<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey" />
			<xsl:with-param name="transformNodeWorkerKey" select="$transformNodeWorkerKey" />
		</xsl:apply-templates>
		
		<xsl:apply-templates select="gml:edge" />

        </graph>

	</xsl:template>
	
   	<!-- select edges in graph -->
   	<xsl:template match="gml:edge">
		<!-- <xsl:copy>
			<xsl:apply-templates select="@*|node()"/>
		</xsl:copy> -->
		<xsl:variable name="edgeID" select="@id" />
		<xsl:variable name="source" select="@source" />
		<xsl:variable name="target" select="@target" />
	    <edge id="{$edgeID}" source="{$source}" target="{$target}" >
    	<xsl:apply-templates select="gml:data" mode="edge" />
		<xsl:if test="string(.) != ''">
			<data key="{$transformEdgeGraphicsKey}">
				<y:PolyLineEdge>
				<y:LineStyle color="#000000" type="dashed" width="1.0"/>
				<y:Arrows source="none" target="white_delta"/>
				<y:BendStyle smoothed="false"/>
				</y:PolyLineEdge>
			</data>
		</xsl:if>
		</edge>
	</xsl:template>
	
	<!-- copy all existing edge data -->
    <xsl:template match="gml:data" mode="edge">
		<xsl:copy>
			<xsl:apply-templates select="@*|node()"/>
		</xsl:copy>
    </xsl:template>	
	
	<!-- copy all existing node data -->
    <xsl:template match="gml:data" mode="node">
		<xsl:param name="transformNodeXKey" />
		<xsl:param name="transformNodeYKey" />
		<xsl:param name="transformNodeWidthKey" />
		<xsl:param name="transformNodeHeightKey" />
	
		<xsl:variable name="key" select="@key" />
		<xsl:variable name="value" select="string(.)" />
	    <data key="{$key}" >
		<xsl:choose>
		<xsl:when test="string(number(.)) = 'NaN' and $key = $transformNodeXKey">
			<xsl:value-of select="0" />
		</xsl:when>
		<xsl:when test="string(number(.)) = 'NaN' and $key = $transformNodeYKey">
			<xsl:value-of select="0" />
		</xsl:when>
		<xsl:when test="string(number(.)) = 'NaN' and $key = $transformNodeWidthKey">
			<xsl:value-of select="0" />
		</xsl:when>
		<xsl:when test="string(number(.)) = 'NaN' and $key = $transformNodeHeightKey">
			<xsl:value-of select="0" />
		</xsl:when>
		<xsl:otherwise>
			<xsl:value-of select="$value" />
		</xsl:otherwise>
		</xsl:choose>
		</data>
    </xsl:template>	
	
   	<!-- select nodes in graph -->
	<xsl:template match="gml:node">
		<xsl:param name="parentX" />
	    <xsl:param name="parentY" />
	    <xsl:param name="graphWidth" />
	    <xsl:param name="graphHeight" />
		<xsl:param name="transformNodeXKey" />
		<xsl:param name="transformNodeYKey" />
		<xsl:param name="transformNodeWidthKey" />
		<xsl:param name="transformNodeHeightKey" />
		<xsl:param name="transformNodeLabelKey" />
		<xsl:param name="transformNodeKindKey" />
		<xsl:param name="transformNodeWorkerKey" />
		
		<xsl:variable name="xValue"><xsl:value-of select="gml:data[@key=$transformNodeXKey]"/></xsl:variable>
		<xsl:variable name="yValue"><xsl:value-of select="gml:data[@key=$transformNodeYKey]"/></xsl:variable>
		<xsl:variable name="widthValue"><xsl:value-of select="gml:data[@key=$transformNodeWidthKey]"/></xsl:variable>
		<xsl:variable name="heightValue"><xsl:value-of select="gml:data[@key=$transformNodeHeightKey]"/></xsl:variable>
		<xsl:variable name="label"><xsl:value-of select="gml:data[@key=$transformNodeLabelKey]"/></xsl:variable>
		<xsl:variable name="kind"><xsl:value-of select="gml:data[@key=$transformNodeKindKey]"/></xsl:variable>
		<xsl:variable name="worker"><xsl:value-of select="gml:data[@key=$transformNodeWorkerKey]"/></xsl:variable>
	
	    <xsl:variable name="nodeID" select="@id" />
	    <node id="{$nodeID}">
    	    <xsl:apply-templates select="gml:data" mode="node" >
				<xsl:with-param name="transformNodeXKey" select="$transformNodeXKey" />
				<xsl:with-param name="transformNodeYKey" select="$transformNodeYKey" />
				<xsl:with-param name="transformNodeWidthKey" select="$transformNodeWidthKey" />
				<xsl:with-param name="transformNodeHeightKey" select="$transformNodeHeightKey" />
			</xsl:apply-templates>
			<!-- If we have a non group node assign the x, y, label and Image -->
			<xsl:if test="count(child::gml:graph) = 0">
    			<data key="{$transformNodeGraphicsKey}">	
				<y:ImageNode>
				<xsl:variable name="absX" select="$parentX + $xValue" />
				<xsl:variable name="absY" select="$parentY + $yValue" />
				<y:Geometry height="41.0" width="41.0" x="{$absX}" y="{$absY}" />
				<y:NodeLabel alignment="center" autoSizePolicy="node_width" modelName="sandwich" modelPosition="s" >
				<xsl:value-of select="$label" />
				</y:NodeLabel>
				<xsl:call-template name="image">
					<xsl:with-param name="kind" select="$kind" />
					<xsl:with-param name="worker" select="$worker" />
				</xsl:call-template>
				</y:ImageNode>
				</data>
			</xsl:if>
			
	    <xsl:apply-templates select="gml:graph">
		    <xsl:with-param name="graphId" select="concat($nodeID,':')" />
			<xsl:with-param name="graphLabel" select="$label" />
			<xsl:with-param name="graphX" select="$parentX + $xValue" />
			<xsl:with-param name="graphY" select="$parentY + $yValue" />
			<xsl:with-param name="graphWidth" select="$graphWidth" />
			<xsl:with-param name="graphHeight" select="$graphHeight" />
			<xsl:with-param name="graphKind" select="$kind" />
			<xsl:with-param name="transformNodeXKey" select="$transformNodeXKey" />
			<xsl:with-param name="transformNodeYKey" select="$transformNodeYKey" />
			<xsl:with-param name="transformNodeWidthKey" select="$transformNodeWidthKey" />
			<xsl:with-param name="transformNodeHeightKey" select="$transformNodeHeightKey" />
			<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey" />
			<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey" />
			<xsl:with-param name="transformNodeWorkerKey" select="$transformNodeWorkerKey" />
   	    </xsl:apply-templates>
        </node>
    </xsl:template>
   
    <!-- insert image graphics for group type nodes -->
    <xsl:template name="image">
	    <xsl:param name="kind" />
	    <xsl:param name="worker" />

		<xsl:choose>
			<xsl:when test="contains($kind,'InEventPort')">
				<y:Image alphaImage="true" refid="{$imageInEventPort}"/>
			</xsl:when>
			<xsl:when test="contains($kind,'OutEventPort')">
				<y:Image alphaImage="true" refid="{$imageOutEventPort}"/>
			</xsl:when>
			<xsl:when test="contains($kind,'Attribute')">
				<y:Image alphaImage="true" refid="{$imageAttribute}"/>
			</xsl:when>
			<xsl:when test="contains($kind,'IDL')">
				<y:Image alphaImage="true" refid="{$imageIDL}"/>
			</xsl:when>
			<xsl:when test="contains($kind,'Component') and not(contains($kind,'Assembly'))">
				<y:Image alphaImage="true" refid="{$imageComponent}"/>
			</xsl:when>
			<xsl:when test="$kind = 'Variable'">
				<y:Image alphaImage="true" refid="{$imageVariable}"/>
			</xsl:when>
			<xsl:when test="$kind = 'PeriodicEvent'">
				<y:Image alphaImage="true" refid="{$imagePeriodicEvent}"/>
			</xsl:when>
			<xsl:when test="$kind = 'Condition'">
				<y:Image alphaImage="true" refid="{$imageCondition}"/>
			</xsl:when>
			<xsl:when test="$kind = 'Termination'">
				<y:Image alphaImage="true" refid="{$imageTermination}"/>
			</xsl:when>
			<xsl:when test="$kind = 'Event'">
				<y:Image alphaImage="true" refid="{$imageEvent}"/>
			</xsl:when>
			<xsl:when test="contains($kind,'Aggregate')">
				<y:Image alphaImage="true" refid="{$imageAggregate}"/>
			</xsl:when>
			<xsl:when test="$kind = 'ArrayMember'">
				<y:Image alphaImage="true" refid="{$imageArrayMember}"/>
			</xsl:when>
			<xsl:when test="contains($kind,'Member')">
				<y:Image alphaImage="true" refid="{$imageMember}"/>
			</xsl:when>
			<xsl:when test="$kind = 'Workload'">
				<y:Image alphaImage="true" refid="{$imageWorkload}"/>
			</xsl:when>
			<xsl:when test="$kind = 'Process'">
				<xsl:choose>
				<xsl:when test="contains($worker,'Log')" >
					<y:Image alphaImage="true" refid="{$imageLOG}"/>
				</xsl:when>
				<xsl:when test="contains($worker,'CPU')" >
					<y:Image alphaImage="true" refid="{$imageCPU}"/>
				</xsl:when>
				<xsl:when test="contains($worker,'Memory')" >
					<y:Image alphaImage="true" refid="{$imageRAM}"/>
				</xsl:when>
				<xsl:when test="contains($worker,'Database')" >
					<y:Image alphaImage="true" refid="{$imageDB}"/>
				</xsl:when>
				<xsl:otherwise>
					<y:Image alphaImage="true" refid="{$imageProcess}"/>
				</xsl:otherwise>
				</xsl:choose>
				<!-- Other types of processes with different images  
				<xsl:when test="$kind = 'HDD'">
					<y:Image alphaImage="true" refid="{$imageHDD}"/>
				</xsl:when>
				<xsl:when test="$kind = 'Complexity'">
					<y:Image alphaImage="true" refid="{$imageComplexity}"/>
				</xsl:when> -->
			</xsl:when>
			<xsl:when test="$kind = 'ComponenetAssembly'">
				<y:Image alphaImage="true" refid="{$imageAssembly}"/>
			</xsl:when>
			<xsl:when test="$kind = 'BranchState'">
				<y:Image alphaImage="true" refid="{$imageBranchState}"/>
			</xsl:when>
			<xsl:when test="$kind = 'WhileLoop'">
				<y:Image alphaImage="true" refid="{$imageWhileLoop}"/>
			</xsl:when>
			<xsl:when test="$kind = 'InputParameter'">
				<y:Image alphaImage="true" refid="{$imageInputParameter}"/>
			</xsl:when>
			<xsl:when test="$kind = 'ReturnParameter'">
				<y:Image alphaImage="true" refid="{$imageReturnParameter}"/>
			</xsl:when>
			<xsl:when test="contains($kind,'Vector')">
				<y:Image alphaImage="true" refid="{$imageVector}"/>
			</xsl:when>
			<xsl:when test="$kind = 'ProvidedRequestPort'">
				<y:Image alphaImage="true" refid="{$imageProvidedRequestPort}"/>
			</xsl:when>
			<xsl:when test="$kind = 'RequiredRequestPort'">
				<y:Image alphaImage="true" refid="{$imageRequiredRequestPort}"/>
			</xsl:when>
			<xsl:when test="$kind = 'HardwareNode'">
				<y:Image alphaImage="true" refid="{$imageHardwareNode}"/>
			</xsl:when>
			<xsl:when test="$kind = 'HardwareCluster'">
				<y:Image alphaImage="true" refid="{$imageHardwareCluster}"/>
			</xsl:when>
			<xsl:otherwise>
				<y:Image alphaImage="true" refid="{$imageEvent}"/>
			</xsl:otherwise>
		</xsl:choose> 					
    </xsl:template>
	
    <!-- insert icon graphics for group type nodes -->
    <xsl:template name="icon">
   	    <xsl:param name="graphLabel" />
   	    <xsl:param name="graphX" />
	    <xsl:param name="graphY" />
	    <xsl:param name="graphWidth" />
	    <xsl:param name="graphHeight" />
	    <xsl:param name="graphKind" />

		<y:ProxyAutoBoundsNode>
		  <y:Realizers active="0">
			<y:GroupNode>
			  <y:Geometry x="{$graphX}" y="{$graphY}" width="{$graphWidth}" height="{$graphHeight}"/> 
			  <y:Fill color="#F5F5F5" transparent="false"/>
			  <y:BorderStyle color="#808080" type="line" width="2.0"/>
			  <xsl:choose>
				<xsl:when test="contains($graphKind,'InEventPort')">
					<xsl:variable name="icon" select="100 + $imageInEventPort"/>
					<y:NodeLabel alignment="right" autoSizePolicy="node_width" modelName="internal" modelPosition="t" fontSize="16" horizontalTextPosition="right" iconData="{$icon}" iconTextGap="4" verticalTextPosition="center" >
					<xsl:value-of select="$graphLabel" />
					</y:NodeLabel>
				</xsl:when>
				<xsl:when test="contains($graphKind,'OutEventPort')">
					<xsl:variable name="icon" select="100 + $imageOutEventPort"/>
					<y:NodeLabel alignment="right" autoSizePolicy="node_width" modelName="internal" modelPosition="t" fontSize="16" horizontalTextPosition="right" iconData="{$icon}" iconTextGap="4" verticalTextPosition="center" >
					<xsl:value-of select="$graphLabel" />
					</y:NodeLabel>
				</xsl:when>
				<xsl:when test="contains($graphKind,'Attribute')">
					<xsl:variable name="icon" select="100 + $imageAttribute"/>
					<y:NodeLabel alignment="right" autoSizePolicy="node_width" modelName="internal" modelPosition="t" fontSize="16" horizontalTextPosition="right" iconData="{$icon}" iconTextGap="4" verticalTextPosition="center" >
					<xsl:value-of select="$graphLabel" />
					</y:NodeLabel>
				</xsl:when>
				<xsl:when test="contains($graphKind,'IDL')">
					<xsl:variable name="icon" select="100 + $imageIDL"/>
					<y:NodeLabel alignment="right" autoSizePolicy="node_width" modelName="internal" modelPosition="t" fontSize="16" horizontalTextPosition="right" iconData="{$icon}" iconTextGap="4" verticalTextPosition="center" >
					<xsl:value-of select="$graphLabel" />
					</y:NodeLabel>
				</xsl:when>
				<xsl:when test="contains($graphKind,'Component') and not(contains($graphKind,'Assembly'))">
					<xsl:variable name="icon" select="100 + $imageComponent"/>
					<y:NodeLabel alignment="right" autoSizePolicy="node_width" modelName="internal" modelPosition="t" fontSize="16" horizontalTextPosition="right" iconData="{$icon}" iconTextGap="4" verticalTextPosition="center" >
					<xsl:value-of select="$graphLabel" />
					</y:NodeLabel>
				</xsl:when>
				<xsl:when test="$graphKind = 'Variable'">
					<xsl:variable name="icon" select="100 + $imageVariable"/>
					<y:NodeLabel alignment="right" autoSizePolicy="node_width" modelName="internal" modelPosition="t" fontSize="16" horizontalTextPosition="right" iconData="{$icon}" iconTextGap="4" verticalTextPosition="center" >
					<xsl:value-of select="$graphLabel" />
					</y:NodeLabel>
				</xsl:when>
				<xsl:when test="$graphKind = 'PeriodicEvent'">
					<xsl:variable name="icon" select="100 + $imagePeriodicEvent"/>
					<y:NodeLabel alignment="right" autoSizePolicy="node_width" modelName="internal" modelPosition="t" fontSize="16" horizontalTextPosition="right" iconData="{$icon}" iconTextGap="4" verticalTextPosition="center" >
					<xsl:value-of select="$graphLabel" />
					</y:NodeLabel>
				</xsl:when>
				<xsl:when test="$graphKind = 'Condition'">
					<xsl:variable name="icon" select="100 + $imageCondition"/>
					<y:NodeLabel alignment="right" autoSizePolicy="node_width" modelName="internal" modelPosition="t" fontSize="16" horizontalTextPosition="right" iconData="{$icon}" iconTextGap="4" verticalTextPosition="center" >
					<xsl:value-of select="$graphLabel" />
					</y:NodeLabel>
				</xsl:when>
				<xsl:when test="$graphKind = 'Termination'">
					<xsl:variable name="icon" select="100 + $imageTermination"/>
					<y:NodeLabel alignment="right" autoSizePolicy="node_width" modelName="internal" modelPosition="t" fontSize="16" horizontalTextPosition="right" iconData="{$icon}" iconTextGap="4" verticalTextPosition="center" >
					<xsl:value-of select="$graphLabel" />
					</y:NodeLabel>
				</xsl:when>
				<xsl:when test="$graphKind = 'Event'">
					<xsl:variable name="icon" select="100 + $imageEvent"/>
					<y:NodeLabel alignment="right" autoSizePolicy="node_width" modelName="internal" modelPosition="t" fontSize="16" horizontalTextPosition="right" iconData="{$icon}" iconTextGap="4" verticalTextPosition="center" >
					<xsl:value-of select="$graphLabel" />
					</y:NodeLabel>
				</xsl:when>
				<xsl:when test="contains($graphKind,'Aggregate')">
					<xsl:variable name="icon" select="100 + $imageAggregate"/>
					<y:NodeLabel alignment="right" autoSizePolicy="node_width" modelName="internal" modelPosition="t" fontSize="16" horizontalTextPosition="right" iconData="{$icon}" iconTextGap="4" verticalTextPosition="center" >
					<xsl:value-of select="$graphLabel" />
					</y:NodeLabel>
				</xsl:when>
				<xsl:when test="$graphKind = 'ArrayMember'">
					<xsl:variable name="icon" select="100 + $imageArrayMember"/>
					<y:NodeLabel alignment="right" autoSizePolicy="node_width" modelName="internal" modelPosition="t" fontSize="16" horizontalTextPosition="right" iconData="{$icon}" iconTextGap="4" verticalTextPosition="center" >
					<xsl:value-of select="$graphLabel" />
					</y:NodeLabel>
				</xsl:when>
				<xsl:when test="contains($graphKind,'Member')">
					<xsl:variable name="icon" select="100 + $imageMember"/>
					<y:NodeLabel alignment="right" autoSizePolicy="node_width" modelName="internal" modelPosition="t" fontSize="16" horizontalTextPosition="right" iconData="{$icon}" iconTextGap="4" verticalTextPosition="center" >
					<xsl:value-of select="$graphLabel" />
					</y:NodeLabel>
				</xsl:when>
				<xsl:when test="$graphKind = 'Workload'">
					<xsl:variable name="icon" select="100 + $imageWorkload"/>
					<y:NodeLabel alignment="right" autoSizePolicy="node_width" modelName="internal" modelPosition="t" fontSize="16" horizontalTextPosition="right" iconData="{$icon}" iconTextGap="4" verticalTextPosition="center" >
					<xsl:value-of select="$graphLabel" />
					</y:NodeLabel>
				</xsl:when>
				<xsl:when test="$graphKind = 'CPU'">
					<xsl:variable name="icon" select="100 + $imageCPU"/>
					<y:NodeLabel alignment="right" autoSizePolicy="node_width" modelName="internal" modelPosition="t" fontSize="16" horizontalTextPosition="right" iconData="{$icon}" iconTextGap="4" verticalTextPosition="center" >
					<xsl:value-of select="$graphLabel" />
					</y:NodeLabel>
				</xsl:when>
				<xsl:when test="$graphKind = 'DB'">
					<xsl:variable name="icon" select="100 + $imageDB"/>
					<y:NodeLabel alignment="right" autoSizePolicy="node_width" modelName="internal" modelPosition="t" fontSize="16" horizontalTextPosition="right" iconData="{$icon}" iconTextGap="4" verticalTextPosition="center" >
					<xsl:value-of select="$graphLabel" />
					</y:NodeLabel>
				</xsl:when>
				<xsl:when test="$graphKind = 'HDD'">
					<xsl:variable name="icon" select="100 + $imageHDD"/>
					<y:NodeLabel alignment="right" autoSizePolicy="node_width" modelName="internal" modelPosition="t" fontSize="16" horizontalTextPosition="right" iconData="{$icon}" iconTextGap="4" verticalTextPosition="center" >
					<xsl:value-of select="$graphLabel" />
					</y:NodeLabel>
				</xsl:when>
				<xsl:when test="$graphKind = 'RAM'">
					<xsl:variable name="icon" select="100 + $imageRAM"/>
					<y:NodeLabel alignment="right" autoSizePolicy="node_width" modelName="internal" modelPosition="t" fontSize="16" horizontalTextPosition="right" iconData="{$icon}" iconTextGap="4" verticalTextPosition="center" >
					<xsl:value-of select="$graphLabel" />
					</y:NodeLabel>
				</xsl:when>
				<xsl:when test="$graphKind = 'LOG'">
					<xsl:variable name="icon" select="100 + $imageLOG"/>
					<y:NodeLabel alignment="right" autoSizePolicy="node_width" modelName="internal" modelPosition="t" fontSize="16" horizontalTextPosition="right" iconData="{$icon}" iconTextGap="4" verticalTextPosition="center" >
					<xsl:value-of select="$graphLabel" />
					</y:NodeLabel>
				</xsl:when>
				<xsl:when test="$graphKind = 'Complexity'">
					<xsl:variable name="icon" select="100 + $imageComplexity"/>
					<y:NodeLabel alignment="right" autoSizePolicy="node_width" modelName="internal" modelPosition="t" fontSize="16" horizontalTextPosition="right" iconData="{$icon}" iconTextGap="4" verticalTextPosition="center" >
					<xsl:value-of select="$graphLabel" />
					</y:NodeLabel>
				</xsl:when>
				<xsl:when test="$graphKind = 'Activate'">
					<xsl:variable name="icon" select="100 + $imageProcess"/>
					<y:NodeLabel alignment="right" autoSizePolicy="node_width" modelName="internal" modelPosition="t" fontSize="16" horizontalTextPosition="right" iconData="{$icon}" iconTextGap="4" verticalTextPosition="center" >
					<xsl:value-of select="$graphLabel" />
					</y:NodeLabel>
				</xsl:when>
				<xsl:when test="$graphKind = 'Passivate'">
					<xsl:variable name="icon" select="100 + $imageProcess"/>
					<y:NodeLabel alignment="right" autoSizePolicy="node_width" modelName="internal" modelPosition="t" fontSize="16" horizontalTextPosition="right" iconData="{$icon}" iconTextGap="4" verticalTextPosition="center" >
					<xsl:value-of select="$graphLabel" />
					</y:NodeLabel>
				</xsl:when>
				<xsl:when test="$graphKind = 'Process'">
					<xsl:variable name="icon" select="100 + $imageProcess"/>
					<y:NodeLabel alignment="right" autoSizePolicy="node_width" modelName="internal" modelPosition="t" fontSize="16" horizontalTextPosition="right" iconData="{$icon}" iconTextGap="4" verticalTextPosition="center" >
					<xsl:value-of select="$graphLabel" />
					</y:NodeLabel>
				</xsl:when>
				<xsl:when test="$graphKind = 'Preprocess'">
					<xsl:variable name="icon" select="100 + $imageProcess"/>
					<y:NodeLabel alignment="right" autoSizePolicy="node_width" modelName="internal" modelPosition="t" fontSize="16" horizontalTextPosition="right" iconData="{$icon}" iconTextGap="4" verticalTextPosition="center" >
					<xsl:value-of select="$graphLabel" />
					</y:NodeLabel>
				</xsl:when>
				<xsl:when test="$graphKind = 'Postprocess'">
					<xsl:variable name="icon" select="100 + $imageProcess"/>
					<y:NodeLabel alignment="right" autoSizePolicy="node_width" modelName="internal" modelPosition="t" fontSize="16" horizontalTextPosition="right" iconData="{$icon}" iconTextGap="4" verticalTextPosition="center" >
					<xsl:value-of select="$graphLabel" />
					</y:NodeLabel>
				</xsl:when>
				<xsl:when test="$graphKind = 'ComponentAssembly'">
					<xsl:variable name="icon" select="100 + $imageAssembly"/>
					<y:NodeLabel alignment="right" autoSizePolicy="node_width" modelName="internal" modelPosition="t" fontSize="16" horizontalTextPosition="right" iconData="{$icon}" iconTextGap="4" verticalTextPosition="center" >
					<xsl:value-of select="$graphLabel" />
					</y:NodeLabel>
				</xsl:when>
				<xsl:when test="$graphKind = 'BranchState'">
					<xsl:variable name="icon" select="100 + $imageBranchState"/>
					<y:NodeLabel alignment="right" autoSizePolicy="node_width" modelName="internal" modelPosition="t" fontSize="16" horizontalTextPosition="right" iconData="{$icon}" iconTextGap="4" verticalTextPosition="center" >
					<xsl:value-of select="$graphLabel" />
					</y:NodeLabel>
				</xsl:when>
				<xsl:when test="$graphKind = 'WhileLoop'">
					<xsl:variable name="icon" select="100 + $imageWhileLoop"/>
					<y:NodeLabel alignment="right" autoSizePolicy="node_width" modelName="internal" modelPosition="t" fontSize="16" horizontalTextPosition="right" iconData="{$icon}" iconTextGap="4" verticalTextPosition="center" >
					<xsl:value-of select="$graphLabel" />
					</y:NodeLabel>
				</xsl:when>
				<xsl:when test="$graphKind = 'InputParameter'">
					<xsl:variable name="icon" select="100 + $imageInputParameter"/>
					<y:NodeLabel alignment="right" autoSizePolicy="node_width" modelName="internal" modelPosition="t" fontSize="16" horizontalTextPosition="right" iconData="{$icon}" iconTextGap="4" verticalTextPosition="center" >
					<xsl:value-of select="$graphLabel" />
					</y:NodeLabel>
				</xsl:when>
				<xsl:when test="$graphKind = 'ReturnParameter'">
					<xsl:variable name="icon" select="100 + $imageReturnParameter"/>
					<y:NodeLabel alignment="right" autoSizePolicy="node_width" modelName="internal" modelPosition="t" fontSize="16" horizontalTextPosition="right" iconData="{$icon}" iconTextGap="4" verticalTextPosition="center" >
					<xsl:value-of select="$graphLabel" />
					</y:NodeLabel>
				</xsl:when>
				<xsl:when test="contains($graphKind, 'Vector')">
					<xsl:variable name="icon" select="100 + $imageVector"/>
					<y:NodeLabel alignment="right" autoSizePolicy="node_width" modelName="internal" modelPosition="t" fontSize="16" horizontalTextPosition="right" iconData="{$icon}" iconTextGap="4" verticalTextPosition="center" >
					<xsl:value-of select="$graphLabel" />
					</y:NodeLabel>
				</xsl:when>
				<xsl:when test="$graphKind = 'ProvidedRequestPort'">
					<xsl:variable name="icon" select="100 + $imageProvidedRequestPort"/>
					<y:NodeLabel alignment="right" autoSizePolicy="node_width" modelName="internal" modelPosition="t" fontSize="16" horizontalTextPosition="right" iconData="{$icon}" iconTextGap="4" verticalTextPosition="center" >
					<xsl:value-of select="$graphLabel" />
					</y:NodeLabel>
				</xsl:when>
				<xsl:when test="$graphKind = 'RequiredRequestPort'">
					<xsl:variable name="icon" select="100 + $imageRequiredRequestPort"/>
					<y:NodeLabel alignment="right" autoSizePolicy="node_width" modelName="internal" modelPosition="t" fontSize="16" horizontalTextPosition="right" iconData="{$icon}" iconTextGap="4" verticalTextPosition="center" >
					<xsl:value-of select="$graphLabel" />
					</y:NodeLabel>
				</xsl:when>
				<xsl:when test="$graphKind = 'HardwareNode'">
					<xsl:variable name="icon" select="100 + $imageHardwareNode"/>
					<y:NodeLabel alignment="right" autoSizePolicy="node_width" modelName="internal" modelPosition="t" fontSize="16" horizontalTextPosition="right" iconData="{$icon}" iconTextGap="4" verticalTextPosition="center" >
					<xsl:value-of select="$graphLabel" />
					</y:NodeLabel>
				</xsl:when>
				<xsl:when test="$graphKind = 'HardwareCluster'">
					<xsl:variable name="icon" select="100 + $imageHardwareCluster"/>
					<y:NodeLabel alignment="right" autoSizePolicy="node_width" modelName="internal" modelPosition="t" fontSize="16" horizontalTextPosition="right" iconData="{$icon}" iconTextGap="4" verticalTextPosition="center" >
					<xsl:value-of select="$graphLabel" />
					</y:NodeLabel>
				</xsl:when>
				<xsl:otherwise>
					<y:NodeLabel alignment="right" autoSizePolicy="node_width" modelName="internal" modelPosition="t" fontSize="16" >
					<xsl:value-of select="$graphLabel" />
					</y:NodeLabel>
				</xsl:otherwise>
			  </xsl:choose> 			
			  <y:Shape type="roundrectangle"/>
			</y:GroupNode>
		  </y:Realizers>
		</y:ProxyAutoBoundsNode>
    </xsl:template>
   


	<xsl:template match="@*|node()">
	  <xsl:copy>
		<xsl:apply-templates select="@*|node()"/>
	  </xsl:copy>
	</xsl:template>

</xsl:stylesheet>