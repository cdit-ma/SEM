<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="2.0" 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:gml="http://graphml.graphdrawing.org/xmlns"
	xmlns="http://graphml.graphdrawing.org/xmlns"
	xmlns:exsl="http://exslt.org/common"
	exclude-result-prefixes="gml #default exsl" >
	
    <xsl:output method="xml" 
        version="1.0" 
        indent="yes" />
		<!-- doctype-system="graphml.dtd"  can use this if validation method uses dtd, for graphml xml better to use schemaLocation-->
	<xsl:strip-space elements="*" />
 
    <!--
        Purpose:

        This template will convert the PICML project to a .graphml 
        file. This captures Interface Definitions, Behaviour/Workload
		and Assembly Definitions. The Interface Definitions contain 
		node data to create the required IDL files. The Workload Execution 
		Definition captures the Behaviour as well as the Event and Member 
		nodes. The Assembly Definitions create the Component instances 
		with links to the InterfaceDefinition nodes.
    -->
	
	
	<!-- Assign data keys as used by yEd 3.12.2 -->
	<xsl:include href="graphmlKeyVariables.xsl" />
	
	<!-- apply all templates starting from the root -->
    <xsl:template match="/">
	    <!-- Output start of graph -->
		<graphml
			xmlns="http://graphml.graphdrawing.org/xmlns"  
			xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
			xsi:schemaLocation="http://graphml.graphdrawing.org/xmlns http://graphml.graphdrawing.org/xmlns/1.0/graphml.xsd" >
			<!-- Assign id's for data keys -->
			<!--    convention is to have id="d0..." but yEd may reassign, see Properties Mapper... -->
			
			<!-- Default yEd data keys -->
			<key attr.name="url" attr.type="string" for="node" id="{$nodeURLKey}"/>
			<key attr.name="description" attr.type="string" for="node" id="{$nodeDescriptionKey}"/>
			<key attr.name="Description" attr.type="string" for="graph" id="{$graphDescriptionKey}"/>
			<key attr.name="url" attr.type="string" for="edge" id="{$edgeURLKey}"/>
			<key attr.name="description" attr.type="string" for="edge" id="{$edgeDescriptionKey}"/>
			
			<!-- PICML specific attributes -->
			<key attr.name="label" attr.type="string" for="node" id="{$nodeLabelKey}"/>
			<key attr.name="x" attr.type="double" for="node" id="{$nodeXKey}"/>
			<key attr.name="y" attr.type="double" for="node" id="{$nodeYKey}"/>
			<key attr.name="kind" attr.type="string" for="node" id="{$nodeKindKey}"/>
			<key attr.name="type" attr.type="string" for="node" id="{$nodeTypeKey}"/>
			<key attr.name="value" attr.type="string" for="node" id="{$nodeValueKey}"/>
			<key attr.name="key" attr.type="boolean" for="node" id="{$nodeKeyMemberKey}"/>
			
			<key attr.name="middleware" attr.type="string" for="node" id="{$nodeMiddlewareKey}"/>
			<key attr.name="topicName" attr.type="string" for="node" id="{$nodeTopicNameKey}"/>
			<key attr.name="async" attr.type="boolean" for="node" id="{$nodeAsyncKey}"/>
			<key attr.name="stream" attr.type="boolean" for="node" id="{$nodeStreamKey}"/>

			<key attr.name="folder" attr.type="string" for="node" id="{$nodeFolderKey}"/>
			<key attr.name="file" attr.type="string" for="node" id="{$nodeFileKey}"/>
			<key attr.name="project" attr.type="string" for="node" id="{$nodeProjectKey}"/>
			<key attr.name="container" attr.type="string" for="node" id="{$nodeContainerKey}"/>
			<key attr.name="worker" attr.type="string" for="node" id="{$nodeWorkerKey}"/>
			<key attr.name="operation" attr.type="string" for="node" id="{$nodeOperationKey}"/>
			<key attr.name="actionOn" attr.type="string" for="node" id="{$nodeActionOnKey}"/>
			<key attr.name="complexity" attr.type="string" for="node" id="{$nodeComplexityKey}"/>
			<key attr.name="parameters" attr.type="string" for="node" id="{$nodeParametersKey}"/>

			<key attr.name="ip_address" attr.type="string" for="node" id="{$nodeIp_addressKey}"/>
			<key attr.name="os" attr.type="string" for="node" id="{$nodeOsKey}"/>
			<key attr.name="architecture" attr.type="string" for="node" id="{$nodeArchitectureKey}"/>
			<key attr.name="os_version" attr.type="string" for="node" id="{$nodeOs_versionKey}"/>
			<key attr.name="shared_directory" attr.type="string" for="node" id="{$nodeShared_directoryKey}"/>

			<key attr.name="projectUUID" attr.type="string" for="node" id="{$nodeProjectUUID}"/>
			<key attr.name="componentUUID" attr.type="string" for="node" id="{$nodeComponentUUID}"/>
			<key attr.name="implUUID" attr.type="string" for="node" id="{$nodeImplUUID}"/>
			<key attr.name="svntUUID" attr.type="string" for="node" id="{$nodeSvntUUID}"/>
			<key attr.name="componentInstanceUUID" attr.type="string" for="node" id="{$nodeComponentInstanceUUID}"/>

			<key attr.name="width" attr.type="double" for="node" id="{$nodeWidthKey}"/>
			<key attr.name="height" attr.type="double" for="node" id="{$nodeHeightKey}"/>
			
			<key attr.name="sortOrder" attr.type="double" for="node" id="{$nodeSortOrderKey}"/>
			<key attr.name="frequency" attr.type="double" for="node" id="{$nodeFrequencyKey}"/>
			<key attr.name="code" attr.type="string" for="node" id="{$nodeCodeKey}"/>
			
		<graph edgedefault="directed" id="G">

        <xsl:apply-templates />

	    <!-- Output end of graph -->
	    </graph>
		</graphml>
    </xsl:template>
    
	<xsl:template match="project">
		<xsl:variable name="ProjectId" select="concat('n',generate-id(.))" />
	    <node id="{$ProjectId}">
        <data key="{$nodeLabelKey}">
		<xsl:value-of select="./folder[@kind='RootFolder']/name" />
		</data>
		<data key="{$nodeXKey}">0</data>
		<data key="{$nodeYKey}">0</data>
		<data key="{$nodeKindKey}">
		<xsl:value-of select="'Model'" />
		</data>
		<data key="{$nodeMiddlewareKey}">
		<xsl:value-of select="'tao'" />
		</data>
		<!-- project name explicit in Model label	
		<data key="{$nodeProjectKey}">
		<xsl:value-of select="/project/folder[@kind='RootFolder']/name" />
		</data>
		-->
		<data key="{$nodeProjectUUID}">
		<xsl:value-of select="translate( ./@guid , '{}', '') " />
		</data>

		<graph edgedefault="directed" id="{$ProjectId}:">
		
		<xsl:apply-templates select="folder" />
		
		</graph>
		</node>

    </xsl:template>

	<!-- select folder that we are interested in -->
	<xsl:template match="folder">
		<xsl:variable name="InterfaceDefinitionsId" select="concat('IDL', generate-id(./folder[@kind='InterfaceDefinitions']) )" />
	    <node id="{$InterfaceDefinitionsId}">
        <data key="{$nodeLabelKey}">
		<xsl:value-of select="'InterfaceDefinitions'" />
		</data>
		<data key="{$nodeXKey}">15</data>
		<data key="{$nodeYKey}">30</data>
		<data key="{$nodeKindKey}">
		<xsl:value-of select="'InterfaceDefinitions'" />
		</data>
		<graph edgedefault="directed" id="{$InterfaceDefinitionsId}:">
			<xsl:for-each select="folder[@kind='InterfaceDefinitions']">	
				<xsl:call-template name="InterfaceDefinitions">
					<xsl:with-param name="mode" select="'IDL'" />
				</xsl:call-template>
			</xsl:for-each>
		</graph>
		</node>

		<xsl:variable name="numBehaviorNodes" select="count(./folder[@kind='InterfaceDefinitions']/descendant::*/regnode[@name='Behavior'])" /> 
		<xsl:if test="$numBehaviorNodes > 0" >
			<xsl:variable name="BehaviourDefinitionsId" select="concat('Impl', generate-id(./folder[@kind='InterfaceDefinitions']) )" />
			<node id="{$BehaviourDefinitionsId}">
			<data key="{$nodeLabelKey}">
			<xsl:value-of select="'BehaviourDefinitions'" />
			</data>
			<data key="{$nodeXKey}">1015</data>
			<data key="{$nodeYKey}">30</data>
			<data key="{$nodeKindKey}">
			<xsl:value-of select="'BehaviourDefinitions'" />
			</data>
			<graph edgedefault="directed" id="{$BehaviourDefinitionsId}:">
				<xsl:for-each select="folder[@kind='InterfaceDefinitions']">	
					<xsl:call-template name="InterfaceDefinitions">
						<xsl:with-param name="mode" select="'Impl'" />
					</xsl:call-template>
				</xsl:for-each>
			</graph>
			</node>
		</xsl:if>
		
		<xsl:variable name="numComponentImplementations" select="count(./folder[@kind='ComponentImplementations'])" /> 
		<xsl:if test="$numComponentImplementations > 0" >
			<!-- Cannot guarantee that there is a Deployment Plan specified for the model
            			<xsl:variable name="DeploymentDefinitionsId" select="generate-id(./folder[@kind='DeploymentPlans'])" /> -->
			<xsl:variable name="DeploymentDefinitionsId" select="concat('dp', generate-id(./folder[@kind='ComponentImplementations']))" />

			<node id="{$DeploymentDefinitionsId}">
			<data key="{$nodeLabelKey}">
			<xsl:value-of select="'DeploymentDefinitions'" />
			</data>
			<data key="{$nodeXKey}">15</data>
			<data key="{$nodeYKey}">1030</data>
			<data key="{$nodeKindKey}">
			<xsl:value-of select="'DeploymentDefinitions'" />
			</data>
			<graph edgedefault="directed" id="{$DeploymentDefinitionsId}:">
				<xsl:variable name="AssemblyDefinitionsId" select="generate-id(./folder[@kind='ComponentImplementations'])" />
				<node id="{$AssemblyDefinitionsId}">
				<data key="{$nodeLabelKey}">
				<xsl:value-of select="'AssemblyDefinitions'" />
				</data>
				<data key="{$nodeXKey}">15</data>
				<data key="{$nodeYKey}">30</data>
				<data key="{$nodeKindKey}">
				<xsl:value-of select="'AssemblyDefinitions'" />
				</data>
				<graph edgedefault="directed" id="{$AssemblyDefinitionsId}:">
					<xsl:apply-templates select="folder[@kind='ComponentImplementations']" />
				</graph>
				</node>
				<xsl:variable name="HardwareDefinitionsId" select="concat('hd', generate-id(./folder[@kind='ComponentImplementations']))" />
				<node id="{$HardwareDefinitionsId}">
				<data key="{$nodeLabelKey}">
				<xsl:value-of select="'HardwareDefinitions'" />
				</data>
				<data key="{$nodeXKey}">1015</data>
				<data key="{$nodeYKey}">30</data>
				<data key="{$nodeKindKey}">
				<xsl:value-of select="'HardwareDefinitions'" />
				</data>
				<graph edgedefault="directed" id="{$HardwareDefinitionsId}:">
				</graph>
				</node>
			</graph>
			</node>
		</xsl:if>
		
    </xsl:template>

	<xsl:template name="InterfaceDefinitions">
		<xsl:param name="mode" />
		
		<!-- Search for contained Files recursive create -->
		<xsl:for-each select="model[@kind='File']">	
			<xsl:choose>
				<xsl:when test="contains($mode, 'IDL')">
					<xsl:call-template name="ProcessIdlFile">
						<xsl:with-param name="mode" select="$mode" />
					</xsl:call-template>
				</xsl:when>
				<xsl:otherwise>
					<xsl:call-template name="ProcessImplFile">
						<xsl:with-param name="mode" select="$mode" />
					</xsl:call-template>
				</xsl:otherwise>
			</xsl:choose>
		</xsl:for-each>
		
		<!-- Now create links in IDL for Aggregates -->
		<xsl:if test="contains($mode, 'IDL')">
			<xsl:for-each select="./descendant::*/model[@kind='Event']">
				<!-- Find Aggregate for this Event, and if not there create aggregate -->
				<xsl:variable name="aggregateName">
					<xsl:choose>
						<xsl:when test="contains(./name, 'Event')">
							<xsl:value-of select="substring-before(./name,'Event')" />
						</xsl:when>
						<xsl:otherwise>
							<xsl:value-of select="./name" />
						</xsl:otherwise>
					</xsl:choose>
				</xsl:variable>
				<xsl:variable name="aggregate" select="/project/folder[@kind='RootFolder']/folder[@kind='InterfaceDefinitions']/descendant::*/model[@kind='Aggregate'][name=$aggregateName]" />
				<xsl:if test="not($aggregate)">
					<xsl:call-template name="AggregateEdge" />
				</xsl:if>
			</xsl:for-each>
			<xsl:for-each select="./descendant::*/model[@kind='Aggregate']">
					<xsl:call-template name="AggregateEdge" />
			</xsl:for-each>
			
			<xsl:for-each select="./descendant::*/model[@kind='Component']">
				<xsl:variable name="relPos" select="generate-id(.)" />
				<xsl:variable name="componentId" select="concat($mode,$relPos)" />
				
				<xsl:for-each select="./reference[@kind='OutEventPort']">
					<xsl:variable name="outEventPortId" select="concat(concat($componentId,'::n'),generate-id(.))" />
					<xsl:call-template name="EventPortEdge">
						<xsl:with-param name="EventPortId" select="$outEventPortId" />
					</xsl:call-template>
				</xsl:for-each>
				
				<xsl:for-each select="./reference[@kind='InEventPort']">
					<xsl:variable name="inEventPortId" select="concat(concat($componentId,'::n'),generate-id(.))" />
					<xsl:call-template name="EventPortEdge">
						<xsl:with-param name="EventPortId" select="$inEventPortId" />
					</xsl:call-template>
				</xsl:for-each>
				
			</xsl:for-each>
		</xsl:if>		
    </xsl:template>
	
	
	<xsl:template name="EventPortEdge" >
		<xsl:param name="EventPortId" />

		<!-- Insert Event information into this EventPort -->
		<xsl:variable name="InterfaceDefinitionsId" select="concat('IDL', generate-id(folder[@kind='InterfaceDefinitions'][0]) )" />
		<xsl:variable name="referred" select="./@referred" />
		<xsl:variable name="itemIsReferredModel" select="/project/folder[@kind='RootFolder']/folder[@kind='InterfaceDefinitions']/descendant::*/model[@id=$referred]" />
		<xsl:variable name="aggregateUsed">
			<xsl:choose>
				<xsl:when test="contains($itemIsReferredModel/name, 'Event')">
					<xsl:value-of select="substring-before($itemIsReferredModel/name,'Event')" />
				</xsl:when>
				<xsl:otherwise>
					<xsl:value-of select="$itemIsReferredModel/name" />
				</xsl:otherwise>
			</xsl:choose>
		</xsl:variable>
		<xsl:variable name="referredAggregate" select="/project/folder[@kind='RootFolder']/folder[@kind='InterfaceDefinitions']/descendant::*/model[@kind='Aggregate'][name=$aggregateUsed]" />
		<xsl:variable name="referredAggregateId" select="concat('n',generate-id($referredAggregate))" />
		<xsl:if test="$referredAggregate">
			<edge id="{$InterfaceDefinitionsId}::{$EventPortId}" source="{$EventPortId}" target="{$referredAggregateId}" >
				<data key="{$edgeDescriptionKey}">contains aggregate</data>
			</edge>
		</xsl:if>
		
    </xsl:template>
	
	
	<xsl:template name="AggregateEdge" >
		<xsl:variable name="InterfaceDefinitionsId" select="concat('IDL', generate-id(folder[@kind='InterfaceDefinitions'][0]) )" />
		<xsl:variable name="AggregateId" select="concat('n',generate-id(.))" />
		<xsl:for-each select="./reference[@kind='Member']">
			<xsl:variable name="memberId" select="position() - 1" />
			<xsl:variable name="memberIdLong" select="concat(concat($AggregateId, '::n'), $memberId)"/>
			<!-- find type may be predefined type or refer to an aggregate structure -->
			<xsl:variable name="referred" select="./@referred" />
			<xsl:variable name="referredAggregate" select="/project/folder[@kind='RootFolder']/folder[@kind='InterfaceDefinitions']/descendant::*/model[@id=$referred]" />
			<xsl:variable name="referredAggregateId" select="concat('n',generate-id($referredAggregate))" />
			<xsl:if test="$referredAggregate">
				<edge id="{$InterfaceDefinitionsId}::ea{$memberIdLong}" source="{$memberIdLong}" target="{$referredAggregateId}" >
					<data key="{$edgeDescriptionKey}">contains aggregate</data>
				</edge>
			</xsl:if>
		</xsl:for-each>
	</xsl:template>
	
	
	<xsl:template name="ProcessIdlFile" >
		<xsl:param name="mode" />
		
		<xsl:variable name="IDLfile"><xsl:value-of select="./name" /></xsl:variable>
		
		<xsl:variable name="FileId" select="concat('n',generate-id(.))" />
	    <node id="{$FileId}">
        <data key="{$nodeLabelKey}">
		<xsl:value-of select="./name" />
		</data>
		<data key="{$nodeXKey}">10</data>
		<data key="{$nodeYKey}">10</data>
		<data key="{$nodeKindKey}">
		<xsl:value-of select="'IDL'" />
		</data>
		<!-- file and project name explicit in Model and File node labels 
		<data key="{$nodeFileKey}">
		<xsl:value-of select="$IDLfile" />
		</data>
		<data key="{$nodeProjectKey}">
		<xsl:value-of select="/project/folder[@kind='RootFolder']/name" />
		</data>
		-->
		<graph edgedefault="directed" id="{$FileId}:">
		
		<xsl:call-template name="Package">
			<xsl:with-param name="mode" select="$mode" />
		</xsl:call-template>
		
		</graph>
		</node>
    </xsl:template>

	<xsl:template name="ProcessImplFile" >
		<xsl:param name="mode" />
		
		<xsl:call-template name="Package">
			<xsl:with-param name="mode" select="$mode" />
		</xsl:call-template>
		
    </xsl:template>
	
	<!-- Process Packages, flatten into one file (not supporting packages for the moment) -->	
	<xsl:template name="Package" >	
		<xsl:param name="mode" />

		<xsl:if test="contains($mode, 'IDL')">
			<!-- transform Events into Aggregates for simplicity of a common structure for both CORBA and DDS -->
			<!-- The IDL and code generation will automate the duplication of Event/Aggregate structure -->
			<xsl:apply-templates select="model[@kind='Event']" />
			
			<!-- transform any extra aggregate structures not already transformed -->
			<xsl:apply-templates select="model[@kind='Aggregate']" />
		</xsl:if>
		
		<!-- select kind of models that we are interested in -->
		<xsl:apply-templates select="model[@kind='Component']">
			<xsl:with-param name="mode" select="$mode" />
   	    </xsl:apply-templates>
		
		<!-- Search for contained Packages recursive create -->
		<xsl:for-each select="model[@kind='Package']">
			<!-- Flatten all Packages into the File, if legacy model relies on packages/namespace modifications may be required execute the system -->
			<xsl:variable name="packageName" select="./name" />
			
			<xsl:call-template name="Package">
				<xsl:with-param name="mode" select="$mode" />
			</xsl:call-template>		
		</xsl:for-each>
	</xsl:template>

	<!-- Aggregate and Member nodes -->
    <xsl:template match="model[@kind='Aggregate']">
		<xsl:call-template name="Aggregates" />
    </xsl:template>

	<!-- Event to Aggregate and Member nodes -->
    <xsl:template match="model[@kind='Event']">
		
		<!-- Find Aggregate for this Event, and if not there create aggregate -->
		<xsl:variable name="aggregateName">
			<xsl:choose>
				<xsl:when test="contains(./name, 'Event')">
					<xsl:value-of select="substring-before(./name,'Event')" />
				</xsl:when>
				<xsl:otherwise>
					<xsl:value-of select="./name" />
				</xsl:otherwise>
			</xsl:choose>
		</xsl:variable>
		<xsl:variable name="aggregate" select="/project/folder[@kind='RootFolder']/folder[@kind='InterfaceDefinitions']/descendant::*/model[@kind='Aggregate'][name=$aggregateName]" />
		<xsl:if test="not($aggregate)">
			<xsl:call-template name="Aggregates" />
		</xsl:if>
    </xsl:template>
	
	<!-- Aggregate and Member nodes -->
    <xsl:template name="Aggregates">
		
	    <!-- Output Event/Aggregate node -->
		<xsl:variable name="AggregateId" select="concat('n',generate-id(.))" />
	    <node id="{$AggregateId}">
        <data key="{$nodeLabelKey}">
		<xsl:value-of select="./name" />
		</data>
		<xsl:call-template name="nodeXYKeyData">
			<xsl:with-param name="node" select="./regnode[@name='PartRegs']/regnode[@name='InterfaceDefinition']/regnode[@name='Position']" />
		</xsl:call-template>
		<data key="{$nodeKindKey}">
		<xsl:value-of select="'Aggregate'" />
		</data>
		<graph edgedefault="directed" id="{$AggregateId}:">
		
		<!-- Output each simple Member of the Event  --> 
			<xsl:call-template name="AggregateMembers">
				<xsl:with-param name="event" select="."/>
				<xsl:with-param name="parentId" select="$AggregateId"/>
				<xsl:with-param name="memberType" select="'Member'"/>
				<xsl:with-param name="W" select="''"/>
			</xsl:call-template>
		</graph>
        </node>
		
    </xsl:template>
		
	<!-- Component and Event Port nodes -->
    <xsl:template match="model[@kind='Component']">
		<xsl:param name="mode" />
		
		<!-- Output Component node -->
		<xsl:variable name="relPos" select="generate-id(.)" />
		<xsl:variable name="componentId" select="concat($mode,$relPos)" />
		<xsl:variable name="componentIDL" select="concat('IDL',$relPos)" />
		<xsl:variable name="componentEdge" select="concat('e',$relPos)" />
		<xsl:variable name="componentModel" select="." />
		
	    <node id="{$componentId}">
        <data key="{$nodeLabelKey}">
		<xsl:value-of select="./name" />
		</data>
		<xsl:call-template name="nodeXYKeyData">
			<xsl:with-param name="node" select="./regnode[@name='PartRegs']/regnode[@name='InterfaceDefinition']/regnode[@name='Position']" />
		</xsl:call-template>
		<data key="{$nodeKindKey}">
			<xsl:choose>
				<xsl:when test="contains($mode, 'Impl')">
					<xsl:value-of select="'ComponentImpl'" />	
				</xsl:when>
				<xsl:otherwise>
					<xsl:value-of select="./@kind" />	
				</xsl:otherwise>
			</xsl:choose>
		</data>
		<xsl:if test="contains($mode, 'IDL')">
			<xsl:variable name="FindId" select="./@id" />
			<!-- link back to MonolithicImplementation through ComponentRef in ComponentImplementationContainer --> 
			<xsl:variable name="ImplContainer" select="/project/folder[@kind='RootFolder']/folder[@kind='ComponentImplementations']/descendant::*/reference[@kind='ComponentRef'][@referred=$FindId]/.."/>
			<data key="{$nodeComponentUUID}">
			<xsl:value-of select="$ImplContainer/atom[@kind='MonolithicImplementation']/attribute[@kind='UUID']/value"/>
			</data>
			<!-- link to Impl and Svnt artifacts through references, don't rely on names -->
			<xsl:variable name="ImplArtifactId" select="$ImplContainer/reference[@kind='ComponentImplementationArtifact']/@referred" />
			<xsl:variable name="SvntArtifactId" select="$ImplContainer/reference[@kind='ComponentServantArtifact']/@referred" />
			<data key="{$nodeImplUUID}">
			<xsl:value-of select="/project/folder[@kind='RootFolder']/folder[@kind='ImplementationArtifacts']/descendant::*/atom[@id=$ImplArtifactId]/attribute[@kind='UUID']/value" />
			</data>
			<data key="{$nodeSvntUUID}">
			<xsl:value-of select="/project/folder[@kind='RootFolder']/folder[@kind='ImplementationArtifacts']/descendant::*/atom[@id=$SvntArtifactId]/attribute[@kind='UUID']/value" />
			</data>
		</xsl:if>
		<!-- include type if writing the behaviour impl node -->
		<xsl:if test="contains($mode, 'Impl')">
			<data key="{$nodeTypeKey}">
			<xsl:value-of select="./name" />
			</data>
		</xsl:if>
			
		<!-- Assume that a component will always be a graph containing event ports and other nodes -->
		<graph edgedefault="directed" id="{$componentId}:">	
		
		<!-- Output each simple Attribute of the Component --> 
		<xsl:variable name="countAttributes" select="count(./model[@kind='Attribute'])" />
		<xsl:for-each select="./model[@kind='Attribute']">
			<xsl:variable name="a" select="position() - 1" />
			<xsl:variable name="attributeId" select="concat(concat($componentId,'::n'),$a)" />
			<node id="{$attributeId}">
			<data key="{$nodeLabelKey}">
			<xsl:value-of select="./name" />
			</data>
			<xsl:choose>
				<xsl:when test="contains($mode, 'IDL')">
					<xsl:call-template name="nodeXYKeyData">
						<xsl:with-param name="node" select="./regnode[@name='PartRegs']/regnode[@name='InterfaceDefinition']/regnode[@name='Position']" />
					</xsl:call-template>
				</xsl:when>
				<xsl:otherwise>
					<xsl:call-template name="nodeXYKeyData">
						<xsl:with-param name="node" select="./regnode[@name='PartRegs']/regnode[@name='Behavior']/regnode[@name='Position']" />
					</xsl:call-template>
				</xsl:otherwise>
			</xsl:choose>
			<data key="{$nodeKindKey}">
			<xsl:choose>
				<xsl:when test="contains($mode, 'Impl')">
					<xsl:value-of select="'AttributeImpl'" />	
				</xsl:when>
				<xsl:otherwise>
					<xsl:value-of select="./@kind" />	
				</xsl:otherwise>
			</xsl:choose>
			</data>
			<data key="{$nodeTypeKey}">
			<xsl:variable name="referred" select="./reference/@referred" />
			<!-- should be in main PredefinedTypes folder, but could be linked to PredefinedTypes in a Worker Library -->
			<xsl:value-of select="/descendant::*/folder[@kind='PredefinedTypes']/atom[@id=$referred]/name" />
			</data>
			</node>
		</xsl:for-each>

		<!-- Output each simple Variable of the Component including initial value --> 
		<xsl:variable name="countVariables" select="count(./reference[@kind='Variable'])" />
		<xsl:if test="contains($mode, 'Impl')">
			<xsl:for-each select="./reference[@kind='Variable']">
				<xsl:variable name="v" select="$countAttributes + position() - 1" />
				<xsl:variable name="variableId" select="concat(concat($componentId,'::n'),$v)" />
				<node id="{$variableId}">
				<data key="{$nodeLabelKey}">
				<xsl:value-of select="./name" />
				</data>
				<xsl:call-template name="nodeXYKeyData">
					<xsl:with-param name="node" select="./regnode[@name='PartRegs']/regnode[@name='Behavior']/regnode[@name='Position']" />
				</xsl:call-template>
				<data key="{$nodeKindKey}">
				<xsl:value-of select="./@kind" />
				</data>
				<data key="{$nodeTypeKey}">
				<xsl:variable name="Vreferred" select="./@referred" />
				<!-- should be in main PredefinedTypes folder, but could be linked to PredefinedTypes in a Worker Library -->
				<xsl:value-of select="/descendant::*/folder[@kind='PredefinedTypes']/atom[@id=$Vreferred]/name" />
				</data>
				<data key="{$nodeValueKey}">
					<xsl:value-of select="./attribute[@kind='InitialValue']/value" />
				</data>
				</node>
			</xsl:for-each>
		</xsl:if>
		
		<!-- Output each OutEventPort of the Component --> 
		<xsl:variable name="countOutEventPorts" select="count(./reference[@kind='OutEventPort'])" />
		<xsl:if test="contains($mode, 'IDL')">
			<xsl:for-each select="./reference[@kind='OutEventPort']">
				<xsl:variable name="outEventPortId" select="concat(concat($componentId,'::n'),generate-id(.))" />

				<node id="{$outEventPortId}">
				<data key="{$nodeLabelKey}">
				<xsl:value-of select="./name" />
				</data>
				<xsl:call-template name="nodeXYKeyData">
					<xsl:with-param name="node" select="./regnode[@name='PartRegs']/regnode[@name='InterfaceDefinition']/regnode[@name='Position']" />
				</xsl:call-template>
				<data key="{$nodeKindKey}">
				<xsl:value-of select="./@kind" />
				</data>
				</node>
			</xsl:for-each>
		</xsl:if>
			
		<!-- Output each InEventPort of the Component --> 
		<xsl:variable name="countInEventPorts" select="count(./reference[@kind='InEventPort'])" />
		<xsl:for-each select="./reference[@kind='InEventPort']">
			<!-- <xsl:variable name="i" select="$countAttributes + $countVariables + $countOutEventPorts + position() - 1" /> -->
			<xsl:variable name="inEventPortIDL" select="concat(concat($componentId,'::n'), generate-id(.))" />
			<xsl:variable name="inEventPortEdgeId" select="concat(concat($componentId,'::e'), generate-id(.))" />
			<xsl:if test="contains($mode, 'IDL')">
				<node id="{$inEventPortIDL}">
				<data key="{$nodeLabelKey}">
				<xsl:value-of select="./name" />
				</data>
				<xsl:call-template name="nodeXYKeyData">
					<xsl:with-param name="node" select="./regnode[@name='PartRegs']/regnode[@name='InterfaceDefinition']/regnode[@name='Position']" />
				</xsl:call-template>
				<data key="{$nodeKindKey}">
				<xsl:value-of select="./@kind" />
				</data>
				<xsl:variable name="InEreferred" select="./@referred" />
				</node>
			</xsl:if>
			
			<xsl:if test="contains($mode, 'Impl')">
				<!-- Now connect to behaviour if InEventPort has a InputAction -->
				<xsl:variable name="candidateBehaviorNodes" select="../*[self::model|self::atom|self::reference]" />
				
				<xsl:variable name="connections" select="../connection"/>
				<xsl:variable name="InEventPortNode" select="." />
				<xsl:variable name="startNode" select="$candidateBehaviorNodes[@id=$connections/connpoint[@role='src'][@target=$InEventPortNode/@id]/../connpoint[@role='dst']/@target]" />
		
				<xsl:if test="$startNode[@kind='InputAction'] != ''">
					<xsl:variable name="nextNodeId" select="concat(concat(concat($componentId,'::n'), generate-id(.)), 01)" />
					<xsl:call-template name="InEventNode">
						<xsl:with-param name="nextNodeId" select="$nextNodeId"/>
						<xsl:with-param name="E" select="../connection"/>
						<xsl:with-param name="W" select="."/>
						<xsl:with-param name="kind" select="'InEventPortImpl'"/>
					</xsl:call-template>

					<xsl:call-template name="dfsEventReceived">
						<xsl:with-param name="S" select="$startNode"/>
						<xsl:with-param name="T" select="''"/>
						<xsl:with-param name="I" select="$nextNodeId"/>  <!-- source node for edge -->
						<xsl:with-param name="E" select="$connections"/>
						<xsl:with-param name="N" select="$candidateBehaviorNodes"/>
						<xsl:with-param name="W" select="$InEventPortNode"/>
						<xsl:with-param name="O" select="1"/>
					</xsl:call-template>
					
					<!-- create edge for implementation of InEventPort --> 
					<xsl:variable name="IDLtarget" select="concat('IDL',substring-after($inEventPortIDL,'Impl'))" />
					<edge id="{$inEventPortEdgeId}" source="{$nextNodeId}" target="{$IDLtarget}" >
						<data key="{$edgeDescriptionKey}">implementation of</data>
					</edge>  
				</xsl:if>
			</xsl:if>

		</xsl:for-each>

		<xsl:if test="contains($mode, 'Impl')">

			<!-- Output each PeriodicEvent of the Component --> 
			<xsl:variable name="countPeriodicEvent" select="count(./atom[@kind='PeriodicEvent'])" />
			<!-- collect details on periodic event actions for behaviour processing -->
			<xsl:for-each select="./atom[@kind='PeriodicEvent']">
				<xsl:variable name="p" select="$countAttributes + $countVariables + $countOutEventPorts + $countInEventPorts + position() - 1" />
				<xsl:variable name="periodicEventId" select="concat(concat($componentId,'::n'),$p)" />
				<node id="{$periodicEventId}">
				<data key="{$nodeLabelKey}">
				<xsl:value-of select="./name" />
				</data>
				<xsl:call-template name="nodeXYKeyData">
					<xsl:with-param name="node" select="./regnode[@name='PartRegs']/regnode[@name='Behavior']/regnode[@name='Position']" />
				</xsl:call-template>
				<data key="{$nodeKindKey}">
				<xsl:value-of select="./@kind" />
				</data>
				<data key="{$nodeTypeKey}">
					<xsl:value-of select="./attribute[@kind='Distribution']/value" />
				</data>
				<data key="{$nodeFrequencyKey}">
					<xsl:value-of select="./attribute[@kind='Hertz']/value" />
				</data>
				</node>
				
				<!-- Now connect to behaviour if PeriodicEvent has a InputAction -->
				<xsl:variable name="candidBehaviorNodes" select="../*[self::model|self::atom|self::reference]" />

				<xsl:variable name="connects" select="../connection"/>
				<xsl:variable name="PEventPortNode" select="." />
				<xsl:variable name="PstartNode" select="$candidBehaviorNodes[@id=$connects/connpoint[@role='src'][@target=$PEventPortNode/@id]/../connpoint[@role='dst']/@target]" />
				
				<xsl:if test="$PstartNode[@kind='InputAction'] != ''">
					<xsl:call-template name="dfsEventReceived">
						<xsl:with-param name="S" select="$PstartNode"/>
						<xsl:with-param name="T" select="''"/>
						<xsl:with-param name="I" select="$periodicEventId"/>  <!-- source node for edge -->
						<xsl:with-param name="E" select="$connects"/>
						<xsl:with-param name="N" select="$candidBehaviorNodes"/>
						<xsl:with-param name="W" select="$PEventPortNode"/>
						<xsl:with-param name="O" select="1"/>
					</xsl:call-template>
				</xsl:if>

			</xsl:for-each>
			
			<!-- Output a workload node to contain activate, passivate and callback functions of the Component --> 
			<xsl:variable name="EnvironmentNode" select="./atom[@kind='Environment']" />
			<xsl:if test="$EnvironmentNode">
				
				<xsl:variable name="e" select="$countAttributes + $countVariables + $countOutEventPorts + $countInEventPorts + $countPeriodicEvent + position() - 1" />
				<xsl:variable name="EnvironmentId" select="concat(concat($componentId,'::n'),$e)" />
				<node id="{$EnvironmentId}">
				<data key="{$nodeLabelKey}">
				<xsl:value-of select="$EnvironmentNode/name" />
				</data>
