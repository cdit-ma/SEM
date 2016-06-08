<?xml version="1.0"?>
<xsl:stylesheet version="2.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:gml="http://graphml.graphdrawing.org/xmlns"
	xmlns="http://graphml.graphdrawing.org/xmlns"
	xmlns:uml="http://schema.omg.org/spec/UML/2.1"
	xmlns:xmi="http://schema.omg.org/spec/XMI/2.1"
	xmlns:exsl="http://exslt.org/common"
	xmlns:xalan="http://xml.apache.org/xslt"
	exclude-result-prefixes="gml #default exsl xalan">

<!-- Runtime parameter - A CSV string of Class IDs to Transform -->
<!-- Defaults to a blank string -->
<xsl:param name="class_ids" select ="''" />
<xsl:param name="debug_mode" select="'false'" />

<!-- Permute the list of required class ids -->
<xsl:variable name="required_class_ids"> 
	<xsl:call-template name="get_required_ids">
		<xsl:with-param name="ids" select="$class_ids" />
	</xsl:call-template>	
</xsl:variable>



<!-- include the variables.xsl for graphml keys -->
<xsl:include href="graphmlKeyVariables.xsl" />

<xsl:output method="xml" version="1.0" indent="yes" xalan:indent-amount="4"/>

<xsl:template match="/">
    <graphml
		xmlns="http://graphml.graphdrawing.org/xmlns"  
		xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
		xsi:schemaLocation="http://graphml.graphdrawing.org/xmlns http://graphml.graphdrawing.org/xmlns/1.0/graphml.xsd">

		<!-- PICML specific attributes -->
		<key attr.name="label" attr.type="string" for="node" id="{$nodeLabelKey}"/>
		<key attr.name="kind" attr.type="string" for="node" id="{$nodeKindKey}"/>
		<key attr.name="type" attr.type="string" for="node" id="{$nodeTypeKey}"/>
		<key attr.name="key" attr.type="boolean" for="node" id="{$nodeKeyMemberKey}"/>
		<key attr.name="original_type" attr.type="string" for="node" id="{$nodeOriginalTypeKey}"/>
		<key attr.name="sortOrder" attr.type="double" for="node" id="{$nodeSortOrderKey}"/>

		<graph edgedefault="directed" id="G">
			<node id="M">
				<data key="{$nodeKindKey}">Model</data>
				<graph id="Mg">
					<node id="ID">
						<data key="{$nodeKindKey}">InterfaceDefinitions</data>
						<graph id="IDg">
							<!-- Select all uml:Package elements from Document -->
							<xsl:for-each select="//packagedElement[@xmi:type='uml:Package']">
								<xsl:call-template name="package2idl" />
							</xsl:for-each>
						</graph>
					</node>
				</graph>
			</node>
		</graph>
	</graphml>
</xsl:template>

<!-- Transform the current 'uml:Package' packagedElement element to a GraphML IDL -->
<xsl:template name="package2idl">
	<xsl:variable name="id" select="@xmi:id" />

	<!-- Run test to see if we need to process this 'uml:Package' element -->
	<xsl:variable name="is_required">
		<xsl:call-template name="requires_package" />
	</xsl:variable>

	<!-- Print debug message -->
	<xsl:if test="$debug_mode != 'false'">
		<xsl:message>uml:Package [<xsl:value-of select="$id"/>] is Required: <xsl:value-of select="$is_required"/></xsl:message>
	</xsl:if>

	<xsl:if test="$is_required = 'true'">
		<!-- Sanitize label -->
		<xsl:variable name="safe_label">
			<xsl:call-template name="sanitize_label">
				<xsl:with-param name="label" select="@name" />
			</xsl:call-template>
		</xsl:variable>

		<node id="{$id}">
			<data key="{$nodeKindKey}">IDL</data>
			<data key="{$nodeLabelKey}"><xsl:value-of select="$safe_label"/></data>
			<graph id="{$id}g">
				<!-- Select all child uml:Class elements -->
				<xsl:for-each select="packagedElement[@xmi:type = 'uml:Class']">
					<xsl:call-template name="class2aggregate" />
				</xsl:for-each>
			</graph>
		</node>
	</xsl:if>
</xsl:template>

