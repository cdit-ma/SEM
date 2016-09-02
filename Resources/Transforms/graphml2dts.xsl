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
		<DeploymentSave xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="DeploymentToolSaveSchema.xsd">
		
			<!-- Output all nodes from original graphml, inserting yEd specific formatting -->		
			<xsl:apply-templates select="gml:graphml" />
			
		<!-- Output end of Deployment:domain -->
		</DeploymentSave>
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
		
		<xsl:variable name="transformNodeMiddlewareKey">
			<xsl:call-template name="findNodeKey">
				<xsl:with-param name="attrName" select="'middleware'" />
				<xsl:with-param name="defaultId" select="$nodeMiddlewareKey" />
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
		
		<xsl:variable name="transformNodeWorkerKey">
			<xsl:call-template name="findNodeKey">
				<xsl:with-param name="attrName" select="'worker'" />
				<xsl:with-param name="defaultId" select="$nodeWorkerKey" />
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
		
		<xsl:variable name="transformNodeIp_addressKey">
			<xsl:call-template name="findNodeKey">
				<xsl:with-param name="attrName" select="'ip_address'" />
				<xsl:with-param name="defaultId" select="$nodeIp_addressKey" />
			</xsl:call-template>	
		</xsl:variable>	
		
		<xsl:variable name="transformNodeOsKey">
			<xsl:call-template name="findNodeKey">
				<xsl:with-param name="attrName" select="'os'" />
				<xsl:with-param name="defaultId" select="$nodeOsKey" />
			</xsl:call-template>	
		</xsl:variable>	

		<xsl:variable name="transformNodeArchitectureKey">
			<xsl:call-template name="findNodeKey">
				<xsl:with-param name="attrName" select="'architecture'" />
				<xsl:with-param name="defaultId" select="$nodeArchitectureKey" />
			</xsl:call-template>	
		</xsl:variable>	
		
		<xsl:variable name="transformNodeOs_versionKey">
			<xsl:call-template name="findNodeKey">
				<xsl:with-param name="attrName" select="'os_version'" />
				<xsl:with-param name="defaultId" select="$nodeOs_versionKey" />
			</xsl:call-template>	
		</xsl:variable>	
		
		<xsl:variable name="transformNodeShared_directoryKey">
			<xsl:call-template name="findNodeKey">
				<xsl:with-param name="attrName" select="'shared_directory'" />
				<xsl:with-param name="defaultId" select="$nodeShared_directoryKey" />
			</xsl:call-template>	
		</xsl:variable>	
		
		<!-- Use top level Model for project information, eg projectUUID for this deployment UUID -->
		<xsl:variable name="projectModel" select="./descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'Model']/.." />
		<xsl:variable name="UUID">
			<xsl:choose>
				<xsl:when test="$projectModel[1]" >
					<xsl:value-of select="$projectModel[1]/gml:data[@key=$transformNodeProjectUUIDKey]/text()" />
				</xsl:when>
				<xsl:otherwise>
					<xsl:value-of select="generate-id(.)" />
				</xsl:otherwise>
			</xsl:choose>
		</xsl:variable>
		<xsl:variable name="projectModelName" select="$projectModel[1]/gml:data[@key=$transformNodeLabelKey]/text()" />
		<xsl:variable name="projectMiddleware" select="$projectModel[1]/gml:data[@key=$transformNodeMiddlewareKey]/text()" />
		
		<!-- Use first ComponentAssembly name for this deployment -->
		<xsl:variable name="firstComponentAssembly" select="./descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'ComponentAssembly']/.." />
		<xsl:variable name="container" select="$firstComponentAssembly[1]/gml:data[@key=$transformNodeContainerKey]/text()" />
		<xsl:variable name="assembly" select="$firstComponentAssembly[1]/gml:data[@key=$transformNodeLabelKey]/text()" />

		<!-- dts project information -->
		<deploymentToolVersion>v1.6.2</deploymentToolVersion>
		<projectName><xsl:value-of select="$projectModelName"/></projectName>
		<deploymentName>vUAVdeployment</deploymentName>
		<userProjectName>vUAVdeployment</userProjectName>
		<projectAssemblyName><xsl:value-of select="$assembly"/></projectAssemblyName>
		<projectImplName><xsl:value-of select="$container"/></projectImplName>
		<projectMiddleware><xsl:value-of select="$projectMiddleware"/></projectMiddleware>
		
		<!-- Use first HardwareNode for all processes required for this deployment -->
		<xsl:variable name="firstHardwareNode" select="./descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'HardwareNode']/.." />
		<xsl:variable name="hardware" select="$firstHardwareNode[1]/gml:data[@key=$transformNodeLabelKey]/text()" />
		<xsl:choose>
			<xsl:when test="not($hardware)">
				<DaNCE_Execution_Manager_Node>MainNode</DaNCE_Execution_Manager_Node>
				<DaNCE_Plan_Launcher_Node>MainNode</DaNCE_Plan_Launcher_Node>
				<CUTS_CIAO_Logging_Server_Node>MainNode</CUTS_CIAO_Logging_Server_Node>
			</xsl:when>
			<xsl:otherwise>
				<DaNCE_Execution_Manager_Node><xsl:value-of select="$hardware"/></DaNCE_Execution_Manager_Node>
				<DaNCE_Plan_Launcher_Node><xsl:value-of select="$hardware"/></DaNCE_Plan_Launcher_Node>
				<CUTS_CIAO_Logging_Server_Node><xsl:value-of select="$hardware"/></CUTS_CIAO_Logging_Server_Node>
			</xsl:otherwise>
		</xsl:choose>
		
		<!-- instance -->
		<xsl:for-each select="./descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'ComponentInstance']/..">
		
			<xsl:call-template name="Instance"> 
				<xsl:with-param name="labelKey" select="$transformNodeLabelKey" />
				<xsl:with-param name="kindKey" select="$transformNodeKindKey" />
				<xsl:with-param name="typeKey" select="$transformNodeTypeKey" />
				<xsl:with-param name="valueKey" select="$transformNodeValueKey" />
				<xsl:with-param name="workerKey" select="$transformNodeWorkerKey" />
				<xsl:with-param name="componentInstanceUUIDKey" select="$transformNodeComponentInstanceUUIDKey" />
				<xsl:with-param name="componentUUIDKey" select="$transformNodeComponentUUIDKey" />
				<xsl:with-param name="implUUIDKey" select="$transformNodeImplUUIDKey" />
				<xsl:with-param name="svntUUIDKey" select="$transformNodeSvntUUIDKey" />
			</xsl:call-template> 	
		</xsl:for-each>
		
		<!-- Hardware nodes, as well as a default Hardware node used for any unassigned ComponentInstances -->
		<xsl:for-each select="./descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'HardwareNode']/.." >
			<xsl:call-template name="HardwareNode">
				<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey" />
				<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey" />
				<xsl:with-param name="transformNodeIp_addressKey" select="$transformNodeIp_addressKey" />
				<xsl:with-param name="transformNodeOsKey" select="$transformNodeOsKey" />
				<xsl:with-param name="transformNodeArchitectureKey" select="$transformNodeArchitectureKey" />
				<xsl:with-param name="transformNodeOs_versionKey" select="$transformNodeOs_versionKey" />
				<xsl:with-param name="transformNodeShared_directoryKey" select="$transformNodeShared_directoryKey" />
			</xsl:call-template>
		</xsl:for-each>
		<xsl:call-template name="DefaultHardwareNode" />
		
		<!-- DeploymentConnection -->
		<xsl:for-each select="./descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'ComponentInstance']/..">
		
			<xsl:call-template name="DeploymentConnection"> 
				<xsl:with-param name="labelKey" select="$transformNodeLabelKey" />
				<xsl:with-param name="kindKey" select="$transformNodeKindKey" />
			</xsl:call-template> 	
		</xsl:for-each>
		
		<!-- connection -->
		<xsl:for-each select="./descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'OutEventPortInstance']/..">

			<xsl:variable name="outEventPortId"><xsl:value-of select="./@id"/></xsl:variable>
			<xsl:variable name="outEventPortName"><xsl:value-of select="./gml:data[@key=$transformNodeLabelKey]/text()"/></xsl:variable>
			
			<xsl:call-template name="findConnections"> 
				<xsl:with-param name="outEventPortName" select="$outEventPortName" />
				<xsl:with-param name="outEventPortId" select="$outEventPortId" />
				<xsl:with-param name="labelKey" select="$transformNodeLabelKey" />
				<xsl:with-param name="kindKey" select="$transformNodeKindKey" />
				<xsl:with-param name="componentInstanceUUIDKey" select="$transformNodeComponentInstanceUUIDKey" />
			</xsl:call-template>			
			
		</xsl:for-each>
		
    </xsl:template>
	
		
	<!-- component instance links back to implementation and hardware deployment node -->
	<xsl:template name="Instance">
		<xsl:param name="labelKey" />
		<xsl:param name="kindKey" />
		<xsl:param name="typeKey" />
		<xsl:param name="valueKey" />
		<xsl:param name="workerKey" />
		<xsl:param name="componentInstanceUUIDKey" />
		<xsl:param name="componentUUIDKey" />
		<xsl:param name="implUUIDKey" />
		<xsl:param name="svntUUIDKey" />

		<xsl:variable name="label" select="./gml:data[@key=$labelKey]/text()" />
		<xsl:variable name="componentInstanceUUID" select="./gml:data[@key=$componentInstanceUUIDKey]/text()" />
		<xsl:variable name="componentInstanceId" select="./@id" />
		<xsl:variable name="componentId">
			<xsl:for-each select="/descendant::*/gml:edge[@source=$componentInstanceId]" >
				<xsl:variable name="targetId" select="./@target" />
				<xsl:variable name="targetNode" select="/descendant::*/gml:node[@id=$targetId]" />
				<xsl:if test="$targetNode/gml:data[@key=$kindKey][text() = 'Component']">
					<xsl:value-of select="$targetNode/@id" />  
				</xsl:if>
			</xsl:for-each>
		</xsl:variable>
		<xsl:variable name="componentNode" select="/descendant::*/gml:node[@id=$componentId]" />
		<xsl:variable name="componentUUID" select="$componentNode/gml:data[@key=$componentUUIDKey]/text()" />
		<xsl:variable name="componentLabel" select="$componentNode/gml:data[@key=$labelKey]/text()" />
		<xsl:variable name="componentFile" select="concat($componentNode/../../gml:data[@key=$labelKey]/text(), $svntSuffix)" />
		<xsl:variable name="svntUUID" select="$componentNode/gml:data[@key=$svntUUIDKey]/text()" />
		<xsl:variable name="implUUID" select="$componentNode/gml:data[@key=$implUUIDKey]/text()" />
		
		<component>
		<name><xsl:value-of select="$label"/></name>
		<attachedWorkers>
			<!-- need some way to return distinct worker names, somewhat difficult with vanilla XSLT -->
			<xsl:variable name="allWorkers">
				<xsl:for-each select="$componentNode/descendant::*/gml:data[@key=$workerKey]" > 
					<xsl:sort select="text()" order="ascending"/>
					<xsl:value-of select="concat(text(), ',')" />
				</xsl:for-each>
			</xsl:variable>
			<worker>DIG_LogMessage_Worker</worker>
		</attachedWorkers>
		<xsl:for-each select="./gml:graph/gml:node/gml:data[@key=$kindKey][text() = 'InEventPortInstance']/..">
			<inputAction>
				<inputActionName><xsl:value-of select="./gml:data[@key=$labelKey]/text()" /></inputActionName>
				<inputActionID><xsl:value-of select="./@id" /></inputActionID>
				<xposAssm>54</xposAssm>
				<yposAssm>134</yposAssm>
				<isLeft>1</isLeft>
			</inputAction>
		</xsl:for-each>
		<xsl:for-each select="./gml:graph/gml:node/gml:data[@key=$kindKey][text() = 'OutEventPortInstance']/..">
			<outputAction>
				<outputActionName><xsl:value-of select="./gml:data[@key=$labelKey]/text()" /></outputActionName>
				<outputActionID><xsl:value-of select="./@id" /></outputActionID>
				<xposAssm>141</xposAssm>
				<yposAssm>134</yposAssm>
				<isLeft>0</isLeft>
			</outputAction>
		</xsl:for-each>

		<xsl:for-each select="./gml:graph/gml:node/gml:data[@key=$kindKey][text() = 'AttributeInstance']/..">
			<xsl:call-template name="configProperty" > 
				<xsl:with-param name="name" select="./gml:data[@key=$labelKey]/text()" />
				<xsl:with-param name="type" select="./gml:data[@key=$typeKey]/text()" />
				<xsl:with-param name="value" select="./gml:data[@key=$valueKey]/text()" />
			</xsl:call-template>
		</xsl:for-each>
			
		<implName><xsl:value-of select="concat($componentLabel, $ImplSuffix)"/></implName>
		<implID><xsl:value-of select="$componentUUID"/></implID>
		<artifactName><xsl:value-of select="concat($componentLabel, 'Artifacts')"/></artifactName>
		<svntArtifactName><xsl:value-of select="concat($componentLabel, $svntSuffix)"/></svntArtifactName>
		<implArtifactName><xsl:value-of select="concat($componentLabel, $implSuffix)"/></implArtifactName>
		<svntArtifactEntryPoint><xsl:value-of select="concat(concat(concat('create_', $componentLabel), '_'), $ServantSuffix)"/></svntArtifactEntryPoint>
		<implArtifactEntryPoint><xsl:value-of select="concat(concat(concat('create_', $componentLabel), '_'), $ImplSuffix)"/></implArtifactEntryPoint>
		<svntArtifactUUID><xsl:value-of select="$svntUUID"/></svntArtifactUUID>
		<implArtifactUUID><xsl:value-of select="$implUUID"/></implArtifactUUID>
		<instanceAssemblyName><xsl:value-of select="$label"/></instanceAssemblyName>
		<instanceUUID><xsl:value-of select="$componentInstanceUUID"/></instanceUUID>
		<svntLocation><xsl:value-of select="$componentFile"/></svntLocation>
		<implLocation><xsl:value-of select="concat($componentLabel, $implSuffix)"/></implLocation>
		<referenceName><xsl:value-of select="$componentLabel"/></referenceName>
		
		<xposAssm>69</xposAssm>
		<yposAssm>134</yposAssm>
		<isMainItem>true</isMainItem>
		<xpos>527</xpos>
		<ypos>179</ypos>
		</component>
	</xsl:template>	

	<!-- search for HardwareNode associated with ComponentInstance or Parent Assembly -->
	<xsl:template name="searchHardwareNode">	
		<xsl:param name="labelKey" />	
		<xsl:param name="kindKey" />
		<xsl:param name="nextNode" />

		<xsl:variable name="foundHardware" >
			<xsl:if test="($nextNode/gml:data[@key=$kindKey][text() = 'ComponentInstance']) or ($nextNode/gml:data[@key=$kindKey][text() = 'ComponentAssembly'])" >
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
	
	
	<!-- Output each hardware node -->
	<xsl:template name="HardwareNode">
		<xsl:param name="transformNodeLabelKey" />
		<xsl:param name="transformNodeKindKey" />
		<xsl:param name="transformNodeIp_addressKey" />
		<xsl:param name="transformNodeOsKey" />
		<xsl:param name="transformNodeArchitectureKey" />
		<xsl:param name="transformNodeOs_versionKey" />
		<xsl:param name="transformNodeShared_directoryKey" />
		
		<xsl:variable name="label"><xsl:value-of select="gml:data[@key=$transformNodeLabelKey]"/></xsl:variable>
		<xsl:variable name="kind"><xsl:value-of select="gml:data[@key=$transformNodeKindKey]"/></xsl:variable>
		<xsl:variable name="ip"><xsl:value-of select="gml:data[@key=$transformNodeIp_addressKey]"/></xsl:variable>
		<xsl:variable name="os"><xsl:value-of select="gml:data[@key=$transformNodeOsKey]"/></xsl:variable>
		<xsl:variable name="arch"><xsl:value-of select="gml:data[@key=$transformNodeArchitectureKey]"/></xsl:variable>
		<xsl:variable name="osver"><xsl:value-of select="gml:data[@key=$transformNodeOs_versionKey]"/></xsl:variable>
		<xsl:variable name="sharedir"><xsl:value-of select="gml:data[@key=$transformNodeShared_directoryKey]"/></xsl:variable>
	
		<xsl:if test="$kind = 'HardwareNode'" >
			<node>
				<name><xsl:value-of select="$label" /></name>
				<address><xsl:value-of select="concat(concat($ip, ':'), 60000 + position() )" /></address>
				<label><xsl:value-of select="$label" /></label>
				<virtual>0</virtual>
				<os><xsl:value-of select="$os" /></os>
				<arch><xsl:value-of select="$arch" /></arch>
				<osver><xsl:value-of select="$osver" /></osver>
				<sharedir><xsl:value-of select="$sharedir" /></sharedir>

				<DIG_SystemMonitor_Client>False</DIG_SystemMonitor_Client>
				<isMainItem>true</isMainItem>
				<xpos>212</xpos>
				<ypos>78</ypos>
			</node>
		
		</xsl:if>
	</xsl:template>
	
	<!-- Output Default hardware node -->
	<xsl:template name="DefaultHardwareNode">
		<node>
			<name>MainNode</name>
			<address>localhost:60001</address>
			<label>MainNode</label>
			<virtual>0</virtual>
			<os>Linux</os>
			<arch>x64</arch>
			<osver>2.6.32-220.el6.x86_64</osver>
			<sharedir>/mnt/Jenkins/share/</sharedir>
			<DIG_SystemMonitor_Client>False</DIG_SystemMonitor_Client>
			<isMainItem>true</isMainItem>
			<xpos>212</xpos>
			<ypos>78</ypos>
		</node>		
    </xsl:template>
	
	
	<!-- component instance links back to implementation and hardware deployment node -->
	<xsl:template name="DeploymentConnection">
		<xsl:param name="labelKey" />
		<xsl:param name="kindKey" />

		<xsl:variable name="label" select="./gml:data[@key=$labelKey]/text()" />
		
		<xsl:variable name="hardwareNode">
			<xsl:call-template name="searchHardwareNode" > 
				<xsl:with-param name="labelKey" select="$labelKey" />
				<xsl:with-param name="kindKey" select="$kindKey" />
				<xsl:with-param name="nextNode" select="." />
			</xsl:call-template>
		</xsl:variable>		
		<connection>
			<src><xsl:value-of select="$label" /></src>
			<dst><xsl:value-of select="$hardwareNode" /></dst>
		</connection> 			
    </xsl:template>
	
	
	<!-- configProperty -->
	<xsl:template name="configProperty">	
		<xsl:param name="name" />	
		<xsl:param name="type" />
		<xsl:param name="value" />

		<attribute>
			<attribName><xsl:value-of select="$name"/></attribName>
			<attribType>
				<xsl:call-template name="dataTypeKind">
					<xsl:with-param name="type" select="$type" />
				</xsl:call-template>
			</attribType>
			<attribValue><xsl:value-of select="$value"/></attribValue>
		</attribute>
		
    </xsl:template>	
	
	<!-- data type kind -->
	<xsl:template name="dataTypeKind">
		<xsl:param name="type" />	
		
		<xsl:choose>
			<xsl:when test="$type = 'Boolean'"><xsl:value-of select="'boolean'"/></xsl:when>
			<xsl:when test="$type = 'Byte'"><xsl:value-of select="'octet'"/></xsl:when>
			<xsl:when test="$type = 'Char'"><xsl:value-of select="'char'"/></xsl:when>
			<xsl:when test="$type = 'WideChar'"><xsl:value-of select="'wchar'"/></xsl:when>
			<xsl:when test="$type = 'String'"><xsl:value-of select="'string'"/></xsl:when>
			<xsl:when test="$type = 'WideString'"><xsl:value-of select="'wstring'"/></xsl:when>
			<xsl:when test="$type = 'FloatNumber'"><xsl:value-of select="'float'"/></xsl:when>
			<xsl:when test="$type = 'DoubleNumber'"><xsl:value-of select="'double'"/></xsl:when>
			<xsl:when test="$type = 'LongDoubleNumber'"><xsl:value-of select="'longdouble'"/></xsl:when>
			<xsl:when test="$type = 'ShortInteger'"><xsl:value-of select="'short'"/></xsl:when>
			<xsl:when test="$type = 'LongInteger'"><xsl:value-of select="'long'"/></xsl:when>
			<xsl:when test="$type = 'LongLongInteger'"><xsl:value-of select="'longlong'"/></xsl:when>
			<xsl:when test="$type = 'UnsignedShortInteger'"><xsl:value-of select="'ushort'"/></xsl:when>
			<xsl:when test="$type = 'UnsignedLongInteger'"><xsl:value-of select="'ulong'"/></xsl:when>
			<xsl:when test="$type = 'UnsignedLongLongInteger'"><xsl:value-of select="'ulonglong'"/></xsl:when>
			<xsl:otherwise><xsl:value-of select="'string'"/></xsl:otherwise>
		</xsl:choose>
    </xsl:template>	
	
	
	<!-- find connections to InEventPort -->
	<xsl:template name="findConnections">
		<xsl:param name="outEventPortName" />
		<xsl:param name="outEventPortId" />
		<xsl:param name="labelKey" />	
		<xsl:param name="kindKey" />

		<!-- Need to recursively search the links until all InEventPortInstance are found -->
		<!-- Simple case of one OutEventPortInstance to many InEventPortInstance -->
		<xsl:for-each select="/descendant::*/gml:edge[@source=$outEventPortId]" >
			<xsl:variable name="targetId" select="./@target" />
			<xsl:variable name="targetNode" select="/descendant::*/gml:node[@id=$targetId]" />
			
			<xsl:choose>
				<xsl:when test="$targetNode/gml:data[@key=$kindKey][text() = 'InEventPortInstance']">
					<xsl:variable name="inEventPortId"><xsl:value-of select="$targetNode/@id"/></xsl:variable>
					<xsl:variable name="inEventPortName"><xsl:value-of select="$targetNode/gml:data[@key=$labelKey]/text()"/></xsl:variable>
				
					<xsl:call-template name="Connection"> 
						<xsl:with-param name="outEventPortName" select="$outEventPortName" />
						<xsl:with-param name="outEventPortId" select="$outEventPortId" />
						<xsl:with-param name="inEventPortName" select="$inEventPortName" />
						<xsl:with-param name="inEventPortId" select="$inEventPortId" />
					</xsl:call-template>
				</xsl:when>
				<xsl:otherwise>
					<xsl:call-template name="findConnections"> 
						<xsl:with-param name="outEventPortName" select="$outEventPortName" />
						<xsl:with-param name="outEventPortId" select="$targetId" />
						<xsl:with-param name="labelKey" select="$labelKey" />
						<xsl:with-param name="kindKey" select="$kindKey" />
					</xsl:call-template>			

				</xsl:otherwise>
			</xsl:choose>
		</xsl:for-each>
	</xsl:template>	
	
	<!-- connections -->
	<xsl:template name="Connection">
		<xsl:param name="outEventPortName" />
		<xsl:param name="outEventPortId" />
		<xsl:param name="inEventPortName" />
		<xsl:param name="inEventPortId" />
		
		<assemConnection>
			<srcName><xsl:value-of select="$outEventPortName"/></srcName>
			<srcID><xsl:value-of select="$outEventPortId"/></srcID>
			<dstName><xsl:value-of select="$inEventPortName"/></dstName>
			<dstID><xsl:value-of select="$inEventPortId"/></dstID>
		</assemConnection>
	
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