<!--				<xsl:call-template name="nodeXYKeyData">
					<xsl:with-param name="node" select="$EnvironmentNode/regnode[@name='PartRegs']/regnode[@name='Behavior']/regnode[@name='Position']" />
				</xsl:call-template>
-->
				<xsl:variable name="ExValue" select="substring-before($EnvironmentNode/regnode[@name='PartRegs']/regnode[@name='Behavior']/regnode[@name='Position']/value, ',')" />
				<data key="{$nodeXKey}">
				<xsl:value-of select="$ExValue" />
				</data>
				<xsl:variable name="EyValue" select="substring-after($EnvironmentNode/regnode[@name='PartRegs']/regnode[@name='Behavior']/regnode[@name='Position']/value, ',')" />
				<data key="{$nodeYKey}">
				<xsl:value-of select="$EyValue" />
				</data>
				<data key="{$nodeKindKey}">
				<xsl:value-of select="'Workload'" />
				</data>
				
				<graph edgedefault="directed" id="{$EnvironmentId}:">	
				
				<!-- Now connect to behaviour if Environment has a MultiInputAction -->
				<xsl:variable name="EbehaviorNodes" select="./*[self::model|self::atom|self::reference]" />
				<xsl:variable name="Econnects" select="./connection"/>
				<xsl:variable name="EstartNode" select="$EbehaviorNodes[@id=$Econnects/connpoint[@role='src'][@target=$EnvironmentNode/@id]/../connpoint[@role='dst']/@target]" />
				
				<xsl:for-each select="$EstartNode[@kind='MultiInputAction']">
					<xsl:call-template name="dfsProcesses">
						<xsl:with-param name="S" select="."/>
						<xsl:with-param name="T" select="./name"/>
						<xsl:with-param name="I" select="$EnvironmentId"/>  
						<xsl:with-param name="E" select="$Econnects"/>
						<xsl:with-param name="N" select="$EbehaviorNodes"/>
						<xsl:with-param name="W" select="."/>
						<xsl:with-param name="x" select="$ExValue"/>
						<xsl:with-param name="y" select="$EyValue"/>
					</xsl:call-template>
				</xsl:for-each> 
				
				</graph>
				</node>
			</xsl:if>
			
		</xsl:if>
		
		</graph>
		</node>

		<!-- create edge for implementation of ComponentImpl to Component -->
		<xsl:if test="contains($mode, 'Impl')">
			<edge id="{$componentEdge}" source="{$componentId}" target="{$componentIDL}" >
				<data key="{$edgeDescriptionKey}">implementation of</data>
			</edge>  
		</xsl:if>
	</xsl:template>
	
	<!-- Recursive Search for activate and passivate behaviours starting at MultiInputAction, assumes no branching! -->
	<xsl:template name="dfsProcesses">
		<xsl:param name="S"/> <!-- start node = end node -->
		<xsl:param name="T"/> <!-- type of process activate passivate or user defined -->
		<xsl:param name="I"/> <!-- source for edge to next node -->
		<xsl:param name="E"/> <!-- edges -->
		<xsl:param name="N"/> <!-- candidate nodes -->
		<xsl:param name="W"/> <!-- node[s] in progress -->
		<xsl:param name="x"/> <!-- x -->
		<xsl:param name="y"/> <!-- y -->
		
		<!-- follow connection to next node -->
		<xsl:variable name="next" select="$N[@id=$E/connpoint[@role='src'][@target=$W/@id]/../connpoint[@role='dst']/@target]"/>
			
		<!-- next node in this branch -->
		<xsl:if test="($next[1] != $S)"> 
			
			<xsl:variable name="nextNodeId" select="concat(concat(substring-before($I,':'),'::n'),generate-id($next[1]))" />				

			<!-- output the Workload Process nodes as found -->
			<xsl:variable name="edge" select="$E/connpoint[@role='src'][@target=$W/@id]/.." />
			<xsl:variable name="postcondition" select="$edge[@kind='Effect' or @kind='InputEffect']/attribute[@kind='Postcondition']/value" />
			
			<!-- MultiInputAction InputEffect -->		
			<xsl:if test="($W/@kind = 'MultiInputAction') and ($postcondition != '')">
				<node id="{concat($nextNodeId,'ie')}">
				<data key="{$nodeLabelKey}"> <xsl:value-of select="$edge/@kind" /> </data>
				<data key="{$nodeXKey}"> <xsl:value-of select="15" /> </data>
				<data key="{$nodeYKey}"> <xsl:value-of select="60" /> </data>
				<data key="{$nodeKindKey}"> <xsl:value-of select="'Process'" /> </data>	
				<xsl:choose>
					<xsl:when test="$T='activate'">
						<data key="{$nodeActionOnKey}"> <xsl:value-of select="'Activate'" /> </data>
						</xsl:when>
					<xsl:when test="$T='passivate'">
						<data key="{$nodeActionOnKey}"> <xsl:value-of select="'Passivate'" /> </data>
					</xsl:when>
					<xsl:otherwise>
						<data key="{$nodeActionOnKey}"> <xsl:value-of select="'Callback'" /> </data>
					</xsl:otherwise>
				</xsl:choose>
				<data key="{$nodeCodeKey}">
					<xsl:value-of select="$postcondition" />
				</data> 
				</node>
			</xsl:if>
			
			<!-- Insert Workload information into this Node -->
			<xsl:if test="($next[1]/@kind != 'State') and ($next[1]/@kind != 'MultiInputAction') ">
				<!-- Workload ActionType -->
				<node id="{$nextNodeId}">
				<data key="{$nodeLabelKey}"> <xsl:value-of select="$next[1]/name" /> </data>
				<xsl:variable name="ExValue" select="substring-before($next[1]/regnode[@name='PartRegs']/regnode[@name='Behavior']/regnode[@name='Position']/value, ',')" />
				<data key="{$nodeXKey}"> <xsl:value-of select="$ExValue - $x" /> </data>
				<xsl:variable name="EyValue" select="substring-after($next[1]/regnode[@name='PartRegs']/regnode[@name='Behavior']/regnode[@name='Position']/value, ',')" />
				<data key="{$nodeYKey}"> <xsl:value-of select="$EyValue - $y" /> </data>

				<xsl:variable name="referred" select="$next[1]/reference[@kind='ActionType']/@referred" />
				<xsl:variable name="itemIsWorkerModel" select="/descendant::*/model[@kind='Worker']/model[@id=$referred]/.." />
				<data key="{$nodeKindKey}"> <xsl:value-of select="'Process'" /> </data>	
				<data key="{$nodeFolderKey}"> <xsl:value-of select="$itemIsWorkerModel/../attribute[@kind='Location']/value" /> </data>	
				<data key="{$nodeFileKey}"> <xsl:value-of select="$itemIsWorkerModel/../name" /> </data>		
				<data key="{$nodeWorkerKey}"> <xsl:value-of select="$itemIsWorkerModel/name" /> </data>
				<data key="{$nodeOperationKey}"> <xsl:value-of select="$next[1]/reference[@kind='ActionType']/name" /> </data>
				<xsl:choose>
					<xsl:when test="$T='activate'">
						<data key="{$nodeActionOnKey}"> <xsl:value-of select="'Activate'" /> </data>
						</xsl:when>
					<xsl:when test="$T='passivate'">
						<data key="{$nodeActionOnKey}"> <xsl:value-of select="'Passivate'" /> </data>
					</xsl:when>
					<xsl:otherwise>
						<data key="{$nodeActionOnKey}"> <xsl:value-of select="'Callback'" /> </data>
					</xsl:otherwise>
				</xsl:choose>
				<data key="{$nodeParametersKey}">
				<xsl:variable name="sortedParameters">
					<xsl:for-each select="$next[1]/reference[@kind='SimpleProperty']">
						<!-- sort by position from left to right -->
						<xsl:sort select="substring-before(./regnode[@name='PartRegs']/regnode[@name='Behavior']/regnode[@name='Position']/value, ',')" data-type="number" order="ascending" /> 
						<xsl:value-of select="concat(',', ./attribute[@kind='Value']/value)" />
					</xsl:for-each>
				</xsl:variable>
				<xsl:value-of select="substring-after($sortedParameters, ',')" />
				</data>
				</node>
			</xsl:if>
			
			<!-- Workload Postcondition/Postprocess -->		
			<xsl:variable name="Eedge" select="$E/connpoint[@role='src'][@target=$next[1]/@id]/.." />
			<xsl:variable name="Epostcondition" select="$Eedge[@kind='Effect' or @kind='InputEffect']/attribute[@kind='Postcondition']/value" />
			<xsl:if test="($next[1]/@kind != 'State') and ($Epostcondition != '')">
				<node id="{concat($nextNodeId,'e')}">
				<data key="{$nodeLabelKey}"> <xsl:value-of select="$Eedge/@kind" /> </data>
				<xsl:variable name="xValue" select="substring-before($next[1]/regnode[@name='PartRegs']/regnode[@name='Behavior']/regnode[@name='Position']/value, ',')" />
				<xsl:variable name="yValue" select="substring-after($next[1]/regnode[@name='PartRegs']/regnode[@name='Behavior']/regnode[@name='Position']/value, ',')" />
				<data key="{$nodeXKey}"> <xsl:value-of select="$xValue - $x + 30" /> </data>
				<data key="{$nodeYKey}"> <xsl:value-of select="$yValue - $y + 20" /> </data>
				<data key="{$nodeKindKey}"> <xsl:value-of select="'Process'" /> </data>	
				<xsl:choose>
					<xsl:when test="$T='activate'">
						<data key="{$nodeActionOnKey}"> <xsl:value-of select="'Activate'" /> </data>
						</xsl:when>
					<xsl:when test="$T='passivate'">
						<data key="{$nodeActionOnKey}"> <xsl:value-of select="'Passivate'" /> </data>
					</xsl:when>
					<xsl:otherwise>
						<data key="{$nodeActionOnKey}"> <xsl:value-of select="'Callback'" /> </data>
					</xsl:otherwise>
				</xsl:choose>
				<data key="{$nodeCodeKey}">
					<xsl:value-of select="$Epostcondition" />
				</data> 
				</node>
			</xsl:if>
			
			<!-- only increment if new node and edge are created -->
			<xsl:variable name="nextI">
				<xsl:choose>
					<!-- If the InputAction has no effect, do not produce a node in the graph -->
					<xsl:when test="$next[1][@kind='InputAction']"> 
						<xsl:variable name="testEdge" select="$E/connpoint[@role='src'][@target=$next[1]/@id]/.." />
						<xsl:variable name="testInputEffect" select="$testEdge[@kind='InputEffect']/attribute[@kind='Postcondition']/value" />
						<xsl:choose>
						<xsl:when test="$testInputEffect = ''">	<xsl:value-of select="$I"/> </xsl:when>
						<xsl:otherwise> 						<xsl:value-of select="$nextNodeId"/>	</xsl:otherwise>
						</xsl:choose>
					</xsl:when>
					<!-- Remove all State kind nodes from graph -->
					<xsl:when test="$next[1][@kind='State']">	<xsl:value-of select="$I"/> </xsl:when>
					<xsl:otherwise> 							<xsl:value-of select="$nextNodeId"/>	</xsl:otherwise>
				</xsl:choose>
			</xsl:variable>
		
			<!-- recursive Depth First Search until start/end node detected -->
			<xsl:call-template name="dfsProcesses">
				<xsl:with-param name="S" select="$S"/>
				<xsl:with-param name="T" select="./name"/>
				<xsl:with-param name="I" select="$nextI"/>  <!-- source node for edge -->
				<xsl:with-param name="E" select="$E"/>
				<xsl:with-param name="N" select="$N"/>
				<xsl:with-param name="W" select="$next[1]"/>
				<xsl:with-param name="x" select="$x"/>
				<xsl:with-param name="y" select="$y"/>
			</xsl:call-template>
			
		</xsl:if>
	</xsl:template>
	
	<!-- Recursive Search of behaviour state machine starting at InputAction -->
	<xsl:template name="dfsEventReceived">
		<xsl:param name="S"/> <!-- start node = end node -->
		<xsl:param name="T"/> <!-- terminal node -->
		<xsl:param name="I"/> <!-- source for edge to next node -->
		<xsl:param name="E"/> <!-- edges -->
		<xsl:param name="N"/> <!-- candidate nodes -->
		<xsl:param name="W"/> <!-- node[s] in progress -->
		<xsl:param name="O"/> <!-- order of next node to process -->
		
		<!-- follow connection to next node -->
		<xsl:variable name="next" select="$N[@id=$E/connpoint[@role='src'][@target=$W/@id]/../connpoint[@role='dst']/@target]"/>
			
		<!-- next node in this branch -->
		<xsl:if test="($W/@kind = 'InEventPort') or ($W/@kind = 'PeriodicEvent') or (($next[1] != $S) and ($next[1]/@kind != 'Terminal')) "> 
			
			<xsl:choose>			
			<xsl:when test="$next[1][@kind='BranchState']">	
				
				<xsl:variable name="nextNodeId" select="concat(concat(substring-before($I,':'),'::n'),generate-id($next[1]))" />			
				<!-- output the edge and node as it is found including the Terminal -->		
				<xsl:call-template name="behaviourNode">
					<xsl:with-param name="prevNodeId" select="$I"/>
					<xsl:with-param name="next" select="$next[1]"/>		
					<xsl:with-param name="E" select="$E"/>
					<xsl:with-param name="N" select="$N"/>
					<xsl:with-param name="prevTerminal" select="$T"/>
				</xsl:call-template>
			
				<!-- find the terminal for this BranchState, then return all branches to this found terminal node -->
				<xsl:variable name="findTerminal">
				    <xsl:call-template name="dfsFindTerminal">
						<xsl:with-param name="S" select="$S"/>
						<xsl:with-param name="E" select="$E"/>
						<xsl:with-param name="N" select="$N"/>
						<xsl:with-param name="W" select="$next[1]"/>
						<xsl:with-param name="L" select="0"/>
					</xsl:call-template>
				</xsl:variable>
				<xsl:variable name="nextTerminal" select="$N[@id=substring-before($findTerminal,',')]"/>
				<xsl:variable name="terminalNodeId" select="concat(concat(substring-before($I,':'),'::n'),generate-id($nextTerminal))" />
				
				<!-- output the edge and node of the terminal found -->
				<xsl:if test="$nextTerminal[@kind='Terminal']" >
					<xsl:call-template name="behaviourNode">
						<xsl:with-param name="prevNodeId" select="$nextNodeId"/>
						<xsl:with-param name="next" select="$nextTerminal"/>		
						<xsl:with-param name="E" select="$E"/>
						<xsl:with-param name="N" select="$N"/>
						<xsl:with-param name="prevTerminal" select="''"/>
					</xsl:call-template>
				</xsl:if>
			
				<!-- find all outgoing connections from this BranchState -->
				<xsl:for-each select="$N[@id=$E/connpoint[@role='src'][@target=$next[1]/@id]/../connpoint[@role='dst']/@target]">
					
					<xsl:variable name="nextCondition" select="concat(concat($nextNodeId,'::n'), string(position() - 1))"/>
					
					<!-- recursive Depth First Search until branch terminate detected -->
					<xsl:call-template name="dfsEventReceived">
						<xsl:with-param name="S" select="$S"/>
						<xsl:with-param name="T" select="$terminalNodeId"/>
						<xsl:with-param name="I" select="$nextCondition"/>  <!-- start from condition node -->
						<xsl:with-param name="E" select="$E"/>
						<xsl:with-param name="N" select="$N"/>
						<xsl:with-param name="W" select="$next[1]"/>
						<xsl:with-param name="O" select="position()"/>
					</xsl:call-template>
					
				</xsl:for-each>
				
				<!-- continue with nodes after terminal -->
				<xsl:call-template name="dfsEventReceived">
					<xsl:with-param name="S" select="$S"/>
					<xsl:with-param name="T" select="$T"/>
					<xsl:with-param name="I" select="$terminalNodeId"/>  <!-- source node for edge -->
					<xsl:with-param name="E" select="$E"/>
					<xsl:with-param name="N" select="$N"/>
					<xsl:with-param name="W" select="$nextTerminal"/>
					<xsl:with-param name="O" select="$O"/>
				</xsl:call-template>
				
			</xsl:when>
			<xsl:when test="($W/@kind = 'BranchState')">	
			
				<xsl:variable name="nextNodeId" select="concat(concat(substring-before($I,':'),'::n'),generate-id($next[$O]))" />			
				<!-- output the edge and node as it is found including the Terminal -->		
				<xsl:call-template name="behaviourNode">
					<xsl:with-param name="prevNodeId" select="$I"/>
					<xsl:with-param name="next" select="$next[$O]"/>		
					<xsl:with-param name="E" select="$E"/>
					<xsl:with-param name="N" select="$N"/>
					<xsl:with-param name="prevTerminal" select="$T"/>
				</xsl:call-template>
				
				<!-- only increment if new node and edge are created -->
				<xsl:variable name="nextI">
					<xsl:choose>
						<xsl:when test="$next[$O][not(@kind='State')]">	<xsl:value-of select="$nextNodeId"/> </xsl:when>
						<xsl:otherwise> 								<xsl:value-of select="$I"/>	</xsl:otherwise>
					</xsl:choose>
				</xsl:variable>
				
				<!-- recursive Depth First Search until branch terminate detected -->
				<xsl:call-template name="dfsEventReceived">
					<xsl:with-param name="S" select="$S"/>
					<xsl:with-param name="T" select="$T"/>
					<xsl:with-param name="I" select="$nextI"/>   <!-- source node for edge -->
					<xsl:with-param name="E" select="$E"/>
					<xsl:with-param name="N" select="$N"/>
					<xsl:with-param name="W" select="$next[$O]"/>
					<xsl:with-param name="O" select="$O"/>
				</xsl:call-template>
				
			</xsl:when>
			<xsl:otherwise> 
				
				<xsl:variable name="nextNodeId" select="concat(concat(substring-before($I,':'),'::n'),generate-id($next[1]))" />				
				<!-- output the edge and node as it is found -->
				<xsl:call-template name="behaviourNode">
					<xsl:with-param name="prevNodeId" select="$I"/>
					<xsl:with-param name="next" select="$next[1]"/>		
					<xsl:with-param name="E" select="$E"/>
					<xsl:with-param name="N" select="$N"/>
					<xsl:with-param name="prevTerminal" select="$T"/>
				</xsl:call-template>
				
				<!-- only increment if new node and edge are created -->
				<xsl:variable name="nextI">
					<xsl:choose>
						<!-- If the InputAction has no effect, do not produce a node in the graph -->
						<xsl:when test="$next[1][@kind='InputAction']"> 
							<xsl:variable name="testEdge" select="$E/connpoint[@role='src'][@target=$next[1]/@id]/.." />
							<xsl:variable name="testInputEffect" select="$testEdge[@kind='InputEffect']/attribute[@kind='Postcondition']/value" />
							<xsl:choose>
							<xsl:when test="$testInputEffect = ''">	<xsl:value-of select="$I"/> </xsl:when>
							<xsl:otherwise> 						<xsl:value-of select="$nextNodeId"/>	</xsl:otherwise>
							</xsl:choose>
						</xsl:when>
						<!-- Remove all State kind nodes from graph -->
						<xsl:when test="$next[1][@kind='State']">	<xsl:value-of select="$I"/> </xsl:when>
						<xsl:otherwise> 							<xsl:value-of select="$nextNodeId"/>	</xsl:otherwise>
					</xsl:choose>
				</xsl:variable>
			
				<!-- recursive Depth First Search until start/end node detected -->
				<xsl:call-template name="dfsEventReceived">
					<xsl:with-param name="S" select="$S"/>
					<xsl:with-param name="T" select="$T"/>
					<xsl:with-param name="I" select="$nextI"/>  <!-- source node for edge -->
					<xsl:with-param name="E" select="$E"/>
					<xsl:with-param name="N" select="$N"/>
					<xsl:with-param name="W" select="$next[1]"/>
					<xsl:with-param name="O" select="$O"/>
				</xsl:call-template>
				
			</xsl:otherwise>
			</xsl:choose>
			
		</xsl:if>
	</xsl:template>

	<!-- Recursive Search of behaviour state machine starting at BranchState -->
	<xsl:template name="dfsFindTerminal">
		<xsl:param name="S"/> <!-- start node = end node -->
		<xsl:param name="E"/> <!-- edges -->
		<xsl:param name="N"/> <!-- candidate nodes -->
		<xsl:param name="W"/> <!-- node[s] in progress -->
		<xsl:param name="L"/> <!-- level of nesting, want to return terminal on same level as branch -->
		
		<!-- follow connection to next node -->
		<xsl:variable name="next" select="$N[@id=$E/connpoint[@role='src'][@target=$W/@id]/../connpoint[@role='dst']/@target]"/>
		
		<!-- if another branch found, increment level -->
		<xsl:variable name="Level">
			<xsl:choose>
				<xsl:when test="$next[1]/@kind = 'BranchState'">
					<xsl:value-of select="$L + 1"/>
				</xsl:when>
				<xsl:when test="$next[1]/@kind = 'Terminal'">
					<xsl:value-of select="$L - 1"/>
				</xsl:when>
				<xsl:otherwise>
					<xsl:value-of select="$L"/>
				</xsl:otherwise>
			</xsl:choose>
		</xsl:variable>
		
		<!-- next node in this branch -->
		<xsl:if test="($next[1] != $S) and ($next[1]/@kind != 'Terminal' or $L != 0)"> 
			<!-- recursive Depth First Search until branch terminate detected -->
			<xsl:call-template name="dfsFindTerminal">
				<xsl:with-param name="S" select="$S"/>
				<xsl:with-param name="E" select="$E"/>
				<xsl:with-param name="N" select="$N"/>
				<xsl:with-param name="W" select="$next[1]"/>
				<xsl:with-param name="L" select="$Level"/>
			</xsl:call-template>
		</xsl:if>
		<!-- output DFS subtree in reverse order visited, ie Terminal that matches Branch is first in list -->  
		<xsl:if test="($next[1]/@kind = 'Terminal')">
			<xsl:value-of select="concat(($next[1]/@id),',')"/> 
		</xsl:if>
	</xsl:template>

	<!-- Output OutEventNode, if OutputAction grab value of data member -->
	<xsl:template name="OutEventNode">
		<xsl:param name="nextNodeId"/>  <!-- next available node number -->
		<xsl:param name="E"/> <!-- edges -->
		<xsl:param name="W"/>  <!-- node to progress -->
		<xsl:param name="kind"/>  <!-- node is OutEventPortImpl kind -->

		<!-- Insert Event information into this EventPort -->
		<xsl:variable name="OutEreferred" select="$W/../reference[@kind='OutEventPort'][name=string($W/name)]/@referred" />
		<xsl:variable name="itemIsOutEReferredModel" select="/project/folder[@kind='RootFolder']/folder[@kind='InterfaceDefinitions']/descendant::*/model[@id=$OutEreferred]" />
		<xsl:variable name="OutEaggregateUsed">
			<xsl:choose>
				<xsl:when test="contains($itemIsOutEReferredModel/name, 'Event')">
					<xsl:value-of select="substring-before($itemIsOutEReferredModel/name,'Event')" />
				</xsl:when>
				<xsl:otherwise>
					<xsl:value-of select="$itemIsOutEReferredModel/name" />
				</xsl:otherwise>
			</xsl:choose>
		</xsl:variable>
		<xsl:variable name="OutEcountMembers" select="count($itemIsOutEReferredModel/reference[@kind='Member'])" />
		<xsl:variable name="AggregateId" select="concat(concat(concat($nextNodeId, '::'), 'n'), generate-id($itemIsOutEReferredModel))" />

		<node id="{$nextNodeId}">
		<data key="{$nodeLabelKey}">
			<xsl:value-of select="$W/name" />
		</data>
		<xsl:call-template name="nodeXYKeyData">
			<xsl:with-param name="node" select="$W/regnode[@name='PartRegs']/regnode[@name='Behavior']/regnode[@name='Position']" />
		</xsl:call-template>
		<!-- Fixed Kind of OutEventPort rather than $W/@kind -->
		<data key="{$nodeKindKey}">
			<xsl:value-of select="$kind" />
		</data>
		<data key="{$nodeTypeKey}">
			<xsl:value-of select="$W/name" />
		</data>

		<xsl:if test="$OutEcountMembers != 0">
			<graph edgedefault="directed" id="{$nextNodeId}:">
			    <!-- Output Event/Aggregate node -->
				<node id="{$AggregateId}">
				<data key="{$nodeLabelKey}">
				<xsl:value-of select="$OutEaggregateUsed" />
				</data>
				<data key="{$nodeXKey}">15</data>
				<data key="{$nodeYKey}">30</data>
				<data key="{$nodeKindKey}">
				<xsl:value-of select="'AggregateInstance'" />
				</data>
				<data key="{$nodeTypeKey}">
				<xsl:value-of select="$OutEaggregateUsed" />
				</data>
				<graph edgedefault="directed" id="{$AggregateId}:">
				<!-- Output each simple Member of the Event --> 
				<xsl:call-template name="AggregateMembers">
					<xsl:with-param name="event" select="$itemIsOutEReferredModel"/>
					<xsl:with-param name="parentId" select="$AggregateId"/>
					<xsl:with-param name="memberType" select="'MemberInstance'"/>
					<xsl:with-param name="W" select="$W"/>
				</xsl:call-template>
				</graph>
				</node>
			</graph>
		</xsl:if>
		</node>

		<xsl:if test="$OutEcountMembers != 0">
			<!-- Create edge to AggregateInstance -->
			<xsl:variable name="referredAggregate" select="/project/folder[@kind='RootFolder']/folder[@kind='InterfaceDefinitions']/descendant::*/model[@kind='Aggregate'][name=$OutEaggregateUsed]" />
			<xsl:variable name="referredAggregateId" select="concat('n',generate-id($referredAggregate))" />
			<xsl:if test="$referredAggregate">
				<edge id="{$nextNodeId}::eai{generate-id($itemIsOutEReferredModel)}" source="{$AggregateId}" target="{$referredAggregateId}" >
					<data key="{$edgeDescriptionKey}">instance of</data>
				</edge>	
			</xsl:if>
		</xsl:if>
			
	</xsl:template>

	<!-- Output Members of aggregate used -->
	<xsl:template name="AggregateMembers">
		<xsl:param name="event"/>     <!-- Event or Aggregate node to process -->
		<xsl:param name="parentId"/>  <!-- parent Id for all members -->
		<xsl:param name="memberType"/>  <!-- output members as this type -->
		<xsl:param name="W"/>  <!-- EventPort node in progress, used to assign value for this member -->

		<!-- Process Member nodes in sorted order -->
		<xsl:variable name="sortedMembers" >
			<xsl:for-each select="$event/reference[@kind='Member']">
				<!-- sort order is from top to bottom -->
				<xsl:sort select="substring-after(./regnode[@name='PartRegs']/regnode[@name='InterfaceDefinition']/regnode[@name='Position']/value, ',')" data-type="number" order="ascending" /> 
				<xsl:value-of select="concat(',', ./@id)" />
			</xsl:for-each>
		</xsl:variable>
		<xsl:variable name="countDelim" select="string-length($sortedMembers) - string-length(translate($sortedMembers, ',', ''))" />
				
		<!-- Output each simple Member of the Event --> 
		<xsl:for-each select="$event/reference[@kind='Member']">
			<!-- using for-each and position() iteration info, but retrieving Member Node from sorted list -->
			<xsl:variable name="memberNodeId">
				<xsl:call-template name="splitList">
					<xsl:with-param name="pText" select="$sortedMembers" />
					<xsl:with-param name="count" select="$countDelim" />
					<xsl:with-param name="idx" select="position()" />
					<xsl:with-param name="delim" select="','" />
				</xsl:call-template>
			</xsl:variable> 
			<xsl:variable name="memberNode" select="$event/reference[@id=$memberNodeId]/." />

			<xsl:variable name="memberId" select="position() - 1" />
			<xsl:variable name="memberIdLong" select="concat(concat($parentId, '::n'), $memberId)"/>
			<node id="{$memberIdLong}">
			<xsl:variable name="memberName" select="$memberNode/name" />						
			<data key="{$nodeLabelKey}">
			<xsl:value-of select="$memberName" />
			</data>
			<xsl:call-template name="nodeXYKeyData">
				<xsl:with-param name="node" select="$memberNode/regnode[@name='PartRegs']/regnode[@name='InterfaceDefinition']/regnode[@name='Position']" />
			</xsl:call-template>
			<!-- find type may be predefined type or refer to an aggregate structure -->
			<xsl:variable name="referred" select="$memberNode/@referred" />
			<!-- should be in main PredefinedTypes folder, but could be linked to PredefinedTypes in a Worker Library -->
			<xsl:variable name="predefinedTypeName" select="/descendant::*/folder[@kind='PredefinedTypes']/atom[@id=$referred]/name" />
			<data key="{$nodeKindKey}">
				<xsl:choose>
					<xsl:when test="$predefinedTypeName != ''">
						<xsl:value-of select="$memberType" />
					</xsl:when>
					<xsl:otherwise>
						<xsl:value-of select="'AggregateInstance'"/>
					</xsl:otherwise>
				</xsl:choose>
			</data>
			<data key="{$nodeTypeKey}">
				<xsl:choose>
					<xsl:when test="$predefinedTypeName != ''">
						<xsl:value-of select="$predefinedTypeName"/>
					</xsl:when>
					<xsl:otherwise>
						<xsl:value-of select="/project/folder[@kind='RootFolder']/folder[@kind='InterfaceDefinitions']/descendant::*/model[@id=$referred]/name"/>
					</xsl:otherwise>
				</xsl:choose>
			</data>
			<!-- Find Value of OutputAction for this member -->
			<xsl:if test="$W">
				<data key="{$nodeValueKey}">
				<xsl:value-of select="$W/descendant-or-self::*/reference[name=$memberName]/attribute[@kind='Value']/value" />
				</data>
			</xsl:if>
			<!-- Find associated aggregate member to assign key members   /name[name=$DDSAggregate]/..-->	
			<xsl:variable name="DDSAggregate">
				<xsl:choose>
					<xsl:when test="contains($event/name, 'Event')">
						<xsl:value-of select="substring-before($event/name,'Event')" />
					</xsl:when>
					<xsl:otherwise>
						<xsl:value-of select="$event/name" />
					</xsl:otherwise>
				</xsl:choose>
			</xsl:variable>
			<xsl:variable name="itemIsReferredAgregate" select="/project/folder[@kind='RootFolder']/folder[@kind='InterfaceDefinitions']/descendant::*/model[@kind='Aggregate'][name=$DDSAggregate]" />
			<xsl:variable name="keyMemberId" select="$itemIsReferredAgregate/reference[@kind='Member'][name=$memberName]/@id" />
			<xsl:variable name="keyMembertarget" select="$itemIsReferredAgregate/connection[@kind='KeyMember']/connpoint[@target=$keyMemberId]/@target" />
			<xsl:choose>
			  <xsl:when test="string($keyMembertarget) != ''">
				<data key="{$nodeKeyMemberKey}">true</data>
			  </xsl:when>
			  <xsl:otherwise>
				<data key="{$nodeKeyMemberKey}">false</data>
			  </xsl:otherwise>
			</xsl:choose>						
			<data key="{$nodeSortOrderKey}">
				<xsl:value-of select="$memberId" />
			</data>
					
			<!-- recursive search for members and output nodes as instances (ie only top level can be Aggregate Member definition ) -->
			<xsl:if test="not($predefinedTypeName != '')">
				<xsl:variable name="aggregateMember" select="/project/folder[@kind='RootFolder']/folder[@kind='InterfaceDefinitions']/descendant::*/model[@id=$referred]"/>
				<graph edgedefault="directed" id="{$memberIdLong}:">
					<!-- Output each simple Member of the Event --> 
					<xsl:call-template name="AggregateMembers">
						<xsl:with-param name="event" select="$aggregateMember"/>
						<xsl:with-param name="parentId" select="$memberIdLong"/>
						<xsl:with-param name="memberType" select="'MemberInstance'"/>
						<xsl:with-param name="W" select="$W"/>
					</xsl:call-template>
				</graph>
			</xsl:if>
			
			</node>
		</xsl:for-each>
		
	</xsl:template>
	
	<!-- Output InEventNode -->
	<xsl:template name="InEventNode">
		<xsl:param name="nextNodeId"/>  <!-- next available node number -->
		<xsl:param name="E"/> <!-- edges -->
		<xsl:param name="W"/>  <!-- node to progress -->
		<xsl:param name="kind"/>  <!-- node can be either InEventPort or InEventPortIDL -->	
	
		<!-- Insert Event into this EventPort -->
		<xsl:variable name="InEreferred" select="$W/@referred" />
		<xsl:variable name="itemIsInEReferredModel" select="/project/folder[@kind='RootFolder']/folder[@kind='InterfaceDefinitions']/descendant::*/model[@id=$InEreferred]" />
		<xsl:variable name="InEaggregateUsed">
			<xsl:choose>
				<xsl:when test="contains($itemIsInEReferredModel/name, 'Event')">
					<xsl:value-of select="substring-before($itemIsInEReferredModel/name,'Event')" />
				</xsl:when>
				<xsl:otherwise>
					<xsl:value-of select="$itemIsInEReferredModel/name" />
				</xsl:otherwise>
			</xsl:choose>
		</xsl:variable>
		<xsl:variable name="InEcountMembers" select="count($itemIsInEReferredModel/reference[@kind='Member'])" />
		<xsl:variable name="AggregateId" select="concat(concat(concat($nextNodeId,'::'), 'n'), generate-id($itemIsInEReferredModel))" />

		<node id="{$nextNodeId}">
		<data key="{$nodeLabelKey}">
			<xsl:value-of select="$W/name" />
		</data>
		<xsl:call-template name="nodeXYKeyData">
			<xsl:with-param name="node" select="$W/regnode[@name='PartRegs']/regnode[@name='Behavior']/regnode[@name='Position']" />
		</xsl:call-template>
		<data key="{$nodeKindKey}">
			<xsl:value-of select="$kind" />
		</data>
		<data key="{$nodeTypeKey}">
			<xsl:value-of select="$W/name" />
		</data>

		<xsl:if test="$InEcountMembers != 0">
			<graph edgedefault="directed" id="{$nextNodeId}:">
				<!-- Output Event/Aggregate node -->
				<node id="{$AggregateId}">
				<data key="{$nodeLabelKey}">
				<xsl:value-of select="$InEaggregateUsed" />
				</data>
				<data key="{$nodeXKey}">15</data>
				<data key="{$nodeYKey}">30</data>
				<data key="{$nodeKindKey}">
				<xsl:value-of select="'AggregateInstance'" />
				</data>
				<data key="{$nodeTypeKey}">
				<xsl:value-of select="$InEaggregateUsed" />
				</data>
				<graph edgedefault="directed" id="{$AggregateId}:">		
				<!-- Output each simple Member of the Event --> 
				<xsl:call-template name="AggregateMembers">
					<xsl:with-param name="event" select="$itemIsInEReferredModel"/>
					<xsl:with-param name="parentId" select="$AggregateId"/>
					<xsl:with-param name="memberType" select="'MemberInstance'"/>
					<xsl:with-param name="W" select="$W"/>
				</xsl:call-template>
				</graph>
				</node>
			</graph>
		</xsl:if>
		</node>
		
		<xsl:if test="$InEcountMembers != 0">
			<!-- Create edge to AggregateInstance -->
			<xsl:variable name="referredAggregate" select="/project/folder[@kind='RootFolder']/folder[@kind='InterfaceDefinitions']/descendant::*/model[@kind='Aggregate'][name=$InEaggregateUsed]" />
			<xsl:variable name="referredAggregateId" select="concat('n',generate-id($referredAggregate))" />
			<xsl:if test="$referredAggregate">
				<edge id="{$nextNodeId}::eai{generate-id($itemIsInEReferredModel)}" source="{$AggregateId}" target="{$referredAggregateId}" >
					<data key="{$edgeDescriptionKey}">instance of</data>
				</edge>	
			</xsl:if>
		</xsl:if>
		
	</xsl:template>
	
	<!-- Output Workload Node -->
	<xsl:template name="WorkloadNode">
		<xsl:param name="nextNodeId"/> <!-- next available node number -->
		<xsl:param name="E"/> <!-- edges -->
		<xsl:param name="W"/> <!-- node to progress -->

		<xsl:variable name="edge" select="$E/connpoint[@role='src'][@target=$W/@id]/.." />
		<xsl:variable name="postcondition" select="$edge[@kind='Effect']/attribute[@kind='Postcondition']/value" />
		
		<node id="{$nextNodeId}">
		<data key="{$nodeLabelKey}">
		<xsl:value-of select="$W/name" />
		</data>
		<xsl:call-template name="nodeXYKeyData">
			<xsl:with-param name="node" select="$W/regnode[@name='PartRegs']/regnode[@name='Behavior']/regnode[@name='Position']" />
		</xsl:call-template>
		<!-- Fixed Kind of Workload rather than $W/@kind -->
		<data key="{$nodeKindKey}">
		<xsl:value-of select="'Workload'" />
		</data>
		
		<graph edgedefault="directed" id="{$nextNodeId}:">	
		
		<!-- Insert Workload information into this Node -->
		<!-- Workload ActionType -->
		<node id="{$nextNodeId}::n0">
		<data key="{$nodeLabelKey}"> <xsl:value-of select="$W/name" /> </data>
		<data key="{$nodeXKey}"> <xsl:value-of select="15" /> </data>
		<data key="{$nodeYKey}"> <xsl:value-of select="60" /> </data>
		<xsl:variable name="referred" select="$W/reference[@kind='ActionType']/@referred" />
		<xsl:variable name="itemIsWorkerModel" select="/descendant::*/model[@kind='Worker']/model[@id=$referred]/.." />
		<data key="{$nodeKindKey}"> <xsl:value-of select="'Process'" /> </data>	
		<data key="{$nodeFolderKey}"> <xsl:value-of select="$itemIsWorkerModel/../attribute[@kind='Location']/value" /> </data>	
		<data key="{$nodeFileKey}"> <xsl:value-of select="$itemIsWorkerModel/../name" /> </data>		
		<data key="{$nodeWorkerKey}"> <xsl:value-of select="$itemIsWorkerModel/name" /> </data>
		<data key="{$nodeOperationKey}"> <xsl:value-of select="$W/reference[@kind='ActionType']/name" /> </data>
		<data key="{$nodeActionOnKey}"> <xsl:value-of select="'Mainprocess'" /> </data>
		<data key="{$nodeParametersKey}">
		<xsl:variable name="sortedParameters">
			<xsl:for-each select="$W/reference[@kind='SimpleProperty']">
				<!-- sort by position from left to right -->
				<xsl:sort select="substring-before(./regnode[@name='PartRegs']/regnode[@name='Behavior']/regnode[@name='Position']/value, ',')" data-type="number" order="ascending" /> 
				<xsl:value-of select="concat(',', ./attribute[@kind='Value']/value)" />
			</xsl:for-each>
		</xsl:variable>
		<xsl:value-of select="substring-after($sortedParameters, ',')" />
		</data>
		</node>
		
		<!-- Workload Postcondition/Postprocess -->		
		<xsl:if test="not($postcondition = '')">
			<node id="{$nextNodeId}::n1">
			<data key="{$nodeLabelKey}"> <xsl:value-of select="$edge/@kind" /> </data>
			<data key="{$nodeXKey}"> <xsl:value-of select="15" /> </data>
			<data key="{$nodeYKey}"> <xsl:value-of select="120" /> </data>
			<data key="{$nodeKindKey}"> <xsl:value-of select="'Process'" /> </data>	
			<data key="{$nodeActionOnKey}"> <xsl:value-of select="'Postprocess'" /> </data>
			<data key="{$nodeCodeKey}">
			<xsl:value-of select="$postcondition" />
			</data> 
			</node>
		</xsl:if>
		
		</graph>
		</node>
	</xsl:template>
	
	<!-- Output InEffect Node -->
	<xsl:template name="InEffectNode">
		<xsl:param name="nextNodeId"/> <!-- next available node number -->
		<xsl:param name="E"/> <!-- edges -->
		<xsl:param name="W"/> <!-- connection to progress -->

		<xsl:variable name="edge" select="$E/connpoint[@role='src'][@target=$W/@id]/.." />
		<xsl:variable name="inputEffect" select="$edge[@kind='InputEffect']/attribute[@kind='Postcondition']/value" />
		
		<xsl:if test="not($inputEffect = '')">
			<node id="{$nextNodeId}">
			<data key="{$nodeLabelKey}"> <xsl:value-of select="$W/name" /> </data>
			<xsl:call-template name="nodeXYKeyData">
				<xsl:with-param name="node" select="$W/regnode[@name='PartRegs']/regnode[@name='Behavior']/regnode[@name='Position']" />
			</xsl:call-template>
			<!-- Fixed Kind of Workload rather than $W/@kind -->
			<data key="{$nodeKindKey}"> <xsl:value-of select="'Workload'" /> </data>
		
			<graph edgedefault="directed" id="{$nextNodeId}:">	
		
			<!-- Insert Workload information into this Node -->
			<!-- Workload Postcondition/Postprocess -->
			<node id="{$nextNodeId}::n1">
			<data key="{$nodeLabelKey}"> <xsl:value-of select="$edge/@kind" /> </data>
			<data key="{$nodeXKey}"> <xsl:value-of select="15" /> </data>
			<data key="{$nodeYKey}"> <xsl:value-of select="60" /> </data>
			<data key="{$nodeKindKey}"> <xsl:value-of select="'Process'" /> </data>
			<!-- Convert type to new format to allow different icons to be displayed -->
			<data key="{$nodeActionOnKey}"> <xsl:value-of select="'Postprocess'" /> </data>
			<data key="{$nodeCodeKey}"> <xsl:value-of select="$inputEffect" /> </data> 
			</node>
	
			</graph>
			</node>
		</xsl:if>
	</xsl:template>

	<!-- Output OutEffect Node -->
	<xsl:template name="OutEffectNode">
		<xsl:param name="nextNodeId"/> <!-- next available node number -->
		<xsl:param name="E"/> <!-- edges -->
		<xsl:param name="W"/> <!-- connection to progress -->

		<xsl:variable name="edge" select="$E/connpoint[@role='src'][@target=$W/@id]/.." />
		<xsl:variable name="outputEffect" select="$edge[@kind='Effect']/attribute[@kind='Postcondition']/value" />

		<node id="{$nextNodeId}">
		<data key="{$nodeLabelKey}">
		<xsl:value-of select="'OutputAction'" />
		</data>
		<xsl:call-template name="nodeXYKeyData">
			<xsl:with-param name="node" select="$W/regnode[@name='PartRegs']/regnode[@name='Behavior']/regnode[@name='Position']" />
		</xsl:call-template>
		<!-- Fixed Kind of Workload rather than $W/@kind -->
		<data key="{$nodeKindKey}">
		<xsl:value-of select="'Workload'" />
		</data>
		<graph edgedefault="directed" id="{$nextNodeId}:">	
		
		<!-- Insert Workload information into this Node -->
		<!-- Workload Postcondition/Postprocess -->
		<node id="{$nextNodeId}::n1">
		<data key="{$nodeLabelKey}"> <xsl:value-of select="$edge/@kind" /> </data>
		<data key="{$nodeXKey}"> <xsl:value-of select="15" /> </data>
		<data key="{$nodeYKey}"> <xsl:value-of select="60" /> </data>
		<data key="{$nodeKindKey}"> <xsl:value-of select="'Process'" /> </data>
		<!-- Convert type to new format to allow different icons to be displayed -->
		<data key="{$nodeActionOnKey}"> <xsl:value-of select="'Postprocess'" /> </data>
		<data key="{$nodeCodeKey}"> <xsl:value-of select="$outputEffect" /> </data> 
		</node>
	
		</graph>
		</node>
	</xsl:template>
	
	<!-- Output behaviour node -->
	<xsl:template name="behaviourNode">
		<xsl:param name="prevNodeId"/> <!-- previous node id -->
		<xsl:param name="next"/> <!-- next element from input xml -->
		<xsl:param name="E"/> <!-- edges -->
		<xsl:param name="N"/> <!-- candidate nodes -->
		<xsl:param name="prevTerminal"/> <!-- terminal that has been created in this nested branch -->

	    <xsl:variable name="nextNodeId" select="concat(concat(substring-before($prevNodeId,':'),'::n'),generate-id($next))" /> <!-- new unique node id -->
		<xsl:variable name="nextEdgeId" select="concat(concat(substring-before($prevNodeId,':'),'::e'),generate-id($next))" /> <!-- next available edge number -->
				
		<xsl:choose>
		<xsl:when test="$next[@kind='State']" >   <!-- check if we need to connect condition to terminal -->
			<xsl:variable name="findTerminal" select="$N[@id=$E/connpoint[@role='src'][@target=$next/@id]/../connpoint[@role='dst']/@target]"/>
			<xsl:if test="$findTerminal/@kind = 'Terminal' and $prevTerminal != ''">  <!-- $next/@kind = 'Terminal' and $prevTerminal != ''" -->
				<edge id="{$nextEdgeId}" source="{$prevNodeId}" target="{$prevTerminal}" />
			</xsl:if>	
		</xsl:when>
		<xsl:when test="$next/@kind = 'OutputAction'">
			<!-- also need to create Workload for Effect if any Postcondition script exists -->
			<xsl:variable name="edge" select="$E/connpoint[@role='src'][@target=$next/@id]/.." />
			<xsl:variable name="outputEffect" select="$edge[@kind='Effect']/attribute[@kind='Postcondition']/value" />
			<xsl:choose>
				<xsl:when test="not($outputEffect = '')">
					<xsl:variable name="partNodeId" select="concat(concat(substring-before($prevNodeId,':'),'::nn'),generate-id($next))" /> <!-- new unique node id -->
					<xsl:variable name="partEdgeId" select="concat(concat(substring-before($prevNodeId,':'),'::ee'),generate-id($next))" /> <!-- next available edge number -->
					<xsl:call-template name="OutEventNode">
						<xsl:with-param name="nextNodeId" select="$partNodeId"/>
						<xsl:with-param name="E" select="$E"/>
						<xsl:with-param name="W" select="$next"/>
						<xsl:with-param name="kind" select="'OutEventPortImpl'"/>
					</xsl:call-template>
					<edge id="{$partEdgeId}" source="{$prevNodeId}" target="{$partNodeId}" />
					
					<xsl:call-template name="OutEffectNode">
						<xsl:with-param name="nextNodeId" select="$nextNodeId"/>
						<xsl:with-param name="E" select="$E"/>
						<xsl:with-param name="W" select="$next"/>
					</xsl:call-template>
					<edge id="{$nextEdgeId}" source="{$partNodeId}" target="{$nextNodeId}" />	
				</xsl:when>
				<xsl:otherwise>
					<xsl:call-template name="OutEventNode">
						<xsl:with-param name="nextNodeId" select="$nextNodeId"/>
						<xsl:with-param name="E" select="$E"/>
						<xsl:with-param name="W" select="$next"/>
						<xsl:with-param name="kind" select="'OutEventPortImpl'"/>
					</xsl:call-template>
					<edge id="{$nextEdgeId}" source="{$prevNodeId}" target="{$nextNodeId}" />
				</xsl:otherwise>
			</xsl:choose>
			<!-- Need to place an implementation of edge to OutEventPortIDL -->
			<xsl:variable name="OutEventPortId">
				<xsl:choose>
				<xsl:when test="not($outputEffect = '')">
					<xsl:value-of select="concat(concat(substring-before($prevNodeId,':'),'::nn'),generate-id($next))" /> <!-- new unique node id -->
				</xsl:when>
				<xsl:otherwise>
					<xsl:value-of select="$nextNodeId" /> <!-- new unique node id -->
				</xsl:otherwise>
				</xsl:choose>
			</xsl:variable>
			<xsl:variable name="instanceEdgeId" select="concat(concat(substring-before($prevNodeId,':'),'::ei'),generate-id($next))" /> <!-- next unique edge number -->
			<xsl:variable name="OutEventIDL" select="$next/../reference[@kind='OutEventPort'][name=string($next/name)]" />
			<xsl:variable name="outEventPortIDL" select="concat(concat(concat('IDL',substring-before(substring-after($prevNodeId,'Impl'),':')),'::n'),generate-id($OutEventIDL))" />
			<!-- Create implementation of edge OutEventPort -->
			<edge id="{$instanceEdgeId}" source="{$OutEventPortId}" target="{$outEventPortIDL}">
				<data key="{$edgeDescriptionKey}">implementation of</data>
			</edge>  
		</xsl:when>
		<xsl:when test="$next/@kind = 'BranchState'">		
			<node id="{$nextNodeId}">
			<data key="{$nodeLabelKey}">
			<xsl:value-of select="$next/name" />
			</data>
			<xsl:call-template name="nodeXYKeyData">
				<xsl:with-param name="node" select="$next/regnode[@name='PartRegs']/regnode[@name='Behavior']/regnode[@name='Position']" />
			</xsl:call-template>
			<data key="{$nodeKindKey}">
			<xsl:value-of select="'BranchState'" />  <!-- previously Condition -->
			</data>
			<!-- Add child condition nodes -->			
			<xsl:variable name="branchTransitions" select="$E/connpoint[@role='src'][@target=$next/@id]/.." />
			<xsl:variable name="countBranchTransitions" select="count($branchTransitions)" />
			<xsl:if test="$countBranchTransitions > 0">
				<graph edgedefault="directed" id="{$nextNodeId}:">	
				
				<xsl:for-each select="$branchTransitions">
					<node id="{$nextNodeId}::n{position() - 1}">
					<data key="{$nodeLabelKey}">
					<xsl:value-of select="concat('Condition',position())" />
					</data>
					<data key="{$nodeXKey}">
					<xsl:value-of select="15" />
					</data>
					<data key="{$nodeYKey}">
					<xsl:value-of select="position() * 50" />
					</data>
					<data key="{$nodeKindKey}">
					<xsl:value-of select="'Condition'" />  
					</data>
					<data key="{$nodeValueKey}">
					<xsl:value-of select="./attribute[@kind='Condition']/value" />
					</data>
					<data key="{$nodeSortOrderKey}">
					<xsl:value-of select="position() - 1" />
					</data>
					</node>
				</xsl:for-each>
	
				</graph>	
			</xsl:if>
		
			</node>	
			<edge id="{$nextEdgeId}" source="{$prevNodeId}" target="{$nextNodeId}" />
		</xsl:when>
		<xsl:when test="$next/@kind = 'Terminal'">
			<node id="{$nextNodeId}">
			<data key="{$nodeLabelKey}">
			<xsl:value-of select="$next/name" />
			</data>
			<xsl:call-template name="nodeXYKeyData">
				<xsl:with-param name="node" select="$next/regnode[@name='PartRegs']/regnode[@name='Behavior']/regnode[@name='Position']" />
			</xsl:call-template>
			<data key="{$nodeKindKey}">
			<xsl:value-of select="'Termination'" />
			</data>
			</node>	
			<edge id="{$nextEdgeId}" source="{$prevNodeId}" target="{$nextNodeId}" />
		</xsl:when>
		<xsl:when test="$next/@kind = 'InputAction'">
			<xsl:variable name="edge" select="$E/connpoint[@role='src'][@target=$next/@id]/.." />
			<xsl:variable name="inputEffect" select="$edge[@kind='InputEffect']/attribute[@kind='Postcondition']/value" />
			<xsl:if test="not($inputEffect = '')">
				<xsl:call-template name="InEffectNode">
					<xsl:with-param name="nextNodeId" select="$nextNodeId"/>
					<xsl:with-param name="E" select="$E"/>
					<xsl:with-param name="W" select="$next"/>
				</xsl:call-template>
				<edge id="{$nextEdgeId}" source="{$prevNodeId}" target="{$nextNodeId}" />
			</xsl:if>
		</xsl:when> 
		<xsl:otherwise>		
			<xsl:call-template name="WorkloadNode">
				<xsl:with-param name="nextNodeId" select="$nextNodeId"/>
				<xsl:with-param name="E" select="$E"/>
				<xsl:with-param name="W" select="$next"/>
			</xsl:call-template>
			<edge id="{$nextEdgeId}" source="{$prevNodeId}" target="{$nextNodeId}" />
		</xsl:otherwise>
		</xsl:choose>

	</xsl:template>

	<!-- Create Component Implementations (ie Instances) and connect instances to base nodes -->
	<xsl:template match="folder[@kind='ComponentImplementations']" name="ComponentImplementations">
	    <xsl:apply-templates select="model[@kind='ComponentImplementationContainer']" />
		
		<!-- Map all Event Port nodes, to link old id value to new id value -->
		<xsl:variable name="portMap" >
			<xsl:for-each select="./descendant::*/model[@kind='ComponentAssembly']">
				<xsl:call-template name="ComponentAssemblyMap" />
			</xsl:for-each>
			<xsl:call-template name="ComponentIDLMap" />
		</xsl:variable>
		<!-- <xsl:copy-of select="$portMap"/>  -->
	
		<xsl:for-each select="./descendant::*/model[@kind='ComponentAssembly']">
			<xsl:variable name="asmId" select="concat('n',generate-id(.))" />
			
			<!-- Create 'delegates to' link from OutEventPort to OutEventPortDelegate -->		
			<xsl:for-each select="./connection[@kind='EventSourceDelegate']" >
			
				<xsl:variable name="EventSourceDelegateId">
					<xsl:choose>
					<xsl:when test="string(./@derivedfrom) = '' " >
						<xsl:value-of select="./@id" />
					</xsl:when>
					<xsl:otherwise>
						<xsl:value-of select="./@derivedfrom" />
					</xsl:otherwise>
					</xsl:choose>
				</xsl:variable>
				<xsl:variable name="EventSourceDelegate" select="/project/folder[@kind='RootFolder']/folder[@kind='ComponentImplementations']/descendant::*/model[@kind='ComponentAssembly']/connection[@id=$EventSourceDelegateId]"/>
				<xsl:variable name="OutEventPortDelegateId">
					<xsl:choose>
					<xsl:when test="string(./@derivedfrom) = '' " >
						<xsl:value-of select="$EventSourceDelegate/connpoint[@role='dst']/@target" />
					</xsl:when>
					<xsl:otherwise>
						<xsl:value-of select="../atom[@kind='OutEventPortDelegate'][@derivedfrom=$EventSourceDelegate/connpoint[@role='dst']/@target]/@id" />
					</xsl:otherwise>
					</xsl:choose>
				</xsl:variable>				
				<xsl:variable name="OutEventPortInstanceId">
					<xsl:choose>
					<xsl:when test="string(./@derivedfrom) = '' " >
						<xsl:value-of select="$EventSourceDelegate/connpoint[@role='src']/@target" />
					</xsl:when>
					<xsl:otherwise>
						<xsl:value-of select="../model[@kind='ComponentInstance']/reference[@kind='OutEventPortInstance'][@derivedfrom=$EventSourceDelegate/connpoint[@role='src']/@target]/@id" />
					</xsl:otherwise>
					</xsl:choose>
				</xsl:variable>	
				
				<xsl:variable name="newOId" select="substring-before(substring-after($portMap,$OutEventPortInstanceId), ',')" />
				<xsl:variable name="newDelegateId" select="substring-before(substring-after($portMap,$OutEventPortDelegateId), ',')" />
				<xsl:variable name="DelegatesToPos" select="position() - 1" />

				<!-- create edge for delegates to link -->
				<edge id="{$asmId}::eod{$DelegatesToPos}" source="{$newOId}" target="{$newDelegateId}" >
					<data key="{$edgeDescriptionKey}">delegates to</data>
				</edge>
			</xsl:for-each>
			
			<!-- Create 'delegates to' link from InEventPortDelegate to InEventPort-->		
			<xsl:for-each select="./connection[@kind='EventSinkDelegate']" >
				<xsl:variable name="EventSinkDelegateId">
					<xsl:choose>
					<xsl:when test="string(./@derivedfrom) = '' " >
						<xsl:value-of select="./@id" />
					</xsl:when>
					<xsl:otherwise>
						<xsl:value-of select="./@derivedfrom" />
					</xsl:otherwise>
					</xsl:choose>
				</xsl:variable>
				<xsl:variable name="EventSinkDelegate" select="/project/folder[@kind='RootFolder']/folder[@kind='ComponentImplementations']/descendant::*/model[@kind='ComponentAssembly']/connection[@id=$EventSinkDelegateId]"/>
				<xsl:variable name="InEventPortDelegateId">
					<xsl:choose>
					<xsl:when test="string(./@derivedfrom) = '' " >
						<xsl:value-of select="$EventSinkDelegate/connpoint[@role='src']/@target" />
					</xsl:when>
					<xsl:otherwise>
						<xsl:value-of select="../atom[@kind='InEventPortDelegate'][@derivedfrom=$EventSinkDelegate/connpoint[@role='src']/@target]/@id" />
					</xsl:otherwise>
					</xsl:choose>
				</xsl:variable>	
				<xsl:variable name="InEventPortInstanceId">
					<xsl:choose>
					<xsl:when test="string(./@derivedfrom) = '' " >
						<xsl:value-of select="$EventSinkDelegate/connpoint[@role='dst']/@target" />
					</xsl:when>
					<xsl:otherwise>
						<xsl:value-of select="../model[@kind='ComponentInstance']/reference[@kind='InEventPortInstance'][@derivedfrom=$EventSinkDelegate/connpoint[@role='dst']/@target]/@id" />
					</xsl:otherwise>
					</xsl:choose>
				</xsl:variable>	
				
				<xsl:variable name="newIId" select="substring-before(substring-after($portMap,$InEventPortInstanceId), ',')" />
				<xsl:variable name="newIDelegateId" select="substring-before(substring-after($portMap,$InEventPortDelegateId), ',')" />
				<xsl:variable name="DelegateToPos" select="position() - 1" />
				<!-- create edge for delegates to link -->
				<edge id="{$asmId}::eid{$DelegateToPos}" source="{$newIDelegateId}" target="{$newIId}" >
					<data key="{$edgeDescriptionKey}">delegates to</data>
				</edge>
			</xsl:for-each>
			
			<!-- Create all network connections between Event Ports -->
			<xsl:for-each select="./connection[@kind='SendsTo']" >
				<xsl:variable name="outEventPortId" select="./connpoint[@role='src']/@target" />
				<xsl:variable name="newOutId" select="substring-before(substring-after($portMap,$outEventPortId), ',')" />
				<xsl:variable name="inEventPortId" select="./connpoint[@role='dst']/@target" />
				<xsl:variable name="newInId" select="substring-before(substring-after($portMap,$inEventPortId), ',')" />
				<xsl:variable name="sendToPos" select="position() - 1" />
				<edge id="{$asmId}::e{$sendToPos}" source="{$newOutId}" target="{$newInId}" />
			</xsl:for-each>	
			
			<!-- Create instance connections between ComponentInstances and Components -->
			<xsl:for-each select="./model[@kind='ComponentInstance']">
				<xsl:variable name="ComponentInstanceId" select="./@id" />
				<xsl:variable name="ComponentInstanceTypeReferred" select="./reference[@kind='ComponentInstanceType']/@referred" />
				<xsl:variable name="ComponentImplementationContainer" select="/project/folder[@kind='RootFolder']/folder[@kind='ComponentImplementations']/model[@kind='ComponentImplementationContainer']/connection[@kind='Implements']/connpoint[@role='src'][@target=$ComponentInstanceTypeReferred]/../.." />
				<xsl:variable name="Implements" select="$ComponentImplementationContainer/connection[@kind='Implements']/connpoint[@role='src'][@target=$ComponentInstanceTypeReferred]/../connpoint[@role='dst']/@target" />
				<xsl:variable name="ComponentReferred" select="$ComponentImplementationContainer/reference[@kind='ComponentRef']/@referred" />
				
				<xsl:variable name="newInstanceId" select="substring-before(substring-after($portMap,$ComponentInstanceId), ',')" />
				<xsl:variable name="newComponentId" select="substring-before(substring-after($portMap,$ComponentReferred), ',')" />
				<xsl:variable name="InstanceOfPos" select="position() - 1" />
				<!-- create edge for instance of definition -->	
				<edge id="{$asmId}::eci{$InstanceOfPos}" source="{$newInstanceId}" target="{$newComponentId}" >
					<data key="{$edgeDescriptionKey}">instance of</data>
				</edge>	

				<xsl:variable name="countOutEventPorts" select="count(./reference[@kind='OutEventPortInstance'])" />
				<xsl:for-each select="./reference[@kind='OutEventPortInstance']">
					<xsl:variable name="OutEventPortInstanceId" select="./@id" />
					<xsl:variable name="OutEventPortReferred" select="./@referred" />
					<xsl:variable name="newOutEventPortInstanceId" select="substring-before(substring-after($portMap,$OutEventPortInstanceId), ',')" />
					<xsl:variable name="newOutEventPortId" select="substring-before(substring-after($portMap,$OutEventPortReferred), ',')" />
					<xsl:variable name="OutPortPos" select="position() - 1" />
					<!-- create edge for instance of definition -->	
					<edge id="{$asmId}::eci{$InstanceOfPos}{$OutPortPos}" source="{$newOutEventPortInstanceId}" target="{$newOutEventPortId}" >
						<data key="{$edgeDescriptionKey}">instance of</data>
					</edge>	
				</xsl:for-each>
			
				<xsl:for-each select="./reference[@kind='InEventPortInstance']">
					<xsl:variable name="InEventPortInstanceId" select="./@id" />
					<xsl:variable name="InEventPortReferred" select="./@referred" />
					<xsl:variable name="newInEventPortInstanceId" select="substring-before(substring-after($portMap,$InEventPortInstanceId), ',')" />
					<xsl:variable name="newInEventPortId" select="substring-before(substring-after($portMap,$InEventPortReferred), ',')" />
					<xsl:variable name="InPortPos" select="position() + $countOutEventPorts" />
					<!-- create edge for instance of definition -->	
					<edge id="{$asmId}::eci{$InstanceOfPos}{$InPortPos}" source="{$newInEventPortInstanceId}" target="{$newInEventPortId}" >
						<data key="{$edgeDescriptionKey}">instance of</data>
					</edge>	
				</xsl:for-each>

			</xsl:for-each>
			
		</xsl:for-each>
    </xsl:template>

	<xsl:template match="model[@kind='ComponentImplementationContainer']" name="ComponentImplementationContainer">
	    <xsl:apply-templates select="model[@kind='ComponentAssembly']">
			<xsl:with-param name="containerName" select="./name" />
		</xsl:apply-templates>
    </xsl:template>

	<!-- need to know the IDL file name to save as key data in each node -->	
	<xsl:template match="model[@kind='ComponentAssembly']" name="ComponentAssembly">
		<xsl:param name="containerName" />
			
		<xsl:variable name="projectName"><xsl:value-of select="/project/folder[@kind='RootFolder']/name" /></xsl:variable>
		<xsl:variable name="assemblyName"><xsl:value-of select="./name" /></xsl:variable>

		<!-- Output Assembly node -->
		<xsl:variable name="asmId" select="concat('n',generate-id(.))" />
	    <node id="{$asmId}">
        <data key="{$nodeLabelKey}">
		<xsl:value-of select="$assemblyName" />
		</data>
		<xsl:call-template name="nodeXYKeyData">
			<xsl:with-param name="node" select="./regnode[@name='PartRegs']/regnode[@name='Packaging']/regnode[@name='Position']" />
		</xsl:call-template>
		<data key="{$nodeKindKey}">
		<xsl:value-of select="./@kind" />
		</data>
		<!-- project and UUID moved to Model node 
		<data key="{$nodeProjectKey}">
		<xsl:value-of select="$projectName" />
		</data>
		<data key="{$nodeProjectUUID}">
		<xsl:value-of select="translate( /project/@guid , '{}', '') " />
		</data>
		-->
		<data key="{$nodeContainerKey}">
		<xsl:value-of select="$containerName" />
		</data>
		<graph edgedefault="directed" id="{$asmId}:">
		
		<!-- Output each OutEventPortDelegate in this Assembly --> 
		<xsl:for-each select="./atom[@kind='OutEventPortDelegate']">
			<node id="{$asmId}::n{generate-id(.)}">
			<xsl:variable name="OutEventPortDelegateId">
				<xsl:choose>
				<xsl:when test="string(./@derivedfrom) = '' " >
					<xsl:value-of select="./@id" />
				</xsl:when>
				<xsl:otherwise>
					<xsl:value-of select="./@derivedfrom" />
				</xsl:otherwise>
				</xsl:choose>
			</xsl:variable>
			<xsl:variable name="OutEventPortDelegate" select="/project/folder[@kind='RootFolder']/folder[@kind='ComponentImplementations']/descendant::*/model[@kind='ComponentAssembly']/atom[@id=$OutEventPortDelegateId]"/>
			<xsl:variable name="OutEventPortInstanceId" select="/project/folder[@kind='RootFolder']/folder[@kind='ComponentImplementations']/descendant::*/connection[@kind='EventSourceDelegate']/connpoint[@role='dst'][@target=$OutEventPortDelegateId]/../connpoint[@role='src']/@target" />
			<xsl:variable name="OutEventPortReferred" select="/project/folder[@kind='RootFolder']/folder[@kind='ComponentImplementations']/descendant::*/model[@kind='ComponentInstance']/reference[@id=$OutEventPortInstanceId]/@referred" />
			<xsl:variable name="OutEventReferred" select="/project/folder[@kind='RootFolder']/folder[@kind='InterfaceDefinitions']/descendant::*/model[@kind='Component']/reference[@id=$OutEventPortReferred]/@referred" />
			<data key="{$nodeLabelKey}">
			<xsl:value-of select="/project/folder[@kind='RootFolder']/folder[@kind='InterfaceDefinitions']/descendant::*/model[@kind='Component']/reference[@id=$OutEventPortReferred]/name" />
			</data>
			<xsl:call-template name="nodeXYKeyData">
				<xsl:with-param name="node" select="$OutEventPortDelegate/regnode[@name='PartRegs']/regnode[@name='Packaging']/regnode[@name='Position']" />
			</xsl:call-template>
			<data key="{$nodeKindKey}">
			<xsl:value-of select="./@kind" />
			</data>
