<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" >

    <!--
        Purpose:

        This template contains all the key attributes to define
		data keys for the graphml representation of the model
		
		Not used as yet, may need to use document("graphmlKeyAttributes.xsl") rather than <xsl:include href="graphmlKeyAttributes.xsl" />
   -->
	<!-- Assign id's for data keys -->
	<!--    convention is to have id="d0..." but yEd may reassign, see Properties Mapper... -->

	<!-- Default yEd data keys
	<key for="graphml" id="{$resourcesKey}" yfiles.type="resources"/>
	<key for="port" id="{$portGraphicsKey}" yfiles.type="portgraphics"/>
	<key for="port" id="{$portGeometryKey}" yfiles.type="portgeometry"/>
	<key for="port" id="{$portUserDataKey}" yfiles.type="portuserdata"/>  -->
	
	<!-- Default yEd data keys -->
	<key attr.name="url" attr.type="string" for="node" id="{$nodeURLKey}"/>
	<key attr.name="description" attr.type="string" for="node" id="{$nodeDescriptionKey}"/>
	<key attr.name="Description" attr.type="string" for="graph" id="{$graphDescriptionKey}"/>
	<key attr.name="url" attr.type="string" for="edge" id="{$edgeURLKey}"/>
	<key attr.name="description" attr.type="string" for="edge" id="{$edgeDescriptionKey}"/>
	
	<!-- Specific yEd data keys for graphics 
	<key for="node" id="{$nodeGraphicsKey}" yfiles.type="nodegraphics"/>
	<key for="edge" id="{$edgeGraphicsKey}" yfiles.type="edgegraphics"/> -->
	
	<!-- PICML specific attributes -->
	<key attr.name="label" attr.type="string" for="node" id="{$nodeLabelKey}"/>
	<key attr.name="x" attr.type="double" for="node" id="{$nodeXKey}"/>
	<key attr.name="y" attr.type="double" for="node" id="{$nodeYKey}"/>
	<key attr.name="kind" attr.type="string" for="node" id="{$nodeKindKey}"/>
	<key attr.name="type" attr.type="string" for="node" id="{$nodeTypeKey}"/>
	<key attr.name="value" attr.type="string" for="node" id="{$nodeValueKey}"/>
	<key attr.name="key" attr.type="boolean" for="node" id="{$nodeKeyMemberKey}"/>
	<key attr.name="middleware" attr.type="string" for="node" id="{$nodeMiddlewareKey}"/>
	<key attr.name="topicName" attr.type="string" for="node" id="{$nodeTopicNameKey}"/>
	<key attr.name="async" attr.type="boolean" for="node" id="{$nodeAsyncKey}"/>
	<key attr.name="stream" attr.type="boolean" for="node" id="{$nodeStreamKey}"/>

	<key attr.name="folder" attr.type="string" for="node" id="{$nodeFolderKey}"/>
	<key attr.name="file" attr.type="string" for="node" id="{$nodeFileKey}"/>
	<key attr.name="project" attr.type="string" for="node" id="{$nodeProjectKey}"/>
	<key attr.name="container" attr.type="string" for="node" id="{$nodeContainerKey}"/>
	<key attr.name="worker" attr.type="string" for="node" id="{$nodeWorkerKey}"/>
	<key attr.name="operation" attr.type="string" for="node" id="{$nodeOperationKey}"/>
	<key attr.name="complexity" attr.type="string" for="node" id="{$nodeComplexityKey}"/>
	<key attr.name="parameters" attr.type="string" for="node" id="{$nodeParametersKey}"/>
	<key attr.name="script" attr.type="string" for="node" id="{$nodeScriptKey}"/>

	<key attr.name="width" attr.type="double" for="node" id="{$nodeWidthKey}"/>
	<key attr.name="height" attr.type="double" for="node" id="{$nodeHeightKey}"/>
			
</xsl:stylesheet>