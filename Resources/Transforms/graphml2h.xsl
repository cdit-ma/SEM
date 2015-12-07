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

        This template will convert the .graphml project to the selected h file. 
		This transform can be used for component implementations. The driving program must
		make the selection of which nodes to process for a given deployment. 
		That is we need to process all "<component>Impl.h" transforms for a model/project.
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
				<xsl:with-param name="defaultId" select="$nodeWorkerKey" />
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
								
		<!-- Begin
		<xsl:value-of select="'// This file was generated by:&#xA;'"/>
		<xsl:value-of select="'// graphml2h.xsl&#xA;&#xA;'"/>
		 -->
		<xsl:value-of select="'// -*- C++ -*-&#xA;&#xA;'"/>
		<xsl:value-of select="concat('#ifndef ', '_', $uppercaseFile, '_H_', '&#xA;')"/>
		<xsl:value-of select="concat('#define ', '_', $uppercaseFile, '_H_', '&#xA;', '&#xA;')"/>
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
		<xsl:value-of select="'#if !defined (ACE_LACKS_PRAGMA_ONCE)&#xA;'"/>
		<xsl:value-of select="'#pragma once&#xA;'"/>
		<xsl:value-of select="'#endif /* ACE_LACKS_PRAGMA_ONCE */&#xA;&#xA;'"/>
		<xsl:value-of select="'#include &quot;ace/pre.h&quot;&#xA;&#xA;'"/>
		<xsl:value-of select="concat('#include &quot;', $includename, '_iCCMC.h&quot;&#xA;')"/>
		<xsl:value-of select="'#include &quot;cuts/arch/ccm/CCM_Component_T.h&quot;&#xA;&#xA;'"/>
		
		<!-- Write the includes for the worker files. -->
		<xsl:call-template name="Include_Worker_Files">
			<xsl:with-param name="implNode" select="$implNode" />
			<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey"/>
			<xsl:with-param name="transformNodeFileKey" select="$transformNodeFileKey"/>
			<xsl:with-param name="transformNodeWorkerKey" select="$transformNodeWorkerKey"/>
		</xsl:call-template>

<!-- ??? not sure what this is including, looks like WorkerFiles that we have already done in Prelude   
  // Write the include files for this implementation.
  CUTS_BE::visit <CONTEXT> (impl->include_,
    boost::bind (&CUTS_BE_Impl_Generator_T::Visit_Include,
    boost::ref (this), _1));
  this->ctx_.header_ << CUTS_BE_CPP::include (include);