<!-- Not implemented in Modelling tool, unsure what type OutEventPortDelegates will have...
			<xsl:variable name="itemIsOutEReferredModel" select="/project/folder[@kind='RootFolder']/folder[@kind='InterfaceDefinitions']/descendant::*/model[@id=$OutEventReferred]" />
			<xsl:variable name="OutEaggregateUsed">
				<xsl:choose>
					<xsl:when test="contains($itemIsOutEReferredModel/name, 'Event')">
						<xsl:value-of select="substring-before($itemIsOutEReferredModel/name,'Event')" />
					</xsl:when>
					<xsl:otherwise>
						<xsl:value-of select="$itemIsOutEReferredModel/name" />
					</xsl:otherwise>
				</xsl:choose>
			</xsl:variable>
			<data key="{$nodeTypeKey}">
			<xsl:value-of select="$OutEaggregateUsed" />
			</data>
-->
			</node>
		</xsl:for-each>

		<!-- Output each InEventPortDelegate in this Assembly --> 
		<xsl:for-each select="./atom[@kind='InEventPortDelegate']">
			<node id="{$asmId}::n{generate-id(.)}">
			<xsl:variable name="InEventPortDelegateId">
				<xsl:choose>
				<xsl:when test="string(./@derivedfrom) = '' " >
					<xsl:value-of select="./@id" />
				</xsl:when>
				<xsl:otherwise>
					<xsl:value-of select="./@derivedfrom" />
				</xsl:otherwise>
				</xsl:choose>
			</xsl:variable>
			<xsl:variable name="InEventPortDelegate" select="/project/folder[@kind='RootFolder']/folder[@kind='ComponentImplementations']/descendant::*/model[@kind='ComponentAssembly']/atom[@id=$InEventPortDelegateId]"/>
			<xsl:variable name="InEventPortInstanceId" select="/project/folder[@kind='RootFolder']/folder[@kind='ComponentImplementations']/descendant::*/connection[@kind='EventSinkDelegate']/connpoint[@role='src'][@target=$InEventPortDelegateId]/../connpoint[@role='dst']/@target" />
			<xsl:variable name="InEventPortReferred" select="/project/folder[@kind='RootFolder']/folder[@kind='ComponentImplementations']/descendant::*/model[@kind='ComponentInstance']/reference[@id=$InEventPortInstanceId]/@referred" />
			<xsl:variable name="InEventReferred" select="/project/folder[@kind='RootFolder']/folder[@kind='InterfaceDefinitions']/descendant::*/model[@kind='Component']/reference[@id=$InEventPortReferred]/@referred" />
			<data key="{$nodeLabelKey}">
			<xsl:value-of select="/project/folder[@kind='RootFolder']/folder[@kind='InterfaceDefinitions']/descendant::*/model[@kind='Component']/reference[@id=$InEventPortReferred]/name" />
			</data>
			<xsl:call-template name="nodeXYKeyData">
				<xsl:with-param name="node" select="$InEventPortDelegate/regnode[@name='PartRegs']/regnode[@name='Packaging']/regnode[@name='Position']" />
			</xsl:call-template>
			<data key="{$nodeKindKey}">
			<xsl:value-of select="./@kind" />
			</data>
