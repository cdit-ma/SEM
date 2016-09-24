<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:gml="http://graphml.graphdrawing.org/xmlns"

	xmlns:exsl="http://exslt.org/common"
	xmlns:xalan="http://xml.apache.org/xslt"	
	xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" 
	xsi:schemaLocation="http://www.omg.org/Deployment Deployment.xsd"
	xmlns:redirect = "org.apache. xalan.xslt.extensions.Redirect"
	
	exclude-result-prefixes="gml exsl xalan">
		
    <xsl:output method="text" 
		omit-xml-declaration="yes"
        indent="no" />
	<xsl:strip-space elements="*" />
	
    <!--
        Purpose:

        This template will convert the .graphml project to the selected cpp file. 
		This transform can be used for component implementations. The driving program must
		make the selection of which nodes to process for a given deployment. 
		That is we need to process all "<component>Impl.cpp" transforms for a model/project.
    -->
	
	<!-- Runtime parameters -->
	<xsl:param name="File"></xsl:param>
	
	<!-- Assign default data keys as used by yEd 3.12.2 -->
	<xsl:include href="graphmlKeyVariables.xsl" />
	
	<!-- Assign global variables for deployment       -->
	<xsl:variable name="implSuffix" select="'_impl'" />
	<xsl:variable name="svntSuffix" select="'_svnt'" />
	<xsl:variable name="ImplSuffix" select="'Impl'" />
	<xsl:variable name="ServantSuffix" select="'Servant'" />
	<xsl:variable name="stubSuffix" select="'_stub'" />
	<xsl:variable name="skelSuffix" select="'_skel'" />
	<xsl:variable name="execSuffix" select="'_exec'" />
	<xsl:variable name="eidlSuffix" select="'_EIDL_Gen'" />
	<xsl:variable name="EventSuffix" select="'Event'" />

	<!-- apply all templates starting from the root -->
    <xsl:template match="/">
		<!-- Assign the transform node key ids from existing keys -->
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
		
		<xsl:variable name="transformNodeKeyMemberKey">
			<xsl:call-template name="findNodeKey">
				<xsl:with-param name="attrName" select="'key'" />
				<xsl:with-param name="defaultId" select="$nodeKeyMemberKey" />
			</xsl:call-template>	
		</xsl:variable>			
		
		<xsl:variable name="transformNodeFolderKey">
			<xsl:call-template name="findNodeKey">
				<xsl:with-param name="attrName" select="'folder'" />
				<xsl:with-param name="defaultId" select="$nodeFolderKey" />
			</xsl:call-template>	
		</xsl:variable>			

		<xsl:variable name="transformNodeFileKey">
			<xsl:call-template name="findNodeKey">
				<xsl:with-param name="attrName" select="'file'" />
				<xsl:with-param name="defaultId" select="$nodeFolderKey" />
			</xsl:call-template>	
		</xsl:variable>	

		<xsl:variable name="transformNodeWorkerKey">
			<xsl:call-template name="findNodeKey">
				<xsl:with-param name="attrName" select="'worker'" />
				<xsl:with-param name="defaultId" select="$nodeWorkerKey" />
			</xsl:call-template>	
		</xsl:variable>	

		<xsl:variable name="transformNodeWorkerIDKey">
			<xsl:call-template name="findNodeKey">
				<xsl:with-param name="attrName" select="'workerID'" />
				<xsl:with-param name="defaultId" select="$nodeWorkerIDKey" />
			</xsl:call-template>	
		</xsl:variable>	
		
		<xsl:variable name="transformNodeOperationKey">
			<xsl:call-template name="findNodeKey">
				<xsl:with-param name="attrName" select="'operation'" />
				<xsl:with-param name="defaultId" select="$nodeOperationKey" />
			</xsl:call-template>	
		</xsl:variable>	

		<xsl:variable name="transformNodeComplexityKey">
			<xsl:call-template name="findNodeKey">
				<xsl:with-param name="attrName" select="'complexity'" />
				<xsl:with-param name="defaultId" select="$nodeComplexityKey" />
			</xsl:call-template>	
		</xsl:variable>	

		<xsl:variable name="transformNodeComplexityParametersKey">
			<xsl:call-template name="findNodeKey">
				<xsl:with-param name="attrName" select="'complexityParameters'" />
				<xsl:with-param name="defaultId" select="$nodeComplexityParametersKey" />
			</xsl:call-template>	
		</xsl:variable>	
		
		<xsl:variable name="transformNodeParametersKey">
			<xsl:call-template name="findNodeKey">
				<xsl:with-param name="attrName" select="'parameters'" />
				<xsl:with-param name="defaultId" select="$nodeParametersKey" />
			</xsl:call-template>	
		</xsl:variable>	
		
		<xsl:variable name="transformNodeMiddlewareKey">
			<xsl:call-template name="findNodeKey">
				<xsl:with-param name="attrName" select="'middleware'" />
				<xsl:with-param name="defaultId" select="$nodeMiddlewareKey" />
			</xsl:call-template>	
		</xsl:variable>	
		
		<xsl:variable name="transformNodeAsyncKey">
			<xsl:call-template name="findNodeKey">
				<xsl:with-param name="attrName" select="'async'" />
				<xsl:with-param name="defaultId" select="$nodeAsyncKey" />
			</xsl:call-template>	
		</xsl:variable>	
		
		<xsl:variable name="transformNodeTypeKey">
			<xsl:call-template name="findNodeKey">
				<xsl:with-param name="attrName" select="'type'" />
				<xsl:with-param name="defaultId" select="$nodeTypeKey" />
			</xsl:call-template>	
		</xsl:variable>	

		<xsl:variable name="transformNodeActionOnKey">
			<xsl:call-template name="findNodeKey">
				<xsl:with-param name="attrName" select="'actionOn'" />
				<xsl:with-param name="defaultId" select="$nodeActionOnKey" />
			</xsl:call-template>	
		</xsl:variable>	

		<xsl:variable name="transformNodeValueKey">
			<xsl:call-template name="findNodeKey">
				<xsl:with-param name="attrName" select="'value'" />
				<xsl:with-param name="defaultId" select="$nodeValueKey" />
			</xsl:call-template>	
		</xsl:variable>	

		<xsl:variable name="transformNodeCodeKey">
			<xsl:call-template name="findNodeKey">
				<xsl:with-param name="attrName" select="'code'" />
				<xsl:with-param name="defaultId" select="$nodeCodeKey" />
			</xsl:call-template>	
		</xsl:variable>	

		<xsl:variable name="transformNodeFrequencyKey">
			<xsl:call-template name="findNodeKey">
				<xsl:with-param name="attrName" select="'frequency'" />
				<xsl:with-param name="defaultId" select="$nodeFrequencyKey" />
			</xsl:call-template>	
		</xsl:variable>	
		
		<xsl:variable name="transformNodeSortOrderKey">
			<xsl:call-template name="findNodeKey">
				<xsl:with-param name="attrName" select="'sortOrder'" />
				<xsl:with-param name="defaultId" select="$nodeSortOrderKey" />
			</xsl:call-template>	
		</xsl:variable>

		<!-- find requested Component node -->
		<xsl:variable name="nodeName" >
			<xsl:choose>
			<xsl:when test="contains($File, $ImplSuffix)" >
				<xsl:value-of select="substring-before($File, $ImplSuffix)" />
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="substring-before(concat($File, '.'),'.')" />
			</xsl:otherwise>
			</xsl:choose>
		</xsl:variable>
		<xsl:variable name="componentNode" select="/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'Component']/../gml:data[@key=$transformNodeLabelKey][text() = $nodeName]/..
												 | /descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'BlackBox']/../gml:data[@key=$transformNodeLabelKey][text() = $nodeName]/.." />
		<!-- find Behaviour/Workload Definition for this Component node -->
		<xsl:variable name="behaviourDefs" select="/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'BehaviourDefinitions']/.." />
		<xsl:variable name="implNodeId" select="/descendant::*/gml:edge[@target=$componentNode/@id]/@source" />
		<xsl:variable name="implNode" select="$behaviourDefs/descendant::*/gml:node[@id=$implNodeId]/gml:data[@key=$transformNodeKindKey][text() = 'ComponentImpl']/.." />
		<xsl:variable name="implName" select="concat($nodeName, $ImplSuffix)" />
		
		<!-- Prologue Generate -->
		<!-- Write the prologue for the file. -->
		<!-- Generate the hash definition for this file. -->
		<xsl:variable name="uppercaseFile" select="translate($implName,
                                'abcdefghijklmnopqrstuvwxyz',
                                'ABCDEFGHIJKLMNOPQRSTUVWXYZ')" />
								
		<!-- Begin -->
		<xsl:variable name="refFile" select="$componentNode/ancestor::*/gml:data[@key=$transformNodeKindKey][text() = 'IDL']/.." />
		<xsl:variable name="includeFileName" select="$refFile/gml:data[@key=$transformNodeLabelKey]/text()" />
		<xsl:variable name="includeFolderName" select="$refFile/gml:data[@key=$transformNodeFolderKey]/text()" />
		<xsl:variable name="includename">
			<xsl:choose>
			<xsl:when test="$includeFolderName != ''" >
				<xsl:value-of select="concat($includeFolderName, '/', $includeFileName)" />
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="$includeFileName" />
			</xsl:otherwise>
			</xsl:choose>
		</xsl:variable>
		<xsl:value-of select="'// This file was generated by:&#xA;'"/>
		<xsl:value-of select="'// graphml2cpp.xsl&#xA;&#xA;'"/>  
		<xsl:value-of select="concat('#include &quot;', $implName, '.h&quot;&#xA;')"/>
		<!-- @todo We should only include the following header if there are
		///       are output events (or output ports) for this component.
		///       Otherwise, we can leave this include out of this source. -->
		<xsl:value-of select="'#include &quot;cuts/arch/ccm/CCM_Events_T.h&quot;&#xA;&#xA;'"/>

		<!-- Figure out what type of component we are implementing. Right
			now there is a one-to-one implementation to component type
			mapping. Therefore, the component has the known behavior
			for this respective implementation. -->	
		<xsl:choose>
		<xsl:when test="count($implNode) &gt; 0" >
			<xsl:call-template name="Impl_Generator">
				<xsl:with-param name="node" select="$componentNode" />
				<xsl:with-param name="implNode" select="$implNode" />
				<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey"/>
				<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey"/>
				<xsl:with-param name="transformNodeFolderKey" select="$transformNodeFolderKey"/>
				<xsl:with-param name="transformNodeWorkerKey" select="$transformNodeWorkerKey"/>
				<xsl:with-param name="transformNodeWorkerIDKey" select="$transformNodeWorkerIDKey"/>
				<xsl:with-param name="transformNodeOperationKey" select="$transformNodeOperationKey"/>
				<xsl:with-param name="transformNodeComplexityKey" select="$transformNodeComplexityKey"/>
				<xsl:with-param name="transformNodeComplexityParametersKey" select="$transformNodeComplexityParametersKey"/>
				<xsl:with-param name="transformNodeParametersKey" select="$transformNodeParametersKey"/>
				<xsl:with-param name="transformNodeMiddlewareKey" select="$transformNodeMiddlewareKey"/>
				<xsl:with-param name="transformNodeAsyncKey" select="$transformNodeAsyncKey"/>
				<xsl:with-param name="transformNodeTypeKey" select="$transformNodeTypeKey"/>
				<xsl:with-param name="transformNodeActionOnKey" select="$transformNodeActionOnKey"/>
				<xsl:with-param name="transformNodeValueKey" select="$transformNodeValueKey"/>
				<xsl:with-param name="transformNodeCodeKey" select="$transformNodeCodeKey"/>
				<xsl:with-param name="transformNodeFrequencyKey" select="$transformNodeFrequencyKey"/>
				<xsl:with-param name="transformNodeSortOrderKey" select="$transformNodeSortOrderKey"/>
			</xsl:call-template>	
		</xsl:when>

		<xsl:when test="count($implNode) = 0" >
			<xsl:call-template name="Wrap_Generator">
				<xsl:with-param name="node" select="$componentNode" />
				<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey"/>
				<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey"/>
				<xsl:with-param name="transformNodeFolderKey" select="$transformNodeFolderKey"/>
				<xsl:with-param name="transformNodeWorkerKey" select="$transformNodeWorkerKey"/>
				<xsl:with-param name="transformNodeWorkerIDKey" select="$transformNodeWorkerIDKey"/>
				<xsl:with-param name="transformNodeOperationKey" select="$transformNodeOperationKey"/>
				<xsl:with-param name="transformNodeComplexityKey" select="$transformNodeComplexityKey"/>
				<xsl:with-param name="transformNodeComplexityParametersKey" select="$transformNodeComplexityParametersKey"/>
				<xsl:with-param name="transformNodeParametersKey" select="$transformNodeParametersKey"/>
				<xsl:with-param name="transformNodeMiddlewareKey" select="$transformNodeMiddlewareKey"/>
				<xsl:with-param name="transformNodeAsyncKey" select="$transformNodeAsyncKey"/>
				<xsl:with-param name="transformNodeTypeKey" select="$transformNodeTypeKey"/>
				<xsl:with-param name="transformNodeActionOnKey" select="$transformNodeActionOnKey"/>
				<xsl:with-param name="transformNodeValueKey" select="$transformNodeValueKey"/>
				<xsl:with-param name="transformNodeCodeKey" select="$transformNodeCodeKey"/>
				<xsl:with-param name="transformNodeFrequencyKey" select="$transformNodeFrequencyKey"/>
				<xsl:with-param name="transformNodeSortOrderKey" select="$transformNodeSortOrderKey"/>
			</xsl:call-template>	
		</xsl:when>
		</xsl:choose>
		
		<!-- Write the epilogue for the file, then close it. -->
		<!-- Epilogue_Generate -->
		<xsl:value-of select="'&#xA;// end of auto-generated file&#xA;&#xA;'"/>
    </xsl:template>
		
	<!-- Component Implementation cpp file -->
	<xsl:template name="Impl_Generator">
		<xsl:param name="node" />
		<xsl:param name="implNode" />
		<xsl:param name="transformNodeKindKey" />
		<xsl:param name="transformNodeLabelKey" />
		<xsl:param name="transformNodeFolderKey" />
		<xsl:param name="transformNodeWorkerKey" />
		<xsl:param name="transformNodeWorkerIDKey" />
		<xsl:param name="transformNodeOperationKey" />
		<xsl:param name="transformNodeComplexityKey" />
		<xsl:param name="transformNodeComplexityParametersKey" />
		<xsl:param name="transformNodeParametersKey" />
		<xsl:param name="transformNodeMiddlewareKey" />
		<xsl:param name="transformNodeAsyncKey" />
		<xsl:param name="transformNodeTypeKey" />
		<xsl:param name="transformNodeActionOnKey" />
		<xsl:param name="transformNodeValueKey" />
		<xsl:param name="transformNodeCodeKey" />
		<xsl:param name="transformNodeFrequencyKey" />
		<xsl:param name="transformNodeSortOrderKey" />
		
		<!-- Extract the component type being implemented. -->
		<xsl:call-template name="Component_Impl_Begin">
			<xsl:with-param name="node" select="$node" />
			<xsl:with-param name="implNode" select="$implNode" />
			<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey"/>
			<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey"/>
			<xsl:with-param name="transformNodeMiddlewareKey" select="$transformNodeMiddlewareKey"/>
			<xsl:with-param name="transformNodeAsyncKey" select="$transformNodeAsyncKey"/>
			<xsl:with-param name="transformNodeValueKey" select="$transformNodeValueKey"/>
			<xsl:with-param name="transformNodeFrequencyKey" select="$transformNodeFrequencyKey"/>
			<xsl:with-param name="transformNodeTypeKey" select="$transformNodeTypeKey"/>
		</xsl:call-template>	
		
		<!-- Visit the component. -->
		<xsl:call-template name="Visit_Component">
			<xsl:with-param name="node" select="$node" />
			<xsl:with-param name="implNode" select="$implNode" />
			<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey"/>
			<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey"/>
			<xsl:with-param name="transformNodeFolderKey" select="$transformNodeFolderKey"/>
			<xsl:with-param name="transformNodeWorkerKey" select="$transformNodeWorkerKey"/>
			<xsl:with-param name="transformNodeWorkerIDKey" select="$transformNodeWorkerIDKey"/>
			<xsl:with-param name="transformNodeOperationKey" select="$transformNodeOperationKey"/>
			<xsl:with-param name="transformNodeComplexityKey" select="$transformNodeComplexityKey"/>
			<xsl:with-param name="transformNodeComplexityParametersKey" select="$transformNodeComplexityParametersKey"/>
			<xsl:with-param name="transformNodeParametersKey" select="$transformNodeParametersKey"/>
			<xsl:with-param name="transformNodeMiddlewareKey" select="$transformNodeMiddlewareKey"/>
			<xsl:with-param name="transformNodeAsyncKey" select="$transformNodeAsyncKey"/>
			<xsl:with-param name="transformNodeValueKey" select="$transformNodeValueKey"/>
			<xsl:with-param name="transformNodeCodeKey" select="$transformNodeCodeKey"/>
			<xsl:with-param name="transformNodeTypeKey" select="$transformNodeTypeKey"/>
			<xsl:with-param name="transformNodeActionOnKey" select="$transformNodeActionOnKey"/>
			<xsl:with-param name="transformNodeSortOrderKey" select="$transformNodeSortOrderKey"/>
		</xsl:call-template>	
		
		<!-- Write the end of the component's implementation. -->
		<xsl:value-of select="'}&#xA;'" />
	
		<!-- generate the implementation entrypoint for this component's implementation. -->
		<xsl:variable name="nodeName" select="$node/gml:data[@key=$transformNodeLabelKey]/text()" />
		<xsl:variable name="entrypoint" select="concat('create_', $nodeName, '_', $ImplSuffix)" />
		<xsl:variable name="export_basename" select="concat($nodeName, $ImplSuffix)" />
		<xsl:variable name="export_macro" select="translate($export_basename,
								'abcdefghijklmnopqrstuvwxyz',
								'ABCDEFGHIJKLMNOPQRSTUVWXYZ')" />
	
		<xsl:value-of select="concat('&#xA;//&#xA;// ', $entrypoint, '&#xA;//&#xA;')" />
		<xsl:value-of select="'::Components::EnterpriseComponent_ptr&#xA;'" />

		<xsl:value-of select="concat($entrypoint, ' (void)', '&#xA;{&#xA;')" />
		<xsl:value-of select="'::Components::EnterpriseComponent_ptr retval =&#xA;'" />
		<xsl:value-of select="'  ::Components::EnterpriseComponent::_nil ();&#xA;&#xA;'" />
		<xsl:value-of select="'ACE_NEW_RETURN (retval,&#xA;'" />
		<xsl:value-of select="concat('::', $export_basename, '::', $nodeName)" />
		<xsl:value-of select="' (),&#xA;'" />
		<xsl:value-of select="'::Components::EnterpriseComponent::_nil ());&#xA;'" />
		<xsl:value-of select="'return retval;&#xA;}&#xA;&#xA;'" />
	</xsl:template>	
	
	<!-- Component_Impl_Begin -->
	<xsl:template name="Component_Impl_Begin">
		<xsl:param name="node" />
		<xsl:param name="implNode" />
		<xsl:param name="transformNodeKindKey" />
		<xsl:param name="transformNodeLabelKey" />
		<xsl:param name="transformNodeMiddlewareKey" />
		<xsl:param name="transformNodeAsyncKey" />
		<xsl:param name="transformNodeValueKey" />
		<xsl:param name="transformNodeFrequencyKey" />
		<xsl:param name="transformNodeTypeKey" />
		
		<xsl:variable name="nodeName" select="$node/gml:data[@key=$transformNodeLabelKey]/text()" />
		<xsl:variable name="implName" select="concat($nodeName, $ImplSuffix)" />
		<!-- This part of the code generates the header file. -->
		<xsl:variable name="namespace_name" select="$implName" />
		<xsl:variable name="destructor" select="concat('~', $nodeName)" />
		<!-- Construct the name of the executor. fq_type first param, includes package/module/namespace of component ??? -->
		<xsl:variable name="exec" select="concat('CIAO_', $nodeName, '_Impl::', $nodeName, '_Exec')" />
		<!-- Construct the name of the context. scope includes namespace of component ??? assume top level :: -->
		<xsl:variable name="context" select="concat('::iCCM_', $nodeName, '_Context')" />
		<xsl:variable name="basetype" select="concat('CUTS_CCM_Component_T &lt; ', $exec, ', ', $context, ' &gt;')" />

		<xsl:value-of select="concat('&#xA;namespace ', $namespace_name, '&#xA;{')" />
		<xsl:value-of select="concat('&#xA;//&#xA;// ', $nodeName, '&#xA;//&#xA;')" />
		<xsl:value-of select="concat($nodeName, '::', $nodeName, ' (void)')" />

		<!-- locate variables with initial values -->
		<xsl:variable name="variables" select="$implNode/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'Variable']/.." />
		<!-- Write the initial lead in for the first variable initial value. -->
		<xsl:variable name="basicVars" select="$variables[not(./gml:graph)]" />
		<xsl:if test="count($basicVars) &gt; 0">
			<xsl:value-of select="'&#xA;:'" />
		</xsl:if>
		<xsl:for-each select="$basicVars"> 
