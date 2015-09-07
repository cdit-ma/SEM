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

        This template will convert the .graphml project to the selected mpc file. 
		This transform can be used for both component implementations and aggregate
		data contained in the interface definitions. The driving program must
		make the selection of which nodes to process for a given deployment. 
		That is we need to process all "<component>Impl.mpc" and "<idlfile>.mpc" 
		tranforms for a model/project.
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
	<xsl:variable name="idlSuffix" select="'_IDL_Gen'" />

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

		<xsl:variable name="transformNodeWorkerKey">
			<xsl:call-template name="findNodeKey">
				<xsl:with-param name="attrName" select="'worker'" />
				<xsl:with-param name="defaultId" select="$nodeWorkerKey" />
			</xsl:call-template>	
		</xsl:variable>	
		
		<xsl:variable name="transformNodeOperationKey">
			<xsl:call-template name="findNodeKey">
				<xsl:with-param name="attrName" select="'operation'" />
				<xsl:with-param name="defaultId" select="$nodeWorkerKey" />
			</xsl:call-template>	
		</xsl:variable>	
		
		<xsl:variable name="transformNodeMiddlewareKey">
			<xsl:call-template name="findNodeKey">
				<xsl:with-param name="attrName" select="'middleware'" />
				<xsl:with-param name="defaultId" select="$nodeMiddlewareKey" />
			</xsl:call-template>	
		</xsl:variable>	
		
		<xsl:variable name="uppercaseFile" select="translate($File,
                                'abcdefghijklmnopqrstuvwxyz',
                                'ABCDEFGHIJKLMNOPQRSTUVWXYZ')" />
		
		<!-- find requested Component node or IDL file node -->
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
		<xsl:variable name="fileNode" select="/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'IDL']/../gml:data[@key=$transformNodeLabelKey][text() = $nodeName]/.." />

		<xsl:if test="contains($File, $ImplSuffix) and count($componentNode) &gt; 0" >
			<xsl:call-template name="ProjectFileImplNode">
				<xsl:with-param name="node" select="$componentNode" />
				<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey"/>
				<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey"/>
				<xsl:with-param name="transformNodeFolderKey" select="$transformNodeFolderKey"/>
				<xsl:with-param name="transformNodeWorkerKey" select="$transformNodeWorkerKey"/>
				<xsl:with-param name="transformNodeOperationKey" select="$transformNodeOperationKey"/>
			</xsl:call-template>	
		</xsl:if>

		<xsl:if test="not(contains($File, $ImplSuffix)) and count($fileNode) &gt; 0" >
			<xsl:call-template name="ProjectFileIDLNode">
				<xsl:with-param name="node" select="$fileNode" />
				<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey"/>
				<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey"/>
				<xsl:with-param name="transformNodeMiddlewareKey" select="$transformNodeMiddlewareKey"/>
				<xsl:with-param name="transformNodeKeyMemberKey" select="$transformNodeKeyMemberKey"/>
			</xsl:call-template>	
		</xsl:if>
		
    </xsl:template>
		
	<!-- Component Implementation mpc file -->
	<xsl:template name="ProjectFileImplNode">
		<xsl:param name="node" />
		<xsl:param name="transformNodeKindKey" />
		<xsl:param name="transformNodeLabelKey" />
		<xsl:param name="transformNodeFolderKey" />
		<xsl:param name="transformNodeWorkerKey" />
		<xsl:param name="transformNodeOperationKey" />

		<!-- Project_File_Begin -->
		<xsl:value-of select="'// This project file was generated by:&#xA;'"/>
		<xsl:value-of select="'// graphml2mpc.xsl&#xA;&#xA;'"/>
		
		<!-- Project_Write generate_impl_project -->
		<xsl:variable name="impl_basename" select="concat($node/gml:data[@key=$transformNodeLabelKey]/text(),$implSuffix)" />
		<xsl:variable name="impl_export" select="translate($impl_basename,
                                'abcdefghijklmnopqrstuvwxyz',
                                'ABCDEFGHIJKLMNOPQRSTUVWXYZ')" />

		<!-- Generate the executor project. -->
		<xsl:value-of select="concat('project (', $impl_basename  ,') : cuts_coworker_exec {&#xA;')"/>
		<xsl:value-of select="concat('  sharedname    = ', $impl_basename, '&#xA;&#xA;')"/>
		<xsl:value-of select="concat('  dynamicflags += ', $impl_export, '_BUILD_DLL&#xA;&#xA;')"/>
		<xsl:value-of select="concat('  prebuild = perl -- $(ACE_ROOT)/bin/generate_export_file.pl ', $impl_export, ' &gt; $(PROJECT_ROOT)/', $impl_basename, '_export.h&#xA;')"/>

		<!-- Filter the nodes correctly. -->
		<!--   from given Component node, find all event port references to other idl files -->
		<xsl:variable name="interfaceDefs" select="/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'InterfaceDefinitions']/.." />
		<xsl:variable name="componentChildrenIds" select="$node/gml:graph/gml:node/@id" />
		<xsl:variable name="refNodeIds" select="/descendant::*/gml:edge[@source=$componentChildrenIds]/@target" />
		<xsl:variable name="refFiles" select="$interfaceDefs/descendant::*/gml:node[@id=$refNodeIds]/gml:data[@key=$transformNodeKindKey][text() = 'Aggregate']/ancestor::*/gml:data[@key=$transformNodeKindKey][text() = 'IDL']/.." />
		<xsl:variable name="nodeFile" select="$node/ancestor::*/gml:data[@key=$transformNodeKindKey][text() = 'IDL']/.." />
		<!--   list of all Files required, include the current Component File -->
		<xsl:variable name="reqFiles" select="$refFiles | $nodeFile" />
		<xsl:variable name="reqExecFiles" select="$reqFiles/gml:graph/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'Component']/../../..
												| $reqFiles/gml:graph/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'BlackBox']/../../.." />
		
		<!-- generate the after declaration. -->
		<xsl:value-of select="'  after += '" />
		<xsl:for-each select="$reqExecFiles">
			<xsl:call-template name="generate_listing">
				<xsl:with-param name="node" select="." />
				<xsl:with-param name="suffix" select="$execSuffix" />
				<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey"/>
				<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey"/>
			</xsl:call-template>	
		</xsl:for-each>
		<xsl:for-each select="$reqFiles">
			<xsl:call-template name="generate_listing">
				<xsl:with-param name="node" select="." />
				<xsl:with-param name="suffix" select="$stubSuffix" />
				<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey"/>
				<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey"/>
			</xsl:call-template>	
		</xsl:for-each>
		<xsl:value-of select="'&#xA;&#xA;'"/>  
		
		<!-- generate the libs declaration. -->
		<xsl:value-of select="'  libs += '" />
		<xsl:for-each select="$reqExecFiles">
			<xsl:call-template name="generate_listing">
				<xsl:with-param name="node" select="." />
				<xsl:with-param name="suffix" select="$execSuffix" />
				<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey"/>
				<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey"/>
			</xsl:call-template>	
		</xsl:for-each>
		<xsl:for-each select="$reqFiles">
			<xsl:call-template name="generate_listing">
				<xsl:with-param name="node" select="." />
				<xsl:with-param name="suffix" select="$stubSuffix" />
				<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey"/>
				<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey"/>
			</xsl:call-template>	
		</xsl:for-each>
		<xsl:value-of select="'&#xA;&#xA;'"/> 

		<xsl:call-template name="generate_mpc_i">
			<xsl:with-param name="node" select="$node" />
			<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey"/>
			<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey"/>
			<xsl:with-param name="transformNodeFolderKey" select="$transformNodeFolderKey"/>
			<xsl:with-param name="transformNodeWorkerKey" select="$transformNodeWorkerKey"/>
			<xsl:with-param name="transformNodeOperationKey" select="$transformNodeOperationKey"/>
		</xsl:call-template>	

		<!-- Generate the source files. -->
		<xsl:value-of select=       "'  Source_Files {&#xA;'"/>
		<xsl:value-of select="concat('    ', $node/gml:data[@key=$transformNodeLabelKey]/text(), $ImplSuffix, '.cpp&#xA;')"/>
		<xsl:value-of select=       "'  }&#xA;&#xA;'"/>
		<!-- Generate the header files. -->
		<xsl:value-of select=       "'  Header_Files {&#xA;&#xA;'"/>
		<xsl:value-of select=       "'  }&#xA;&#xA;'"/>
		<xsl:value-of select=       "'  Inline_Files {&#xA;&#xA;'"/>
		<xsl:value-of select=       "'  }&#xA;&#xA;'"/>
		<xsl:value-of select=       "'  IDL_Files {&#xA;&#xA;'"/>
		<xsl:value-of select=       "'  }&#xA;'"/>
		<xsl:value-of select=       "'}&#xA;&#xA;'"/>

		<xsl:value-of select=       "'// end of auto-generated project file&#xA;&#xA;'"/>

	</xsl:template>	

	<!-- generate_listing for given suffix -->
	<xsl:template name="generate_listing">
		<xsl:param name="node" />
		<xsl:param name="suffix" />
		<xsl:param name="transformNodeLabelKey" />
		<xsl:param name="transformNodeKindKey" />
		
		<!-- listing always generated for stubSuffix, otherwise must have componentNodes to be included in listing -->
		<!-- If this IDL file has components -->
		<xsl:variable name="componentNodes" select="$node/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'Component']/..
												  | $node/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'BlackBox']/.." />
		<xsl:if test="($suffix = $stubSuffix) or (count($componentNodes) &gt; 0)" >
			<xsl:value-of select="' \&#xA;'"/>
			<xsl:value-of select="concat('    ', $node/gml:data[@key=$transformNodeLabelKey]/text(), $suffix)" />
		</xsl:if>
    </xsl:template>	
	
	<!-- generate_mpc_i -->
	<xsl:template name="generate_mpc_i">
		<xsl:param name="node" />
		<xsl:param name="transformNodeKindKey" />
		<xsl:param name="transformNodeLabelKey" />
		<xsl:param name="transformNodeFolderKey" />
		<xsl:param name="transformNodeWorkerKey" />
		<xsl:param name="transformNodeOperationKey" />
	
		<!-- find Behaviour/Workload Definition for this Component node -->
		<xsl:variable name="behaviourDefs" select="/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'BehaviourDefinitions']/.." />
		<xsl:variable name="implNodeId" select="/descendant::*/gml:edge[@target=$node/@id]/@source" />
		<xsl:variable name="implNode" select="$behaviourDefs/descendant::*/gml:node[@id=$implNodeId]/gml:data[@key=$transformNodeKindKey][text() = 'ComponentImpl']/.." />
		
		<xsl:variable name="workerIncludes" select="$implNode/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'Process']/../gml:data[@key=$transformNodeWorkerKey][text() != '']/.." />
		
		<!-- find all workers and add include paths -->
		<xsl:call-template name="generate_mpc_values">
			<xsl:with-param name="nodeList" select="$workerIncludes" />
			<xsl:with-param name="heading" select="'includes'" />
			<xsl:with-param name="transformNodeValueKey" select="$transformNodeFolderKey"/>
		</xsl:call-template>	
		
		<!-- if any workers, add the CUTS lib path -->
		<xsl:variable name="workerLibs" select="$implNode/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'Process']/../gml:data[@key=$transformNodeWorkerKey][text() != '']/../gml:data[@key=$transformNodeOperationKey][text() != '']/.." />

		<xsl:if test="count($workerLibs) &gt; 0" >
			<xsl:value-of select="'  libpaths +='" />

			<xsl:value-of select="' \&#xA;'"/>
			<xsl:value-of select="'    $(CUTS_ROOT)/lib' " />
			
			<xsl:value-of select="'&#xA;&#xA;'"/>
		</xsl:if>
		<!-- find all workers and add worker libs -->
		<xsl:call-template name="generate_mpc_values">
			<xsl:with-param name="nodeList" select="$workerLibs" />
			<xsl:with-param name="heading" select="'libs'" />
			<xsl:with-param name="transformNodeValueKey" select="$transformNodeWorkerKey"/>
		</xsl:call-template>	

	</xsl:template>	
	
	<!-- generate_mpc_values -->
	<xsl:template name="generate_mpc_values">
		<xsl:param name="nodeList" />
		<xsl:param name="heading" />
		<xsl:param name="transformNodeValueKey" />

		<xsl:if test="count($nodeList) &gt; 0" >
			<xsl:value-of select="concat('  ', $heading, ' +=')" />

			<!-- sort the list of items to output -->
			<xsl:variable name="delim" select="','" />
			<xsl:variable name="sortedNodeList">
				<xsl:for-each select="$nodeList"> 
					<xsl:sort select="./gml:data[@key=$transformNodeValueKey]/text()" data-type="text" />
					<xsl:value-of select="concat(./gml:data[@key=$transformNodeValueKey]/text(), $delim)"/>
				</xsl:for-each>
			</xsl:variable>

			<!-- only output unique values from the sorted list -->
			<xsl:call-template name="unique_mpc_values" >
				<xsl:with-param name="compare" select="''" />
				<xsl:with-param name="pText" select="$sortedNodeList" />
				<xsl:with-param name="delim" select="$delim" />
			</xsl:call-template>	

			 <xsl:value-of select="'&#xA;&#xA;'"/>
		</xsl:if>
	</xsl:template>	
	
	<!-- Output unique items from delimited list -->
	 <xsl:template name="unique_mpc_values">
		<xsl:param name="compare" />
		<xsl:param name="pText" />
		<xsl:param name="delim" />
		
		<xsl:if test="string-length($pText) > 0">
			<xsl:variable name="pCount" select="string-length($pText) - string-length(translate($pText, $delim, ''))" />

			<xsl:variable name="thisNode" select="substring-before(concat($pText, $delim), $delim)"/>

		    <xsl:if test="not( $thisNode = $compare )">
					<xsl:value-of select="' \&#xA;'"/>
					<xsl:value-of select="concat('    ', $thisNode )" />
            </xsl:if>
			
			<xsl:call-template name="unique_mpc_values">
				<xsl:with-param name="compare" select="$thisNode"/>
				<xsl:with-param name="pText" select="substring-after($pText, $delim)"/>
				<xsl:with-param name="delim" select="$delim" />
			</xsl:call-template> 
		</xsl:if>
	</xsl:template>

		
	<!-- IDL mpc files generated to contain IDL_Gen stub skel EIDL_Gen exec and svnt projects-->
	<xsl:template name="ProjectFileIDLNode">	
		<xsl:param name="node" />
		<xsl:param name="transformNodeKindKey" />
		<xsl:param name="transformNodeLabelKey" />
		<xsl:param name="transformNodeMiddlewareKey" />
		<xsl:param name="transformNodeKeyMemberKey" />
		
		<!-- Project_File_Begin -->
		<!-- Write the preamble for the project. -->
		<xsl:value-of select="'// This project file was generated by&#xA;'"/>
		<xsl:value-of select="'// graphml2mpc.xsl&#xA;&#xA;'"/>

		<xsl:variable name="middleware" select="/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'Model']/../gml:data[@key=$transformNodeMiddlewareKey]/text()" />
		<!-- from given IDL file node, find all event port references to other idl files -->
		<!--   future work may include worker complexType and attribute complexType defined in the model -->
		<xsl:variable name="interfaceDefs" select="/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'InterfaceDefinitions']/.." />
		<xsl:variable name="componentChildrenIds" select="$node/gml:graph/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'Component']/../gml:graph/gml:node/@id
														| $node/gml:graph/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'BlackBox']/../gml:graph/gml:node/@id" />
		<xsl:variable name="refNodeIds" select="/descendant::*/gml:edge[@source=$componentChildrenIds]/@target" />
		<xsl:variable name="refFiles" select="$interfaceDefs/descendant::*/gml:node[@id=$refNodeIds]/gml:data[@key=$transformNodeKindKey][text() = 'Aggregate']/ancestor::*/gml:data[@key=$transformNodeKindKey][text() = 'IDL']/.." />

		<!-- Let the traits write to the top of the project. -->

		<xsl:call-template name="write_top">
			<xsl:with-param name="node" select="$node" />
			<xsl:with-param name="middleware" select="$middleware" />
			<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey"/>
			<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey"/>
			<xsl:with-param name="transformNodeKeyMemberKey" select="$transformNodeKeyMemberKey"/>
		</xsl:call-template>
			
		<!-- Project_Write -->
		<xsl:call-template name="generate_idlgen_project">
			<xsl:with-param name="node" select="$node" />
			<xsl:with-param name="references" select="$refFiles"/>
			<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey"/>
			<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey"/>
			<xsl:with-param name="transformNodeKeyMemberKey" select="$transformNodeKeyMemberKey"/>
		</xsl:call-template>	

		<xsl:call-template name="generate_stub_project">
			<xsl:with-param name="node" select="$node" />
			<xsl:with-param name="middleware" select="$middleware"/>
			<xsl:with-param name="references" select="$refFiles"/>
			<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey"/>
			<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey"/>
			<xsl:with-param name="transformNodeKeyMemberKey" select="$transformNodeKeyMemberKey"/>
		</xsl:call-template>	
		
		<!-- If this IDL file has components -->
		<xsl:variable name="componentNodes" select="$node/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'Component']/..
												  | $node/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'BlackBox']/.." />
		<xsl:if test="count($componentNodes) &gt; 0" >  <!-- or has_interfaces -->
		
			<xsl:call-template name="generate_skel_project">
				<xsl:with-param name="node" select="$node" />
				<xsl:with-param name="middleware" select="$middleware"/>
				<xsl:with-param name="references" select="$refFiles"/>
				<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey"/>
				<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey"/>
			</xsl:call-template>
			<!--   if (this->file_has_object_with_reference (node.file_))  ie ProvidedRequestPort or RequiredRequestPort -->
			<xsl:call-template name="generate_eidl_project">
				<xsl:with-param name="node" select="$node" />
				<xsl:with-param name="middleware" select="$middleware"/>
				<xsl:with-param name="references" select="$refFiles"/>
				<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey"/>
				<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey"/>
			</xsl:call-template>
			<!--   if (this->file_has_object_with_reference (node.file_))  ie ProvidedRequestPort or RequiredRequestPort -->
			<xsl:call-template name="generate_exec_project">
				<xsl:with-param name="node" select="$node" />
				<xsl:with-param name="middleware" select="$middleware"/>
				<xsl:with-param name="references" select="$refFiles"/>
				<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey"/>
				<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey"/>
			</xsl:call-template>
			
			<xsl:call-template name="generate_svnt_project">
				<xsl:with-param name="node" select="$node" />
				<xsl:with-param name="middleware" select="$middleware"/>
				<xsl:with-param name="references" select="$refFiles"/>
				<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey"/>
				<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey"/>
			</xsl:call-template>
	
		</xsl:if>
		
		<!-- Project_File_End -->
		<xsl:value-of select="'// end of auto-generated project file&#xA;'"/>
	</xsl:template>	
	
	<!-- Traits write_top -->
	<xsl:template name="write_top">
		<xsl:param name="node" />
		<xsl:param name="middleware" />	
		<xsl:param name="transformNodeKindKey" />
		<xsl:param name="transformNodeLabelKey" />
		<xsl:param name="transformNodeKeyMemberKey" />
		
		<xsl:variable name="nodeName" select="$node/gml:data[@key=$transformNodeLabelKey]/text()" />
		<xsl:variable name="macro_basename" select="translate($nodeName,
                                'abcdefghijklmnopqrstuvwxyz',
                                'ABCDEFGHIJKLMNOPQRSTUVWXYZ')" />
		<xsl:variable name="componentNodes" select="$node/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'Component']/..
												  | $node/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'BlackBox']/.." />
		<xsl:variable name="has_dds_events" select="$node/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'Aggregate' or text() = 'Member']/../gml:data[@key=$transformNodeKeyMemberKey][text() = 'true']/.." />

		<xsl:choose>
		<xsl:when test="$middleware = 'tao'" >
			<xsl:if test="count($componentNodes) &gt; 0" >
				<xsl:variable name="project_iCCM_IDL_Gen" select="concat($nodeName, '_iCCM_IDL_Gen')" />
				<xsl:value-of select="concat('project (', $project_iCCM_IDL_Gen, ') : iccm_tao {&#xA;')"/>
				<xsl:value-of select=       "'  custom_only = 1&#xA;&#xA;'"/>
				<xsl:value-of select="concat('  after += ', $nodeName, '_IDL_Gen&#xA;&#xA;')"/>
				<xsl:value-of select="concat('  iccm_tao_flags += -Wb,stub_export_macro=', $macro_basename, '_STUB_Export \&#xA;')"/>
				<xsl:value-of select="concat('                    -Wb,stub_export_include=', $nodeName, '_stub_export.h \&#xA;')"/>
				<xsl:value-of select="concat('                    -Wb,svnt_export_macro=', $macro_basename, '_SVNT_Export \&#xA;')"/>
				<xsl:value-of select="concat('                    -Wb,svnt_export_include=', $nodeName, '_svnt_export.h \&#xA;')"/>
				<xsl:value-of select=       "'                    -Glem -Gsv&#xA;&#xA;'"/>
				<xsl:value-of select=       "'  ICCM_Files {&#xA;'"/>
				<xsl:value-of select=       "'    gendir = .&#xA;&#xA;'"/>
				<xsl:value-of select="concat('    ', $nodeName, '.idl&#xA;')"/>
				<xsl:value-of select=       "'  }&#xA;'"/>
				<xsl:value-of select=       "'}&#xA;&#xA;'" />
				<!-- this->stub_after_.insert (project_iCCM_IDL_Gen); -->
			</xsl:if>
		</xsl:when>

		<xsl:when test="$middleware = 'opensplice'" >
			<xsl:if test="count($has_dds_events) &gt; 0">	
				<!-- Because the file has a DDS event, we need to make sure we run
					 the OpenSplice IDL preprocessor on this file. This will ensure
					 we have the DDS types defined and implemented. -->
				<xsl:variable name="project_OSPL_IDL_Gen" select="concat($nodeName, '_OSPL_IDL_Gen')" />
				<xsl:value-of select="concat('project (', $project_OSPL_IDL_Gen, ') : splice_ts_defaults, requires_opensplice {&#xA;')"/>
				<xsl:value-of select=       "'  custom_only = 1&#xA;&#xA;'"/>
				<xsl:value-of select="concat('  after += ', $nodeName, '_IDL_Gen&#xA;&#xA;')"/>
				<xsl:value-of select=       "'  idlflags += -o . \&#xA;'"/>
				<xsl:value-of select="concat('              -Wb,export_macro=', $macro_basename, '_STUB_Export \&#xA;')"/>
				<xsl:value-of select="concat('              -Wb,export_include=', $nodeName, '_stub_export.h&#xA;&#xA;')"/>
				<xsl:value-of select=       "'  splice_ts_flags += -d . \&#xA;'"/>
				<xsl:value-of select=       "'                     -I $(TAO_ROOT) -I $(CIAO_ROOT) -I $(CIAO_ROOT)/ccm&#xA;&#xA;'"/>
				<xsl:value-of select=       "'  specific (prop:windows) {&#xA;'"/>
				<xsl:value-of select="concat('    splice_ts_flags += -P ', $macro_basename, '_STUB_Export,', $nodeName, '_stub_export.h&#xA;')"/>
				<xsl:value-of select=       "'  }&#xA;&#xA;'"/>
				<xsl:value-of select=       "'  SpliceTypeSupport_Files {&#xA;'"/>
				<xsl:value-of select=       "'    gendir = .&#xA;&#xA;'"/>
				<xsl:value-of select="concat('    ', $nodeName, '.idl&#xA;')"/>
				<xsl:value-of select=       "'  }&#xA;&#xA;'"/>
				<xsl:value-of select=       "'  IDL_Files {&#xA;'"/>
				<xsl:value-of select=       "'    gendir = .&#xA;&#xA;'"/>
				<xsl:value-of select="concat('    ', $nodeName, 'Dcps.idl&#xA;')"/>
				<xsl:value-of select=       "'  }&#xA;'"/>
				<xsl:value-of select=       "'}&#xA;&#xA;'"/>
			</xsl:if>
	 
			<xsl:if test="count($componentNodes) &gt; 0" >
				<xsl:variable name="project_iCCM_IDL_Gen" select="concat($nodeName, '_iCCM_IDL_Gen')" />
				<xsl:value-of select="concat('project (', $project_iCCM_IDL_Gen, ') : iccm_ospl, requires_opensplice {&#xA;')"/>
				<xsl:value-of select=       "'  custom_only = 1&#xA;&#xA;'"/>
				<xsl:value-of select="concat('  after += ', $nodeName, '_IDL_Gen&#xA;&#xA;')"/>
				<xsl:value-of select="concat('  iccm_ospl_flags += -Wb,stub_export_macro=', $macro_basename, '_STUB_Export \&#xA;')"/>
				<xsl:value-of select="concat('                     -Wb,stub_export_include=', $nodeName, '_stub_export.h \&#xA;')"/>
				<xsl:value-of select="concat('                     -Wb,svnt_export_macro=', $macro_basename, '_SVNT_Export \&#xA;')"/>
				<xsl:value-of select="concat('                     -Wb,svnt_export_include=', $nodeName, '_svnt_export.h \&#xA;')"/>
				<xsl:value-of select=       "'                     -Glem -Gsv&#xA;&#xA;'"/>
				<xsl:value-of select=       "'  ICCM_Files {&#xA;'"/>
				<xsl:value-of select=       "'    gendir = .&#xA;&#xA;'"/>
				<xsl:value-of select="concat('    ', $nodeName, '.idl&#xA;')"/>
				<xsl:value-of select=       "'  }&#xA;'"/>
				<xsl:value-of select=       "'}&#xA;&#xA;'"/>
				<!-- this->stub_after_.insert (project_iCCM_IDL_Gen); -->
			</xsl:if>
		</xsl:when>
		
		<xsl:when test="$middleware = 'rtidds'" >
			<xsl:if test="count($has_dds_events) &gt; 0">	
				<!-- Write the project that generates type support for the dds events in the idl file. -->
				<xsl:variable name="project_NDDS_IDL_Gen" select="concat($nodeName, '_NDDS_IDL_Gen')" />
				<xsl:value-of select="concat('project (', $project_NDDS_IDL_Gen, ') : ndds_cck_ts_defaults, requires_rtidds {&#xA;')"/>
				<xsl:value-of select=       "'  custom_only = 1&#xA;&#xA;'"/>
				<xsl:value-of select="concat('  after += ', $nodeName, '_IDL_Gen&#xA;&#xA;')"/>
				<xsl:value-of select=       "'  ndds_ts_flags += -d .&#xA;'"/>
				<xsl:value-of select=       "'  ndds_ts_flags -= $(PLATFORM_NDDS_FLAGS)&#xA;&#xA;'"/>
				<xsl:value-of select=       "'  NDDSTypeSupport_Files {&#xA;'"/>
				<xsl:value-of select="concat('    ', $nodeName, '.idl&#xA;')"/>
				<xsl:value-of select=       "'  }&#xA;'"/>
				<xsl:value-of select=       "'}&#xA;&#xA;'"/>
				<!-- Make sure the stub builds after this project.
				this->stub_after_.insert (project_NDDS_IDL_Gen); -->
			</xsl:if>
				
			<xsl:if test="count($componentNodes) &gt; 0" >
				<!-- Generate the iCCM source files. -->
 				<xsl:variable name="project_iCCM_IDL_Gen" select="concat($nodeName, '_iCCM_IDL_Gen')" />
				<xsl:value-of select="concat('project (', $project_iCCM_IDL_Gen, ') : iccm_rtidds, requires_rtidds {&#xA;')"/>
				<xsl:value-of select=       "'  custom_only = 1&#xA;&#xA;'"/>
				<xsl:value-of select="concat('  after += ', $nodeName, '_IDL_Gen&#xA;&#xA;')"/>
				<xsl:value-of select="concat('  iccm_rtidds_flags += -Wb,stub_export_macro=', $macro_basename, '_STUB_Export \&#xA;')"/>
				<xsl:value-of select="concat('                       -Wb,stub_export_include=', $nodeName, '_stub_export.h \&#xA;')"/>
				<xsl:value-of select="concat('                       -Wb,svnt_export_macro=', $macro_basename, '_SVNT_Export \&#xA;')"/>
				<xsl:value-of select="concat('                       -Wb,svnt_export_include=', $nodeName, '_svnt_export.h \&#xA;')"/>
				<xsl:value-of select=       "'                       -Glem -Gsv&#xA;&#xA;'"/>
				<xsl:value-of select=       "'  ICCM_Files {&#xA;'"/>
				<xsl:value-of select=       "'    gendir = .&#xA;&#xA;'"/>
				<xsl:value-of select="concat('    ', $nodeName, '.idl&#xA;')"/>
				<xsl:value-of select=       "'  }&#xA;'"/>
				<xsl:value-of select=       "'}&#xA;&#xA;'"/>
				<!-- Make sure the stub project builds after this project. this->stub_after_.insert (project_iCCM_IDL_Gen); -->
			</xsl:if>
		</xsl:when>

		<xsl:when test="$middleware = 'coredx'" >
			<xsl:if test="count($has_dds_events) &gt; 0">	
				<!-- Write the project that generates type support for the dds events in the idl file. -->
				<xsl:variable name="project_CoreDX_DDL_Gen" select="concat($nodeName, '_CoreDX_DDL_Gen')" />
				<xsl:value-of select="concat('project (', $project_CoreDX_DDL_Gen, ') : requires_coredx, coredx_ts_defaults, iccm_coredx_idl2ddl {&#xA;')"/>
				<xsl:value-of select=       "'  custom_only = 1&#xA;&#xA;'"/>
				<xsl:value-of select="concat('  after += ', $nodeName, '_IDL_Gen&#xA;&#xA;')"/>
				<xsl:value-of select=       "'  IDL2DDL_Files {&#xA;'"/>
				<xsl:value-of select=       "'    gendir = .&#xA;'"/>
				<xsl:value-of select="concat('    ', $nodeName, '.idl&#xA;')"/>
				<xsl:value-of select=       "'  }&#xA;'"/>
				<xsl:value-of select=       "'  CoreDXTypeSupport_Files {&#xA;'"/>
				<xsl:value-of select=       "'    gendir = .&#xA;'"/>
				<xsl:value-of select="concat('    CoreDX_', $nodeName, '.ddl&#xA;')"/>
				<xsl:value-of select=       "'  }&#xA;'"/>
				<xsl:value-of select=       "'}&#xA;&#xA;'"/>
			</xsl:if>
				
			<xsl:if test="count($componentNodes) &gt; 0" >
				<!-- Generate the iCCM source files. -->
 				<xsl:variable name="project_iCCM_IDL_Gen" select="concat($nodeName, '_iCCM_IDL_Gen')" />
				<xsl:value-of select="concat('project (', $project_iCCM_IDL_Gen, ') : iccm_coredx, requires_coredx {&#xA;')"/>
				<xsl:value-of select=       "'  custom_only = 1&#xA;&#xA;'"/>
				<xsl:value-of select="concat('  after += ', $nodeName, '_IDL_Gen&#xA;&#xA;')"/>
				<xsl:value-of select="concat('  iccm_coredx_flags += -Wb,stub_export_macro=', $macro_basename, '_STUB_Export \&#xA;')"/>
				<xsl:value-of select="concat('                       -Wb,stub_export_include=', $nodeName, '_stub_export.h \&#xA;')"/>
				<xsl:value-of select="concat('                       -Wb,svnt_export_macro=', $macro_basename, '_SVNT_Export \&#xA;')"/>
				<xsl:value-of select="concat('                       -Wb,svnt_export_include=', $nodeName, '_svnt_export.h \&#xA;')"/>
				<xsl:value-of select=       "'                       -Glem -Gsv&#xA;&#xA;'"/>
				<xsl:value-of select=       "'  ICCM_Files {&#xA;'"/>
				<xsl:value-of select=       "'    gendir = .&#xA;&#xA;'"/>
				<xsl:value-of select="concat('    ', $nodeName, '.idl&#xA;')"/>
				<xsl:value-of select=       "'  }&#xA;'"/>
				<xsl:value-of select=       "'}&#xA;&#xA;'"/>
				<!-- Make sure the stub project builds after this project. this->stub_after_.insert (project_iCCM_IDL_Gen); -->
			</xsl:if>
		</xsl:when>
		
		<xsl:when test="$middleware = 'qpidpb'" >
			<xsl:if test="count($has_dds_events) &gt; 0">	
				<!-- Write the project that generates type support for the dds events in the idl file. -->
				<xsl:variable name="project_QpidPB_IDL2Proto_Gen" select="concat($nodeName, '_QpidPB_IDL2Proto_Gen')" />
				<xsl:value-of select="concat('project (',$project_QpidPB_IDL2Proto_Gen, ') : requires_qpidpb, iccm_qpidpb_idl2proto {&#xA;')"/>
				<xsl:value-of select=       "'  custom_only = 1&#xA;&#xA;'"/>
				<xsl:value-of select="concat('  after += ', $nodeName, '_IDL_Gen&#xA;&#xA;')"/>
				<xsl:value-of select=       "'  IDL2Proto_Files {&#xA;'"/>
				<xsl:value-of select=       "'  gendir = .&#xA;&#xA;'"/>
				<xsl:value-of select="concat('    ', $nodeName, '.idl&#xA;')"/>
				<xsl:value-of select=       "'  }&#xA;'"/>
				<xsl:value-of select=       "'}&#xA;'"/>				
				
				<xsl:variable name="project_QpidPB_Proto_Gen" select="concat($nodeName, '_QpidPB_Proto_Gen')" />
				<xsl:value-of select="concat('project (', $project_QpidPB_Proto_Gen, ') : requires_qpidpb, cuts_protoc {&#xA;')"/>
				<xsl:value-of select=       "'  custom_only = 1&#xA;&#xA;'"/>
				<xsl:value-of select="concat('  after += ', $nodeName, '_QpidPB_IDL2Proto_Gen&#xA;&#xA;')"/>
				<xsl:value-of select=       "'  specific(prop:windows) {&#xA;'"/>
				<xsl:value-of select="concat('    cuts_protoc_flags += --cpp_out=dllexport_decl=', $macro_basename, '_STUB_Export:.&#xA;')"/>
				<xsl:value-of select=       "'  } else {&#xA;'"/>
				<xsl:value-of select=       "'    cuts_protoc_flags += --cpp_out=.&#xA;'"/>
				<xsl:value-of select=       "'  }&#xA;&#xA;'"/>
				<xsl:value-of select=       "'  Protoc_Files {&#xA;'"/>
				<xsl:value-of select=       "'    gendir = .&#xA;&#xA;'"/>
				<xsl:value-of select="concat('    ', $nodeName, '.proto&#xA;')"/>
				<xsl:value-of select=       "'  }&#xA;'"/>
				<xsl:value-of select=       "'}&#xA;'"/>
			</xsl:if>
				
			<xsl:if test="count($componentNodes) &gt; 0" >
				<!-- Generate the iCCM source files. -->
 				<xsl:variable name="project_iCCM_IDL_Gen" select="concat($nodeName, '_iCCM_IDL_Gen')" />
				<xsl:value-of select="concat('project (', $project_iCCM_IDL_Gen, ') : iccm_qpidpb, requires_qpidpb {&#xA;')"/>
				<xsl:value-of select=       "'  custom_only = 1&#xA;&#xA;'"/>
				<xsl:value-of select="concat('  after += ', $nodeName, '_IDL_Gen&#xA;&#xA;')"/>
				<xsl:value-of select="concat('  iccm_qpidpb_flags += -Wb,stub_export_macro=', $macro_basename, '_STUB_Export \&#xA;')"/>
				<xsl:value-of select="concat('                       -Wb,stub_export_include=', $nodeName, '_stub_export.h \&#xA;')"/>
				<xsl:value-of select="concat('                       -Wb,svnt_export_macro=', $macro_basename, '_SVNT_Export \&#xA;')"/>
				<xsl:value-of select="concat('                       -Wb,svnt_export_include=', $nodeName, '_svnt_export.h \&#xA;')"/>
				<xsl:value-of select=       "'                       -Glem -Gsv&#xA;&#xA;'"/>
				<xsl:value-of select=       "'  ICCM_Files {&#xA;'"/>
				<xsl:value-of select=       "'    gendir = .&#xA;&#xA;'"/>
				<xsl:value-of select="concat('    ', $nodeName, '.idl&#xA;')"/>
				<xsl:value-of select=       "'  }&#xA;'"/>
				<xsl:value-of select=       "'}&#xA;&#xA;'"/>
				<!-- Make sure the stub project builds after this project. this->stub_after_.insert (project_iCCM_IDL_Gen); -->
			</xsl:if>
		</xsl:when>
		
		<xsl:when test="$middleware = 'tcpip'" >
			<xsl:if test="count($componentNodes) &gt; 0" >
				<xsl:variable name="project_iCCM_IDL_Gen" select="concat($nodeName, '_iCCM_IDL_Gen')" />
				<!-- Generate the iCCM source files. -->
				<xsl:value-of select="concat('project (', $project_iCCM_IDL_Gen, ') : iccm_tcpip, requires_tcpip {&#xA;')"/>
				<xsl:value-of select=       "'  custom_only = 1&#xA;&#xA;'"/>
				<xsl:value-of select="concat('  after += ', $nodeName, '_IDL_Gen&#xA;&#xA;')"/>
				<xsl:value-of select="concat('  iccm_tcpip_flags += -Wb,stub_export_macro=', $macro_basename, '_STUB_Export \&#xA;')"/>
				<xsl:value-of select="concat('                      -Wb,stub_export_include=', $nodeName, '_stub_export.h \&#xA;')"/>
				<xsl:value-of select="concat('                      -Wb,svnt_export_macro=', $macro_basename, '_SVNT_Export \&#xA;')"/>
				<xsl:value-of select="concat('                      -Wb,svnt_export_include=', $nodeName, '_svnt_export.h \&#xA;')"/>
				<xsl:value-of select=       "'                      -Glem -Gsv&#xA;&#xA;'"/>
				<xsl:value-of select=       "'  ICCM_Files {&#xA;'"/>
				<xsl:value-of select=       "'    gendir = .&#xA;&#xA;'"/>
				<xsl:value-of select="concat('    ', $nodeName, '.idl&#xA;')"/>
				<xsl:value-of select=       "'  }&#xA;'"/>
				<xsl:value-of select=       "'}&#xA;&#xA;'"/>
				<!-- this->stub_after_.insert (project_iCCM_IDL_Gen); -->
			</xsl:if>
		</xsl:when>
		
		
		<xsl:otherwise>
			<xsl:value-of select="''" />
		</xsl:otherwise>
		</xsl:choose>
	</xsl:template>	
	
	<!-- generate_idlgen_project -->
	<xsl:template name="generate_idlgen_project">
		<xsl:param name="node" />
		<xsl:param name="references" />
		<xsl:param name="transformNodeKindKey" />
		<xsl:param name="transformNodeLabelKey" />
		<xsl:param name="transformNodeKeyMemberKey" />
		
		<xsl:variable name="nodeName" select="$node/gml:data[@key=$transformNodeLabelKey]/text()" />

		<xsl:variable name="macro_basename" select="translate($nodeName,
                                'abcdefghijklmnopqrstuvwxyz',
                                'ABCDEFGHIJKLMNOPQRSTUVWXYZ')" />

		<!-- Generate the project. -->
		<xsl:value-of select="concat('project (', $nodeName,  '_IDL_Gen) : ')" />
		<xsl:variable name="componentNodes" select="$node/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'Component']/..
												  | $node/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'BlackBox']/.." />
		<xsl:variable name="has_dds_events" select="$node/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'Aggregate' or text() = 'Member']/../gml:data[@key=$transformNodeKeyMemberKey][text() = 'true']/.." />

		<xsl:choose>
		<xsl:when test="count($componentNodes) &gt; 0" >
			<xsl:value-of select="'component'" />
		</xsl:when>
		<xsl:otherwise>
			<xsl:value-of select="'ciao'" />
		</xsl:otherwise>
		</xsl:choose>
		<xsl:value-of select=       "'idldefaults, cuts_codegen_defaults {&#xA;'"/>
		<xsl:value-of select=       "'  custom_only = 1&#xA;&#xA;'"/>
		<xsl:value-of select="concat('  idlflags += -Wb,stub_export_macro=', $macro_basename, '_STUB_Export \&#xA;')"/>
		<xsl:value-of select="concat('              -Wb,stub_export_include=', $nodeName, '_stub_export.h \&#xA;')"/>
		<xsl:value-of select="concat('              -Wb,skel_export_macro=', $macro_basename, '_SKEL_Export \&#xA;')"/>
		<xsl:value-of select="concat('              -Wb,skel_export_include=', $nodeName, '_skel_export.h \&#xA;')"/>
		<xsl:value-of select=       "'              -Gxhst -Gxhsk \&#xA;'"/>

		<xsl:if test="(count($has_dds_events) &gt; 0) or (count($componentNodes) &gt; 0)" >  <!-- or has_interfaces -->
		
			<xsl:value-of select="concat('              -Wb,exec_export_macro=', $macro_basename, '_EXEC_Export \&#xA;')"/>
			<xsl:value-of select="concat('              -Wb,exec_export_include=', $nodeName, '_exec_export.h \&#xA;')"/>
			<xsl:value-of select=       "'              -Gxhex \&#xA;'"/>

			<xsl:if test="count($componentNodes) &gt; 0" >
				<xsl:value-of select="concat('              -Wb,svnt_export_macro=', $macro_basename, '_SVNT_Export \&#xA;')"/>
				<xsl:value-of select="concat('              -Wb,svnt_export_include=', $nodeName, '_svnt_export.h \&#xA;')"/>
				<xsl:value-of select=       "'              -Gxhsv \&#xA;'"/>
			</xsl:if>
		</xsl:if>
		
		<!--   if (this->file_has_object_with_reference (node.file_))  ie ProvidedRequestPort or RequiredRequestPort
			<xsl:value-of select=       "'              -Glem -Gsv \&#xA;'"/>
	    -->
		
		<xsl:value-of select=       "'              -Sa -Sal -St&#xA;'"/>

		<!-- for every middleware used the generate_default_servant is false -->
		<!-- if (node.has_components_ && !this->ctx_.traits_->generate_default_servant ()) -->
		<!-- so just test if it has components -->
		<xsl:if test="count($componentNodes) &gt; 0" >
			<xsl:value-of select="'&#xA;  idlflags -= -Gsv&#xA;'"/>
		</xsl:if>

		<xsl:if test="count($references) &gt; 0">
			<!-- Generate the dependencies for this project. This will be all the client projects of the references for this node.   -->
			<xsl:value-of select="'&#xA;'"/>
			<xsl:value-of select="'  after +='"/>
			<xsl:for-each select="$references">
				<xsl:call-template name="generate_listing">
					<xsl:with-param name="node" select="." />
					<xsl:with-param name="suffix" select="$idlSuffix" />
					<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey"/>
					<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey"/>
				</xsl:call-template>	
			</xsl:for-each>
			<xsl:value-of select="'&#xA;'"/> 
		</xsl:if> 
		
		<xsl:value-of select="'&#xA;  IDL_Files {&#xA;'"/>
		<xsl:value-of select="concat('    ', $nodeName, '.idl&#xA;')"/>

		<!-- nothing is produced by this function -->
		<!-- this->ctx_.traits_->write_idl_gen_files (this->ctx_.project_, node); -->
		<xsl:value-of select="'  }&#xA;'"/>
		<xsl:value-of select="'}&#xA;&#xA;'"/>
		
	</xsl:template>	
	
	<!-- generate_stub_project -->
	<xsl:template name="generate_stub_project">
		<xsl:param name="node" />
		<xsl:param name="middleware" />
		<xsl:param name="references" />
		<xsl:param name="transformNodeKindKey" />
		<xsl:param name="transformNodeLabelKey" />
		<xsl:param name="transformNodeKeyMemberKey" />

		<xsl:variable name="nodeName" select="$node/gml:data[@key=$transformNodeLabelKey]/text()" />

		<!-- Generate the export file for the project. -->
		<xsl:variable name="stub_name" select="concat( $nodeName, $stubSuffix )" />
		<xsl:variable name="stub_export" select="translate($stub_name,
                                'abcdefghijklmnopqrstuvwxyz',
                                'ABCDEFGHIJKLMNOPQRSTUVWXYZ')" />
		<xsl:variable name="stub_export_macro" select="concat($stub_export, '_Export')" />
		<xsl:variable name="stub_export_file" select="concat($stub_name, '_export.h')" />

		<!-- Generate the project. -->
		<xsl:value-of select="concat('project (', $stub_name, ') : avoids_ace_for_tao, ')" />
		<xsl:call-template name="stub_base_project">
			<xsl:with-param name="middleware" select="$middleware" />
		</xsl:call-template>	
		<xsl:value-of select="', cuts_codegen_defaults {&#xA;'"/>
		<xsl:value-of select="concat('  sharedname    = ', $stub_name, '&#xA;')"/>
		<xsl:value-of select="concat('  dynamicflags += ', $stub_export, '_BUILD_DLL&#xA;&#xA;')"/>
		<xsl:value-of select="concat('  after        += ', $nodeName, '_IDL_Gen')" />

		<xsl:call-template name="write_stub_after">
			<xsl:with-param name="node" select="$node" />
			<xsl:with-param name="middleware" select="$middleware" />
			<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey"/>
			<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey"/>
			<xsl:with-param name="transformNodeKeyMemberKey" select="$transformNodeKeyMemberKey"/>
		</xsl:call-template>
		<xsl:value-of select="'&#xA;&#xA;'"/>
		
		<xsl:if test="count($references) &gt; 0">
			<!-- Generate the dependencies for this project. This will be all the client projects of the references for this node.   -->
			<xsl:value-of select="'&#xA;'"/>
			<xsl:value-of select="'  // listing of project dependencies&#xA;'"/>
			<xsl:value-of select="'  after +='"/>
			<xsl:for-each select="$references">
				<xsl:call-template name="generate_listing">
					<xsl:with-param name="node" select="." />
					<xsl:with-param name="suffix" select="$stubSuffix" />
					<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey"/>
					<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey"/>
				</xsl:call-template>	
			</xsl:for-each>
			<!-- Generate the import libraries for this project. This will be all client project of the references for this node. 
			     They will also be the same as those specified in the "after" tag.  -->
			<xsl:value-of select="'&#xA;&#xA;'"/>
			<xsl:value-of select="'  libs  +='"/>
			<xsl:for-each select="$references">
				<xsl:call-template name="generate_listing">
					<xsl:with-param name="node" select="." />
					<xsl:with-param name="suffix" select="$stubSuffix" />
					<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey"/>
					<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey"/>
				</xsl:call-template>	
			</xsl:for-each>
			<xsl:value-of select="'&#xA;'"/> 
		</xsl:if> 
		
		<!-- Allow the backend to write custom declarations to the project. (specific middleware stuff) -->
		<xsl:call-template name="write_stub_custom">
			<xsl:with-param name="node" select="$node" />
			<xsl:with-param name="middleware" select="$middleware" />
			<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey"/>
			<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey"/>
		</xsl:call-template>
		<xsl:value-of select="'&#xA;'"/> 
		
		<!-- Generate the IDL files for this project. -->
		<xsl:value-of select="concat('  IDL_Files {', '&#xA;&#xA;')"/>
		<xsl:value-of select="concat('  }', '&#xA;&#xA;')"/> 
		<!-- Generate the source files for this project. -->
		<xsl:value-of select="concat('  Source_Files {', '&#xA;')"/>
		<xsl:value-of select="concat('    ', $nodeName, 'C.cpp', '&#xA;')"/> 
		
		<xsl:call-template name="write_stub_source_files">
			<xsl:with-param name="node" select="$node" />
			<xsl:with-param name="middleware" select="$middleware" />
			<xsl:with-param name="references" select="$references" />
			<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey"/>
			<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey"/>
			<xsl:with-param name="transformNodeKeyMemberKey" select="$transformNodeKeyMemberKey"/>
		</xsl:call-template>
		<xsl:value-of select="'  }&#xA;&#xA;'"/> 

		<!-- Generate the header files for this project. -->
		<xsl:value-of select="'  Header_Files {&#xA;'"/> 
		<xsl:value-of select="concat('    ', $nodeName, 'C.h&#xA;')"/> 
		<xsl:value-of select="'  }&#xA;&#xA;'"/> 
		<xsl:value-of select="'  Inline_Files {&#xA;'"/> 
		<xsl:value-of select="concat('    ', $nodeName, 'C.inl&#xA;')"/> 
		<xsl:value-of select="'  }&#xA;'"/> 
		<xsl:value-of select="'}&#xA;&#xA;'"/> 
	
	</xsl:template>	
	
	<!-- Traits stub_base_project -->
	<xsl:template name="stub_base_project">
		<xsl:param name="middleware" />	
		
		<xsl:choose>
		<xsl:when test="$middleware = 'tao'" >
			<xsl:value-of select="'iccm_tao_stub'" />
		</xsl:when>
		<xsl:when test="$middleware = 'opensplice'" >
			<xsl:value-of select="'iccm_opensplice_stub'" />
		</xsl:when>
		<xsl:when test="$middleware = 'rtidds'" >
			<xsl:value-of select="'iccm_rtidds_stub'" />
		</xsl:when>
		<xsl:when test="$middleware = 'coredx'" >
			<xsl:value-of select="'iccm_coredx_stub'" />
		</xsl:when>
		<xsl:when test="$middleware = 'qpidpb'" >
			<xsl:value-of select="'iccm_qpidpb_stub'" />
		</xsl:when>
		<xsl:when test="$middleware = 'tcpip'" >
			<xsl:value-of select="'iccm_tcpip_stub'" />
		</xsl:when>
		<xsl:otherwise>
			<xsl:value-of select="''" />
		</xsl:otherwise>
		</xsl:choose>
	</xsl:template>	
	
	<!-- Traits write_stub_after --> 
	<!-- Make sure the stub project builds after projects in write_top -->
	<xsl:template name="write_stub_after"> 
		<xsl:param name="node" />
		<xsl:param name="middleware" />	
		<xsl:param name="transformNodeKindKey" />
		<xsl:param name="transformNodeLabelKey" />
		<xsl:param name="transformNodeKeyMemberKey" />
		
		<xsl:variable name="nodeName" select="$node/gml:data[@key=$transformNodeLabelKey]/text()" />
		<xsl:variable name="componentNodes" select="$node/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'Component']/..
												  | $node/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'BlackBox']/.." />
		<xsl:variable name="has_dds_events" select="$node/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'Aggregate' or text() = 'Member']/../gml:data[@key=$transformNodeKeyMemberKey][text() = 'true']/.." />

		<xsl:choose>
		<xsl:when test="$middleware = 'tao'" >
			<xsl:if test="count($componentNodes) &gt; 0" >
				<xsl:variable name="project_iCCM_IDL_Gen" select="concat($nodeName, '_iCCM_IDL_Gen')" />
				<xsl:value-of select="concat(' ', $project_iCCM_IDL_Gen)" />
			</xsl:if>
		</xsl:when>

		<xsl:when test="$middleware = 'opensplice'" >
			<xsl:if test="count($has_dds_events) &gt; 0" >
				<xsl:variable name="project_OSPL_IDL_Gen" select="concat($nodeName, '_OSPL_IDL_Gen')" />
				<xsl:value-of select="concat(' ', $project_OSPL_IDL_Gen)" />
			</xsl:if>
			<xsl:if test="count($componentNodes) &gt; 0" >
	 			<xsl:variable name="project_iCCM_IDL_Gen" select="concat($nodeName, '_iCCM_IDL_Gen')" />
				<xsl:value-of select="concat(' ', $project_iCCM_IDL_Gen)" />
			</xsl:if>
		</xsl:when>
		
		<xsl:when test="$middleware = 'rtidds'" >
			<xsl:if test="count($has_dds_events) &gt; 0" >
				<xsl:variable name="project_NDDS_IDL_Gen" select="concat($nodeName, '_NDDS_IDL_Gen')" />
				<xsl:value-of select="concat(' ', $project_NDDS_IDL_Gen)" />
			</xsl:if>
			<xsl:if test="count($componentNodes) &gt; 0" >
 				<xsl:variable name="project_iCCM_IDL_Gen" select="concat($nodeName, '_iCCM_IDL_Gen')" />
				<xsl:value-of select="concat(' ', $project_iCCM_IDL_Gen)" />
			</xsl:if>
		</xsl:when>

		<xsl:when test="$middleware = 'coredx'" >
			<xsl:if test="count($has_dds_events) &gt; 0" >
				<xsl:variable name="project_CoreDX_DDL_Gen" select="concat($nodeName, '_CoreDX_DDL_Gen')" />
				<xsl:value-of select="concat(' ', $project_CoreDX_DDL_Gen)" />
			</xsl:if>
			<xsl:if test="count($componentNodes) &gt; 0" >
 				<xsl:variable name="project_iCCM_IDL_Gen" select="concat($nodeName, '_iCCM_IDL_Gen')" />
				<xsl:value-of select="concat(' ', $project_iCCM_IDL_Gen)" />
			</xsl:if>
		</xsl:when>
		
		<xsl:when test="$middleware = 'qpidpb'" >
			<xsl:if test="count($has_dds_events) &gt; 0" >
				<xsl:variable name="project_QpidPB_Proto_Gen" select="concat($nodeName, '_QpidPB_Proto_Gen')" />
				<xsl:value-of select="concat(' ', $project_QpidPB_Proto_Gen)" />
			</xsl:if>
			<xsl:if test="count($componentNodes) &gt; 0" >
 				<xsl:variable name="project_iCCM_IDL_Gen" select="concat($nodeName, '_iCCM_IDL_Gen')" />
				<xsl:value-of select="concat(' ', $project_iCCM_IDL_Gen)" />
			</xsl:if>
		</xsl:when>
		
		<xsl:when test="$middleware = 'tcpip'" >
			<xsl:if test="count($componentNodes) &gt; 0" >
				<xsl:variable name="project_iCCM_IDL_Gen" select="concat($nodeName, '_iCCM_IDL_Gen')" />
				<xsl:value-of select="concat(' ', $project_iCCM_IDL_Gen)" />
			</xsl:if>
		</xsl:when>
		<xsl:otherwise>
			<xsl:value-of select="''" />
		</xsl:otherwise>
		</xsl:choose>
	</xsl:template>	
	
	<!-- Traits write_stub_custom --> 
	<xsl:template name="write_stub_custom"> 
		<xsl:param name="node" />
		<xsl:param name="middleware" />	
		<xsl:param name="transformNodeKindKey" />
		<xsl:param name="transformNodeLabelKey" />

		<xsl:variable name="nodeName" select="$node/gml:data[@key=$transformNodeLabelKey]/text()" />
		<xsl:variable name="componentNodes" select="$node/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'Component']/..
												|   $node/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'BlackBox']/.." />

		<xsl:choose>
		<xsl:when test="$middleware = 'rtidds'" >
			<xsl:value-of select="'&#xA;'"/>
			<xsl:value-of select="'  specific (prop:windows) {&#xA;'"/>
			<xsl:value-of select="'    dynamicflags += NDDS_USER_DLL_EXPORT&#xA;'"/>
			<xsl:value-of select="'  }&#xA;'"/>
		</xsl:when>	
		<xsl:when test="$middleware = 'coredx'" >
			<xsl:if test="count($componentNodes) = 0" >
				<xsl:value-of select="'&#xA;'"/>
				<xsl:value-of select="'  specific (prop:windows) {&#xA;'"/>
				<xsl:value-of select="'    dynamicflags += COREDX_DLL_TS_EXPORTS LD&#xA;'"/>
				<xsl:value-of select="'  }&#xA;'"/>
			</xsl:if>
		</xsl:when>
		<xsl:when test="$middleware = 'qpidpb'" >
			<xsl:if test="count($componentNodes) = 0" >
				<xsl:value-of select=       "'  specific(prop:windows) {&#xA;'"/>
				<xsl:value-of select="concat('     compile_flags += /FI', $nodeName, '_stub_export.h&#xA;')"/>
				<xsl:value-of select=       "'  }&#xA;'"/>
			</xsl:if>
		</xsl:when>
		<xsl:otherwise>
			<xsl:value-of select="''" />
		</xsl:otherwise>
		</xsl:choose>
	</xsl:template>	
	
	<!-- Traits write_stub_source_files -->
	<xsl:template name="write_stub_source_files">
		<xsl:param name="node" />
		<xsl:param name="middleware" />	
		<xsl:param name="references" />	
		<xsl:param name="transformNodeKindKey" />
		<xsl:param name="transformNodeLabelKey" />
		<xsl:param name="transformNodeKeyMemberKey" />
		
		<xsl:variable name="nodeName" select="$node/gml:data[@key=$transformNodeLabelKey]/text()" />
		<xsl:variable name="componentNodes" select="$node/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'Component']/..
												  | $node/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'BlackBox']/.." />
		<xsl:variable name="has_dds_events" select="$node/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'Aggregate' or text() = 'Member']/../gml:data[@key=$transformNodeKeyMemberKey][text() = 'true']/.." />

		<xsl:choose>
		<xsl:when test="$middleware = 'tao'" >
			<xsl:if test="count($componentNodes) &gt; 0" >
				<xsl:value-of select="concat('    TAO_', $nodeName, 'C.cpp&#xA;')"/>
			</xsl:if>
		</xsl:when>
		<xsl:when test="$middleware = 'opensplice'" >
			<xsl:if test="count($has_dds_events) &gt; 0">	
				<xsl:value-of select="concat('    ', $nodeName, 'DcpsC.cpp&#xA;')"/>
				<xsl:value-of select="concat('    ', $nodeName, 'SplDcps.cpp&#xA;')"/>
				<xsl:value-of select="concat('    ', $nodeName, 'Dcps_impl.cpp&#xA;')"/>
			</xsl:if>
			<xsl:if test="count($componentNodes) &gt; 0" >
				<xsl:value-of select="concat('    OpenSplice_', $nodeName, 'C.cpp&#xA;')"/>
			</xsl:if>
		</xsl:when>
		<xsl:when test="$middleware = 'rtidds'" >
			<xsl:if test="count($has_dds_events) &gt; 0">	
				<xsl:value-of select="concat('    ', $nodeName, '.cxx&#xA;')"/>
				<xsl:value-of select="concat('    ', $nodeName, 'Plugin.cxx&#xA;')"/>
				<xsl:value-of select="concat('    ', $nodeName, 'Support.cxx&#xA;')"/>
			</xsl:if>
			<xsl:if test="count($componentNodes) &gt; 0" >
				<xsl:value-of select="concat('    RTIDDS_', $nodeName, 'C.cpp&#xA;')"/>
			</xsl:if>
		</xsl:when>
		<xsl:when test="$middleware = 'coredx'" >
			<xsl:if test="count($has_dds_events) &gt; 0">	
				<xsl:value-of select="concat('    CoreDX_', $nodeName, '.cc&#xA;')"/>
				<xsl:value-of select="concat('    CoreDX_', $nodeName, 'DataReader.cc&#xA;')"/>
				<xsl:value-of select="concat('    CoreDX_', $nodeName, 'DataWriter.cc&#xA;')"/>
				<xsl:value-of select="concat('    CoreDX_', $nodeName, 'TypeSupport.cc&#xA;')"/>
			</xsl:if>
			<xsl:if test="count($componentNodes) &gt; 0" >
				<xsl:value-of select="concat('    CoreDX_', $nodeName, 'C.cpp&#xA;')"/>
			</xsl:if>
		</xsl:when>
		<xsl:when test="$middleware = 'qpidpb'" >
			<xsl:if test="count($has_dds_events) &gt; 0" >
				<xsl:value-of select="concat('    ', $nodeName, '.pb.cc&#xA;')"/>
			</xsl:if>
			<xsl:if test="count($componentNodes) &gt; 0" >
				<xsl:value-of select="concat('    QpidPB_', $nodeName, 'C.cpp&#xA;')"/>
			</xsl:if>
		</xsl:when>
		<xsl:when test="$middleware = 'tcpip'" >
			<xsl:if test="count($componentNodes) &gt; 0" >
				<xsl:value-of select="concat('    TCPIP_', $nodeName, 'C.cpp&#xA;')"/>
			</xsl:if>
		</xsl:when>
		<xsl:otherwise>
			<xsl:value-of select="''" />
		</xsl:otherwise>
		</xsl:choose>
	</xsl:template>	
	
	<!-- generate_skel_project -->
	<xsl:template name="generate_skel_project">
		<xsl:param name="node" />
		<xsl:param name="middleware" />
		<xsl:param name="references" />	
		<xsl:param name="transformNodeKindKey" />
		<xsl:param name="transformNodeLabelKey" />
		
		<xsl:variable name="nodeName" select="$node/gml:data[@key=$transformNodeLabelKey]/text()" />
		<xsl:variable name="componentNodes" select="$node/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'Component']/..
												  | $node/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'BlackBox']/.." />
		
		<!-- Generate the export file for the project. -->
		<!-- make sure we have file name only, ie basename -->
		<xsl:variable name="delim" select="'/'" />
		<xsl:variable name="lastDelim" select="string-length($nodeName) - string-length(translate($nodeName, $delim, ''))" />
		<xsl:variable name="basename" >
			<xsl:call-template name="splitList">
				<xsl:with-param name="pText" select="$nodeName"/>
				<xsl:with-param name="count" select="$lastDelim" />
				<xsl:with-param name="idx" select="$lastDelim" />
				<xsl:with-param name="delim" select="$delim" />
			</xsl:call-template> 
		</xsl:variable>
		<xsl:variable name="skel_name" select="concat($nodeName, $skelSuffix)" />
 		<xsl:variable name="skel_export" select="translate($skel_name,
                                'abcdefghijklmnopqrstuvwxyz',
                                'ABCDEFGHIJKLMNOPQRSTUVWXYZ')" />
		<xsl:variable name="skel_export_macro" select="concat($skel_export, '_Export')" />
		<xsl:variable name="skel_export_file" select="concat($skel_name, '_export.h')" />
		
		<!-- Generate the project. -->
		<xsl:value-of select="concat('project (', $skel_name, ') : avoids_ace_for_tao, ')" />

		<xsl:variable name="skel_base" >
			<xsl:call-template name="skel_base_project">
				<xsl:with-param name="middleware" select="$middleware" />
			</xsl:call-template>	
		</xsl:variable>	
		<xsl:if test="not(skel_base = '')"> 
			<xsl:value-of select="concat( $skel_base, ', ')" />
		</xsl:if>

		<xsl:value-of select="'cuts_codegen_defaults {&#xA;'"/>
		<xsl:value-of select="concat('  sharedname   = ', $skel_name, '&#xA;')"/>
		<xsl:value-of select="concat('  dynamicflags += ', $skel_export, '_BUILD_DLL&#xA;&#xA;')"/>
		<xsl:value-of select="concat('  after       += ', $nodeName, $stubSuffix, '&#xA;')"/>
		<xsl:value-of select="concat('  libs        += ', $nodeName, $stubSuffix, '&#xA;&#xA;')"/>

		<xsl:if test="count($references) &gt; 0">
			<!-- Generate the dependencies for this project. This will be all the client projects of the references for this node. -->
			<xsl:value-of select="'&#xA;  after +='" />
			<xsl:for-each select="$references">
				<xsl:call-template name="generate_listing">
					<xsl:with-param name="node" select="." />
					<xsl:with-param name="suffix" select="$skelSuffix" />
					<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey"/>
					<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey"/>
				</xsl:call-template>	
			</xsl:for-each>
			<!-- Generate the import libraries for this project. This will be all client project of the references for this node. 
				The will also be the same as those specified in the "after" tag. -->
			<xsl:value-of select="'&#xA;&#xA;  libs  +='"/>
			<xsl:for-each select="$references">
				<xsl:call-template name="generate_listing">
					<xsl:with-param name="node" select="." />
					<xsl:with-param name="suffix" select="$skelSuffix" />
					<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey"/>
					<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey"/>
				</xsl:call-template>	
			</xsl:for-each>
			<xsl:for-each select="$references">
				<xsl:call-template name="generate_listing">
					<xsl:with-param name="node" select="." />
					<xsl:with-param name="suffix" select="$stubSuffix" />
					<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey"/>
					<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey"/>
				</xsl:call-template>	
			</xsl:for-each>
			<xsl:value-of select="'&#xA;'" />
		</xsl:if>
		<xsl:value-of select="'&#xA;'"/>

		<!-- Generate the IDL files for this project. -->
		<xsl:value-of select="'  IDL_Files {&#xA;&#xA;'" />
		<xsl:value-of select="'  }&#xA;&#xA;'" />
		<!-- Generate the source files for this project. -->
		<xsl:value-of select="'  Source_Files {&#xA;'" />
		<xsl:value-of select="concat('    ', $nodeName, 'S.cpp&#xA;')" />
		<xsl:value-of select="'  }&#xA;&#xA;'" />
		<!-- Generate the header files for this project. -->
		<xsl:value-of select="'  Header_Files {&#xA;'" />
		<xsl:value-of select="concat('    ', $nodeName, 'S.h&#xA;')" />
		<xsl:value-of select="'  }&#xA;&#xA;'" />
		<xsl:value-of select="'  Inline_Files {&#xA;'" />
		<xsl:value-of select="concat('    ', $nodeName, 'S.inl&#xA;')" />
		<xsl:value-of select="'  }&#xA;'" />
		<xsl:value-of select="'}&#xA;&#xA;'" />

	</xsl:template>	

	<!-- Traits skel_base_project -->
	<xsl:template name="skel_base_project">
		<xsl:param name="middleware" />	
		
		<xsl:choose>
		<xsl:when test="$middleware = 'tao'" >
			<xsl:value-of select="'iccm_tao_skel'" />
		</xsl:when>
		<xsl:when test="$middleware = 'opensplice'" >
			<xsl:value-of select="'iccm_opensplice_skel'" />
		</xsl:when>
		<xsl:when test="$middleware = 'rtidds'" >
			<xsl:value-of select="'iccm_rtidds_skel'" />
		</xsl:when>
		<xsl:when test="$middleware = 'coredx'" >
			<xsl:value-of select="'iccm_coredx_skel'" />
		</xsl:when>
		<xsl:when test="$middleware = 'qpidpb'" >
			<xsl:value-of select="'iccm_qpidpb_skel'" />
		</xsl:when>
		<xsl:when test="$middleware = 'tcpip'" >
			<xsl:value-of select="'iccm_tcpip_skel'" />
		</xsl:when>
		<xsl:otherwise>
			<xsl:value-of select="''" />
		</xsl:otherwise>
		</xsl:choose>
	</xsl:template>	
	
	<!-- generate_eidl_project -->
	<xsl:template name="generate_eidl_project">
		<xsl:param name="node" />
		<xsl:param name="middleware" />
		<xsl:param name="references" />	
		<xsl:param name="transformNodeKindKey" />
		<xsl:param name="transformNodeLabelKey" />
		
		<!-- node.file_.name () -->
		<xsl:variable name="nodeName" select="$node/gml:data[@key=$transformNodeLabelKey]/text()" />
		<xsl:variable name="componentNodes" select="$node/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'Component']/..
												  | $node/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'BlackBox']/.." />
 		<xsl:variable name="export_basename" select="translate($nodeName,
                                'abcdefghijklmnopqrstuvwxyz',
                                'ABCDEFGHIJKLMNOPQRSTUVWXYZ')" />

		<xsl:value-of select="concat('project (', $nodeName, $eidlSuffix, ') : ciaoidldefaults, cuts_codegen_defaults {&#xA;')" />
		<xsl:value-of select="'  custom_only = 1&#xA;&#xA;'" />
		<xsl:value-of select="concat('  after    += ', $nodeName, '_IDL_Gen ', $nodeName, '_iCCM_IDL_Gen')" />
		<xsl:for-each select="$references">
			<xsl:call-template name="generate_listing">
				<xsl:with-param name="node" select="." />
				<xsl:with-param name="suffix" select="$eidlSuffix" />
				<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey"/>
				<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey"/>
			</xsl:call-template>	
		</xsl:for-each>
		<xsl:value-of select="'&#xA;&#xA;'"/>
		<xsl:value-of select="concat('  idlflags += -Wb,export_macro=', $export_basename, '_EXEC_Export \&#xA;')" />
		<xsl:value-of select="concat('              -Wb,export_include=', $nodeName, '_exec_export.h \&#xA;')" />
		<xsl:value-of select="'              -Sa -Sal -SS&#xA;&#xA;'" />
		<xsl:value-of select="'  IDL_Files {&#xA;'" />
		<xsl:value-of select="concat('    ', $nodeName, 'E.idl&#xA;')" />
		<!-- Traits write_exec_idl_files, this produces the same text for all middlewares  -->
		<xsl:if test="count($componentNodes) &gt; 0" >
			<xsl:value-of select="concat('    ', $nodeName, '_iCCM.idl&#xA;')"/>
		</xsl:if>
		<xsl:value-of select="'  }&#xA;'" />
		<xsl:value-of select="'}&#xA;&#xA;'" />
	</xsl:template>	

	<!-- generate_exec_project -->
	<xsl:template name="generate_exec_project">
		<xsl:param name="node" />
		<xsl:param name="middleware" />
		<xsl:param name="references" />	
		<xsl:param name="transformNodeKindKey" />
		<xsl:param name="transformNodeLabelKey" />
		
		<xsl:variable name="nodeName" select="$node/gml:data[@key=$transformNodeLabelKey]/text()" />
		<xsl:variable name="project_name" select="concat($nodeName, $execSuffix)" />
		<xsl:variable name="macro_basename" select="translate($nodeName,
                                'abcdefghijklmnopqrstuvwxyz',
                                'ABCDEFGHIJKLMNOPQRSTUVWXYZ')" />

		<xsl:variable name="componentNodes" select="$node/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'Component']/..
												  | $node/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'BlackBox']/.." />
		
		<xsl:value-of select="concat('project (', $project_name, ') : ')" />
		<xsl:variable name="skel_base" >
			<xsl:call-template name="skel_base_project">
				<xsl:with-param name="middleware" select="$middleware" />
			</xsl:call-template>	
		</xsl:variable>	
		<xsl:if test="not(skel_base = '')"> 
			<xsl:value-of select="concat( $skel_base, ', ')" />
		</xsl:if>
		<xsl:value-of select="'cuts_codegen_defaults {&#xA;'" />
		<xsl:value-of select="concat('  sharedname    = ', $project_name, '&#xA;')" />
		<xsl:value-of select="concat('  dynamicflags += ', $macro_basename, '_EXEC_BUILD_DLL&#xA;&#xA;')" />
		<xsl:value-of select="concat('  after += ', $nodeName, $eidlSuffix, ' \&#xA;')" />
		<xsl:value-of select="concat('    ', $nodeName, $stubSuffix)" />
		<xsl:for-each select="$references">
			<xsl:call-template name="generate_listing">
				<xsl:with-param name="node" select="." />
				<xsl:with-param name="suffix" select="$stubSuffix" />
				<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey"/>
				<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey"/>
			</xsl:call-template>	
		</xsl:for-each>
		<xsl:for-each select="$references">
			<xsl:call-template name="generate_listing">
				<xsl:with-param name="node" select="." />
				<xsl:with-param name="suffix" select="$execSuffix" />
				<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey"/>
				<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey"/>
			</xsl:call-template>	
		</xsl:for-each>
		<xsl:value-of select="'&#xA;&#xA;'"/>
		<xsl:value-of select="concat('  libs  += ', $nodeName, $stubSuffix)" />
		<xsl:for-each select="$references">
			<xsl:call-template name="generate_listing">
				<xsl:with-param name="node" select="." />
				<xsl:with-param name="suffix" select="$stubSuffix" />
				<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey"/>
				<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey"/>
			</xsl:call-template>	
		</xsl:for-each>
		<xsl:for-each select="$references">
			<xsl:call-template name="generate_listing">
				<xsl:with-param name="node" select="." />
				<xsl:with-param name="suffix" select="$execSuffix" />
				<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey"/>
				<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey"/>
			</xsl:call-template>	
		</xsl:for-each>
		<xsl:value-of select="'&#xA;&#xA;'"/>
		<xsl:value-of select="'  Source_Files {&#xA;'"/>
		<xsl:value-of select="concat('    ', $nodeName, 'EC.cpp&#xA;')"/>
		
		<!-- Traits write_exec_source_files, this produces the same text for all middlewares  -->
		<xsl:if test="count($componentNodes) &gt; 0" >
			<xsl:value-of select="concat('    ', $nodeName, '_iCCMC.cpp&#xA;')"/>
		</xsl:if>
		<xsl:value-of select="'  }&#xA;&#xA;'"/>
		<xsl:value-of select="'  Inline_Files {&#xA;'"/>
		<xsl:value-of select="'  }&#xA;&#xA;'"/>
		<xsl:value-of select="'  Header_Files {&#xA;'"/>
		<xsl:value-of select="concat('    ', $nodeName, 'EC.h&#xA;')"/>
		<xsl:value-of select="'  }&#xA;&#xA;'"/>
		<xsl:value-of select="'  IDL_Files {&#xA;'"/>
		<xsl:value-of select="'  }&#xA;'"/>
		<xsl:value-of select="'}&#xA;&#xA;'"/>
	</xsl:template>	
	
	<!-- generate_svnt_project -->
	<xsl:template name="generate_svnt_project">
		<xsl:param name="node" />
		<xsl:param name="middleware" />
		<xsl:param name="references" />	
		<xsl:param name="transformNodeKindKey" />
		<xsl:param name="transformNodeLabelKey" />
		
		<xsl:variable name="nodeName" select="$node/gml:data[@key=$transformNodeLabelKey]/text()" />
		<xsl:variable name="svnt_project" select="concat($nodeName, $svntSuffix)" />
		<xsl:variable name="export_basename" select="translate($svnt_project,
                                'abcdefghijklmnopqrstuvwxyz',
                                'ABCDEFGHIJKLMNOPQRSTUVWXYZ')" />

		<xsl:variable name="componentNodes" select="$node/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'Component']/..
												  | $node/descendant::*/gml:node/gml:data[@key=$transformNodeKindKey][text() = 'BlackBox']/.." />

		<xsl:value-of select="concat('project (', $svnt_project, ') : ')" />
		<xsl:call-template name="svnt_base_project">
			<xsl:with-param name="middleware" select="$middleware" />
		</xsl:call-template>	
		<xsl:value-of select="', cuts_codegen_defaults'"/>
		<xsl:value-of select="' {&#xA;'"/>
		<xsl:value-of select="concat('  sharedname   = ', $svnt_project, '&#xA;')"/>
		<xsl:value-of select="concat('  dynamicflags += ', $export_basename, '_BUILD_DLL&#xA;&#xA;')"/>
		<xsl:value-of select="'  after += '" />
		<xsl:value-of select="concat($nodeName, $execSuffix, ' ')"/>
		<xsl:value-of select="concat($nodeName, $stubSuffix, ' ')"/>
		<xsl:value-of select="concat($nodeName, $skelSuffix)"/>
		
		<!-- Filter the nodes correctly. -->
		<xsl:for-each select="$references">
			<xsl:call-template name="generate_listing">
				<xsl:with-param name="node" select="." />
				<xsl:with-param name="suffix" select="$stubSuffix" />
				<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey"/>
				<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey"/>
			</xsl:call-template>	
		</xsl:for-each>
		<xsl:for-each select="$references">
			<xsl:call-template name="generate_listing">
				<xsl:with-param name="node" select="." />
				<xsl:with-param name="suffix" select="$skelSuffix" />
				<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey"/>
				<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey"/>
			</xsl:call-template>	
		</xsl:for-each>
		<xsl:for-each select="$references">
			<xsl:call-template name="generate_listing">
				<xsl:with-param name="node" select="." />
				<xsl:with-param name="suffix" select="$execSuffix" />
				<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey"/>
				<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey"/>
			</xsl:call-template>	
		</xsl:for-each>
		<xsl:value-of select="'&#xA;&#xA;'"/>
		<!-- iterate over references once more. This time we are generating the libs declaration. -->
		<xsl:value-of select="'  libs += '" />
		<xsl:value-of select="concat($nodeName, $execSuffix, ' ')"/>
		<xsl:value-of select="concat($nodeName, $stubSuffix, ' ')"/>
		<xsl:value-of select="concat($nodeName, $skelSuffix)"/>
		<!-- Filter the nodes correctly. -->
		<xsl:for-each select="$references">
			<xsl:call-template name="generate_listing">
				<xsl:with-param name="node" select="." />
				<xsl:with-param name="suffix" select="$stubSuffix" />
				<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey"/>
				<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey"/>
			</xsl:call-template>	
		</xsl:for-each>
		<xsl:for-each select="$references">
			<xsl:call-template name="generate_listing">
				<xsl:with-param name="node" select="." />
				<xsl:with-param name="suffix" select="$skelSuffix" />
				<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey"/>
				<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey"/>
			</xsl:call-template>	
		</xsl:for-each>
		<xsl:for-each select="$references">
			<xsl:call-template name="generate_listing">
				<xsl:with-param name="node" select="." />
				<xsl:with-param name="suffix" select="$execSuffix" />
				<xsl:with-param name="transformNodeLabelKey" select="$transformNodeLabelKey"/>
				<xsl:with-param name="transformNodeKindKey" select="$transformNodeKindKey"/>
			</xsl:call-template>	
		</xsl:for-each>
		<xsl:value-of select="'&#xA;&#xA;'"/>
		
		<!-- Generate the source files -->
		<xsl:value-of select="'  Source_Files {&#xA;'"/>
		<xsl:value-of select="concat('    ', $nodeName, '_svnt.cpp&#xA;')"/>
		<xsl:value-of select="'  }&#xA;&#xA;'"/>
		<!-- Generate the header files -->
		<xsl:value-of select="'  Header_Files {&#xA;'"/>
		<xsl:value-of select="concat('    ', $nodeName, '_svnt.h&#xA;')"/>
		<xsl:value-of select="'  }&#xA;&#xA;'"/>
		<xsl:value-of select="'  Inline_Files {&#xA;&#xA;'"/>
		<xsl:value-of select="'  }&#xA;'"/>
		<xsl:value-of select="'  IDL_Files {&#xA;&#xA;'"/>
		<xsl:value-of select="'  }&#xA;'"/>
		<xsl:value-of select="'}&#xA;&#xA;'"/>
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
		<xsl:when test="$middleware = 'coredx'" >
			<xsl:value-of select="'iccm_coredx_svnt'" />
		</xsl:when>
		<xsl:when test="$middleware = 'qpidpb'" >
			<xsl:value-of select="'iccm_qpidpb_svnt'" />
		</xsl:when>
		<xsl:when test="$middleware = 'tcpip'" >
			<xsl:value-of select="'iccm_tcpip_svnt'" />
		</xsl:when>
		<xsl:otherwise>
			<xsl:value-of select="''" />
		</xsl:otherwise>
		</xsl:choose>
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