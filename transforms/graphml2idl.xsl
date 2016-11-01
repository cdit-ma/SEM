<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:gml="http://graphml.graphdrawing.org/xmlns"

	xmlns:exsl="http://exslt.org/common"
	xmlns:xalan="http://xml.apache.org/xslt"	
	xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" 
	xsi:schemaLocation="http://www.omg.org/Deployment Deployment.xsd"
	xmlns:redirect = "org.apache. xalan.xslt.extensions.Redirect"
	
	exclude-result-prefixes="gml exsl xalan">
		
    <xsl:output method="text" 
		omit-xml-declaration="yes"
        indent="no" />
	<xsl:strip-space elements="*" />
	
    <!--
        Purpose:

        This template will convert the .graphml project to the selected IDL file. 
		It will work on a raw graphml, but is better to run the PreprocessIDL.xsl 
		transform to give the correct sort order for Aggregate definitions.
		The Package parameter is for backward compatibility with xme files that have a Package defined.
    -->
	
	<!-- Runtime parameters -->
	<xsl:param name="File"></xsl:param>
	<xsl:param name="Package"></xsl:param>

	<!-- xml:space="preserve" ??? -->
	
	<!-- Assign default data keys as used by yEd 3.12.2 -->
	<xsl:include href="graphmlKeyVariables.xsl" />
	
	<!-- apply all templates starting from the root -->
    <xsl:template match="/">
		<!-- Assign the transform node key ids from existing keys -->
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
		
		<xsl:variable name="transformNodeSortOrderKey">
			<xsl:call-template name="findNodeKey">
				<xsl:with-param name="attrName" select="'sortOrder'" />
				<xsl:with-param name="defaultId" select="$nodeSortOrderKey" />
			</xsl:call-template>	
		</xsl:variable>

		<xsl:variable name="transformNodeTypeKey">
			<xsl:call-template name="findNodeKey">
				<xsl:with-param name="attrName" select="'type'" />
				<xsl:with-param name="defaultId" select="$nodeTypeKey" />
			</xsl:call-template>	
		</xsl:variable>	

		<xsl:variable name="transformNodeMaxSizeKey">
			<xsl:call-template name="findNodeKey">
				<xsl:with-param name="attrName" select="'max_size'" />
				<xsl:with-param name="defaultId" select="$nodeMaxSizeKey" />
			</xsl:call-template>	
		</xsl:variable>	
		
		<xsl:variable name="transformNodeKeyMemberKey">
			<xsl:call-template name="findNodeKey">
				<xsl:with-param name="attrName" select="'key'" />
				<xsl:with-param name="defaultId" select="$nodeKeyMemberKey" />
			</xsl:call-template>	
		</xsl:variable>			
		
		<xsl:variable name="transformNodeFolderKey">
			<xsl:call-template name="findNodeKey">
				<xsl:with-param name="attrName" select="'folder'" />
				<xsl:with-param name="defaultId" select="$nodeFolderKey" />
			</xsl:call-template>	
		</xsl:variable>			
		
		<!-- find requested IDL file node -->
		<!-- trim extension if it exists -->
		<xsl:variable name="nodeName" >
			<xsl:value-of select="substring-before(concat($File, '.'),'.')" />
		</xsl:variable>
				
		<xsl:variable name="uppercaseFile" select="translate($nodeName,
                                'abcdefghijklmnopqrstuvwxyz',
                                'ABCDEFGHIJKLMNOPQRSTUVWXYZ')" />
		
		<!-- Write start of idl text file -->
		<xsl:value-of select="concat('#ifndef ', '_', $uppercaseFile, '_IDL_', '&#xA;')"/>
		<xsl:value-of select="concat('#define ', '_', $uppercaseFile, '_IDL_', '&#xA;', '&#xA;')"/>

		<!-- find requested file node -->
		<xsl:variable name="fileNode" select="/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'IDL']/../gml:data[@key=$transformNodeLabelKey][text() = $nodeName]/.." />

		<!-- include files -->
		<!-- Generate the basename for this file. -->
		<xsl:variable name="folderName" select="$fileNode/gml:data[@key=$transformNodeFolderKey]/text()" />
		<xsl:variable name="fileName" select="$fileNode/gml:data[@key=$transformNodeLabelKey]/text()" />
		<xsl:variable name="basename">
			<xsl:choose>
			<xsl:when test="$folderName != ''" >
				<xsl:value-of select="concat($folderName, '/', $fileName)" />
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="$fileName" />
			</xsl:otherwise>
		</xsl:choose>
		</xsl:variable>

		<xsl:variable name="componentNodes" select="$fileNode/gml:graph/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'Component']
												  | $fileNode/gml:graph/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'BlackBox']" />
		<xsl:if test="count($componentNodes) &gt; 0" >
			<xsl:value-of select="concat('#include &lt;Components.idl&gt;', '&#xA;')" />
		</xsl:if>

		<xsl:variable name="interfaceDefs" select="/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'InterfaceDefinitions']/.." />
		
		<!-- Generate the include files for aggregates and vectors used in this file. -->
		<xsl:variable name="childrenIds" select="$fileNode/descendant::*/gml:node/@id" />
		<xsl:variable name="refNodeIds" select="/descendant::*/gml:edge[@source=$childrenIds]/@target" />
		<xsl:variable name="refFiles" select="$interfaceDefs/descendant::*/gml:node[@id=$refNodeIds]/gml:data[@key=$transformNodeKindKey][text() = 'Aggregate' or text() =  'Vector']/ancestor::*/gml:data[@key=$transformNodeKindKey][text() = 'IDL']/.." />
		<xsl:for-each select="$refFiles">
			<xsl:variable name="includeFileName" select="./gml:data[@key=$transformNodeLabelKey]/text()" />
			<xsl:if test="$includeFileName != $nodeName">
				<!-- Generate the includename for this include file. -->
				<xsl:variable name="includeFolderName" select="$refFiles/gml:data[@key=$transformNodeFolderKey]/text()" />
				<xsl:variable name="includename">
					<xsl:choose>
					<xsl:when test="$includeFolderName != ''" >
						<xsl:value-of select="concat($includeFolderName, '/', $includeFileName)" />
					</xsl:when>
					<xsl:otherwise>
						<xsl:value-of select="$includeFileName" />
					</xsl:otherwise>
				</xsl:choose>
				</xsl:variable>
				<xsl:value-of select="concat('#include &quot;', $includename, '.idl&quot;', '&#xA;')" />
			</xsl:if>
		</xsl:for-each>
		
		<!-- Write the pragma statement for the local executor mapping. ??? this->file_has_object_with_reference (file) ???-->
		<xsl:if test="count($componentNodes) &gt; 0" >
			<xsl:value-of select="concat('#pragma ciao lem &quot;', $basename, 'E.idl&quot;', '&#xA;')" />
		</xsl:if>

		<!-- Write the pragma statement for the type support. if any aggregate has a key -->
		<xsl:variable name="allKeyMembers" select="$fileNode/gml:graph/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'Aggregate']/../gml:graph/gml:node/gml:data[@key=$transformNodeKeyMemberKey][text() = 'true']/.." />
		<xsl:if test="count($allKeyMembers) &gt; 0">		
			<xsl:value-of select="'&#xA;'" />
			<xsl:value-of select="concat('#pragma ndds typesupport &quot;', $basename, 'Support.h&quot;', '&#xA;')" />
			<xsl:value-of select="concat('#pragma opendds typesupport &quot;', $basename, 'TypeSupportC.h&quot;', '&#xA;')" />
			<xsl:value-of select="concat('#pragma splice typesupport &quot;', $basename, 'DscpC.h&quot;', '&#xA;')" />
		</xsl:if>
		<xsl:value-of select="'&#xA;'"/>
		
		<!-- Write start the Package/module if specified as a parameter, ie backward compatible with xme -->
		<xsl:if test="$Package != ''" >
			<xsl:value-of select="concat('module ', $Package)" />
			<xsl:value-of select="concat('&#xA;', '{', '&#xA;')" />
		</xsl:if>

		<!-- Output Events if this file contains Components or BlackBox -->
		<xsl:variable name="componentChildrenIds" 
		select="$fileNode/gml:graph/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'Component']/../gml:graph/gml:node/@id
		    	| $fileNode/gml:graph/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'BlackBox']/../gml:graph/gml:node/@id" />
		<xsl:variable name="componentRefNodeIds" select="/descendant::*/gml:edge[@source=$componentChildrenIds]/@target" />
		<xsl:variable name="refNodes" select="$interfaceDefs/descendant::*/gml:node[@id=$componentRefNodeIds]/gml:data[@key=$transformNodeKindKey][text() = 'Aggregate']/.." />
		<xsl:for-each select="$refNodes">
			<xsl:call-template name="Event">
				<xsl:with-param name="eventNode" select="."/> <!-- aggregate used to create event -->
				<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey"/>
				<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey"/>
				<xsl:with-param name="transformNodeSortOrderKey" select="$transformNodeSortOrderKey"/>
				<xsl:with-param name="transformNodeTypeKey" select="$transformNodeTypeKey"/>
				<xsl:with-param name="transformNodeKeyMemberKey" select="$transformNodeKeyMemberKey"/>
			</xsl:call-template>
		</xsl:for-each>
		
		<!-- Output all Aggregates or Components, can assume that only one or the other type exists in the File -->
		<xsl:for-each select="$fileNode/gml:graph/gml:node" >
			<xsl:sort select="./gml:data[@key=$transformNodeSortOrderKey]" data-type="number" order="ascending" />
			
			<xsl:choose>
			<xsl:when test="./gml:data[@key=$transformNodeKindKey]/text() = 'Aggregate'" >
				<xsl:call-template name="Aggregate">
					<xsl:with-param name="aggregateNode" select="."/>
					<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey"/>
					<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey"/>
					<xsl:with-param name="transformNodeSortOrderKey" select="$transformNodeSortOrderKey"/>
					<xsl:with-param name="transformNodeTypeKey" select="$transformNodeTypeKey"/>
					<xsl:with-param name="transformNodeKeyMemberKey" select="$transformNodeKeyMemberKey"/>
				</xsl:call-template>
			</xsl:when>
			<xsl:when test="./gml:data[@key=$transformNodeKindKey]/text() = 'Vector'" >
				<xsl:call-template name="Vector">
					<xsl:with-param name="vectorNode" select="."/>
					<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey"/>
					<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey"/>
					<xsl:with-param name="transformNodeTypeKey" select="$transformNodeTypeKey"/>
					<xsl:with-param name="transformNodeMaxSizeKey" select="$transformNodeMaxSizeKey"/>
				</xsl:call-template>
			</xsl:when>
			<xsl:when test="./gml:data[@key=$transformNodeKindKey]/text() = 'Component'" >
				<xsl:call-template name="Component">
					<xsl:with-param name="componentNode" select="."/>
					<xsl:with-param name="interfaceDefs" select="$interfaceDefs"/>
					<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey"/>
					<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey"/>
					<xsl:with-param name="transformNodeSortOrderKey" select="$transformNodeSortOrderKey"/>
					<xsl:with-param name="transformNodeTypeKey" select="$transformNodeTypeKey"/>
					<xsl:with-param name="transformNodeKeyMemberKey" select="$transformNodeKeyMemberKey"/>
				</xsl:call-template>
			</xsl:when>
			<xsl:when test="./gml:data[@key=$transformNodeKindKey]/text() = 'BlackBox'" >
				<xsl:call-template name="Component">
					<xsl:with-param name="componentNode" select="."/>
					<xsl:with-param name="interfaceDefs" select="$interfaceDefs"/>
					<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey"/>
					<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey"/>
					<xsl:with-param name="transformNodeSortOrderKey" select="$transformNodeSortOrderKey"/>
					<xsl:with-param name="transformNodeTypeKey" select="$transformNodeTypeKey"/>
					<xsl:with-param name="transformNodeKeyMemberKey" select="$transformNodeKeyMemberKey"/>
				</xsl:call-template>
			</xsl:when>
			<xsl:otherwise>
			</xsl:otherwise>
			</xsl:choose>
			
		</xsl:for-each>
		
		<!-- Write end of package. -->
		<xsl:if test="$Package != ''" >
		<xsl:value-of select="concat('};', '&#xA;')" />
		</xsl:if>
			
		<!-- Write end of idl text file -->
		<xsl:value-of select="concat('&#xA;', '#endif    // !defined ', '_', $uppercaseFile, '_IDL_', '&#xA;', '&#xA;')"/>
		<xsl:value-of select="'&#xA;'"/>
					
    </xsl:template>

	<!-- Aggregate -->	
	<xsl:template name="Aggregate">		
		<xsl:param name="aggregateNode" />
		<xsl:param name="transformNodeKindKey" />
		<xsl:param name="transformNodeLabelKey" />
		<xsl:param name="transformNodeSortOrderKey" />
		<xsl:param name="transformNodeTypeKey" />
		<xsl:param name="transformNodeKeyMemberKey" />

		<xsl:variable name="aggregateName" select="$aggregateNode/gml:data[@key=$transformNodeLabelKey]/text()" />

		<xsl:variable name="EventPortRefNodeIds" select="/descendant::*/gml:edge[@target=$aggregateNode/@id]/@source" />
		<xsl:variable name="EventPortRefNodes" select="/descendant::*/gml:node[@id=$EventPortRefNodeIds]/gml:data[@key=$transformNodeKindKey][text() = 'OutEventPort' or text() = 'InEventPort']/.." />

		<!-- find KeyMembers for Aggregate -->
		<xsl:variable name="keyMembers" select="$aggregateNode/gml:graph/gml:node/gml:data[@key=$transformNodeKeyMemberKey][text() = 'true']/.." />
		<xsl:if test="count($keyMembers) &gt; 0" >
			<!-- We can declare this as a DDS type. -->
			<xsl:value-of select="concat('#pragma DCPS_DATA_TYPE &quot;', $aggregateName, '&quot;', '&#xA;')" />
 			<xsl:for-each select="$keyMembers" >
				<xsl:sort select="./gml:data[@key=$transformNodeSortOrderKey]" data-type="number" order="ascending" />
				<xsl:value-of select="concat('#pragma DCPS_DATA_KEY &quot;', $aggregateName, ' ', ./gml:data[@key=$transformNodeLabelKey]/text(), '&quot;', '&#xA;')" />
			</xsl:for-each>
			<xsl:value-of select="'&#xA;'" />
		</xsl:if>
		
		<!-- Write start of Aggregate -->
		<xsl:value-of select="concat('struct ', $aggregateName, '&#xA;', '{')" />
		
		<!-- Members and AggregateInstances of Aggregate -->
		<xsl:for-each select="$aggregateNode/gml:graph/gml:node">
			<xsl:sort select="./gml:data[@key=$transformNodeSortOrderKey]" data-type="number" order="ascending" /> 
			
			<!-- Process Members and AggregateInstances using the same template -->
			<xsl:call-template name="Member">
				<xsl:with-param name="memberNode" select="."/>
				<xsl:with-param name="isEvent" select="0"/>
				<xsl:with-param name="keyMember" select="./gml:data[@key=$transformNodeKeyMemberKey]/text()"/> 
				<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey"/>
				<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey"/>
				<xsl:with-param name="transformNodeTypeKey" select="$transformNodeTypeKey"/>
			</xsl:call-template> 
			
		</xsl:for-each>
		
		<!-- Write end of Aggregate -->
		<xsl:value-of select="concat('};', '&#xA;', '&#xA;')" />
		
		<!-- KeyMembers for Aggregate -->
		<xsl:choose>
		<xsl:when test="count($keyMembers) &gt; 0" >
		    <!-- Since there is a key in this aggregate, we need to write at least an empty key list. -->
			<xsl:value-of select="concat('#pragma keylist ', $aggregateName )" />
			<xsl:for-each select="$keyMembers">
				<xsl:sort select="./gml:data[@key=$transformNodeSortOrderKey]" data-type="number" order="ascending" />
				<xsl:value-of select="concat(' ', ./gml:data[@key=$transformNodeLabelKey]/text() )" />
			</xsl:for-each>
			<xsl:value-of select="concat('&#xA;', '&#xA;')" />
		</xsl:when>
		<xsl:otherwise>
			<!-- If there are no keyMembers, but this aggregate is used as a topic must have empty keylist -->
			<xsl:if test="count($EventPortRefNodes) &gt; 0" >
				<xsl:value-of select="concat('#pragma keylist ', $aggregateName, '&#xA;')" />
			</xsl:if>
		</xsl:otherwise>
		</xsl:choose>

	</xsl:template>

	<!-- Member -->
	<xsl:template name="Member">		
		<xsl:param name="memberNode" />
		<xsl:param name="isEvent" />
		<xsl:param name="keyMember" />
		<xsl:param name="transformNodeKindKey" />
		<xsl:param name="transformNodeLabelKey" />
		<xsl:param name="transformNodeTypeKey" />
		
		<xsl:value-of select="'&#xA;'" />

		<xsl:if test="$isEvent != 0">  
			<xsl:value-of select="'public '" /> 
		</xsl:if>
		
		<xsl:choose>
		<xsl:when test="$memberNode/gml:data[@key=$transformNodeKindKey][text() = 'Member']">
			<xsl:call-template name="MemberType">
				<xsl:with-param name="type" select="$memberNode/gml:data[@key=$transformNodeTypeKey]/text()"/>
			</xsl:call-template>
		</xsl:when>
		<xsl:otherwise>
			<xsl:value-of select="'::'" />
			<xsl:if test="$Package != ''">
				<xsl:value-of select="concat($Package, '::')" />
			</xsl:if>
			<xsl:value-of select="$memberNode/gml:data[@key=$transformNodeTypeKey]/text()" />
		</xsl:otherwise>
		</xsl:choose>
		
		<xsl:value-of select="concat(' ', $memberNode/gml:data[@key=$transformNodeLabelKey]/text(), ';')" />

		<xsl:if test="$keyMember = 'true'" >
			<xsl:value-of select="'  // @key'" />
		</xsl:if>
		
		<xsl:value-of select="'&#xA;'" />
	</xsl:template>

	<!--Event -->
	<xsl:template name="Event">	
		<xsl:param name="eventNode" />
		<xsl:param name="transformNodeKindKey" />
		<xsl:param name="transformNodeLabelKey" />
		<xsl:param name="transformNodeSortOrderKey" />
		<xsl:param name="transformNodeTypeKey" />
		<xsl:param name="transformNodeKeyMemberKey" />
		
		<xsl:value-of select="concat('eventtype ', $eventNode/gml:data[@key=$transformNodeLabelKey]/text(), 'Event')" />
		<xsl:value-of select="concat('&#xA;', '{')" />
		
		<xsl:for-each select="$eventNode/gml:graph/gml:node">
			<xsl:sort select="./gml:data[@key=$transformNodeSortOrderKey]" data-type="number" order="ascending" /> 
			<xsl:call-template name="Member">
				<xsl:with-param name="memberNode" select="."/>
				<xsl:with-param name="isEvent" select="1"/>
				<xsl:with-param name="isKeyMember" select="0"/>  <!-- key only used in Aggregates not Events-->
				<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey"/>
				<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey"/>
				<xsl:with-param name="transformNodeTypeKey" select="$transformNodeTypeKey"/>
			</xsl:call-template>
		</xsl:for-each>
		<xsl:value-of select="concat('};', '&#xA;')" />
	</xsl:template>

	<!-- Vector -->
	<xsl:template name="Vector">
		<xsl:param name="vectorNode" />
		<xsl:param name="transformNodeKindKey" />
		<xsl:param name="transformNodeLabelKey" />
		<xsl:param name="transformNodeTypeKey" />
		<xsl:param name="transformNodeMaxSizeKey" />
		
		<xsl:variable name="vectorName" select="$vectorNode/gml:data[@key=$transformNodeLabelKey]/text()" />
		<xsl:variable name="vectorBound" select="$vectorNode/gml:data[@key=$transformNodeMaxSizeKey]/text()" />
		
		<xsl:value-of select="'typedef sequence &lt; '"/>

		<xsl:variable name="memberNode" select="$vectorNode/gml:graph/gml:node" />
		<!-- should be only one memberNode -->
		<xsl:choose>
		<xsl:when test="$memberNode[1]/gml:data[@key=$transformNodeKindKey][text() = 'Member']">
			<xsl:call-template name="MemberType">
				<xsl:with-param name="type" select="$memberNode[1]/gml:data[@key=$transformNodeTypeKey]/text()"/>
			</xsl:call-template>
		</xsl:when>
		<xsl:otherwise>
			<xsl:value-of select="'::'" />
			<xsl:if test="$Package != ''">
				<xsl:value-of select="concat($Package, '::')" />
			</xsl:if>
			<xsl:value-of select="$memberNode[1]/gml:data[@key=$transformNodeTypeKey]/text()" />
		</xsl:otherwise>
		</xsl:choose>
		
		<xsl:if test="$vectorBound != ''">
			<xsl:value-of select="concat(', ', $vectorBound)" />
		</xsl:if>
		<xsl:value-of select="concat(' &gt; ', $vectorName, ';', '&#xA;&#xA;')" />

	</xsl:template>

	<!-- Component -->
	<xsl:template name="Component">			
		<xsl:param name="componentNode" />
		<xsl:param name="interfaceDefs" />
		<xsl:param name="transformNodeKindKey" />
		<xsl:param name="transformNodeLabelKey" />
		<xsl:param name="transformNodeSortOrderKey" />
		<xsl:param name="transformNodeTypeKey" />
		<xsl:param name="transformNodeKeyMemberKey" />
		
		<xsl:value-of select="concat('&#xA;', 'component ', $componentNode/gml:data[@key=$transformNodeLabelKey]/text() )" />

		<xsl:value-of select="concat('&#xA;', '{', '&#xA;')" />
			
		 <!-- // Write each of the ports for the component. -->
		<xsl:for-each select="$componentNode/gml:graph/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'InEventPort']/..">
			<xsl:variable name="inEventComponent" select="." />
			<xsl:variable name="inEventPortRefNodeIds" select="/descendant::*/gml:edge[@source=$inEventComponent/@id]/@target" />
			<xsl:variable name="inEventPortRefNodes" select="$interfaceDefs/descendant::*/gml:node[@id=$inEventPortRefNodeIds]/gml:data[@key=$transformNodeKindKey][text() = 'Aggregate']/.." />
			<xsl:variable name="inEventPortRef" >
				<xsl:value-of select="'::'" />
				<xsl:if test="$Package != ''">
					<xsl:value-of select="concat($Package, '::')" />
				</xsl:if>
				<!-- should be only one aggregate that this port uses -->
				<xsl:value-of select="concat($inEventPortRefNodes[1]/gml:data[@key=$transformNodeLabelKey]/text(), 'Event')"/>
			</xsl:variable>
			<xsl:call-template name="InEventPort">
				<xsl:with-param name="portRef" select="$inEventPortRef"/>
				<xsl:with-param name="portName" select="./gml:data[@key=$transformNodeLabelKey]/text()"/>
			</xsl:call-template>
		</xsl:for-each>

		<xsl:for-each select="$componentNode/gml:graph/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'OutEventPort']/..">
			<xsl:variable name="outEventComponent" select="." />
			<xsl:variable name="outEventPortRefNodeIds" select="/descendant::*/gml:edge[@source=$outEventComponent/@id]/@target" />
			<xsl:variable name="outEventPortRefNodes" select="$interfaceDefs/descendant::*/gml:node[@id=$outEventPortRefNodeIds]/gml:data[@key=$transformNodeKindKey][text() = 'Aggregate']/.." />
			<xsl:variable name="outEventPortRef" >
				<xsl:value-of select="'::'" />
				<xsl:if test="$Package != ''">
					<xsl:value-of select="concat($Package, '::')" />
				</xsl:if>
				<!-- should be only one aggregate that this port uses -->
				<xsl:value-of select="concat($outEventPortRefNodes[1]/gml:data[@key=$transformNodeLabelKey]/text(), 'Event')"/>
			</xsl:variable>
			<xsl:call-template name="OutEventPort">
				<xsl:with-param name="portRef" select="$outEventPortRef"/>
				<xsl:with-param name="portName" select="./gml:data[@key=$transformNodeLabelKey]/text()"/>
			</xsl:call-template>
		</xsl:for-each>
		
		<!-- Also RequestPorts when implemented in SEM 
		 	<xsl:call-template name="ProvidedRequestPort">
				<xsl:with-param name="portRef" />
				<xsl:with-param name="portName" />
			</xsl:call-template>
		 	<xsl:call-template name="RequiredRequestPort">
				<xsl:with-param name="portRef" />
				<xsl:with-param name="portName" />
			</xsl:call-template> -->
		
		<xsl:for-each select="$componentNode/gml:graph/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'Attribute']/..">
			<xsl:call-template name="Attribute">
				<xsl:with-param name="attributeNode" select="."/>
				<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey"/>
				<xsl:with-param name="transformNodeTypeKey" select="$transformNodeTypeKey"/>
			</xsl:call-template> 
		</xsl:for-each>
			
		<xsl:value-of select="concat('};', '&#xA;')" />
	</xsl:template>

	<!-- ProvidedRequestPort -->
	<xsl:template name="ProvidedRequestPort">	
		<xsl:param name="portRef" />
		<xsl:param name="portName" />
		
		<xsl:value-of select="concat('provides ', $portRef, ' ', $portName, ';', '&#xA;')" />
	</xsl:template>

	<!-- RequiredRequestPort -->
	<xsl:template name="RequiredRequestPort">			
		<xsl:param name="portRef" />
		<xsl:param name="portName" />
		<xsl:param name="multiConnection" select="0"/>

		<xsl:value-of select="'uses '" />
		<xsl:if test="$multiConnection &gt; 1">
			<xsl:value-of select="'multiple '" />
		</xsl:if>
		<xsl:value-of select="concat($portRef, ' ', $portName, ';', '&#xA;')" />
	</xsl:template>

	<!-- InEventPort -->
	<xsl:template name="InEventPort">			
		<xsl:param name="portRef" />
		<xsl:param name="portName" />

		<xsl:value-of select="concat('consumes ', $portRef, ' ', $portName, ';', '&#xA;')" />
	</xsl:template>

	<!-- OutEventPort -->
	<xsl:template name="OutEventPort">			
		<xsl:param name="portRef" />
		<xsl:param name="portName" />

		<!--		<xsl:value-of select="'emits '" />     ??? single_destination true/false flag in model ??? -->
		
		<xsl:value-of select="'publishes '" />

		<xsl:value-of select="concat($portRef, ' ', $portName, ';', '&#xA;')" />
	</xsl:template>

	<!-- Attribute -->
	<xsl:template name="Attribute">		
		<xsl:param name="attributeNode" />
		<xsl:param name="transformNodeLabelKey" />
		<xsl:param name="transformNodeTypeKey" />

		<xsl:value-of select="'attribute '" />
		<xsl:call-template name="MemberType">
			<xsl:with-param name="type" select="$attributeNode/gml:data[@key=$transformNodeTypeKey]/text()"/>
		</xsl:call-template>
		<xsl:value-of select="concat(' ', $attributeNode/gml:data[@key=$transformNodeLabelKey]/text(), ';', '&#xA;')" />
		
	</xsl:template>

	<!-- Member or Event Type -->	
	<xsl:template name="MemberType">	
		<xsl:param name="type" />
		
		<xsl:choose>
			<xsl:when test="$type = 'Byte'" >
				<xsl:value-of select="'octet'" />
			</xsl:when>
			<xsl:when test="$type = 'Char'" >
				<xsl:value-of select="'char'" />
			</xsl:when>
			<xsl:when test="$type = 'WideChar'" >
				<xsl:value-of select="'wchar'" />
			</xsl:when>	
			<xsl:when test="$type = 'Boolean'" >
				<xsl:value-of select="'boolean'" />
			</xsl:when>
			<xsl:when test="$type = 'String'" >
				<xsl:value-of select="'string'" />
			</xsl:when>
			<xsl:when test="$type = 'WideString'" >
				<xsl:value-of select="'wstring'" />
			</xsl:when>
			<xsl:when test="$type = 'UnsignedShortInteger'" >
				<xsl:value-of select="'unsigned short'" />
			</xsl:when>
			<xsl:when test="$type = 'UnsignedLongInteger'" >
				<xsl:value-of select="'unsigned long'" />
			</xsl:when>
			<xsl:when test="$type = 'UnsignedLongLongInteger'" >
				<xsl:value-of select="'unsigned long long'" />
			</xsl:when>
			<xsl:when test="$type = 'ShortInteger'" >
				<xsl:value-of select="'short'" />
			</xsl:when>
			<xsl:when test="$type = 'LongInteger'" >
				<xsl:value-of select="'long'" />
			</xsl:when>
			<xsl:when test="$type = 'LongLongInteger'" >
				<xsl:value-of select="'long long'" />
			</xsl:when>
			<xsl:when test="$type = 'FloatNumber'" >
				<xsl:value-of select="'float'" />
			</xsl:when>
			<xsl:when test="$type = 'DoubleNumber'" >
				<xsl:value-of select="'double'" />
			</xsl:when>
			<xsl:when test="$type = 'LongDoubleNumber'" >
				<xsl:value-of select="'long double'" />
			</xsl:when>
			<xsl:when test="$type = 'GenericObject'" >
				<xsl:value-of select="'Object'" />
			</xsl:when>
			<xsl:when test="$type = 'GenericValue'" >
				<xsl:value-of select="'any'" />
			</xsl:when>
			<xsl:when test="$type = 'GenericValueObject'" >
				<xsl:value-of select="'ValueBase'" />
			</xsl:when>
			<!-- return type given as is, model validation should identify if type is incorrect -->
			<xsl:otherwise> 
				<xsl:value-of select="$type" />
			</xsl:otherwise>
		</xsl:choose>
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