<!--			<xsl:sort select="./gml:data[@key=$transformNodeLabelKey]/text()" data-type="text" /> not required and stuffs test for last() -->
			<xsl:variable name="basicVar" select="." />
			<xsl:variable name="basicVarName" select="$basicVar/gml:data[@key=$transformNodeLabelKey]/text()" />
			<xsl:variable name="initValue" select="$basicVar/gml:data[@key=$transformNodeValueKey]/text()" /> 
			<!-- Write the initial values of the basic variables. -->
			<xsl:if test="not($initValue = '')" >
				<xsl:value-of select="concat('  ', $basicVarName, '_ (', $initValue, ')')" />
			</xsl:if>
			<xsl:if test="position() != last()">
				<xsl:value-of select="',&#xA;'" />
			</xsl:if> 
		</xsl:for-each> 
		<xsl:value-of select="'&#xA;{&#xA;'" />
 
		<!-- Write all the complex variable new object -->
		<xsl:variable name="complexVars" select="$variables/gml:graph/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'AggregateInstance' or text() = 'VectorInstance']/../../.." />
		<xsl:for-each select="$complexVars">
			<xsl:variable name="complexVar" select="." />
			<xsl:variable name="varName" select="$complexVar/gml:data[@key=$transformNodeLabelKey]/text()" />
			<xsl:variable name="varType" select="$complexVar/gml:graph/gml:node/gml:data[@key=$transformNodeTypeKey]/text()" />
			<xsl:if test="not($varType = '')">
				<xsl:value-of select="concat('this-&gt;', $varName, '_ = new ', $varType, ' ();&#xA;')" />
			</xsl:if>
			<!-- need to give initial values to aggregate variables with a recursive template to get the full variable name -->
			<xsl:variable name="complexInitValues" select="$complexVar/descendant::*/gml:node/gml:data[@key=$transformNodeValueKey][text() != '']/.." />
			<xsl:for-each select="$complexInitValues">
				<xsl:variable name="sourceDataNode" select="." />
				<xsl:variable name="firstLevelAggregate" select="$complexVar/gml:graph/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'AggregateInstance' or text() = 'VectorInstance']/.." />
				<xsl:call-template name="recurseParameter">
					<xsl:with-param name="variableName" select="concat('this-&gt;', $varName, '_')"/>
					<xsl:with-param name="firstLevelAggregate" select="$firstLevelAggregate"/>
					<xsl:with-param name="firstLevelFunction" select="'false'"/>
					<xsl:with-param name="sourceDataNode" select="$sourceDataNode" />
					<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey" />
				</xsl:call-template> 

				<xsl:variable name="complexInitVal" select="./gml:data[@key=$transformNodeValueKey]/text()" />
				<xsl:variable name="complexInitType" select="./gml:data[@key=$transformNodeTypeKey]/text()" />
				<xsl:value-of select="' = '" />
				<xsl:if test="$complexInitType = 'String' or $complexInitType = 'WideString'">
					<xsl:value-of select="'CORBA::string_dup'" />
				</xsl:if>
				<xsl:value-of select="concat('( ', $complexInitVal, ' );&#xA;')" />
			</xsl:for-each>
		</xsl:for-each>
		
		<xsl:variable name="periodics" select="$implNode/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'PeriodicEvent']/.." />
		<xsl:for-each select="$periodics">
			<xsl:variable name="periodic" select="." />
			<xsl:variable name="periodicName" select="$periodic/gml:data[@key=$transformNodeLabelKey]/text()" />
			<xsl:variable name="distribution" select="$periodic/gml:data[@key=$transformNodeTypeKey]/text()" />
			<xsl:variable name="Hertz" select="$periodic/gml:data[@key=$transformNodeFrequencyKey]/text()" />

			<xsl:value-of select="concat('this-&gt;periodic_', $periodicName, '_.init (this, &amp;')" />
			<xsl:value-of select="concat($nodeName, '::periodic_', $periodicName, ');&#xA;')" />
			<xsl:value-of select="concat('this-&gt;periodic_', $periodicName, '_.configure (CUTS_Periodic_Event::')" />
			<xsl:choose>
			<xsl:when test="$distribution = 'Constant'">
				<xsl:value-of select="'PE_CONSTANT, '" />
			</xsl:when>
			<xsl:when test="$distribution = 'Exponential'">
				<xsl:value-of select="'PE_EXPONENTIAL, '" />
			</xsl:when>
			<xsl:otherwise> 
				<xsl:value-of select="'PE_UNDEFINED, '" />
			</xsl:otherwise>
			</xsl:choose>
			<xsl:value-of select="concat($Hertz, ');&#xA;')" />
			<xsl:value-of select="concat('this-&gt;register_object (&amp;this-&gt;periodic_', $periodicName, '_);&#xA;')" />
		</xsl:for-each>
		
		<!-- if CIAO we may need to emulate asynchronous function. -->
		<xsl:variable name="middleware" select="/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'Model']/../gml:data[@key=$transformNodeMiddlewareKey]/text()" />
		<xsl:if test="$middleware = 'tao'" >
			<xsl:variable name="sinks" select="$node/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'InEventPort']/.." />
			<xsl:for-each select="$sinks" >
				<xsl:variable name="sink" select="." />
				<xsl:variable name="sinkName" select="$sink/gml:data[@key=$transformNodeLabelKey]/text()" />
				<!-- Determine if this input event is asynchronous so need to emulate asynchronous function. -->
				<xsl:variable name="is_async" select="$sink/gml:data[@key=$transformNodeAsyncKey]/text()" />
				<xsl:if test="$is_async = 'true'" >
					<!-- Register the event handler object. -->
					<xsl:variable name="varname" select="concat($sinkName, '_event_handler_')" />
					<xsl:value-of select="concat('this-&gt;', $varname, '.init (this, &amp;', $nodeName, '::push_', $sinkName, '_i);&#xA;')" />
					<xsl:value-of select="concat('this-&gt;register_object (&amp;this-&gt;', $varname, ');&#xA;')" />
				</xsl:if>
			</xsl:for-each>
		</xsl:if>
		<!-- Initialize_Entity::Visit_WorkerType special code for Sensor_Worker ??? -->
				  
		<!-- Finish the constructor. -->
		<xsl:value-of select="'&#xA;}&#xA;'" />
		<!-- Write the destructor's implementation. -->
		<xsl:value-of select="concat('&#xA;//&#xA;// ', $destructor, '&#xA;//&#xA;')" />
		<xsl:value-of select="concat($nodeName, '::', $destructor, ' (void)&#xA;{&#xA;}&#xA;')" />
	</xsl:template>	

	<!-- Visit_Component -->
	<xsl:template name="Visit_Component">
		<xsl:param name="node" />
		<xsl:param name="implNode" />
		<xsl:param name="transformNodeKindKey" />
		<xsl:param name="transformNodeLabelKey" />
		<xsl:param name="transformNodeFolderKey" />
		<xsl:param name="transformNodeWorkerKey" />
		<xsl:param name="transformNodeWorkerIDKey" />
		<xsl:param name="transformNodeOperationKey" />
		<xsl:param name="transformNodeComplexityKey" />
		<xsl:param name="transformNodeComplexityParametersKey" />
		<xsl:param name="transformNodeParametersKey" />
		<xsl:param name="transformNodeMiddlewareKey" />
		<xsl:param name="transformNodeAsyncKey" />
		<xsl:param name="transformNodeValueKey" />
		<xsl:param name="transformNodeCodeKey" />
		<xsl:param name="transformNodeTypeKey" />
		<xsl:param name="transformNodeActionOnKey" />
		<xsl:param name="transformNodeSortOrderKey" />
		
		<xsl:variable name="nodeName" select="$node/gml:data[@key=$transformNodeLabelKey]/text()" />
		<xsl:variable name="middleware" select="/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'Model']/../gml:data[@key=$transformNodeMiddlewareKey]/text()" />
		<xsl:variable name="interfaceDefs" select="/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'InterfaceDefinitions']/.." />
		<xsl:variable name="edges" select="/descendant::*/gml:edge"/>
		
		<!-- Visit all the InEventPort elements of the <component>. -->
		<xsl:variable name="sinks" select="$node/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'InEventPort']/.." />
		<!-- Get the connection properties from the port. "asynchronous" -->
		<!-- InEventPort_Begin We are generating a regular event port. -->
		<xsl:for-each select="$sinks" >
			<xsl:variable name="sink" select="." />
			<xsl:variable name="sinkName" select="$sink/gml:data[@key=$transformNodeLabelKey]/text()" />
			<xsl:variable name="typeNodeId" select="/descendant::*/gml:edge[@source=$sink/@id]/@target" />
			<xsl:variable name="typeNode" select="$interfaceDefs/descendant::*/gml:node[@id=$typeNodeId]/gml:data[@key=$transformNodeKindKey][text() = 'Aggregate']/.." />
			<xsl:variable name="baseName" select="$typeNode/gml:data[@key=$transformNodeLabelKey]/text()" />
			<xsl:variable name="typeName" select="concat($baseName, $EventSuffix)" />
			<!-- Construct the name with scoped namespace of component ??? assume top level :: -->
			<xsl:variable name="fq_name" select="concat('::',$typeName)" />
			<!-- Make sure this is not a template event port. ie has event type aggregate -->
			<xsl:if test="count($typeNode) &gt; 0" >
			
				<xsl:value-of select="concat('&#xA;//&#xA;// sink: ', $sinkName, '&#xA;//&#xA;')" />
				<xsl:value-of select="concat('void ', $nodeName, '::push_', $sinkName, ' (', $fq_name, ' * ev)&#xA;{&#xA;')" />

				<!-- Generate the appropriate methods. -->
				<!-- Determine if this input event is asynchronous and need to emulate asynchronous function. -->
				<xsl:variable name="is_async" select="$sink/gml:data[@key=$transformNodeAsyncKey]/text()" />
				<xsl:if test="$is_async = 'true' and $middleware = 'tao'" >
					<xsl:value-of select="concat('this-&gt;', $sinkName, '_event_handler_.handle_event (ev);&#xA;}&#xA;')" />
					<xsl:value-of select="concat('&#xA;//&#xA;// sink: ', $sinkName, '&#xA;//&#xA;')" />
					<xsl:value-of select="concat('void ', $nodeName, '::push_', $sinkName, '_i (', $fq_name, ' * ev)&#xA;{&#xA;')" />
				</xsl:if>
				
				<!-- Copy event to base structure for use in data connections -->
				<xsl:value-of select="concat('::', $baseName, ' __ev_base__;&#xA;')" />
				<xsl:for-each select="$sink/gml:graph/gml:node/gml:graph/gml:node" >
					<xsl:variable name="aggregateChild" select="." />
					<xsl:variable name="memberLabel" select="$aggregateChild/gml:data[@key=$transformNodeLabelKey]/text()" />
					<xsl:variable name="memberType" select="$aggregateChild/gml:data[@key=$transformNodeTypeKey]/text()" />
					<xsl:value-of select="concat('__ev_base__.', $memberLabel, ' = ' )" />
					<xsl:if test="$memberType = 'String' or $memberType = 'WideString'">
						<xsl:value-of select="'CORBA::string_dup'" />
					</xsl:if>
					<xsl:value-of select="concat('(ev-&gt;', $memberLabel, '());&#xA;')" />
				</xsl:for-each>
  			</xsl:if>
			<xsl:variable name="implSink" select="$implNode/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'InEventPortImpl']/../gml:data[@key=$transformNodeLabelKey][text() = $sinkName]/.." />
			<xsl:call-template name="Execution_Visitor">
				<xsl:with-param name="S" select="$implSink"/>
				<xsl:with-param name="T" select="''"/>
				<xsl:with-param name="E" select="$edges"/>
				<xsl:with-param name="N" select="$implNode"/>
				<xsl:with-param name="W" select="$implSink"/>
				<xsl:with-param name="O" select="0"/>
				<xsl:with-param name="B" select="0"/>
				<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey" />
				<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey" />
				<xsl:with-param name="transformNodeWorkerKey"  select="$transformNodeWorkerKey" />
				<xsl:with-param name="transformNodeWorkerIDKey"  select="$transformNodeWorkerIDKey" />
				<xsl:with-param name="transformNodeOperationKey"  select="$transformNodeOperationKey" />
				<xsl:with-param name="transformNodeComplexityKey" select="$transformNodeComplexityKey"/>
				<xsl:with-param name="transformNodeComplexityParametersKey" select="$transformNodeComplexityParametersKey"/>
				<xsl:with-param name="transformNodeParametersKey" select="$transformNodeParametersKey"/>
				<xsl:with-param name="transformNodeActionOnKey"  select="$transformNodeActionOnKey" />
				<xsl:with-param name="transformNodeSortOrderKey"  select="$transformNodeSortOrderKey" />
				<xsl:with-param name="transformNodeValueKey"  select="$transformNodeValueKey" />
				<xsl:with-param name="transformNodeCodeKey"  select="$transformNodeCodeKey" />
				<xsl:with-param name="transformNodeTypeKey"  select="$transformNodeTypeKey" />
			</xsl:call-template>
			<xsl:value-of select="'ACE_UNUSED_ARG (ev);&#xA;}&#xA;'" />
		</xsl:for-each>

		<!-- Visit all the PeriodicEvent elements of the <component>. -->
		<xsl:variable name="periodics" select="$implNode/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'PeriodicEvent']/.." />
		<xsl:for-each select="$periodics">
			<xsl:variable name="periodic" select="." />
			<!-- PeriodicEvent_Begin -->
			<xsl:variable name="periodicName" select="$periodic/gml:data[@key=$transformNodeLabelKey]/text()" />
			<xsl:variable name="func_name" select="concat('periodic_', $periodicName)" />
			<xsl:value-of select="concat('&#xA;//&#xA;// PeriodicEvent: ', $periodicName, '&#xA;//&#xA;')" />
			<xsl:value-of select="concat('void ', $nodeName, '::', $func_name, ' (void)&#xA;{&#xA;')" />
			<xsl:call-template name="Execution_Visitor">
				<xsl:with-param name="S" select="$periodic"/>
				<xsl:with-param name="T" select="''"/>
				<xsl:with-param name="E" select="$edges"/>
				<xsl:with-param name="N" select="$implNode"/>
				<xsl:with-param name="W" select="$periodic"/>
				<xsl:with-param name="O" select="0"/>
				<xsl:with-param name="B" select="0"/>
				<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey" />
				<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey" />
				<xsl:with-param name="transformNodeWorkerKey"  select="$transformNodeWorkerKey" />
				<xsl:with-param name="transformNodeWorkerIDKey"  select="$transformNodeWorkerIDKey" />
				<xsl:with-param name="transformNodeOperationKey"  select="$transformNodeOperationKey" />
				<xsl:with-param name="transformNodeComplexityKey" select="$transformNodeComplexityKey"/>
				<xsl:with-param name="transformNodeComplexityParametersKey" select="$transformNodeComplexityParametersKey"/>
				<xsl:with-param name="transformNodeParametersKey" select="$transformNodeParametersKey"/>
				<xsl:with-param name="transformNodeActionOnKey"  select="$transformNodeActionOnKey" />
				<xsl:with-param name="transformNodeSortOrderKey"  select="$transformNodeSortOrderKey" />
				<xsl:with-param name="transformNodeValueKey"  select="$transformNodeValueKey" />
				<xsl:with-param name="transformNodeCodeKey"  select="$transformNodeCodeKey" />
				<xsl:with-param name="transformNodeTypeKey"  select="$transformNodeTypeKey" />
			</xsl:call-template>
			<xsl:value-of select="'&#xA;}&#xA;'" />
		</xsl:for-each>
		
		<!-- Visit all the Attribute elements of the <component>. -->
		<xsl:variable name="attrs" select="$node/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'Attribute']/.." />
		<xsl:for-each select="$attrs">
			<xsl:variable name="attr" select="." />
			<xsl:variable name="attrName" select="$attr/gml:data[@key=$transformNodeLabelKey]/text()" />
			<!-- Assume that attributes are predefined types, may extend to aggregate types in future ???  -->
			<xsl:variable name="member_type" select="$attr/gml:data[@key=$transformNodeTypeKey]/text()" />
			<!-- Attribute_Begin -->
			<xsl:if test="not($member_type = '')">		
				<xsl:value-of select="concat('&#xA;//&#xA;// attribute setter: ', $attrName, '&#xA;//&#xA;')" />
				<xsl:value-of select="concat('void ', $nodeName, '::', $attrName, ' (')" />
				<xsl:call-template name="MemberType">
					<xsl:with-param name="type" select="$member_type"/>
				</xsl:call-template>
				<xsl:value-of select="concat(' ', $attrName, ')')" />
				<xsl:value-of select="'&#xA;{&#xA;'" />
				<!-- Attribute_End -->
				<!-- Assume not using GenericObject, GenericValue or TypeEncoding ??? see Component_Implementation.cpp -->
				<xsl:value-of select="concat('this-&gt;', $attrName, '_ = ')" />
				<xsl:value-of select="concat($attrName, ';&#xA;}&#xA;')" />
				<!-- Make sure we generate the <readonly> variant. ie getter functions -->
				<!-- ReadonlyAttribute_Begin -->
				<xsl:value-of select="concat('&#xA;//&#xA;// attribute getter: ', $attrName, '&#xA;//&#xA;')" />
				<xsl:call-template name="MemberType">
					<xsl:with-param name="type" select="$member_type"/>
					<xsl:with-param name="retn_type" select="'true'"/>
				</xsl:call-template>
				<xsl:value-of select="concat(' ', $nodeName, '::', $attrName, ' (void)&#xA;{&#xA;')" />
				<!-- ReadonlyAttribute_End -->
				<!-- Assume not using GenericObject, GenericValue or TypeEncoding ??? see Component_Implementation.cpp -->
				<xsl:choose>
				<xsl:when test="$member_type = 'String'">
					<!-- Strings are special case. We need to return a duplicate copy of the string, or we will have major problems. -->
					<xsl:value-of select="concat('::CORBA::String_var s =&#xA;  ::CORBA::string_dup (this-&gt;', $attrName, '_.c_str ());&#xA;')" />
					<xsl:value-of select="'return s._retn ();'" />
				</xsl:when>
				<xsl:otherwise>
					<xsl:value-of select="concat('return this-&gt;', $attrName, '_;')" />
				</xsl:otherwise>
				</xsl:choose>
				<xsl:value-of select="'&#xA;}&#xA;'" />
			</xsl:if>
		</xsl:for-each>

		<!-- Get the environment for the component. -->
		<xsl:variable name="workloadNodes" select="$implNode/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'Workload']/.." />
		<!-- in the new structure the environment is a workload that has no connections, ie is not part of a behaviour path -->
		<xsl:variable name="envNodes" select="$workloadNodes[not(@id = /descendant::*/gml:edge/@source)]/." />

		<xsl:if test="count($envNodes) &gt; 0">
			<!-- Environment actions are now process nodes -->
			<xsl:variable name="processNodes" select="$envNodes/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'Process']/.." />

				<!-- Environment_Method_Begin -->
				<!-- "worker_action" worker callback to be implemented later ??? -->
				<xsl:if test="$processNodes/gml:data[@key=$transformNodeActionOnKey]/text() = 'Activate'">
					<xsl:value-of select="'&#xA;//&#xA;// ccm_activate&#xA;//&#xA;'" />
					<xsl:value-of select="concat('void ', $nodeName, '::ccm_activate (void)&#xA;{&#xA;')" />
					<xsl:variable name="activateNodes" select="$processNodes/gml:data[@key=$transformNodeActionOnKey][text() = 'Activate']/.." />
					<!-- Visit_MultiInputAction, now set of Activate Processes -->
					<xsl:call-template name="Execution_Process">
						<xsl:with-param name="processNodes" select="$activateNodes"/>
						<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey" />
						<xsl:with-param name="transformNodeSortOrderKey" select="$transformNodeSortOrderKey"/>
						<xsl:with-param name="transformNodeWorkerKey" select="$transformNodeWorkerKey"/>
						<xsl:with-param name="transformNodeWorkerIDKey"  select="$transformNodeWorkerIDKey" />
						<xsl:with-param name="transformNodeOperationKey" select="$transformNodeOperationKey"/>
						<xsl:with-param name="transformNodeComplexityKey" select="$transformNodeComplexityKey"/>
						<xsl:with-param name="transformNodeComplexityParametersKey" select="$transformNodeComplexityParametersKey"/>
						<xsl:with-param name="transformNodeParametersKey" select="$transformNodeParametersKey"/>
						<xsl:with-param name="transformNodeCodeKey" select="$transformNodeCodeKey"/>
						<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey"/>
						<xsl:with-param name="transformNodeValueKey"  select="$transformNodeValueKey" />
						<xsl:with-param name="transformNodeTypeKey"  select="$transformNodeTypeKey" />
					</xsl:call-template> 
				    <xsl:value-of select="'this->base_type::ccm_activate ();&#xA;}&#xA;'" />
				</xsl:if>
				<xsl:if test="$processNodes/gml:data[@key=$transformNodeActionOnKey]/text() = 'Configuration_complete'">
					<xsl:value-of select="'&#xA;//&#xA;// configuration_complete&#xA;//&#xA;'" />
					<xsl:value-of select="concat('void ', $nodeName, '::configuration_complete (void)&#xA;{&#xA;')" />
					<xsl:variable name="completeNodes" select="$processNodes/gml:data[@key=$transformNodeActionOnKey][text() = 'Configuration_complete']/.." />
					<!-- Visit_MultiInputAction, now set of Activate Processes -->
					<xsl:call-template name="Execution_Process">
						<xsl:with-param name="processNodes" select="$completeNodes"/>
						<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey" />
						<xsl:with-param name="transformNodeSortOrderKey" select="$transformNodeSortOrderKey"/>
						<xsl:with-param name="transformNodeWorkerKey" select="$transformNodeWorkerKey"/>
						<xsl:with-param name="transformNodeWorkerIDKey"  select="$transformNodeWorkerIDKey" />
						<xsl:with-param name="transformNodeOperationKey" select="$transformNodeOperationKey"/>
						<xsl:with-param name="transformNodeComplexityKey" select="$transformNodeComplexityKey"/>
						<xsl:with-param name="transformNodeComplexityParametersKey" select="$transformNodeComplexityParametersKey"/>
						<xsl:with-param name="transformNodeParametersKey" select="$transformNodeParametersKey"/>
						<xsl:with-param name="transformNodeCodeKey" select="$transformNodeCodeKey"/>
						<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey"/>
						<xsl:with-param name="transformNodeValueKey"  select="$transformNodeValueKey" />
						<xsl:with-param name="transformNodeTypeKey"  select="$transformNodeTypeKey" />
					</xsl:call-template> 
				    <xsl:value-of select="'this->base_type::configuration_complete ();&#xA;}&#xA;'" />
				</xsl:if>
				<xsl:if test="$processNodes/gml:data[@key=$transformNodeActionOnKey]/text() = 'Passivate'">
					<xsl:value-of select="'&#xA;//&#xA;// ccm_passivate&#xA;//&#xA;'" />
					<xsl:value-of select="concat('void ', $nodeName, '::ccm_passivate (void)&#xA;{&#xA;')" />
					<xsl:value-of select="'this->base_type::ccm_passivate ();&#xA;'" />
					<xsl:variable name="pasivateNodes" select="$processNodes/gml:data[@key=$transformNodeActionOnKey][text() = 'Passivate']/.." />
					<!-- Visit_MultiInputAction, now set of Activate Processes -->
					<xsl:call-template name="Execution_Process">
						<xsl:with-param name="processNodes" select="$pasivateNodes"/>
						<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey" />
						<xsl:with-param name="transformNodeSortOrderKey" select="$transformNodeSortOrderKey"/>
						<xsl:with-param name="transformNodeWorkerKey" select="$transformNodeWorkerKey"/>
						<xsl:with-param name="transformNodeWorkerIDKey"  select="$transformNodeWorkerIDKey" />
						<xsl:with-param name="transformNodeOperationKey" select="$transformNodeOperationKey"/>
						<xsl:with-param name="transformNodeComplexityKey" select="$transformNodeComplexityKey"/>
						<xsl:with-param name="transformNodeComplexityParametersKey" select="$transformNodeComplexityParametersKey"/>
						<xsl:with-param name="transformNodeParametersKey" select="$transformNodeParametersKey"/>
						<xsl:with-param name="transformNodeCodeKey" select="$transformNodeCodeKey"/>
						<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey"/>
						<xsl:with-param name="transformNodeValueKey"  select="$transformNodeValueKey" />
						<xsl:with-param name="transformNodeTypeKey"  select="$transformNodeTypeKey" />
					</xsl:call-template> 
					<xsl:value-of select="'}&#xA;'" />
				</xsl:if>
				<xsl:if test="$processNodes/gml:data[@key=$transformNodeActionOnKey]/text() = 'Remove'">
					<xsl:value-of select="'&#xA;//&#xA;// ccm_remove&#xA;//&#xA;'" />
					<xsl:value-of select="concat('void ', $nodeName, '::ccm_remove (void)&#xA;{&#xA;')" />
					<xsl:value-of select="'this->base_type::ccm_remove ();&#xA;'" />
					<xsl:variable name="removeNodes" select="$processNodes/gml:data[@key=$transformNodeActionOnKey][text() = 'Remove']/.." />
					<!-- Visit_MultiInputAction, now set of Activate Processes -->
					<xsl:call-template name="Execution_Process">
						<xsl:with-param name="processNodes" select="$removeNodes"/>
						<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey" />
						<xsl:with-param name="transformNodeSortOrderKey" select="$transformNodeSortOrderKey"/>
						<xsl:with-param name="transformNodeWorkerKey" select="$transformNodeWorkerKey"/>
						<xsl:with-param name="transformNodeWorkerIDKey"  select="$transformNodeWorkerIDKey" />
						<xsl:with-param name="transformNodeOperationKey" select="$transformNodeOperationKey"/>
						<xsl:with-param name="transformNodeComplexityKey" select="$transformNodeComplexityKey"/>
						<xsl:with-param name="transformNodeComplexityParametersKey" select="$transformNodeComplexityParametersKey"/>
						<xsl:with-param name="transformNodeParametersKey" select="$transformNodeParametersKey"/>
						<xsl:with-param name="transformNodeCodeKey" select="$transformNodeCodeKey"/>
						<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey"/>
						<xsl:with-param name="transformNodeValueKey"  select="$transformNodeValueKey" />
						<xsl:with-param name="transformNodeTypeKey"  select="$transformNodeTypeKey" />
					</xsl:call-template> 
					<xsl:value-of select="'}&#xA;'" />
				</xsl:if>
		</xsl:if>
	</xsl:template>	

	<!-- Member or Event Type -->	
	<xsl:template name="MemberType">	
		<xsl:param name="type" />
		<xsl:param name="retn_type" select="''" />
		
		<xsl:choose>
			<xsl:when test="$type = 'Byte'" >
				<xsl:value-of select="'::CORBA::Octet'" />
			</xsl:when>
			<xsl:when test="$type = 'Char'" >
				<xsl:value-of select="'char'" />
			</xsl:when>
			<xsl:when test="$type = 'WideChar'" >
				<xsl:value-of select="'wchar_t'" />
			</xsl:when>	
			<xsl:when test="$type = 'Boolean'" >
				<xsl:value-of select="'::CORBA::Boolean'" />
			</xsl:when>
			<xsl:when test="$type = 'String'" >
				<xsl:if test="$retn_type = ''">
					<xsl:value-of select="'const '" />
				</xsl:if>
				<xsl:value-of select="'char *'" />
			</xsl:when>
			<xsl:when test="$type = 'WideString'" >
				<xsl:if test="$retn_type = ''">
					<xsl:value-of select="'const '" />
				</xsl:if>
				<xsl:value-of select="'wchar_t *'" />
			</xsl:when>
			<xsl:when test="$type = 'UnsignedShortInteger'" >
				<xsl:value-of select="'::CORBA::UShort'" />
			</xsl:when>
			<xsl:when test="$type = 'UnsignedLongInteger'" >
				<xsl:value-of select="'::CORBA::ULong'" />
			</xsl:when>
			<xsl:when test="$type = 'UnsignedLongLongInteger'" >
				<xsl:value-of select="'::CORBA::ULongLong'" />
			</xsl:when>
			<xsl:when test="$type = 'ShortInteger'" >
				<xsl:value-of select="'::CORBA::Short'" />
			</xsl:when>
			<xsl:when test="$type = 'LongInteger'" >
				<xsl:value-of select="'::CORBA::Long'" />
			</xsl:when>
			<xsl:when test="$type = 'LongLongInteger'" >
				<xsl:value-of select="'::CORBA::LongLong'" />
			</xsl:when>
			<xsl:when test="$type = 'FloatNumber'" >
				<xsl:value-of select="'::CORBA::Float'" />
			</xsl:when>
			<xsl:when test="$type = 'DoubleNumber'" >
				<xsl:value-of select="'::CORBA::Double'" />
			</xsl:when>
			<xsl:when test="$type = 'LongDoubleNumber'" >
				<xsl:value-of select="'::CORBA::LongDouble'" />
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="$type" />
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>

	<!-- Member or Event Type -->	
	<xsl:template name="MemberInitType">	
		<xsl:param name="type" />
		
		<xsl:choose>
			<xsl:when test="$type = 'String' or $type = 'WideString'" >
				<xsl:value-of select="' = &quot;&quot;'" />
			</xsl:when>
			<xsl:when test="$type = 'Byte' or $type = 'Char' or $type = 'WideChar' or $type = 'Boolean' or $type = 'UnsignedShortInteger'
				or $type = 'UnsignedLongInteger' or $type = 'UnsignedLongLongInteger' or $type = 'ShortInteger' or $type = 'LongInteger'
				or $type = 'LongLongInteger' or $type = 'FloatNumber' or $type = 'DoubleNumber' or $type = 'LongDoubleNumber'" >
				<xsl:value-of select="' = 0'" />
			</xsl:when>
			<xsl:otherwise> <!-- no value assigned when aggregate type -->
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>

	<!-- Execution_Visitor  -->
	<!-- Recursive Search of behaviour state machine starting at InEventPort -->
	<xsl:template name="Execution_Visitor">
		<xsl:param name="S"/> <!-- start node = end node -->
		<xsl:param name="T"/> <!-- terminal node -->
		<xsl:param name="E"/> <!-- edges -->
		<xsl:param name="N"/> <!-- candidate nodes -->
		<xsl:param name="W"/> <!-- node[s] in progress -->
		<xsl:param name="O"/> <!-- order of next node to process -->
		<xsl:param name="B"/> <!-- order of previous branch node -->
		<xsl:param name="transformNodeKindKey" />
		<xsl:param name="transformNodeLabelKey" />
		<xsl:param name="transformNodeWorkerKey" />
		<xsl:param name="transformNodeWorkerIDKey" />
		<xsl:param name="transformNodeOperationKey" />
		<xsl:param name="transformNodeComplexityKey" />
		<xsl:param name="transformNodeComplexityParametersKey" />
		<xsl:param name="transformNodeParametersKey" />
		<xsl:param name="transformNodeActionOnKey" />
		<xsl:param name="transformNodeSortOrderKey" />
		<xsl:param name="transformNodeValueKey" />
		<xsl:param name="transformNodeCodeKey" />
		<xsl:param name="transformNodeTypeKey" />

		<!-- Process node W -->
		<!-- output appropriate source code depending on the type of node -->
		<!--	<xsl:value-of select="concat($W/gml:data[@key=$transformNodeKindKey]/text(), ' -Kind- ')" /> -->
		<xsl:variable name="toOrder" select="count($E[@target=$W/@id])" />
		<xsl:variable name="notOrLastTerminal">
			<xsl:choose>
			<xsl:when test="not($W/gml:data[@key=$transformNodeKindKey]/text() = 'Termination')">
				<xsl:value-of select="'true'" />
			</xsl:when>
			<xsl:when test="($W/gml:data[@key=$transformNodeKindKey]/text() = 'Termination') and ($toOrder = $O)">
				<xsl:value-of select="'true'" />
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="'false'" />
			</xsl:otherwise>
			</xsl:choose>
		</xsl:variable>
				
		<xsl:choose>
		<xsl:when test="$W/gml:data[@key=$transformNodeKindKey]/text() = 'Workload'">
			<xsl:variable name="preProcessNodes" select="$W/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'Process']/../gml:data[@key=$transformNodeActionOnKey][text() = 'Preprocess']/.." />
			<xsl:call-template name="Execution_Process">
				<xsl:with-param name="processNodes" select="$preProcessNodes"/>
				<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey" />
				<xsl:with-param name="transformNodeSortOrderKey" select="$transformNodeSortOrderKey"/>
				<xsl:with-param name="transformNodeWorkerKey" select="$transformNodeWorkerKey"/>
				<xsl:with-param name="transformNodeWorkerIDKey" select="$transformNodeWorkerIDKey"/>
				<xsl:with-param name="transformNodeOperationKey" select="$transformNodeOperationKey"/>
				<xsl:with-param name="transformNodeComplexityKey" select="$transformNodeComplexityKey"/>
				<xsl:with-param name="transformNodeComplexityParametersKey" select="$transformNodeComplexityParametersKey"/>
				<xsl:with-param name="transformNodeParametersKey" select="$transformNodeParametersKey"/>
				<xsl:with-param name="transformNodeCodeKey" select="$transformNodeCodeKey"/>
				<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey"/>
				<xsl:with-param name="transformNodeValueKey"  select="$transformNodeValueKey" />
				<xsl:with-param name="transformNodeTypeKey"  select="$transformNodeTypeKey" />
			</xsl:call-template> 
			<xsl:variable name="mainProcessNodes" select="$W/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'Process']/../gml:data[@key=$transformNodeActionOnKey][text() = 'Mainprocess']/.." />
			<xsl:call-template name="Execution_Process">
				<xsl:with-param name="processNodes" select="$mainProcessNodes"/>
				<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey" />
				<xsl:with-param name="transformNodeSortOrderKey" select="$transformNodeSortOrderKey"/>
				<xsl:with-param name="transformNodeWorkerKey" select="$transformNodeWorkerKey"/>
				<xsl:with-param name="transformNodeWorkerIDKey" select="$transformNodeWorkerIDKey"/>
				<xsl:with-param name="transformNodeOperationKey" select="$transformNodeOperationKey"/>
				<xsl:with-param name="transformNodeComplexityKey" select="$transformNodeComplexityKey"/>
				<xsl:with-param name="transformNodeComplexityParametersKey" select="$transformNodeComplexityParametersKey"/>
				<xsl:with-param name="transformNodeParametersKey" select="$transformNodeParametersKey"/>
				<xsl:with-param name="transformNodeCodeKey" select="$transformNodeCodeKey"/>
				<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey"/>
				<xsl:with-param name="transformNodeValueKey"  select="$transformNodeValueKey" />
				<xsl:with-param name="transformNodeTypeKey"  select="$transformNodeTypeKey" />
			</xsl:call-template>
			<xsl:variable name="postProcessNodes" select="$W/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'Process']/../gml:data[@key=$transformNodeActionOnKey][text() = 'Postprocess']/.." />
			<xsl:call-template name="Execution_Process">
				<xsl:with-param name="processNodes" select="$postProcessNodes"/>
				<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey" />
				<xsl:with-param name="transformNodeSortOrderKey" select="$transformNodeSortOrderKey"/>
				<xsl:with-param name="transformNodeWorkerKey" select="$transformNodeWorkerKey"/>
				<xsl:with-param name="transformNodeWorkerIDKey" select="$transformNodeWorkerIDKey"/>
				<xsl:with-param name="transformNodeOperationKey" select="$transformNodeOperationKey"/>
				<xsl:with-param name="transformNodeComplexityKey" select="$transformNodeComplexityKey"/>
				<xsl:with-param name="transformNodeComplexityParametersKey" select="$transformNodeComplexityParametersKey"/>
				<xsl:with-param name="transformNodeParametersKey" select="$transformNodeParametersKey"/>
				<xsl:with-param name="transformNodeCodeKey" select="$transformNodeCodeKey"/>
				<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey"/>
				<xsl:with-param name="transformNodeValueKey"  select="$transformNodeValueKey" />
				<xsl:with-param name="transformNodeTypeKey"  select="$transformNodeTypeKey" />
			</xsl:call-template>
		</xsl:when>
		
		<xsl:when test="$W/gml:data[@key=$transformNodeKindKey]/text() = 'WhileLoop'">
			<!-- should only have one condition, only process first found -->
			<xsl:variable name="loopConditionNode" select="$W/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'Condition']/.." />
			<xsl:variable name="loopCondition" select="$loopConditionNode[1]/gml:data[@key=$transformNodeValueKey]/text()" />
			<!-- While_Condition_Begin -->
			<xsl:value-of select="'while ('" />
			<!-- Precondition -->
			<xsl:choose>
			<xsl:when test="$loopCondition and not($loopCondition = '')">
				<xsl:value-of select="$loopCondition" />
			</xsl:when>
			<xsl:otherwise> <!-- assume this defaults to true -->
				<xsl:value-of select="' true '" />
			</xsl:otherwise>
			</xsl:choose>
			<!-- While_Condition_End -->
			<xsl:value-of select="')'" />
			<!-- While_Begin -->
			<xsl:value-of select="'&#xA;{&#xA;'" />
			<!-- follow connection from Condition to next node -->
			<xsl:variable name="nextInBranch" select="$N/descendant::*/gml:node[@id=$E[@source=$loopConditionNode/@id]/@target]" />
			<xsl:if test="$nextInBranch"> 
				<!-- recurse -->
				<xsl:call-template name="Execution_Visitor">
					<xsl:with-param name="S" select="$S"/>
					<xsl:with-param name="T" select="$T"/>
					<xsl:with-param name="E" select="$E"/>
					<xsl:with-param name="N" select="$N"/>
					<xsl:with-param name="W" select="$nextInBranch"/>
					<xsl:with-param name="O" select="1"/>
					<xsl:with-param name="B" select="concat($B,',',$O)"/> <!-- push order of branch on stack -->
					<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey" />
					<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey" />
					<xsl:with-param name="transformNodeWorkerKey"  select="$transformNodeWorkerKey" />
					<xsl:with-param name="transformNodeWorkerIDKey" select="$transformNodeWorkerIDKey"/>
					<xsl:with-param name="transformNodeOperationKey"  select="$transformNodeOperationKey" />
					<xsl:with-param name="transformNodeComplexityKey" select="$transformNodeComplexityKey"/>
					<xsl:with-param name="transformNodeComplexityParametersKey" select="$transformNodeComplexityParametersKey"/>
					<xsl:with-param name="transformNodeParametersKey" select="$transformNodeParametersKey"/>
					<xsl:with-param name="transformNodeActionOnKey"  select="$transformNodeActionOnKey" />
					<xsl:with-param name="transformNodeSortOrderKey"  select="$transformNodeSortOrderKey" />
					<xsl:with-param name="transformNodeValueKey"  select="$transformNodeValueKey" />
					<xsl:with-param name="transformNodeCodeKey"  select="$transformNodeCodeKey" />
					<xsl:with-param name="transformNodeTypeKey"  select="$transformNodeTypeKey" />
				</xsl:call-template>
			</xsl:if>
			<!-- While_End handled in Termination } -->
 		</xsl:when>
		
		<xsl:when test="$W/gml:data[@key=$transformNodeKindKey]/text() = 'BranchState'">
			<xsl:variable name="conditions" select="$W/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'Condition']/.." />
			<!-- Process Condition nodes in sorted order -->
			<xsl:variable name="sortedConditions" >
				<xsl:for-each select="$conditions" >
					<xsl:sort select="./gml:data[@key=$transformNodeSortOrderKey]" data-type="number" order="ascending" /> 
					<xsl:value-of select="concat(',', ./@id)" />
				</xsl:for-each>
			</xsl:variable>
			<xsl:variable name="sortedOriginalPosition" >
				<xsl:for-each select="$conditions" >
					<xsl:sort select="./gml:data[@key=$transformNodeSortOrderKey]" data-type="number" order="ascending" /> 
					<xsl:value-of select="concat(',', position())" />
				</xsl:for-each>
			</xsl:variable>
			<xsl:variable name="countDelim" select="string-length($sortedConditions) - string-length(translate($sortedConditions, ',', ''))" />
			<xsl:for-each select="$conditions" >
				<!-- using for-each and position() iteration info, but retrieving condition Node from sorted list -->
				<xsl:variable name="conditionNodeId">
					<xsl:call-template name="splitList">
						<xsl:with-param name="pText" select="$sortedConditions" />
						<xsl:with-param name="count" select="$countDelim" />
						<xsl:with-param name="idx" select="position()" />
						<xsl:with-param name="delim" select="','" />
					</xsl:call-template>
				</xsl:variable> 
				<xsl:variable name="originalPosition">
					<xsl:call-template name="splitList">
						<xsl:with-param name="pText" select="$sortedOriginalPosition" />
						<xsl:with-param name="count" select="$countDelim" />
						<xsl:with-param name="idx" select="position()" />
						<xsl:with-param name="delim" select="','" />
					</xsl:call-template>
				</xsl:variable> 
				<xsl:variable name="conditionNode" select="$W/descendant::*/gml:node[@id=$conditionNodeId]/." />
				<xsl:variable name="condition" select="$conditionNode/gml:data[@key=$transformNodeValueKey]/text()" />
				<!-- We first need to write the condition for the branch before writing the actual branch statements. -->
				<!-- Branch_Condition_Begin -->
				<xsl:if test="position() &gt; 1"> 
					<xsl:value-of select="'else '" />
				</xsl:if>
				<xsl:choose>
				<xsl:when test="$condition and not($condition = '')">
					<xsl:value-of select="concat('if (', $condition, ')')" />
				</xsl:when>
				<xsl:otherwise> <!-- assume this must be the else case, ie PICML backwards compatible -->
					<xsl:value-of select="'if ( true )'" />
				</xsl:otherwise>
				</xsl:choose>
				<!-- We are now ready to write the branch statements. -->
				<xsl:value-of select="'&#xA;{&#xA;'" />
				<!-- follow connection from Condition to next node -->
				<xsl:variable name="nextInBranch" select="$N/descendant::*/gml:node[@id=$E[@source=$conditionNode/@id]/@target]" />
				<xsl:if test="$nextInBranch"> 
					<!-- recurse -->
					<xsl:call-template name="Execution_Visitor">
						<xsl:with-param name="S" select="$S"/>
						<xsl:with-param name="T" select="$T"/>
						<xsl:with-param name="E" select="$E"/>
						<xsl:with-param name="N" select="$N"/>
						<xsl:with-param name="W" select="$nextInBranch"/>
						<xsl:with-param name="O" select="$originalPosition"/>
						<xsl:with-param name="B" select="concat($B,',',$O)"/> <!-- push order of branch on stack -->
						<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey" />
						<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey" />
						<xsl:with-param name="transformNodeWorkerKey"  select="$transformNodeWorkerKey" />
						<xsl:with-param name="transformNodeWorkerIDKey" select="$transformNodeWorkerIDKey"/>
						<xsl:with-param name="transformNodeOperationKey"  select="$transformNodeOperationKey" />
						<xsl:with-param name="transformNodeComplexityKey" select="$transformNodeComplexityKey"/>
						<xsl:with-param name="transformNodeComplexityParametersKey" select="$transformNodeComplexityParametersKey"/>
						<xsl:with-param name="transformNodeParametersKey" select="$transformNodeParametersKey"/>
						<xsl:with-param name="transformNodeActionOnKey"  select="$transformNodeActionOnKey" />
						<xsl:with-param name="transformNodeSortOrderKey"  select="$transformNodeSortOrderKey" />
						<xsl:with-param name="transformNodeValueKey"  select="$transformNodeValueKey" />
						<xsl:with-param name="transformNodeCodeKey"  select="$transformNodeCodeKey" />
						<xsl:with-param name="transformNodeTypeKey"  select="$transformNodeTypeKey" />
					</xsl:call-template>
				</xsl:if>
				<xsl:value-of select="'&#xA;}&#xA;'" />
			</xsl:for-each>
			<!-- else branch with no condition is final link from BranchState to Termination -->
			<xsl:value-of select="'else&#xA;{&#xA;'" />   
		</xsl:when>

		<xsl:when test="$W/gml:data[@key=$transformNodeKindKey]/text() = 'Termination'">
			<!-- Have we have finished the branching?
				Compare the in order of the terminal node to the condition position() -->
			<xsl:if test="$toOrder = $O">
				<xsl:value-of select="'&#xA;}&#xA;'" />
			</xsl:if> 
		</xsl:when>

		<xsl:when test="$W/gml:data[@key=$transformNodeKindKey]/text() = 'OutEventPortImpl'">
		
			<!-- get scoped_name for OutEventPortImpl type, not implemented as not doing package/module/namespace -->
			<!-- find aggregate for this OutEventPort for this OutEventPortImpl -->
			<xsl:variable name="OutEventPortId" select="$E[@source=$W/@id]/@target" />
			<xsl:variable name="typeNodeId" select="$E[@source=$OutEventPortId]/@target" />
			<xsl:variable name="typeNode" select="/descendant::*/gml:node[@id=$typeNodeId]/gml:data[@key=$transformNodeKindKey][text() = 'Aggregate']/.." />
			<xsl:variable name="typeName" select="concat($typeNode/gml:data[@key=$transformNodeLabelKey]/text(), $EventSuffix)" />

			<xsl:variable name="outEventPortName" select="$W/gml:data[@key=$transformNodeLabelKey]/text()" />
			<xsl:variable name="varName" select="concat('__event_', generate-id($W), '__')" />
			<xsl:choose>
			<xsl:when test="$typeName and not($typeName = '')" >
				<xsl:value-of select="concat('::', $typeName, '_var ', $varName, ' = this-&gt;ctx_-&gt;new_', $outEventPortName, '_event ();&#xA;')" />
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="concat('// could not locate scoped name for ', $outEventPortName, $transformNodeTypeKey, '&#xA;')" />
			</xsl:otherwise>
			</xsl:choose>
			
			<!-- SimpleProperty, ie top level aggregate members there is only one top level aggregate! -->
			<xsl:variable name="aggregateInstance" select="$W/gml:graph/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'AggregateInstance']/.." />
			<xsl:call-template name="Execution_Aggregate">
				<xsl:with-param name="processNodes" select="$aggregateInstance"/>
				<xsl:with-param name="varName" select="$varName" />
				<xsl:with-param name="level" select="'top'" />
				<xsl:with-param name="recursed" select="''" />
				<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey"/>
				<xsl:with-param name="transformNodeValueKey" select="$transformNodeValueKey"/>
				<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey"/>
				<xsl:with-param name="transformNodeTypeKey"  select="$transformNodeTypeKey" />
				<xsl:with-param name="transformNodeSortOrderKey"  select="$transformNodeSortOrderKey" />
			</xsl:call-template>
			
			<xsl:value-of select="concat('this->ctx_-&gt;push_', $outEventPortName, ' (', $varName, '.in ());&#xA;')" />
		</xsl:when>

		<xsl:when test="$W/gml:data[@key=$transformNodeKindKey]/text() = 'InEventPortImpl'">
		</xsl:when>

		</xsl:choose>

		<!-- keep track of branch order for each behaviour case -->
		<xsl:variable name="nextO">
			<xsl:choose>
			<xsl:when test="$W/gml:data[@key=$transformNodeKindKey]/text() = 'BranchState' or 
							$W/gml:data[@key=$transformNodeKindKey]/text() = 'WhileLoop'">
				<xsl:variable name="conditions" select="$W/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'Condition']/.." />
				<xsl:value-of select="count($conditions) + 1"/>
			</xsl:when>
			<xsl:when test="($W/gml:data[@key=$transformNodeKindKey]/text() = 'Termination') and ($toOrder = $O)">
				<xsl:variable name="lastDelim" select="string-length($B) - string-length(translate($B, ',', ''))" />
				<xsl:call-template name="splitList">
					<xsl:with-param name="pText" select="$B" />
					<xsl:with-param name="count" select="$lastDelim" />
					<xsl:with-param name="idx" select="$lastDelim" />
					<xsl:with-param name="delim" select="','" />
				</xsl:call-template>
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="$O" />
			</xsl:otherwise>
			</xsl:choose>
		</xsl:variable>

		<xsl:variable name="nextB">
			<xsl:choose>
			<xsl:when test="$W/gml:data[@key=$transformNodeKindKey]/text() = 'BranchState' or 
							$W/gml:data[@key=$transformNodeKindKey]/text() = 'WhileLoop'">
				<xsl:value-of select="concat($B,',',$O)"/>
			</xsl:when>
			<xsl:when test="($W/gml:data[@key=$transformNodeKindKey]/text() = 'Termination') and ($toOrder = $O)">
				<xsl:call-template name="stripLast">
					<xsl:with-param name="pText" select="$B" />
					<xsl:with-param name="delim" select="','" />
				</xsl:call-template>
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="$B" />
			</xsl:otherwise>
			</xsl:choose>
		</xsl:variable>
		
		<xsl:if test="$notOrLastTerminal = 'true'">
			<!-- follow connection to next node -->
			<xsl:variable name="next" select="$N/descendant::*/gml:node[@id=$E[@source=$W/@id]/@target]" />
			<xsl:if test="$next"> 
				<!-- recurse -->
				<xsl:call-template name="Execution_Visitor">
					<xsl:with-param name="S" select="$S"/>
					<xsl:with-param name="T" select="$T"/>
					<xsl:with-param name="E" select="$E"/>
					<xsl:with-param name="N" select="$N"/>
					<xsl:with-param name="W" select="$next"/>
					<xsl:with-param name="O" select="$nextO"/>
					<xsl:with-param name="B" select="$nextB"/>
					<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey" />
					<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey" />
					<xsl:with-param name="transformNodeWorkerKey"  select="$transformNodeWorkerKey" />
					<xsl:with-param name="transformNodeWorkerIDKey" select="$transformNodeWorkerIDKey"/>
					<xsl:with-param name="transformNodeOperationKey"  select="$transformNodeOperationKey" />
					<xsl:with-param name="transformNodeComplexityKey" select="$transformNodeComplexityKey"/>
					<xsl:with-param name="transformNodeComplexityParametersKey" select="$transformNodeComplexityParametersKey"/>
					<xsl:with-param name="transformNodeParametersKey" select="$transformNodeParametersKey"/>
					<xsl:with-param name="transformNodeActionOnKey"  select="$transformNodeActionOnKey" />
					<xsl:with-param name="transformNodeSortOrderKey"  select="$transformNodeSortOrderKey" />
					<xsl:with-param name="transformNodeValueKey"  select="$transformNodeValueKey" />
					<xsl:with-param name="transformNodeCodeKey"  select="$transformNodeCodeKey" />
					<xsl:with-param name="transformNodeTypeKey"  select="$transformNodeTypeKey" />
				</xsl:call-template>
			</xsl:if>
		</xsl:if>
	</xsl:template>
	
	<!-- Execution_Process -->
	<xsl:template name="Execution_Process">
		<xsl:param name="processNodes"/> 
		<xsl:param name="transformNodeKindKey" />
		<xsl:param name="transformNodeSortOrderKey" />
		<xsl:param name="transformNodeWorkerKey" />
		<xsl:param name="transformNodeWorkerIDKey" />
		<xsl:param name="transformNodeOperationKey" />
		<xsl:param name="transformNodeComplexityKey" />
		<xsl:param name="transformNodeComplexityParametersKey" />
		<xsl:param name="transformNodeParametersKey" />
		<xsl:param name="transformNodeCodeKey" />
		<xsl:param name="transformNodeLabelKey" />
		<xsl:param name="transformNodeValueKey" />
		<xsl:param name="transformNodeTypeKey" />
		
		<!-- Generate the worker operation calls, excludes pointer parameter for the Synthetic worker add_worker action. ??? -->
		
		<xsl:for-each select="$processNodes" >
			<xsl:sort select="./gml:data[@key=$transformNodeSortOrderKey]" data-type="number" order="ascending" />
			
			<xsl:variable name="processNode" select="." />
			<xsl:variable name="worker" select="$processNode/gml:data[@key=$transformNodeWorkerKey]/text()" />
			<xsl:variable name="workerID" select="$processNode/gml:data[@key=$transformNodeWorkerIDKey]/text()" />
			<xsl:variable name="processName" select="$processNode/gml:data[@key=$transformNodeLabelKey]/text()" />
			<xsl:variable name="opName" select="$processNode/gml:data[@key=$transformNodeOperationKey]/text()" />  
			<xsl:variable name="complexity" select="$processNode/gml:data[@key=$transformNodeComplexityKey]/text()" />  
			<xsl:variable name="complexityParameters" select="$processNode/gml:data[@key=$transformNodeComplexityParametersKey]/text()" />  
			<xsl:variable name="parameters" select="$processNode/gml:data[@key=$transformNodeParametersKey]/text()" />  
			<xsl:variable name="inputParameters" select="$processNode/gml:graph/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'InputParameter']/.." />
			<xsl:variable name="returnParameter" select="$processNode/gml:graph/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'ReturnParameter']/.." />
			<xsl:variable name="interfaceDefs" select="/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'InterfaceDefinitions']/.." />
			<xsl:variable name="behaviourDefs" select="/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'BehaviourDefinitions']/.." />
				
			<xsl:choose>
			<!-- Process VectorOperation, using input and return parameters -->		
			<xsl:when test="$worker = 'VectorOperation'" >
				<xsl:value-of select="concat('// Process ', $processName, ' ',$worker, ' ', $opName, '&#xA;')" />

				<!-- Find the type of the elements of the vector -->
				<xsl:variable name="sourceDataId" select="/descendant::*/gml:edge[@target=$inputParameters[1]/@id]/@source" />
				<xsl:variable name="inputTypeNodeId" select="/descendant::*/gml:edge[@source=$sourceDataId]/@target" />
				<xsl:variable name="inputTypeNode" select="$interfaceDefs/descendant::*/gml:node[@id=$inputTypeNodeId]" />
				<xsl:variable name="returnDataId" select="/descendant::*/gml:edge[@source=$returnParameter[1]/@id]/@target" />
				<xsl:variable name="returnDataNode" select="$behaviourDefs/descendant::*/gml:node[@id=$returnDataId]/." />
				
				<xsl:variable name="elementType" >
					<xsl:choose>
					<!-- return value for length must be ULong -->
					<xsl:when test="$opName = 'length'" >  
						<xsl:value-of select ="'UnsignedLongInteger'" />
					</xsl:when>
					<!-- type for set should be from source link for the WE_UTE_Vector -->
					<xsl:when test="$opName = 'set' and $sourceDataId and not($sourceDataId = '')" >
						<xsl:value-of select="$inputTypeNode/gml:graph/gml:node/gml:data[@key=$transformNodeTypeKey]/text()" />
					</xsl:when> 
					<!-- if return data is used, match type to linked object -->
					<xsl:when test="$returnDataId and not($returnDataId = '')" >
						<xsl:value-of select="$inputTypeNode/gml:graph/gml:node/gml:data[@key=$transformNodeTypeKey]/text()" /> 
						<!-- "$returnDataNode/gml:data[@key=$transformNodeTypeKey]/text()" /> -->
					</xsl:when>
					<!-- if returnParameter has a type use that -->
					<xsl:when test="not($returnParameter[1]/gml:data[@key=$transformNodeTypeKey]/text() = '')" >
						<xsl:value-of select="$returnParameter[1]/gml:data[@key=$transformNodeTypeKey]/text()" />
					</xsl:when>
					<!-- give a default type for return parameter -->
					<xsl:otherwise>
						<xsl:value-of select="'UnsignedLongInteger'" />
					</xsl:otherwise>
					</xsl:choose>
				</xsl:variable>	
			
				<!-- Get first parameter, should be a vector -->
				<xsl:variable name="param1">
					<xsl:call-template name="Execution_InputParameter">
						<xsl:with-param name="inputParameter" select="$inputParameters[1]"/>
						<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey" />
						<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey"/>
						<xsl:with-param name="transformNodeValueKey"  select="$transformNodeValueKey" />
					</xsl:call-template>
				</xsl:variable>
				
				<!-- Get second parameter, should be index for get, set or remove operation -->
				<xsl:variable name="param2">
					<xsl:call-template name="Execution_InputParameter">
						<xsl:with-param name="inputParameter" select="$inputParameters[2]"/>
						<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey" />
						<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey"/>
						<xsl:with-param name="transformNodeValueKey"  select="$transformNodeValueKey" />
					</xsl:call-template>
				</xsl:variable>
				
				<!-- Get third parameter, should be value for set operation -->
				<xsl:variable name="param3">
					<xsl:call-template name="Execution_InputParameter">
						<xsl:with-param name="inputParameter" select="$inputParameters[3]"/>
						<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey" />
						<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey"/>
						<xsl:with-param name="transformNodeValueKey"  select="$transformNodeValueKey" />
					</xsl:call-template>
				</xsl:variable>
				
				<!-- Check which operator to use, . if ev | -&gt; if _var -->
				<xsl:variable name="op">
					<xsl:choose>
					<xsl:when test="starts-with($param1, 'ev-&gt;')">
						<xsl:value-of select="'.'" />
					</xsl:when>
					<xsl:otherwise>
						<xsl:value-of select="'-&gt;'" />
					</xsl:otherwise>
					</xsl:choose>
				</xsl:variable>
				
				<!-- Write functions get, set, length, remove, clear --> 
				<xsl:choose>
				<xsl:when test="$opName = 'get'">
					<xsl:call-template name="MemberType">
						<xsl:with-param name="type" select="$elementType"/>
						<xsl:with-param name="retn_type" select="'true'"/>
					</xsl:call-template>
					<xsl:if test="$inputTypeNode/gml:graph/gml:node/gml:data[@key=$transformNodeKindKey]/text() = 'AggregateInstance'" >
						<xsl:value-of select="'_var'" />
					</xsl:if>
					<xsl:value-of select="concat(' __', $returnParameter[1]/gml:data[@key=$transformNodeLabelKey]/text(), '_', generate-id($returnParameter[1]), '__')" />
					<xsl:call-template name="MemberInitType">
						<xsl:with-param name="type" select="$elementType"/>
					</xsl:call-template>
					<xsl:value-of select="';&#xA;'" />
					<!-- test if index is within length -->
					<xsl:value-of select="concat('if ( ', $param2, '&lt; 0 || ', $param2, '&gt;= ', $param1, $op, 'length())&#xA;')" />
					<xsl:value-of select="'   ACE_DEBUG ((LM_ERROR, ACE_TEXT (&quot;VectorOperation index outside vector length\n&quot;) ));&#xA;'" />
					<xsl:value-of select="'else {&#xA;   '" />
					<xsl:value-of select="concat(' __', $returnParameter[1]/gml:data[@key=$transformNodeLabelKey]/text(), '_', generate-id($returnParameter[1]), '__ = '  )" />
					<xsl:if test="$inputTypeNode/gml:graph/gml:node/gml:data[@key=$transformNodeKindKey]/text() = 'AggregateInstance'" >
						<xsl:value-of select="'new '" />
					</xsl:if>
					<!-- make result to the appropriate type -->
					<xsl:value-of select="'('" />
					<xsl:call-template name="MemberType">
						<xsl:with-param name="type" select="$elementType"/>
						<xsl:with-param name="retn_type" select="'true'"/>
					</xsl:call-template>
					<xsl:value-of select="') '" />
					<xsl:if test="$elementType = 'String' or $elementType = 'WideString'">
						<xsl:value-of select="'CORBA::string_dup'" />
					</xsl:if>
					<xsl:value-of select="concat('(', $param1, '[ (CORBA::ULong) ', $param2, ' ]')" />
					<xsl:if test="$elementType = 'String' or $elementType = 'WideString'">
						<xsl:value-of select="'.in ()'" />
					</xsl:if>
					<xsl:value-of select="');&#xA;'" />
					<xsl:value-of select="'}&#xA;'" />
				</xsl:when>
				<xsl:when test="$opName = 'set'">
					<!-- test if length is within bounds -->
					<xsl:value-of select="concat('if ( ', $param2, '&gt;= ', $param1, $op, 'maximum())&#xA;')" />
					<xsl:value-of select="'   ACE_DEBUG ((LM_ERROR, ACE_TEXT (&quot;VectorOperation exceeds bounded size\n&quot;) ));&#xA;'" />
					<xsl:value-of select="'else {&#xA;'" />
					<xsl:value-of select="concat('   if (', $param2, '&gt;= ', $param1, $op, 'length() ) ', $param1, $op, 'length (', $param2, '+ 1);&#xA;' ) " />
					<xsl:value-of select="concat('   ', $param1, '[ (CORBA::ULong) ', $param2, '] = ' ) " />
					<xsl:if test="$elementType = 'String' or $elementType = 'WideString'">
						<xsl:value-of select="'CORBA::string_dup'" />
					</xsl:if>
					<xsl:value-of select="concat('(', $param3, ');&#xA;')" />
					<xsl:value-of select="'}&#xA;'" />
				</xsl:when>
				<xsl:when test="$opName = 'length'">				
					<xsl:call-template name="MemberType">
						<xsl:with-param name="type" select="$elementType"/>
						<xsl:with-param name="retn_type" select="'true'"/>
					</xsl:call-template>
					<xsl:value-of select="concat(' __', $returnParameter[1]/gml:data[@key=$transformNodeLabelKey]/text(), '_', generate-id($returnParameter[1]), '__'  )" />
					<!-- cast result to the appropriate type -->
					<xsl:value-of select="' = ('" />
					<xsl:call-template name="MemberType">
						<xsl:with-param name="type" select="$elementType"/>
						<xsl:with-param name="retn_type" select="'true'"/>
					</xsl:call-template>
					<xsl:value-of select="') '" />
					<xsl:value-of select="concat($param1, $op, 'length ();&#xA;')" />
				</xsl:when>
				<xsl:when test="$opName = 'remove'">
					<xsl:call-template name="MemberType">
						<xsl:with-param name="type" select="$elementType"/>
						<xsl:with-param name="retn_type" select="'true'"/>
					</xsl:call-template>
					<xsl:if test="$inputTypeNode/gml:graph/gml:node/gml:data[@key=$transformNodeKindKey]/text() = 'AggregateInstance'" >
						<xsl:value-of select="'_var'" />
					</xsl:if>
					<xsl:value-of select="concat(' __', $returnParameter[1]/gml:data[@key=$transformNodeLabelKey]/text(), '_', generate-id($returnParameter[1]), '__')" />
					<xsl:call-template name="MemberInitType">
						<xsl:with-param name="type" select="$elementType"/>
					</xsl:call-template>
					<xsl:value-of select="';&#xA;'" />
					<!-- test if index is within length -->
					<xsl:value-of select="concat('if ( ', $param2, '&lt; 0 || ', $param2, '&gt;= ', $param1, $op, 'length())&#xA;')" />
					<xsl:value-of select="'   ACE_DEBUG ((LM_ERROR, ACE_TEXT (&quot;VectorOperation index outside vector length\n&quot;) ));&#xA;'" />
					<xsl:value-of select="'else {&#xA;   '" />
					<xsl:value-of select="concat(' __', $returnParameter[1]/gml:data[@key=$transformNodeLabelKey]/text(), '_', generate-id($returnParameter[1]), '__ = '  )" />
					<xsl:if test="$inputTypeNode/gml:graph/gml:node/gml:data[@key=$transformNodeKindKey]/text() = 'AggregateInstance'" >
						<xsl:value-of select="'new '" />
					</xsl:if>
					<!-- make result to the appropriate type -->
					<xsl:value-of select="'('" />
						<xsl:call-template name="MemberType">
						<xsl:with-param name="type" select="$elementType"/>
						<xsl:with-param name="retn_type" select="'true'"/>
					</xsl:call-template>
					<xsl:value-of select="') '" />
					<xsl:if test="$elementType = 'String' or $elementType = 'WideString'">
						<xsl:value-of select="'CORBA::string_dup'" />
					</xsl:if>
					<xsl:value-of select="concat('(', $param1, '[ (CORBA::ULong) ', $param2, ']')" />
					<xsl:if test="$elementType = 'String' or $elementType = 'WideString'">
						<xsl:value-of select="'.in ()'" />
					</xsl:if>
					<xsl:value-of select="');&#xA;'" />
					<xsl:value-of select="concat('   for (CORBA::ULong i = ', $param2, '; i &lt; ', $param1, $op, 'length() - 1; i++)&#xA;')" />
					<xsl:value-of select="concat('      ', $param1, '[i] = ', $param1, '[i + 1];&#xA;')" />
					<xsl:value-of select="concat('   ', $param1, $op, 'length(', $param1, $op, 'length() - 1);&#xA;')" />
					<xsl:value-of select="'}&#xA;'" />
				</xsl:when>
				<xsl:when test="$opName = 'clear'">
					<xsl:value-of select="concat($param1, $op, 'length (0);&#xA;')" />
				</xsl:when>
				<xsl:otherwise>
					<xsl:value-of select="'// unknown worker operation &#xA;'" />
				</xsl:otherwise>
				</xsl:choose>
			</xsl:when>

			<!-- Process Worker defined operation, using input and return parameters -->		
			<xsl:when test="$opName and not($opName = '') and ( count($inputParameters) &gt; 0 or count($returnParameter) &gt; 0 )" >
				<xsl:if test="count($returnParameter) &gt; 0">
					<xsl:call-template name="MemberType">
						<xsl:with-param name="type" select="$returnParameter[1]/gml:data[@key=$transformNodeTypeKey]/text()"/>
						<xsl:with-param name="retn_type" select="'true'"/>
					</xsl:call-template>
					<xsl:value-of select="concat(' __', $returnParameter[1]/gml:data[@key=$transformNodeLabelKey]/text(), '_', generate-id($returnParameter[1]), '__ = '  )" />
				</xsl:if>
				<!-- changed to use workerID rather than process label -->
				<xsl:choose>
				<xsl:when test="$workerID and not($workerID = '')">
					<xsl:value-of select="concat('this->', $workerID, '_.', $opName, ' (')" />
				</xsl:when>
				<xsl:otherwise>
					<xsl:value-of select="concat('this->', $processName, '_.', $opName, ' (')" />
				</xsl:otherwise>
				</xsl:choose>
				<xsl:call-template name="Execution_InputParameters">
						<xsl:with-param name="inputParameters" select="$inputParameters"/>
						<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey" />
						<xsl:with-param name="transformNodeSortOrderKey" select="$transformNodeSortOrderKey"/>
						<xsl:with-param name="transformNodeWorkerKey" select="$transformNodeWorkerKey"/>
						<xsl:with-param name="transformNodeWorkerIDKey" select="$transformNodeWorkerIDKey"/>
						<xsl:with-param name="transformNodeOperationKey" select="$transformNodeOperationKey"/>
						<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey"/>
						<xsl:with-param name="transformNodeValueKey"  select="$transformNodeValueKey" />
						<xsl:with-param name="transformNodeTypeKey"  select="$transformNodeTypeKey" />
				</xsl:call-template> 
				<xsl:value-of select="');&#xA;'" />
			</xsl:when>
			
			<!-- Process with Worker operation, deprecated format of complexity and parameters functionality -->			
			<xsl:when test="$opName and not($opName = '') and ($complexity or $complexityParameters or $parameters)">
				<!-- changed to use workerID rather than process label -->
				<xsl:choose>
				<xsl:when test="$workerID and not($workerID = '')">
				<xsl:value-of select="concat('this->', $workerID, '_.', $opName, ' (')" />
				</xsl:when>
				<xsl:otherwise>
				<xsl:value-of select="concat('this->', $processName, '_.', $opName, ' (')" />
				</xsl:otherwise>
				</xsl:choose>				
				<!-- Generate the parameters for the action, where complexity parameters are defined separately. -->
				<!-- if complexity is given assume that the evaluateComplexity() is is the first parameter -->
				<xsl:if test="$complexity and not($complexity = '')">
					<xsl:value-of select="concat('this->we_ute_.evaluateComplexity(', $complexity, ',')" />
					<!-- Write the value of the other parameters in a comma delimited string value -->
					<!-- cast all parameters double -->
					<xsl:call-template name="doubleList">
						<xsl:with-param name="pText" select="$complexityParameters" />
						<xsl:with-param name="delim" select="','" />
					</xsl:call-template>
					<xsl:value-of select="')'" />
				</xsl:if>
				<xsl:if test="$complexity and not($complexity = '') and $parameters and not($parameters = '')">
					<xsl:value-of select="', '" />
				</xsl:if>
				<xsl:if test="$parameters and not($parameters = '')">
					<!-- Write the value of the other parameters in a comma delimited string value -->
					<xsl:value-of select="$parameters" />
				</xsl:if>
				<xsl:value-of select="');&#xA;'" />
			</xsl:when>
			
			<xsl:otherwise>
				<!-- Visit_InputEffect/Effect/etc and Write the postcondition for this <effect>. -->
				<xsl:value-of select="concat($processNode/gml:data[@key=$transformNodeCodeKey]/text(), '&#xA;')" />
			</xsl:otherwise>
			</xsl:choose>
							
			<!-- if returnParameter linked to Variable, set the value now ??? tmp solution until VariableOperations are implemented ??? --> 
			<xsl:variable name="returnDataId" select="/descendant::*/gml:edge[@source=$returnParameter[1]/@id]/@target" />
			<xsl:for-each select="$returnDataId" >
				<xsl:variable name="rtnDataId" select="." />
				<xsl:variable name="outputNode" select="$behaviourDefs/descendant::*/gml:node[@id=$rtnDataId]"/>	
				<xsl:if test="$outputNode/gml:data[@key=$transformNodeKindKey]/text() = 'Variable'">
					<xsl:value-of select="concat('this-&gt;', $outputNode/gml:data[@key=$transformNodeLabelKey]/text(), '_ ' )" />
					<xsl:value-of select="' = '" />
					<xsl:value-of select="concat('__', $returnParameter[1]/gml:data[@key=$transformNodeLabelKey]/text(), '_', generate-id($returnParameter[1]), '__ ;&#xA;' )" />
				</xsl:if>
				<xsl:if test="$outputNode/../../gml:data[@key=$transformNodeKindKey]/text() = 'Variable'">
					<xsl:value-of select="concat('this-&gt;', $outputNode/../../gml:data[@key=$transformNodeLabelKey]/text(), '_ ' )" />
					<xsl:value-of select="' = '" />
					<xsl:value-of select="concat('__', $returnParameter[1]/gml:data[@key=$transformNodeLabelKey]/text(), '_', generate-id($returnParameter[1]), '__ ;&#xA;' )" />
				</xsl:if>
			</xsl:for-each>
			<xsl:value-of select="'&#xA;'" />
		</xsl:for-each>
	</xsl:template>

	<!-- Execution_InputParameters -->
	<xsl:template name="Execution_InputParameters">
		<xsl:param name="inputParameters"/> 
		<xsl:param name="transformNodeKindKey" />
		<xsl:param name="transformNodeSortOrderKey" />
		<xsl:param name="transformNodeWorkerKey" />
		<xsl:param name="transformNodeWorkerIDKey" />
		<xsl:param name="transformNodeOperationKey" />
		<xsl:param name="transformNodeLabelKey" />
		<xsl:param name="transformNodeValueKey" />
		<xsl:param name="transformNodeTypeKey" />
		
		<xsl:variable name="worker" select="$inputParameters/../../gml:data[@key=$transformNodeWorkerKey]/text()" />
		<xsl:variable name="opName" select="$inputParameters/../../gml:data[@key=$transformNodeOperationKey]/text()" />  
		<xsl:for-each select="$inputParameters" >
			<xsl:sort select="./gml:data[@key=$transformNodeSortOrderKey]" data-type="number" order="ascending" /> 
			<xsl:variable name="inputParameter" select="." />
			<xsl:if test="position() &gt; 1" >
				<xsl:value-of select="', '" />
			</xsl:if>
			<!-- deal with all the different types of parameters available -->	
			<xsl:variable name="paramType" select="$inputParameter/gml:data[@key=$transformNodeTypeKey]/text()" />
			<xsl:variable name="param">
				<xsl:call-template name="Execution_InputParameter">
					<xsl:with-param name="inputParameter" select="$inputParameter"/>
					<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey" />
					<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey"/>
					<xsl:with-param name="transformNodeValueKey"  select="$transformNodeValueKey" />
				</xsl:call-template> 
			</xsl:variable>

			<!-- for WE_GPU processes with WE_UTE_Vector parameters, use the WE_UTE_Vector structure to pass a void pointer  -->
			<xsl:if test="$worker = 'WE_GPU' and $paramType = 'WE_UTE_Vector'" > 
				<xsl:value-of select="'WE_UTE_Vector'" />
			</xsl:if>
			<!-- for WE_UTE evaluateComplexity need to cast to double -->
			<xsl:if test="position() &gt; 1 and $worker = 'WE_UTE' and $opName = 'evaluateComplexity'" > 
				<xsl:value-of select="'(double)'" />
			</xsl:if>

			<xsl:value-of select="concat('(', $param)" />
			
			<!-- need to add .in () function for variables of _var type, ie strings and vectors. -->
			<xsl:variable name="behaviourDefs" select="/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'BehaviourDefinitions']/.." />
			<xsl:variable name="sourceDataId" select="/descendant::*/gml:edge[@target=$inputParameter/@id]/@source" />
			<xsl:variable name="sourceDataNode" select="$behaviourDefs/descendant::*/gml:node[@id=$sourceDataId]/." />
			<xsl:variable name="sourceDataNodeType" select="$sourceDataNode/gml:data[@key=$transformNodeTypeKey]/text()" />
			
			<xsl:if test="($paramType = 'WE_UTE_Vector' or $sourceDataNodeType = 'String' or $sourceDataNodeType = 'WideString')
							and not( starts-with($param, 'ev-&gt;') )" >
				<xsl:value-of select="'.in ()'" />
			</xsl:if>
			<xsl:value-of select="' )'" />
		</xsl:for-each>	

	</xsl:template>
	
	<!-- Execution_InputParameter -->
	<xsl:template name="Execution_InputParameter">
		<xsl:param name="inputParameter"/> 
		<xsl:param name="transformNodeKindKey" />
		<xsl:param name="transformNodeLabelKey" />
		<xsl:param name="transformNodeValueKey" />
		
			<xsl:variable name="behaviourDefs" select="/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'BehaviourDefinitions']/.." />
			<xsl:variable name="sourceDataId" select="/descendant::*/gml:edge[@target=$inputParameter/@id]/@source" />
			<xsl:variable name="sourceDataNode" select="$behaviourDefs/descendant::*/gml:node[@id=$sourceDataId]/." />
			<xsl:variable name="sourceDataNodeKind" select="$sourceDataNode/gml:data[@key=$transformNodeKindKey]/text()" />
			<xsl:variable name="sourceDataEvent" select="$sourceDataNode/ancestor::*/gml:data[@key=$transformNodeKindKey][text() = 'InEventPortImpl']/.." />
			<xsl:variable name="sourceDataVariable" select="$sourceDataNode/ancestor::*/gml:data[@key=$transformNodeKindKey][text() = 'Variable']/.." />

			<xsl:if test ="$sourceDataId[1]" >
				<xsl:choose>
				<xsl:when test="$sourceDataNodeKind = 'Variable'">
					<xsl:value-of select="concat('this-&gt;', $sourceDataNode[1]/gml:data[@key=$transformNodeLabelKey]/text(), '_ ' )" />
				</xsl:when>

				<!-- ancestor is a Variable, so get data from variable using structured data --> 
				<xsl:when test="$sourceDataVariable[last()]">
					<!-- event data form depends on the nested aggregates, nested levels accessed as part of structure -->
					<xsl:variable name="firstLevelAggregate" select="$sourceDataVariable[last()]/gml:graph/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'AggregateInstance' or text() = 'VectorInstance']/.." />
					<xsl:call-template name="recurseParameter">
						<xsl:with-param name="variableName" select="concat($sourceDataVariable[last()]/gml:data[@key=$transformNodeLabelKey]/text(), '_')"/>
						<xsl:with-param name="firstLevelAggregate" select="$firstLevelAggregate"/>
						<xsl:with-param name="firstLevelFunction" select="'false'"/>
						<xsl:with-param name="sourceDataNode" select="$sourceDataNode" />
						<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey" />
					</xsl:call-template> 
				</xsl:when>
				
				<xsl:when test="$sourceDataNodeKind = 'AttributeImpl'">
					<xsl:value-of select="concat('this-&gt;', $sourceDataNode[1]/gml:data[@key=$transformNodeLabelKey]/text(), ' () ' )" />
				</xsl:when>

				<xsl:when test="$sourceDataNodeKind = 'ReturnParameter'">
					<xsl:value-of select="concat('__', $sourceDataNode[1]/gml:data[@key=$transformNodeLabelKey]/text(), '_', generate-id($sourceDataNode[1]), '__ ' )" />
				</xsl:when>
				
				<!-- ancestor is an InEventPortImpl, so get data from event --> 
				<xsl:when test="$sourceDataEvent[last()]">
					<!-- event data form depends on the nested aggregates, nested levels accessed as part of structure -->
					<xsl:variable name="firstLevelAggregate" select="$sourceDataEvent[last()]/gml:graph/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'AggregateInstance' or text() = 'VectorInstance']/.." />
					<xsl:call-template name="recurseParameter">
						<xsl:with-param name="variableName" select="'ev'"/>
						<xsl:with-param name="firstLevelAggregate" select="$firstLevelAggregate"/>
						<xsl:with-param name="firstLevelFunction" select="'true'"/>
						<xsl:with-param name="sourceDataNode" select="$sourceDataNode" />
						<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey" />
					</xsl:call-template> 
				</xsl:when>
				</xsl:choose>
			</xsl:if>
			<!-- if no link to data source exists, default to value provided -->
			<xsl:if test="not($sourceDataId[1])">
				<xsl:value-of select="$inputParameter/gml:data[@key=$transformNodeValueKey]/text()" />
			</xsl:if>
	</xsl:template>
	
	<!-- For structured data need to recurse to add all levels of structre -->
	 <xsl:template name="recurseParameter">
		<xsl:param name="variableName" />
		<xsl:param name="firstLevelAggregate" />
		<xsl:param name="firstLevelFunction" />
		<xsl:param name="sourceDataNode" />
		<xsl:param name="transformNodeLabelKey" />
		
		<xsl:choose>
		<xsl:when test="$sourceDataNode[1]/@id = $firstLevelAggregate/@id and $variableName = 'ev'">
			<xsl:value-of select="concat('__', $variableName, '_base__')" />
		</xsl:when>

		<xsl:when test="$sourceDataNode[1]/@id = $firstLevelAggregate/@id">
			<xsl:value-of select="$variableName" />
		</xsl:when>

		<xsl:when test="$sourceDataNode[1]/../../@id != $firstLevelAggregate/@id">
			<xsl:call-template name="recurseParameter">
				<xsl:with-param name="variableName" select="$variableName"/>
				<xsl:with-param name="firstLevelAggregate" select="$firstLevelAggregate"/>
				<xsl:with-param name="firstLevelFunction" select="$firstLevelFunction"/>
				<xsl:with-param name="sourceDataNode" select="$sourceDataNode[1]/../.." />
				<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey" />
			</xsl:call-template> 
			
			<xsl:value-of select="concat('.',  $sourceDataNode[1]/gml:data[@key=$transformNodeLabelKey]/text() )" />
		</xsl:when>

		<xsl:otherwise>
			<xsl:value-of select="concat( $variableName, '-&gt;', $sourceDataNode[1]/gml:data[@key=$transformNodeLabelKey]/text() )" />
			<xsl:if test="$firstLevelFunction = 'true'" > 
				<xsl:value-of select="' ()'" />
			</xsl:if>
		</xsl:otherwise>
		</xsl:choose>
	</xsl:template>
	
	<!-- Execution_Aggregate -->
	<xsl:template name="Execution_Aggregate">
		<xsl:param name="processNodes"/> 
		<xsl:param name="varName" />
		<xsl:param name="level" />
		<xsl:param name="recursed" />
		<xsl:param name="transformNodeKindKey" />
		<xsl:param name="transformNodeValueKey" />
		<xsl:param name="transformNodeLabelKey" />
		<xsl:param name="transformNodeTypeKey" />
		<xsl:param name="transformNodeSortOrderKey" />
		
		<xsl:variable name="instances" select="$processNodes/gml:graph/gml:node" />
		<!-- process each memberInstance, aggregateInstance and vectorInstance -->
		<xsl:for-each select="$instances" >
			<!-- order not important for assigning values to MemberInstance of OutEventPorts -->
			<!-- <xsl:sort select="./gml:data[@key=$transformNodeYKey]" data-type="number" order="ascending" /> -->
			<xsl:variable name="instNode" select="." />
			<xsl:variable name="instType" select="$instNode/gml:data[@key=$transformNodeKindKey]/text()" />
			
			<xsl:choose>
			<xsl:when test="$instType = 'MemberInstance'">
				<!-- Write the contents for a simple property. -->
				<xsl:variable name="memberName" select="$instNode/gml:data[@key=$transformNodeLabelKey]/text()" />
				<xsl:choose>
				<xsl:when test="$level = 'top'">
					<!-- top level aggregate members need different format -->
					<xsl:value-of select="concat($varName, '-&gt;', $memberName, ' (')" />
				</xsl:when>
				<xsl:otherwise>
					<xsl:value-of select="concat($recursed, '.', $memberName, ' = (')" />
				</xsl:otherwise>
				</xsl:choose>
				<!-- check if connected to attribute or variable, otherwise use value -->
				<xsl:call-template name="Execution_InputParameter">
					<xsl:with-param name="inputParameter" select="$instNode"/>
					<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey" />
					<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey"/>
					<xsl:with-param name="transformNodeValueKey"  select="$transformNodeValueKey" />
				</xsl:call-template>
				<!-- if (value == "$TIMEOFDAY") memberValue = "ACE_OS::gettimeofday ().msec ()"; ??? -->
				<xsl:value-of select="');&#xA;'" />
			</xsl:when>
			<xsl:when test="$instType = 'AggregateInstance'">
				<xsl:variable name="aggregateName" select="$instNode/gml:data[@key=$transformNodeLabelKey]/text()" />
				<xsl:variable name="structure" >
					<xsl:choose>
					<xsl:when test="$level = 'top'">
						<!-- top level aggregate members start recursing -->
						<xsl:value-of select="concat($varName, '-&gt;', $aggregateName, ' ()')" />
					</xsl:when>
					<xsl:otherwise>
						<!-- continue recursing -->
						<xsl:value-of select="concat($recursed, '.', $aggregateName)" />
					</xsl:otherwise>
					</xsl:choose>
				</xsl:variable>
				<!-- recursive function -->
				<xsl:call-template name="Execution_Aggregate">
					<xsl:with-param name="processNodes" select="$instNode"/>
					<xsl:with-param name="varName" select="$varName" />
					<xsl:with-param name="level" select="'aggregate'" />
					<xsl:with-param name="recursed" select="$structure" />
					<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey"/>
					<xsl:with-param name="transformNodeValueKey" select="$transformNodeValueKey"/>
					<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey"/>
					<xsl:with-param name="transformNodeTypeKey"  select="$transformNodeTypeKey" />
					<xsl:with-param name="transformNodeSortOrderKey"  select="$transformNodeSortOrderKey" />
				</xsl:call-template>
			</xsl:when>
			<xsl:when test="$instType = 'VectorInstance'">
				<xsl:variable name="vectorName" select="$instNode/gml:data[@key=$transformNodeLabelKey]/text()" />
				<xsl:variable name="vectorType" select="$instNode/gml:data[@key=$transformNodeTypeKey]/text()" />
				<!-- Create vector to load values 
				<xsl:value-of select="concat($vectorType, '* buf_', generate-id($instNode) ,' = new ::', $vectorType, '(')"/>  -->
				<xsl:variable name="eventParam">
					<xsl:call-template name="Execution_InputParameter">
						<xsl:with-param name="inputParameter" select="$instNode"/>
						<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey" />
						<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey"/>
						<xsl:with-param name="transformNodeValueKey"  select="$transformNodeValueKey" />
					</xsl:call-template>	
				</xsl:variable>
				<!-- assign vector to event -->
				<xsl:value-of select="concat($varName, '-&gt;', $vectorName, ' (') " />
				<xsl:value-of select="$eventParam" />
				<xsl:if test="not( starts-with($eventParam, 'ev-&gt;') )" >
					<xsl:value-of select="'.in ()'" />
				</xsl:if>	
				<xsl:value-of select="');&#xA;'" />	
			</xsl:when>
			</xsl:choose>
		</xsl:for-each>
	</xsl:template>
	
	<!-- Execution_Vector -->
	<xsl:template name="Execution_Vector">
		<xsl:param name="processNodes"/> 
		<xsl:param name="varName" />
		<xsl:param name="level" />
		<xsl:param name="recursed" />
		<xsl:param name="transformNodeKindKey" />
		<xsl:param name="transformNodeValueKey" />
		<xsl:param name="transformNodeLabelKey" />
		<xsl:param name="transformNodeTypeKey" />
		<xsl:param name="transformNodeSortOrderKey" />
		
		<xsl:variable name="instances" select="$processNodes/gml:graph/gml:node" />
		<!-- process each memberInstance and aggregateInstance within a vectorInstance, assumes only one vectorInstance can exist in hierarchy -->
		<xsl:for-each select="$instances" >
			<!-- order not important for assigning values to MemberInstance of OutEventPorts -->
			<!-- <xsl:sort select="./gml:data[@key=$transformNodeYKey]" data-type="number" order="ascending" /> -->
			<xsl:variable name="instNode" select="." />
			<xsl:variable name="instType" select="$instNode/gml:data[@key=$transformNodeKindKey]/text()" />
			
			<xsl:choose>
			<xsl:when test="$instType = 'MemberInstance'">
				<!-- Write the contents for a simple property. -->
				<xsl:variable name="memberName" select="$instNode/gml:data[@key=$transformNodeLabelKey]/text()" />
				<xsl:variable name="memberValue" select="$instNode/gml:data[@key=$transformNodeValueKey]/text()" />
				<xsl:variable name="memberType" select="$instNode/gml:data[@key=$transformNodeTypeKey]/text()" />
				<xsl:variable name="aggregateSortOrder" select="floor( number( $instNode/gml:data[@key=$transformNodeSortOrderKey]/text() ) )" />
				<xsl:variable name="stringDup">
					<xsl:choose>
					<xsl:when test="$memberType = 'String'">
						<xsl:value-of select="' CORBA::string_dup '" />
					</xsl:when>
					<xsl:otherwise>
						<xsl:value-of select="''" />
					</xsl:otherwise>
					</xsl:choose>
				</xsl:variable>
				<xsl:if test="$memberValue != ''">
					<xsl:choose>
					<xsl:when test="$level = 'top'">
						<!-- top level aggregate members need different format -->
						<xsl:value-of select="concat($varName, '[', $aggregateSortOrder, '] = ', $stringDup, ' (', $memberValue, ');&#xA;' )" />
					</xsl:when>
					<xsl:otherwise>
						<xsl:value-of select="concat($recursed, '.', $memberName, ' = ', $stringDup, ' (', $memberValue, ');&#xA;')" />
					</xsl:otherwise>
					</xsl:choose>
				</xsl:if>
			</xsl:when>
			<xsl:when test="$instType = 'AggregateInstance'">
				<xsl:variable name="aggregateName" select="$instNode/gml:data[@key=$transformNodeLabelKey]/text()" />
				<xsl:variable name="aggregateSortOrder" select="floor( number( $instNode/gml:data[@key=$transformNodeSortOrderKey]/text() ) )" />
				<xsl:variable name="structure" >
					<xsl:choose>
					<xsl:when test="$level = 'top'">
						<!-- top level vector members start recursing -->
						<xsl:value-of select="concat($varName, '[', $aggregateSortOrder, ']')" />
					</xsl:when>
					<xsl:otherwise>
						<!-- continue recursing -->
						<xsl:value-of select="concat($recursed, '.', $aggregateName)" />
					</xsl:otherwise>
					</xsl:choose>
				</xsl:variable>
				<!-- recursive function -->
				<xsl:call-template name="Execution_Vector">
					<xsl:with-param name="processNodes" select="$instNode"/>
					<xsl:with-param name="varName" select="$varName" />
					<xsl:with-param name="level" select="'aggregate'" />
					<xsl:with-param name="recursed" select="$structure" />
					<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey"/>
					<xsl:with-param name="transformNodeValueKey" select="$transformNodeValueKey"/>
					<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey"/>
					<xsl:with-param name="transformNodeTypeKey"  select="$transformNodeTypeKey" />
					<xsl:with-param name="transformNodeSortOrderKey"  select="$transformNodeSortOrderKey" />
				</xsl:call-template>
			</xsl:when>
			</xsl:choose>
		</xsl:for-each>
	</xsl:template>
	
	<!-- BlackBox Wrapper cpp file -->
	<xsl:template name="Wrap_Generator">
		<xsl:param name="node" />
		<xsl:param name="transformNodeKindKey" />
		<xsl:param name="transformNodeLabelKey" />
		<xsl:param name="transformNodeFolderKey" />
		<xsl:param name="transformNodeWorkerKey" />
		<xsl:param name="transformNodeWorkerIDKey" />
		<xsl:param name="transformNodeOperationKey" />
		<xsl:param name="transformNodeComplexityKey" />
		<xsl:param name="transformNodeComplexityParametersKey" />
		<xsl:param name="transformNodeParametersKey" />
		<xsl:param name="transformNodeMiddlewareKey" />
		<xsl:param name="transformNodeAsyncKey" />
		<xsl:param name="transformNodeTypeKey" />
		<xsl:param name="transformNodeActionOnKey" />
		<xsl:param name="transformNodeValueKey" />
		<xsl:param name="transformNodeCodeKey" />
		<xsl:param name="transformNodeFrequencyKey" />
		<xsl:param name="transformNodeSortOrderKey" />
		
		<!-- Extract the component type being implemented. -->
		<xsl:call-template name="Component_Wrap_Begin">
			<xsl:with-param name="node" select="$node" />
			<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey"/>
			<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey"/>
			<xsl:with-param name="transformNodeMiddlewareKey" select="$transformNodeMiddlewareKey"/>
			<xsl:with-param name="transformNodeAsyncKey" select="$transformNodeAsyncKey"/>
			<xsl:with-param name="transformNodeValueKey" select="$transformNodeValueKey"/>
			<xsl:with-param name="transformNodeFrequencyKey" select="$transformNodeFrequencyKey"/>
			<xsl:with-param name="transformNodeTypeKey" select="$transformNodeTypeKey"/>
		</xsl:call-template>	
		
		<!-- Visit the component. -->
		<xsl:call-template name="Visit_Wrap_Component">
			<xsl:with-param name="node" select="$node" />
			<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey"/>
			<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey"/>
			<xsl:with-param name="transformNodeFolderKey" select="$transformNodeFolderKey"/>
			<xsl:with-param name="transformNodeWorkerKey" select="$transformNodeWorkerKey"/>
			<xsl:with-param name="transformNodeWorkerIDKey" select="$transformNodeWorkerIDKey"/>
			<xsl:with-param name="transformNodeOperationKey" select="$transformNodeOperationKey"/>
			<xsl:with-param name="transformNodeComplexityKey" select="$transformNodeComplexityKey"/>
			<xsl:with-param name="transformNodeComplexityParametersKey" select="$transformNodeComplexityParametersKey"/>
			<xsl:with-param name="transformNodeParametersKey" select="$transformNodeParametersKey"/>
			<xsl:with-param name="transformNodeMiddlewareKey" select="$transformNodeMiddlewareKey"/>
			<xsl:with-param name="transformNodeAsyncKey" select="$transformNodeAsyncKey"/>
			<xsl:with-param name="transformNodeValueKey" select="$transformNodeValueKey"/>
			<xsl:with-param name="transformNodeCodeKey" select="$transformNodeCodeKey"/>
			<xsl:with-param name="transformNodeTypeKey" select="$transformNodeTypeKey"/>
			<xsl:with-param name="transformNodeActionOnKey" select="$transformNodeActionOnKey"/>
			<xsl:with-param name="transformNodeSortOrderKey" select="$transformNodeSortOrderKey"/>
		</xsl:call-template>	
		
		<!-- Write the end of the component's implementation. -->
		<xsl:value-of select="'}&#xA;'" />
	
		<!-- generate the implementation entrypoint for this component's implementation. -->
		<xsl:variable name="nodeName" select="$node/gml:data[@key=$transformNodeLabelKey]/text()" />
		<xsl:variable name="entrypoint" select="concat('create_', $nodeName, '_', $ImplSuffix)" />
		<xsl:variable name="export_basename" select="concat($nodeName, $ImplSuffix)" />
		<xsl:variable name="export_macro" select="translate($export_basename,
								'abcdefghijklmnopqrstuvwxyz',
								'ABCDEFGHIJKLMNOPQRSTUVWXYZ')" />
	
		<xsl:value-of select="concat('&#xA;//&#xA;// ', $entrypoint, '&#xA;//&#xA;')" />
		<xsl:value-of select="'::Components::EnterpriseComponent_ptr&#xA;'" />

		<xsl:value-of select="concat($entrypoint, ' (void)', '&#xA;{&#xA;')" />
		<xsl:value-of select="'::Components::EnterpriseComponent_ptr retval =&#xA;'" />
		<xsl:value-of select="'  ::Components::EnterpriseComponent::_nil ();&#xA;&#xA;'" />
		<xsl:value-of select="'ACE_NEW_RETURN (retval,&#xA;'" />
		<xsl:value-of select="concat('::', $export_basename, '::', $nodeName)" />
		<xsl:value-of select="' (),&#xA;'" />
		<xsl:value-of select="'::Components::EnterpriseComponent::_nil ());&#xA;'" />
		<xsl:value-of select="'return retval;&#xA;}&#xA;&#xA;'" />
	</xsl:template>	
	
	<!-- Component_Wrap_Begin -->
	<xsl:template name="Component_Wrap_Begin">
		<xsl:param name="node" />
		<xsl:param name="transformNodeKindKey" />
		<xsl:param name="transformNodeLabelKey" />
		<xsl:param name="transformNodeMiddlewareKey" />
		<xsl:param name="transformNodeAsyncKey" />
		<xsl:param name="transformNodeValueKey" />
		<xsl:param name="transformNodeFrequencyKey" />
		<xsl:param name="transformNodeTypeKey" />
		
		<xsl:variable name="nodeName" select="$node/gml:data[@key=$transformNodeLabelKey]/text()" />
		<xsl:variable name="implName" select="concat($nodeName, $ImplSuffix)" />
		<!-- This part of the code generates the header file. -->
		<xsl:variable name="namespace_name" select="$implName" />
		<xsl:variable name="destructor" select="concat('~', $nodeName)" />
		<!-- Construct the name of the executor. fq_type first param, includes package/module/namespace of component ??? -->
		<xsl:variable name="exec" select="concat('CIAO_', $nodeName, '_Impl::', $nodeName, '_Exec')" />
		<!-- Construct the name of the context. scope includes namespace of component ??? assume top level :: -->
		<xsl:variable name="context" select="concat('::iCCM_', $nodeName, '_Context')" />
		<xsl:variable name="basetype" select="concat('CUTS_CCM_Component_T &lt; ', $exec, ', ', $context, ' &gt;')" />

		<xsl:value-of select="concat('&#xA;namespace ', $namespace_name, '&#xA;{')" />
		<xsl:value-of select="concat('&#xA;//&#xA;// ', $nodeName, '&#xA;//&#xA;')" />
		<xsl:value-of select="concat($nodeName, '::', $nodeName, ' (void)')" />
		<xsl:value-of select="'&#xA;{&#xA;'" />
		<!-- For BlackBox there are no variables or periodic events -->
		<!-- if CIAO we may need to emulate asynchronous function. -->
		<xsl:variable name="middleware" select="/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'Model']/../gml:data[@key=$transformNodeMiddlewareKey]/text()" />
		<xsl:if test="$middleware = 'tao'" >
			<xsl:variable name="sinks" select="$node/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'InEventPort']/.." />
			<xsl:for-each select="$sinks" >
				<xsl:variable name="sink" select="." />
				<xsl:variable name="sinkName" select="$sink/gml:data[@key=$transformNodeLabelKey]/text()" />
				<!-- Determine if this input event is asynchronous so need to emulate asynchronous function. -->
				<xsl:variable name="is_async" select="$sink/gml:data[@key=$transformNodeAsyncKey]/text()" />
				<xsl:if test="$is_async = 'true'" >
					<!-- Register the event handler object. -->
					<xsl:variable name="varname" select="concat($sinkName, '_event_handler_')" />
					<xsl:value-of select="concat('this-&gt;', $varname, '.init (this, &amp;', $nodeName, '::push_', $sinkName, '_i);&#xA;')" />
					<xsl:value-of select="concat('this-&gt;register_object (&amp;this-&gt;', $varname, ');&#xA;')" />
				</xsl:if>
			</xsl:for-each>
		</xsl:if>
				  
		<!-- Finish the constructor. -->
		<xsl:value-of select="'&#xA;}&#xA;'" />
		<!-- Write the destructor's implementation. -->
		<xsl:value-of select="concat('&#xA;//&#xA;// ', $destructor, '&#xA;//&#xA;')" />
		<xsl:value-of select="concat($nodeName, '::', $destructor, ' (void)&#xA;{&#xA;}&#xA;')" />
	</xsl:template>	
	
	<!-- Visit_Wrap_Component -->
	<xsl:template name="Visit_Wrap_Component">
		<xsl:param name="node" />
		<xsl:param name="transformNodeKindKey" />
		<xsl:param name="transformNodeLabelKey" />
		<xsl:param name="transformNodeFolderKey" />
		<xsl:param name="transformNodeWorkerKey" />
		<xsl:param name="transformNodeWorkerIDKey" />
		<xsl:param name="transformNodeOperationKey" />
		<xsl:param name="transformNodeComplexityKey" />
		<xsl:param name="transformNodeComplexityParametersKey" />
		<xsl:param name="transformNodeParametersKey" />
		<xsl:param name="transformNodeMiddlewareKey" />
		<xsl:param name="transformNodeAsyncKey" />
		<xsl:param name="transformNodeValueKey" />
		<xsl:param name="transformNodeCodeKey" />
		<xsl:param name="transformNodeTypeKey" />
		<xsl:param name="transformNodeActionOnKey" />
		<xsl:param name="transformNodeSortOrderKey" />
		
		<xsl:variable name="nodeName" select="$node/gml:data[@key=$transformNodeLabelKey]/text()" />
		<xsl:variable name="middleware" select="/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'Model']/../gml:data[@key=$transformNodeMiddlewareKey]/text()" />
		<xsl:variable name="interfaceDefs" select="/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'InterfaceDefinitions']/.." />
		<xsl:variable name="edges" select="/descendant::*/gml:edge"/>
		
		<!-- Visit all the InEventPort elements of the <component>. -->
		<xsl:variable name="sinks" select="$node/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'InEventPort']/.." />
		<!-- Get the connection properties from the port. "asynchronous" -->
		<!-- InEventPort_Begin We are generating a regular event port. -->
		<xsl:for-each select="$sinks" >
			<xsl:variable name="sink" select="." />
			<xsl:variable name="sinkName" select="$sink/gml:data[@key=$transformNodeLabelKey]/text()" />
			<xsl:variable name="typeNodeId" select="/descendant::*/gml:edge[@source=$sink/@id]/@target" />
			<xsl:variable name="typeNode" select="$interfaceDefs/descendant::*/gml:node[@id=$typeNodeId]/gml:data[@key=$transformNodeKindKey][text() = 'Aggregate']/.." />
			<xsl:variable name="typeName" select="concat($typeNode/gml:data[@key=$transformNodeLabelKey]/text(), $EventSuffix)" />
			<!-- Construct the name with scoped namespace of component ??? assume top level :: -->
			<xsl:variable name="fq_name" select="concat('::',$typeName)" />
			<!-- Make sure this is not a template event port. ie has event type aggregate -->
			<xsl:if test="count($typeNode) &gt; 0" >
			
				<xsl:value-of select="concat('&#xA;//&#xA;// sink: ', $sinkName, '&#xA;//&#xA;')" />
				<xsl:value-of select="concat('void ', $nodeName, '::push_', $sinkName, ' (', $fq_name, ' * ev)&#xA;{&#xA;')" />

				<!-- Generate the appropriate methods. -->
				<!-- Determine if this input event is asynchronous and need to emulate asynchronous function. -->
				<xsl:variable name="is_async" select="$sink/gml:data[@key=$transformNodeAsyncKey]/text()" />
				<xsl:if test="$is_async = 'true' and $middleware = 'tao'" >
					<xsl:value-of select="concat('this-&gt;', $sinkName, '_event_handler_.handle_event (ev);&#xA;}&#xA;')" />
					<xsl:value-of select="concat('&#xA;//&#xA;// sink: ', $sinkName, '&#xA;//&#xA;')" />
					<xsl:value-of select="concat('void ', $nodeName, '::push_', $sinkName, '_i (', $fq_name, ' * ev)&#xA;{&#xA;')" />
				</xsl:if>
  			</xsl:if>
			<!-- BlackBox has no behaviour only wraper for InEventPort -->
			<xsl:value-of select="'ACE_UNUSED_ARG (ev);&#xA;}&#xA;'" />
		</xsl:for-each>

		<!-- BlackBox has no PeriodicEvent elements -->
		
		<!-- BlackBox has no Attribute elements but Component without Impl may ...-->
		<!-- Visit all the Attribute elements of the <component>. -->
		<xsl:variable name="attrs" select="$node/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'Attribute']/.." />
		<xsl:for-each select="$attrs">
			<xsl:variable name="attr" select="." />
			<xsl:variable name="attrName" select="$attr/gml:data[@key=$transformNodeLabelKey]/text()" />
			<!-- Assume that attributes are predefined types, may extend to aggregate types in future ???  -->
			<xsl:variable name="member_type" select="$attr/gml:data[@key=$transformNodeTypeKey]/text()" />
			<!-- Attribute_Begin -->
			<xsl:if test="not($member_type = '')">		
				<xsl:value-of select="concat('&#xA;//&#xA;// attribute setter: ', $attrName, '&#xA;//&#xA;')" />
				<xsl:value-of select="concat('void ', $nodeName, '::', $attrName, ' (')" />
				<xsl:call-template name="MemberType">
					<xsl:with-param name="type" select="$member_type"/>
				</xsl:call-template>
				<xsl:value-of select="concat(' ', $attrName, ')')" />
				<xsl:value-of select="'&#xA;{&#xA;'" />
				<!-- Attribute_End -->
				<!-- Assume not using GenericObject, GenericValue or TypeEncoding ??? see Component_Implementation.cpp -->
				<xsl:value-of select="concat('this-&gt;', $attrName, '_ = ')" />
				<xsl:value-of select="concat($attrName, ';&#xA;}&#xA;')" />
				<!-- Make sure we generate the <readonly> variant. ie getter functions -->
				<!-- ReadonlyAttribute_Begin -->
				<xsl:value-of select="concat('&#xA;//&#xA;// attribute getter: ', $attrName, '&#xA;//&#xA;')" />
				<xsl:call-template name="MemberType">
					<xsl:with-param name="type" select="$member_type"/>
					<xsl:with-param name="retn_type" select="'true'"/>
				</xsl:call-template>
				<xsl:value-of select="concat(' ', $nodeName, '::', $attrName, ' (void)&#xA;{&#xA;')" />
				<!-- ReadonlyAttribute_End -->
				<!-- Assume not using GenericObject, GenericValue or TypeEncoding ??? see Component_Implementation.cpp -->
				<xsl:choose>
				<xsl:when test="$member_type = 'String'">
					<!-- Strings are special case. We need to return a duplicate copy of the string, or we will have major problems. -->
					<xsl:value-of select="concat('::CORBA::String_var s =&#xA;  ::CORBA::string_dup (this-&gt;', $attrName, '_.c_str ());&#xA;')" />
					<xsl:value-of select="'return s._retn ();'" />
				</xsl:when>
				<xsl:otherwise>
					<xsl:value-of select="concat('return this-&gt;', $attrName, '_;')" />
				</xsl:otherwise>
				</xsl:choose>
				<xsl:value-of select="'&#xA;}&#xA;'" />
			</xsl:if>
		</xsl:for-each>

		<!-- BlackBox has no Workload -->
	</xsl:template>	
	
	<!-- insert (double) to delimited list and return list -->
	 <xsl:template name="doubleList">
		<xsl:param name="pText" />
		<xsl:param name="delim" />
		
		<xsl:if test="string-length($pText) > 0">
			<xsl:variable name="pCount" select="string-length($pText) - string-length(translate($pText, $delim, ''))" />
			
			<xsl:value-of select="concat(' (double) ',substring-before(concat($pText, $delim), $delim))"/>

			<xsl:if test="$pCount > 0">	
				<xsl:value-of select="$delim" />
				<xsl:call-template name="doubleList">
					<xsl:with-param name="pText" select="substring-after($pText, $delim)"/>
					<xsl:with-param name="delim" select="$delim" />
				</xsl:call-template> 
			</xsl:if>
		</xsl:if>
	</xsl:template>
	
	<!-- Split delimited list and return required item, idx from 0 to count-1 -->
	 <xsl:template name="splitList">
		<xsl:param name="pText" />
		<xsl:param name="count" />
		<xsl:param name="idx" />
		<xsl:param name="delim" />
		
		<xsl:if test="string-length($pText) > 0">
			<xsl:variable name="pCount" select="string-length($pText) - string-length(translate($pText, $delim, ''))" />
			
			<xsl:choose>
				<xsl:when test="($count - $pCount) = $idx" >
					<xsl:value-of select="substring-before(concat($pText, $delim), $delim)"/>
				</xsl:when>
				<xsl:otherwise>
						
					<xsl:call-template name="splitList">
						<xsl:with-param name="pText" select="substring-after($pText, $delim)"/>
						<xsl:with-param name="count" select="$count" />
						<xsl:with-param name="idx" select="$idx" />
						<xsl:with-param name="delim" select="$delim" />
					</xsl:call-template> 
				</xsl:otherwise>
			</xsl:choose>
		</xsl:if>
	</xsl:template>
	
	<xsl:template name="stripLast">
		<xsl:param name="pText" />
		<xsl:param name="delim" />

		<xsl:if test="contains($pText, $delim)">
			<xsl:value-of select="substring-before($pText, $delim)"/>
			<xsl:if test="contains(substring-after($pText, $delim), $delim)">
				<xsl:value-of select="$delim"/>
			</xsl:if>
			<xsl:call-template name="stripLast">
				<xsl:with-param name="pText" select="substring-after($pText, $delim)"/>
				<xsl:with-param name="delim" select="$delim"/>
			</xsl:call-template>
		</xsl:if>
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