<!-- Transform the current 'uml:Class' packagedElement element to a GraphML Aggregate -->
<xsl:template name="class2aggregate">
	<xsl:variable name="id" select="@xmi:id" />

	<!-- Run test to see if we need to process this 'uml:Class' element -->
	<xsl:variable name="is_required">
		<xsl:call-template name="requires_class" />
	</xsl:variable>

	<!-- Print debug message -->
	<xsl:if test="$debug_mode != 'false'">
		<xsl:message>uml:Class [<xsl:value-of select="$id"/>] is Required: <xsl:value-of select="$is_required"/></xsl:message>
	</xsl:if>
	
	<xsl:if test="$is_required='true'">
		<node id="{$id}">
			<!-- Sanitize label -->
			<xsl:variable name="safe_label">
				<xsl:call-template name="sanitize_label">
					<xsl:with-param name="label" select="@name" />
				</xsl:call-template>
			</xsl:variable>

			<data key="{$nodeKindKey}">Aggregate</data>
			<data key="{$nodeLabelKey}"><xsl:value-of select="$safe_label"/></data>

			<graph id="{$id}g">
				<!-- Select all child public 'uml:Property' elements -->
				<xsl:for-each select="ownedAttribute[@xmi:type='uml:Property' and @visibility='public']">
					<xsl:call-template name="property2member" />
				</xsl:for-each>
			</graph>
		</node>
	</xsl:if>
</xsl:template>

<!-- Transform the current 'uml:Property' ownedAttribute element to a GraphML AggregateInstance or Member -->
<xsl:template name="property2member">
	<xsl:variable name="id" select="@xmi:id" />
	<xsl:variable name="type_id" select="type/@xmi:idref" />

	<!-- Sanitize label -->
	<xsl:variable name="safe_label">
		<xsl:call-template name="sanitize_label">
			<xsl:with-param name="label" select="@name" />
		</xsl:call-template>
	</xsl:variable>

	<!-- Get the string version of the type-->
	<xsl:variable name="type_name">
		<xsl:call-template name="get_type_name">
			<xsl:with-param name="type_id" select="$type_id" />
		</xsl:call-template>
	</xsl:variable>

	<!-- Check to see if this has a linked type -->
	<xsl:variable name="linked_type">
		<xsl:call-template name="got_linked_type">
			<xsl:with-param name="type_id" select="$type_id" />
		</xsl:call-template>
	</xsl:variable>

	<!-- If we have got an Unknown type, set as string, else use the $type -->
	<xsl:variable name="valid_type_name">
		<xsl:choose>
			<xsl:when test="$type_name = 'Unknown'">
				<xsl:value-of select="'String'"/>
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="$type_name"/>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>
	
	<!-- Checks whether to see if this has a linked type, if it does it's an AggregateInstance, else it's a Member -->
	<xsl:variable name="kind">
		<xsl:choose>
			<xsl:when test="$linked_type = 'true'">
				<xsl:value-of select="'AggregateInstance'"/>
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="'Member'"/>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>

	<node id="{$id}">
		<data key="{$nodeKindKey}"><xsl:value-of select="$kind"/></data>
		<data key="{$nodeLabelKey}"><xsl:value-of select="$safe_label"/></data>
		<data key="{$nodeTypeKey}"><xsl:value-of select="$valid_type_name"/></data>

		<!-- Output the original $type_id if we have an Unknown type-->
		<xsl:if test="$type_name = 'Unknown' and string-length($type_id) > 0">
			<data key="{$nodeOriginalTypeKey}"><xsl:value-of select="$type_id"/></data>
		</xsl:if>
	</node>

	<!-- If this is a linked type, we should construct an edge to the definition -->
	<xsl:if test="$linked_type='true'">
		<edge id="{$id}{$type_id}" source="{$id}" target ="{$type_id}" />
	</xsl:if>
</xsl:template>

<!-- Returns a string which contains all of the ID's (Recursive) -->
<xsl:template name="get_required_ids">
	<xsl:param name="ids"/>
	<xsl:param name="required_ids" select="$ids"/>

	<!-- Get all the links/dependencies/imports for any item which has id contained in $check_ids -->
	<xsl:variable name="dependencies" select="//packagedElement[@xmi:type='uml:Dependency' and contains($ids, @client)]/@supplier" />
	<xsl:variable name="importies" select="//packagedElement[@xmi:type='uml:Class' and contains($ids, @xmi:id)]/elementImport/@importedElement" />
	<xsl:variable name="properties" select="//packagedElement[@xmi:type='uml:Class' and contains($ids, @xmi:id)]/ownedAttribute/type/@xmi:idref" />

	<!-- Combine all variables into one list -->
	<xsl:variable name="new_required_ids" select="$dependencies | $importies | $properties" />

	<xsl:choose>
		<xsl:when test="count($new_required_ids) > 0">
			<!-- Convert $new_required_ids to a string -->
			<xsl:variable name="required_ids_str">
				<xsl:call-template name="list2str">
					<xsl:with-param name="list" select="$new_required_ids" />
				</xsl:call-template>
			</xsl:variable>

			<!-- Recurse, checking for required_ids of the required_ids , which will increase the size of $required_ids -->
			<xsl:call-template name="get_required_ids">
					<xsl:with-param name="ids" select="$required_ids_str" />
					<xsl:with-param name="required_ids" select="concat($required_ids, $required_ids_str)" />
			</xsl:call-template>
		</xsl:when>
		<xsl:otherwise>
			<!-- Return the $required_ids-->
			<xsl:value-of select="$required_ids" />
		</xsl:otherwise>
	</xsl:choose>