-->
		<!-- Figure out what type of component we are implementing. Right
			now there is a one-to-one implementation to component type
			mapping. Therefore, the component has the known behavior
			for this respective implementation. -->			
		<xsl:if test="count($implNode) &gt; 0" >
			<xsl:call-template name="Impl_Generator">
				<xsl:with-param name="node" select="$componentNode" />
				<xsl:with-param name="implNode" select="$implNode" />
				<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey"/>
				<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey"/>
				<xsl:with-param name="transformNodeFolderKey" select="$transformNodeFolderKey"/>
				<xsl:with-param name="transformNodeWorkerKey" select="$transformNodeWorkerKey"/>
				<xsl:with-param name="transformNodeWorkerIDKey" select="$transformNodeWorkerIDKey"/>
				<xsl:with-param name="transformNodeMiddlewareKey" select="$transformNodeMiddlewareKey"/>
				<xsl:with-param name="transformNodeAsyncKey" select="$transformNodeAsyncKey"/>
				<xsl:with-param name="transformNodeTypeKey" select="$transformNodeTypeKey"/>
				<xsl:with-param name="transformNodeActionOnKey" select="$transformNodeActionOnKey"/>
			</xsl:call-template>	
		</xsl:if>
		<xsl:if test="count($implNode) = 0" >
			<xsl:call-template name="Wrap_Generator">
				<xsl:with-param name="node" select="$componentNode" />
				<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey"/>
				<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey"/>
				<xsl:with-param name="transformNodeFolderKey" select="$transformNodeFolderKey"/>
				<xsl:with-param name="transformNodeWorkerKey" select="$transformNodeWorkerKey"/>
				<xsl:with-param name="transformNodeMiddlewareKey" select="$transformNodeMiddlewareKey"/>
				<xsl:with-param name="transformNodeAsyncKey" select="$transformNodeAsyncKey"/>
				<xsl:with-param name="transformNodeTypeKey" select="$transformNodeTypeKey"/>
				<xsl:with-param name="transformNodeActionOnKey" select="$transformNodeActionOnKey"/>
			</xsl:call-template>	
		</xsl:if>
		
		<!-- Write the epilogue for the file, then close it. -->
		<!-- Epilogue_Generate -->
		<xsl:value-of select="'#include &quot;ace/post.h&quot;&#xA;&#xA;'"/>
		<xsl:value-of select="concat('#endif  // !defined _', $uppercaseFile, '_H_&#xA;')"/>
		<xsl:value-of select="'&#xA;// end of auto-generated file&#xA;&#xA;'"/>
    </xsl:template>
		
	<!-- Component Implementation h file -->
	<xsl:template name="Impl_Generator">
		<xsl:param name="node" />
		<xsl:param name="implNode" />
		<xsl:param name="transformNodeKindKey" />
		<xsl:param name="transformNodeLabelKey" />
		<xsl:param name="transformNodeFolderKey" />
		<xsl:param name="transformNodeWorkerKey" />
		<xsl:param name="transformNodeWorkerIDKey" />
		<xsl:param name="transformNodeMiddlewareKey" />
		<xsl:param name="transformNodeAsyncKey" />
		<xsl:param name="transformNodeTypeKey" />
		<xsl:param name="transformNodeActionOnKey" />
		
		<!-- Extract the component type being implemented. -->
		<xsl:call-template name="Component_Impl_Begin">
			<xsl:with-param name="node" select="$node" />
			<xsl:with-param name="implNode" select="$implNode" />
			<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey"/>
			<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey"/>
			<xsl:with-param name="transformNodeFolderKey" select="$transformNodeFolderKey"/>
			<xsl:with-param name="transformNodeWorkerKey" select="$transformNodeWorkerKey"/>
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
			<xsl:with-param name="transformNodeMiddlewareKey" select="$transformNodeMiddlewareKey"/>
			<xsl:with-param name="transformNodeAsyncKey" select="$transformNodeAsyncKey"/>
			<xsl:with-param name="transformNodeTypeKey" select="$transformNodeTypeKey"/>
			<xsl:with-param name="transformNodeActionOnKey" select="$transformNodeActionOnKey"/>
		</xsl:call-template>	
		
		<!-- Write the end of the component's implementation. -->
		<xsl:value-of select="'};&#xA;}&#xA;&#xA;'" />

		<!-- generate the implementation entrypoint for this component's implementation. -->
		<xsl:variable name="nodeName" select="$node/gml:data[@key=$transformNodeLabelKey]/text()" />
		<xsl:variable name="entrypoint" select="concat('create_', $nodeName, '_', $ImplSuffix)" />
		<xsl:variable name="export_basename" select="concat($nodeName, $implSuffix)" />
		<xsl:variable name="export_macro" select="translate($export_basename,
								'abcdefghijklmnopqrstuvwxyz',
								'ABCDEFGHIJKLMNOPQRSTUVWXYZ')" />
		<xsl:value-of select="concat('#include &quot;', $export_basename, '_export.h&quot;&#xA;&#xA;')"/>

		<xsl:value-of select="concat('//&#xA;//', $entrypoint, '&#xA;//&#xA;')" />
		<xsl:value-of select="concat('extern &quot;C&quot; ', $export_macro, '_Export&#xA;')"/>
		<xsl:value-of select="concat('::Components::EnterpriseComponent_ptr ', $entrypoint, ' (void);&#xA;&#xA;')"/>
	</xsl:template>	

	<!-- _generate -->
	<xsl:template name="Component_Impl_Begin">
		<xsl:param name="node" />
		<xsl:param name="implNode" />
		<xsl:param name="transformNodeKindKey" />
		<xsl:param name="transformNodeLabelKey" />
		<xsl:param name="transformNodeFolderKey" />
		<xsl:param name="transformNodeWorkerKey" />
		
		<xsl:variable name="nodeName" select="$node/gml:data[@key=$transformNodeLabelKey]/text()" />
		<xsl:variable name="implName" select="concat($nodeName, $ImplSuffix)" />
		<!-- This part of the code generates the header file. -->
		<xsl:variable name="namespace_name" select="$implName" />
		
		<xsl:variable name="periodics" select="$implNode/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'PeriodicEvent']/.." />
		<xsl:if test="count($periodics) &gt; 0">
			<xsl:value-of select="'#include &quot;cuts/Periodic_Event_T.h&quot;&#xA;'"/>
		</xsl:if>
	
		<xsl:value-of select="concat('&#xA;namespace ', $namespace_name,'&#xA;{')"/>

		<xsl:variable name="destructor" select="concat('~', $nodeName)" />
		<!-- Construct the name of the executor. fq_type first param, includes package/module/namespace of component ??? -->
		<xsl:variable name="exec" select="concat('CIAO_', $nodeName, '_Impl::', $nodeName, '_Exec')" />
		<!-- Construct the name of the context. scope includes namespace of component ??? assume top level :: -->
		<xsl:variable name="context" select="concat('::iCCM_', $nodeName, '_Context')" />
		<xsl:variable name="basetype" select="concat('CUTS_CCM_Component_T &lt; ', $exec, ', ', $context, ' &gt;')" />

		<xsl:value-of select="'&#xA;// Type definition of the implementation base type&#xA;'"/>
		<xsl:value-of select="concat('typedef ', $basetype, ' ', $nodeName, '_Base;&#xA;&#xA;')"/>
		<xsl:value-of select="'/**&#xA;'"/>
		<xsl:value-of select="concat(' * @class ', $nodeName, '&#xA;')"/>
		<xsl:value-of select="' *&#xA;'"/>
		<xsl:value-of select="concat(' * Implementation of the ', $exec, ' component executor&#xA;')"/>
		<xsl:value-of select="' */&#xA;'"/>
		<xsl:value-of select="concat('class ', $nodeName, ' :&#xA;')"/>
		<xsl:value-of select="concat('  public ', $nodeName, '_Base')" />
		<xsl:value-of select="'&#xA;{&#xA;'" />
		<xsl:value-of select="'public:'"/>
		<xsl:value-of select="'&#xA;// Type definition of the base component type&#xA;'"/>
		<xsl:value-of select="concat('typedef ', $nodeName, '_Base base_type;&#xA;')"/>
		<!-- Write the default constructor. -->
		<xsl:value-of select="'&#xA;// Default constructor&#xA;'"/>
		<xsl:value-of select="concat($nodeName, ' (void);&#xA;')"/>
		<!-- Write the destructor. -->
		<xsl:value-of select="'&#xA;// Destructor&#xA;'"/>
		<xsl:value-of select="concat('virtual ', $destructor, ' (void);&#xA;')"/>
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
		<xsl:param name="transformNodeMiddlewareKey" />
		<xsl:param name="transformNodeAsyncKey" />
		<xsl:param name="transformNodeTypeKey" />
		<xsl:param name="transformNodeActionOnKey" />
		
		<xsl:variable name="nodeName" select="$node/gml:data[@key=$transformNodeLabelKey]/text()" />
		<xsl:variable name="middleware" select="/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'Model']/../gml:data[@key=$transformNodeMiddlewareKey]/text()" />
		<xsl:variable name="interfaceDefs" select="/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'InterfaceDefinitions']/.." />
		
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
				<!-- Generate the appropriate methods. -->
				<xsl:value-of select="concat('&#xA;// sink: ', $sinkName, '&#xA;')"/>
				<xsl:value-of select="concat('virtual void push_', $sinkName, ' (', $fq_name, ' * ev);&#xA;')" />
				<!-- Determine if this input event is asynchronous and need to emulate asynchronous function. -->
				<xsl:variable name="is_async" select="$sink/gml:data[@key=$transformNodeAsyncKey]/text()" />
				<xsl:if test="$is_async = 'true' and $middleware = 'tao'" >
					<xsl:value-of select="'private:&#xA;'"/>
					<xsl:value-of select="concat('&#xA;// async event handler variable: ', $sinkName, '&#xA;')"/>
					<xsl:value-of select="concat('CUTS_CCM_Event_Handler_T &lt; ', $nodeName, ', ', $fq_name, ' &gt; ', $sinkName, '_event_handler_;&#xA;')"/>
					<xsl:value-of select="'public:&#xA;'"/>
					<xsl:value-of select="concat('&#xA;// sink impl: ', $sinkName, '&#xA;')" />
					<xsl:value-of select="concat('void push_', $sinkName, '_i (', $fq_name, ' * ev);&#xA;')"/>
				</xsl:if>
  			</xsl:if>
		</xsl:for-each>

		<!-- Visit all the PeriodicEvent elements of the <component>. -->
		<xsl:variable name="periodics" select="$implNode/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'PeriodicEvent']/.." />
		
		<xsl:for-each select="$periodics">
			<xsl:variable name="periodic" select="." />
			<!-- PeriodicEvent_Begin -->
			<xsl:variable name="periodicName" select="$periodic/gml:data[@key=$transformNodeLabelKey]/text()" />
			<xsl:variable name="func_name" select="concat('periodic_', $periodicName)" />
			<xsl:value-of select="concat('&#xA;// PeriodicEvent: ', $periodicName, '&#xA;')" />
			<xsl:value-of select="concat('void ', $func_name, ' (void);&#xA;')"/>
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
				<xsl:value-of select="concat('&#xA;// attribute setter: ', $attrName, '&#xA;')" />
				<xsl:value-of select="concat('virtual void ', $attrName, ' (' )" />
				<xsl:call-template name="MemberType">
					<xsl:with-param name="type" select="$member_type"/>
				</xsl:call-template>
				<xsl:value-of select="concat(' ', $attrName, ');&#xA;')" />

			<!-- Attribute_End -->
			<!-- Assume not using GenericObject, GenericValue or TypeEncoding ??? see Component_Implementation.cpp -->

			<!-- Make sure we generate the <readonly> variant. ie getter functions -->
			<!-- ReadonlyAttribute_Begin -->

				<xsl:value-of select="concat('&#xA;// attribute getter: ', $attrName, '&#xA;')" />
				<xsl:value-of select="'virtual '" />
				<xsl:call-template name="MemberType">
					<xsl:with-param name="type" select="$member_type"/>
					<xsl:with-param name="retn_type" select="'true'"/>
				</xsl:call-template>
				<xsl:value-of select="concat(' ', $attrName, ' (void);&#xA;')" />

			<!-- ReadonlyAttribute_End -->
			<!-- Assume not using GenericObject, GenericValue or TypeEncoding ??? see Component_Implementation.cpp -->

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
			    <xsl:value-of select="'&#xA;virtual void ccm_activate (void);&#xA;'" />
			</xsl:if>
			<xsl:if test="$processNodes/gml:data[@key=$transformNodeActionOnKey]/text() = 'Configuration_complete'">
			    <xsl:value-of select="'&#xA;virtual void configuration_complete (void);&#xA;'" />
			</xsl:if>
			
			<xsl:if test="$processNodes/gml:data[@key=$transformNodeActionOnKey]/text() = 'Passivate'">
				<xsl:value-of select="'&#xA;virtual void ccm_passivate (void);&#xA;'" />
			</xsl:if>

			<xsl:if test="$processNodes/gml:data[@key=$transformNodeActionOnKey]/text() = 'Remove'">
				<xsl:value-of select="'&#xA;virtual void ccm_remove (void);&#xA;'" />
			</xsl:if>

			<!-- Visit_MultiInputAction -->
