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

        This template will convert the .graphml project to a .cdd 
        file. The transform creates a xml file used for system deployment.
    -->
	
	<!-- Runtime parameters -->
	<xsl:param name="MainNode">0</xsl:param>
	
	<!-- Assign default data keys as used by yEd 3.12.2 -->
	<xsl:include href="graphmlKeyVariables.xsl" />
	
	<!-- apply all templates starting from the root -->
    <xsl:template match="/">
	    <!-- Output start of Deployment:domain -->
		<Deployment:domain xmlns:Deployment="http://www.omg.org/Deployment" xmlns:xmi="http://www.omg.org/XMI" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:schemaLocation="http://www.omg.org/Deployment Deployment.xsd">
			
			<!-- Output all nodes from original graphml, inserting yEd specific formatting -->		
			<xsl:apply-templates select="gml:graphml" />
			
		<!-- Output end of Deployment:domain -->
		</Deployment:domain>
    </xsl:template>
 
	<xsl:template match="gml:graphml">
		<!-- Assign the transform node keys for Label and Kind from existing keys -->

		<xsl:variable name="projectUUIDFound" select="./gml:key[@attr.name='projectUUID'][@for='node']" />
		<xsl:variable name="transformNodeProjectUUIDKey">
			<xsl:choose>
				<xsl:when test="not($projectUUIDFound)">
					<xsl:value-of select="$nodeProjectUUID"/>
				</xsl:when>
				<xsl:otherwise>
					<xsl:value-of select="$projectUUIDFound/@id"/>
				</xsl:otherwise>
			</xsl:choose>
		</xsl:variable>	
		
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

		<!-- Use top level Model for project information, eg projectUUID for this deployment UUID -->
		<xsl:variable name="UUID">
			<xsl:variable name="projectModel" select="./descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'Model']/.." />
			<xsl:choose>
				<xsl:when test="$projectModel[1]" >
					<xsl:value-of select="$projectModel[1]/gml:data[@key=$transformNodeProjectUUIDKey]/text()" />
				</xsl:when>
				<xsl:otherwise>
					<xsl:value-of select="generate-id(.)" />
				</xsl:otherwise>
			</xsl:choose>
		</xsl:variable>

		<!-- Deployment UUID, given unique id of top level graph that must match the cdp file!!! -->
		<UUID>
			<xsl:value-of select="$UUID"/>
		</UUID>
		 
		<label>
		</label>	
		
		<!-- Hardware nodes, as well as a default Hardware node used for any unassigned ComponentInstances -->
		<xsl:for-each select="./descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'HardwareNode']/.." >
			<xsl:call-template name="HardwareNode">
				<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey" />
				<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey" />
				<xsl:with-param name="transformNodeIp_addressKey" select="$transformNodeIp_addressKey" />
			</xsl:call-template>
		</xsl:for-each>
		
		<xsl:if test="$MainNode != 0">
			<xsl:call-template name="DefaultHardwareNode" />
		</xsl:if>
		
    </xsl:template>

	<!-- Output each hardware node -->
	<xsl:template name="HardwareNode">
		<xsl:param name="transformNodeLabelKey" />
		<xsl:param name="transformNodeKindKey" />
		<xsl:param name="transformNodeIp_addressKey" />
		
		<xsl:variable name="label"><xsl:value-of select="gml:data[@key=$transformNodeLabelKey]"/></xsl:variable>
		<xsl:variable name="kind"><xsl:value-of select="gml:data[@key=$transformNodeKindKey]"/></xsl:variable>
		<xsl:variable name="ip"><xsl:value-of select="gml:data[@key=$transformNodeIp_addressKey]"/></xsl:variable>
	
		<xsl:if test="$kind = 'HardwareNode'" >
			<node>
			
				<name>
					<xsl:value-of select="$label" />   <!-- MainNodeRef -->
				</name>
				<label/>
				<resource>
				  <name>Node Address</name>
				  <resourceType>edu.vanderbilt.dre.DAnCE.NodeAddress</resourceType>
				  <property>
					<name>edu.vanderbilt.dre.DAnCE.StringIOR</name>
					<kind>Attribute</kind>
					<dynamic>false</dynamic>
					<value>
					  <type>
						<kind>tk_string</kind>
					  </type>
					  <value>
						<string>
							<xsl:variable name="corbaloc" select="concat(concat(concat('corbaloc:iiop:', $ip), ':'), 60000 + position() )" />
							<xsl:value-of select="$corbaloc" />
						</string>
					  </value>
					</value>
				  </property>
				</resource>

			</node>
		</xsl:if>
		
    </xsl:template>

	<!-- Output Default hardware node -->
	<xsl:template name="DefaultHardwareNode">
	
		<node>
			<name>
				<xsl:value-of select="'MainNode'" />   
			</name>
			<label/>
			<resource>
			  <name>Node Address</name>
			  <resourceType>edu.vanderbilt.dre.DAnCE.NodeAddress</resourceType>
			  <property>
				<name>edu.vanderbilt.dre.DAnCE.StringIOR</name>
				<kind>Attribute</kind>
				<dynamic>false</dynamic>
				<value>
				  <type>
					<kind>tk_string</kind>
				  </type>
				  <value>
					<string>
						<xsl:value-of select="'corbaloc:iiop:localhost:60001'" />
					</string>
				  </value>
				</value>
			  </property>
			</resource>
		</node>
		
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