<!-- Not implemented in Modelling tool, unsure what type InEventPortDelegates will have...
			<xsl:variable name="itemIsInEReferredModel" select="/project/folder[@kind='RootFolder']/folder[@kind='InterfaceDefinitions']/descendant::*/model[@id=$InEventReferred]" />
			<xsl:variable name="InEaggregateUsed">
				<xsl:choose>
					<xsl:when test="contains($itemIsInEReferredModel/name, 'Event')">
						<xsl:value-of select="substring-before($itemIsInEReferredModel/name,'Event')" />
					</xsl:when>
					<xsl:otherwise>
						<xsl:value-of select="$itemIsInEReferredModel/name" />
					</xsl:otherwise>
				</xsl:choose>
			</xsl:variable>
			<data key="{$nodeTypeKey}">
			<xsl:value-of select="$InEaggregateUsed" />
			</data> -->
			</node>
		</xsl:for-each>		

		<!-- Output each ComponentInstance in this Assembly --> 
		<xsl:for-each select="./model[@kind='ComponentInstance']" >
		
			<xsl:variable name="componentInstId">
			<xsl:choose>
				<xsl:when test="string(./@derivedfrom) = '' " >
					<xsl:value-of select="./@id" />
				</xsl:when>
				<xsl:otherwise>
					<xsl:value-of select="./@derivedfrom" />
				</xsl:otherwise>
				</xsl:choose>
			</xsl:variable>
			<xsl:variable name="componentInst" select="/project/folder[@kind='RootFolder']/folder[@kind='ComponentImplementations']/descendant::*/model[@kind='ComponentInstance'][@id=$componentInstId]/." />
			<xsl:variable name="componentInstanceType" select="$componentInst/reference[@kind='ComponentInstanceType']" />
			<xsl:variable name="monolithicImplementation" select="/project/folder[@kind='RootFolder']/folder[@kind='ComponentImplementations']/model/atom[@id=$componentInstanceType/@referred]" />
			<xsl:variable name="componentImplementationContainer" select="$monolithicImplementation/.." />
			<xsl:variable name="implements" select="$componentImplementationContainer/connection[@kind='Implements']/connpoint[@role='src'][@target=$componentInstanceType/@referred]/.." />
			<xsl:variable name="componentRef" select="$componentImplementationContainer/reference[@kind='ComponentRef'][@id=$implements/connpoint[@role='dst']/@target]" />
			<xsl:variable name="component" select="/project/folder[@kind='RootFolder']/folder[@kind='InterfaceDefinitions']/descendant::*/model[@id=$componentRef/@referred]" />

			<xsl:call-template name="ComponentNode">
				<xsl:with-param name="parentNodeId" select="$asmId" />
				<xsl:with-param name="startNodeId" select="position() - 1" />
				<xsl:with-param name="Inst" select="$componentInst" />
				<xsl:with-param name="Impl" select="$component" />
				<xsl:with-param name="container" select="$containerName" />
			</xsl:call-template>

		</xsl:for-each>

		<xsl:for-each select="./model[@kind='ComponentAssembly']" >
			<xsl:call-template name="ComponentAssembly">
				<xsl:with-param name="containerName" select="$containerName" />
			</xsl:call-template>
		</xsl:for-each>
		
		</graph>
		</node>
		
	</xsl:template>

	<!-- Component and Event Port nodes -->
    <xsl:template name="ComponentNode">
	    <xsl:param name="parentNodeId" />
	    <xsl:param name="startNodeId" />
	    <xsl:param name="Inst" />
	    <xsl:param name="Impl" />
	    <xsl:param name="container" />
		
		<!-- Output Component node -->
	    <node id="{$parentNodeId}::n{$startNodeId}">
		
        <data key="{$nodeLabelKey}">
		<xsl:value-of select="$Inst/name" />
		</data>
		<xsl:call-template name="nodeXYKeyData">
			<xsl:with-param name="node" select="$Inst/regnode[@name='PartRegs']/regnode[@name='Packaging']/regnode[@name='Position']" />
		</xsl:call-template>
		<data key="{$nodeKindKey}">
		<xsl:value-of select="$Inst/@kind" />
		</data>
		<data key="{$nodeTypeKey}">
		<xsl:value-of select="$Impl/name" />
		</data>
