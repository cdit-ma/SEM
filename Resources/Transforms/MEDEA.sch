<?xml version="1.0" encoding="utf-8"?>
<schema    
  xmlns="http://purl.oclc.org/dsdl/schematron"  
  xmlns:iso="http://purl.oclc.org/dsdl/schematron" 
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:gml="http://graphml.graphdrawing.org/xmlns"
  xmlns:xalan="http://xml.apache.org/xalan"
  queryBinding='xslt'
  schemaVersion='ISO19757-3'
  defaultPhase='#ALL'
  >                  

  <title>ISO schematron file for MEDEA graphml format</title>
  
  <!-- Define namespace used for this schematron -->
  <!-- dependant on xalan as we use xalan-c as the xslt processor, ie using xalan:nodeset() function -->
  <ns prefix='gml' uri='http://graphml.graphdrawing.org/xmlns'/> 
  <ns prefix='xalan' uri='http://xml.apache.org/xalan'/> 

  <!-- Define global parameters for transform -->
  <let name="selectID" value="'#ALL'"/>
  <let name="nodeKindKey" value="/gml:graphml/gml:key[@attr.name='kind']/@id"/>
  <let name="nodeLabelKey" value="/gml:graphml/gml:key[@attr.name='label']/@id"/>
  <let name="nodeTypeKey" value="/gml:graphml/gml:key[@attr.name='type']/@id"/>
  <let name="nodeValueKey" value="/gml:graphml/gml:key[@attr.name='value']/@id"/>
  <let name="nodeKeyMemberKey" value="/gml:graphml/gml:key[@attr.name='key']/@id"/>
  <let name="nodeMiddlewareKey" value="/gml:graphml/gml:key[@attr.name='middleware']/@id"/>
  
  <!--  <iso:include href='Predefined.sch' /> cannot use include as this uses the xalan-c unsupported function document() -->
  <let name="PredefinedMemberTypes">
    <Predefined nodeKind="Member" keyName="type" keyType="#ALL">
		<value>Boolean</value>
		<value>Byte</value>		
		<value>Char</value>
		<value>WideChar</value>
		<value>ShortInteger</value>
		<value>LongInteger</value>
		<value>LongLongInteger</value> 
		<value>UnsignedShortInteger</value>
		<value>UnsignedLongInteger</value>
		<value>UnsignedLongLongInteger</value>
		<value>FloatNumber</value>
		<value>DoubleNumber</value> 
		<value>LongDoubleNumber</value>
		<value>String</value>
		<value>WideString</value>
	</Predefined>
  </let>
  <let name="PredefinedAttributeTypes">
    <Predefined nodeKind="Attribute" keyName="type" keyType="#ALL">
		<value>Boolean</value>
		<value>Byte</value>		
		<value>Char</value>
		<value>WideChar</value>
		<value>ShortInteger</value>
		<value>LongInteger</value>
		<value>LongLongInteger</value> 
		<value>UnsignedShortInteger</value>
		<value>UnsignedLongInteger</value>
		<value>UnsignedLongLongInteger</value>
		<value>FloatNumber</value>
		<value>DoubleNumber</value> 
		<value>LongDoubleNumber</value>
		<value>String</value>
		<value>WideString</value>
	</Predefined>
  </let>
    <let name="PredefinedAttributeInstanceTypes">
    <Predefined nodeKind="AttributeInstance" keyName="type" keyType="#ALL">
		<value>Boolean</value>
		<value>Byte</value>		
		<value>Char</value>
		<value>WideChar</value>
		<value>ShortInteger</value>
		<value>LongInteger</value>
		<value>LongLongInteger</value> 
		<value>UnsignedShortInteger</value>
		<value>UnsignedLongInteger</value>
		<value>UnsignedLongLongInteger</value>
		<value>FloatNumber</value>
		<value>DoubleNumber</value> 
		<value>LongDoubleNumber</value>
		<value>String</value>
		<value>WideString</value>
	</Predefined>
  </let>
  <let name="PredefinedBooleanValues">
	<Predefined nodeKind="#ALL" keyName="#ALL" keyType="boolean">
		<value>true</value>
		<value>false</value>		
	</Predefined>
  </let>
  <let name="PredefinedModelMiddleware">
	<Predefined nodeKind="Model" keyName="middleware" keyType="#ALL">
		<value>tao</value>
		<value>rtidds</value>		
		<value>opensplice</value>
		<value>coredx</value>
		<value>tcpip</value>
		<value>qpidpb</value>
	</Predefined>	
  </let>
  <let name="PredefinedProcessActionOnValues">
	<Predefined nodeKind="Process" keyName="actionOn" keyType="#ALL">
		<value>Activate</value>
		<value>Configuration_complete</value>
		<value>Preprocess</value>		
		<value>Mainprocess</value>
		<value>Postprocess</value>
		<value>Passivate</value>
		<value>Remove</value>
	</Predefined>	
  </let>
  <let name="PredefinedIDLReservedWords">
	<Predefined nodeKind="#ALL" keyName="label" keyType="#ALL">
		<value>abstract</value>
		<value>any</value>
		<value>attribute</value>		
		<value>boolean</value>
		<value>case</value>
		<value>char</value>
		<value>component</value>
		<value>const</value>
		<value>consumes</value>
		<value>context</value>
		<value>custom</value>
		<value>default</value>
		<value>double</value>
		<value>exception</value>
		<value>emits</value>
		<value>enum</value>
		<value>eventtype</value>
		<value>factory</value>
		<value>false</value>
		<value>finder</value>
		<value>fixed</value>
		<value>float</value>
		<value>getraises</value>
		<value>home</value>
		<value>import</value>
		<value>in</value>
		<value>inout</value>
		<value>interface</value>
		<value>local</value>
		<value>long</value>
		<value>module</value>
		<value>multiple</value>
		<value>native</value>
		<value>object</value>
		<value>octet</value>
		<value>oneway</value>
		<value>out</value>
		<value>primarykey</value>
		<value>private</value>
		<value>provides</value>
		<value>public</value>
		<value>publishes</value>
		<value>raises</value>
		<value>readonly</value>
		<value>setraises</value>
		<value>sequence</value>
		<value>short</value>
		<value>string</value>
		<value>struct</value>
		<value>supports</value>
		<value>switch</value>
		<value>true</value>
		<value>truncatable</value>
		<value>typedef</value>
		<value>typeid</value>
		<value>typeprefix</value>
		<value>unsigned</value>
		<value>union</value>
		<value>uses</value>
		<value>valuebase</value>
		<value>valuetype</value>
		<value>void</value>
		<value>wchar</value>
		<value>wstring</value>
	</Predefined>	
  </let>
  
  <!-- Define the keys for testing unique labels in the model -->
  <!-- This works, <xsl:key name="fileLabels" match="gml:node[gml:data[@key='d14']/text()='IDL']" use="gml:data[@key='d11']"/> --> 
  <!-- but this is broken <xsl:key name="fileLabels" match="gml:node[gml:data[@key='{$nodeKindKey}']/text()='IDL']" use="gml:data[@key='{$nodeLabelKey}']"/> -->
  <!-- Create key index based on any data for the node containing the kind we want. Need to do better selection in assert rule -->
  <xsl:key name="fileLabels" match="gml:node[gml:data/text()='IDL']" use="gml:data"/>
  <xsl:key name="componentLabels" match="gml:node[gml:data/text()='Component' or gml:data/text()='BlackBox']" use="gml:data"/>
  
  <!-- Define one phase for each kind of node in graphml, and each definition view -->
  <phase id="InterfaceDefinitions">
	<active pattern="graphml.checks"/>
	<active pattern="IDL.checks"/>
	<active pattern="Component.checks"/>
	<active pattern="BlackBox.checks"/>
	<active pattern="InEventPort.checks"/>
	<active pattern="OutEventPort.checks"/>
	<active pattern="Attribute.checks"/>
	<active pattern="Aggregate.checks"/>
	<active pattern="Aggregate.AggregateInstance.checks"/>
	<active pattern="Member.checks"/>
  </phase> 
  <phase id="IDL">
	<active pattern="graphml.checks"/>
	<active pattern="IDL.checks"/>
  </phase>
  <phase id="Component">
	<active pattern="graphml.checks"/>
	<active pattern="Component.checks"/>
  </phase>
  <phase id="BlackBox">
	<active pattern="graphml.checks"/>
	<active pattern="BlackBox.checks"/>
  </phase>
  <phase id="InEventPort">
	<active pattern="graphml.checks"/>
	<active pattern="InEventPort.checks"/>
  </phase> 
  <phase id="OutEventPort">
	<active pattern="graphml.checks"/>
	<active pattern="OutEventPort.checks"/>
  </phase> 
  <phase id="Attribute">
	<active pattern="graphml.checks"/>
	<active pattern="Attribute.checks"/>
  </phase>  
  <phase id="Aggregate">
	<active pattern="graphml.checks"/>
	<active pattern="Aggregate.checks"/>
  </phase>  
  <phase id="Aggregate.AggregateInstance">
	<active pattern="graphml.checks"/>
	<active pattern="Aggregate.AggregateInstance.checks"/>
  </phase> 
  <phase id="Member">
	<active pattern="graphml.checks"/>
	<active pattern="Member.checks"/>
  </phase> 

  <phase id="AssemblyDefinitions">
	<active pattern="graphml.checks"/>
	<active pattern="ComponentAssembly.checks"/>
	<active pattern="ComponentInstance.checks"/>
	<active pattern="BlackBoxInstance.checks"/>
	<active pattern="InEventPortInstance.checks"/>
	<active pattern="OutEventPortInstance.checks"/>
	<active pattern="AttributeInstance.checks"/>
  </phase> 
  <phase id="ComponentAssembly">
	<active pattern="graphml.checks"/>
	<active pattern="ComponentAssembly.checks"/>
  </phase> 
  <phase id="ComponentInstance">
	<active pattern="graphml.checks"/>
	<active pattern="ComponentInstance.checks"/>
  </phase> 
    <phase id="BlackBoxInstance">
	<active pattern="graphml.checks"/>
	<active pattern="BlackBoxInstance.checks"/>
  </phase> 
  <phase id="InEventPortInstance">
	<active pattern="graphml.checks"/>
	<active pattern="InEventPortInstance.checks"/>
  </phase> 
  <phase id="OutEventPortInstance">
	<active pattern="graphml.checks"/>
	<active pattern="OutEventPortInstance.checks"/>
  </phase>   
  <phase id="AttributeInstance">
	<active pattern="graphml.checks"/>
	<active pattern="AttributeInstance.checks"/>
  </phase>   
  <phase id="InEventPortDelegate">
	<active pattern="graphml.checks"/>
	<active pattern="InEventPortDelegate.checks"/>
  </phase> 
  <phase id="OutEventPortDelegate">
	<active pattern="graphml.checks"/>
	<active pattern="OutEventPortDelegate.checks"/>
  </phase>  
  
  <!-- Global graphml checks -->            
  <pattern id="graphml.checks">
	<title>Checking Model graphml</title>
	<rule context="gml:graphml/gml:graph/gml:node/gml:data[@key=$nodeKindKey][text()='Model']">
		<let name="node" value="parent::gml:node" />
		<let name="label" value="$node/gml:data[@key=$nodeLabelKey]/text()" />
		<let name="middleware" value="$node/gml:data[@key=$nodeMiddlewareKey]/text()" />
		<report test="$node" 
			role="information">[<value-of select="$node/@id"/>] <value-of select="$label"/> Model Validation Report</report>
		<let name="middlewareList">
			<axsl:for-each select="xalan:nodeset($PredefinedModelMiddleware)/node()[@nodeKind='Model'][@keyName='middleware']//iso:value" xmlns:axsl="http://www.w3.org/1999/XSL/Transform">
				<axsl:value-of select="concat(', ', .)"  xmlns:axsl="http://www.w3.org/1999/XSL/Transform"/>
			</axsl:for-each>
		</let>
		<assert test="xalan:nodeset($PredefinedModelMiddleware)/node()[@nodeKind='Model'][@keyName='middleware']//iso:value[text()=$middleware]"
			role="critical">[<value-of select="$node/@id"/>] <value-of select="$label"/> Model must have one of the following middlewares<value-of select="$middlewareList"/></assert> 
	</rule>
  </pattern>

  <!-- Specific node kind checks -->
  <pattern id="IDL.checks">
	<title>Checking IDL entities</title>
	<rule context="gml:node[$selectID='#ALL' or @id=$selectID]/gml:data[@key=$nodeKindKey][text()='IDL']">
		<let name="node" value="parent::gml:node" />
		<let name="label" value="$node/gml:data[@key=$nodeLabelKey]/text()" />
		<report test="$node"
			role="information">[<value-of select="$node/@id"/>] <value-of select="$label"/> IDL Validation Rules</report>
		<assert test="string-length(normalize-space(translate($label,'\/:*?&quot;&gt;&lt;|',''))) = string-length($label)" 
			role="critical">[<value-of select="$node/@id"/>] <value-of select="$label"/> IDL label must not contain \/:*?&quot;&gt;&lt;| or space characters</assert>
		<assert test="count(key('fileLabels', $label)[gml:data[@key=$nodeKindKey]/text() = 'IDL'][gml:data[@key=$nodeLabelKey]/text() = $label]) = 1"
			role="critical">[<value-of select="$node/@id"/>] <value-of select="$label"/> IDL label must be unique within Model</assert>
		<assert test="count($node/gml:graph/gml:node/gml:data[@key=$nodeKindKey][text()='Component']) &gt; 0
					  or count($node/gml:graph/gml:node/gml:data[@key=$nodeKindKey][text()='BlackBox']) &gt; 0
					  or count($node/gml:graph/gml:node/gml:data[@key=$nodeKindKey][text()='Aggregate']) &gt; 0" 
			role="warning">[<value-of select="$node/@id"/>] <value-of select="$label"/> IDL should contain either Component, BlackBox or Aggregate entities</assert>
		<assert test="count($node/gml:graph/gml:node/gml:data[@key=$nodeKindKey][text()='Aggregate']) = 0
					  or (count($node/gml:graph/gml:node/gml:data[@key=$nodeKindKey][text()='Aggregate']) > 0 
				      and count($node/gml:graph/gml:node/gml:data[@key=$nodeKindKey][text()!='Aggregate' and text()!='Vector']) = 0)" 
			role="critical">[<value-of select="$node/@id"/>] <value-of select="$label"/> IDL with Aggregate entities can only contain other Aggregate or Vector entities</assert> 
		<assert test="count($node/gml:graph/gml:node/gml:data[@key=$nodeKindKey][text()='Vector']) = 0
					  or (count($node/gml:graph/gml:node/gml:data[@key=$nodeKindKey][text()='Vector']) > 0 
				      and count($node/gml:graph/gml:node/gml:data[@key=$nodeKindKey][text()!='Aggregate' and text()!='Vector']) = 0)" 
			role="critical">[<value-of select="$node/@id"/>] <value-of select="$label"/> IDL with Aggregate entities can only contain other Aggregate or Vector entities</assert> 
		<assert test="count($node/gml:graph/gml:node/gml:data[@key=$nodeKindKey][text()='Component']) = 0
					  or (count($node/gml:graph/gml:node/gml:data[@key=$nodeKindKey][text()='Component']) > 0
				      and count($node/gml:graph/gml:node/gml:data[@key=$nodeKindKey][text()!='Component']) = 0)" 
			role="critical">[<value-of select="$node/@id"/>] <value-of select="$label"/> IDL with Component entities can only contain other Component entities</assert>
		<assert test="count($node/gml:graph/gml:node/gml:data[@key=$nodeKindKey][text()='BlackBox']) = 0
					  or (count($node/gml:graph/gml:node/gml:data[@key=$nodeKindKey][text()='BlackBox']) > 0
				      and count($node/gml:graph/gml:node/gml:data[@key=$nodeKindKey][text()!='BlackBox']) = 0)" 
			role="critical">[<value-of select="$node/@id"/>] <value-of select="$label"/> IDL with BlackBox entities can only contain other BlackBox entities</assert>
	</rule>
  </pattern>

  <pattern id="Component.checks">
	<title>Checking Component entities</title>
	<rule context="gml:node[$selectID='#ALL' or @id=$selectID]/gml:data[@key=$nodeKindKey][text()='Component']">
		<let name="node" value="parent::gml:node" />
		<let name="label" value="$node/gml:data[@key=$nodeLabelKey]/text()" />
		<let name="childNode" value="$node/gml:graph/gml:node" />
		<report test="$node"
			role="information">[<value-of select="$node/@id"/>] <value-of select="$label"/> Component Validation Rules</report>
		<assert test="string-length(normalize-space(translate($label,'\/:*?&quot;&gt;&lt;|',''))) = string-length($label)" 
			role="critical">[<value-of select="$node/@id"/>] <value-of select="$label"/> Component label must not contain \/:*?&quot;&gt;&lt;| or space characters</assert>
		<assert test="count(key('componentLabels', $label)[gml:data[@key=$nodeKindKey]/text() = 'Component' or gml:data[@key=$nodeKindKey]/text() = 'BlackBox'][gml:data[@key=$nodeLabelKey]/text() = $label]) = 1"
			role="critical">[<value-of select="$node/@id"/>] <value-of select="$label"/> Component label must be unique within Model</assert>

		<let name="reservedList">
			<axsl:for-each select="xalan:nodeset($PredefinedIDLReservedWords)/node()[@keyName='label']//iso:value" xmlns:axsl="http://www.w3.org/1999/XSL/Transform">
				<axsl:value-of select="concat(. , ', ')" xmlns:axsl="http://www.w3.org/1999/XSL/Transform"/>
			</axsl:for-each>
		</let>
		<assert test="not( contains( $reservedList, concat(translate($label,'ABCDEFGHIJKLMNOPQRSTUVWXYZ','abcdefghijklmnopqrstuvwxyz'), ', ') ) )"
			role="critical">[<value-of select="$node/@id"/>] <value-of select="$label"/> Component label must not be an IDL reserved word </assert> 
				
	    <assert test="count($childNode/gml:data[@key=$nodeKindKey][text()='InEventPort']) &gt; 0
					  or count($childNode/gml:data[@key=$nodeKindKey][text()='OutEventPort']) &gt; 0
					  or count($childNode/gml:data[@key=$nodeKindKey][text()='Attribute']) &gt; 0" 
			role="warning">[<value-of select="$node/@id"/>] <value-of select="$label"/> Component should contain InEventPort, OutEventPort and Attribute kind of entities</assert> 
		<assert test="count($childNode/gml:data[@key=$nodeKindKey][text()!='InEventPort' and text()!='OutEventPort' and text()!='Attribute']) = 0" 
			role="critical">[<value-of select="$node/@id"/>] <value-of select="$label"/> Component must contain only InEventPort, OutEventPort and Attribute kind of entities</assert> 
	</rule>
  </pattern>
  
  <pattern id="BlackBox.checks">
	<title>Checking BlackBox entities</title>
	<rule context="gml:node[$selectID='#ALL' or @id=$selectID]/gml:data[@key=$nodeKindKey][text()='BlackBox']">
		<let name="node" value="parent::gml:node" />
		<let name="label" value="$node/gml:data[@key=$nodeLabelKey]/text()" />
		<let name="childNode" value="$node/gml:graph/gml:node" />
		<report test="$node"
			role="information">[<value-of select="$node/@id"/>] <value-of select="$label"/> BlackBox Validation Rules</report>
		<assert test="string-length(normalize-space(translate($label,'\/:*?&quot;&gt;&lt;|',''))) = string-length($label)" 
			role="critical">[<value-of select="$node/@id"/>] <value-of select="$label"/> BlackBox label must not contain \/:*?&quot;&gt;&lt;| or space characters</assert>
		<assert test="count(key('componentLabels', $label)[gml:data[@key=$nodeKindKey]/text() = 'Component' or gml:data[@key=$nodeKindKey]/text() = 'BlackBox'][gml:data[@key=$nodeLabelKey]/text() = $label]) = 1"
			role="critical">[<value-of select="$node/@id"/>] <value-of select="$label"/> BlackBox label must be unique within Model</assert>
		<let name="reservedList">
			<axsl:for-each select="xalan:nodeset($PredefinedIDLReservedWords)/node()[@keyName='label']//iso:value" xmlns:axsl="http://www.w3.org/1999/XSL/Transform">
				<axsl:value-of select="concat(. , ', ')" xmlns:axsl="http://www.w3.org/1999/XSL/Transform"/>
			</axsl:for-each>
		</let>
		<assert test="not( contains( $reservedList, concat(translate($label,'ABCDEFGHIJKLMNOPQRSTUVWXYZ','abcdefghijklmnopqrstuvwxyz'), ', ') ) )"
			role="critical">[<value-of select="$node/@id"/>] <value-of select="$label"/> BlackBox label must not be an IDL reserved word </assert> 
		<assert test="count($childNode/gml:data[@key=$nodeKindKey][text()='InEventPort']) &gt; 0
					  or count($childNode/gml:data[@key=$nodeKindKey][text()='OutEventPort']) &gt; 0" 
			role="warning">[<value-of select="$node/@id"/>] <value-of select="$label"/> BlackBox should contain InEventPort and OutEventPort kind of entities</assert> 
		<assert test="count($childNode/gml:data[@key=$nodeKindKey][text()!='InEventPort' and text()!='OutEventPort']) = 0" 
			role="critical">[<value-of select="$node/@id"/>] <value-of select="$label"/> BlackBox must contain only InEventPort and OutEventPort kind of entities</assert> 
	</rule>
  </pattern>

  <pattern id="InEventPort.checks">
	<title>Checking InEventPort entities</title>
	<rule context="gml:node[$selectID='#ALL' or @id=$selectID]/gml:data[@key=$nodeKindKey][text()='InEventPort']">
		<let name="node" value="parent::gml:node" />
		<let name="label" value="$node/gml:data[@key=$nodeLabelKey]/text()" />
		<report test="$node"
			role="information">[<value-of select="$node/@id"/>] <value-of select="$label"/> InEventPort Validation Rules</report>
		<assert test="string-length(normalize-space(translate($label,'\/:*?&quot;&gt;&lt;|',''))) = string-length($label)" 
			role="critical">[<value-of select="$node/@id"/>] <value-of select="$label"/> InEventPort label must not contain \/:*?&quot;&gt;&lt;| or space characters</assert>
		<let name="reservedList">
			<axsl:for-each select="xalan:nodeset($PredefinedIDLReservedWords)/node()[@keyName='label']//iso:value" xmlns:axsl="http://www.w3.org/1999/XSL/Transform">
				<axsl:value-of select="concat(. , ', ')" xmlns:axsl="http://www.w3.org/1999/XSL/Transform"/>
			</axsl:for-each>
		</let>
		<assert test="not( contains( $reservedList, concat(translate($label,'ABCDEFGHIJKLMNOPQRSTUVWXYZ','abcdefghijklmnopqrstuvwxyz'), ', ') ) )"
			role="critical">[<value-of select="$node/@id"/>] <value-of select="$label"/> InEventPort label must not be an IDL reserved word </assert> 
		<assert test="count(//gml:edge[@source=$node/@id]) = 1" 
			role="critical">[<value-of select="$node/@id"/>] <value-of select="$label"/> InEventPort must be the source connection for one link to an Aggregate entity</assert> 
		<let name="aggregateID" value="//gml:edge[@source=$node/@id]/@target" />
	    <assert test="count(//gml:node[@id=$aggregateID]/gml:data[@key=$nodeKindKey][text()='Aggregate']) = 1" 
			role="critical">[<value-of select="$node/@id"/>] <value-of select="$label"/> InEventPort must be connected to an Aggregate entity</assert> 
	</rule>
  </pattern>
  
  <pattern id="OutEventPort.checks">
	<title>Checking OutEventPort entities</title>
	<rule context="gml:node[$selectID='#ALL' or @id=$selectID]/gml:data[@key=$nodeKindKey][text()='OutEventPort']">
		<let name="node" value="parent::gml:node" />
		<let name="label" value="$node/gml:data[@key=$nodeLabelKey]/text()" />
		<report test="$node"
			role="information">[<value-of select="$node/@id"/>] <value-of select="$label"/> OutEventPort Validation Rules</report>
		<assert test="string-length(normalize-space(translate($label,' \/:*?&quot;&gt;&lt;|',''))) = string-length($label)" 
			role="critical">[<value-of select="$node/@id"/>] <value-of select="$label"/> OutEventPort label must not contain \/:*?&quot;&gt;&lt;| or space characters</assert>
		<let name="reservedList">
			<axsl:for-each select="xalan:nodeset($PredefinedIDLReservedWords)/node()[@keyName='label']//iso:value" xmlns:axsl="http://www.w3.org/1999/XSL/Transform">
				<axsl:value-of select="concat(. , ', ')" xmlns:axsl="http://www.w3.org/1999/XSL/Transform"/>
			</axsl:for-each>
		</let>
		<assert test="not( contains( $reservedList, concat(translate($label,'ABCDEFGHIJKLMNOPQRSTUVWXYZ','abcdefghijklmnopqrstuvwxyz'), ', ') ) )"
			role="critical">[<value-of select="$node/@id"/>] <value-of select="$label"/> OutEventPort label must not be an IDL reserved word </assert> 
		<assert test="count(//gml:edge[@source=$node/@id]) = 1" 
			role="critical">[<value-of select="$node/@id"/>] <value-of select="$label"/> OutEventPort must be the source connection for one link to an Aggregate entity</assert> 
		<let name="aggregateID" value="//gml:edge[@source=$node/@id]/@target" />
	    <assert test="count(//gml:node[@id=$aggregateID]/gml:data[@key=$nodeKindKey][text()='Aggregate']) = 1" 
			role="critical">[<value-of select="$node/@id"/>] <value-of select="$label"/> OutEventPort must be connected to an Aggregate entity</assert> 
	</rule>
  </pattern>

  <pattern id="Attribute.checks">
	<title>Checking Attribute entities</title>
	<rule context="gml:node[$selectID='#ALL' or @id=$selectID]/gml:data[@key=$nodeKindKey][text()='Attribute']">
		<let name="node" value="parent::gml:node" />
		<let name="label" value="$node/gml:data[@key=$nodeLabelKey]/text()" />
		<let name="type" value="$node/gml:data[@key=$nodeTypeKey]/text()" />
		<report test="$node"
			role="information">[<value-of select="$node/@id"/>] <value-of select="$label"/> Attribute Validation Rules</report>
		<assert test="string-length(normalize-space(translate($label,'\/:*?&quot;&gt;&lt;|',''))) = string-length($label)" 
			role="critical">[<value-of select="$node/@id"/>] <value-of select="$label"/> Attribute label must not contain \/:*?&quot;&gt;&lt;| or space characters</assert>
		<let name="reservedList">
			<axsl:for-each select="xalan:nodeset($PredefinedIDLReservedWords)/node()[@keyName='label']//iso:value" xmlns:axsl="http://www.w3.org/1999/XSL/Transform">
				<axsl:value-of select="concat(. , ', ')" xmlns:axsl="http://www.w3.org/1999/XSL/Transform"/>
			</axsl:for-each>
		</let>
		<assert test="not( contains( $reservedList, concat(translate($label,'ABCDEFGHIJKLMNOPQRSTUVWXYZ','abcdefghijklmnopqrstuvwxyz'), ', ') ) )"
			role="critical">[<value-of select="$node/@id"/>] <value-of select="$label"/> Attribute label must not be an IDL reserved word </assert> 
		<let name="typeList">
			<axsl:for-each select="xalan:nodeset($PredefinedAttributeTypes)/node()[@nodeKind='Attribute'][@keyName='type']//iso:value" xmlns:axsl="http://www.w3.org/1999/XSL/Transform">
				<axsl:value-of select="concat(', ', .)"  xmlns:axsl="http://www.w3.org/1999/XSL/Transform"/>
			</axsl:for-each>
		</let>
		<assert test="xalan:nodeset($PredefinedAttributeTypes)/node()[@nodeKind='Attribute'][@keyName='type']//iso:value[text()=$type]"
			role="critical">[<value-of select="$node/@id"/>] <value-of select="$label"/> Attribute must have one of the following types<value-of select="$typeList"/></assert> 
