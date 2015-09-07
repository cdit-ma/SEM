<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:gml="http://graphml.graphdrawing.org/xmlns"

	xmlns:exsl="http://exslt.org/common"
	xmlns:xalan="http://xml.apache.org/xslt"	
	xmlns:xmi="http://www.omg.org/XMI" 

	xmlns="http://www.dre.vanderbilt.edu/CUTS" 
	xmlns:cuts="http://www.dre.vanderbilt.edu/CUTS" 
	xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" 
	xsi:schemaLocation="http://www.dre.vanderbilt.edu/CUTS test.xsd"
	
	exclude-result-prefixes="gml exsl xalan">
		
    <xsl:output method="xml" 
        version="1.0" 
        indent="yes" 
		standalone="no"
		xalan:indent-amount="4"/>
	<xsl:strip-space elements="*" />
	
    <!--
        Purpose:

        This template will convert the .graphml project to a test.config 
        file. The transform creates a xml file used for system deployment.
    -->
	
	<!-- Runtime parameters -->
	<xsl:param name="MainNode">0</xsl:param>
	<xsl:param name="File"></xsl:param>
	
	<!-- Assign default data keys as used by yEd 3.12.2 -->
	<xsl:include href="graphmlKeyVariables.xsl" />
	
	<!-- apply all templates starting from the root -->
    <xsl:template match="/">
	    <!-- Output start of cuts:test -->
		<cuts:test xmlns="http://www.dre.vanderbilt.edu/CUTS" xmlns:cuts="http://www.dre.vanderbilt.edu/CUTS" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:schemaLocation="http://www.dre.vanderbilt.edu/CUTS test.xsd">

		<!-- Output all nodes from original graphml, inserting yEd specific formatting -->		
		<xsl:apply-templates select="gml:graphml" />
			
		<!-- Output end of cuts:test -->
		</cuts:test>
    </xsl:template>
 
	<xsl:template match="gml:graphml" >
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
		
		<!-- Use top level Model for project information if no File is given -->
		<xsl:variable name="ModelName">
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
		<!-- find ManagementComponent to get ip address for the DancePlanLauncher -->
		<xsl:variable name="ManagementNode" select="./descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'ManagementComponent']/../gml:data[@key=$transformNodeTypeKey][text() = 'DANCE_PLAN_LAUNCHER']/.." />
		<xsl:variable name="HardwareNodeId" select="/descendant::*/gml:edge[@source = $ManagementNode/@id]/@target" />
		<xsl:variable name="HardwareNode" select="/descendant::*/gml:node[@id=$HardwareNodeId]" />
		<xsl:variable name="DancePlanLauncherIP" select="$HardwareNode/gml:data[@key=$transformNodeIp_addressKey]/text()" />
		<xsl:variable name="ShareFolder">
			<xsl:choose>
			<xsl:when test="$MainNode != 0">
				<xsl:value-of select="''" />
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="$HardwareNode/gml:data[@key=$transformNodeShared_directoryKey]/text()" />
			</xsl:otherwise>
			</xsl:choose>
		</xsl:variable>
		<xsl:variable name="fileName">
			<xsl:choose>
			<xsl:when test="not($ShareFolder) or $ShareFolder = ''">
				<xsl:value-of select="$ModelName" />
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="concat($ShareFolder, '/', $ModelName)" />
			</xsl:otherwise>
			</xsl:choose>
		</xsl:variable>
		
		<startup>
			<executable>
			<xsl:value-of select="'${DANCE_ROOT}/bin/dance_plan_launcher'" /> 
			</executable>
			<arguments>
			<xsl:value-of select="concat('-k file://', $fileName, '_EM.ior -x ', $ModelName, '.cdp')" />
			</arguments>
		</startup>
		<shutdown>
			<executable>
			<xsl:value-of select="'${DANCE_ROOT}/bin/dance_plan_launcher'" /> 
			</executable>
			<arguments>
				<xsl:value-of select="concat('-k file://', $fileName, '_EM.ior -x ', $ModelName, '.cdp -s')" /> 
			</arguments>
		</shutdown>
	
		<xsl:choose>
		<xsl:when test="$MainNode = 0 and $DancePlanLauncherIP and $DancePlanLauncherIP != ''" >
			<xsl:call-template name="ManagementComponent">
				<xsl:with-param name="ip" select="$DancePlanLauncherIP" />
			</xsl:call-template>		
		</xsl:when>
		<xsl:otherwise>
			<xsl:call-template name="DefaultHardwareNode" />
		</xsl:otherwise>
		</xsl:choose>
	
    </xsl:template>

	<!-- Output each hardware node -->
	<xsl:template name="ManagementComponent">
		<xsl:param name="ip" />
		
		<services>
			<service id="service.logging">
				<location>CUTS_Testing_Log_Message_Listener</location>
				<entryPoint>_make_CUTS_Testing_Log_Message_Listener</entryPoint>
				<params>
					<xsl:value-of select="concat('-ORBInitRef LoggingServer=corbaloc:iiop:', $ip, ':50001/LoggingServer')" />
				</params>
			</service>
		</services>
    </xsl:template>

	<!-- Output Default hardware node -->	
	<xsl:template name="DefaultHardwareNode">
		<services>
			<service id="service.logging">
				<location>CUTS_Testing_Log_Message_Listener</location>
				<entryPoint>_make_CUTS_Testing_Log_Message_Listener</entryPoint>
				<params>-ORBInitRef LoggingServer=corbaloc:iiop:localhost:50001/LoggingServer</params>
			</service>
		</services>
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