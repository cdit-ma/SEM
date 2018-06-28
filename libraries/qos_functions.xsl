<xsl:stylesheet version="2.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:xs="http://www.w3.org/2001/XMLSchema"
    xmlns:gml="http://graphml.graphdrawing.org/xmlns"
    xmlns:cdit="http://github.com/cdit-ma"
    xmlns:o="http://github.com/cdit-ma"
    >
    <xsl:import href="functions.xsl"/>

    <xsl:function name="o:tabbed_xml_comment">
        <xsl:param name="text" as="xs:string"  />
        <xsl:param name="tab" as="xs:integer"  />

        <xsl:variable name="comment" select="concat('!-- ', $text, ' --')" />

        
        <xsl:value-of select="concat(o:t($tab), o:angle_wrap($comment), o:nl(1))" />
    </xsl:function>


    


    
    <xsl:function name="cdit:generates_qos_element">
        <xsl:param name="kind"/>
        <xsl:param name="parent_tag"/>

        <xsl:variable name="is_datareader" select="$parent_tag = 'datareader_qos'" />
        <xsl:variable name="is_datawriter" select="$parent_tag = 'datawriter_qos'" />
        <xsl:variable name="is_domain" select="$parent_tag = 'domainparticipant_qos'" />
        <xsl:variable name="is_subscriber" select="$parent_tag = 'subscriber_qos'" />
        <xsl:variable name="is_publisher" select="$parent_tag = 'publisher_qos'" />
        <xsl:variable name="is_topic" select="$parent_tag = 'topic_qos'" />


        <xsl:choose>
            <xsl:when test="$kind = 'DDS_DeadlineQosPolicy'">
                <xsl:value-of select="$is_datareader or $is_datawriter or $is_topic" />
            </xsl:when>
            <xsl:when test="$kind = 'DDS_DestinationOrderQosPolicy'">
                <xsl:value-of select="$is_datareader or $is_datawriter or $is_topic" />
            </xsl:when>
            <xsl:when test="$kind = 'DDS_DurabilityQosPolicy'">
                <xsl:value-of select="$is_datareader or $is_datawriter or $is_topic" />
            </xsl:when>
            <xsl:when test="$kind = 'DDS_DurabilityServiceQosPolicy'">
                <xsl:value-of select="$is_datawriter or $is_topic" />
            </xsl:when>
            <xsl:when test="$kind = 'DDS_EntityFactoryQosPolicy'">
                <xsl:value-of select="$is_domain or $is_subscriber or $is_publisher" />
            </xsl:when>
            <xsl:when test="$kind = 'DDS_GroupDataQosPolicy'">
                <xsl:value-of select="$is_subscriber or $is_publisher" />
            </xsl:when>
            <xsl:when test="$kind = 'DDS_HistoryQosPolicy'">
                <xsl:value-of select="$is_datareader or $is_datawriter or $is_topic" />
            </xsl:when>
            <xsl:when test="$kind = 'DDS_LatencyBudgetQosPolicy'">
                <xsl:value-of select="$is_datareader or $is_datawriter or $is_topic" />
            </xsl:when> 
            <xsl:when test="$kind = 'DDS_LifespanQosPolicy'">
                <xsl:value-of select="$is_datawriter or $is_topic" />
            </xsl:when>
            <xsl:when test="$kind = 'DDS_LivelinessQosPolicy'">
                <xsl:value-of select="$is_datareader or $is_datawriter or $is_topic" />
            </xsl:when>
            <xsl:when test="$kind = 'DDS_OwnershipQosPolicy'">
                <xsl:value-of select="$is_datareader or $is_datawriter or $is_topic" />
            </xsl:when>
            <xsl:when test="$kind = 'DDS_OwnershipStrengthQosPolicy'">
                <xsl:value-of select="$is_datawriter" />
            </xsl:when>
            <xsl:when test="$kind = 'DDS_PartitionQosPolicy'">
                <xsl:value-of select="$is_subscriber or $is_publisher" />
            </xsl:when>
            <xsl:when test="$kind = 'DDS_PresentationQosPolicy'">
                <xsl:value-of select="$is_subscriber or $is_publisher" />
            </xsl:when>
            <xsl:when test="$kind = 'DDS_ReaderDataLifecycleQosPolicy'">
                <xsl:value-of select="$is_datareader" />
            </xsl:when>
            <xsl:when test="$kind = 'DDS_ReliabilityQosPolicy'">
                <xsl:value-of select="$is_datareader or $is_datawriter or $is_topic" />
            </xsl:when>
            <xsl:when test="$kind = 'DDS_ResourceLimitsQosPolicy'">
                <xsl:value-of select="$is_datareader or $is_datawriter or $is_topic" />
            </xsl:when>
            <xsl:when test="$kind = 'DDS_TimeBasedFilterQosPolicy'">
                <xsl:value-of select="$is_datareader" />
            </xsl:when>
            <xsl:when test="$kind = 'DDS_TopicDataQosPolicy'">  
                <xsl:value-of select="$is_topic" />
            </xsl:when>
            <xsl:when test="$kind = 'DDS_TransportPriorityQosPolicy'">
                <xsl:value-of select="$is_topic or $is_datawriter" />
            </xsl:when>
            <xsl:when test="$kind = 'DDS_UserDataQosPolicy'">
                <xsl:value-of select="$is_datareader or $is_datawriter or $is_domain" />
            </xsl:when>
            <xsl:when test="$kind = 'DDS_WriterDataLifecycleQosPolicy'">
                <xsl:value-of select="$is_datawriter" />
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="o:tabbed_xml_comment(concat('generates_qos_element(', $kind, ') Not Implemented!'), 0)" />
            </xsl:otherwise>
        </xsl:choose>
    </xsl:function>






    <xsl:function name="cdit:get_qos_profile">
        <xsl:param name="qos_profile" />
        <xsl:param name="middleware" />

        <xsl:variable name="has_library" select="$middleware = 'rti'" />

        <xsl:variable name="tab" select="if($has_library) then 2 else 1" />


        <xsl:variable name="label" select="cdit:get_key_value($qos_profile, 'label')" />

        <xsl:variable name="datareader_qos">
            <xsl:for-each select="$qos_profile/gml:graph[1]/gml:node">
                <xsl:value-of select="cdit:generate_qos_element(., 'datareader_qos', $tab + 2)" />
            </xsl:for-each>
        </xsl:variable>

        <xsl:variable name="datawriter_qos">
            <xsl:for-each select="$qos_profile/gml:graph[1]/gml:node">
                <xsl:value-of select="cdit:generate_qos_element(., 'datawriter_qos', $tab + 2)" />
            </xsl:for-each>
        </xsl:variable>

        <xsl:variable name="topic_qos">
            <xsl:for-each select="$qos_profile/gml:graph[1]/gml:node">
                <xsl:value-of select="cdit:generate_qos_element(., 'topic_qos', $tab + 2)" />
            </xsl:for-each>
        </xsl:variable>

        <xsl:variable name="qos">
            <xsl:value-of select="o:xml_wrap('datareader_qos', '', $datareader_qos, $tab + 1)" />
            <xsl:value-of select="o:xml_wrap('datawriter_qos', '', $datawriter_qos, $tab + 1)" />
            <xsl:value-of select="o:xml_wrap('topic_qos', '', $topic_qos, $tab + 1)" />
        </xsl:variable>

        

        

        <xsl:variable name="dds_xsi" select="o:dblquote_wrap('http://www.w3.org/2001/XMLSchema-instance')" />
        <xsl:variable name="profile" select="o:xml_wrap('qos_profile', concat('name=', o:dblquote_wrap($label)), $qos, $tab)" />

        <xsl:variable name="library">
            <xsl:choose>
                <xsl:when test="$has_library = true()">
                    <xsl:value-of select="o:xml_wrap('qos_library', concat('name=', o:dblquote_wrap($label)), $profile, 1)" />
                </xsl:when>
                <xsl:otherwise>
                    <xsl:value-of select="$profile" />
                </xsl:otherwise>
            </xsl:choose>
        </xsl:variable>
         
        <xsl:value-of select="o:xml_wrap('dds', concat('xmlns:xsi=', $dds_xsi),  $library, 0)" />
    </xsl:function>

    <xsl:function name="cdit:generate_qos_element_generic_kind">
        <xsl:param name="qos_element"/>
        <xsl:param name="name"/>
        <xsl:param name="middleware"/>
        <xsl:param name="tab"/>

        <xsl:variable name="qos_dds_kind" select="cdit:get_key_value($qos_element, 'qos_dds_kind')" />

        <xsl:variable name="kind_xml" select="o:xml_wrap('kind', '', $qos_dds_kind, $tab + 1)" />
        <xsl:value-of select="o:xml_wrap($name, '', $kind_xml, $tab)" />
    </xsl:function>

    <xsl:function name="cdit:get_qos_period">
        <xsl:param name="name"/>
        <xsl:param name="period_sec"/>
        <xsl:param name="period_nsec"/>
        <xsl:param name="middleware"/>
        <xsl:param name="tab"/>
        
        <xsl:variable name="sec_xml" select="o:sequence_to_string(o:xml_wrap('sec', '', $period_sec, $tab + 1))" />
        <xsl:variable name="nanosec_xml" select="o:sequence_to_string(o:xml_wrap('nanosec', '', $period_nsec, $tab + 1))" />
        
        <xsl:value-of select="o:xml_wrap($name, '', concat($sec_xml, $nanosec_xml), $tab)" />
    </xsl:function>

    <xsl:function name="cdit:generate_qos_element_generic_period">
        <xsl:param name="qos_element"/>
        <xsl:param name="name"/>
        <xsl:param name="period_name"/>
        <xsl:param name="period_key"/>
        <xsl:param name="middleware"/>
        <xsl:param name="tab"/>

        <xsl:variable name="qos_dds_period_sec" select="cdit:get_key_value($qos_element, concat($period_key, '_sec'))" />
        <xsl:variable name="qos_dds_period_nsec" select="cdit:get_key_value($qos_element, concat($period_key, '_nanosec'))" />

        <xsl:variable name="period_xml" select="cdit:get_qos_period($period_name, $qos_dds_period_sec, $qos_dds_period_nsec, $middleware, $tab + 1)" />
        <xsl:value-of select="o:xml_wrap($name, '', $period_xml, $tab)" />
    </xsl:function>

    <xsl:function name="cdit:generate_qos_durability_service">
        <xsl:param name="qos_element"/>
        <xsl:param name="middleware"/>
        <xsl:param name="tab"/>

        <xsl:variable name="contents_xml">
            <xsl:variable name="qos_dds_sec" select="cdit:get_key_value($qos_element, 'qos_dds_service_cleanup_delay_sec')" />
            <xsl:variable name="qos_dds_nsec" select="cdit:get_key_value($qos_element, 'qos_dds_service_cleanup_delay_nanosec')" />

            <!-- service_cleanup_delay -->
            <xsl:value-of select="cdit:get_qos_period('service_cleanup_delay', $qos_dds_sec, $qos_dds_nsec, $middleware, $tab + 1)" />
            <!-- history_kind -->
            <xsl:value-of select="o:xml_wrap('history_kind', '', cdit:get_key_value($qos_element, 'qos_dds_history_kind'), $tab + 1)" />
            <!-- history_depth -->
            <xsl:value-of select="o:xml_wrap('history_depth', '', cdit:get_key_value($qos_element, 'qos_dds_history_depth'), $tab + 1)" />
            <!-- max_samples -->
            <xsl:value-of select="o:xml_wrap('max_samples', '', cdit:get_key_value($qos_element, 'qos_dds_max_samples'), $tab + 1)" />
            <!-- max_instances -->
            <xsl:value-of select="o:xml_wrap('max_instances', '', cdit:get_key_value($qos_element, 'qos_dds_max_instances'), $tab + 1)" />
            <!-- max_samples_per_instance -->
            <xsl:value-of select="o:xml_wrap('max_samples_per_instance', '', cdit:get_key_value($qos_element, 'qos_dds_max_samples_per_instance'), $tab + 1)" />
        </xsl:variable>
        <xsl:value-of select="o:xml_wrap('durability_service', '', $contents_xml, $tab)" />
    </xsl:function>

    <xsl:function name="cdit:generate_qos_liveliness">
        <xsl:param name="qos_element"/>
        <xsl:param name="middleware"/>
        <xsl:param name="tab"/>

        <xsl:variable name="contents_xml">
             <!-- kind -->
            <xsl:value-of select="o:xml_wrap('kind', '', cdit:get_key_value($qos_element, 'qos_dds_kind'), $tab + 1)" />

            <xsl:variable name="qos_dds_sec" select="cdit:get_key_value($qos_element, 'qos_dds_lease_duration_sec')" />
            <xsl:variable name="qos_dds_nsec" select="cdit:get_key_value($qos_element, 'qos_dds_lease_duration_nanosec')" />
            <!-- lease_duration -->
            <xsl:value-of select="cdit:get_qos_period('lease_duration', $qos_dds_sec, $qos_dds_nsec, $middleware, $tab + 1)" />
        </xsl:variable>
        <xsl:value-of select="o:xml_wrap('liveliness', '', $contents_xml, $tab)" />
    </xsl:function>

    <xsl:function name="cdit:generate_qos_presentation">
        <xsl:param name="qos_element"/>
        <xsl:param name="middleware"/>
        <xsl:param name="tab"/>

        <xsl:variable name="contents_xml">
             <!-- access_scope -->
            <xsl:value-of select="o:xml_wrap('access_scope', '', cdit:get_key_value($qos_element, 'qos_dds_access_scope'), $tab + 1)" />
            <!-- coherent_access -->
            <xsl:value-of select="o:xml_wrap('coherent_access', '', cdit:get_key_value($qos_element, 'qos_dds_coherent_access'), $tab + 1)" />
            <!-- ordered_access -->
            <xsl:value-of select="o:xml_wrap('ordered_access', '', cdit:get_key_value($qos_element, 'qos_dds_ordered_access'), $tab + 1)" />
        </xsl:variable>
        <xsl:value-of select="o:xml_wrap('presentation', '', $contents_xml, $tab)" />
    </xsl:function>

    <xsl:function name="cdit:generate_qos_reader_data_lifecycle">
        <xsl:param name="qos_element"/>
        <xsl:param name="middleware"/>
        <xsl:param name="tab"/>

        <xsl:variable name="contents_xml">
            <xsl:variable name="qos_dds_1_sec" select="cdit:get_key_value($qos_element, 'qos_dds_autopurge_disposed_samples_delay_sec')" />
            <xsl:variable name="qos_dds_1_nsec" select="cdit:get_key_value($qos_element, 'qos_dds_autopurge_disposed_samples_delay_nanosec')" />
            <xsl:variable name="qos_dds_2_sec" select="cdit:get_key_value($qos_element, 'qos_dds_autopurge_nowriter_samples_delay_sec')" />
            <xsl:variable name="qos_dds_2_nsec" select="cdit:get_key_value($qos_element, 'qos_dds_autopurge_nowriter_samples_delay_nanosec')" />


            <!-- autopurge_nowriter_samples_delay -->
            <xsl:value-of select="cdit:get_qos_period('autopurge_nowriter_samples_delay', $qos_dds_1_sec, $qos_dds_1_nsec, $middleware, $tab + 1)" />
            <!-- autopurge_disposed_samples_delay -->
            <xsl:value-of select="cdit:get_qos_period('autopurge_disposed_samples_delay', $qos_dds_2_sec, $qos_dds_2_nsec, $middleware, $tab + 1)" />
        </xsl:variable>
        <xsl:value-of select="o:xml_wrap('reader_data_lifecycle', '', $contents_xml, $tab)" />
    </xsl:function>

    <xsl:function name="cdit:generate_qos_reliability">
        <xsl:param name="qos_element"/>
        <xsl:param name="middleware"/>
        <xsl:param name="tab"/>

        <xsl:variable name="contents_xml">
            <!-- kind -->
            <xsl:value-of select="o:xml_wrap('kind', '', cdit:get_key_value($qos_element, 'qos_dds_kind'), $tab + 1)" />

            <xsl:variable name="qos_dds_sec" select="cdit:get_key_value($qos_element, 'qos_dds_max_blocking_time_sec')" />
            <xsl:variable name="qos_dds_nsec" select="cdit:get_key_value($qos_element, 'qos_dds_max_blocking_time_nanosec')" />

            <!-- max_blocking_time -->
            <xsl:value-of select="cdit:get_qos_period('max_blocking_time', $qos_dds_sec, $qos_dds_nsec, $middleware, $tab + 1)" />
        </xsl:variable>
        <xsl:value-of select="o:xml_wrap('reliability', '', $contents_xml, $tab)" />
    </xsl:function>

    <xsl:function name="cdit:generate_qos_resource_limits">
        <xsl:param name="qos_element"/>
        <xsl:param name="middleware"/>
        <xsl:param name="tab"/>

        <xsl:variable name="contents_xml">
            <!-- max_samples -->
            <xsl:value-of select="o:xml_wrap('max_samples', '', cdit:get_key_value($qos_element, 'qos_dds_max_samples'), $tab + 1)" />
            <!-- max_instances -->
            <xsl:value-of select="o:xml_wrap('max_instances', '', cdit:get_key_value($qos_element, 'qos_dds_max_instances'), $tab + 1)" />
            <!-- max_samples_per_instance -->
            <xsl:value-of select="o:xml_wrap('max_samples_per_instance', '', cdit:get_key_value($qos_element, 'qos_dds_max_samples_per_instance'), $tab + 1)" />
        </xsl:variable>
        <xsl:value-of select="o:xml_wrap('resource_limits', '', $contents_xml, $tab)" />
    </xsl:function>

    


    

    

    

    

    <xsl:function name="cdit:generate_qos_history">
        <xsl:param name="qos_element"/>
        <xsl:param name="middleware"/>
        <xsl:param name="tab"/>

        <xsl:variable name="contents_xml">
            <!-- kind -->
            <xsl:value-of select="o:xml_wrap('kind', '', cdit:get_key_value($qos_element, 'qos_dds_kind'), $tab + 1)" />
            <!-- depth -->
            <xsl:value-of select="o:xml_wrap('depth', '', cdit:get_key_value($qos_element, 'qos_dds_depth'), $tab + 1)" />
        </xsl:variable>
        <xsl:value-of select="o:xml_wrap('history', '', $contents_xml, $tab)" />
    </xsl:function>


    <xsl:function name="cdit:generate_qos_generic_single_value">
        <xsl:param name="qos_element"/>
        <xsl:param name="name"/>
        <xsl:param name="key_name"/>
        <xsl:param name="key_graphml"/>
        <xsl:param name="middleware"/>
        <xsl:param name="tab"/>

        <xsl:variable name="contents_xml">
            <!-- autoenable_created_entities -->
            <xsl:value-of select="o:xml_wrap($key_name, '', cdit:get_key_value($qos_element, $key_graphml), $tab + 1)" />
        </xsl:variable>
        <xsl:value-of select="o:xml_wrap($name, '', $contents_xml, $tab)" />
    </xsl:function>


    

    <xsl:function name="cdit:generate_qos_entity_factory">
        <xsl:param name="qos_element"/>
        <xsl:param name="middleware"/>
        <xsl:param name="tab"/>

        <xsl:variable name="contents_xml">
            <!-- autoenable_created_entities -->
            <xsl:value-of select="o:xml_wrap('autoenable_created_entities', '', cdit:get_key_value($qos_element, 'qos_dds_autoenable_created_entities'), $tab + 1)" />
        </xsl:variable>
        <xsl:value-of select="o:xml_wrap('entity_factory', '', $contents_xml, $tab)" />
    </xsl:function>

    <xsl:function name="cdit:generate_qos_group_data">
        <xsl:param name="qos_element"/>
        <xsl:param name="middleware"/>
        <xsl:param name="tab"/>

        <xsl:variable name="contents_xml">
            <!-- autoenable_created_entities -->
            <xsl:value-of select="o:xml_wrap('value', '', cdit:get_key_value($qos_element, 'value'), $tab + 1)" />
        </xsl:variable>
        <xsl:value-of select="o:xml_wrap('group_data', '', $contents_xml, $tab)" />
    </xsl:function>
    
    

    


    <xsl:function name="cdit:generate_qos_element">
        <xsl:param name="qos_element"/>
        <xsl:param name="parent_tag"/>
        <xsl:param name="tab"/>

        <xsl:variable name="middleware" select="''" />
        <xsl:variable name="id" select="cdit:get_node_id($qos_element)" />
        <xsl:variable name="kind" select="cdit:get_key_value($qos_element, 'kind')" />
        <xsl:variable name="label" select="cdit:get_key_value($qos_element, 'label')" />


        <xsl:if test="cdit:generates_qos_element($kind, $parent_tag) = true()">
            <xsl:choose>
                <xsl:when test="$kind = 'DDS_DeadlineQosPolicy'">
                    <xsl:value-of select="cdit:generate_qos_element_generic_period($qos_element, 'deadline', 'period', 'qos_dds_period', $middleware, $tab)" />
                </xsl:when>
                <xsl:when test="$kind = 'DDS_DestinationOrderQosPolicy'">
                    <xsl:value-of select="cdit:generate_qos_generic_single_value($qos_element, 'destination_order', 'kind', 'qos_dds_kind', $middleware, $tab)" />
                </xsl:when>
                <xsl:when test="$kind = 'DDS_DurabilityQosPolicy'">
                    <xsl:value-of select="cdit:generate_qos_generic_single_value($qos_element, 'durability', 'kind', 'qos_dds_kind', $middleware, $tab)" />
                </xsl:when>
                <xsl:when test="$kind = 'DDS_DurabilityServiceQosPolicy'">
                    <xsl:value-of select="cdit:generate_qos_durability_service($qos_element, $middleware, $tab)" />
                </xsl:when>
                <xsl:when test="$kind = 'DDS_EntityFactoryQosPolicy'">
                    <xsl:value-of select="cdit:generate_qos_entity_factory($qos_element, $middleware, $tab)" />
                </xsl:when>
                <xsl:when test="$kind = 'DDS_GroupDataQosPolicy'">
                    <xsl:value-of select="cdit:generate_qos_generic_single_value($qos_element, 'group_data', 'value', 'qos_dds_str_value', $middleware, $tab)" />
                </xsl:when>
                <xsl:when test="$kind = 'DDS_HistoryQosPolicy'">
                    <xsl:value-of select="cdit:generate_qos_history($qos_element, $middleware, $tab)" />
                </xsl:when>
                <xsl:when test="$kind = 'DDS_LatencyBudgetQosPolicy'">
                    <xsl:value-of select="cdit:generate_qos_element_generic_period($qos_element, 'latency_budget', 'duration', 'qos_dds_duration', $middleware, $tab)" />
                </xsl:when>
                <xsl:when test="$kind = 'DDS_LifespanQosPolicy'">
                    <xsl:value-of select="cdit:generate_qos_element_generic_period($qos_element, 'lifespan', 'duration', 'qos_dds_duration', $middleware, $tab)" />
                </xsl:when>
                <xsl:when test="$kind = 'DDS_LivelinessQosPolicy'">
                    <xsl:value-of select="cdit:generate_qos_liveliness($qos_element, $middleware, $tab)" />
                </xsl:when>
                <xsl:when test="$kind = 'DDS_OwnershipQosPolicy'">
                    <xsl:value-of select="cdit:generate_qos_generic_single_value($qos_element, 'ownership', 'kind', 'qos_dds_kind', $middleware, $tab)" />
                </xsl:when>
                <xsl:when test="$kind = 'DDS_OwnershipStrengthQosPolicy'">
                    <xsl:value-of select="cdit:generate_qos_generic_single_value($qos_element, 'ownership_strength', 'value', 'qos_dds_int_value', $middleware, $tab)" />
                </xsl:when>
                <xsl:when test="$kind = 'DDS_PartitionQosPolicy'">
                    <xsl:value-of select="cdit:generate_qos_generic_single_value($qos_element, 'partition', 'name', 'qos_dds_name', $middleware, $tab)" />
                </xsl:when>
                <xsl:when test="$kind = 'DDS_PresentationQosPolicy'">
                    <xsl:value-of select="cdit:generate_qos_presentation($qos_element, $middleware, $tab)" />
                </xsl:when>
                <xsl:when test="$kind = 'DDS_ReaderDataLifecycleQosPolicy'">
                    <xsl:value-of select="cdit:generate_qos_reader_data_lifecycle($qos_element, $middleware, $tab)" />
                </xsl:when>
                <xsl:when test="$kind = 'DDS_ReliabilityQosPolicy'">
                    <xsl:value-of select="cdit:generate_qos_reliability($qos_element, $middleware, $tab)" />
                </xsl:when>
                <xsl:when test="$kind = 'DDS_ResourceLimitsQosPolicy'">
                    <xsl:value-of select="cdit:generate_qos_resource_limits($qos_element, $middleware, $tab)" />
                </xsl:when>
                <xsl:when test="$kind = 'DDS_TimeBasedFilterQosPolicy'">
                    <xsl:value-of select="cdit:generate_qos_element_generic_period($qos_element, 'time_based_filter', 'minimum_separation', 'qos_dds_minimum_separation', $middleware, $tab)" />
                </xsl:when>
                <xsl:when test="$kind = 'DDS_TopicDataQosPolicy'">
                    <xsl:value-of select="cdit:generate_qos_generic_single_value($qos_element, 'topic_data', 'value', 'qos_dds_str_value', $middleware, $tab)" />
                </xsl:when>
                <xsl:when test="$kind = 'DDS_TransportPriorityQosPolicy'">
                    <xsl:value-of select="cdit:generate_qos_generic_single_value($qos_element, 'transport_priority', 'value', 'qos_dds_int_value', $middleware, $tab)" />
                </xsl:when>
                <xsl:when test="$kind = 'DDS_UserDataQosPolicy'">
                    <xsl:value-of select="cdit:generate_qos_generic_single_value($qos_element, 'user_data', 'value', 'qos_dds_str_value', $middleware, $tab)" />
                </xsl:when>
                <xsl:when test="$kind = 'DDS_WriterDataLifecycleQosPolicy'">
                    <xsl:value-of select="cdit:generate_qos_generic_single_value($qos_element, 'writer_data_lifecycle', 'autodispose_unregistered_instances', 'qos_dds_autodispose_unregistered_instances', $middleware, $tab)" />
                </xsl:when>
                <xsl:otherwise>
                    <xsl:value-of select="o:tabbed_xml_comment(concat('generate_qos_element(', $kind, ') Not Implemented!'), $tab)" />
                </xsl:otherwise>
            </xsl:choose>
        </xsl:if>
    </xsl:function>



</xsl:stylesheet>