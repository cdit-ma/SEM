<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:gml="http://graphml.graphdrawing.org/xmlns"

	xmlns:exsl="http://exslt.org/common"
	xmlns:xalan="http://xml.apache.org/xslt"
	xmlns:xmi="http://www.omg.org/XMI" 
	
	xmlns="http://www.cs.iupui.edu/CUTS/schemas" 
	xmlns:cuts="http://www.cs.iupui.edu/CUTS/schemas" 
	xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" 
	xsi:schemaLocation="http://www.cs.iupui.edu/CUTS/schemas cutsnode.xsd"
	
	exclude-result-prefixes="gml exsl xalan">
		
    <xsl:output method="xml" 
        version="1.0" 
        indent="yes" 
		standalone="no"
		xalan:indent-amount="4"/>
	<xsl:strip-space elements="*" />
	
    <!--
        Purpose:

        This template will convert the .graphml project to a deploy.config 
        file. The transform creates a xml file used for system deployment.
    -->
	
	<!-- Runtime parameters -->
	<xsl:param name="MainNode">0</xsl:param>
	<xsl:param name="HardwareNode"></xsl:param>
	<xsl:param name="File"></xsl:param>

	<!-- Assign default data keys as used by yEd 3.12.2 -->
	<xsl:include href="graphmlKeyVariables.xsl" />
	
	<!-- apply all templates starting from the root -->
    <xsl:template match="/">
	    <!-- Output start of cuts:test -->
		<cuts:node xmlns="http://www.cs.iupui.edu/CUTS/schemas" xmlns:cuts="http://www.cs.iupui.edu/CUTS/schemas" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:schemaLocation="http://www.cs.iupui.edu/CUTS/schemas cutsnode.xsd">
		<environment active="true" id="default" inherit="true">

		<!-- Output all nodes from original graphml, inserting yEd specific formatting -->		
		<xsl:apply-templates select="gml:graphml" />
			
		<!-- Output end of cuts:test -->
		</environment>
		</cuts:node>
    </xsl:template>
 
	<xsl:template match="gml:graphml">
		<!-- Assign the transform node keys for Label and Kind from existing keys -->
		
		<xsl:variable name="labelnodeFound" select="./gml:key[@attr.name='label'][@for='node']" />
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

		<xsl:variable name="typenodeFound" select="./gml:key[@attr.name='type'][@for='node']" />
		<xsl:variable name="transformNodeTypeKey">
			<xsl:choose>
				<xsl:when test="not($typenodeFound)">
					<xsl:value-of select="$nodeTypeKey"/>
				</xsl:when>
				<xsl:otherwise>
					<xsl:value-of select="$typenodeFound/@id"/>
				</xsl:otherwise>
			</xsl:choose>
		</xsl:variable>
		
		<xsl:variable name="kindnodeFound" select="./gml:key[@attr.name='kind'][@for='node']" />
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
	
		<xsl:variable name="ipnodeFound" select="./gml:key[@attr.name='ip_address'][@for='node']" />
		<xsl:variable name="transformNodeIp_addressKey">
			<xsl:choose>
				<xsl:when test="not($ipnodeFound)">
					<xsl:value-of select="$nodeIp_addressKey"/>
				</xsl:when>
				<xsl:otherwise>
					<xsl:value-of select="$ipnodeFound/@id"/>
				</xsl:otherwise>
			</xsl:choose>
		</xsl:variable>		

		<xsl:variable name="sharednodeFound" select="./gml:key[@attr.name='shared_directory'][@for='node']" />
		<xsl:variable name="transformNodeShared_directoryKey">
			<xsl:choose>
				<xsl:when test="not($sharednodeFound)">
					<xsl:value-of select="$nodeShared_directoryKey"/>
				</xsl:when>
				<xsl:otherwise>
					<xsl:value-of select="$sharednodeFound/@id"/>
				</xsl:otherwise>
			</xsl:choose>
		</xsl:variable>	
		
		<xsl:variable name="middlewarenodeFound" select="./gml:key[@attr.name='middleware'][@for='node']" />
		<xsl:variable name="transformNodeMiddlewareKey">
			<xsl:choose>
				<xsl:when test="not($middlewarenodeFound)">
					<xsl:value-of select="$nodeMiddlewareKey"/>
				</xsl:when>
				<xsl:otherwise>
					<xsl:value-of select="$middlewarenodeFound/@id"/>
				</xsl:otherwise>
			</xsl:choose>
		</xsl:variable>
		
		<!-- Use top level Model for project information if no File is given -->
		<xsl:variable name="fileName">
			<xsl:choose>
			<xsl:when test="$File = ''">
				<xsl:value-of select="./descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'Model']/../gml:data[@key=$transformNodeLabelKey]/text()" />
			</xsl:when>
			<xsl:when test="contains($File, '.')">
				<xsl:value-of select="substring-before($File, '.')" />
			</xsl:when>			
			<xsl:otherwise>
				<xsl:value-of select="$File" />
			</xsl:otherwise>
			</xsl:choose>
		</xsl:variable>

		<!-- Find middleware used for this model/project -->
		<xsl:variable name="middleware" select="/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'Model']/../gml:data[@key=$transformNodeMiddlewareKey]/text()" />

		<!-- find ManagementComponent to get ip address required -->
		<xsl:variable name="ManagementNodes" select="./descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'ManagementComponent']/.." />
		<xsl:variable name="DPLNode" select="$ManagementNodes/gml:data[@key=$transformNodeTypeKey][text() = 'DANCE_PLAN_LAUNCHER']/.." />
		<xsl:variable name="DPLHardwareNodeId" select="/descendant::*/gml:edge[@source = $DPLNode/@id]/@target" />
		<xsl:variable name="DPLHardwareNode" select="/descendant::*/gml:node[@id=$DPLHardwareNodeId]" />
		<xsl:variable name="DancePlanLauncherIP" select="$DPLHardwareNode/gml:data[@key=$transformNodeIp_addressKey]/text()" />
		<xsl:variable name="DEMNode" select="$ManagementNodes/gml:data[@key=$transformNodeTypeKey][text() = 'DANCE_EXECUTION_MANAGER']/.." />
		<xsl:variable name="DEMHardwareNodeId" select="/descendant::*/gml:edge[@source = $DEMNode/@id]/@target" />
		<xsl:variable name="DEMHardwareNode" select="/descendant::*/gml:node[@id=$DEMHardwareNodeId]" />
		<xsl:variable name="DanceExecutionManagerIP" select="$DEMHardwareNode/gml:data[@key=$transformNodeIp_addressKey]/text()" />
		<xsl:variable name="DLSNode" select="$ManagementNodes/gml:data[@key=$transformNodeTypeKey][text() = 'DDS_LOGGING_SERVER']/.." />
		<xsl:variable name="DLSHardwareNodeId" select="/descendant::*/gml:edge[@source = $DLSNode/@id]/@target" />
		<xsl:variable name="DLSHardwareNode" select="/descendant::*/gml:node[@id=$DLSHardwareNodeId]" />
		<xsl:variable name="DDSLoggingServerIP" select="$DLSHardwareNode/gml:data[@key=$transformNodeIp_addressKey]/text()" />
		<xsl:variable name="QBNode" select="$ManagementNodes/gml:data[@key=$transformNodeTypeKey][text() = 'QPID_BROKER']/.." />
		<xsl:variable name="QBHardwareNodeId" select="/descendant::*/gml:edge[@source = $QBNode/@id]/@target" />
		<xsl:variable name="QBHardwareNode" select="/descendant::*/gml:node[@id=$QBHardwareNodeId]" />
		<xsl:variable name="QpidBrokerIP" select="$QBHardwareNode/gml:data[@key=$transformNodeIp_addressKey]/text()" />

		<!--Using same hardware node for DancePlanLauncher and CIAOLoggingServer, may change in future ??? -->
		<xsl:variable name="CIAOLoggingServerIP" select="$DancePlanLauncherIP" />
		<xsl:variable name="ShareFolder">
			<xsl:choose>
			<xsl:when test="$MainNode != 0">
				<xsl:value-of select="''" />
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="$DPLHardwareNode/gml:data[@key=$transformNodeShared_directoryKey]/text()" />
			</xsl:otherwise>
			</xsl:choose>
		</xsl:variable>
		<xsl:variable name="HardwareNodes" select="/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'HardwareNode']/.." />
		<xsl:variable name="thisHardwareNode" select="$HardwareNodes/gml:data[@key=$transformNodeLabelKey][text() = $HardwareNode]/.." />
		<xsl:variable name="thisHardwareNodeIP" select="$thisHardwareNode/gml:data[@key=$transformNodeIp_addressKey]/text()" />
		<xsl:variable name="thisHardwarePosition">
			<xsl:for-each select="$HardwareNodes" >
				<xsl:if test="./@id = $thisHardwareNode/@id" >
					<xsl:value-of select="position()" />
				</xsl:if>
			</xsl:for-each>
		</xsl:variable>
		<xsl:variable name="thisHardwarePort" select="60000 + number($thisHardwarePosition)" />

		<xsl:choose>
		<xsl:when test="$MainNode = 0 and $thisHardwareNode and $thisHardwareNodeIP != ''" >
			<xsl:call-template name="SpecificHardwareNode">
				<xsl:with-param name="fileName" select="$fileName" />
				<xsl:with-param name="middleware" select="$middleware" />
				<xsl:with-param name="thisName" select="$HardwareNode" />
				<xsl:with-param name="thisIP" select="$thisHardwareNodeIP" />
				<xsl:with-param name="thisPort" select="$thisHardwarePort" />
				<xsl:with-param name="ShareFolder" select="$ShareFolder" />
				<xsl:with-param name="DancePlanLauncherIP" select="$DancePlanLauncherIP" />
				<xsl:with-param name="DanceExecutionManagerIP" select="$DanceExecutionManagerIP" />
				<xsl:with-param name="DDSLoggingServerIP" select="$DDSLoggingServerIP" />
				<xsl:with-param name="CIAOLoggingServerIP" select="$CIAOLoggingServerIP" />
				<xsl:with-param name="QpidBrokerIP" select="$QpidBrokerIP" />
			</xsl:call-template>	
		</xsl:when>
		<xsl:otherwise>
			<xsl:call-template name="DefaultHardwareNode">
				<xsl:with-param name="fileName" select="$fileName" />
				<xsl:with-param name="middleware" select="$middleware" />
				<xsl:with-param name="ShareFolder" select="$ShareFolder" />
			</xsl:call-template>		

		</xsl:otherwise>
		</xsl:choose>
	
    </xsl:template>

	<!-- Output HardwareNode specific config -->	
	<xsl:template name="SpecificHardwareNode">
		<xsl:param name="fileName"/>
		<xsl:param name="middleware"/>
		<xsl:param name="thisName"/>
		<xsl:param name="thisIP"/>
		<xsl:param name="thisPort"/>
		<xsl:param name="ShareFolder"/>
		<xsl:param name="DancePlanLauncherIP" />
		<xsl:param name="DanceExecutionManagerIP" />
		<xsl:param name="DDSLoggingServerIP" />
		<xsl:param name="CIAOLoggingServerIP" />
		<xsl:param name="QpidBrokerIP" />
		
		<startup>
			<xsl:if test="$thisIP = $CIAOLoggingServerIP" >
				<xsl:call-template name="CIAOLoggingServer">
					<xsl:with-param name="CIAOLoggingServerIP" select="$CIAOLoggingServerIP" />
				</xsl:call-template>	
			</xsl:if>
		  <process delay="10.0" id="cuts.logging.client" waitforcompletion="false">
			<executable>${CUTS_ROOT}/bin/cuts-logging-client</executable>
			<arguments>
				<xsl:value-of select="'--register-with-iortable=LoggingClient -ORBEndpoint iiop://localhost:50000 -v --debug '" />
				<xsl:value-of select="concat('-ORBInitRef LoggingServer=corbaloc:iiop:', $CIAOLoggingServerIP, ':50001/LoggingServer')" />
			</arguments>
		  </process>
		  <process id="dance.nodemanager.{$thisName}" waitforcompletion="false">
			<executable>${DANCE_ROOT}/bin/dance_node_manager</executable>
			<arguments>
				<xsl:value-of select="concat('-ORBEndpoint iiop://', $thisIP, ':', $thisPort, ' -s ${DANCE_ROOT}/bin/dance_locality_manager ')" />
				<xsl:value-of select="concat('-n ', $thisName, '=', $ShareFolder, $fileName, '_', $thisName, '.ior -t 10 --locality-config ${CUTS_ROOT}/bin/handlers/', $middleware, '.locality.config')" />
			</arguments>
			<workingdirectory>../lib</workingdirectory>
		  </process>
			<xsl:if test="$thisIP = $DanceExecutionManagerIP" >
				<xsl:call-template name="DanceExecutionManager">
					<xsl:with-param name="fileName" select="$fileName" />
					<xsl:with-param name="ShareFolder" select="$ShareFolder" />
				</xsl:call-template>	
			</xsl:if>
			<xsl:if test="$middleware = 'qpidpb'">
				<xsl:if test="$thisIP = $QpidBrokerIP" >
					<xsl:call-template name="QpidBroker" />
				</xsl:if>
			</xsl:if>
		</startup>

    </xsl:template>
	
	<!-- Output logging server process -->	
	<xsl:template name="CIAOLoggingServer">
		<xsl:param name="CIAOLoggingServerIP" />
		
		<process id="cuts.logging.server" waitforcompletion="false">
			<executable>${CUTS_ROOT}/bin/cuts-logging-server</executable>
			<arguments>
				<xsl:value-of select="concat('--register-with-iortable=LoggingServer -ORBEndpoint iiop://', $CIAOLoggingServerIP, ':50001 -v --debug')" />
			</arguments>
		</process>
    </xsl:template>

	<!-- Output execution manager -->	
	<xsl:template name="DanceExecutionManager">
		<xsl:param name="fileName"/>
		<xsl:param name="ShareFolder"/>
		
		<process id="dance.executionmanager" waitforcompletion="false">
			<executable>${DANCE_ROOT}/bin/dance_execution_manager</executable>
			<arguments>
				<xsl:value-of select="concat('-e', $ShareFolder, $fileName, '_EM.ior --cdd ', $fileName, '.cdd')" />
			</arguments>
		</process>
    </xsl:template>

	<!-- Output qpid broker -->	
	<xsl:template name="QpidBroker">
		<process id="qpid.broker" waitforcompletion="false">
			<executable>${QPID_ROOT}/bin/qpidd</executable>
			<arguments>--no-data-dir --auth no</arguments>
		</process>
    </xsl:template>
	
	<!-- Output Default hardware node -->	
	<xsl:template name="DefaultHardwareNode">
		<xsl:param name="fileName"/>
		<xsl:param name="middleware"/>
		<xsl:param name="ShareFolder"/>
		
		<startup>
		  <process id="cuts.logging.server" waitforcompletion="false">
			<executable>${CUTS_ROOT}/bin/cuts-logging-server</executable>
			<arguments>--register-with-iortable=LoggingServer -ORBEndpoint iiop://localhost:50001 -v --debug</arguments>
		  </process>
		  <process delay="2.0" id="cuts.logging.client" waitforcompletion="false">
			<executable>${CUTS_ROOT}/bin/cuts-logging-client</executable>
			<arguments>--register-with-iortable=LoggingClient -ORBEndpoint iiop://localhost:50000 -v --debug -ORBInitRef LoggingServer=corbaloc:iiop:localhost:50001/LoggingServer</arguments>
		  </process>
		  <process id="dance.nodemanager.MainNode" waitforcompletion="false">
			<executable>${DANCE_ROOT}/bin/dance_node_manager</executable>
			<arguments>
				<xsl:value-of select="concat('-ORBEndpoint iiop://localhost:60001 -s ${DANCE_ROOT}/bin/dance_locality_manager -n MainNode=MainNode.ior -t 3 --locality-config ${CUTS_ROOT}/bin/handlers/', $middleware, '.locality.config')" />
			</arguments>
			<workingdirectory>../lib</workingdirectory>
		  </process>
		  <process id="dance.executionmanager" waitforcompletion="false">
			<executable>${DANCE_ROOT}/bin/dance_execution_manager</executable>
			<arguments>
				<xsl:value-of select="concat('-e', $ShareFolder, $fileName, '_EM.ior --cdd ', $fileName, '.cdd')" />
			</arguments>
		  </process>
		  <xsl:if test="$middleware = 'qpidpb'">
			<xsl:call-template name="QpidBroker" />
		  </xsl:if>
		</startup>

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