<!--		The container is defined in the parent node
		<data key="{$nodeContainerKey}">
		<xsl:value-of select="$container" />
		</data>
-->
		<data key="{$nodeComponentInstanceUUID}">
		<xsl:value-of select="./attribute[@kind='UUID']/value"/>
		</data>	

		<graph edgedefault="directed" id="{$parentNodeId}::n{$startNodeId}:">

		<!-- Output each simple Attribute of the Component --> 
		<xsl:variable name="countAttributes" select="count($Impl/model[@kind='Attribute'])" />
		<xsl:for-each select="$Impl/model[@kind='Attribute']">
			<xsl:variable name="i" select="position()" />
			<node id="{$parentNodeId}::n{$startNodeId}::n{$i - 1}">
			<data key="{$nodeLabelKey}">
			<xsl:value-of select="./name" />
			</data>
			<xsl:call-template name="nodeXYKeyData">
				<xsl:with-param name="node" select="./regnode[@name='PartRegs']/regnode[@name='InterfaceDefinition']/regnode[@name='Position']" />
			</xsl:call-template>
			<data key="{$nodeKindKey}">
			<xsl:value-of select="'AttributeInstance'" />
			</data>
			<data key="{$nodeTypeKey}">
			<xsl:variable name="referred" select="./reference/@referred" />
			<!-- should be in main PredefinedTypes folder, but could be linked to PredefinedTypes in a Worker Library -->
			<xsl:value-of select="/descendant::*/folder[@kind='PredefinedTypes']/atom[@id=$referred]/name" />
			</data>
			<!-- Find value assigned to the attribute instance -->
			<xsl:variable name="attributeId" select="./@id" />
			<xsl:variable name="attributeInstance" select="$Inst/reference[@kind='AttributeInstance'][@referred=$attributeId]" />
			<xsl:variable name="attributeValue" select="$Inst/../connection/connpoint[@role='src'][@target=$attributeInstance/@id]/.." />
			<xsl:variable name="simpleProperty" select="$Inst/../reference[@id=$attributeValue/connpoint[@role='dst']/@target]" />
			<data key="{$nodeValueKey}">
				<xsl:value-of select="$simpleProperty/attribute[@kind='Value']/value" /> 
			</data>	
			</node>
		</xsl:for-each>
		
		<!-- Output each OutEventPort of the Component --> 
        <xsl:variable name="countOutEventPorts" select="count($Impl/reference[@kind='OutEventPort'])" />
		<xsl:for-each select="$Impl/reference[@kind='OutEventPort']">
			<xsl:variable name="j" select="position()" />
			<node id="{$parentNodeId}::n{$startNodeId}::n{$countAttributes + $j - 1}">
			<data key="{$nodeLabelKey}">
			<xsl:value-of select="./name" />
			</data>
			<xsl:call-template name="nodeXYKeyData">
				<xsl:with-param name="node" select="./regnode[@name='PartRegs']/regnode[@name='InterfaceDefinition']/regnode[@name='Position']" />
			</xsl:call-template>
			<data key="{$nodeKindKey}">
			<xsl:value-of select="'OutEventPortInstance'" />
			</data>