</xsl:template>
	
<!-- Checks to see whether or not the current 'uml:Package' contains any 'uml:Class' elements which id's are in in the $required_class_ids -->
<xsl:template name="requires_package">
	<xsl:choose>
		<!-- If we have a valid $required_class_ids; check if we have any required child 'uml:class' elements -->
		<xsl:when test="$required_class_ids != ''">
			<xsl:value-of select="count(packagedElement[@xmi:type='uml:Class' and contains($required_class_ids, @xmi:id)]) > 0"/>
		</xsl:when>
		<!-- If we have an empty $required_class_ids; all 'uml:Package' elements are required -->
		<xsl:otherwise>
			<xsl:value-of select="true()"/>
		</xsl:otherwise>
	</xsl:choose>
</xsl:template>
	
<!-- Checks to see whether or not the current 'uml:Class' xmi:id attribute is in the $required_class_ids -->
<xsl:template name="requires_class">
	<xsl:variable name="id_required">
		<xsl:call-template name="contained_in_list">
				<xsl:with-param name="list" select="$required_class_ids" />
				<xsl:with-param name="element" select="@xmi:id" />
		</xsl:call-template>	
	</xsl:variable>
	<xsl:value-of select="$id_required" />
</xsl:template>

<!-- Returns the type name of the 'uml:Property' ownedAttribute element-->
<xsl:template name="get_type_name">
	<xsl:param name="type_id" />

	<!-- Check if we have a linked type -->
	<xsl:variable name="linked_type">
		<xsl:call-template name="got_linked_type">
			<xsl:with-param name="type_id" select="$type_id" />
		</xsl:call-template>
	</xsl:variable>

	<xsl:choose>
		<xsl:when test="$linked_type = 'true'">
			<xsl:variable name="safe_type_name">
				<xsl:call-template name="sanitize_label">
					<xsl:with-param name="label" select="//packagedElement[@xmi:type = 'uml:Class' and @xmi:id=$type_id]/@name" />
				</xsl:call-template>
			</xsl:variable>
			<xsl:value-of select="$safe_type_name"/>
		</xsl:when>
		<xsl:otherwise>
			<!-- If this item doesn't have a linked type, we should format it in the type that GraphML Understands -->
			<xsl:variable name="corba_type">
				<xsl:call-template name="type2corbatype">
					<xsl:with-param name="type" select="$type_id" />
				</xsl:call-template>
			</xsl:variable>
			<xsl:value-of select="$corba_type"/>
		</xsl:otherwise>
	</xsl:choose>
</xsl:template>

<!-- Checks to see if there is a 'uml:Class' packagedElement with @xmi:id = $type_id -->
<xsl:template name="got_linked_type">
	<xsl:param name="type_id" />
	<!-- Check for at least one packagedElement with the xmi:id $type_id -->
	<xsl:variable name="got_element" select="count(//packagedElement[@xmi:type = 'uml:Class' and @xmi:id=$type_id]) > 0" />
	<xsl:value-of select="$got_element"/>
</xsl:template>

<!-- TODO -->
<xsl:template name="sanitize_label">
	<xsl:param name="label" />
	<xsl:value-of select="$label"/>
</xsl:template>

<!-- Translates from *** to Corba IDL Type ***-->
<xsl:template name="type2corbatype">
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

<!-- Converts a node list into a comma-seperated string -->
<xsl:template name="list2str">
	<xsl:param name="list" />
	<xsl:for-each select="$list">
		<xsl:value-of select="." />
		<xsl:if test="position() != last()">
			<xsl:text>,</xsl:text>
		</xsl:if>
	</xsl:for-each>
</xsl:template>

<!-- Returns whether or not an element is contained in a list (or string), will return true if the list is emtpy-->
<xsl:template name="contained_in_list">
	<xsl:param name="list" />
	<xsl:param name="element" />
	<xsl:choose>
		<!-- Empty $list, so return true -->
		<xsl:when test="$list = ''">
			<xsl:value-of select="true()"/>
		</xsl:when>
		<!-- Check for $element in $list -->
		<xsl:otherwise>
			<xsl:value-of select="contains($list, $element)"/>
		</xsl:otherwise>
	</xsl:choose>
</xsl:template>
</xsl:stylesheet>