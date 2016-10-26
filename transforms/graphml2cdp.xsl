<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="2.0" 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:gml="http://graphml.graphdrawing.org/xmlns"

	xmlns:exsl="http://exslt.org/common"
	xmlns:xalan="http://xml.apache.org/xslt"	
	xmlns:Deployment="http://www.omg.org/Deployment" 
	xmlns:xmi="http://www.omg.org/XMI" 
	xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" 
	xsi:schemaLocation="http://www.omg.org/Deployment Deployment.xsd"
	
	exclude-result-prefixes="gml exsl xalan">
		
    <xsl:output method="xml" 
        version="1.0" 
        indent="yes" 
		standalone="no"
		xalan:indent-amount="4"/>
	<xsl:strip-space elements="*" />
	
    <!--
        Purpose:

        This template will convert the .graphml project to a .cdp 
        file. The transform creates a xml file used for system deployment.
    -->
	
	<!-- Runtime parameters -->
	<xsl:param name="MainNode">0</xsl:param>
	
	<!-- Assign default data keys as used by yEd 3.12.2 -->
	<xsl:include href="graphmlKeyVariables.xsl" />
	
	<!-- Assign global variables for deployment -->
	<xsl:variable name="implSuffix" select="'_impl'" />
	<xsl:variable name="svntSuffix" select="'_svnt'" />
	<xsl:variable name="ImplSuffix" select="'Impl'" />
	<xsl:variable name="ServantSuffix" select="'Servant'" />
	
	<!-- apply all templates starting from the root -->
    <xsl:template match="/">
	    <!-- Output start of Deployment:domain -->
		<Deployment:DeploymentPlan xmlns:Deployment="http://www.omg.org/Deployment" xmlns:xmi="http://www.omg.org/XMI" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:schemaLocation="http://www.omg.org/Deployment Deployment.xsd">
			
			<!-- Output all nodes from original graphml, inserting yEd specific formatting -->		
			<xsl:apply-templates select="gml:graphml" />
			
		<!-- Output end of Deployment:domain -->
		</Deployment:DeploymentPlan>
    </xsl:template>
 
	<xsl:template match="gml:graphml">
		<!-- Assign the transform node keys for Label and Kind from existing keys -->

		<xsl:variable name="transformNodeProjectUUIDKey">
			<xsl:call-template name="findNodeKey">
				<xsl:with-param name="attrName" select="'projectUUID'" />
				<xsl:with-param name="defaultId" select="$nodeProjectUUID" />
			</xsl:call-template>	
		</xsl:variable>	
		
		<xsl:variable name="transformNodeProjectKey">
			<xsl:call-template name="findNodeKey">
				<xsl:with-param name="attrName" select="'project'" />
				<xsl:with-param name="defaultId" select="$nodeProjectKey" />
			</xsl:call-template>	
		</xsl:variable>	
		
		<xsl:variable name="transformNodeContainerKey">
			<xsl:call-template name="findNodeKey">
				<xsl:with-param name="attrName" select="'container'" />
				<xsl:with-param name="defaultId" select="$nodeContainerKey" />
			</xsl:call-template>	
		</xsl:variable>	
		
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

		<xsl:variable name="transformNodeTypeKey">
			<xsl:call-template name="findNodeKey">
				<xsl:with-param name="attrName" select="'type'" />
				<xsl:with-param name="defaultId" select="$nodeTypeKey" />
			</xsl:call-template>	
		</xsl:variable>	

		<xsl:variable name="transformNodeValueKey">
			<xsl:call-template name="findNodeKey">
				<xsl:with-param name="attrName" select="'value'" />
				<xsl:with-param name="defaultId" select="$nodeValueKey" />
			</xsl:call-template>	
		</xsl:variable>	
		
		<xsl:variable name="transformNodeFileKey">
			<xsl:call-template name="findNodeKey">
				<xsl:with-param name="attrName" select="'file'" />
				<xsl:with-param name="defaultId" select="$nodeFileKey" />
			</xsl:call-template>	
		</xsl:variable>	
		
		<xsl:variable name="transformNodeComponentUUIDKey">
			<xsl:call-template name="findNodeKey">
				<xsl:with-param name="attrName" select="'componentUUID'" />
				<xsl:with-param name="defaultId" select="$nodeComponentUUID" />
			</xsl:call-template>	
		</xsl:variable>	
		
		<xsl:variable name="transformNodeImplUUIDKey">
			<xsl:call-template name="findNodeKey">
				<xsl:with-param name="attrName" select="'implUUID'" />
				<xsl:with-param name="defaultId" select="$nodeImplUUID" />
			</xsl:call-template>	
		</xsl:variable>	
		
		<xsl:variable name="transformNodeSvntUUIDKey">
			<xsl:call-template name="findNodeKey">
				<xsl:with-param name="attrName" select="'svntUUID'" />
				<xsl:with-param name="defaultId" select="$nodeSvntUUID" />
			</xsl:call-template>	
		</xsl:variable>	
		
		<xsl:variable name="transformNodeComponentInstanceUUIDKey">
			<xsl:call-template name="findNodeKey">
				<xsl:with-param name="attrName" select="'componentInstanceUUID'" />
				<xsl:with-param name="defaultId" select="$nodeComponentInstanceUUID" />
			</xsl:call-template>	
		</xsl:variable>	
		
		<xsl:variable name="transformNodeMiddlewareKey">
			<xsl:call-template name="findNodeKey">
				<xsl:with-param name="attrName" select="'middleware'" />
				<xsl:with-param name="defaultId" select="$nodeMiddlewareKey" />
			</xsl:call-template>	
		</xsl:variable>	
		
		<xsl:variable name="transformNodeIp_addressKey">
			<xsl:call-template name="findNodeKey">
				<xsl:with-param name="attrName" select="'ip_address'" />
				<xsl:with-param name="defaultId" select="$nodeIp_addressKey" />
			</xsl:call-template>	
		</xsl:variable>	
		
		<!-- Use top level Model for project information, eg projectUUID for this deployment UUID -->
		<xsl:variable name="projectModel" select="./descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'Model']/.." />
		<xsl:variable name="UUID">
			<xsl:choose>
				<xsl:when test="normalize-space($projectModel[1]/gml:data[@key=$transformNodeProjectUUIDKey])" >
					<xsl:value-of select="$projectModel[1]/gml:data[@key=$transformNodeProjectUUIDKey]/text()" />
				</xsl:when>
				<xsl:otherwise>
					<xsl:value-of select="generate-id(.)" />
				</xsl:otherwise>
			</xsl:choose>
		</xsl:variable>		

		<xsl:variable name="projectModelName">
			<xsl:choose>
				<xsl:when test="$projectModel[1]" >
					<xsl:value-of select="$projectModel[1]/gml:data[@key=$transformNodeLabelKey]/text()" />
				</xsl:when>
				<xsl:otherwise>
					<xsl:value-of select="(Untitled)" />
				</xsl:otherwise>
			</xsl:choose>
		</xsl:variable>		

		<!-- Deployment UUID, given unique id of top level graph that must match the cdp file!!! -->
		<label>
			<!-- <xsl:value-of select="gml:graph/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'Component']" /> -->
		</label>	
		<UUID>
			<xsl:value-of select="$UUID"/>
		</UUID>
		
		<!-- implementation -->
		<xsl:for-each select="./descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'Component']/..
							| ./descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'BlackBox']/..">
			<xsl:call-template name="Implementation"> 
				<xsl:with-param name="project" select="$projectModelName" />
				<xsl:with-param name="labelKey" select="$transformNodeLabelKey" />
				<xsl:with-param name="componentUUIDKey" select="$transformNodeComponentUUIDKey" />
				<xsl:with-param name="implUUIDKey" select="$transformNodeImplUUIDKey" />
				<xsl:with-param name="svntUUIDKey" select="$transformNodeSvntUUIDKey" />
			</xsl:call-template> 	
		</xsl:for-each>
		
		<!-- locality manager implementation -->
		<xsl:call-template name="LMImplementation" />
		
		<!-- instance -->
		<xsl:for-each select="./descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'ComponentInstance']/..
							| ./descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'BlackBoxInstance']/..">
		
			<xsl:call-template name="Instance"> 
				<xsl:with-param name="project" select="$projectModelName" />
				<xsl:with-param name="containerKey" select="$transformNodeContainerKey" />
				<xsl:with-param name="labelKey" select="$transformNodeLabelKey" />
				<xsl:with-param name="kindKey" select="$transformNodeKindKey" />
				<xsl:with-param name="typeKey" select="$transformNodeTypeKey" />
				<xsl:with-param name="valueKey" select="$transformNodeValueKey" />
				<xsl:with-param name="componentInstanceUUIDKey" select="$transformNodeComponentInstanceUUIDKey" />
				<xsl:with-param name="componentUUIDKey" select="$transformNodeComponentUUIDKey" />
				<xsl:with-param name="middlewareKey" select="$transformNodeMiddlewareKey" />
				<xsl:with-param name="ip_addressKey" select="$transformNodeIp_addressKey" />

			</xsl:call-template> 	
		</xsl:for-each>
		
		<!-- DefaultGroupInstance -->
		<xsl:for-each select="./descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'HardwareNode']/..">
			
			<!-- Check if this Hardware node will be used in this deployment -->
			<xsl:variable name="nodeId" select="./@id"/>
			<xsl:if test="count(/descendant::*/gml:edge[@target=$nodeId]) > 0">
				<xsl:variable name="label"><xsl:value-of select="./gml:data[@key=$transformNodeLabelKey]"/></xsl:variable>
				<xsl:call-template name="DefaultGroupInstance"> 
					<xsl:with-param name="name" select="$label" />
				</xsl:call-template> 	
			</xsl:if>
		</xsl:for-each>
		
		<!-- Default MainNode, used if the ComponentInstance has not been assigned to a Hardware Node -->
		<xsl:if test="$MainNode != 0" >
			<xsl:call-template name="DefaultGroupInstance"> 
				<xsl:with-param name="name" select="'MainNode'" />
			</xsl:call-template> 
		</xsl:if>
		
		<!-- connection -->
		<xsl:for-each select="./descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'OutEventPortInstance']/..">

			<xsl:variable name="outEventPortId"><xsl:value-of select="./@id"/></xsl:variable>
			<xsl:variable name="outEventPortName"><xsl:value-of select="./gml:data[@key=$transformNodeLabelKey]/text()"/></xsl:variable>
			<xsl:variable name="outComponentInstanceUUID">
				<xsl:choose>
				<xsl:when test="normalize-space(../../gml:data[@key=$transformNodeComponentInstanceUUIDKey])">
					<xsl:value-of select="concat('_', ../../gml:data[@key=$transformNodeComponentInstanceUUIDKey]/text() )"/>
				</xsl:when>
				<xsl:otherwise> <!-- generate unique id from component instance node -->
					<xsl:value-of select="concat('_', generate-id(../..) )"/>
				</xsl:otherwise>
				</xsl:choose>
			</xsl:variable>
			<xsl:variable name="longOutInstName" >
				<xsl:call-template name="longInstName" > 
					<xsl:with-param name="labelKey" select="$transformNodeLabelKey" />
					<xsl:with-param name="kindKey" select="$transformNodeKindKey" />
					<xsl:with-param name="nextNode" select="." />
				</xsl:call-template>
			</xsl:variable>
			<!-- The parent Assembly should note the project and container to be consistent with legacy deployments -->
			<!-- <xsl:variable name="project" select="../../../../gml:data[@key=$transformNodeProjectKey]/text()" /> -->
			<xsl:variable name="container" select="../../../../gml:data[@key=$transformNodeContainerKey]/text()" />
			<xsl:variable name="outEventPortLongName" select="concat(concat(concat(concat(concat($projectModelName, '.ComponentImplementations.'), $container), $longOutInstName), '.'), $outEventPortName)" />
			
			<xsl:call-template name="findConnections"> 
				<xsl:with-param name="project" select="$projectModelName" />
				<xsl:with-param name="container" select="$container" />
				<xsl:with-param name="outEventPortLongName" select="$outEventPortLongName" />
				<xsl:with-param name="outEventPortName" select="$outEventPortName" />
				<xsl:with-param name="outComponentInstanceUUID" select="$outComponentInstanceUUID" />
				<xsl:with-param name="outEventPortId" select="$outEventPortId" />
				<xsl:with-param name="labelKey" select="$transformNodeLabelKey" />
				<xsl:with-param name="kindKey" select="$transformNodeKindKey" />
				<xsl:with-param name="componentInstanceUUIDKey" select="$transformNodeComponentInstanceUUIDKey" />
			</xsl:call-template>			
			
		</xsl:for-each>
		
		<!-- artifacts -->
		<xsl:for-each select="./descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'Component']/..
							| ./descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'BlackBox']/..">
			<xsl:call-template name="Artifacts"> 
				<xsl:with-param name="project" select="$projectModelName" />
				<xsl:with-param name="labelKey" select="$transformNodeLabelKey" />
				<xsl:with-param name="implUUIDKey" select="$transformNodeImplUUIDKey" />
				<xsl:with-param name="svntUUIDKey" select="$transformNodeSvntUUIDKey" />
			</xsl:call-template> 	
		</xsl:for-each>

		<!-- locality manager artifact -->
		<xsl:call-template name="LMArtifact" />
		
		<!-- localityConstraint -->
		<xsl:for-each select="./descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'HardwareNode']/..">

			<!-- Check if this Hardware node will be used in this deployment -->
			<xsl:variable name="nodeId" select="./@id"/>
			<xsl:if test="count(/descendant::*/gml:edge[@target=$nodeId]) > 0">
			
				<xsl:variable name="hardwareNode"><xsl:value-of select="./gml:data[@key=$transformNodeLabelKey]"/></xsl:variable>

				<localityConstraint>
				<constraint>SameProcess</constraint>
				<constrainedInstance xmi:idref="{$hardwareNode}.DefaultGroup"/>		
				
				<xsl:for-each select="/gml:graphml/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'ComponentInstance']/..
									| /gml:graphml/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'BlackBoxInstance']/..">
			
					<xsl:call-template name="LocalityConstraint"> 
						<xsl:with-param name="selectHardwareNode" select="$hardwareNode" />
						<xsl:with-param name="project" select="$projectModelName" />
						<xsl:with-param name="containerKey" select="$transformNodeContainerKey" />
						<xsl:with-param name="labelKey" select="$transformNodeLabelKey" />
						<xsl:with-param name="kindKey" select="$transformNodeKindKey" />
						<xsl:with-param name="componentInstanceUUIDKey" select="$transformNodeComponentInstanceUUIDKey" />
					</xsl:call-template> 	
				</xsl:for-each>
				
				</localityConstraint>
			</xsl:if>
		</xsl:for-each>
		<!-- Check for any unassigned ComponentInstance nodes -->
		<xsl:if test="$MainNode != 0" >
			<xsl:variable name="hardwareNode"><xsl:value-of select="'MainNode'"/></xsl:variable>

			<localityConstraint>
			<constraint>SameProcess</constraint>
			<constrainedInstance xmi:idref="{$hardwareNode}.DefaultGroup"/>		
			
			<xsl:for-each select="/gml:graphml/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'ComponentInstance']/..
								| /gml:graphml/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'BlackBoxInstance']/..">
		
				<xsl:call-template name="LocalityConstraint"> 
					<xsl:with-param name="selectHardwareNode" select="$hardwareNode" />
					<xsl:with-param name="project" select="$projectModelName" />
					<xsl:with-param name="containerKey" select="$transformNodeContainerKey" />
					<xsl:with-param name="labelKey" select="$transformNodeLabelKey" />
					<xsl:with-param name="kindKey" select="$transformNodeKindKey" />
					<xsl:with-param name="componentInstanceUUIDKey" select="$transformNodeComponentInstanceUUIDKey" />
				</xsl:call-template> 	
			</xsl:for-each>
			</localityConstraint>
		</xsl:if>	
		
    </xsl:template>
	
	<!-- implementation -->
	<xsl:template name="Implementation" > 
		<xsl:param name="project" />
		<xsl:param name="labelKey" />
		<xsl:param name="componentUUIDKey" />
		<xsl:param name="implUUIDKey" />
		<xsl:param name="svntUUIDKey" />
			
			
		<!--- <xsl:variable name="project" select="../../gml:data[@key=$projectKey]/text()" /> -->
		<xsl:variable name="label" select="./gml:data[@key=$labelKey]/text()" />
		<xsl:variable name="componentUUID">
			<xsl:choose>
			<xsl:when test="normalize-space(./gml:data[@key=$componentUUIDKey])">
				<xsl:value-of select="concat('_', ./gml:data[@key=$componentUUIDKey]/text() )"/>
			</xsl:when>
			<xsl:otherwise> <!-- generate unique id from component node -->
				<xsl:value-of select="concat('_', generate-id(.), '-1')"/>
			</xsl:otherwise>
			</xsl:choose>
		</xsl:variable>
		<xsl:variable name="svntUUID">
			<xsl:choose>
			<xsl:when test="normalize-space(./gml:data[@key=$svntUUIDKey])">
				<xsl:value-of select="concat('_', ./gml:data[@key=$svntUUIDKey]/text() )"/>
			</xsl:when>
			<xsl:otherwise> <!-- generate unique id from component node -->
				<xsl:value-of select="concat('_', generate-id(.), '-2')"/>
			</xsl:otherwise>
			</xsl:choose>
		</xsl:variable>	
		<xsl:variable name="implUUID">
			<xsl:choose>
			<xsl:when test="normalize-space(./gml:data[@key=$implUUIDKey])">
				<xsl:value-of select="concat('_', ./gml:data[@key=$implUUIDKey]/text() )"/>
			</xsl:when>
			<xsl:otherwise> <!-- generate unique id from component node -->
				<xsl:value-of select="concat('_', generate-id(.), '-3')"/>
			</xsl:otherwise>
			</xsl:choose>
		</xsl:variable>		
		
		<implementation xmi:id="{$componentUUID}">
			
			<name>
				<xsl:value-of select="concat(concat(concat(concat(concat(concat($project, '.ComponentImplementations.'), $label), $ImplSuffix), '.'), $label), $ImplSuffix)" />
			</name>
			
			<source/>
			
			<artifact xmi:idref="{$svntUUID}"/>
			<artifact xmi:idref="{$implUUID}"/>
			
			<xsl:call-template name="execParameter"> 
				<xsl:with-param name="name" select="'component factory'" />
				<xsl:with-param name="string" select="concat(concat(concat('create_', $label), '_'), $ImplSuffix)" />
			</xsl:call-template> 	
			<xsl:call-template name="execParameter"> 
				<xsl:with-param name="name" select="'edu.vanderbilt.dre.CIAO.ExecutorArtifact'" />
				<xsl:with-param name="string" select="concat(concat(concat(concat(concat($project, '.ImplementationArtifacts.'), $label), 'Artifacts.'), $label), $implSuffix)" />
			</xsl:call-template> 	
			<xsl:call-template name="execParameter"> 
				<xsl:with-param name="name" select="'edu.vanderbilt.dre.CIAO.ServantEntrypoint'" />
				<xsl:with-param name="string" select="concat(concat(concat('create_', $label), '_'), $ServantSuffix)" />
			</xsl:call-template> 	
			<xsl:call-template name="execParameter"> 
				<xsl:with-param name="name" select="'edu.vanderbilt.dre.CIAO.ServantArtifact'" />
				<xsl:with-param name="string" select="concat(concat(concat(concat(concat($project, '.ImplementationArtifacts.'), $label), 'Artifacts.'), $label), $svntSuffix)" />
			</xsl:call-template> 	

		</implementation>
    
	</xsl:template>	
	
	<xsl:template name="execParameter" >
		<xsl:param name="name" />
		<xsl:param name="string" />
		
		<execParameter>
		  <name><xsl:value-of select="$name" /></name>
		  <value> 
			<type> <kind>tk_string</kind> </type>
			<value>
			  <string><xsl:value-of select="$string" /></string>
			</value>
		  </value>
		</execParameter>
	</xsl:template>		
	
	<!-- Locality Manager implementation -->
	<xsl:template name="LMImplementation" > 
		<!-- Where to get UUID from ??? -->
		<implementation xmi:id="_1C073253-AACD-4130-B0A8-6AED69931E25_">
			<name>DAnCE_LocalityManager_Impl</name>
			<source/>
			<artifact xmi:idref="_2DBE7BA9-5DB7-48e2-A260-D141CB66DEB1"/>
			<execParameter>
			  <name>edu.vanderbilt.dre.DAnCE.ImplementationType</name>
			  <value>
				<type>
				  <kind>tk_string</kind>
				</type>
				<value>
				  <string>edu.vanderbilt.dre.DAnCE.LocalityManager</string>
				</value>
			  </value>
			</execParameter>
		</implementation>
		
	</xsl:template>	
		
	<!-- component instance links back to implementation and hardware deployment node -->
	<xsl:template name="Instance">
		<xsl:param name="project" />
		<xsl:param name="containerKey" />
		<xsl:param name="labelKey" />
		<xsl:param name="kindKey" />
		<xsl:param name="typeKey" />
		<xsl:param name="valueKey" />
		<xsl:param name="componentInstanceUUIDKey" />
		<xsl:param name="componentUUIDKey" />
		<xsl:param name="middlewareKey" />
		<xsl:param name="ip_addressKey" />

                <xsl:variable name="componentInstanceID" select="@id" />


		<!-- Containing Assembly should contain project and container -->
		<!-- <xsl:variable name="project" select="../../gml:data[@key=$projectKey]/text()" /> -->
		<xsl:variable name="container" select="../../gml:data[@key=$containerKey]/text()" />
		
		<xsl:variable name="hardwareNode">
			<xsl:call-template name="searchHardwareNode" > 
				<xsl:with-param name="labelKey" select="$labelKey" />
				<xsl:with-param name="kindKey" select="$kindKey" />
				<xsl:with-param name="nextNode" select="." />
			</xsl:call-template>
		</xsl:variable>		
		
		<!-- If MainNode is to be used as default, output this instance otherwise this componentInstance has not been deployed -->
		<xsl:if test="$hardwareNode != 'MainNode' or $MainNode != 0" >
			<xsl:variable name="componentInstanceUUID">
				<xsl:choose>
				<xsl:when test="normalize-space(./gml:data[@key=$componentInstanceUUIDKey])">
					<xsl:value-of select="concat('_', ./gml:data[@key=$componentInstanceUUIDKey]/text() )"/>
				</xsl:when>
				<xsl:otherwise> <!-- generate unique id from component instance node -->
					<xsl:value-of select="concat('_', generate-id(.) )"/>
				</xsl:otherwise>
				</xsl:choose>
			</xsl:variable>

			<instance xmi:id="{$componentInstanceUUID}">
			
				<xsl:variable name="longInstName" >
					<xsl:call-template name="longInstName" > 
						<xsl:with-param name="labelKey" select="$labelKey" />
						<xsl:with-param name="kindKey" select="$kindKey" />
						<xsl:with-param name="nextNode" select="." />
					</xsl:call-template>
				</xsl:variable>
				<xsl:variable name="fullInstName" select="concat(concat(concat($project, '.ComponentImplementations.'), $container), $longInstName)" />
				<name><xsl:value-of select="$fullInstName"/></name>
				
				<node>
					<xsl:value-of select="$hardwareNode" />
				</node>
				
				<source/>
				
				<xsl:variable name="componentInstanceId" select="./@id" />			
				<xsl:variable name="componentUUID">
					<xsl:for-each select="/descendant::*/gml:edge[@source=$componentInstanceId]" >
						<xsl:variable name="targetId" select="./@target" />
						<xsl:variable name="targetNode" select="/descendant::*/gml:node[@id=$targetId]" />
						<xsl:if test="$targetNode/gml:data[@key=$kindKey][text() = 'Component']
								   or $targetNode/gml:data[@key=$kindKey][text() = 'BlackBox']">
							<xsl:choose>
							<xsl:when test="normalize-space($targetNode/gml:data[@key=$componentUUIDKey])">
								<xsl:value-of select="concat('_', $targetNode/gml:data[@key=$componentUUIDKey]/text() )"/>
							</xsl:when>
							<xsl:otherwise> <!-- generate unique id from component node -->
								<xsl:value-of select="concat('_', generate-id($targetNode), '-1' )"/>
							</xsl:otherwise>
							</xsl:choose>
						</xsl:if>
					</xsl:for-each>
				</xsl:variable>
				<implementation xmi:idref="{$componentUUID}"/>
				
				<xsl:for-each select="./gml:graph/gml:node/gml:data[@key=$kindKey][text() = 'AttributeInstance']/..">
					<xsl:call-template name="configProperty" > 
						<xsl:with-param name="name" select="./gml:data[@key=$labelKey]/text()" />
						<xsl:with-param name="type" select="./gml:data[@key=$typeKey]/text()" />
						<xsl:with-param name="value" select="./gml:data[@key=$valueKey]/text()" />
					</xsl:call-template>
				</xsl:for-each>
				
                                <xsl:variable name="fileInstName" select="concat('ComponentInst_', concat($componentInstanceID, '%QoS.dpd'))" />
				<xsl:call-template name="participantQosConfigProperty" > 
                                        <xsl:with-param name="value" select="$fileInstName" />
				</xsl:call-template>
				
				<!-- if middleware is qpidpb and we have QPID_BROKER deployed to a HardwareNode -->
				<xsl:variable name="middleware" select="/descendant::*/gml:node/gml:data[@key=$kindKey][text() = 'Model']/../gml:data[@key=$middlewareKey]/text()" />
				<xsl:if test="$middleware = 'qpidpb'">
					<!-- find ManagementComponent to get ip address required -->
					<xsl:variable name="ManagementNodes" select="/descendant::*/gml:node/gml:data[@key=$kindKey][text() = 'ManagementComponent']/.." />
					<xsl:variable name="QBNode" select="$ManagementNodes/gml:data[@key=$typeKey][text() = 'QPID_BROKER']/.." />
					<xsl:variable name="QBHardwareNodeId" select="/descendant::*/gml:edge[@source = $QBNode/@id]/@target" />
					<xsl:variable name="QBHardwareNode" select="/descendant::*/gml:node[@id=$QBHardwareNodeId]" />
					<xsl:variable name="QpidBrokerIP" select="$QBHardwareNode/gml:data[@key=$ip_addressKey]/text()" />
					<xsl:call-template name="qpidBrokerConfigProperty" > 
						<xsl:with-param name="value" select="$QpidBrokerIP" />
					</xsl:call-template>
				</xsl:if>
			 </instance>
		 </xsl:if>
	</xsl:template>	

	<!-- search for HardwareNode associated with ComponentInstance or Parent Assembly -->
	<xsl:template name="searchHardwareNode">	
		<xsl:param name="labelKey" />	
		<xsl:param name="kindKey" />
		<xsl:param name="nextNode" />

		<xsl:variable name="foundHardware" >
			<xsl:if test="($nextNode/gml:data[@key=$kindKey][text() = 'ComponentInstance']) 
					   or ($nextNode/gml:data[@key=$kindKey][text() = 'BlackBoxInstance'])
					   or ($nextNode/gml:data[@key=$kindKey][text() = 'ComponentAssembly'])" >
				<xsl:variable name="componentInstanceId" select="$nextNode/@id" />
				<xsl:for-each select="/gml:graphml/gml:graph/gml:edge[@source=$componentInstanceId]" >
					<xsl:variable name="targetId" select="./@target" />
					<xsl:variable name="targetNode" select="/descendant::*/gml:node[@id=$targetId]" />
					<xsl:if test="$targetNode/gml:data[@key=$kindKey][text() = 'HardwareNode']">
						<xsl:value-of select="$targetNode/gml:data[@key=$labelKey]/text() " />  
					</xsl:if>
				</xsl:for-each>
			</xsl:if>	
		</xsl:variable>
		
		<xsl:choose>
			<xsl:when test="$foundHardware != ''" >
				<xsl:value-of select="$foundHardware" />
			</xsl:when>
			<xsl:when test="not(not($nextNode/parent::*))" >
				<xsl:call-template name="searchHardwareNode" > 
					<xsl:with-param name="labelKey" select="$labelKey" />
					<xsl:with-param name="kindKey" select="$kindKey" />
					<xsl:with-param name="nextNode" select="$nextNode/.." />
				</xsl:call-template>
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="'MainNode'" />
			</xsl:otherwise>
		</xsl:choose>
		
	</xsl:template>	
	
	<!-- assembly + component instance name -->
	<xsl:template name="longInstName">	
		<xsl:param name="labelKey" />	
		<xsl:param name="kindKey" />
		<xsl:param name="nextNode" />
		
		<xsl:if test="not(not($nextNode/parent::*))" >
			<xsl:call-template name="longInstName" > 
				<xsl:with-param name="labelKey" select="$labelKey" />
				<xsl:with-param name="kindKey" select="$kindKey" />
				<xsl:with-param name="nextNode" select="$nextNode/.." />
			</xsl:call-template>
		</xsl:if>
		
		<xsl:if test="($nextNode/gml:data[@key=$kindKey][text() = 'ComponentInstance']) 
				   or ($nextNode/gml:data[@key=$kindKey][text() = 'BlackBoxInstance'])
				   or ($nextNode/gml:data[@key=$kindKey][text() = 'ComponentAssembly'])" >
			<xsl:value-of select="concat('.', $nextNode/gml:data[@key=$labelKey]/text())" />
		</xsl:if>
	</xsl:template>	
	
	<!-- configProperty -->
	<xsl:template name="configProperty">	
		<xsl:param name="name" />	
		<xsl:param name="type" />
		<xsl:param name="value" />
		
		<configProperty>
		  <name><xsl:value-of select="$name"/></name>
		  <value>
			<type>
			  <kind>
				<xsl:call-template name="dataTypeKind">
					<xsl:with-param name="type" select="$type" />
				</xsl:call-template>
			  </kind>
			</type>
			<value>
				<xsl:call-template name="dataValueKind">
					<xsl:with-param name="type" select="$type" />
					<xsl:with-param name="value" select="$value" />
				</xsl:call-template>
			</value>
		  </value>
		</configProperty>
    </xsl:template>	

	<!-- ParticipantQoS configProperty -->
	<xsl:template name="participantQosConfigProperty">	
		<xsl:param name="value" />
		
		<configProperty>
		  <name>ParticipantQoS</name>
		  <value>
			<type>
			  <kind>tk_string</kind>
			</type>
			<value>
			  <string><xsl:value-of select="$value"/></string>
			</value>
		  </value>
		</configProperty>
    </xsl:template>

	<!-- Qpid Broker configProperty -->
	<xsl:template name="qpidBrokerConfigProperty">	
		<xsl:param name="value" />
		<!-- Currently the port is not assigned in the graphml, only have the IP address of where the QPID_BROKER ManagementComponent is deployed -->
		<configProperty>
		  <name>QPIDBroker</name>
		  <value>
			<type>
			  <kind>tk_string</kind>
			</type>
			<value>
			  <string><xsl:value-of select="concat($value,':5672')"/></string>
			</value>
		  </value>
		</configProperty>
		<!-- Also put in configProperty to ensure message isprivate topic -->
		<configProperty>
		  <name>QPIDUniqueQueues</name>
		  <value>
			<type>
			  <kind>tk_string</kind>
			</type>
			<value>
			  <string>true</string>
			</value>
		  </value>
		</configProperty>
    </xsl:template>
	
	<!-- data type kind -->
	<xsl:template name="dataTypeKind">
		<xsl:param name="type" />	
		
		<xsl:choose>
			<xsl:when test="$type = 'Boolean'"><xsl:value-of select="'tk_boolean'"/></xsl:when>
			<xsl:when test="$type = 'Byte'"><xsl:value-of select="'tk_octet'"/></xsl:when>
			<xsl:when test="$type = 'Char'"><xsl:value-of select="'tk_char'"/></xsl:when>
			<xsl:when test="$type = 'WideChar'"><xsl:value-of select="'tk_wchar'"/></xsl:when>
			<xsl:when test="$type = 'String'"><xsl:value-of select="'tk_string'"/></xsl:when>
			<xsl:when test="$type = 'WideString'"><xsl:value-of select="'tk_wstring'"/></xsl:when>
			<xsl:when test="$type = 'FloatNumber'"><xsl:value-of select="'tk_float'"/></xsl:when>
			<xsl:when test="$type = 'DoubleNumber'"><xsl:value-of select="'tk_double'"/></xsl:when>
			<xsl:when test="$type = 'LongDoubleNumber'"><xsl:value-of select="'tk_longdouble'"/></xsl:when>
			<xsl:when test="$type = 'ShortInteger'"><xsl:value-of select="'tk_short'"/></xsl:when>
			<xsl:when test="$type = 'LongInteger'"><xsl:value-of select="'tk_long'"/></xsl:when>
			<xsl:when test="$type = 'LongLongInteger'"><xsl:value-of select="'tk_longlong'"/></xsl:when>
			<xsl:when test="$type = 'UnsignedShortInteger'"><xsl:value-of select="'tk_ushort'"/></xsl:when>
			<xsl:when test="$type = 'UnsignedLongInteger'"><xsl:value-of select="'tk_ulong'"/></xsl:when>
			<xsl:when test="$type = 'UnsignedLongLongInteger'"><xsl:value-of select="'tk_ulonglong'"/></xsl:when>
			<xsl:otherwise><xsl:value-of select="'tk_string'"/></xsl:otherwise>
		</xsl:choose>
    </xsl:template>	

	<!-- data value and tags -->
	<xsl:template name="dataValueKind">	
		<xsl:param name="type" />	
		<xsl:param name="value" />	
	
		<xsl:choose>
			<xsl:when test="$type = 'Boolean'"><boolean><xsl:value-of select="$value"/></boolean></xsl:when>
			<xsl:when test="$type = 'Byte'"><octet><xsl:value-of select="$value"/></octet></xsl:when>
			<xsl:when test="$type = 'Char'"><char><xsl:value-of select="$value"/></char></xsl:when>
			<xsl:when test="$type = 'WideChar'"><wchar><xsl:value-of select="$value"/></wchar></xsl:when>
			<xsl:when test="$type = 'String'"><string><xsl:value-of select="$value"/></string></xsl:when>
			<xsl:when test="$type = 'WideString'"><wstring><xsl:value-of select="$value"/></wstring></xsl:when>
			<xsl:when test="$type = 'FloatNumber'"><float><xsl:value-of select="$value"/></float></xsl:when>
			<xsl:when test="$type = 'DoubleNumber'"><double><xsl:value-of select="$value"/></double></xsl:when>
			<xsl:when test="$type = 'LongDoubleNumber'"><longdouble><xsl:value-of select="$value"/></longdouble></xsl:when>
			<xsl:when test="$type = 'ShortInteger'"><short><xsl:value-of select="$value"/></short></xsl:when>
			<xsl:when test="$type = 'LongInteger'"><long><xsl:value-of select="$value"/></long></xsl:when>
			<xsl:when test="$type = 'LongLongInteger'"><longlong><xsl:value-of select="$value"/></longlong></xsl:when>
			<xsl:when test="$type = 'UnsignedShortInteger'"><ushort><xsl:value-of select="$value"/></ushort></xsl:when>
			<xsl:when test="$type = 'UnsignedLongInteger'"><ulong><xsl:value-of select="$value"/></ulong></xsl:when>
			<xsl:when test="$type = 'UnsignedLongLongInteger'"><ulonglong><xsl:value-of select="$value"/></ulonglong></xsl:when>
			<xsl:otherwise><string><xsl:value-of select="$value"/></string></xsl:otherwise>
		</xsl:choose>
    </xsl:template>	
	
	<!-- DefaultGroup instance -->
	<xsl:template name="DefaultGroupInstance">	
		<xsl:param name="name" />	
		
		<xsl:variable name="longGroupName" select="concat($name, '.DefaultGroup')" />
		<instance xmi:id="{$longGroupName}">
			<name><xsl:value-of select="$longGroupName"/></name>
			<node><xsl:value-of select="$name"/></node>
			<source/>
			<!-- Where to get UUID from ???, hardcoded to match LMimplementation -->
			<implementation xmi:idref="_1C073253-AACD-4130-B0A8-6AED69931E25_"/>
			<configProperty>
			  <name>edu.vanderbilt.dre.DAnCE.LocalityManager.ProcessName</name>
			  <value>
				<type>
				  <kind>tk_string</kind>
				</type>
				<value>
				  <string>DefaultGroup</string>
				</value>
			  </value>
			</configProperty>
			<configProperty>
			  <name>DomainQoS</name>
			  <value>
				<type>
				  <kind>tk_string</kind>
				</type>
				<value>
				  <xsl:variable name="GroupQoS" select="concat($name, '%QoS.ddd')" />
				  <string><xsl:value-of select="$GroupQoS"/></string>
				</value>
			  </value>
			</configProperty>
		</instance>
    </xsl:template>

	<!-- find connections to InEventPort -->
	<xsl:template name="findConnections">
		<xsl:param name="project" />
		<xsl:param name="container" />
		<xsl:param name="outEventPortLongName" />
		<xsl:param name="outEventPortName" />
		<xsl:param name="outComponentInstanceUUID" />
		<xsl:param name="outEventPortId" />
		<xsl:param name="labelKey" />	
		<xsl:param name="kindKey" />
		<xsl:param name="componentInstanceUUIDKey" />

		<!-- Need to recursively search the links until all InEventPortInstance are found -->
		<!-- Simple case of one OutEventPortInstance to many InEventPortInstance -->
		<xsl:for-each select="/descendant::*/gml:edge[@source=$outEventPortId]" >
			<xsl:variable name="targetId" select="./@target" />
			<xsl:variable name="targetNode" select="/descendant::*/gml:node[@id=$targetId]" />
			
			<xsl:choose>
				<xsl:when test="$targetNode/gml:data[@key=$kindKey][text() = 'InEventPortInstance']">
					<xsl:variable name="inEventPortId"><xsl:value-of select="$targetNode/@id"/></xsl:variable>
					<xsl:variable name="inEventPortName"><xsl:value-of select="$targetNode/gml:data[@key=$labelKey]/text()"/></xsl:variable>
					<xsl:variable name="inComponentInstanceUUID">
						<xsl:choose>
						<xsl:when test="normalize-space($targetNode/../../gml:data[@key=$componentInstanceUUIDKey])">
							<xsl:value-of select="concat('_', $targetNode/../../gml:data[@key=$componentInstanceUUIDKey]/text() )"/>
						</xsl:when>
						<xsl:otherwise> <!-- generate unique id from component instance node -->
							<xsl:value-of select="concat('_', generate-id($targetNode/../..) )"/>
						</xsl:otherwise>
						</xsl:choose>
					</xsl:variable>
					<xsl:variable name="longInInstName" >
						<xsl:call-template name="longInstName" > 
							<xsl:with-param name="labelKey" select="$labelKey" />
							<xsl:with-param name="kindKey" select="$kindKey" />
							<xsl:with-param name="nextNode" select="$targetNode" />
						</xsl:call-template>
					</xsl:variable>
					<xsl:variable name="inEventPortLongName" select="concat(concat(concat(concat(concat($project, '.ComponentImplementations.'), $container), $longInInstName), '.'), $inEventPortName)" />
				
					<xsl:call-template name="Connection"> 
						<xsl:with-param name="outEventPortLongName" select="$outEventPortLongName" />
						<xsl:with-param name="outEventPortName" select="$outEventPortName" />
						<xsl:with-param name="outComponentInstanceUUID" select="$outComponentInstanceUUID" />
						<xsl:with-param name="inEventPortLongName" select="$inEventPortLongName" />
						<xsl:with-param name="inEventPortName" select="$inEventPortName" />
						<xsl:with-param name="inComponentInstanceUUID" select="$inComponentInstanceUUID" />
					</xsl:call-template>
				</xsl:when>
				<xsl:otherwise>
					<xsl:call-template name="findConnections"> 
						<xsl:with-param name="project" select="$project" />
						<xsl:with-param name="container" select="$container" />
						<xsl:with-param name="outEventPortLongName" select="$outEventPortLongName" />
						<xsl:with-param name="outEventPortName" select="$outEventPortName" />
						<xsl:with-param name="outComponentInstanceUUID" select="$outComponentInstanceUUID" />
						<xsl:with-param name="outEventPortId" select="$targetId" />
						<xsl:with-param name="labelKey" select="$labelKey" />
						<xsl:with-param name="kindKey" select="$kindKey" />
						<xsl:with-param name="componentInstanceUUIDKey" select="$componentInstanceUUIDKey" />
					</xsl:call-template>			

				</xsl:otherwise>
			</xsl:choose>
		</xsl:for-each>
	</xsl:template>	
	
	<!-- connections -->
	<xsl:template name="Connection">
		<xsl:param name="outEventPortLongName" />
		<xsl:param name="outEventPortName" />
		<xsl:param name="outComponentInstanceUUID" />
		<xsl:param name="inEventPortLongName" />
		<xsl:param name="inEventPortName" />
		<xsl:param name="inComponentInstanceUUID" />

		<connection>
			<name><xsl:value-of select="concat(concat($outEventPortLongName, '::'), $inEventPortLongName)"/></name>
			<internalEndpoint>
			  <portName><xsl:value-of select="$outEventPortName"/></portName>
			  <provider>false</provider>
			  <kind>EventPublisher</kind>
			  <instance xmi:idref="{$outComponentInstanceUUID}"/>
			</internalEndpoint>
			<internalEndpoint>
			  <portName><xsl:value-of select="$inEventPortName"/></portName>
			  <provider>true</provider>
			  <kind>EventConsumer</kind>
			  <instance xmi:idref="{$inComponentInstanceUUID}"/>
			</internalEndpoint>
		</connection>
    </xsl:template>	

	<!-- artifacts associated with implementation -->
	<xsl:template name="Artifacts" > 
		<xsl:param name="project" />
		<xsl:param name="labelKey" />
		<xsl:param name="implUUIDKey" />
		<xsl:param name="svntUUIDKey" />
		
		<!-- <xsl:variable name="project" select="../../gml:data[@key=$projectKey]/text()" /> -->
		<xsl:variable name="label" select="./gml:data[@key=$labelKey]/text()" />
		<xsl:variable name="svntUUID">
			<xsl:choose>
			<xsl:when test="normalize-space(./gml:data[@key=$svntUUIDKey])">
				<xsl:value-of select="concat('_', ./gml:data[@key=$svntUUIDKey]/text() )"/>
			</xsl:when>
			<xsl:otherwise> <!-- generate unique id from component node -->
				<xsl:value-of select="concat('_', generate-id(.), '-2')"/>
			</xsl:otherwise>
			</xsl:choose>
		</xsl:variable>	
		<xsl:variable name="implUUID">
			<xsl:choose>
			<xsl:when test="normalize-space(./gml:data[@key=$implUUIDKey])">
				<xsl:value-of select="concat('_', ./gml:data[@key=$implUUIDKey]/text() )"/>
			</xsl:when>
			<xsl:otherwise> <!-- generate unique id from component node -->
				<xsl:value-of select="concat('_', generate-id(.), '-3')"/>
			</xsl:otherwise>
			</xsl:choose>
		</xsl:variable>		
		
		<artifact xmi:id="{$svntUUID}">
			<name>
				<xsl:value-of select="concat(concat(concat(concat(concat($project, '.ImplementationArtifacts.'), $label), 'Artifacts.'), $label), $svntSuffix)" />
			</name>
			<source/>
			<node/>
			<location>
				<xsl:value-of select="concat( ../../gml:data[@key=$labelKey]/text(), $svntSuffix)" />
			</location>
		</artifact>

		<artifact xmi:id="{$implUUID}">
			<name>
				<xsl:value-of select="concat(concat(concat(concat(concat($project, '.ImplementationArtifacts.'), $label), 'Artifacts.'), $label), $implSuffix)" />
			</name>
			<source/>
			<node/>
			<location>
				<xsl:value-of select="concat($label, $implSuffix)" />
			</location>
		</artifact> 
	</xsl:template>	

	<!-- locality manager artifact -->
	<xsl:template name="LMArtifact" > 
		<!-- Locality Manager artifact, where should we get a UUID? -->
		<artifact xmi:id="_2DBE7BA9-5DB7-48e2-A260-D141CB66DEB1">
			<name>DAnCE_LocalityManager_Artifact</name>
			<source/>
			<node/>
			<location>dance_locality_manager</location>
		</artifact>
	</xsl:template>	
	
	<!-- localityConstraint -->
	<xsl:template name="LocalityConstraint">
		<xsl:param name="selectHardwareNode" />
		<xsl:param name="project" />
		<xsl:param name="containerKey" />
		<xsl:param name="labelKey" />
		<xsl:param name="kindKey" />
		<xsl:param name="componentInstanceUUIDKey" />

		<!-- <xsl:variable name="project" select="../../gml:data[@key=$projectKey]/text()" /> -->
		<xsl:variable name="container" select="../../gml:data[@key=$containerKey]/text()" />
		<xsl:variable name="componentInstanceId" select="./@id" />		
		<xsl:variable name="hardwareNode">
			<xsl:call-template name="searchHardwareNode" > 
				<xsl:with-param name="labelKey" select="$labelKey" />
				<xsl:with-param name="kindKey" select="$kindKey" />
				<xsl:with-param name="nextNode" select="." />
			</xsl:call-template>
		</xsl:variable>			

		<xsl:if test="$hardwareNode = $selectHardwareNode" >
			<xsl:variable name="longInstName" >
				<xsl:call-template name="longInstName" > 
					<xsl:with-param name="labelKey" select="$labelKey" />
					<xsl:with-param name="kindKey" select="$kindKey" />
					<xsl:with-param name="nextNode" select="." />
				</xsl:call-template>
			</xsl:variable>
			<xsl:variable name="fullInstName" select="concat(concat(concat($project, '.ComponentImplementations.'), $container), $longInstName)" />
			<xsl:text disable-output-escaping="yes">&#10;&lt;!--</xsl:text><xsl:value-of select="$fullInstName" /><xsl:text disable-output-escaping="yes">--&gt;&#10;</xsl:text>

			<xsl:variable name="componentInstanceUUID">
				<xsl:choose>
				<xsl:when test="normalize-space(./gml:data[@key=$componentInstanceUUIDKey])">
					<xsl:value-of select="concat('_', ./gml:data[@key=$componentInstanceUUIDKey]/text() )"/>
				</xsl:when>
				<xsl:otherwise> <!-- generate unique id from component instance node -->
					<xsl:value-of select="concat('_', generate-id(.) )"/>
				</xsl:otherwise>
				</xsl:choose>
			</xsl:variable>

			<constrainedInstance xmi:idref="{$componentInstanceUUID}" />
		</xsl:if>
			
    </xsl:template>	
	
	<!-- find the key for specific attribute,  -->
	<xsl:template name="findNodeKey">
		<xsl:param name="attrName" />
		<xsl:param name="defaultId" />
		
		<xsl:variable name="found" select="./gml:key[@attr.name=$attrName]" />
		<xsl:choose>
			<xsl:when test="not($found)">
				<xsl:value-of select="$defaultId"/>
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="$found/@id"/>
			</xsl:otherwise>
		</xsl:choose>
    </xsl:template>	
		
	<!-- copy all existing node data -->
    <xsl:template match="gml:data" name="NodeData">
		<xsl:copy>
			<xsl:apply-templates select="@*|node()"/>
		</xsl:copy>
    </xsl:template>

	<xsl:template match="@*|node()">
	  <xsl:copy>
		<xsl:apply-templates select="@*|node()"/>
	  </xsl:copy>
	</xsl:template>

</xsl:stylesheet>