<!--
			<xsl:for-each select="$processNodes">
				<xsl:variable name="processNode" select="." />
				<xsl:variable name="processName" select="$processNode
			CUTS_BE_Execution_Visitor_T <behavior_type> exec_visitor (this->context_);
			PICML::MultiInputAction (action).Accept (exec_visitor);
-->
			<!-- Environment_Method_End -->
		</xsl:if>
		<!-- Determine if we are writing the variables -->
		<xsl:value-of select="'&#xA;private:'" />
	
		<!-- Write all the basic variables. -->
		<xsl:variable name="vars" select="$implNode/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'Variable']/.." />
		<xsl:variable name="basicVars" select="$vars[not(./gml:graph)]" />
		<xsl:for-each select="$basicVars">
			<xsl:variable name="var" select="." />
			<xsl:variable name="varName" select="$var/gml:data[@key=$transformNodeLabelKey]/text()" />
			<xsl:variable name="varType" select="$var/gml:data[@key=$transformNodeTypeKey]/text()" />
			<xsl:if test="not($varType = '')">
				<xsl:value-of select="concat('&#xA;// variable: ', $varName, '&#xA;')" />
				<xsl:call-template name="VariableType">
					<xsl:with-param name="type" select="$varType"/>
				</xsl:call-template>
				<xsl:value-of select="concat(' ', $varName, '_;&#xA;')" />
			</xsl:if>
		</xsl:for-each>
		
		<!-- Write all the complex variables. -->
		<xsl:variable name="aggregateVars" select="$vars/gml:graph/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'AggregateInstance' or text() = 'VectorInstance']/../../.." />
		<xsl:for-each select="$aggregateVars">
			<xsl:variable name="agVar" select="." />
			<xsl:variable name="agVarName" select="$agVar/gml:data[@key=$transformNodeLabelKey]/text()" />
			<xsl:variable name="agVarType" select="$agVar/gml:graph/gml:node/gml:data[@key=$transformNodeTypeKey]/text()" />
			<xsl:if test="not($agVarType = '')">
				<xsl:value-of select="concat('&#xA;// variable: ', $agVarName, '&#xA;')" />
				<xsl:value-of select="concat('::', $agVarType, '_var ')" />
				<xsl:if test="$agVar/gml:graph/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'VectorInstance']" >
					<xsl:value-of select="'*'" />
				</xsl:if>
				<xsl:value-of select="concat(' ', $agVarName, '_;&#xA;')" />
			</xsl:if>
		</xsl:for-each>
							  
		<!-- Write all the worker related variables. -->
		<!-- Specialised Workers to be implemented later, ie callback workers ??? -->
		<xsl:call-template name="Write_Worker_Variables">
			<xsl:with-param name="implNode" select="$implNode" />
			<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey"/>
			<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey"/>
			<xsl:with-param name="transformNodeWorkerKey" select="$transformNodeWorkerKey"/>
			<xsl:with-param name="transformNodeWorkerIDKey" select="$transformNodeWorkerIDKey"/>
		</xsl:call-template>

		<!-- Write the attribute variables. -->
		<xsl:variable name="attrVars" select="$node/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'Attribute']/.." />
		<xsl:for-each select="$attrVars">
			<xsl:variable name="attrVar" select="." />
			<xsl:variable name="attrVarName" select="$attrVar/gml:data[@key=$transformNodeLabelKey]/text()" />
			<xsl:variable name="attrVarType" select="$attrVar/gml:data[@key=$transformNodeTypeKey]/text()" />
			<xsl:if test="not($attrVarType = '')">
				<xsl:value-of select="concat('&#xA;// attribute: ', $attrVarName, '&#xA;')" />
				<xsl:call-template name="VariableType">
					<xsl:with-param name="type" select="$attrVarType"/>
				</xsl:call-template>
				<xsl:value-of select="concat(' ', $attrVarName, '_;&#xA;')" />
			</xsl:if>
		</xsl:for-each>					  

		<!-- Write the periodic event variables. -->
		<xsl:for-each select="$periodics">
			<xsl:variable name="periodic" select="." />
			<!-- PeriodicEvent_Begin -->
			<xsl:variable name="periodicName" select="$periodic/gml:data[@key=$transformNodeLabelKey]/text()" />
			<xsl:value-of select="concat('&#xA;// periodic: ', $periodicName, '&#xA;')" />
			<xsl:value-of select="concat('CUTS_Periodic_Event_T &lt; ', $nodeName, ' &gt; periodic_', $periodicName, '_;&#xA;')" />
		</xsl:for-each>
	</xsl:template>	

	<!-- generate_listing for given suffix -->
	<xsl:template name="generate_listing">
		<xsl:param name="node" />
		<xsl:param name="suffix" />
		<xsl:param name="transformNodeLabelKey" />
		<xsl:param name="transformNodeKindKey" />
		
		<!-- listing always generated for stubSuffix, otherwise must have componentNodes to be included in listing -->
		<!-- If this IDL file has components -->
		<xsl:variable name="componentNodes" select="$node/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'Component']/.." />
		<xsl:if test="($suffix = $stubSuffix) or (count($componentNodes) &gt; 0)" >
			<xsl:value-of select="' \&#xA;'"/>
			<xsl:value-of select="concat('    ', $node/gml:data[@key=$transformNodeLabelKey]/text(), $suffix)" />
		</xsl:if>
    </xsl:template>	
	
	<!-- generate_worker_includes -->
	<xsl:template name="Include_Worker_Files">
		<xsl:param name="implNode" />
		<xsl:param name="transformNodeKindKey" />
		<xsl:param name="transformNodeFileKey" />
		<xsl:param name="transformNodeWorkerKey" />
	
		<xsl:variable name="workerIncludes" select="$implNode/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'Process']/../gml:data[@key=$transformNodeWorkerKey][text() != '']/.." />
		
		<!-- find all workers and add include paths -->
		<xsl:if test="count($workerIncludes) &gt; 0" >

			<!-- sort the list of items to output -->
			<xsl:variable name="delim" select="','" />
			<xsl:variable name="sortedNodeList">
				<xsl:for-each select="$workerIncludes"> 
					<xsl:sort select="./gml:data[@key=$transformNodeFileKey]/text()" data-type="text" />
					<xsl:value-of select="concat(./gml:data[@key=$transformNodeFileKey]/text(), $delim)"/>
				</xsl:for-each>
				<!-- Add Utility worker for any components that use a WE_ worker -->
				<xsl:for-each select="$workerIncludes"> 
					<xsl:if test="'WE' = substring-before( concat(./gml:data[@key=$transformNodeFileKey]/text(), '_'), '_') "> 
						<xsl:value-of select="concat('WE_UTE', $delim)"/>
					</xsl:if> 
				</xsl:for-each>
			</xsl:variable>

			<!-- only output unique values from the sorted list -->
			<xsl:call-template name="unique_worker_values" >
				<xsl:with-param name="compare" select="''" />
				<xsl:with-param name="pText" select="$sortedNodeList" />
				<xsl:with-param name="delim" select="$delim" />
			</xsl:call-template>	
		</xsl:if>
	</xsl:template>	
	
	<!-- Output unique items from delimited list -->
	 <xsl:template name="unique_worker_values">
		<xsl:param name="compare" />
		<xsl:param name="pText" />
		<xsl:param name="delim" />
		
		<xsl:if test="string-length($pText) > 0">
			<xsl:variable name="pCount" select="string-length($pText) - string-length(translate($pText, $delim, ''))" />

			<xsl:variable name="thisNode" select="substring-before(concat($pText, $delim), $delim)"/>
		
			<!-- Special processing for callback type of workers, see Sensor_Worker ??? -->

		    <xsl:if test="not( $thisNode = $compare )">
				<xsl:value-of select="concat('#include &quot;', $thisNode, '.h&quot;&#xA;')"/>
            </xsl:if>
			
			<xsl:call-template name="unique_worker_values">
				<xsl:with-param name="compare" select="$thisNode"/>
				<xsl:with-param name="pText" select="substring-after($pText, $delim)"/>
				<xsl:with-param name="delim" select="$delim" />
			</xsl:call-template> 
		</xsl:if>
	</xsl:template>

	<!-- generate_worker_variables -->
	<xsl:template name="Write_Worker_Variables">
		<xsl:param name="implNode" />
		<xsl:param name="transformNodeKindKey" />
		<xsl:param name="transformNodeLabelKey" />
		<xsl:param name="transformNodeWorkerKey" />
		<xsl:param name="transformNodeWorkerIDKey" />
	
		<xsl:variable name="workerVariables" select="$implNode/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'Process']/../gml:data[@key=$transformNodeWorkerKey][text() != '']/.." />
		
		<!-- find all workers and add include paths -->
		<xsl:if test="count($workerVariables) &gt; 0" >
			<xsl:variable name="utilityVar" select="$workerVariables/gml:data[@key=$transformNodeWorkerIDKey][text() = 'we_ute']/.
													| $workerVariables/gml:data[@key=$transformNodeLabelKey][text() = 'we_ute']/." />
			
			<!-- sort the list of items to output -->
			<xsl:variable name="delim" select="','" />
			<xsl:variable name="subDelim" select="';'" />
			<xsl:variable name="sortedVarTypeList">
				<xsl:for-each select="$workerVariables"> 
					<xsl:sort select="./gml:data[@key=$transformNodeWorkerKey]/text()" data-type="text" />
					<xsl:sort select="./gml:data[@key=$transformNodeWorkerIDKey]/text()" data-type="text" />
					<xsl:choose>
					<xsl:when test="./gml:data[@key=$transformNodeWorkerIDKey]/text() and not(./gml:data[@key=$transformNodeWorkerIDKey]/text() ='')">
						<xsl:value-of select="concat(./gml:data[@key=$transformNodeWorkerKey]/text(), $subDelim, ./gml:data[@key=$transformNodeWorkerIDKey]/text(), $delim)"/>
					</xsl:when>
					<xsl:otherwise>
						<xsl:value-of select="concat(./gml:data[@key=$transformNodeWorkerKey]/text(), $subDelim, ./gml:data[@key=$transformNodeLabelKey]/text(), $delim)"/>
					</xsl:otherwise>
					</xsl:choose>
				</xsl:for-each>
				<!-- Add Utility worker for any components that use a WE_ worker, if not already included -->
				<xsl:for-each select="$workerVariables"> 
					<xsl:if test="( 'WE' = substring-before( concat(./gml:data[@key=$transformNodeWorkerKey]/text(), '_'), '_') 
								and count($utilityVar) = 0 )" > 
						<xsl:value-of select="concat('WE_UTE', $subDelim, 'we_ute', $delim)"/>
					</xsl:if> 
				</xsl:for-each>
			</xsl:variable>

			<!-- only output unique values from the sorted list -->
			<xsl:call-template name="unique_worker_variables" >
				<xsl:with-param name="compare" select="''" />
				<xsl:with-param name="pText" select="$sortedVarTypeList" />
				<xsl:with-param name="delim" select="$delim" />
				<xsl:with-param name="subDelim" select="$subDelim" />
			</xsl:call-template>	
		</xsl:if>
	</xsl:template>	
	
	<!-- Output unique items from delimited list -->
	<xsl:template name="unique_worker_variables">
		<xsl:param name="compare" />
		<xsl:param name="pText" />
		<xsl:param name="delim" />
		<xsl:param name="subDelim" />
		
		<xsl:if test="string-length($pText) > 0">
			<xsl:variable name="pCount" select="string-length($pText) - string-length(translate($pText, $delim, ''))" />

			<xsl:variable name="thisNode" select="substring-before(concat($pText, $delim), $delim)"/>
			<xsl:variable name="processWorker" select="substring-before($thisNode, $subDelim)" />
			<xsl:variable name="processName" select="substring-after($thisNode, $subDelim)" />
		    <xsl:if test="not( concat($processWorker,$processName) = $compare )">
				<!-- Special processing for specific workers, eg Sensor_Worker requires callback ??? -->
				<xsl:choose>
				<!-- VectorOperations are not defined as variables, will be inline code generated process in cpp -->
				<xsl:when test="$processWorker = 'VectorOperation'" />
				<xsl:otherwise>
					<xsl:value-of select="concat('&#xA;// worker variable: ', $processName, '&#xA;')" />
					<xsl:value-of select="concat($processWorker, ' ', $processName, '_;&#xA;')" />
				</xsl:otherwise>
				</xsl:choose>
            </xsl:if>
			
			<xsl:call-template name="unique_worker_variables">
				<xsl:with-param name="compare" select="concat($processWorker,$processName)"/>
				<xsl:with-param name="pText" select="substring-after($pText, $delim)"/>
				<xsl:with-param name="delim" select="$delim" />
				<xsl:with-param name="subDelim" select="$subDelim" />
			</xsl:call-template> 
		</xsl:if>
	</xsl:template>

	
	<!-- Traits svnt_base_project -->
	<xsl:template name="svnt_base_project">
		<xsl:param name="middleware" />	
		
		<xsl:choose>
		<xsl:when test="$middleware = 'tao'" >
			<xsl:value-of select="'iccm_tao_svnt'" />
		</xsl:when>
		<xsl:when test="$middleware = 'opensplice'" >
			<xsl:value-of select="'iccm_opensplice_svnt'" />
		</xsl:when>
		<xsl:when test="$middleware = 'rtidds'" >
			<xsl:value-of select="'iccm_rtidds_svnt'" />
		</xsl:when>
		<xsl:when test="$middleware = 'tcpip'" >
			<xsl:value-of select="'iccm_tcpip_svnt'" />
		</xsl:when>
		<xsl:otherwise>
			<xsl:value-of select="''" />
		</xsl:otherwise>
		</xsl:choose>
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
				<xsl:value-of select="concat('/* unknown type (', $type, ') */')" />
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>

	<!-- Attribute or Variable Type -->	
	<xsl:template name="VariableType">	
		<xsl:param name="type" />
		
		<xsl:choose>
			<xsl:when test="$type = 'Byte'" >
				<xsl:value-of select="'::CORBA::Octet'" />
			</xsl:when>
			<xsl:when test="$type = 'Char'" >
				<xsl:value-of select="'::CORBA::Char'" />
			</xsl:when>
			<xsl:when test="$type = 'WideChar'" >
				<xsl:value-of select="'::CORBA::WChar'" />
			</xsl:when>	
			<xsl:when test="$type = 'Boolean'" >
				<xsl:value-of select="'::CORBA::Boolean'" />
			</xsl:when>
			<xsl:when test="$type = 'String'" >
				<xsl:value-of select="'ACE_CString'" />
			</xsl:when>
			<xsl:when test="$type = 'WideString'" >
				<xsl:value-of select="'ACE_WString'" />
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
				<xsl:value-of select="concat('/* unknown type (', $type, ') */')" />
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>

	<!-- BlackBox Wrapper h file -->
	<xsl:template name="Wrap_Generator">
		<xsl:param name="node" />
		<xsl:param name="transformNodeKindKey" />
		<xsl:param name="transformNodeLabelKey" />
		<xsl:param name="transformNodeFolderKey" />
		<xsl:param name="transformNodeWorkerKey" />
		<xsl:param name="transformNodeMiddlewareKey" />
		<xsl:param name="transformNodeAsyncKey" />
		<xsl:param name="transformNodeTypeKey" />
		<xsl:param name="transformNodeActionOnKey" />
		
		<!-- Extract the component type being implemented. -->
		<xsl:call-template name="Component_Wrap_Begin">
			<xsl:with-param name="node" select="$node" />
			<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey"/>
			<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey"/>
			<xsl:with-param name="transformNodeFolderKey" select="$transformNodeFolderKey"/>
			<xsl:with-param name="transformNodeWorkerKey" select="$transformNodeWorkerKey"/>
		</xsl:call-template>	
		
		<!-- Visit the component. -->
		<xsl:call-template name="Visit_Wrap_Component">
			<xsl:with-param name="node" select="$node" />
			<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey"/>
			<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey"/>
			<xsl:with-param name="transformNodeFolderKey" select="$transformNodeFolderKey"/>
			<xsl:with-param name="transformNodeWorkerKey" select="$transformNodeWorkerKey"/>
			<xsl:with-param name="transformNodeMiddlewareKey" select="$transformNodeMiddlewareKey"/>
			<xsl:with-param name="transformNodeAsyncKey" select="$transformNodeAsyncKey"/>
			<xsl:with-param name="transformNodeTypeKey" select="$transformNodeTypeKey"/>
			<xsl:with-param name="transformNodeActionOnKey" select="$transformNodeActionOnKey"/>
		</xsl:call-template>	
		
		<!-- Write the end of the component's implementation. -->
		<xsl:value-of select="'};&#xA;}&#xA;&#xA;'" />

		<!-- generate the implementation entrypoint for this component's implementation. -->
		<xsl:variable name="nodeName" select="$node/gml:data[@key=$transformNodeLabelKey]/text()" />
		<xsl:variable name="entrypoint" select="concat('create_', $nodeName, '_', $ImplSuffix)" />
		<xsl:variable name="export_basename" select="concat($nodeName, $implSuffix)" />
		<xsl:variable name="export_macro" select="translate($export_basename,
								'abcdefghijklmnopqrstuvwxyz',
								'ABCDEFGHIJKLMNOPQRSTUVWXYZ')" />
		<xsl:value-of select="concat('#include &quot;', $export_basename, '_export.h&quot;&#xA;&#xA;')"/>

		<xsl:value-of select="concat('//&#xA;//', $entrypoint, '&#xA;//&#xA;')" />
		<xsl:value-of select="concat('extern &quot;C&quot; ', $export_macro, '_Export&#xA;')"/>
		<xsl:value-of select="concat('::Components::EnterpriseComponent_ptr ', $entrypoint, ' (void);&#xA;&#xA;')"/>
	</xsl:template>	

	<!-- _generate -->
	<xsl:template name="Component_Wrap_Begin">
		<xsl:param name="node" />
		<xsl:param name="transformNodeKindKey" />
		<xsl:param name="transformNodeLabelKey" />
		<xsl:param name="transformNodeFolderKey" />
		<xsl:param name="transformNodeWorkerKey" />
		
		<xsl:variable name="nodeName" select="$node/gml:data[@key=$transformNodeLabelKey]/text()" />
		<xsl:variable name="implName" select="concat($nodeName, $ImplSuffix)" />
		<!-- This part of the code generates the header file. -->
		<xsl:variable name="namespace_name" select="$implName" />
		
		<xsl:value-of select="concat('&#xA;namespace ', $namespace_name,'&#xA;{')"/>

		<xsl:variable name="destructor" select="concat('~', $nodeName)" />
		<!-- Construct the name of the executor. fq_type first param, includes package/module/namespace of component ??? -->
		<xsl:variable name="exec" select="concat('CIAO_', $nodeName, '_Impl::', $nodeName, '_Exec')" />
		<!-- Construct the name of the context. scope includes namespace of component ??? assume top level :: -->
		<xsl:variable name="context" select="concat('::iCCM_', $nodeName, '_Context')" />
		<xsl:variable name="basetype" select="concat('CUTS_CCM_Component_T &lt; ', $exec, ', ', $context, ' &gt;')" />

		<xsl:value-of select="'&#xA;// Type definition of the implementation base type&#xA;'"/>
		<xsl:value-of select="concat('typedef ', $basetype, ' ', $nodeName, '_Base;&#xA;&#xA;')"/>
		<xsl:value-of select="'/**&#xA;'"/>
		<xsl:value-of select="concat(' * @class ', $nodeName, '&#xA;')"/>
		<xsl:value-of select="' *&#xA;'"/>
		<xsl:value-of select="concat(' * Implementation of the ', $exec, ' component executor&#xA;')"/>
		<xsl:value-of select="' */&#xA;'"/>
		<xsl:value-of select="concat('class ', $nodeName, ' :&#xA;')"/>
		<xsl:value-of select="concat('  public ', $nodeName, '_Base')" />
		<xsl:value-of select="'&#xA;{&#xA;'" />
		<xsl:value-of select="'public:'"/>
		<xsl:value-of select="'&#xA;// Type definition of the base component type&#xA;'"/>
		<xsl:value-of select="concat('typedef ', $nodeName, '_Base base_type;&#xA;')"/>
		<!-- Write the default constructor. -->
		<xsl:value-of select="'&#xA;// Default constructor&#xA;'"/>
		<xsl:value-of select="concat($nodeName, ' (void);&#xA;')"/>
		<!-- Write the destructor. -->
		<xsl:value-of select="'&#xA;// Destructor&#xA;'"/>
		<xsl:value-of select="concat('virtual ', $destructor, ' (void);&#xA;')"/>
	</xsl:template>	

	<!-- Visit_Wrap_Component -->
	<xsl:template name="Visit_Wrap_Component">
		<xsl:param name="node" />
		<xsl:param name="transformNodeKindKey" />
		<xsl:param name="transformNodeLabelKey" />
		<xsl:param name="transformNodeFolderKey" />
		<xsl:param name="transformNodeWorkerKey" />
		<xsl:param name="transformNodeMiddlewareKey" />
		<xsl:param name="transformNodeAsyncKey" />
		<xsl:param name="transformNodeTypeKey" />
		<xsl:param name="transformNodeActionOnKey" />
		
		<xsl:variable name="nodeName" select="$node/gml:data[@key=$transformNodeLabelKey]/text()" />
		<xsl:variable name="middleware" select="/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'Model']/../gml:data[@key=$transformNodeMiddlewareKey]/text()" />
		<xsl:variable name="interfaceDefs" select="/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'InterfaceDefinitions']/.." />
		
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
				<!-- Generate the appropriate methods. -->
				<xsl:value-of select="concat('&#xA;// sink: ', $sinkName, '&#xA;')"/>
				<xsl:value-of select="concat('virtual void push_', $sinkName, ' (', $fq_name, ' * ev);&#xA;')" />
				<!-- Determine if this input event is asynchronous and need to emulate asynchronous function. -->
				<xsl:variable name="is_async" select="$sink/gml:data[@key=$transformNodeAsyncKey]/text()" />
				<xsl:if test="$is_async = 'true' and $middleware = 'tao'" >
					<xsl:value-of select="'private:&#xA;'"/>
					<xsl:value-of select="concat('&#xA;// async event handler variable: ', $sinkName, '&#xA;')"/>
					<xsl:value-of select="concat('CUTS_CCM_Event_Handler_T &lt; ', $nodeName, ', ', $fq_name, ' &gt; ', $sinkName, '_event_handler_;&#xA;')"/>
					<xsl:value-of select="'public:&#xA;'"/>
					<xsl:value-of select="concat('&#xA;// sink impl: ', $sinkName, '&#xA;')" />
					<xsl:value-of select="concat('void push_', $sinkName, '_i (', $fq_name, ' * ev);&#xA;')"/>
				</xsl:if>
  			</xsl:if>
		</xsl:for-each>

		<!-- BlackBox has no PeriodicEvent elements -->
		
		<!-- BlackBox has no Attribute elements but Component with no Impl may -->
		<!-- Visit all the Attribute elements of the <component>. -->
		<xsl:variable name="attrs" select="$node/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'Attribute']/.." />
		<xsl:for-each select="$attrs">
			<xsl:variable name="attr" select="." />
			<xsl:variable name="attrName" select="$attr/gml:data[@key=$transformNodeLabelKey]/text()" />
			<!-- Assume that attributes are predefined types, may extend to aggregate types in future ???  -->
			<xsl:variable name="member_type" select="$attr/gml:data[@key=$transformNodeTypeKey]/text()" />
			<!-- Attribute_Begin -->
			<xsl:if test="not($member_type = '')">		
				<xsl:value-of select="concat('&#xA;// attribute setter: ', $attrName, '&#xA;')" />
				<xsl:value-of select="concat('virtual void ', $attrName, ' (' )" />
				<xsl:call-template name="MemberType">
					<xsl:with-param name="type" select="$member_type"/>
				</xsl:call-template>
				<xsl:value-of select="concat(' ', $attrName, ');&#xA;')" />

			<!-- Attribute_End -->
			<!-- Assume not using GenericObject, GenericValue or TypeEncoding ??? see Component_Implementation.cpp -->

			<!-- Make sure we generate the <readonly> variant. ie getter functions -->
			<!-- ReadonlyAttribute_Begin -->

				<xsl:value-of select="concat('&#xA;// attribute getter: ', $attrName, '&#xA;')" />
				<xsl:value-of select="'virtual '" />
				<xsl:call-template name="MemberType">
					<xsl:with-param name="type" select="$member_type"/>
					<xsl:with-param name="retn_type" select="'true'"/>
				</xsl:call-template>
				<xsl:value-of select="concat(' ', $attrName, ' (void);&#xA;')" />

			<!-- ReadonlyAttribute_End -->
			<!-- Assume not using GenericObject, GenericValue or TypeEncoding ??? see Component_Implementation.cpp -->

			</xsl:if>
		</xsl:for-each>

		<!-- BlackBox has no Workload environment -->
		<!-- BlackBox has no Variables -->
		
		<!-- Write the attribute variables. -->
		<xsl:variable name="attrVars" select="$node/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'Attribute']/.." />
		<xsl:for-each select="$attrVars">
			<xsl:variable name="attrVar" select="." />
			<xsl:variable name="attrVarName" select="$attrVar/gml:data[@key=$transformNodeLabelKey]/text()" />
			<xsl:variable name="attrVarType" select="$attrVar/gml:data[@key=$transformNodeTypeKey]/text()" />
			<xsl:if test="not($attrVarType = '')">
				<xsl:value-of select="concat('&#xA;// attribute: ', $attrVarName, '&#xA;')" />
				<xsl:call-template name="VariableType">
					<xsl:with-param name="type" select="$attrVarType"/>
				</xsl:call-template>
				<xsl:value-of select="concat(' ', $attrVarName, '_;&#xA;')" />
			</xsl:if>
		</xsl:for-each>	
						  
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