<!--			<xsl:variable name="referred" select="./@referred" />
			<xsl:variable name="itemIsOutEReferredModel" select="/project/folder[@kind='RootFolder']/folder[@kind='InterfaceDefinitions']/descendant::*/model[@id=$referred]" />
			<xsl:variable name="OutEaggregateUsed">
				<xsl:choose>
					<xsl:when test="contains($itemIsOutEReferredModel/name, 'Event')">
						<xsl:value-of select="substring-before($itemIsOutEReferredModel/name,'Event')" />
					</xsl:when>
					<xsl:otherwise>
						<xsl:value-of select="$itemIsOutEReferredModel/name" />
					</xsl:otherwise>
				</xsl:choose>
			</xsl:variable>
-->
			<data key="{$nodeTypeKey}">
			<xsl:value-of select="./name" />
			</data>
<!--		<data key="{$nodeTopicNameKey}">
			<xsl:value-of select="concat(concat( ../name, '_'), ./name)" />
			</data>
-->
			</node>
		</xsl:for-each>

		<!-- Output each InEventPort of the Component --> 
		<xsl:for-each select="$Impl/reference[@kind='InEventPort']">
			<xsl:variable name="k" select="position()" />
			<node id="{$parentNodeId}::n{$startNodeId}::n{$countAttributes + $countOutEventPorts + $k - 1}">
			<data key="{$nodeLabelKey}">
			<xsl:value-of select="./name" />
			</data>
			<xsl:call-template name="nodeXYKeyData">
				<xsl:with-param name="node" select="./regnode[@name='PartRegs']/regnode[@name='InterfaceDefinition']/regnode[@name='Position']" />
			</xsl:call-template>
			<data key="{$nodeKindKey}">
			<xsl:value-of select="'InEventPortInstance'" />
			</data>
