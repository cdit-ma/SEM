<?xml version="1.0"?>
<xsl:stylesheet version="2.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns:uml="http://schema.omg.org/spec/UML/2.1"
	xmlns:xmi="http://schema.omg.org/spec/XMI/2.1"
	xmlns:xalan="http://xml.apache.org/xslt"
	xmlns:exsl="http://exslt.org/common"
	xmlns:gml="http://graphml.graphdrawing.org/xmlns"
	exclude-result-prefixes="xmi uml xalan gml #default exsl"
>
<!-- Runtime parameters -->
<xsl:param name="class_ids" />
<xsl:include href="graphmlKeyVariables.xsl" />

<xsl:output method="xml" 
        version="1.0" 
        indent="yes" xalan:indent-amount="4"/>
	
	<xsl:strip-space elements="*" />
	
	
    <xsl:template match="/">
    <graphml
			xmlns="http://graphml.graphdrawing.org/xmlns"  
			xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
			xsi:schemaLocation="http://graphml.graphdrawing.org/xmlns http://graphml.graphdrawing.org/xmlns/1.0/graphml.xsd" >

			<!-- PICML specific attributes -->
			<key attr.name="description" attr.type="string" for="node" id="{$nodeDescriptionKey}"/>
			<key attr.name="label" attr.type="string" for="node" id="{$nodeLabelKey}"/>
			<key attr.name="x" attr.type="double" for="node" id="{$nodeXKey}"/>
			<key attr.name="y" attr.type="double" for="node" id="{$nodeYKey}"/>
			<key attr.name="kind" attr.type="string" for="node" id="{$nodeKindKey}"/>
			<key attr.name="type" attr.type="string" for="node" id="{$nodeTypeKey}"/>
			<key attr.name="value" attr.type="string" for="node" id="{$nodeValueKey}"/>
			<key attr.name="key" attr.type="boolean" for="node" id="{$nodeKeyMemberKey}"/>
			<key attr.name="width" attr.type="double" for="node" id="{$nodeWidthKey}"/>
			<key attr.name="height" attr.type="double" for="node" id="{$nodeHeightKey}"/>
			<key attr.name="original_type" attr.type="string" for="node" id="{$nodeOriginalTypeKey}"/>

			<key attr.name="sortOrder" attr.type="double" for="node" id="{$nodeSortOrderKey}"/>
			<graph edgedefault="directed" id="G">
                            <node id="M">
                                <data key="{$nodeKindKey}">Model</data>
                                <graph id="Mg">
                                    <node id="ID">
                                        <data key="{$nodeKindKey}">InterfaceDefinitions</data>
                                        <graph id="IDg">
                                            <!-- Select all packagedElement from document -->
                                            <xsl:for-each select="//packagedElement[@xmi:type='uml:Package']">
                                                    <xsl:variable name="package_name" select="@name" />
                                                    <xsl:variable name="package_id" select="@xmi:id" />
                                                    <xsl:variable name="package_classes" select="packagedElement[@xmi:type='uml:Class']" />

                                                    <xsl:call-template name="write_idl">
                                                            <xsl:with-param name="id" select="$package_id" />
                                                            <xsl:with-param name="label" select="$package_name" />
                                                    </xsl:call-template>
                                            </xsl:for-each>
                                        </graph>
                                    </node>
                                </graph>
                            </node>
			</graph>
		</graphml>
    </xsl:template>

	<xsl:template name="got_linked_type">
		<xsl:param name="type" />
        <xsl:variable name="linked_type" select="//packagedElement[@xmi:id=$type]/@name" />
		<xsl:choose>
           <xsl:when test="string-length($linked_type) > 0">
                <xsl:value-of select="true()"/>
            </xsl:when>
            <xsl:otherwise>
			  	<xsl:value-of select="false()"/>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>
	
	<xsl:template name="get_property_type">
        <xsl:param name="type" />
        <xsl:variable name="linked_type" select="//packagedElement[@xmi:id=$type]/@name" />
        <xsl:choose>
           <xsl:when test="string-length($linked_type) > 0">
                <xsl:value-of select="$linked_type"/>
            </xsl:when>
            <xsl:otherwise>
				<!-- If this item doesn't have a linked type, we should format it in the type that GraphML Understands -->
				<xsl:variable name="translated_type">
					<xsl:call-template name="get_translated_property_type">
						<xsl:with-param name="type" select="$type" />
					</xsl:call-template>
				</xsl:variable>
                <xsl:value-of select="$translated_type"/>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>

	<xsl:template name="get_property_type_id">
        <xsl:param name="type" />
        <xsl:variable name="kind" select="//packagedElement[@xmi:id=$type]/@xmi:id" />
        <xsl:choose>
            <xsl:when test="$kind">
                <xsl:value-of select="$kind"/>
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="$type"/>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>
    
    
    <xsl:template name="splitList">
		<xsl:param name="list" />
		<xsl:param name="delim" />
		
		<xsl:if test="string-length($list) > 0">
			<xsl:variable name="element" select="substring-before(concat($list, $delim), $delim)"/>
			<xsl:value-of select="concat('  ', $element, '&#xA;')"/>
			<xsl:call-template name="splitList">
				<xsl:with-param name="list" select="substring-after($list, $delim)"/>
				<xsl:with-param name="delim" select="$delim" />
			</xsl:call-template> 
		</xsl:if>
	</xsl:template>
	
	<xsl:template name="in_list">
		<xsl:param name="list" />
		<xsl:param name="element" />
		<xsl:choose>
			<xsl:when test="$list = ''">
				<xsl:value-of select="true()"/>
			</xsl:when>
			<xsl:otherwise>
				<xsl:choose>
					<xsl:when test="contains($list, $element)">
						<xsl:value-of select="true()"/>
					</xsl:when>
					<xsl:otherwise>
						<xsl:value-of select="false()"/>
					</xsl:otherwise>
				</xsl:choose>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>
        
	<xsl:template name="write_idl">
		<xsl:param name="id" />
		<xsl:param name="label" />
		<xsl:param name="aggregates" select="packagedElement[@xmi:type='uml:Class']"/>

		<xsl:message><xsl:value-of select="count($aggregates) > 0" /></xsl:message>

		<xsl:if test="count($aggregates) > 0">
			<node id="{$id}">
				<data key="{$nodeKindKey}">IDL</data>
				<data key="{$nodeLabelKey}"><xsl:value-of select="$label"/></data>
				<xsl:if test="count($aggregates) > 0">
				<graph id="{$id}g">
					<xsl:for-each select="$aggregates">
						<xsl:variable name="aggregate_label" select="@name" />
						<xsl:variable name="aggregate_id" select="@xmi:id" />
						<xsl:variable name="aggregate_members" select="ownedAttribute[@xmi:type='uml:Property' and @visibility='public']" />
						<xsl:call-template name="write_aggregate">
								<xsl:with-param name="id" select="$aggregate_id" />
								<xsl:with-param name="label" select="$aggregate_label" />
								<xsl:with-param name="members" select="$aggregate_members" />
						</xsl:call-template>
					</xsl:for-each>
					
				</graph>
				</xsl:if>
			</node>
		</xsl:if>

	</xsl:template>
	
	<xsl:template name="write_aggregate">
		<xsl:param name="id" />
		<xsl:param name="label" />
		<xsl:param name="members" />
		
		<xsl:variable name="process_member">
			<xsl:call-template name="in_list">
					<xsl:with-param name="list" select="$class_ids" />
					<xsl:with-param name="element" select="$id" />
			</xsl:call-template>
		</xsl:variable>
		<xsl:if test="$process_member='true'">
			<node id="{$id}">
				<data key="{$nodeKindKey}">Aggregate</data>
				<data key="{$nodeLabelKey}"><xsl:value-of select="$label"/></data>
				<data key="{$nodeDescriptionKey}"><xsl:value-of select="$process_member"/></data>
				<graph id="{$id}g">
					<xsl:for-each select="$members">
						<xsl:variable name="member_label" select="@name" />
						<xsl:variable name="member_id" select="@xmi:id" />
						<xsl:variable name="member_type_id" select="type/@xmi:idref" />
						
						<xsl:variable name="member_is_linked">
							<xsl:call-template name="got_linked_type">
								<xsl:with-param name="type" select="$member_type_id" />
							</xsl:call-template>
						</xsl:variable>
						
						
						
						<xsl:variable name="member_type">
							<xsl:call-template name="get_property_type">
								<xsl:with-param name="type" select="$member_type_id" />
							</xsl:call-template>
						</xsl:variable>
						
				
						<xsl:if test="$member_is_linked='true'">
							<xsl:variable name="member_aggregate_type_id">
								<xsl:call-template name="get_property_type_id">
									<xsl:with-param name="type" select="$member_type_id" />
								</xsl:call-template>
							</xsl:variable>
							<edge id="{$member_type_id}{$member_aggregate_type_id}" source="{$member_id}" target ="{$member_aggregate_type_id}" />
						</xsl:if>
						
						
						<xsl:call-template name="write_member">
								<xsl:with-param name="id" select="$member_id" />
								<xsl:with-param name="label" select="$member_label" />
								<xsl:with-param name="type" select="$member_type" />
								<xsl:with-param name="type_id" select="$member_type_id" />
								<xsl:with-param name="instance" select="$member_is_linked" />
						</xsl:call-template>
					</xsl:for-each>
				</graph>
			</node>
		</xsl:if>
	</xsl:template>

	<xsl:template name="write_member">
		<xsl:param name="id" />
		<xsl:param name="label" />
		<xsl:param name="type" />
		<xsl:param name="type_id" />
		<xsl:param name="instance" />
		
		<xsl:variable name="chosen_type">
			<xsl:choose>
            <xsl:when test="$type = 'Unknown'">
				<xsl:value-of select="'String'"/>
            </xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="$type"/>
			</xsl:otherwise>
			</xsl:choose>
		</xsl:variable>
		
		<node id="{$id}">
			<xsl:variable name="kind">
				<xsl:choose>
					<xsl:when test="$instance='true'">
						<xsl:value-of select="'AggregateInstance'"/>
					</xsl:when>
					<xsl:otherwise>
						<xsl:value-of select="'Member'"/>
					</xsl:otherwise>
				</xsl:choose>
			</xsl:variable>
			<data key="{$nodeKindKey}"><xsl:value-of select="$kind"/></data>
			<data key="{$nodeLabelKey}"><xsl:value-of select="$label"/></data>

			<data key="{$nodeTypeKey}"><xsl:value-of select="$chosen_type"/></data>
			<xsl:if test="$type = 'Unknown' and string-length($type_id) > 0">
				<data key="{$nodeOriginalTypeKey}"><xsl:value-of select="$type_id"/></data>
			</xsl:if>
			
		</node>
	</xsl:template>
	
	<!-- Translates from *** to Corba IDL Type ***-->
    <xsl:template name="get_translated_property_type">
        <xsl:param name="type" />
		<!-- Strip the EAIDL_ EAJAVA_ off the front of the variable-->
		<xsl:param name="stripped_type" select="substring-after($type, '_')" />
		<xsl:choose>
            <xsl:when test="$stripped_type = 'boolean'">
				<xsl:value-of select="'Boolean'"/>
            </xsl:when>
			<xsl:when test="$stripped_type = 'double'">
				<xsl:value-of select="'DoubleNumber'"/>
            </xsl:when>
			<xsl:when test="$stripped_type = 'unsigned short'">
				<xsl:value-of select="'UnsignedShortInteger'"/>
            </xsl:when>
			<xsl:when test="$stripped_type = 'short'">
				<xsl:value-of select="'ShortInteger'"/>
            </xsl:when>
			<xsl:when test="$stripped_type = 'long'">
				<xsl:value-of select="'LongInteger'"/>
            </xsl:when>
			<xsl:when test="$stripped_type = 'long long'">
				<xsl:value-of select="'LongLongInteger'"/>
            </xsl:when>
			<xsl:when test="$stripped_type = 'unsigned long'">
				<xsl:value-of select="'UnsignedLongInteger'"/>
            </xsl:when>
			<xsl:when test="$stripped_type = 'string'">
				<xsl:value-of select="'String'"/>
            </xsl:when>
			<xsl:when test="$stripped_type = 'float'">
				<xsl:value-of select="'FloatNumber'"/>
            </xsl:when>
			<xsl:when test="$stripped_type = 'unsigned long long'">
				<xsl:value-of select="'UnsignedLongLongInteger'"/>
            </xsl:when>
			<xsl:otherwise>
				<!-- This is an unknown type -->
				<xsl:value-of select="'Unknown'"/>
            </xsl:otherwise>
        </xsl:choose>    
    </xsl:template>
</xsl:stylesheet>

