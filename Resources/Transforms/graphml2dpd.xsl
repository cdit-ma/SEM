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
	<xsl:param name="ComponentInstanceID" />

	<!-- Assign default data keys as used by yEd 3.12.2 -->
	<xsl:include href="graphmlKeyVariables.xsl" />

    <xsl:variable name="kind_key_id">
            <xsl:call-template name="findNodeKey">
                <xsl:with-param name="attrName" select="'kind'" />
                <xsl:with-param name="defaultId" select="$nodeKindKey" />
            </xsl:call-template>	
    </xsl:variable>

    <xsl:variable name="label_key_id">
        <xsl:call-template name="findNodeKey">
            <xsl:with-param name="attrName" select="'label'" />
            <xsl:with-param name="defaultId" select="$nodeLabelKey" />
        </xsl:call-template>	
    </xsl:variable>

    <xsl:variable name="type_key_id">
        <xsl:call-template name="findNodeKey">
            <xsl:with-param name="attrName" select="'type'" />
            <xsl:with-param name="defaultId" select="$nodeLabelKey" />
        </xsl:call-template>
    </xsl:variable>

    <xsl:variable name="topic_key_id">
        <xsl:call-template name="findNodeKey">
            <xsl:with-param name="attrName" select="'topicName'" />
        </xsl:call-template>	
    </xsl:variable>

    <xsl:template match="/">
        <!-- Output start of dds:participant -->
		<dds:participant xmlns="http://cuts.cs.iupui.edu/iccm" xmlns:dds="http://cuts.cs.iupui.edu/iccm" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:schemaLocation="http://cuts.cs.iupui.edu/iccm dds.xsd">
        
        <!-- Find this ComponentInstance -->
        <xsl:variable name="componentInstance" select="//gml:node[@id=$ComponentInstanceID]" />

        <xsl:for-each select="$componentInstance">
            <xsl:variable name="qos_profile_id">
                <xsl:call-template name="getQOSProfileID" />
            </xsl:variable>

            <xsl:variable name="qos_profile" select="//gml:node[@id=$qos_profile_id]" />
            
             <xsl:choose>
                <xsl:when test="$qos_profile">
                    <xsl:comment>Found QOS Profile: '<xsl:value-of select="$qos_profile/gml:data[@key=$label_key_id]" />' for ComponentInstance ID: <xsl:value-of select="$componentInstance/@id" />! Using defaults!</xsl:comment>
                </xsl:when>
                <xsl:otherwise>
                    <xsl:comment>Using QOS Defaults for ComponentInstance ID: <xsl:value-of select="$componentInstance/@id" /></xsl:comment>
                </xsl:otherwise>
            </xsl:choose>


            <xsl:call-template name="generate_entity_factory">
                <xsl:with-param name="qos" select="$qos_profile" />
            </xsl:call-template>

            <!-- Unsure where these values come from -->
		    <watchdog_scheduling scheduling_priority="1">
			    <scheduling_class kind="SCHEDULING_DEFAULT"/>
			    <scheduling_priority_kind kind="PRIORITY_RELATIVE"/>
		    </watchdog_scheduling>
		    <listener_scheduling scheduling_priority="1">
			    <scheduling_class kind="SCHEDULING_TIMESHARING"/>
			    <scheduling_priority_kind kind="PRIORITY_ABSOLUTE"/>
		    </listener_scheduling>



            <!-- Generate Topics -->
            <xsl:for-each select="./gml:graph/gml:node/gml:data[@key=$kind_key_id][contains(text(), 'EventPortInstance')]/..">
                <xsl:variable name="port_qos_profile_id">
                    <xsl:call-template name="getQOSProfileID" />
                </xsl:variable>

                <xsl:variable name="port_qos_profile" select="//gml:node[@id=$port_qos_profile_id]" />
                

                <xsl:choose>
                    <xsl:when test="$port_qos_profile">
                        <xsl:comment>Found QOS Profile: '<xsl:value-of select="$port_qos_profile/gml:data[@key=$label_key_id]" />' for EventPortInstance ID: <xsl:value-of select="@id" /></xsl:comment>
                    </xsl:when>
                    <xsl:otherwise>
                        <xsl:comment>Using QOS Defaults for EventPortInstance ID: <xsl:value-of select="@id" /></xsl:comment>
                    </xsl:otherwise>
                </xsl:choose>

                
                
                <!-- Default topic name to port name -->
                <xsl:variable name="topic_name" select="./gml:data[@key=$type_key_id]" />

                <!-- Default topic name to port name 
                <xsl:variable name="topic_name">
                    <xsl:choose>
				        <xsl:when test="./gml:data[@key=$topic_key_id] = ''">
                            <xsl:value-of select="./gml:data[@key=$label_key_id]" />
                        </xsl:when>
                        <xsl:otherwise>
                            <xsl:value-of select="./gml:data[@key=$topic_key_id]" />
                        </xsl:otherwise>
                    </xsl:choose>
                </xsl:variable> -->
                
                 <xsl:call-template name="generate_topic">
                    <xsl:with-param name="topic_name" select="$topic_name" />
                    <xsl:with-param name="qos" select="$port_qos_profile" />
                </xsl:call-template>
            </xsl:for-each>

            <!-- Generate Publisher and Subscriber -->
            <xsl:call-template name="generate_publisher">
                <xsl:with-param name="qos" select="$qos_profile" />
            </xsl:call-template>

            <xsl:call-template name="generate_subscriber">
                <xsl:with-param name="qos" select="$qos_profile" />
            </xsl:call-template>

            

            <!-- Generate DataWriter -->
            <xsl:for-each select="./gml:graph/gml:node/gml:data[@key=$kind_key_id][contains(text(), 'OutEventPortInstance')]/..">
                <xsl:variable name="port_qos_profile_id">
                    <xsl:call-template name="getQOSProfileID" />
                </xsl:variable>

                <xsl:variable name="port_qos_profile" select="//gml:node[@id=$port_qos_profile_id]" />

                <xsl:choose>
                    <xsl:when test="$port_qos_profile">
                        <xsl:comment>Found QOS Profile: '<xsl:value-of select="$port_qos_profile/gml:data[@key=$label_key_id]" />' for OutEventPortInstance ID: <xsl:value-of select="@id" /></xsl:comment>
                    </xsl:when>
                    <xsl:otherwise>
                        <xsl:comment>Using QOS Defaults for OutEventPortInstance ID: <xsl:value-of select="@id" /></xsl:comment>
                    </xsl:otherwise>
                </xsl:choose>

                <xsl:variable name="port_name" select="./gml:data[@key=$label_key_id]" />

                <!-- Default topic name to port name -->
                <xsl:variable name="topic_name">
                    <xsl:choose>
				        <xsl:when test="./gml:data[@key=$topic_key_id] = ''">
                            <xsl:value-of select="$port_name" />
                        </xsl:when>
                        <xsl:otherwise>
                            <xsl:value-of select="./gml:data[@key=$topic_key_id]" />
                        </xsl:otherwise>
                    </xsl:choose>
                </xsl:variable> 

                 <xsl:call-template name="generate_datawriter">
                    <xsl:with-param name="port_name" select="$port_name" />
                    <xsl:with-param name="topic_name" select="$topic_name" />
                    <xsl:with-param name="qos" select="$port_qos_profile" />
                </xsl:call-template>
            </xsl:for-each>

            <!-- Generate DataReader -->
            <xsl:for-each select="./gml:graph/gml:node/gml:data[@key=$kind_key_id][contains(text(), 'InEventPortInstance')]/..">
                <xsl:variable name="port_qos_profile_id">
                    <xsl:call-template name="getQOSProfileID" />
                </xsl:variable>

                <xsl:variable name="port_qos_profile" select="//gml:node[@id=$port_qos_profile_id]" />

                <xsl:choose>
                    <xsl:when test="$port_qos_profile">
                        <xsl:comment>Found QOS Profile: '<xsl:value-of select="$port_qos_profile/gml:data[@key=$label_key_id]" />' for InEventPortInstance ID: <xsl:value-of select="@id" /></xsl:comment>
                    </xsl:when>
                    <xsl:otherwise>
                        <xsl:comment>Using QOS Defaults for InEventPortInstance ID: <xsl:value-of select="@id" /></xsl:comment>
                    </xsl:otherwise>
                </xsl:choose>

                <xsl:variable name="port_name" select="./gml:data[@key=$label_key_id]" />

                <!-- Default topic name to port name -->
                <xsl:variable name="topic_name">
                    <xsl:choose>
				        <xsl:when test="./gml:data[@key=$topic_key_id] = ''">
                            <xsl:value-of select="$port_name" />
                        </xsl:when>
                        <xsl:otherwise>
                            <xsl:value-of select="./gml:data[@key=$topic_key_id]" />
                        </xsl:otherwise>
                    </xsl:choose>
                </xsl:variable> 

                 <xsl:call-template name="generate_datareader">
                    <xsl:with-param name="port_name" select="$port_name" />
                    <xsl:with-param name="topic_name" select="$topic_name" />
                    <xsl:with-param name="qos" select="$port_qos_profile" />
                </xsl:call-template>
            </xsl:for-each>

        </xsl:for-each>
        </dds:participant>
    </xsl:template>

    <xsl:template name="generate_entity_factory">
         <xsl:param name="qos" />
         <xsl:variable name="policy_kind" select="'DDS_EntityFactoryQosPolicy'" />

         <xsl:variable name="auto_enable_created_entities">
              <xsl:call-template name="get_qos_policy_data_value">
                <xsl:with-param name="qos" select="$qos" />
                <xsl:with-param name="policy_kind" select="$policy_kind" />
                <xsl:with-param name="key_kind" select="'qos_dds_autoenable_created_entities'" />
                <xsl:with-param name="default_value" select="'true'" />
            </xsl:call-template>
        </xsl:variable>

        <entity_factory
            xmlns="http://cuts.cs.iupui.edu/iccm"
            autoenable_created_entities="{$auto_enable_created_entities}"
        />
    </xsl:template>

       
    
    <xsl:template name="generate_datawriter">
         <xsl:param name="port_name" />
         <xsl:param name="topic_name" />
         <xsl:param name="qos" />

        <datawriter
            xmlns="http://cuts.cs.iupui.edu/iccm"
            isinstance="false"
            isprivate="false"
            name="{$port_name}"
            topic="{$port_name}"
            topic_name="{$topic_name}">
            <xsl:call-template name="generate_durability">
                <xsl:with-param name="qos" select="$qos" />
            </xsl:call-template>

            <xsl:call-template name="generate_deadline">
                <xsl:with-param name="qos" select="$qos" />
            </xsl:call-template>

            <xsl:call-template name="generate_liveliness">
                <xsl:with-param name="qos" select="$qos" />
            </xsl:call-template>

            <xsl:call-template name="generate_reliability">
                <xsl:with-param name="qos" select="$qos" />
            </xsl:call-template>

            <xsl:call-template name="generate_destination_order">
                <xsl:with-param name="qos" select="$qos" />
            </xsl:call-template>

            <xsl:call-template name="generate_history">
                <xsl:with-param name="qos" select="$qos" />
            </xsl:call-template>

            <xsl:call-template name="generate_resource_limits">
                <xsl:with-param name="qos" select="$qos" />
            </xsl:call-template>

            <xsl:call-template name="generate_transport_priority">
                <xsl:with-param name="qos" select="$qos" />
            </xsl:call-template>

            <xsl:call-template name="generate_lifespan">
                <xsl:with-param name="qos" select="$qos" />
            </xsl:call-template>

            <xsl:call-template name="generate_ownership">
                <xsl:with-param name="qos" select="$qos" />
            </xsl:call-template>

            <xsl:call-template name="generate_ownership_strength">
                <xsl:with-param name="qos" select="$qos" />
            </xsl:call-template>

            <xsl:call-template name="generate_writer_data_lifecycle">
                <xsl:with-param name="qos" select="$qos" />
            </xsl:call-template>

            <!-- <latency_budget duration="1"/> -->
            <protocol>
				<rtps_reliable_writer heartbeats_per_max_samples="1" />
			</protocol>
        </datawriter>
    </xsl:template>

    <xsl:template name="generate_datareader">
         <xsl:param name="port_name" />
         <xsl:param name="topic_name" />
         <xsl:param name="subscriber" select="'NoDefaultSubscriber'" />
         <xsl:param name="qos" />

        <datareader
            xmlns="http://cuts.cs.iupui.edu/iccm"
            isprivate="true"
            name="{$port_name}"
            subscriber="{$subscriber}"
            topic="{$topic_name}">
            <xsl:call-template name="generate_durability">
                <xsl:with-param name="qos" select="$qos" />
            </xsl:call-template>

            <xsl:call-template name="generate_liveliness">
                <xsl:with-param name="qos" select="$qos" />
            </xsl:call-template>

            <xsl:call-template name="generate_reliability">
                <xsl:with-param name="qos" select="$qos" />
            </xsl:call-template>

            <xsl:call-template name="generate_destination_order">
                <xsl:with-param name="qos" select="$qos" />
            </xsl:call-template>

            <xsl:call-template name="generate_history">
                <xsl:with-param name="qos" select="$qos" />
            </xsl:call-template>

            <xsl:call-template name="generate_resource_limits">
                <xsl:with-param name="qos" select="$qos" />
            </xsl:call-template>

            <xsl:call-template name="generate_ownership">
                <xsl:with-param name="qos" select="$qos" />
            </xsl:call-template>

            <xsl:call-template name="generate_reader_data_lifecycle">
                <xsl:with-param name="qos" select="$qos" />
            </xsl:call-template>

            <!-- <latency_budget duration="1"/> -->
        </datareader>
    </xsl:template>



    <xsl:template name="generate_topic">
         <xsl:param name="topic_name" />
         <xsl:param name="qos" />

         <topic name="{$topic_name}" xmlns="http://cuts.cs.iupui.edu/iccm">
                    <xsl:call-template name="generate_durability">
                        <xsl:with-param name="qos" select="$qos" />
                    </xsl:call-template>

                    <xsl:call-template name="generate_durability_service">
                        <xsl:with-param name="qos" select="$qos" />
                    </xsl:call-template>

                    <xsl:call-template name="generate_deadline">
                        <xsl:with-param name="qos" select="$qos" />
                    </xsl:call-template>

                    <xsl:call-template name="generate_liveliness">
                        <xsl:with-param name="qos" select="$qos" />
                    </xsl:call-template>

                    <xsl:call-template name="generate_reliability">
                        <xsl:with-param name="qos" select="$qos" />
                    </xsl:call-template>

                    <xsl:call-template name="generate_destination_order">
                        <xsl:with-param name="qos" select="$qos" />
                    </xsl:call-template>

                    <xsl:call-template name="generate_history">
                        <xsl:with-param name="qos" select="$qos" />
                    </xsl:call-template>

                    <xsl:call-template name="generate_resource_limits">
                        <xsl:with-param name="qos" select="$qos" />
                    </xsl:call-template>

                    <xsl:call-template name="generate_transport_priority">
                        <xsl:with-param name="qos" select="$qos" />
                    </xsl:call-template>

                    <xsl:call-template name="generate_lifespan">
                        <xsl:with-param name="qos" select="$qos" />
                    </xsl:call-template>

                    <xsl:call-template name="generate_ownership">
                        <xsl:with-param name="qos" select="$qos" />
                    </xsl:call-template>
                </topic>
    </xsl:template>


    <xsl:template name="generate_publisher">
        <xsl:param name="name" select="'@default'" />
        <xsl:param name="qos" />

        <publisher name="{$name}" xmlns="http://cuts.cs.iupui.edu/iccm">
            <!-- <xsl:call-template name="generate_presentation">
                <xsl:with-param name="qos" select="$qos" />
            </xsl:call-template> -->
        </publisher>
    </xsl:template>

    <xsl:template name="generate_subscriber">
        <xsl:param name="name" select="'NoDefaultSubscriber'" />
        <xsl:param name="qos" />

        <subscriber name="{$name}" xmlns="http://cuts.cs.iupui.edu/iccm">
            <!-- <xsl:call-template name="generate_presentation">
                <xsl:with-param name="qos" select="$qos" />
            </xsl:call-template> -->
        </subscriber>
    </xsl:template>
    



    <xsl:template name="generate_durability">
         <xsl:param name="qos" />
         <xsl:variable name="policy_kind" select="'DDS_DurabilityQosPolicy'" />

        <xsl:variable name="durability_kind">
              <xsl:call-template name="get_qos_policy_data_value">
                <xsl:with-param name="qos" select="$qos" />
                <xsl:with-param name="policy_kind" select="$policy_kind" />
                <xsl:with-param name="key_kind" select="'qos_dds_kind'" />
                <xsl:with-param name="default_value" select="'VOLATILE_DURABILITY_QOS'" />
            </xsl:call-template>
        </xsl:variable>

        <durability xmlns="http://cuts.cs.iupui.edu/iccm" kind="{$durability_kind}">
        </durability>
    </xsl:template>


    <xsl:template name="generate_durability_service">
        <xsl:param name="qos" />
        <xsl:variable name="policy_kind" select="'DDS_DurabilityServiceQosPolicy'" />

        <xsl:variable name="history_depth">
              <xsl:call-template name="get_qos_policy_data_value">
                <xsl:with-param name="qos" select="$qos" />
                <xsl:with-param name="policy_kind" select="$policy_kind" />
                <xsl:with-param name="key_kind" select="'qos_dds_history_depth'" />
                <xsl:with-param name="default_value" select="'1'" />
            </xsl:call-template>
        </xsl:variable>

        <xsl:variable name="history_kind">
              <xsl:call-template name="get_qos_policy_data_value">
                <xsl:with-param name="qos" select="$qos" />
                <xsl:with-param name="policy_kind" select="$policy_kind" />
                <xsl:with-param name="key_kind" select="'qos_dds_history_kind'" />
                <xsl:with-param name="default_value" select="'KEEP_LAST_HISTORY_QOS'" />
            </xsl:call-template>
        </xsl:variable>

        <xsl:variable name="max_instances">
            <xsl:call-template name="get_qos_policy_data_value">
                <xsl:with-param name="qos" select="$qos" />
                <xsl:with-param name="policy_kind" select="$policy_kind" />
                <xsl:with-param name="key_kind" select="'qos_dds_max_instances'" />
                <xsl:with-param name="default_value" select="'1'" />
            </xsl:call-template>
        </xsl:variable>
        <xsl:variable name="max_samples">
            <xsl:call-template name="get_qos_policy_data_value">
                <xsl:with-param name="qos" select="$qos" />
                <xsl:with-param name="policy_kind" select="$policy_kind" />
                <xsl:with-param name="key_kind" select="'qos_dds_max_samples'" />
                <xsl:with-param name="default_value" select="'1'" />
            </xsl:call-template>
        </xsl:variable>

        <xsl:variable name="max_samples_per_instance">
            <xsl:call-template name="get_qos_policy_data_value">
                <xsl:with-param name="qos" select="$qos" />
                <xsl:with-param name="policy_kind" select="$policy_kind" />
                <xsl:with-param name="key_kind" select="'qos_dds_max_samples_per_instance'" />
                <xsl:with-param name="default_value" select="'1'" />
            </xsl:call-template>
        </xsl:variable>

        <xsl:variable name="service_cleanup_delay">
            <xsl:call-template name="get_qos_policy_data_value">
                <xsl:with-param name="qos" select="$qos" />
                <xsl:with-param name="policy_kind" select="$policy_kind" />
                <xsl:with-param name="key_kind" select="'qos_dds_service_cleanup_delay'" />
                <xsl:with-param name="default_value" select="'1'" />
            </xsl:call-template>
        </xsl:variable>

        

        <durability_service 
        xmlns="http://cuts.cs.iupui.edu/iccm"
        history_depth = "{$history_depth}"
        history_kind = "{$history_kind}"
        max_instances = "{$max_instances}"
        max_samples = "{$max_samples}"
        max_samples_per_instance = "{$max_samples_per_instance}"
        service_cleanup_delay = "{$service_cleanup_delay}"
        />
    </xsl:template>

   <xsl:template name="generate_deadline">
         <xsl:param name="qos" />
         <xsl:variable name="policy_kind" select="'DDS_DeadlineQosPolicy'" />

        <xsl:variable name="period">
              <xsl:call-template name="get_qos_policy_data_value">
                <xsl:with-param name="qos" select="$qos" />
                <xsl:with-param name="policy_kind" select="$policy_kind" />
                <xsl:with-param name="key_kind" select="'qos_dds_period'" />
                <xsl:with-param name="default_value" select="'1'" />
            </xsl:call-template>
        </xsl:variable>

        <deadline
            xmlns="http://cuts.cs.iupui.edu/iccm"
            period="{$period}">
        </deadline>
    </xsl:template>

    <xsl:template name="generate_liveliness">
         <xsl:param name="qos" />
         <xsl:variable name="policy_kind" select="'DDS_LivelinessQosPolicy'" />

        <xsl:variable name="lease_duration">
              <xsl:call-template name="get_qos_policy_data_value">
                <xsl:with-param name="qos" select="$qos" />
                <xsl:with-param name="policy_kind" select="$policy_kind" />
                <xsl:with-param name="key_kind" select="'qos_dds_lease_duration'" />
                <xsl:with-param name="default_value" select="'1'" />
            </xsl:call-template>
        </xsl:variable>

        <xsl:variable name="kind">
              <xsl:call-template name="get_qos_policy_data_value">
                <xsl:with-param name="qos" select="$qos" />
                <xsl:with-param name="policy_kind" select="$policy_kind" />
                <xsl:with-param name="key_kind" select="'qos_dds_kind'" />
                <xsl:with-param name="default_value" select="'AUTOMATIC_LIVELINESS_QOS'" />
            </xsl:call-template>
        </xsl:variable>

        <liveliness
            xmlns="http://cuts.cs.iupui.edu/iccm"
            lease_duration="{$lease_duration}"
            kind="{$kind}"
        />
    </xsl:template>

    <xsl:template name="generate_reliability">
         <xsl:param name="qos" />
         <xsl:variable name="policy_kind" select="'DDS_ReliabilityQosPolicy'" />

        <xsl:variable name="kind">
              <xsl:call-template name="get_qos_policy_data_value">
                <xsl:with-param name="qos" select="$qos" />
                <xsl:with-param name="policy_kind" select="$policy_kind" />
                <xsl:with-param name="key_kind" select="'qos_dds_kind'" />
                <xsl:with-param name="default_value" select="'BEST_EFFORT_RELIABILITY_QOS'" />
            </xsl:call-template>
        </xsl:variable>

        <xsl:variable name="max_blocking_time">
              <xsl:call-template name="get_qos_policy_data_value">
                <xsl:with-param name="qos" select="$qos" />
                <xsl:with-param name="policy_kind" select="$policy_kind" />
                <xsl:with-param name="key_kind" select="'qos_dds_max_blocking_time'" />
                <xsl:with-param name="default_value" select="'1'" />
            </xsl:call-template>
        </xsl:variable>

        <reliability
            xmlns="http://cuts.cs.iupui.edu/iccm"
            kind="{$kind}"
            max_blocking_time="{$max_blocking_time}"
            synchronous="true"
        />
    </xsl:template>

    <xsl:template name="generate_destination_order">
         <xsl:param name="qos" />
         <xsl:variable name="policy_kind" select="'DDS_DestinationOrderQosPolicy'" />

        <xsl:variable name="kind">
              <xsl:call-template name="get_qos_policy_data_value">
                <xsl:with-param name="qos" select="$qos" />
                <xsl:with-param name="policy_kind" select="$policy_kind" />
                <xsl:with-param name="key_kind" select="'qos_dds_kind'" />
                <xsl:with-param name="default_value" select="'BY_RECEPTION_TIMESTAMP_DESTINATIONORDER_QOS'" />
            </xsl:call-template>
        </xsl:variable>


        <destination_order
            xmlns="http://cuts.cs.iupui.edu/iccm"
            kind="{$kind}"
        />
    </xsl:template>

    <xsl:template name="generate_history">
         <xsl:param name="qos" />
         <xsl:variable name="policy_kind" select="'DDS_HistoryQosPolicy'" />

        <xsl:variable name="depth">
              <xsl:call-template name="get_qos_policy_data_value">
                <xsl:with-param name="qos" select="$qos" />
                <xsl:with-param name="policy_kind" select="$policy_kind" />
                <xsl:with-param name="key_kind" select="'qos_dds_depth'" />
                <xsl:with-param name="default_value" select="'1'" />
            </xsl:call-template>
        </xsl:variable>

        <xsl:variable name="kind">
              <xsl:call-template name="get_qos_policy_data_value">
                <xsl:with-param name="qos" select="$qos" />
                <xsl:with-param name="policy_kind" select="$policy_kind" />
                <xsl:with-param name="key_kind" select="'qos_dds_kind'" />
                <xsl:with-param name="default_value" select="'KEEP_LAST_HISTORY_QOS'" />
            </xsl:call-template>
        </xsl:variable>


        <history
            xmlns="http://cuts.cs.iupui.edu/iccm"
            depth="{$depth}"
            kind="{$kind}"
        />
    </xsl:template>

    <xsl:template name="generate_resource_limits">
         <xsl:param name="qos" />
         <xsl:variable name="policy_kind" select="'DDS_ResourceLimitsQosPolicy'" />

        <xsl:variable name="max_instances">
              <xsl:call-template name="get_qos_policy_data_value">
                <xsl:with-param name="qos" select="$qos" />
                <xsl:with-param name="policy_kind" select="$policy_kind" />
                <xsl:with-param name="key_kind" select="'qos_dds_max_instances'" />
                <xsl:with-param name="default_value" select="'1'" />
            </xsl:call-template>
        </xsl:variable>

        <xsl:variable name="max_samples">
              <xsl:call-template name="get_qos_policy_data_value">
                <xsl:with-param name="qos" select="$qos" />
                <xsl:with-param name="policy_kind" select="$policy_kind" />
                <xsl:with-param name="key_kind" select="'qos_dds_max_samples'" />
                <xsl:with-param name="default_value" select="'1'" />
            </xsl:call-template>
        </xsl:variable>

        <xsl:variable name="max_samples_per_instance">
              <xsl:call-template name="get_qos_policy_data_value">
                <xsl:with-param name="qos" select="$qos" />
                <xsl:with-param name="policy_kind" select="$policy_kind" />
                <xsl:with-param name="key_kind" select="'qos_dds_max_samples_per_instance'" />
                <xsl:with-param name="default_value" select="'1'" />
            </xsl:call-template>
        </xsl:variable>


        <resource_limits
            xmlns="http://cuts.cs.iupui.edu/iccm"
            initial_samples="1"
            initial_instances="1"
            max_instances="{$max_instances}"
            max_samples="{$max_samples}"
            max_samples_per_instance="{$max_samples_per_instance}"
        />
    </xsl:template>

    <xsl:template name="generate_transport_priority">
         <xsl:param name="qos" />
         <xsl:variable name="policy_kind" select="'DDS_TransportPriorityQosPolicy'" />

        <xsl:variable name="value">
              <xsl:call-template name="get_qos_policy_data_value">
                <xsl:with-param name="qos" select="$qos" />
                <xsl:with-param name="policy_kind" select="$policy_kind" />
                <xsl:with-param name="key_kind" select="'qos_dds_int_value'" />
                <xsl:with-param name="default_value" select="'1'" />
            </xsl:call-template>
        </xsl:variable>

        <transport_priority
            xmlns="http://cuts.cs.iupui.edu/iccm"
            value="1"
        />
    </xsl:template>

    <xsl:template name="generate_lifespan">
         <xsl:param name="qos" />
         <xsl:variable name="policy_kind" select="'DDS_LifespanQosPolicy'" />

        <xsl:variable name="duration">
              <xsl:call-template name="get_qos_policy_data_value">
                <xsl:with-param name="qos" select="$qos" />
                <xsl:with-param name="policy_kind" select="$policy_kind" />
                <xsl:with-param name="key_kind" select="'qos_dds_duration'" />
                <xsl:with-param name="default_value" select="'1.0'" />
            </xsl:call-template>
        </xsl:variable>

        <lifespan
            xmlns="http://cuts.cs.iupui.edu/iccm"
            duration="{$duration}"
        />
    </xsl:template>

    <xsl:template name="generate_ownership">
         <xsl:param name="qos" />
         <xsl:variable name="policy_kind" select="'DDS_OwnershipQosPolicy'" />

        <xsl:variable name="kind">
              <xsl:call-template name="get_qos_policy_data_value">
                <xsl:with-param name="qos" select="$qos" />
                <xsl:with-param name="policy_kind" select="$policy_kind" />
                <xsl:with-param name="key_kind" select="'qos_dds_kind'" />
                <xsl:with-param name="default_value" select="'SHARED_OWNERSHIP_QOS'" />
            </xsl:call-template>
        </xsl:variable>

        <ownership
            xmlns="http://cuts.cs.iupui.edu/iccm"
            kind="{$kind}"
        />
    </xsl:template>

    <xsl:template name="generate_ownership_strength">
         <xsl:param name="qos" />
         <xsl:variable name="policy_kind" select="'DDS_OwnershipStrengthQosPolicy'" />

        <xsl:variable name="value">
              <xsl:call-template name="get_qos_policy_data_value">
                <xsl:with-param name="qos" select="$qos" />
                <xsl:with-param name="policy_kind" select="$policy_kind" />
                <xsl:with-param name="key_kind" select="'qos_dds_int_value'" />
                <xsl:with-param name="default_value" select="'1.0'" />
            </xsl:call-template>
        </xsl:variable>

        <ownership_strength
            xmlns="http://cuts.cs.iupui.edu/iccm"
            value="{$value}"
        />
    </xsl:template>

    <xsl:template name="generate_writer_data_lifecycle">
         <xsl:param name="qos" />
         <xsl:variable name="policy_kind" select="'DDS_WriterDataLifecycleQosPolicy'" />

        <xsl:variable name="autodispose_unregistered_instances">
              <xsl:call-template name="get_qos_policy_data_value">
                <xsl:with-param name="qos" select="$qos" />
                <xsl:with-param name="policy_kind" select="$policy_kind" />
                <xsl:with-param name="key_kind" select="'qos_dds_autodispose_unregistered_instances'" />
                <xsl:with-param name="default_value" select="'true'" />
            </xsl:call-template>
        </xsl:variable>

        <writer_data_lifecycle
            xmlns="http://cuts.cs.iupui.edu/iccm"
            autodispose_unregistered_instances="{$autodispose_unregistered_instances}"
            autopurge_suspended_samples_delay="1.0"
			autounregister_instance_delay="1.0"
        />
    </xsl:template>

    <xsl:template name="generate_reader_data_lifecycle">
         <xsl:param name="qos" />
         <xsl:variable name="policy_kind" select="'DDS_ReaderDataLifecycleQosPolicy'" />

        <xsl:variable name="autopurge_nowriter_samples_delay">
              <xsl:call-template name="get_qos_policy_data_value">
                <xsl:with-param name="qos" select="$qos" />
                <xsl:with-param name="policy_kind" select="$policy_kind" />
                <xsl:with-param name="key_kind" select="'qos_dds_autopurge_nowriter_samples_delay'" />
                <xsl:with-param name="default_value" select="'true'" />
            </xsl:call-template>
        </xsl:variable>

        <xsl:variable name="autopurge_disposed_samples_delay">
              <xsl:call-template name="get_qos_policy_data_value">
                <xsl:with-param name="qos" select="$qos" />
                <xsl:with-param name="policy_kind" select="$policy_kind" />
                <xsl:with-param name="key_kind" select="'qos_dds_autopurge_disposed_samples_delay'" />
                <xsl:with-param name="default_value" select="'true'" />
            </xsl:call-template>
        </xsl:variable>

        <reader_data_lifecycle
            xmlns="http://cuts.cs.iupui.edu/iccm"
            enable_invalid_samples="true"
            autopurge_nowriter_samples_delay="{$autopurge_nowriter_samples_delay}"
            autopurge_disposed_samples_delay="{$autopurge_disposed_samples_delay}"
        />
    </xsl:template>

    

    

     <xsl:template name="generate_presentation">
         <xsl:param name="qos" />
         <xsl:variable name="policy_kind" select="'DDS_PresentationQosPolicy'" />

        <xsl:variable name="access_scope">
              <xsl:call-template name="get_qos_policy_data_value">
                <xsl:with-param name="qos" select="$qos" />
                <xsl:with-param name="policy_kind" select="$policy_kind" />
                <xsl:with-param name="key_kind" select="'qos_dds_access_scope'" />
                <xsl:with-param name="default_value" select="'INSTANCE_PRESENTATION_QOS'" />
            </xsl:call-template>
        </xsl:variable>

        <xsl:variable name="coherent_access">
              <xsl:call-template name="get_qos_policy_data_value">
                <xsl:with-param name="qos" select="$qos" />
                <xsl:with-param name="policy_kind" select="$policy_kind" />
                <xsl:with-param name="key_kind" select="'qos_dds_coherent_access'" />
                <xsl:with-param name="default_value" select="'true'" />
            </xsl:call-template>
        </xsl:variable>

         <xsl:variable name="ordered_access">
              <xsl:call-template name="get_qos_policy_data_value">
                <xsl:with-param name="qos" select="$qos" />
                <xsl:with-param name="policy_kind" select="$policy_kind" />
                <xsl:with-param name="key_kind" select="'qos_dds_ordered_access'" />
                <xsl:with-param name="default_value" select="'true'" />
            </xsl:call-template>
        </xsl:variable>

        <presentation
            xmlns="http://cuts.cs.iupui.edu/iccm"
            access_scope="{$access_scope}"
            coherent_access="{$coherent_access}"
            ordered_access="{$ordered_access}"
        />
    </xsl:template>

    

    


    



    
    <xsl:template name="get_qos_policy_data_value">
        <xsl:param name="qos" />
        <xsl:param name="policy_kind" />
        <xsl:param name="key_kind" />
        <xsl:param name="default_value" />

        <!-- Look for the key id for the key_kind provided -->
        <xsl:variable name="key_id">
            <xsl:call-template name="findNodeKey">
                <xsl:with-param name="attrName" select="$key_kind" />
            </xsl:call-template>	
        </xsl:variable>

        <!-- Get the policy from the $qos entity -->
        <xsl:variable name="policy" select="$qos/gml:graph/gml:node/gml:data[@key=$kind_key_id][text() = $policy_kind]/.." />
        <xsl:variable name="policy_value" select="$policy/gml:data[@key=$key_id]" />
        <!--<xsl:message>IN: <xsl:value-of select="$policy/gml:data[@key=$label_key_id]" /> KEY: <xsl:value-of select="$key_kind" /> value =  <xsl:value-of select="$policy_value" /> </xsl:message>-->
        <xsl:choose>
            <xsl:when test="$policy and not($key_id = '') and not($policy_value = '')">
                <!-- If we have a policy and a valid $key_id -->
                <xsl:value-of select="$policy_value/text()" />
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="$default_value" />
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>






<!-- Gets the ID of the QOS Profile attached to an entity.-->
<xsl:template name="getQOSProfileID">
    <xsl:param name="id" select="@id" />
    
    <xsl:variable name="gotQOSProfile" select="//gml:edge[@source=$id]/gml:data[@key=$kind_key_id][text()='Edge_QOS']/../@target" />

    <xsl:choose>
        <xsl:when test="$gotQOSProfile">
            <xsl:value-of select="$gotQOSProfile" />
        </xsl:when>
        <xsl:otherwise>
            <!-- Can't find a QOS edge, we should check our parent (Of Type Component/ComponentInstance). -->
            <xsl:variable name="parent" select="./../../gml:data[@key=$kind_key_id][contains(text(), 'Component')]/.." />

            <xsl:choose>
                <xsl:when test="$parent">
                    <xsl:for-each select="$parent">
                        <xsl:call-template name="getQOSProfileID" />
                    </xsl:for-each>
                </xsl:when>
                <xsl:otherwise>
                    <xsl:value-of select="''" />
                </xsl:otherwise>
            </xsl:choose>
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