<!--			<xsl:variable name="referred" select="./@referred" />
			<xsl:variable name="itemIsInEReferredModel" select="/project/folder[@kind='RootFolder']/folder[@kind='InterfaceDefinitions']/descendant::*/model[@id=$referred]" />
			<xsl:variable name="InEaggregateUsed">
				<xsl:choose>
					<xsl:when test="contains($itemIsInEReferredModel/name, 'Event')">
						<xsl:value-of select="substring-before($itemIsInEReferredModel/name,'Event')" />
					</xsl:when>
					<xsl:otherwise>
						<xsl:value-of select="$itemIsInEReferredModel/name" />
					</xsl:otherwise>
				</xsl:choose>
			</xsl:variable>
-->
			<data key="{$nodeTypeKey}">
			<xsl:value-of select="./name" />
			</data>
<!--		<data key="{$nodeTopicNameKey}">
			<xsl:value-of select="concat(concat( ../name, '_'), ./name)" />
			</data>
-->
			</node>
		</xsl:for-each>		

		</graph>
		</node>
	</xsl:template>

	<!-- ComponentAssembly nodes -->
    <xsl:template name="ComponentAssemblyMap">
		<xsl:variable name="asmId" select="concat('n',generate-id(.))" />
		
		<xsl:value-of select="','"/>
		
		<!-- Output each OutEventPortDelegate in this Assembly --> 
		<xsl:for-each select="./atom[@kind='OutEventPortDelegate']">
			<xsl:variable name="OutEventPortDelegateId" select="./@id" />
			<xsl:value-of select="concat(concat($OutEventPortDelegateId, concat(concat($asmId,'::n'), generate-id(.) ) ),',')" />
		</xsl:for-each>

		<!-- Output each InEventPortDelegate in this Assembly --> 
		<xsl:for-each select="./atom[@kind='InEventPortDelegate']">
			<xsl:variable name="InEventPortDelegateId" select="./@id" />
			<xsl:value-of select="concat(concat($InEventPortDelegateId, concat(concat($asmId,'::n'), generate-id(.) ) ),',')" />
		</xsl:for-each>		
	
		<xsl:for-each select="./model[@kind='ComponentInstance']" >
			<xsl:variable name="componentInstance" select="." />
			<xsl:variable name="componentInstanceType" select="$componentInstance/reference[@kind='ComponentInstanceType']" />
			<xsl:variable name="monolithicImplementation" select="/project/folder[@kind='RootFolder']/folder[@kind='ComponentImplementations']/model/atom[@id=$componentInstanceType/@referred]" />
			<xsl:variable name="componentImplementationContainer" select="$monolithicImplementation/.." />
			<xsl:variable name="implements" select="$componentImplementationContainer/connection[@kind='Implements']/connpoint[@role='src'][@target=$componentInstanceType/@referred]/.." />
			<xsl:variable name="componentRef" select="$componentImplementationContainer/reference[@kind='ComponentRef'][@id=$implements/connpoint[@role='dst']/@target]" />
			<xsl:variable name="component" select="/project/folder[@kind='RootFolder']/folder[@kind='InterfaceDefinitions']/descendant::*/model[@id=$componentRef/@referred]" />

			<xsl:call-template name="ComponentPortMap">
				<xsl:with-param name="parentNodeId" select="$asmId" />
				<xsl:with-param name="startNodeId" select="position() - 1" />
				<xsl:with-param name="Inst" select="$componentInstance" />
				<xsl:with-param name="Impl" select="$component" />
			</xsl:call-template>
		</xsl:for-each>
		
		<xsl:for-each select="./model[@kind='ComponentAssembly']" >
			<xsl:call-template name="ComponentAssemblyMap" />
		</xsl:for-each>
		
	</xsl:template>
	
	<!-- Component and Event Port nodes -->
    <xsl:template name="ComponentPortMap">
	    <xsl:param name="parentNodeId" />
	    <xsl:param name="startNodeId" />
	    <xsl:param name="Inst" />
	    <xsl:param name="Impl" />
		
		<xsl:value-of select="concat(concat($Inst/@id, concat(concat($parentNodeId,'::n'),$startNodeId)) ,',')" />

		<xsl:variable name="countAttributes" select="count($Impl/model[@kind='Attribute'])" />
		
		<!-- Map each OutEventPortInstance of this Component --> 
        <xsl:variable name="countOutEventPorts" select="count($Impl/reference[@kind='OutEventPort'])" />
		<xsl:for-each select="$Impl/reference[@kind='OutEventPort']">
			<xsl:variable name="j" select="position()" />
			<xsl:variable name="outEventPortId" select="./@id" />
			<xsl:variable name="outEventPortInstance" select="$Inst/reference[@kind='OutEventPortInstance'][@referred=$outEventPortId]" />
			<xsl:value-of select="concat(concat($outEventPortInstance/@id, concat(concat(concat(concat($parentNodeId,'::n'),$startNodeId),'::n'),$countAttributes + $j - 1) ),',')" />
		</xsl:for-each>
		
		<!-- Map each InEventPortInstance of this Component --> 
		<xsl:for-each select="$Impl/reference[@kind='InEventPort']">
			<xsl:variable name="k" select="position()" />
			<xsl:variable name="inEventPortId" select="./@id" />
			<xsl:variable name="inEventPortInstance" select="$Inst/reference[@kind='InEventPortInstance'][@referred=$inEventPortId]" />
			<xsl:value-of select="concat(concat($inEventPortInstance/@id, concat(concat(concat(concat($parentNodeId,'::n'),$startNodeId),'::n'),$countAttributes + $countOutEventPorts + $k - 1)),',')" />
		</xsl:for-each>		
		
	</xsl:template>

		<!-- Component and Event Port nodes -->
    <xsl:template name="ComponentIDLMap" >
		<xsl:for-each select="/project/folder[@kind='RootFolder']/folder[@kind='InterfaceDefinitions']/descendant::*/model[@kind='Component']">
			<!-- Output Component node -->
			<xsl:variable name="relPos" select="generate-id(.)" />
			<xsl:variable name="componentId" select="concat('IDL',$relPos)" />
			<xsl:value-of select="concat(concat(./@id, $componentId), ',')" />
			
			<xsl:for-each select="./reference[@kind='OutEventPort']">
				<xsl:variable name="outEventPortId" select="concat(concat($componentId,'::n'),generate-id(.))" />
				<xsl:value-of select="concat(concat(./@id, $outEventPortId), ',')" />
			</xsl:for-each>
			
			<xsl:for-each select="./reference[@kind='InEventPort']">
				<xsl:variable name="inEventPortId" select="concat(concat($componentId,'::n'),generate-id(.))" />
				<xsl:value-of select="concat(concat(./@id, $inEventPortId), ',')" />
			</xsl:for-each>
			
		</xsl:for-each>
		
	</xsl:template>

	<xsl:template name="nodeXYKeyData">
		<xsl:param name="node" />

		<xsl:variable name="xValue" select="substring-before($node/value, ',')" />
		<xsl:choose>
			<xsl:when test="string(number($xValue)) != 'NaN'">
				<data key="{$nodeXKey}"><xsl:value-of select="$xValue" /></data>
			</xsl:when>
			<xsl:otherwise>
				<data key="{$nodeXKey}">0</data>
			</xsl:otherwise>
		</xsl:choose>

		<xsl:variable name="yValue" select="substring-after($node/value, ',')" />
		<xsl:choose>
			<xsl:when test="string(number($yValue)) != 'NaN'">
				<data key="{$nodeYKey}"><xsl:value-of select="$yValue" /></data>
			</xsl:when>
			<xsl:otherwise>
				<data key="{$nodeYKey}">0</data>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>
	
	<!-- Split delimited list and return required item, idx from 0 to count-1 -->
	<xsl:template name="splitList">
		<xsl:param name="pText" />
		<xsl:param name="count" />
		<xsl:param name="idx" />
		<xsl:param name="delim" />
		
		<xsl:if test="string-length($pText) > 0">
			<xsl:variable name="pCount" select="string-length($pText) - string-length(translate($pText, $delim, ''))" />
			
			<xsl:choose>
				<xsl:when test="($count - $pCount) = $idx" >
					<xsl:value-of select="substring-before(concat($pText, $delim), $delim)"/>
				</xsl:when>
				<xsl:otherwise>
						
					<xsl:call-template name="splitList">
						<xsl:with-param name="pText" select="substring-after($pText, $delim)"/>
						<xsl:with-param name="count" select="$count" />
						<xsl:with-param name="idx" select="$idx" />
						<xsl:with-param name="delim" select="$delim" />
					</xsl:call-template> 
				</xsl:otherwise>
			</xsl:choose>
		</xsl:if>
	</xsl:template>

</xsl:stylesheet>