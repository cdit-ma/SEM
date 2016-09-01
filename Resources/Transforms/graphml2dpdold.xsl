<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="2.0" 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:gml="http://graphml.graphdrawing.org/xmlns"

	xmlns:exsl="http://exslt.org/common"
	xmlns:xalan="http://xml.apache.org/xslt"	
	xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" 
	xsi:schemaLocation="http://www.omg.org/Deployment Deployment.xsd"
	xmlns:redirect = "org.apache. xalan.xslt.extensions.Redirect"
	
	exclude-result-prefixes="gml exsl xalan">
		
    <xsl:output method="xml" 
        version="1.0" 
        indent="yes" 
		standalone="no"
		xalan:indent-amount="4"/>
	<xsl:strip-space elements="*" />
	
    <!--
        Purpose:

        This template will convert the .graphml project to a quality of service .dpd 
        file. The transform creates a xml file used for system deployment for dds type middleware.
		This produced file may be extended in the future to allow user settings for each Component Instance.
    -->
	
	<!-- Runtime parameters -->
	<xsl:param name="ComponentInstance" />

	<!-- Assign default data keys as used by yEd 3.12.2 -->
	<xsl:include href="graphmlKeyVariables.xsl" />
	
	<!-- apply all templates starting from the root -->
    <xsl:template match="/">
		
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
		
		<xsl:variable name="transformNodeTopicNameKey">
			<xsl:call-template name="findNodeKey">
				<xsl:with-param name="attrName" select="'topicName'" />
				<xsl:with-param name="defaultId" select="$nodeTopicNameKey" />
			</xsl:call-template>	
		</xsl:variable>	
		
		<!-- Output start of dds:participant -->
		<dds:participant xmlns="http://cuts.cs.iupui.edu/iccm" xmlns:dds="http://cuts.cs.iupui.edu/iccm" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:schemaLocation="http://cuts.cs.iupui.edu/iccm dds.xsd">

		<!-- BEGIN DDS::DomainParticipantQoS parameters -->
		  <entity_factory autoenable_created_entities="true"/>
		  <watchdog_scheduling scheduling_priority="1">
			<scheduling_class kind="SCHEDULING_DEFAULT"/>
			<scheduling_priority_kind kind="PRIORITY_RELATIVE"/>
		  </watchdog_scheduling>
		  <listener_scheduling scheduling_priority="1">
			<scheduling_class kind="SCHEDULING_TIMESHARING"/>
			<scheduling_priority_kind kind="PRIORITY_ABSOLUTE"/>
		  </listener_scheduling>

		<xsl:variable name="AssemblyDefs" select="./descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'AssemblyDefinitions']/.." />
		<xsl:variable name="foundComponentId"> 
			<xsl:call-template name="findComponentInstance" >
				<xsl:with-param name="labelKey" select="$transformNodeLabelKey" />
				<xsl:with-param name="kindKey" select="$transformNodeKindKey" />
				<xsl:with-param name="longName" select="$ComponentInstance" />
				<xsl:with-param name="startNode" select="$AssemblyDefs" />
			</xsl:call-template>
 		 </xsl:variable>  		   		
		<xsl:variable name="thisComponent" select="./descendant::*/gml:node[@id=$foundComponentId]/gml:data[@key=$transformNodeKindKey][text() = 'ComponentInstance']/..
												 | ./descendant::*/gml:node[@id=$foundComponentId]/gml:data[@key=$transformNodeKindKey][text() = 'BlackBoxInstance']/.." />

		  <!-- for each input port create a topic -->
		  <xsl:for-each select="$thisComponent/gml:graph/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'InEventPortInstance']/.." >
			<xsl:variable name="inEventPortLabel" select="./gml:data[@key=$transformNodeLabelKey]/text()" />
		  <!-- BEGIN DDS::TopicQoS parameters -->
		  <topic name="{$inEventPortLabel}">
			<durability kind="VOLATILE_DURABILITY_QOS"/>
			<durability_service history_depth="1" history_kind="KEEP_LAST_HISTORY_QOS" max_instances="1" max_samples="1" max_samples_per_instance="1" service_cleanup_delay="1.0"/>
			<deadline period="1"/>
			<!-- <latency_budget duration="1"/> -->
			<liveliness kind="AUTOMATIC_LIVELINESS_QOS" lease_duration="1"/>
			<reliability kind="BEST_EFFORT_RELIABILITY_QOS" max_blocking_time="1" synchronous="true"/>
			<destination_order kind="BY_RECEPTION_TIMESTAMP_DESTINATIONORDER_QOS"/>
			<history depth="1" kind="KEEP_LAST_HISTORY_QOS"/>
			<resource_limits initial_samples="1" initial_instances="1" max_instances="1" max_samples="1" max_samples_per_instance="1"/>
			<transport_priority value="1"/>
			<lifespan duration="1"/>
			<ownership kind="SHARED_OWNERSHIP_QOS"/>
		  </topic>
		</xsl:for-each>
		
		<!-- for each output port create a topic -->		
		<xsl:for-each select="$thisComponent/gml:graph/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'OutEventPortInstance']/.." >
			<xsl:variable name="outEventPortLabel" select="./gml:data[@key=$transformNodeLabelKey]/text()" />
		  <!-- BEGIN DDS::TopicQoS parameters -->
		  <topic name="{$outEventPortLabel}">
			<durability kind="VOLATILE_DURABILITY_QOS"/>
			<durability_service history_depth="1" history_kind="KEEP_LAST_HISTORY_QOS" max_instances="1" max_samples="1" max_samples_per_instance="1" service_cleanup_delay="1.0"/>
			<deadline period="1"/>
			<!-- <latency_budget duration="1"/> -->
			<liveliness kind="AUTOMATIC_LIVELINESS_QOS" lease_duration="1"/>
			<reliability kind="BEST_EFFORT_RELIABILITY_QOS" max_blocking_time="1" synchronous="true"/>
			<destination_order kind="BY_RECEPTION_TIMESTAMP_DESTINATIONORDER_QOS"/>
			<history depth="1" kind="KEEP_LAST_HISTORY_QOS"/>
			<resource_limits initial_samples="1" initial_instances="1" max_instances="1" max_samples="1" max_samples_per_instance="1"/>
			<transport_priority value="1"/>
			<lifespan duration="1"/>
			<ownership kind="SHARED_OWNERSHIP_QOS"/>
		  </topic>
		</xsl:for-each>
		
		  <!-- BEGIN DDS::PublisherQoS parameters -->
		  <publisher name="@default">
			<!-- <presentation access_scope="INSTANCE_PRESENTATION_QOS" coherent_access="true"/> -->
			<!-- <partition>
			  <name>
				<item>A</item>
			  </name>
			</partition> -->
			<entity_factory autoenable_created_entities="true"/>
		  </publisher>
		  
		  <!-- BEGIN DDS::SubscriberQoS parameters -->
		  <subscriber name="NoDefaultSubscriber">
			<!-- <presentation access_scope="INSTANCE_PRESENTATION_QOS" coherent_access="true"/> -->
			<!-- <partition>
			  <name>
				<item>A</item>
			  </name>
			</partition> -->
			<entity_factory autoenable_created_entities="true"/>
		  </subscriber>
		  
		  <!-- for each output port create datawriter -->
		<xsl:for-each select="$thisComponent/gml:graph/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'OutEventPortInstance']/.." >
			<xsl:variable name="outEventPortLabel" select="./gml:data[@key=$transformNodeLabelKey]/text()" />
			<xsl:variable name="topicName" select="./gml:data[@key=$transformNodeTopicNameKey]/text()" />

			<xsl:choose>
				<xsl:when test="$topicName != ''">
				
		  <!-- BEGIN DDS::DataWriterQoS parameters -->				
		  <datawriter isinstance="false" isprivate="false" name="{$outEventPortLabel}" topic="{$outEventPortLabel}" topic_name="{$topicName}" >
			<durability kind="VOLATILE_DURABILITY_QOS"/>
			<deadline period="1"/>
			<!-- <latency_budget duration="1"/> -->
			<liveliness kind="AUTOMATIC_LIVELINESS_QOS" lease_duration="1"/>
			<reliability kind="BEST_EFFORT_RELIABILITY_QOS" max_blocking_time="1" synchronous="true"/>
			<destination_order kind="BY_RECEPTION_TIMESTAMP_DESTINATIONORDER_QOS"/>
			<history depth="1" kind="KEEP_LAST_HISTORY_QOS"/>
			<resource_limits initial_samples="1" initial_instances="1" max_instances="1" max_samples="1" max_samples_per_instance="1"/>
			<transport_priority value="1"/>
			<lifespan duration="1"/>
			<user_data/>
			<ownership kind="SHARED_OWNERSHIP_QOS"/>
			<ownership_strength value="1"/>
			<writer_data_lifecycle autodispose_unregistered_instances="true" autopurge_suspended_samples_delay="1.0" autounregister_instance_delay="1.0"/>
			<protocol>
				<rtps_reliable_writer heartbeats_per_max_samples="1" />
			</protocol>
		  </datawriter>
		  
				</xsl:when>
				<xsl:otherwise>
		  <!-- BEGIN DDS::DataWriterQoS parameters -->				
		  <datawriter isinstance="false" isprivate="true" name="{$outEventPortLabel}" topic="{$outEventPortLabel}" >
		  	<durability kind="VOLATILE_DURABILITY_QOS"/>
			<deadline period="1"/>
			<!-- <latency_budget duration="1"/> -->
			<liveliness kind="AUTOMATIC_LIVELINESS_QOS" lease_duration="1"/>
			<reliability kind="BEST_EFFORT_RELIABILITY_QOS" max_blocking_time="1" synchronous="true"/>
			<destination_order kind="BY_RECEPTION_TIMESTAMP_DESTINATIONORDER_QOS"/>
			<history depth="1" kind="KEEP_LAST_HISTORY_QOS"/>
			<resource_limits initial_samples="1" initial_instances="1" max_instances="1" max_samples="1" max_samples_per_instance="1"/>
			<transport_priority value="1"/>
			<lifespan duration="1"/>
			<user_data/>
			<ownership kind="SHARED_OWNERSHIP_QOS"/>
			<ownership_strength value="1"/>
			<writer_data_lifecycle autodispose_unregistered_instances="true" autopurge_suspended_samples_delay="1.0" autounregister_instance_delay="1.0"/>
			<protocol>
				<rtps_reliable_writer heartbeats_per_max_samples="1" />
			</protocol>
		  </datawriter>
		  
				</xsl:otherwise>
			</xsl:choose>

		</xsl:for-each>
		
		<!-- for each input port create datareader -->
		<xsl:for-each select="$thisComponent/gml:graph/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'InEventPortInstance']/.." >
			<xsl:variable name="inEventPortLabel" select="./gml:data[@key=$transformNodeLabelKey]/text()" />
			<!-- BEGIN DDS::DataReaderQoS parameters -->
			<datareader isprivate="true" name="{$inEventPortLabel}" subscriber="NoDefaultSubscriber" topic="{$inEventPortLabel}">
			<durability kind="VOLATILE_DURABILITY_QOS"/>
			<!-- <latency_budget duration="1"/> -->
			<liveliness kind="AUTOMATIC_LIVELINESS_QOS" lease_duration="1"/>
			<reliability kind="BEST_EFFORT_RELIABILITY_QOS" max_blocking_time="1" synchronous="true"/>
			<destination_order kind="BY_RECEPTION_TIMESTAMP_DESTINATIONORDER_QOS"/>
			<history depth="1" kind="KEEP_LAST_HISTORY_QOS"/>
			<resource_limits initial_samples="1" initial_instances="1" max_instances="1" max_samples="1" max_samples_per_instance="1"/>
			<user_data/>
			<ownership kind="SHARED_OWNERSHIP_QOS"/>
			<reader_data_lifecycle autopurge_disposed_samples_delay="1" autopurge_nowriter_samples_delay="1" enable_invalid_samples="true"/>
			<reader_lifespan duration="1" use_lifespan="true"/>
		  </datareader>
		</xsl:for-each>
		
		  <!-- Output end of dds:domain -->
		</dds:participant>

    </xsl:template>
		
	<!-- Find ComponentInstance from long name of Assembly_Component -->
	 <xsl:template name="findComponentInstance">
		<xsl:param name="labelKey" />
		<xsl:param name="kindKey" />
		<xsl:param name="longName" />	
		<xsl:param name="startNode" />	 

		<!-- Now using % as delimiter to avoid non-unique names if using _ -->
		<xsl:variable name="delim" select="'%'" />
  		<xsl:variable name="componentParentCount" select="string-length($longName) - string-length(translate($longName, $delim, ''))" />
	  
		<xsl:variable name="Label" > 
			<xsl:call-template name="splitComponentInstance" >
				<xsl:with-param name="pText" select="$longName" />
				<xsl:with-param name="componentParentCount" select="$componentParentCount" />
				<xsl:with-param name="idx" select="0" />
				<xsl:with-param name="delim" select="$delim" />
			</xsl:call-template>	
		</xsl:variable> 
		<xsl:variable name="foundNodeId" select="$startNode/gml:graph/gml:node[gml:data[@key=$labelKey]/text() = $Label]/@id" />
		<xsl:variable name="foundNode" select="$startNode/gml:graph/gml:node[@id=$foundNodeId]" />

		<xsl:choose>
			<xsl:when test="$foundNode/gml:data[@key=$kindKey]/text() = 'ComponentInstance'">
				<xsl:value-of select="$foundNode/@id" />
			</xsl:when>
			<xsl:when test="$foundNode/gml:data[@key=$kindKey]/text() = 'BlackBoxInstance'">
				<xsl:value-of select="$foundNode/@id" />
			</xsl:when>
			<xsl:when test="$foundNode/gml:data[@key=$kindKey]/text() = 'ComponentAssembly'">
			
				<xsl:variable name="remainingText" select="substring-after($longName, $delim)"/>
				
				<xsl:call-template name="findComponentInstance" >
					<xsl:with-param name="labelKey" select="$labelKey" />
					<xsl:with-param name="kindKey" select="$kindKey" />
					<xsl:with-param name="longName" select="$remainingText" />
					<xsl:with-param name="startNode" select="$foundNode" />
				</xsl:call-template>
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="''" />
			</xsl:otherwise>
		</xsl:choose>
    
	</xsl:template>		
		
	<!-- Split delimited list and return required item -->
	 <xsl:template name="splitComponentInstance">
		<xsl:param name="pText" />
		<xsl:param name="componentParentCount" />
		<xsl:param name="idx" />
		<xsl:param name="delim" />
		
		<xsl:if test="string-length($pText) > 0">
			<xsl:variable name="pCount" select="string-length($pText) - string-length(translate($pText, $delim, ''))" />
			
			<xsl:choose>
				<xsl:when test="($componentParentCount - $pCount) = $idx" >
					<xsl:value-of select="substring-before(concat($pText, $delim), $delim)"/>
				</xsl:when>
				<xsl:otherwise>
						
					<xsl:call-template name="splitComponentInstance">
						<xsl:with-param name="pText" select="substring-after($pText, $delim)"/>
						<xsl:with-param name="componentParentCount" select="$componentParentCount" />
						<xsl:with-param name="idx" select="$idx" />
						<xsl:with-param name="delim" select="$delim" />
					</xsl:call-template> 
				</xsl:otherwise>
			</xsl:choose>
		</xsl:if>
	</xsl:template>
	
	<!-- find the key for specific attribute,  -->
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

</xsl:stylesheet>