<!--		<assert test="$type='Boolean' or $type='Byte' or $type='Char' or $type='WideChar' or $type='ShortInteger' or $type='LongInteger' or $type='LongLongInteger' 
					   or $type='UnsignedShortInteger' or $type='UnsignedLongInteger' or $type='UnsignedLongLongInteger' or $type='FloatNumber' or $type='DoubleNumber' 
					   or $type='LongDoubleNumber' or $type='String' or $type='WideString'" 
			role="critical">[<value-of select="$node/@id"/>] <value-of select="$label"/> Attribute must have a type of Boolean,Byte,Char,WideChar,ShortInteger,LongInteger,LongLongInteger,UnsignedShortInteger,UnsignedLongInteger,UnsignedLongLongInteger,FloatNumber,DoubleNumber,LongDoubleNumber,String or WideString</assert> 
-->
	</rule>
  </pattern>
  
  <pattern id="Aggregate.checks">
	<title>Checking Aggregate entities</title>
	<rule context="gml:node[$selectID='#ALL' or @id=$selectID]/gml:data[@key=$nodeKindKey][text()='Aggregate']">
		<let name="node" value="parent::gml:node" />
		<let name="label" value="$node/gml:data[@key=$nodeLabelKey]/text()" />
		<let name="type" value="$node/gml:data[@key=$nodeTypeKey]/text()" />
		<let name="childNode" value="$node/gml:graph/gml:node" />
		<report test="$node"
			role="information">[<value-of select="$node/@id"/>] <value-of select="$label"/> Aggregate Validation Rules</report>
		<assert test="string-length(normalize-space(translate($label,'\/:*?&quot;&gt;&lt;|',''))) = string-length($label)" 
			role="critical">[<value-of select="$node/@id"/>] <value-of select="$label"/> Aggregate label must not contain \/:*?&quot;&gt;&lt;| or space characters</assert>
		<let name="reservedList">
			<axsl:for-each select="xalan:nodeset($PredefinedIDLReservedWords)/node()[@keyName='label']//iso:value" xmlns:axsl="http://www.w3.org/1999/XSL/Transform">
				<axsl:value-of select="concat(. , ', ')" xmlns:axsl="http://www.w3.org/1999/XSL/Transform"/>
			</axsl:for-each>
		</let>
		<assert test="not( contains( $reservedList, concat(translate($label,'ABCDEFGHIJKLMNOPQRSTUVWXYZ','abcdefghijklmnopqrstuvwxyz'), ', ') ) )"
			role="critical">[<value-of select="$node/@id"/>] <value-of select="$label"/> Aggregate label must not be an IDL reserved word </assert> 
		<assert test="count($node/gml:graph/gml:node/gml:data[@key=$nodeKindKey][text()='Member']) &gt; 0
					   or count($node/gml:graph/gml:node/gml:data[@key=$nodeKindKey][text()='AggregateInstance']) &gt; 0
					   or count($node/gml:graph/gml:node/gml:data[@key=$nodeKindKey][text()='VectorInstance']) &gt; 0" 
			role="warning">[<value-of select="$node/@id"/>] <value-of select="$label"/> Aggregate should contain Member, AggregateInstance and VectorInstance entities</assert>
		<assert test="count($node/gml:graph/gml:node/gml:data[@key=$nodeKindKey][text()='Member']/../gml:data[@key=$nodeKeyMemberKey][text()='true'] ) &gt; 0" 
			role="warning">[<value-of select="$node/@id"/>] <value-of select="$label"/> Aggregate should contain at least one Member that has a key of true</assert>
		<assert test="count($childNode/gml:data[@key=$nodeKindKey][text()!='Member' and text()!='AggregateInstance' and text()!='VectorInstance']) = 0" 
			role="critical">[<value-of select="$node/@id"/>] <value-of select="$label"/> Aggregate must contain only Member, AggregateInstance, and VectorInstance kind of entities</assert> 
	</rule>
  </pattern>
 
  <pattern id="Aggregate.AggregateInstance.checks">
	<title>Checking AggregateInstance entities contained in IDLs</title>
	<!-- Rules for AggregateInstances that are part of an IDL file definition, ie parent is Aggregate -->
	<rule context="gml:node[$selectID='#ALL' or @id=$selectID][parent::gml:graph/parent::gml:node/gml:data[@key=$nodeKindKey][text()='Aggregate']]/gml:data[@key=$nodeKindKey][text()='AggregateInstance']">
		<let name="node" value="parent::gml:node" />
		<let name="label" value="$node/gml:data[@key=$nodeLabelKey]/text()" />
		<let name="parentNode" value="$node/parent::gml:graph/parent::gml:node" />
		<!-- Assertions on AggregateInstances that are part of an IDL file definition, ie parent is Aggregate -->
		<report test="count($parentNode/gml:data[@key=$nodeKindKey][text()='Aggregate']) &gt; 0"
			role="information">[<value-of select="$node/@id"/>] <value-of select="$label"/> AggregateInstance Validation Rules</report>
		<assert test="string-length(normalize-space(translate($label,'\/:*?&quot;&gt;&lt;|',''))) = string-length($label)" 
			role="critical">[<value-of select="$node/@id"/>] <value-of select="$label"/> AggregateInstance label must not contain \/:*?&quot;&gt;&lt;| or space characters</assert>
		<let name="reservedList">
			<axsl:for-each select="xalan:nodeset($PredefinedIDLReservedWords)/node()[@keyName='label']//iso:value" xmlns:axsl="http://www.w3.org/1999/XSL/Transform">
				<axsl:value-of select="concat(. , ', ')" xmlns:axsl="http://www.w3.org/1999/XSL/Transform"/>
			</axsl:for-each>
		</let>
		<assert test="not( contains( $reservedList, concat(translate($label,'ABCDEFGHIJKLMNOPQRSTUVWXYZ','abcdefghijklmnopqrstuvwxyz'), ', ') ) )"
			role="critical">[<value-of select="$node/@id"/>] <value-of select="$label"/> AggregateInstance label must not be an IDL reserved word </assert> 
		<assert test="count(//gml:edge[@source=$node/@id]) = 1" 
			role="critical">[<value-of select="$node/@id"/>] <value-of select="$label"/> AggregateInstance must be the source connection for one link to an Aggregate entity</assert> 
		<let name="aggregateID" value="//gml:edge[@source=$node/@id]/@target" />
	    <assert test="count(//gml:node[@id=$aggregateID]/gml:data[@key=$nodeKindKey][text()='Aggregate']) = 1" 
			role="critical">[<value-of select="$node/@id"/>] <value-of select="$label"/> AggregateInstance must be connected to an Aggregate entity</assert> 
	</rule>
  </pattern>


  <pattern id="Member.checks">
	<title>Checking Member entities</title>
	<rule context="gml:node[$selectID='#ALL' or @id=$selectID]/gml:data[@key=$nodeKindKey][text()='Member']">
		<let name="node" value="parent::gml:node" />
		<let name="label" value="$node/gml:data[@key=$nodeLabelKey]/text()" />
		<let name="key" value="$node/gml:data[@key=$nodeKeyMemberKey]/text()" />
		<let name="type" value="$node/gml:data[@key=$nodeTypeKey]/text()" />
		<let name="typeList">
			<axsl:for-each select="xalan:nodeset($PredefinedMemberTypes)/node()[@nodeKind='Member'][@keyName='type']//iso:value" xmlns:axsl="http://www.w3.org/1999/XSL/Transform">
				<axsl:value-of select="concat(', ', .)"  xmlns:axsl="http://www.w3.org/1999/XSL/Transform"/>
			</axsl:for-each>
		</let>

		<report test="$node"
			role="information">[<value-of select="$node/@id"/>] <value-of select="$label"/> Member Validation Rules</report>
		<assert test="string-length(normalize-space(translate($label,'\/:*?&quot;&gt;&lt;|',''))) = string-length($label)" 
			role="critical">[<value-of select="$node/@id"/>] <value-of select="$label"/> Member label must not contain \/:*?&quot;&gt;&lt;| or space characters</assert>
		<let name="reservedList">
			<axsl:for-each select="xalan:nodeset($PredefinedIDLReservedWords)/node()[@keyName='label']//iso:value" xmlns:axsl="http://www.w3.org/1999/XSL/Transform">
				<axsl:value-of select="concat(. , ', ')" xmlns:axsl="http://www.w3.org/1999/XSL/Transform"/>
			</axsl:for-each>
		</let>
		<assert test="not( contains( $reservedList, concat(translate($label,'ABCDEFGHIJKLMNOPQRSTUVWXYZ','abcdefghijklmnopqrstuvwxyz'), ', ') ) )"
			role="critical">[<value-of select="$node/@id"/>] <value-of select="$label"/> Member label must not be an IDL reserved word </assert> 
		<assert test="$key='true' or $key='false'" 
			role="critical">[<value-of select="$node/@id"/>] <value-of select="$label"/> Member must have a key of true or false</assert> 
		<assert test="xalan:nodeset($PredefinedMemberTypes)/node()[@nodeKind='Member'][@keyName='type']//iso:value[text()=$type]"
			role="critical">[<value-of select="$node/@id"/>] <value-of select="$label"/> Member must have one of the following types<value-of select="$typeList"/></assert> 
	</rule>
  </pattern>  

  <pattern id="ComponentAssembly.checks">
	<title>Checking ComponentAssembly entities</title>
	<rule context="gml:node[$selectID='#ALL' or @id=$selectID]/gml:data[@key=$nodeKindKey][text()='ComponentAssembly']">
		<let name="node" value="parent::gml:node" />
		<let name="label" value="$node/gml:data[@key=$nodeLabelKey]/text()" />
		<let name="childNode" value="$node/gml:graph/gml:node" />
		<report test="$node"
			role="information">[<value-of select="$node/@id"/>] <value-of select="$label"/> ComponentAssembly Validation Rules</report>
		<assert test="string-length(normalize-space(translate($label,'\/:*?&quot;&gt;&lt;|',''))) = string-length($label)" 
			role="critical">[<value-of select="$node/@id"/>] <value-of select="$label"/> ComponentAssembly label must not contain \/:*?&quot;&gt;&lt;| or space characters</assert>
		<assert test="count($node/parent::gml:graph/gml:node[gml:data[@key=$nodeLabelKey]/text() = $label]) = 1"
			role="critical">[<value-of select="$node/@id"/>] <value-of select="$label"/> ComponentAssembly label must be unique within parent graph</assert>
	    <assert test="count($childNode/gml:data[@key=$nodeKindKey][text()='ComponentAssembly']) &gt; 0
					  or count($childNode/gml:data[@key=$nodeKindKey][text()='ComponentInstance']) &gt; 0
					  or count($childNode/gml:data[@key=$nodeKindKey][text()='BlackBoxInstance']) &gt; 0
					  or count($childNode/gml:data[@key=$nodeKindKey][text()='InEventPortDelegate']) &gt; 0
					  or count($childNode/gml:data[@key=$nodeKindKey][text()='OutEventPortDelegate']) &gt; 0" 
			role="warning">[<value-of select="$node/@id"/>] <value-of select="$label"/> ComponentAssembly should contain ComponentAssembly, ComponentInstance, BlackBoxInstance, InEventPortDelegate and OutEventPortDelegate kind of entities</assert> 
		<assert test="count($childNode/gml:data[@key=$nodeKindKey][text()!='ComponentAssembly' and text()!='ComponentInstance' and text()!='BlackBoxInstance' and text()!='InEventPortDelegate' and text()!='OutEventPortDelegate']) = 0" 
			role="critical">[<value-of select="$node/@id"/>] <value-of select="$label"/> ComponentAssembly must contain only ComponentAssembly, ComponentInstance, BlackBoxInstance, InEventPortDelegate and OutEventPortDelegate kind of entities</assert> 
	</rule>
  </pattern>  
  
  <pattern id="ComponentInstance.checks">
	<title>Checking ComponentInstance entities</title>
	<rule context="gml:node[$selectID='#ALL' or @id=$selectID]/gml:data[@key=$nodeKindKey][text()='ComponentInstance']">
		<let name="node" value="parent::gml:node" />
		<let name="label" value="$node/gml:data[@key=$nodeLabelKey]/text()" />
		<let name="childNode" value="$node/gml:graph/gml:node" />
		<report test="$node"
			role="information">[<value-of select="$node/@id"/>] <value-of select="$label"/> ComponentInstance Validation Rules</report>
		<assert test="string-length(normalize-space(translate($label,'\/:*?&quot;&gt;&lt;|',''))) = string-length($label)" 
			role="critical">[<value-of select="$node/@id"/>] <value-of select="$label"/> ComponentInstance label must not contain \/:*?&quot;&gt;&lt;| or space characters</assert>
		<assert test="count($node/parent::gml:graph/gml:node[gml:data[@key=$nodeLabelKey]/text() = $label]) = 1"
			role="critical">[<value-of select="$node/@id"/>] <value-of select="$label"/> ComponentInstance label must be unique within parent ComponentAssembly</assert>
	    <assert test="count($childNode/gml:data[@key=$nodeKindKey][text()='InEventPortInstance']) &gt; 0
					  or count($childNode/gml:data[@key=$nodeKindKey][text()='OutEventPortInstance']) &gt; 0
					  or count($childNode/gml:data[@key=$nodeKindKey][text()='AttributeInstance']) &gt; 0" 
			role="warning">[<value-of select="$node/@id"/>] <value-of select="$label"/> ComponentInstance should contain InEventPortInstance, OutEventPortInstance and AttributeInstance kind of entities</assert> 
		<assert test="count($childNode/gml:data[@key=$nodeKindKey][text()!='InEventPortInstance' and text()!='OutEventPortInstance' and text()!='AttributeInstance']) = 0" 
			role="critical">[<value-of select="$node/@id"/>] <value-of select="$label"/> ComponentInstance must contain only InEventPortInstance, OutEventPortInstance and AttributeInstance kind of entities</assert> 
	</rule>
  </pattern>
 
  <pattern id="BlackBoxInstance.checks">
	<title>Checking BlackBoxInstance entities</title>
	<rule context="gml:node[$selectID='#ALL' or @id=$selectID]/gml:data[@key=$nodeKindKey][text()='BlackBoxInstance']">
		<let name="node" value="parent::gml:node" />
		<let name="label" value="$node/gml:data[@key=$nodeLabelKey]/text()" />
		<let name="childNode" value="$node/gml:graph/gml:node" />
		<report test="$node"
			role="information">[<value-of select="$node/@id"/>] <value-of select="$label"/> BlackBoxInstance Validation Rules</report>
		<assert test="string-length(normalize-space(translate($label,'\/:*?&quot;&gt;&lt;|',''))) = string-length($label)" 
			role="critical">[<value-of select="$node/@id"/>] <value-of select="$label"/> BlackBoxInstance label must not contain \/:*?&quot;&gt;&lt;| or space characters</assert>
		<assert test="count($node/parent::gml:graph/gml:node[gml:data[@key=$nodeLabelKey]/text() = $label]) = 1"
			role="critical">[<value-of select="$node/@id"/>] <value-of select="$label"/> BlackBoxInstance label must be unique within parent ComponentAssembly</assert>
	    <assert test="count($childNode/gml:data[@key=$nodeKindKey][text()='InEventPortInstance']) &gt; 0
					  or count($childNode/gml:data[@key=$nodeKindKey][text()='OutEventPortInstance']) &gt; 0" 
			role="warning">[<value-of select="$node/@id"/>] <value-of select="$label"/> BlackBoxInstance should contain InEventPortInstance and OutEventPortInstance kind of entities</assert> 
		<assert test="count($childNode/gml:data[@key=$nodeKindKey][text()!='InEventPortInstance' and text()!='OutEventPortInstance']) = 0" 
			role="critical">[<value-of select="$node/@id"/>] <value-of select="$label"/> BlackBoxInstance must contain only InEventPortInstance and OutEventPortInstance kind of entities</assert> 
	</rule>
  </pattern>

  <pattern id="OutEventPortInstance.checks">
	<title>Checking OutEventPortInstance entities</title>
	<rule context="gml:node[$selectID='#ALL' or @id=$selectID]/gml:data[@key=$nodeKindKey][text()='OutEventPortInstance']">
		<let name="node" value="parent::gml:node" />
		<let name="label" value="$node/gml:data[@key=$nodeLabelKey]/text()" />
		<report test="$node"
			role="information">[<value-of select="$node/@id"/>] <value-of select="$label"/> OutEventPortInstance Validation Rules</report>
		<assert test="string-length(normalize-space(translate($label,'\/:*?&quot;&gt;&lt;|',''))) = string-length($label)" 
			role="critical">[<value-of select="$node/@id"/>] <value-of select="$label"/> OutEventPortInstance label must not contain \/:*?&quot;&gt;&lt;| or space characters</assert>
			
		<assert test="count($node/parent::gml:graph/gml:node[gml:data[@key=$nodeLabelKey]/text() = $label]) = 1"
			role="critical">[<value-of select="$node/@id"/>] <value-of select="$label"/> OutEventPortInstance label must be unique within parent ComponentInstance</assert>
		<let name="sourceEventPortID" value="//gml:edge[@target=$node/@id]/@source" />
		
	    <assert test="not($sourceEventPortID) or count(//gml:node[@id=$sourceEventPortID]/gml:data[@key=$nodeKindKey][text() = 'InEventPortInstance' or text() = 'InEventPortDelegate' or text() = 'OutEventPortDelegate']) = 0" 
			role="critical">[<value-of select="$node/@id"/>] <value-of select="$label"/> OutEventPortInstance cannot have a target connection from InEventPortInstance or EventPortDelegate entities</assert> 
			
		<let name="targetEventPortID" value="//gml:edge[@source=$node/@id]/@target" />
		<let name="targetEventPort" value="//gml:node[@id=$targetEventPortID]/gml:data[@key=$nodeKindKey][text() = 'InEventPortInstance' or text() = 'InEventPortDelegate' or text() = 'OutEventPortDelegate']/.." />
		
	    <assert test="$targetEventPortID and count(//gml:node[@id=$targetEventPortID]/gml:data[@key=$nodeKindKey][text() = 'InEventPortInstance' or text() = 'InEventPortDelegate' or text() = 'OutEventPortDelegate']) &gt; 0" 
			role="warning">[<value-of select="$node/@id"/>] <value-of select="$label"/> OutEventPortInstance should have a source connection to a InEventPortInstance or an EventPortDelegate entity</assert> 
			
	    <assert test="$targetEventPort and (count($targetEventPort/gml:data[@key=$nodeTypeKey][text() != $node/gml:data[@key=$nodeTypeKey]/text()]) = 0 )" 
			role="warning">[<value-of select="$node/@id"/>] <value-of select="$label"/> OutEventPortInstance should have the same type as the connected target entity</assert> 
	</rule> 
  </pattern> 

  <pattern id="InEventPortInstance.checks">
	<title>Checking InEventPortInstance entities</title>
	<rule context="gml:node[$selectID='#ALL' or @id=$selectID]/gml:data[@key=$nodeKindKey][text()='InEventPortInstance']">
		<let name="node" value="parent::gml:node" />
		<let name="label" value="$node/gml:data[@key=$nodeLabelKey]/text()" />
		<report test="$node"
			role="information">[<value-of select="$node/@id"/>] <value-of select="$label"/> InEventPortInstance Validation Rules</report>
		<assert test="string-length(normalize-space(translate($label,'\/:*?&quot;&gt;&lt;|',''))) = string-length($label)" 
			role="critical">[<value-of select="$node/@id"/>] <value-of select="$label"/> InEventPortInstance label must not contain \/:*?&quot;&gt;&lt;| or space characters</assert>
		<assert test="count($node/parent::gml:graph/gml:node[gml:data[@key=$nodeLabelKey]/text() = $label]) = 1"
			role="critical">[<value-of select="$node/@id"/>] <value-of select="$label"/> InEventPortInstance label must be unique within parent ComponentInstance</assert>
			
		<let name="sourceEventPortID" value="//gml:edge[@target=$node/@id]/@source" />
	    <let name="sourceEventPort" value="//gml:node[@id=$sourceEventPortID]/gml:data[@key=$nodeKindKey][text() = 'OutEventPortInstance' or text() = 'InEventPortDelegate' or text() = 'OutEventPortDelegate']/.." />
		
		<assert test="$sourceEventPortID and count(//gml:node[@id=$sourceEventPortID]/gml:data[@key=$nodeKindKey][text() = 'OutEventPortInstance' or text() = 'InEventPortDelegate' or text() = 'OutEventPortDelegate']) &gt; 0" 
			role="warning">[<value-of select="$node/@id"/>] <value-of select="$label"/> InEventPortInstance should have a target connection from an OutEventPortInstance or EventPortDelegate entity</assert> 
		
		<assert test="$sourceEventPort and (count($sourceEventPort/gml:data[@key=$nodeTypeKey][text() != $node/gml:data[@key=$nodeTypeKey]/text()]) = 0 )" 
			role="warning">[<value-of select="$node/@id"/>] <value-of select="$label"/> InEventPortInstance should have the same type as the connected source entity</assert> 	
		
		<let name="targetEventPortID" value="//gml:edge[@source=$node/@id]/@target" />
		
	    <assert test="not($targetEventPortID) or count(//gml:node[@id=$targetEventPortID]/gml:data[@key=$nodeKindKey][text() = 'OutEventPortInstance' or text() = 'InEventPortDelegate' or text() = 'OutEventPortDelegate']) = 0" 
			role="critical">[<value-of select="$node/@id"/>] <value-of select="$label"/> InEventPortInstance cannot have a source connection to a OutEventPortInstance or an EventPortDelegate entity</assert> 
	</rule>
  </pattern>  

  <pattern id="InEventPortDelegate.checks">
	<title>Checking InEventPortDelegate entities</title>
	<rule context="gml:node[$selectID='#ALL' or @id=$selectID]/gml:data[@key=$nodeKindKey][text()='InEventPortDelegate']">
		<let name="node" value="parent::gml:node" />
		<let name="label" value="$node/gml:data[@key=$nodeLabelKey]/text()" />
		<report test="$node"
			role="information">[<value-of select="$node/@id"/>] <value-of select="$label"/> InEventPortDelegate Validation Rules</report>
		<assert test="string-length(normalize-space(translate($label,'\/:*?&quot;&gt;&lt;|',''))) = string-length($label)" 
			role="critical">[<value-of select="$node/@id"/>] <value-of select="$label"/> InEventPortDelegate label must not contain \/:*?&quot;&gt;&lt;| or space characters</assert>
		<assert test="count($node/parent::gml:graph/gml:node[gml:data[@key=$nodeLabelKey]/text() = $label]) = 1"
			role="critical">[<value-of select="$node/@id"/>] <value-of select="$label"/> InEventPortDelegate label must be unique within parent ComponentInstance</assert>
			
		<let name="sourceEventPortID" value="//gml:edge[@target=$node/@id]/@source" />
	    <let name="sourceEventPort" value="//gml:node[@id=$sourceEventPortID]/gml:data[@key=$nodeKindKey][text() = 'OutEventPortInstance' or text() = 'InEventPortDelegate' or text() = 'OutEventPortDelegate']/.." />
		
		<assert test="$sourceEventPortID and count(//gml:node[@id=$sourceEventPortID]/gml:data[@key=$nodeKindKey][text() = 'OutEventPortInstance' or text() = 'InEventPortDelegate' or text() = 'OutEventPortDelegate']) &gt; 0" 
			role="warning">[<value-of select="$node/@id"/>] <value-of select="$label"/> InEventPortDelegate should have a target connection from an OutEventPortInstance or EventPortDelegate entity</assert> 
		
		<assert test="$sourceEventPort and (count($sourceEventPort/gml:data[@key=$nodeTypeKey][text() != $node/gml:data[@key=$nodeTypeKey]/text()]) = 0 )" 
			role="warning">[<value-of select="$node/@id"/>] <value-of select="$label"/> InEventPortDelegate should have the same type as the connected source entity</assert> 	
		
		<let name="targetEventPortID" value="//gml:edge[@source=$node/@id]/@target" />
		<let name="targetEventPort" value="//gml:node[@id=$targetEventPortID]/gml:data[@key=$nodeKindKey][text() = 'InEventPortInstance' or text() = 'InEventPortDelegate']/.." />
		
	    <assert test="not($targetEventPortID) or count(//gml:node[@id=$targetEventPortID]/gml:data[@key=$nodeKindKey][text() = 'OutEventPortInstance' or text() = 'OutEventPortDelegate']) = 0" 
			role="critical">[<value-of select="$node/@id"/>] <value-of select="$label"/> InEventPortDelegate cannot have a source connection to a OutEventPortInstance or an OutEventPortDelegate entity</assert> 
			
		<assert test="$targetEventPort and (count($targetEventPort/gml:data[@key=$nodeTypeKey][text() != $node/gml:data[@key=$nodeTypeKey]/text()]) = 0 )" 
			role="warning">[<value-of select="$node/@id"/>] <value-of select="$label"/> InEventPortDelegate should have the same type as the connected target entity</assert> 
	</rule>
  </pattern>  

  <pattern id="OutEventPortDelegate.checks">
	<title>Checking OutEventPortDelegate entities</title>
	<rule context="gml:node[$selectID='#ALL' or @id=$selectID]/gml:data[@key=$nodeKindKey][text()='OutEventPortDelegate']">
		<let name="node" value="parent::gml:node" />
		<let name="label" value="$node/gml:data[@key=$nodeLabelKey]/text()" />
		<report test="$node"
			role="information">[<value-of select="$node/@id"/>] <value-of select="$label"/> OutEventPortDelegate Validation Rules</report>
		<assert test="string-length(normalize-space(translate($label,'\/:*?&quot;&gt;&lt;|',''))) = string-length($label)" 
			role="critical">[<value-of select="$node/@id"/>] <value-of select="$label"/> OutEventPortDelegate label must not contain \/:*?&quot;&gt;&lt;| or space characters</assert>
			
		<assert test="count($node/parent::gml:graph/gml:node[gml:data[@key=$nodeLabelKey]/text() = $label]) = 1"
			role="critical">[<value-of select="$node/@id"/>] <value-of select="$label"/> OutEventPortDelegate label must be unique within parent ComponentInstance</assert>
		<let name="sourceEventPortID" value="//gml:edge[@target=$node/@id]/@source" />
		<let name="sourceEventPort" value="//gml:node[@id=$sourceEventPortID]/gml:data[@key=$nodeKindKey][text() = 'OutEventPortInstance' or text() = 'OutEventPortDelegate']/.." />
		
	    <assert test="not($sourceEventPortID) or count(//gml:node[@id=$sourceEventPortID]/gml:data[@key=$nodeKindKey][text() = 'InEventPortInstance' or text() = 'InEventPortDelegate']) = 0" 
			role="critical">[<value-of select="$node/@id"/>] <value-of select="$label"/> OutEventPortDelegate cannot have a target connection from InEventPortInstance or InEventPortDelegate entities</assert> 

		<assert test="$sourceEventPort and (count($sourceEventPort/gml:data[@key=$nodeTypeKey][text() != $node/gml:data[@key=$nodeTypeKey]/text()]) = 0 )" 
			role="warning">[<value-of select="$node/@id"/>] <value-of select="$label"/> OutEventPortDelegate should have the same type as the connected source entity</assert> 	
			
		<let name="targetEventPortID" value="//gml:edge[@source=$node/@id]/@target" />
		<let name="targetEventPort" value="//gml:node[@id=$targetEventPortID]/gml:data[@key=$nodeKindKey][text() = 'InEventPortInstance' or text() = 'InEventPortDelegate' or text() = 'OutEventPortDelegate']/.." />
		
	    <assert test="$targetEventPortID and count(//gml:node[@id=$targetEventPortID]/gml:data[@key=$nodeKindKey][text() = 'InEventPortInstance' or text() = 'InEventPortDelegate' or text() = 'OutEventPortDelegate']) &gt; 0" 
			role="warning">[<value-of select="$node/@id"/>] <value-of select="$label"/> OutEventPortDelegate should have a source connection to a InEventPortInstance or an EventPortDelegate entity</assert> 
			
	    <assert test="$targetEventPort and (count($targetEventPort/gml:data[@key=$nodeTypeKey][text() != $node/gml:data[@key=$nodeTypeKey]/text()]) = 0 )" 
			role="warning">[<value-of select="$node/@id"/>] <value-of select="$label"/> OutEventPortDelegate should have the same type as the connected target entity</assert> 
	</rule> 
  </pattern> 

  <pattern id="AttributeInstance.checks">
	<title>Checking AttributeInstance entities</title>
	<rule context="gml:node[$selectID='#ALL' or @id=$selectID]/gml:data[@key=$nodeKindKey][text()='AttributeInstance']">
		<let name="node" value="parent::gml:node" />
		<let name="label" value="$node/gml:data[@key=$nodeLabelKey]/text()" />
		<let name="type" value="$node/gml:data[@key=$nodeTypeKey]/text()" />
		<let name="value" value="$node/gml:data[@key=$nodeValueKey]/text()" />
		<report test="$node"
			role="information">[<value-of select="$node/@id"/>] <value-of select="$label"/> AttributeInstance Validation Rules</report>
		<assert test="string-length(normalize-space(translate($label,'\/:*?&quot;&gt;&lt;|',''))) = string-length($label)" 
			role="critical">[<value-of select="$node/@id"/>] <value-of select="$label"/> AttributeInstance label must not contain \/:*?&quot;&gt;&lt;| or space characters</assert>
		<let name="typeList">
			<axsl:for-each select="xalan:nodeset($PredefinedAttributeInstanceTypes)/node()[@nodeKind='AttributeInstance'][@keyName='type']//iso:value" xmlns:axsl="http://www.w3.org/1999/XSL/Transform">
				<axsl:value-of select="concat(', ', .)"  xmlns:axsl="http://www.w3.org/1999/XSL/Transform"/>
			</axsl:for-each>
		</let>
		<assert test="xalan:nodeset($PredefinedAttributeInstanceTypes)/node()[@nodeKind='AttributeInstance'][@keyName='type']//iso:value[text()=$type]"
			role="critical">[<value-of select="$node/@id"/>] <value-of select="$label"/> AttributeInstance must have one of the following types<value-of select="$typeList"/></assert> 
		<assert test="count($node/parent::gml:graph/gml:node[gml:data[@key=$nodeLabelKey]/text() = $label]) = 1"
			role="critical">[<value-of select="$node/@id"/>] <value-of select="$label"/> AttributeInstance label must be unique within parent ComponentInstance</assert>
		<assert test="$value and $value != ''"
			role="warning">[<value-of select="$node/@id"/>] <value-of select="$label"/> AttributeInstance should have an initial value set</assert>
		<assert test="not($type = 'Boolean') or ($type = 'Boolean' and ($value = 'true' or $value = 'false'))"
			role="critical">[<value-of select="$node/@id"/>] <value-of select="$label"/> AttributeInstance of type Boolean must have value of true or false</assert>
		<assert test="not($type = 'FloatNumber' or $type = 'DoubleNumber' or $type = 'LongDoubleNumber')
				     or (($type = 'FloatNumber' or $type = 'DoubleNumber' or $type = 'LongDoubleNumber') and (string(number($value)) != 'NaN'))"
			role="critical">[<value-of select="$node/@id"/>] <value-of select="$label"/> AttributeInstance of type FloatNumber, or DoubleNumber must have number value</assert>
		<assert test="not($type = 'UnsignedShortInteger' or $type = 'UnsignedLongInteger' or $type = 'UnsignedLongLongInteger')
					 or (($type = 'UnsignedShortInteger' or $type = 'UnsignedLongInteger' or $type = 'UnsignedLongLongInteger') and string(number($value)) != 'NaN' and not(contains($value,'.'))  and not(contains($value,'-')))"
			role="critical">[<value-of select="$node/@id"/>] <value-of select="$label"/> AttributeInstance of type Unsigned Integer must have number value without decimal point and without minus sign</assert>
		<assert test="not($type = 'ShortInteger' or $type = 'LongInteger' or $type = 'LongLongInteger')
					 or (($type = 'ShortInteger' or $type = 'LongInteger' or $type = 'LongLongInteger') and string(number($value)) != 'NaN' and not(contains($value,'.')) )"
			role="critical">[<value-of select="$node/@id"/>] <value-of select="$label"/> AttributeInstance of type Integer must have number value without decimal point</assert>
		<assert test="not($type = 'Byte' or $type = 'Char' or $type = 'WideChar')
					 or (($type = 'Byte' or $type = 'Char' or $type = 'WideChar') and string-length($value) = 1)"
			role="critical">[<value-of select="$node/@id"/>] <value-of select="$label"/> AttributeInstance of type Byte or Char must have only one character in value</assert>
	</rule>
  </pattern>  
  
</schema>
